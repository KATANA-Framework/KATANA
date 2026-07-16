#pragma once

#include "katana/core/arena.hpp"
#include "katana/core/fd_watch.hpp"
#include "katana/core/http.hpp"
#include "katana/core/log.hpp"
#include "katana/core/reactor_pool.hpp"
#include "katana/core/router.hpp"
#include "katana/core/auth.hpp"
#include "katana/core/compression.hpp"
#include "katana/core/shutdown.hpp"
#include "katana/core/tcp_listener.hpp"
#include "katana/core/tcp_socket.hpp"
#include "katana/core/tls.hpp"

#include <array>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <type_traits>
#include <vector>

namespace katana {
namespace http {

namespace detail {
constexpr size_t HTTP_SERVER_RESPONSE_BUFFER_CAPACITY = 8192;
constexpr size_t HTTP_SERVER_ARENA_CAPACITY = 8192;

// Generate a process-unique correlation id (16 lowercase hex chars) into `out`.
// Used when a request arrives without an `X-Request-Id` header. Uniqueness comes from a
// per-thread counter seeded with a process-wide thread ordinal in the top byte, so the hot
// path touches no shared cache line and performs no allocation.
inline void generate_request_id(char (&out)[16]) {
    static std::atomic<uint64_t> thread_ordinal{0};
    thread_local uint64_t counter =
        thread_ordinal.fetch_add(1, std::memory_order_relaxed) << 56;
    const uint64_t n = counter++;
    static constexpr char hex[] = "0123456789abcdef";
    for (int i = 15; i >= 0; --i) {
        out[i] = hex[(n >> ((15 - i) * 4)) & 0xF];
    }
}
} // namespace detail

// Lightweight server-level RED metrics, exported in Prometheus text format via /metrics.
// Counters are by response status class; in_flight is a gauge of requests being processed.
//
// Per-request counters are sharded by thread: every worker writes its own cache-line-aligned
// shard (plain relaxed increments, no cross-core cache-line ping-pong at millions of rps) and
// readers sum the shards on scrape. Only the low-frequency connection-level gauges stay as
// single shared atomics.
struct server_metrics {
    // Request-duration histogram (Prometheus). Bucket upper bounds in microseconds, with the
    // matching `le` labels in seconds. Observe increments exactly one (non-cumulative) bucket;
    // the cumulative `le` counts required by Prometheus are produced by prefix-summing on read.
    static constexpr std::array<int64_t, 14> duration_bounds_micros{
        500,    1000,    2500,    5000,    10000,   25000,    50000,
        100000, 250000,  500000,  1000000, 2500000, 5000000,  10000000};
    static constexpr std::array<std::string_view, 14> duration_bound_labels{
        "0.0005", "0.001", "0.0025", "0.005", "0.01", "0.025", "0.05",
        "0.1",    "0.25",  "0.5",    "1",     "2.5",  "5",      "10"};

    static constexpr size_t SHARD_COUNT = 16; // power of two; covers typical worker counts

    struct alignas(64) shard {
        std::atomic<uint64_t> requests_2xx{0};
        std::atomic<uint64_t> requests_3xx{0};
        std::atomic<uint64_t> requests_4xx{0};
        std::atomic<uint64_t> requests_5xx{0};
        std::atomic<int64_t> in_flight{0};
        std::array<std::atomic<uint64_t>, 14> duration_buckets{}; // per-bucket, not cumulative
        std::atomic<uint64_t> duration_count{0};
        std::atomic<uint64_t> duration_sum_micros{0};
    };
    std::array<shard, SHARD_COUNT> shards{};

    // Connection-level gauges/counters (touched per connection, not per request).
    std::atomic<uint64_t> connection_timeouts{0};
    std::atomic<int64_t> active_connections{0};
    std::atomic<uint64_t> connections_rejected{0};

    [[nodiscard]] static size_t shard_index() noexcept {
        static std::atomic<uint32_t> next_slot{0};
        thread_local const uint32_t slot =
            next_slot.fetch_add(1, std::memory_order_relaxed) & (SHARD_COUNT - 1);
        return slot;
    }
    [[nodiscard]] shard& local_shard() noexcept { return shards[shard_index()]; }

    void begin_request() noexcept {
        local_shard().in_flight.fetch_add(1, std::memory_order_relaxed);
    }

    void end_request() noexcept {
        local_shard().in_flight.fetch_sub(1, std::memory_order_relaxed);
    }

    // Current in-flight gauge (sums shards; used by opt-in load shedding, not the hot path).
    [[nodiscard]] int64_t in_flight_now() const noexcept {
        int64_t total = 0;
        for (const auto& sh : shards) {
            total += sh.in_flight.load(std::memory_order_relaxed);
        }
        return total;
    }

    void observe_duration_micros(int64_t micros) noexcept {
        if (micros < 0) {
            micros = 0;
        }
        auto& sh = local_shard();
        size_t i = 0;
        while (i < duration_bounds_micros.size() && micros > duration_bounds_micros[i]) {
            ++i;
        }
        if (i < sh.duration_buckets.size()) {
            sh.duration_buckets[i].fetch_add(1, std::memory_order_relaxed);
        }
        sh.duration_count.fetch_add(1, std::memory_order_relaxed);
        sh.duration_sum_micros.fetch_add(static_cast<uint64_t>(micros), std::memory_order_relaxed);
    }

