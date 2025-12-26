#include "katana/core/http_server.hpp"
#include "katana/core/problem.hpp"

#include <atomic>
#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>

// Debug logging disabled for performance
#define DEBUG_LOG(fmt, ...)                                                                        \
    do {                                                                                           \
    } while (0)

namespace katana {
namespace http {

namespace {
struct conn_close_counters {
    std::atomic<uint64_t> read_error{0};
    std::atomic<uint64_t> read_eof{0};
    std::atomic<uint64_t> parse_error{0};
    std::atomic<uint64_t> write_error{0};
    std::atomic<uint64_t> close_header{0};
};

conn_close_counters& close_counters() {
    static conn_close_counters counters;
    return counters;
}

bool conn_debug_enabled() {
    static bool enabled = std::getenv("KATANA_CONN_DEBUG") != nullptr;
    return enabled;
}

void maybe_log_close(const char* reason, uint64_t count) {
    if (!conn_debug_enabled()) {
        return;
    }
    if (count <= 20 || count % 1000 == 0) {
        std::cerr << "[conn_debug] close " << reason << " count=" << count << "\n";
    }
}
} // namespace

void server::handle_connection(connection_state& state, [[maybe_unused]] reactor& r) {
    // DEBUG: Track iterations
    static thread_local int iter_count = 0;
    ++iter_count;
    DEBUG_LOG("[DEBUG] handle_connection iter=%d write_buf_empty=%d read_buf_empty=%d\n",
              iter_count,
              state.write_buffer.empty() ? 1 : 0,
              state.read_buffer.empty() ? 1 : 0);

    if (!state.write_buffer.empty()) {
        while (!state.write_buffer.empty()) {
            auto data = state.write_buffer.readable_span();
            auto write_result = state.socket.write(data);

            if (!write_result) {
                if (write_result.error().value() == EAGAIN ||
                    write_result.error().value() == EWOULDBLOCK) {
                    state.watch->modify(event_type::writable);
                    return;
                }
                state.watch.reset();
                return;
            }

            if (write_result.value() == 0) {
                break;
            }

            state.write_buffer.consume(write_result.value());
        }

        if (!state.write_buffer.empty()) {
            state.watch->modify(event_type::writable);
            return;
        }

        // Check if connection should be closed after completing write
        if (state.close_requested) {
            auto count = ++close_counters().close_header;
            maybe_log_close("close_header", count);
            state.watch.reset();
            return;
        }

        state.close_requested = false; // Reset for next request
        state.arena.reset();
        state.http_parser.reset(&state.arena);
        state.write_buffer.clear();
        if (state.read_buffer.empty()) {
            state.watch->modify(event_type::readable);
            return;
        }
    }

    while (true) {
        if (state.read_buffer.empty()) {
            auto buf = state.read_buffer.writable_span(4096);
            auto read_result = state.socket.read(buf);

            if (!read_result) {
                if (read_result.error().value() == EAGAIN ||
                    read_result.error().value() == EWOULDBLOCK) {
                    DEBUG_LOG("[DEBUG] Read EAGAIN, breaking from loop\n");
                    if (state.watch) {
                        state.watch->modify(event_type::readable);
                    }
                    return;
                }
                DEBUG_LOG("[DEBUG] Read error=%d, closing connection\n",
                          read_result.error().value());
                if (read_result.error().value() == static_cast<int>(error_code::ok)) {
                    auto count = ++close_counters().read_eof;
                    maybe_log_close("read_eof", count);
                } else {
                    auto count = ++close_counters().read_error;
                    maybe_log_close("read_error", count);
                }
                state.watch.reset();
                return;
            }

            if (read_result->empty()) {
                DEBUG_LOG("[DEBUG] Read would block, returning to event loop\n");
                if (state.watch) {
                    state.watch->modify(event_type::readable);
                }
                return;
            }

            DEBUG_LOG("[DEBUG] Read %zu bytes\n", read_result->size());
            state.read_buffer.commit(read_result->size());
        }

        auto readable = state.read_buffer.readable_span();
        auto parse_result = state.http_parser.parse(readable);

        if (!parse_result) {
            auto resp = response::error(problem_details::bad_request("Invalid HTTP request"));
            resp.serialize_into(state.write_buffer);
            auto count = ++close_counters().parse_error;
            maybe_log_close("parse_error", count);
            state.watch.reset();
            return;
        }

        if (!state.http_parser.is_complete()) {
            auto buf = state.read_buffer.writable_span(4096);
            auto read_result = state.socket.read(buf);
            if (!read_result) {
                if (read_result.error().value() == EAGAIN ||
                    read_result.error().value() == EWOULDBLOCK) {
                    if (state.watch) {
                        state.watch->modify(event_type::readable);
                    }
                    return;
                }
                auto count = ++close_counters().read_error;
                maybe_log_close("read_error", count);
                state.watch.reset();
                return;
            }
            if (read_result->empty()) {
                if (state.watch) {
                    state.watch->modify(event_type::readable);
                }
                return;
            }
            state.read_buffer.commit(read_result->size());
            continue;
        }

        size_t parsed_bytes = state.http_parser.bytes_parsed();
        state.read_buffer.consume(parsed_bytes);

        DEBUG_LOG("[DEBUG] Request parsed, read_buf_size_after_consume=%zu\n",
                  state.read_buffer.readable_span().size());

        const auto& req = state.http_parser.get_request();
        request_context ctx{state.arena};
        auto resp = dispatch_or_problem(router_, req, ctx);

        if (on_request_callback_) {
            on_request_callback_(req, resp);
        }

        auto connection_header = req.headers.get("Connection");
        bool close_connection =
            connection_header && (*connection_header == "close" || *connection_header == "Close");

        if (!resp.headers.get("Connection")) {
            resp.set_header("Connection", close_connection ? "close" : "keep-alive");
        }

        state.close_requested = close_connection; // Remember for deferred write completion

        resp.serialize_into(state.write_buffer);

        size_t total_sent = 0;
        while (!state.write_buffer.empty()) {
            auto data = state.write_buffer.readable_span();
            auto write_result = state.socket.write(data);

            if (!write_result) {
                if (write_result.error().value() == EAGAIN ||
                    write_result.error().value() == EWOULDBLOCK) {
                    DEBUG_LOG("[DEBUG] Write EAGAIN, total_sent=%zu, remaining=%zu\n",
                              total_sent,
                              state.write_buffer.readable_span().size());
                    state.watch->modify(event_type::writable);
                    return;
                }
                DEBUG_LOG("[DEBUG] Write error=%d, total_sent=%zu\n",
                          write_result.error().value(),
                          total_sent);
                auto err_val = write_result.error().value();
                auto count = ++close_counters().write_error;
                if (conn_debug_enabled() && (count <= 20 || count % 1000 == 0)) {
                    std::cerr << "[conn_debug] close write_error count=" << count
                              << " errno=" << err_val << "\n";
                }
                state.watch.reset();
                return;
            }

            if (write_result.value() == 0) {
                DEBUG_LOG("[DEBUG] Write returned 0, total_sent=%zu\n", total_sent);
                break;
            }

            total_sent += write_result.value();
            state.write_buffer.consume(write_result.value());
        }

        if (!state.write_buffer.empty()) {
            DEBUG_LOG("[DEBUG] Write incomplete, total_sent=%zu, remaining=%zu\n",
                      total_sent,
                      state.write_buffer.readable_span().size());
            state.watch->modify(event_type::writable);
            return;
        }

        DEBUG_LOG("[DEBUG] Write complete, total_sent=%zu bytes\n", total_sent);

        if (close_connection) {
            DEBUG_LOG("[DEBUG] Close connection requested, exiting\n");
            auto count = ++close_counters().close_header;
            maybe_log_close("close_header", count);
            state.watch.reset();
            return;
        }

        DEBUG_LOG("[DEBUG] Response sent, continuing keep-alive loop\n");

        state.close_requested = false; // Reset for next keep-alive request
        state.arena.reset();
        state.http_parser.reset(&state.arena);
        if (state.read_buffer.empty()) {
            DEBUG_LOG("[DEBUG] Read buffer empty, switching to readable and returning\n");
            if (state.watch) {
                state.watch->modify(event_type::readable);
            } else {
                if (conn_debug_enabled()) {
                    std::cerr << "[CRITICAL] state.watch is NULL after response send!\n";
                }
            }
            return;
        } else {
            DEBUG_LOG("[DEBUG] Read buffer has %zu bytes, continuing loop\n",
                      state.read_buffer.readable_span().size());
        }
    }
    DEBUG_LOG("[DEBUG] Exiting handle_connection (while loop ended)\n");
}

void server::accept_connection(reactor& r,
                               tcp_listener& listener,
                               std::vector<std::unique_ptr<connection_state>>& connections) {
    auto accept_result = listener.accept();
    if (!accept_result) {
        return;
    }

    auto state = std::make_unique<connection_state>(std::move(*accept_result));
    int32_t fd = state->socket.native_handle();

    auto* state_ptr = state.get();
    state->watch =
        std::make_unique<fd_watch>(r, fd, event_type::readable, [this, state_ptr, &r](event_type) {
            handle_connection(*state_ptr, r);
        });

    connections.push_back(std::move(state));
}

int server::run() {
    reactor_pool_config config;
    config.reactor_count = static_cast<uint32_t>(worker_count_);
    config.enable_adaptive_balancing = true;
    reactor_pool pool(config);

    std::vector<std::shared_ptr<fd_watch>> accept_watches;

    auto accept_handler = [this](reactor& r, int listener_fd) {
        while (true) {
            int fd = ::accept4(listener_fd, nullptr, nullptr, SOCK_NONBLOCK | SOCK_CLOEXEC);
            if (fd < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    break;
                }
                return;
            }

            auto state = std::make_shared<connection_state>(tcp_socket(fd));
            auto state_ptr = state.get();

            state->watch = std::make_unique<fd_watch>(
                r, fd, event_type::readable, [this, state, state_ptr, &r](event_type) {
                    handle_connection(*state_ptr, r);
                });
        }
    };

