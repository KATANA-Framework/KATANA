#pragma once

#include "katana/core/arena.hpp"
#include "katana/core/fd_watch.hpp"
#include "katana/core/http.hpp"
#include "katana/core/log.hpp"
#include "katana/core/reactor_pool.hpp"
#include "katana/core/router.hpp"
#include "katana/core/shutdown.hpp"
#include "katana/core/tcp_listener.hpp"
#include "katana/core/tcp_socket.hpp"

#include <array>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

namespace katana {
namespace http {

namespace detail {
constexpr size_t HTTP_SERVER_RESPONSE_BUFFER_CAPACITY = 8192;
constexpr size_t HTTP_SERVER_ARENA_CAPACITY = 8192;

// Generate a process-unique correlation id (16 lowercase hex chars from a monotonic counter).
// Used when a request arrives without an `X-Request-Id` header.
inline std::string generate_request_id() {
    static std::atomic<uint64_t> counter{0};
    const uint64_t n = counter.fetch_add(1, std::memory_order_relaxed);
    static constexpr char hex[] = "0123456789abcdef";
    char out[16];
    for (int i = 15; i >= 0; --i) {
        out[i] = hex[(n >> ((15 - i) * 4)) & 0xF];
    }
    return std::string(out, 16);
}
} // namespace detail

// Lightweight server-level RED metrics, exported in Prometheus text format via /metrics.
// Counters are by response status class; in_flight is a gauge of requests being processed.
struct server_metrics {
    std::atomic<uint64_t> requests_2xx{0};
    std::atomic<uint64_t> requests_3xx{0};
    std::atomic<uint64_t> requests_4xx{0};
    std::atomic<uint64_t> requests_5xx{0};
    std::atomic<int64_t> in_flight{0};
    std::atomic<uint64_t> connection_timeouts{0};
    std::atomic<int64_t> active_connections{0};
    std::atomic<uint64_t> connections_rejected{0};

    // Request-duration histogram (Prometheus). Bucket upper bounds in microseconds, with the
    // matching `le` labels in seconds. Buckets are cumulative-on-read (observe increments every
    // bucket whose bound is >= the sample).
    static constexpr std::array<int64_t, 14> duration_bounds_micros{
        500,    1000,    2500,    5000,    10000,   25000,    50000,
        100000, 250000,  500000,  1000000, 2500000, 5000000,  10000000};
    static constexpr std::array<std::string_view, 14> duration_bound_labels{
        "0.0005", "0.001", "0.0025", "0.005", "0.01", "0.025", "0.05",
        "0.1",    "0.25",  "0.5",    "1",     "2.5",  "5",      "10"};
    std::array<std::atomic<uint64_t>, 14> duration_buckets{};
    std::atomic<uint64_t> duration_count{0};
    std::atomic<uint64_t> duration_sum_micros{0};

    void observe_duration_micros(int64_t micros) noexcept {
        if (micros < 0) {
            micros = 0;
        }
        for (size_t i = 0; i < duration_bounds_micros.size(); ++i) {
            if (micros <= duration_bounds_micros[i]) {
                duration_buckets[i].fetch_add(1, std::memory_order_relaxed);
            }
        }
        duration_count.fetch_add(1, std::memory_order_relaxed);
        duration_sum_micros.fetch_add(static_cast<uint64_t>(micros), std::memory_order_relaxed);
    }