    void record_status(int32_t status) noexcept {
        auto& sh = local_shard();
        if (status >= 500) {
            sh.requests_5xx.fetch_add(1, std::memory_order_relaxed);
        } else if (status >= 400) {
            sh.requests_4xx.fetch_add(1, std::memory_order_relaxed);
        } else if (status >= 300) {
            sh.requests_3xx.fetch_add(1, std::memory_order_relaxed);
        } else if (status >= 200) {
            sh.requests_2xx.fetch_add(1, std::memory_order_relaxed);
        }
    }

    // Point-in-time aggregate across shards (relaxed reads; scrape-grade consistency).
    struct totals {
        uint64_t requests_2xx = 0;
        uint64_t requests_3xx = 0;
        uint64_t requests_4xx = 0;
        uint64_t requests_5xx = 0;
        int64_t in_flight = 0;
        std::array<uint64_t, 14> cumulative_buckets{};
        uint64_t duration_count = 0;
        uint64_t duration_sum_micros = 0;
    };

    [[nodiscard]] totals aggregate() const noexcept {
        totals t;
        for (const auto& sh : shards) {
            t.requests_2xx += sh.requests_2xx.load(std::memory_order_relaxed);
            t.requests_3xx += sh.requests_3xx.load(std::memory_order_relaxed);
            t.requests_4xx += sh.requests_4xx.load(std::memory_order_relaxed);
            t.requests_5xx += sh.requests_5xx.load(std::memory_order_relaxed);
            t.in_flight += sh.in_flight.load(std::memory_order_relaxed);
            for (size_t i = 0; i < sh.duration_buckets.size(); ++i) {
                t.cumulative_buckets[i] += sh.duration_buckets[i].load(std::memory_order_relaxed);
            }
            t.duration_count += sh.duration_count.load(std::memory_order_relaxed);
            t.duration_sum_micros += sh.duration_sum_micros.load(std::memory_order_relaxed);
        }
        for (size_t i = 1; i < t.cumulative_buckets.size(); ++i) {
            t.cumulative_buckets[i] += t.cumulative_buckets[i - 1];
        }
        return t;
    }

    // Render the sub-second part of a microsecond count as a zero-padded 6-digit fraction.
    [[nodiscard]] static std::string format_micros_fraction(uint64_t micros_remainder) {
        std::string digits = std::to_string(micros_remainder);
        return std::string(6 - digits.size(), '0') + digits;
    }

    [[nodiscard]] std::string to_prometheus() const {
        auto line = [](std::string& s, std::string_view metric, std::string_view labels,
                       uint64_t v) {
            s += metric;
            s += labels;
            s += ' ';
            s += std::to_string(v);
            s += '\n';
        };
        const totals t = aggregate();
        std::string out;
        out += "# HELP katana_http_requests_total Total HTTP responses by status class.\n";
        out += "# TYPE katana_http_requests_total counter\n";
        line(out, "katana_http_requests_total", "{status=\"2xx\"}", t.requests_2xx);
        line(out, "katana_http_requests_total", "{status=\"3xx\"}", t.requests_3xx);
        line(out, "katana_http_requests_total", "{status=\"4xx\"}", t.requests_4xx);
        line(out, "katana_http_requests_total", "{status=\"5xx\"}", t.requests_5xx);
        out += "# HELP katana_http_requests_in_flight HTTP requests currently being served.\n";
        out += "# TYPE katana_http_requests_in_flight gauge\n";
        const int64_t live = t.in_flight;
        line(out, "katana_http_requests_in_flight", "", static_cast<uint64_t>(live < 0 ? 0 : live));
        out += "# HELP katana_http_connection_timeouts_total Connections closed by an idle/read "
               "timeout.\n";
        out += "# TYPE katana_http_connection_timeouts_total counter\n";
        line(out, "katana_http_connection_timeouts_total", "",
             connection_timeouts.load(std::memory_order_relaxed));
        out += "# HELP katana_http_connections_active Open client connections.\n";
        out += "# TYPE katana_http_connections_active gauge\n";
        const int64_t conns = active_connections.load(std::memory_order_relaxed);
        line(out, "katana_http_connections_active", "", static_cast<uint64_t>(conns < 0 ? 0 : conns));
        out += "# HELP katana_http_connections_rejected_total Connections refused at the "
               "max-connections cap.\n";
        out += "# TYPE katana_http_connections_rejected_total counter\n";
        line(out, "katana_http_connections_rejected_total", "",
             connections_rejected.load(std::memory_order_relaxed));

        const uint64_t dur_count = t.duration_count;
        out += "# HELP katana_http_request_duration_seconds Request handling latency.\n";
        out += "# TYPE katana_http_request_duration_seconds histogram\n";
        for (size_t i = 0; i < duration_bound_labels.size(); ++i) {
            std::string labels = "{le=\"";
            labels += duration_bound_labels[i];
            labels += "\"}";
            line(out, "katana_http_request_duration_seconds_bucket", labels,
                 t.cumulative_buckets[i]);
        }
        line(out, "katana_http_request_duration_seconds_bucket", "{le=\"+Inf\"}", dur_count);
        // _sum is in seconds; format from the integer microsecond accumulator.
        const uint64_t sum_micros = t.duration_sum_micros;
        out += "katana_http_request_duration_seconds_sum ";
        out += std::to_string(sum_micros / 1'000'000);
        out += '.';
        out += format_micros_fraction(sum_micros % 1'000'000);
        out += '\n';
        line(out, "katana_http_request_duration_seconds_count", "", dur_count);
        return out;
    }
};

// Cross-Origin Resource Sharing policy. With defaults (allowed_origins empty) any origin is
// reflected — convenient for a separate SPA/Next.js frontend in dev. For production set an
// explicit allow-list; with allow_credentials the matched origin is echoed (never "*").
struct cors_config {
    std::vector<std::string> allowed_origins{};                         // empty / {"*"} => any
    std::string allowed_methods = "GET, POST, PUT, PATCH, DELETE, OPTIONS";
    std::string allowed_headers = "Content-Type, Authorization";        // "*" allowed
    std::string exposed_headers = "";
    bool allow_credentials = false;
    int max_age_seconds = 600;
};

// Edge admission control. A global in-flight cap sheds load with 503 when the server is saturated;
// a per-client rate caps requests-per-second per source IP with 429. Both off (0) by default.
struct load_shedding_config {
    size_t max_in_flight = 0;  // 0 = unlimited; concurrent in-flight requests before 503
    size_t per_client_rps = 0; // 0 = off; requests/second per client IP before 429
};

// Per-client fixed-window rate limiter keyed by IP. Thread-safe (worker reactors share it). Simple
// by design — the bucket map is not evicted, so it fits an edge guard, not an unbounded keyspace.
class edge_limiter {
public:
    explicit edge_limiter(size_t rps) : rps_(rps) {}

