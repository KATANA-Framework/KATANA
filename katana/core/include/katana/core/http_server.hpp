#pragma once

#include "katana/core/arena.hpp"
#include "katana/core/fd_watch.hpp"
#include "katana/core/http.hpp"
#include "katana/core/reactor_pool.hpp"
#include "katana/core/router.hpp"
#include "katana/core/shutdown.hpp"
#include "katana/core/tcp_listener.hpp"
#include "katana/core/tcp_socket.hpp"

#include <cerrno>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace katana {
namespace http {

namespace detail {
constexpr size_t HTTP_SERVER_RESPONSE_BUFFER_CAPACITY = 8192;
constexpr size_t HTTP_SERVER_ARENA_CAPACITY = 8192;
} // namespace detail

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

    /// Run the server (blocking)
    /// Returns 0 on success, non-zero on error
    int run();

private:
    enum class flush_result : uint8_t { complete, blocked, error };

    void dispatch_request(const request& req, request_context& ctx, response& out) const {
        if (router_) {
            dispatch_or_problem(*router_, req, ctx, out);
            return;
        }

        auto dispatch_result = dispatch_callback_(req, ctx, out);
        if (!dispatch_result) {
            map_route_error(dispatch_result.error(), out);
        }
    }

    struct connection_state {
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
};

} // namespace http
} // namespace katana
