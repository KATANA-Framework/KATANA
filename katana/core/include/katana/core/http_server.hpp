#pragma once

#include "katana/core/arena.hpp"
#include "katana/core/fd_watch.hpp"
#include "katana/core/http.hpp"
#include "katana/core/io_buffer.hpp"
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
#include <vector>

namespace katana {
namespace http {

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
    explicit server(const router& rt) : router_(rt) {}

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
    struct connection_state {
        tcp_socket socket;
        io_buffer read_buffer;
        io_buffer write_buffer;
        monotonic_arena arena;
        parser http_parser;
        std::unique_ptr<fd_watch> watch;
        bool close_requested = false; // Track if connection should close after response

        explicit connection_state(tcp_socket sock)
            : socket(std::move(sock)), read_buffer(8192), write_buffer(8192), arena(8192),
              http_parser(&arena) {}
    };

    void handle_connection(connection_state& state, reactor& r);
    void accept_connection(reactor& r,
                           tcp_listener& listener,
                           std::vector<std::unique_ptr<connection_state>>& connections);

    const router& router_;
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