    [[nodiscard]] bool allow(std::string_view ip) {
        const auto now = std::chrono::steady_clock::now();
        std::lock_guard<std::mutex> lock(mutex_);
        auto& b = buckets_[std::string(ip)];
        if (now - b.window_start >= std::chrono::seconds(1)) {
            b.window_start = now;
            b.count = 0;
        }
        if (b.count >= rps_) {
            return false;
        }
        ++b.count;
        return true;
    }

private:
    struct bucket {
        std::chrono::steady_clock::time_point window_start{};
        size_t count = 0;
    };
    size_t rps_;
    std::mutex mutex_;
    std::unordered_map<std::string, bucket> buckets_;
};

/// High-level HTTP server abstraction
///
/// Encapsulates reactor pool, listener, connection handling, and lifecycle management.
/// Provides a simple, fluent interface for setting up an HTTP server.
///
/// Example:
/// @code
///   router api_router(routes);
///   http::server(api_router)
///       .bind("0.0.0.0", 8080)
///       .workers(4)
///       .graceful_shutdown(std::chrono::seconds(5))
///       .on_start([]() { std::cout << "Server started\n"; })
///       .run();
/// @endcode
class server {
public:
    /// Construct server with a router
    explicit server(const router& rt)
        : router_(&rt),
          dispatch_callback_([&rt](const request& req, request_context& ctx, response& out) {
              return rt.dispatch(req, ctx, out);
          }),
          route_count_fn_([&rt] { return rt.route_count(); }),
          route_label_fn_([&rt](size_t i) { return rt.route_label(i); }) {}

    /// Construct server with a composite_router (several contracts on one server). Per-route
    /// metrics span all contracts via globalised route indices.
    explicit server(const composite_router& composite)
        : dispatch_callback_(
              [&composite](const request& req, request_context& ctx, response& out) {
                  return composite.dispatch_to(req, ctx, out);
              }),
          route_count_fn_([&composite] { return composite.route_count(); }),
          route_label_fn_([&composite](size_t i) { return composite.route_label(i); }) {}

    /// Construct server with a dispatcher that exposes dispatch_to(req, ctx, out)
    template <typename Dispatcher>
        requires requires(const Dispatcher& dispatcher,
                          const request& req,
                          request_context& ctx,
                          response& out) {
            { dispatcher.dispatch_to(req, ctx, out) } -> std::same_as<result<void>>;
        }
    explicit server(const Dispatcher& dispatcher)
        : dispatch_callback_(
              [&dispatcher](const request& req, request_context& ctx, response& out) {
                  return dispatcher.dispatch_to(req, ctx, out);
              }) {}

    /// Set bind address and port
    server& bind(const std::string& host, uint16_t port) {
        host_ = host;
        port_ = port;
        return *this;
    }

    /// Set port only (binds to 0.0.0.0)
    server& listen(uint16_t port) { return bind("0.0.0.0", port); }

    /// Set number of worker threads (reactor pool size)
    server& workers(size_t count) {
        worker_count_ = count;
        return *this;
    }

    /// Set backlog size for listening socket
    server& backlog(int32_t size) {
        backlog_ = size;
        return *this;
    }

    /// Enable/disable SO_REUSEPORT
    server& reuseport(bool enable = true) {
        reuseport_ = enable;
        return *this;
    }

    /// Set graceful shutdown timeout
    server& graceful_shutdown(std::chrono::milliseconds timeout) {
        shutdown_timeout_ = timeout;
        return *this;
    }

    /// Set callback to be called when server starts
    server& on_start(std::function<void()> callback) {
        on_start_callback_ = std::move(callback);
        return *this;
    }

    /// Set callback to be called when server stops
    server& on_stop(std::function<void()> callback) {
        on_stop_callback_ = std::move(callback);
        return *this;
    }

    /// Set callback to be called on each request (for logging, metrics, etc.)
    server& on_request(std::function<void(const request&, const response&)> callback) {
        on_request_callback_ = std::move(callback);
        return *this;
    }