    void record_status(int32_t status) noexcept {
        if (status >= 500) {
            requests_5xx.fetch_add(1, std::memory_order_relaxed);
        } else if (status >= 400) {
            requests_4xx.fetch_add(1, std::memory_order_relaxed);
        } else if (status >= 300) {
            requests_3xx.fetch_add(1, std::memory_order_relaxed);
        } else if (status >= 200) {
            requests_2xx.fetch_add(1, std::memory_order_relaxed);
        }
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
        std::string out;
        out += "# HELP katana_http_requests_total Total HTTP responses by status class.\n";
        out += "# TYPE katana_http_requests_total counter\n";
        line(out, "katana_http_requests_total", "{status=\"2xx\"}",
             requests_2xx.load(std::memory_order_relaxed));
        line(out, "katana_http_requests_total", "{status=\"3xx\"}",
             requests_3xx.load(std::memory_order_relaxed));
        line(out, "katana_http_requests_total", "{status=\"4xx\"}",
             requests_4xx.load(std::memory_order_relaxed));
        line(out, "katana_http_requests_total", "{status=\"5xx\"}",
             requests_5xx.load(std::memory_order_relaxed));
        out += "# HELP katana_http_requests_in_flight HTTP requests currently being served.\n";
        out += "# TYPE katana_http_requests_in_flight gauge\n";
        const int64_t live = in_flight.load(std::memory_order_relaxed);
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

        const uint64_t dur_count = duration_count.load(std::memory_order_relaxed);
        out += "# HELP katana_http_request_duration_seconds Request handling latency.\n";
        out += "# TYPE katana_http_request_duration_seconds histogram\n";
        for (size_t i = 0; i < duration_bound_labels.size(); ++i) {
            std::string labels = "{le=\"";
            labels += duration_bound_labels[i];
            labels += "\"}";
            line(out, "katana_http_request_duration_seconds_bucket", labels,
                 duration_buckets[i].load(std::memory_order_relaxed));
        }
        line(out, "katana_http_request_duration_seconds_bucket", "{le=\"+Inf\"}", dur_count);
        // _sum is in seconds; format from the integer microsecond accumulator.
        const uint64_t sum_micros = duration_sum_micros.load(std::memory_order_relaxed);
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
          }) {}

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

    /// Attach route policy executor to every request context created by this server.
    server& policy_executor(route_policy_executor& executor) {
        policy_executor_ = &executor;
        return *this;
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
        metrics_.in_flight.fetch_add(1, std::memory_order_relaxed);
        if (auto rid = req.header("x-request-id")) {
            ctx.request_id = *rid; // inbound correlation id, visible to handlers
        }
        if (try_serve_cors_preflight(req, out) || try_serve_health(req, out) ||
            try_serve_metrics(req, out)) {
            return;
        }
        if (router_) {
            dispatch_or_problem(*router_, req, ctx, out);
            return;
        }

        auto dispatch_result = dispatch_callback_(req, ctx, out);
        if (!dispatch_result) {
            map_route_error(dispatch_result.error(), out);
        }
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
    static deferred_response_handle make_deferred_response_handle(void* user);
    static bool complete_deferred_response_opaque(std::shared_ptr<void> opaque_state,
                                                  response resp);
    static void cancel_deferred_response_opaque(std::shared_ptr<void> opaque_state);
    void complete_deferred_response(connection_state& state, response resp, reactor& r);
    void handle_connection(connection_state& state, reactor& r);

    const router* router_ = nullptr;
    inplace_function<result<void>(const request&, request_context&, response&), 64>
        dispatch_callback_;
    std::string host_ = "0.0.0.0";
    uint16_t port_ = 8080;
    size_t worker_count_ = 1;
    int32_t backlog_ = 1024;
    bool reuseport_ = true;
    std::chrono::milliseconds shutdown_timeout_{5000};
    std::function<void()> on_start_callback_;
    std::function<void()> on_stop_callback_;
    std::function<void(const request&, const response&)> on_request_callback_;
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
    std::vector<std::atomic<uint64_t>> per_route_requests_;
    std::vector<std::atomic<uint64_t>> per_route_duration_micros_;
    std::vector<std::string> route_labels_;

    void init_per_route_metrics() {
        if (router_ == nullptr) {
            return;
        }
        const size_t n = router_->route_count();
        per_route_requests_ = std::vector<std::atomic<uint64_t>>(n);
        per_route_duration_micros_ = std::vector<std::atomic<uint64_t>>(n);
        route_labels_.clear();
        route_labels_.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            route_labels_.push_back(router_->route_label(i));
        }
    }

    // Append per-route counters to a /metrics body (no-op when no router/routes).
    void append_route_metrics(std::string& body) const {
        if (per_route_requests_.empty()) {
            return;
        }
        body += "# HELP katana_http_route_requests_total Requests handled per route.\n";
        body += "# TYPE katana_http_route_requests_total counter\n";
        for (size_t i = 0; i < per_route_requests_.size(); ++i) {
            body += "katana_http_route_requests_total{route=\"";
            body += route_labels_[i];
            body += "\"} ";
            body += std::to_string(per_route_requests_[i].load(std::memory_order_relaxed));
            body += '\n';
        }
        body += "# HELP katana_http_route_duration_seconds_sum Summed latency per route.\n";
        body += "# TYPE katana_http_route_duration_seconds_sum counter\n";
        for (size_t i = 0; i < per_route_duration_micros_.size(); ++i) {
            const uint64_t us = per_route_duration_micros_[i].load(std::memory_order_relaxed);
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
