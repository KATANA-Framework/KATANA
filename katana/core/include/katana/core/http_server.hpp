#pragma once

#include "katana/core/arena.hpp"
#include "katana/core/fd_watch.hpp"
#include "katana/core/http.hpp"
#include "katana/core/reactor_pool.hpp"
#include "katana/core/router.hpp"
#include "katana/core/shutdown.hpp"
#include "katana/core/tcp_listener.hpp"
#include "katana/core/tcp_socket.hpp"

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
} // namespace detail

// Lightweight server-level RED metrics, exported in Prometheus text format via /metrics.
// Counters are by response status class; in_flight is a gauge of requests being processed.
struct server_metrics {
    std::atomic<uint64_t> requests_2xx{0};
    std::atomic<uint64_t> requests_3xx{0};
    std::atomic<uint64_t> requests_4xx{0};
    std::atomic<uint64_t> requests_5xx{0};
    std::atomic<int64_t> in_flight{0};

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
        return out;
    }
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

    /// Access the live server metrics registry (e.g. for tests or custom export).
    [[nodiscard]] server_metrics& metrics() noexcept { return metrics_; }

    /// Run the server (blocking)
    /// Returns 0 on success, non-zero on error
    int run();

private:
    enum class flush_result : uint8_t { complete, blocked, error };

    void dispatch_request(const request& req, request_context& ctx, response& out) const {
        metrics_.in_flight.fetch_add(1, std::memory_order_relaxed);
        if (try_serve_health(req, out) || try_serve_metrics(req, out)) {
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
        deferred_response_slot deferred_ready_response{};
        bool deferred_response_active = false;
        server* owner_server = nullptr;
        reactor* owner_reactor = nullptr;

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
};

} // namespace http
} // namespace katana