    /// Attach route policy executor to every request context created by this server. When auth is
    /// also enabled, the server composes an auth executor *ahead* of this one (auth runs first).
    server& policy_executor(route_policy_executor& executor) {
        app_policy_executor_ = &executor;
        return *this;
    }

    /// Register a global middleware that runs on every request **before** routing and the route
    /// policy chain (cache/rate-limit/idempotency) — the seam contract-generated routes lack. Use it
    /// for request-id/logging middleware, or path-scoped auth that must gate before policies. Runs in
    /// registration order; a middleware that doesn't call `next` short-circuits the request.
    server& use(middleware_fn middleware) {
        global_middleware_.push_back(std::move(middleware));
        return *this;
    }

    /// Enable JWT bearer auth (HS256/RS256/ES256 + JWKS). Routes annotated with x-katana-auth are
    /// enforced by the contract-first executor; the `auth::require_auth()` middleware can also use
    /// this configuration. Combine with api_key_auth() to accept either.
    server& jwt_auth(auth::jwt_auth_config config) {
        ensure_authenticator().configure_jwt(std::move(config));
        return *this;
    }

    /// Enable API-key auth (a header carrying a key mapped to a principal).
    server& api_key_auth(auth::api_key_config config) {
        ensure_authenticator().configure_api_key(std::move(config));
        return *this;
    }

    /// The server's authenticator (shared with the contract-first executor). Null until jwt_auth /
    /// api_key_auth is called — useful for building `require_auth()` middleware over the same config.
    [[nodiscard]] std::shared_ptr<const auth::authenticator> authenticator() const {
        return authenticator_;
    }

    /// Enable or disable the built-in `/healthz` and `/readyz` endpoints (on by default).
    server& health_endpoints(bool enable) {
        health_endpoints_enabled_ = enable;
        return *this;
    }

    /// Override the liveness/readiness paths (defaults `/healthz`, `/readyz`).
    server& health_paths(std::string liveness, std::string readiness) {
        health_path_ = std::move(liveness);
        ready_path_ = std::move(readiness);
        return *this;
    }

    /// Register a readiness probe. `/readyz` returns 503 unless it returns true (and the
    /// server is not shutting down). Typical use: check the DB pool / dependencies.
    server& readiness_check(std::function<bool()> probe) {
        readiness_probe_ = std::move(probe);
        return *this;
    }

    /// Enable or disable the built-in Prometheus `/metrics` endpoint (on by default).
    server& metrics_endpoint(bool enable) {
        metrics_enabled_ = enable;
        return *this;
    }

    /// Override the metrics path (default `/metrics`).
    server& metrics_path(std::string path) {
        metrics_path_ = std::move(path);
        return *this;
    }

    /// Enable the built-in structured access log: one JSON line per request (method, path,
    /// status, response bytes, correlation id) emitted via `katana::log` at info level. Off by
    /// default — it logs every request, so opt in explicitly.
    server& access_log(bool enable = true, uint32_t sample_every = 1) {
        access_log_enabled_ = enable;
        access_log_sample_every_ = sample_every == 0 ? 1 : sample_every;
        return *this;
    }

    /// Enable per-connection idle/read/write timeouts (slowloris protection). A connection that
    /// sees no progress within the relevant window is closed by the reactor. Off by default;
    /// `read`/`write` bound a single read/write that stalls, `idle` bounds time between
    /// requests on a kept-alive connection. Pass values; call without args for sane defaults.
    server& connection_timeout(std::chrono::milliseconds read = std::chrono::seconds(30),
                               std::chrono::milliseconds write = std::chrono::seconds(30),
                               std::chrono::milliseconds idle = std::chrono::seconds(60)) {
        connection_timeout_ = timeout_config{read, write, idle};
        return *this;
    }

    /// Expose reactor/worker gauges at `/metrics` (worker count plus aggregated per-event
    /// reactor counters: tasks, fd events, timeouts, exceptions). The reactor already collects
    /// these; this only controls whether they appear in the scrape. The worker-count gauge is
    /// always emitted. Off by default.
    server& reactor_metrics(bool enable = true) {
        expose_reactor_metrics_ = enable;
        return *this;
    }

    /// Enable W3C distributed tracing. Each request continues an inbound `traceparent` (or
    /// starts a new trace) into `request_context::trace`; a span is emitted per sampled request
    /// (via `katana::log`, msg `"span"`) with trace/span/parent ids, name, status and duration.
    /// Off by default.
    server& tracing(bool enable = true) {
        tracing_enabled_ = enable;
        return *this;
    }

    /// Replace the default span sink (a `katana::log` "span" line) with a custom exporter,
    /// called once per sampled request with the completed span. This is the seam for OTLP /
    /// Zipkin / any backend — the framework hands you the span; you own the wire protocol.
    server& span_exporter(std::function<void(const tracing::span_record&)> exporter) {
        span_exporter_ = std::move(exporter);
        return *this;
    }

    /// Enable CORS. Handles preflight `OPTIONS` (204 + Access-Control-* headers) before routing
    /// and adds the matching CORS headers to every response for an allowed Origin. Call with no
    /// args for permissive any-origin defaults (handy for a separate frontend in dev), or pass a
    /// configured policy for production.
    server& cors(cors_config config = {}) {
        cors_ = std::move(config);
        return *this;
    }