    if (reuseport_) {
        auto res = pool.start_listening(port_, accept_handler);
        if (!res) {
            std::cerr << "Failed to start listeners on port " << port_ << ": "
                      << res.error().message() << "\n";
            return 1;
        }
    } else {
        // Fallback: single listener on reactor 0
        tcp_listener listener(port_);
        if (!listener) {
            std::cerr << "Failed to create listener on port " << port_ << "\n";
            return 1;
        }
        listener.set_reuseport(false).set_backlog(backlog_);

        auto& r = pool.get_reactor(0);
        auto listen_fd = listener.native_handle();
        auto listen_watch = std::make_shared<fd_watch>(
            r, listen_fd, event_type::readable, [&r, &listener, accept_handler](event_type) {
                accept_handler(r, listener.native_handle());
            });
        accept_watches.push_back(std::move(listen_watch));
    }

    // Setup signal handlers for graceful shutdown
    shutdown_manager::instance().setup_signal_handlers();
    shutdown_manager::instance().set_shutdown_callback([&pool, this]() {
        if (on_stop_callback_) {
            on_stop_callback_();
        }
        pool.graceful_stop(shutdown_timeout_);
    });

    // Call on_start callback
    if (on_start_callback_) {
        on_start_callback_();
    } else {
        std::cout << "HTTP server listening on http://" << host_ << ":" << port_ << "\n";
        std::cout << "Workers: " << worker_count_ << "\n";
        std::cout << "Press Ctrl+C to stop\n\n";
    }

    pool.start();
    pool.wait();
    return 0;
}

} // namespace http
} // namespace katana