    /// Enable TLS/HTTPS termination. Pass a `tls_config` with cert/key paths (and optional ALPN,
    /// CA, ciphers). The context is built once at `run()`; every accepted connection performs the
    /// TLS handshake before its first request. Reads/writes then flow through OpenSSL transparently.
    server& tls(tls::tls_config config) {
        tls_config_ = std::move(config);
        return *this;
    }

    /// Enable response compression (gzip/brotli/zstd). Compressible responses above the size
    /// threshold are encoded per the request's Accept-Encoding, with Content-Encoding + Vary set.
    server& compression(compression_config config = {}) {
        compression_ = config;
        return *this;
    }

    /// Edge admission control: shed load with 503 above `max_in_flight` concurrent requests, and
    /// rate-limit per client IP to `per_client_rps` (429 + Retry-After). Runs before routing.
    server& load_shedding(load_shedding_config config) {
        load_shedding_ = config;
        return *this;
    }

    /// Rebuild the TLS context from the configured cert/key paths and swap it in atomically, so a
    /// rotated certificate is picked up without dropping in-flight connections (they keep their
    /// existing session). Call from a SIGHUP handler / config-reload path. Returns false if the new
    /// cert/key fail to load (the old context stays active). No-op when TLS isn't enabled.
    bool reload_tls();

    /// Cap the number of simultaneously-open client connections (across all workers). Accepts
    /// beyond the cap are closed immediately and counted in
    /// `katana_http_connections_rejected_total`. 0 (default) means unlimited.
    server& max_connections(size_t limit) {
        max_connections_ = limit;
        return *this;
    }

    /// Access the live server metrics registry (e.g. for tests or custom export).
    [[nodiscard]] server_metrics& metrics() noexcept { return metrics_; }

    /// Run the server (blocking)
    /// Returns 0 on success, non-zero on error
    int run();

private:
    enum class flush_result : uint8_t { complete, blocked, error };

    void dispatch_request(const request& req, request_context& ctx, response& out) const {
        metrics_.begin_request();
        if (auto rid = req.headers.get(field::x_request_id)) {
            ctx.request_id = *rid; // inbound correlation id, visible to handlers
        }

        // Edge admission control: shed before any routing work. in_flight was just incremented, so
        // finalize_response balances it whether we serve or shed.
        if (load_shedding_) {
            if (load_shedding_->max_in_flight != 0 &&
                metrics_.in_flight_now() >
                    static_cast<int64_t>(load_shedding_->max_in_flight)) {
                out.assign_error(problem_details::service_unavailable("server overloaded"));
                out.set_header("Retry-After", "1");
                return;
            }
            if (edge_limiter_ && !ctx.client_ip.empty() && !edge_limiter_->allow(ctx.client_ip)) {
                out.assign_error(problem_details::too_many_requests("rate limit exceeded"));
                out.set_header("Retry-After", "1");
                return;
            }
        }

        if (try_serve_cors_preflight(req, out) || try_serve_health(req, out) ||
            try_serve_metrics(req, out)) {
            return;
        }

        // Terminal step: routing + the route's policy chain + handler. Errors are mapped into `out`.
        const handler_fn route_terminal =
            [this](const request& r, request_context& c, response& o) -> result<void> {
            if (router_) {
                dispatch_or_problem(*router_, r, c, o);
                return {};
            }
            auto dr = dispatch_callback_(r, c, o);
            if (!dr) {
                map_route_error(dr.error(), o);
            }
            return {};
        };

        // Global pre-routing middleware (server.use) runs ahead of routing + policies.
        if (global_middleware_.empty()) {
            (void)route_terminal(req, ctx, out);
            return;
        }
        const middleware_chain global{global_middleware_.data(), global_middleware_.size()};
        (void)global.run(req, ctx, route_terminal, out);
    }

    // Serve the built-in liveness/readiness endpoints. Returns true if it handled the
    // request (so the user router is bypassed).
    [[nodiscard]] bool try_serve_health(const request& req, response& out) const {
        if (!health_endpoints_enabled_ || req.http_method != method::get) {
            return false;
        }
        std::string_view path = req.uri;
        if (auto pos = path.find_first_of("?#"); pos != std::string_view::npos) {
            path = path.substr(0, pos);
        }
        const bool is_live = path == health_path_;
        const bool is_ready = path == ready_path_;
        if (!is_live && !is_ready) {
            return false;
        }
        bool ok = true;
        std::string_view state = "alive";
        if (is_ready) {
            ok = !shutdown_manager::instance().is_shutdown_requested() &&
                 (!readiness_probe_ || readiness_probe_());
            state = ok ? "ready" : "not ready";
        }
        out.status = ok ? 200 : 503;
        out.reason.assign(canonical_reason_phrase(out.status));
        out.body = "{\"status\":\"";
        out.body += state;
        out.body += "\"}";
        out.set_header("Content-Type", "application/json");
        return true;
    }

    // Serve the built-in Prometheus /metrics endpoint.
    [[nodiscard]] bool try_serve_metrics(const request& req, response& out) const {
        if (!metrics_enabled_ || req.http_method != method::get) {
            return false;
        }
        std::string_view path = req.uri;
        if (auto pos = path.find_first_of("?#"); pos != std::string_view::npos) {
            path = path.substr(0, pos);
        }
        if (path != metrics_path_) {
            return false;
        }
        out.status = 200;
        out.reason.assign(canonical_reason_phrase(out.status));
        out.body = metrics_.to_prometheus();
        append_route_metrics(out.body);
        append_reactor_metrics(out.body);
        out.set_header("Content-Type", "text/plain; version=0.0.4");
        return true;
    }

    struct connection_state : std::enable_shared_from_this<connection_state> {
        using deferred_response_slot = std::optional<response>;

        tcp_socket socket;
        // When set, the connection speaks TLS: reads/writes route through the SSL session (which
        // owns the SSL* bound to socket's fd, BIO_NOCLOSE). handshake_done gates the parse loop.
        std::optional<tls::ssl_session> tls;
        bool handshake_done = false;
        std::string client_ip; // peer IP captured at accept (for edge rate-limiting / access logs)
        std::string active_response;
        std::string active_response_body;
        std::string queued_response;
        std::string queued_response_body;
        std::string response_scratch;
        size_t write_pos = 0;
        monotonic_arena arena;
        parser http_parser;
        std::unique_ptr<fd_watch> watch;
        bool close_requested = false; // Track if connection should close after response
        size_t active_response_completed_requests = 0;
        bool queued_close_requested = false;
        size_t queued_response_completed_requests = 0;
        event_type watch_events = event_type::readable;
        // When the current request began dispatch — read back in finalize to record latency.
        // Safe across the deferred path: a connection stops reading new requests until its
        // deferred response completes, so this is never overwritten mid-flight.
        std::chrono::steady_clock::time_point request_start{};
        // Tracing span for the current request (same lifetime guarantee as request_start).
        tracing::span_context current_trace{};
        // Matched route index for the current request (-1 if none) — for per-route metrics.
        int current_route_index = -1;
        deferred_response_slot deferred_ready_response{};
        bool deferred_response_active = false;
        server* owner_server = nullptr;
        reactor* owner_reactor = nullptr;
        // When set, decremented on destruction so the server's active-connection gauge tracks
        // this connection's lifetime (incremented at accept).
        std::atomic<int64_t>* active_conn_counter = nullptr;

        ~connection_state() {
            if (active_conn_counter != nullptr) {
                active_conn_counter->fetch_sub(1, std::memory_order_relaxed);
            }
        }

        explicit connection_state(tcp_socket sock)
            : socket(std::move(sock)), arena(detail::HTTP_SERVER_ARENA_CAPACITY),
              http_parser(&arena) {
            active_response.reserve(detail::HTTP_SERVER_RESPONSE_BUFFER_CAPACITY);
            active_response_body.reserve(detail::HTTP_SERVER_RESPONSE_BUFFER_CAPACITY);
            queued_response.reserve(detail::HTTP_SERVER_RESPONSE_BUFFER_CAPACITY);
            queued_response_body.reserve(detail::HTTP_SERVER_RESPONSE_BUFFER_CAPACITY);
            response_scratch.reserve(detail::HTTP_SERVER_RESPONSE_BUFFER_CAPACITY);
        }

        [[nodiscard]] size_t pending_response_bytes() const noexcept {
            return active_response.size() + active_response_body.size();
        }

        [[nodiscard]] bool has_pending_response() const noexcept {
            return write_pos < pending_response_bytes();
        }

        [[nodiscard]] size_t queued_response_bytes() const noexcept {
            return queued_response.size() + queued_response_body.size();
        }

        [[nodiscard]] bool has_queued_response() const noexcept {
            return queued_response_bytes() != 0 || queued_response_completed_requests != 0;
        }

        // Transport dispatch: TLS session when present, else the raw socket. Same read/write/writev
        // contract either way (empty span = would-block, 0 = blocked, error_code::ok = EOF).
        result<std::span<uint8_t>> transport_read(std::span<uint8_t> buf) {
            return tls ? tls->read(buf) : socket.read(buf);
        }
        result<size_t> transport_write(std::span<const uint8_t> data) {
            return tls ? tls->write(data) : socket.write(data);
        }
        result<size_t> transport_writev(const iovec* iov, size_t count) {
            return tls ? tls->writev(iov, count) : socket.writev(iov, count);
        }

        result<void> set_watch_events(event_type events) {
            if (!watch || watch_events == events) {
                return {};
            }
            auto res = watch->modify(events);
            if (res) {
                watch_events = events;
            }
            return res;
        }
    };

    flush_result flush_active_response(connection_state& state);
    void prepare_active_response(connection_state& state, response& resp);
    static deferred_response_handle make_deferred_response_handle(
        void* user, request_context::response_complete_fn on_complete);
    static bool complete_deferred_response_opaque(std::shared_ptr<void> opaque_state,
                                                  response resp);
    static void cancel_deferred_response_opaque(std::shared_ptr<void> opaque_state);
    void complete_deferred_response(connection_state& state, response resp, reactor& r);
    void handle_connection(connection_state& state, reactor& r);

    const router* router_ = nullptr;
    inplace_function<result<void>(const request&, request_context&, response&), 64>
        dispatch_callback_;
    // Route metadata for per-route metrics, supplied by the router/composite_router constructor
    // (empty for a bare custom Dispatcher, which then has no per-route labels).
    std::function<size_t()> route_count_fn_;
    std::function<std::string(size_t)> route_label_fn_;
    std::string host_ = "0.0.0.0";
    uint16_t port_ = 8080;
    size_t worker_count_ = 1;
    int32_t backlog_ = 1024;
    bool reuseport_ = true;
    std::chrono::milliseconds shutdown_timeout_{5000};
    std::function<void()> on_start_callback_;
    std::function<void()> on_stop_callback_;
    std::function<void(const request&, const response&)> on_request_callback_;
    std::vector<middleware_fn> global_middleware_; // server.use(): runs before routing + policies
    route_policy_executor* policy_executor_ = nullptr;

    // Built-in health/readiness endpoints (served before the user router). `/healthz` is
    // liveness (always 200 while the process serves requests); `/readyz` is readiness —
    // 503 while shutting down or when a registered probe reports not-ready, else 200.
    bool health_endpoints_enabled_ = true;
    std::string health_path_ = "/healthz";
    std::string ready_path_ = "/readyz";
    std::function<bool()> readiness_probe_;

    // Built-in Prometheus metrics endpoint. mutable so the const dispatch path can update
    // the in-flight gauge.
    bool metrics_enabled_ = true;
    std::string metrics_path_ = "/metrics";
    mutable server_metrics metrics_;

    // Built-in structured access log (opt-in via access_log()). With sampling, only 1 in
    // `access_log_sample_every_` requests is logged (the counter is shared across workers).
    bool access_log_enabled_ = false;
    uint32_t access_log_sample_every_ = 1;
    mutable std::atomic<uint64_t> access_log_counter_{0};

    // Per-connection idle/read/write timeouts (opt-in via connection_timeout()). When unset,
    // connections are registered without a timeout (legacy behavior).
    std::optional<timeout_config> connection_timeout_;

    // Max simultaneous connections across all workers (0 = unlimited).
    size_t max_connections_ = 0;

    // CORS policy (opt-in via cors()).
    std::optional<cors_config> cors_;
    std::optional<compression_config> compression_;
    std::optional<load_shedding_config> load_shedding_;
    std::unique_ptr<edge_limiter> edge_limiter_; // built in run() when per_client_rps is set

    // TLS: config set via tls(); the shared context is built once in run() and each connection gets
    // its own ssl_session. Held as shared_ptr so a future hot-reload can swap it atomically.
    std::optional<tls::tls_config> tls_config_;
    std::shared_ptr<tls::tls_context> tls_ctx_;      // swapped under tls_ctx_mutex_ for hot-reload
    mutable std::mutex tls_ctx_mutex_;

    // Auth: the shared authenticator (jwt/api-key config) and the contract-first executor built from
    // it, composed ahead of the app's policy executor in run().
    std::shared_ptr<auth::authenticator> authenticator_;
    std::optional<auth::auth_executor> auth_executor_;
    std::optional<route_policy_executor_chain> policy_chain_;
    route_policy_executor* app_policy_executor_ = nullptr;

    auth::authenticator& ensure_authenticator() {
        if (!authenticator_) {
            authenticator_ = std::make_shared<auth::authenticator>();
        }
        return *authenticator_;
    }

    // Thread-safe snapshot of the active TLS context (accept runs on worker threads; reload_tls may
    // swap it concurrently).
    [[nodiscard]] std::shared_ptr<tls::tls_context> current_tls_context() const {
        std::lock_guard<std::mutex> lock(tls_ctx_mutex_);
        return tls_ctx_;
    }

    [[nodiscard]] bool cors_origin_allowed(std::string_view origin) const {
        if (cors_->allowed_origins.empty()) {
            return true; // any origin
        }
        for (const auto& o : cors_->allowed_origins) {
            if (o == "*" || o == origin) {
                return true;
            }
        }
        return false;
    }

    // Add the CORS response headers for an allowed Origin (shared by preflight and actual
    // responses). With credentials the specific origin is echoed; otherwise "*" may be used.
    void add_cors_headers(std::string_view origin, response& out) const {
        const bool any = cors_->allowed_origins.empty() ||
                         (cors_->allowed_origins.size() == 1 && cors_->allowed_origins[0] == "*");
        if (any && !cors_->allow_credentials) {
            out.headers.set_unknown("Access-Control-Allow-Origin", "*");
        } else {
            out.headers.set_unknown("Access-Control-Allow-Origin", origin);
            out.headers.set_unknown("Vary", "Origin");
        }
        if (cors_->allow_credentials) {
            out.headers.set_unknown("Access-Control-Allow-Credentials", "true");
        }
        if (!cors_->exposed_headers.empty()) {
            out.headers.set_unknown("Access-Control-Expose-Headers", cors_->exposed_headers);
        }
    }

    // Serve a CORS preflight (OPTIONS with Origin + Access-Control-Request-Method). Returns true
    // if handled. Disallowed origins get a 403 so the browser surfaces a clear CORS failure.
    [[nodiscard]] bool try_serve_cors_preflight(const request& req, response& out) const {
        if (!cors_ || req.http_method != method::options) {
            return false;
        }
        auto origin = req.header("origin");
        if (!origin || !req.header("access-control-request-method")) {
            return false; // not a CORS preflight
        }
        if (!cors_origin_allowed(*origin)) {
            out.status = 403;
            out.reason.assign(canonical_reason_phrase(403));
            out.body.clear();
            return true;
        }
        add_cors_headers(*origin, out);
        out.headers.set_unknown("Access-Control-Allow-Methods", cors_->allowed_methods);
        // Echo the requested headers when configured to allow any, else advertise the allow-list.
        if (cors_->allowed_headers == "*") {
            if (auto reqh = req.header("access-control-request-headers")) {
                out.headers.set_unknown("Access-Control-Allow-Headers", *reqh);
            }
        } else {
            out.headers.set_unknown("Access-Control-Allow-Headers", cors_->allowed_headers);
        }
        out.headers.set_unknown("Access-Control-Max-Age", std::to_string(cors_->max_age_seconds));
        out.status = 204;
        out.reason.assign(canonical_reason_phrase(204));
        out.body.clear();
        return true;
    }

    // W3C distributed tracing (opt-in via tracing()).
    bool tracing_enabled_ = false;
    std::function<void(const tracing::span_record&)> span_exporter_;

    // Reactor/worker gauges at /metrics (opt-in via reactor_metrics()). pool_ points at the
    // run()-local pool while the server is serving (cleared before run() returns).
    bool expose_reactor_metrics_ = false;
    reactor_pool* pool_ = nullptr;

    // Append reactor/worker gauges to a /metrics body (only when enabled and serving).
    void append_reactor_metrics(std::string& body) const {
        body += "# HELP katana_http_workers Configured worker (reactor) threads.\n";
        body += "# TYPE katana_http_workers gauge\n";
        body += "katana_http_workers ";
        body += std::to_string(worker_count_);
        body += '\n';
        if (!expose_reactor_metrics_ || pool_ == nullptr) {
            return;
        }
        const auto m = pool_->aggregate_metrics();
        const auto gauge = [&](std::string_view name, std::string_view help, uint64_t v) {
            body += "# HELP ";
            body += name;
            body += ' ';
            body += help;
            body += "\n# TYPE ";
            body += name;
            body += " counter\n";
            body += name;
            body += ' ';
            body += std::to_string(v);
            body += '\n';
        };
        gauge("katana_reactor_tasks_executed_total", "Tasks executed across reactors.",
              m.tasks_executed);
        gauge("katana_reactor_fd_events_total", "FD events processed across reactors.",
              m.fd_events_processed);
        gauge("katana_reactor_fd_timeouts_total", "FD timeouts fired across reactors.",
              m.fd_timeouts);
        gauge("katana_reactor_tasks_rejected_total", "Tasks rejected (backpressure).",
              m.tasks_rejected);
        gauge("katana_reactor_exceptions_total", "Exceptions caught in reactor callbacks.",
              m.exceptions_caught);
    }

    // Per-route metrics, sized from the router at run() (index = ctx.route_index). Populated
    // only when constructed with a router. Atomics are written from worker threads and read by
    // the /metrics handler.
    // Per-route counters, sharded like server_metrics: shard-major layout
    // [shard0: route0..routeN | shard1: route0..routeN | ...] so each worker touches only
    // its own region; scrape sums the stride.
    std::vector<std::atomic<uint64_t>> per_route_requests_;
    std::vector<std::atomic<uint64_t>> per_route_duration_micros_;
    size_t per_route_count_ = 0;

    void note_route_request(size_t route_index, uint64_t duration_micros) noexcept {
        if (route_index >= per_route_count_) {
            return;
        }
        const size_t base = server_metrics::shard_index() * per_route_count_ + route_index;
        per_route_requests_[base].fetch_add(1, std::memory_order_relaxed);
        per_route_duration_micros_[base].fetch_add(duration_micros, std::memory_order_relaxed);
    }

    [[nodiscard]] uint64_t sum_route_counter(const std::vector<std::atomic<uint64_t>>& v,
                                             size_t route_index) const noexcept {
        uint64_t total = 0;
        for (size_t s = 0; s < server_metrics::SHARD_COUNT; ++s) {
            total += v[s * per_route_count_ + route_index].load(std::memory_order_relaxed);
        }
        return total;
    }
    std::vector<std::string> route_labels_;

    void init_per_route_metrics() {
        if (!route_count_fn_ || !route_label_fn_) {
            return; // dispatcher without route metadata (e.g. a custom Dispatcher type)
        }
        const size_t n = route_count_fn_();
        per_route_count_ = n;
        per_route_requests_ = std::vector<std::atomic<uint64_t>>(n * server_metrics::SHARD_COUNT);
        per_route_duration_micros_ =
            std::vector<std::atomic<uint64_t>>(n * server_metrics::SHARD_COUNT);
        route_labels_.clear();
        route_labels_.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            route_labels_.push_back(route_label_fn_(i));
        }
    }

    // Append per-route counters to a /metrics body (no-op when no router/routes).
    void append_route_metrics(std::string& body) const {
        if (per_route_requests_.empty()) {
            return;
        }
        body += "# HELP katana_http_route_requests_total Requests handled per route.\n";
        body += "# TYPE katana_http_route_requests_total counter\n";
        for (size_t i = 0; i < per_route_count_; ++i) {
            body += "katana_http_route_requests_total{route=\"";
            body += route_labels_[i];
            body += "\"} ";
            body += std::to_string(sum_route_counter(per_route_requests_, i));
            body += '\n';
        }
        body += "# HELP katana_http_route_duration_seconds_sum Summed latency per route.\n";
        body += "# TYPE katana_http_route_duration_seconds_sum counter\n";
        for (size_t i = 0; i < per_route_count_; ++i) {
            const uint64_t us = sum_route_counter(per_route_duration_micros_, i);
            body += "katana_http_route_duration_seconds_sum{route=\"";
            body += route_labels_[i];
            body += "\"} ";
            body += std::to_string(us / 1'000'000);
            body += '.';
            body += server_metrics::format_micros_fraction(us % 1'000'000);
            body += '\n';
        }
    }
};

} // namespace http
} // namespace katana
