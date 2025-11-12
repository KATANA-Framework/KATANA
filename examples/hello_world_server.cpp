#include "katana/core/reactor_pool.hpp"
#include "katana/core/http.hpp"
#include "katana/core/http_headers.hpp"
#include "katana/core/system_limits.hpp"
#include "katana/core/shutdown.hpp"
#include "katana/core/arena_pool.hpp"
#include "katana/core/io_buffer.hpp"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <chrono>

using namespace katana;
using katana::http::ci_equal;

constexpr uint16_t PORT = 8080;
constexpr size_t MAX_CONNECTIONS = 10000;
constexpr size_t MAX_ACCEPTS_PER_TICK = 10000;

static std::atomic<size_t> active_connections{0};
static std::atomic<size_t> total_requests{0};
static std::atomic<size_t> keepalive_reuses{0};

struct rate_limiter {
    std::chrono::steady_clock::time_point last_reset;
    size_t accepts_this_period{0};
    static constexpr auto PERIOD = std::chrono::milliseconds(100);

    bool should_accept() {
        auto now = std::chrono::steady_clock::now();
        if (now - last_reset >= PERIOD) {
            last_reset = now;
            accepts_this_period = 0;
        }

        if (accepts_this_period >= MAX_ACCEPTS_PER_TICK) {
            return false;
        }

        ++accepts_this_period;
        return true;
    }
};

static rate_limiter accept_limiter;

struct connection {
    std::atomic<int32_t> fd{-1};

    io_buffer read_buffer;
    io_buffer write_buffer;

    http::parser parser;
    bounded_arena_pool::scoped_arena arena_guard;
    monotonic_arena* request_arena = nullptr;

    size_t requests_on_connection = 0;
    bool has_active_request = false;
    bool writing_response = false;
    bool should_close_after_write = false;
    bool corked = false;

    epoll_reactor* reactor = nullptr;

    connection(epoll_reactor* r)
        : parser(nullptr)
        , reactor(r)
    {
        read_buffer.reserve(4096);
    }

    void start_response() {
#ifdef __linux__
        if (!corked) {
            int flag = 1;
            int32_t fd_val = fd.load(std::memory_order_relaxed);
            if (fd_val >= 0) {
                ::setsockopt(fd_val, IPPROTO_TCP, TCP_CORK, &flag, sizeof(flag));
                corked = true;
            }
        }
#endif
    }

    void finish_response() {
#ifdef __linux__
        if (corked) {
            int flag = 0;
            int32_t fd_val = fd.load(std::memory_order_relaxed);
            if (fd_val >= 0) {
                ::setsockopt(fd_val, IPPROTO_TCP, TCP_CORK, &flag, sizeof(flag));
                corked = false;
            }
        }
#endif
    }

    void safe_close() {
        int32_t expected_fd = fd.exchange(-1, std::memory_order_acq_rel);
        if (expected_fd >= 0) {
            if (reactor) {
                reactor->unregister_fd(expected_fd);
            }
            close(expected_fd);
            active_connections.fetch_sub(1, std::memory_order_relaxed);
        }
    }
};

void handle_client(connection& conn) {
    int32_t fd_val = conn.fd.load(std::memory_order_relaxed);
    if (fd_val < 0) {
        return;
    }

    if (conn.writing_response) {
        auto write_span = conn.write_buffer.readable_span();
        if (!write_span.empty()) {
            scatter_gather_write sg_write;
            sg_write.add_buffer(write_span);

            auto write_result = write_vectored(fd_val, sg_write);
            if (!write_result) {
                if (write_result.error().value() == EAGAIN ||
                    write_result.error().value() == EWOULDBLOCK) {
                    return;
                }
                conn.safe_close();
                return;
            }

            size_t written = *write_result;
            if (written == 0) {
                return;
            }
            conn.write_buffer.consume(written);
        }

        if (conn.write_buffer.empty()) {
            conn.finish_response();
            conn.writing_response = false;

            if (conn.should_close_after_write) {
                conn.safe_close();
                return;
            }

            conn.has_active_request = false;
            conn.arena_guard = bounded_arena_pool::scoped_arena();
            conn.request_arena = nullptr;
        }
        return;
    }

    while (true) {
        auto writable = conn.read_buffer.writable_span(4096);
        scatter_gather_read sg_read;
        sg_read.add_buffer(writable);

        auto read_result = read_vectored(fd_val, sg_read);
        if (!read_result) {
            if (read_result.error().value() != EAGAIN &&
                read_result.error().value() != EWOULDBLOCK) {
                conn.safe_close();
            }
            return;
        }

        size_t n = *read_result;
        if (n == 0) {
            conn.safe_close();
            return;
        }

        conn.read_buffer.commit(n);

        if (!conn.has_active_request) {
            conn.arena_guard = conn.reactor->arena_pool().acquire_scoped();
            if (!conn.arena_guard.get()) {
                static const char response_503[] =
                    "HTTP/1.1 503 Service Unavailable\r\n"
                    "Content-Type: text/plain\r\n"
                    "Content-Length: 21\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "Service Unavailable\n";

                conn.write_buffer.append(std::string_view(response_503, sizeof(response_503) - 1));
                conn.writing_response = true;
                conn.should_close_after_write = true;
                return;
            }

            conn.request_arena = conn.arena_guard.get();
            conn.parser.reset(conn.request_arena);
            conn.has_active_request = true;
        }

        auto parse_span = conn.read_buffer.readable_span();
        auto parse_result = conn.parser.parse(parse_span);

        if (!parse_result) {
            static const char response_400[] =
                "HTTP/1.1 400 Bad Request\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 12\r\n"
                "Connection: close\r\n"
                "\r\n"
                "Bad Request\n";

            conn.write_buffer.append(std::string_view(response_400, sizeof(response_400) - 1));
            conn.writing_response = true;
            conn.should_close_after_write = true;
            conn.safe_close();
            return;
        }

        conn.read_buffer.consume(conn.parser.bytes_consumed());

        if (!conn.parser.is_complete()) {
            continue;
        }

        ++conn.requests_on_connection;
        if (conn.requests_on_connection > 1) {
            keepalive_reuses.fetch_add(1, std::memory_order_relaxed);
        }
        total_requests.fetch_add(1, std::memory_order_relaxed);

        auto& req = conn.parser.get_request();

        bool should_close = false;
        auto connection_header = req.header("Connection");
        if (connection_header && ci_equal(*connection_header, "close")) {
            should_close = true;
        }

        if (conn.requests_on_connection >= 1000) {
            should_close = true;
        }

        static const char* response_keepalive =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "Connection: keep-alive\r\n"
            "Keep-Alive: timeout=60, max=1000\r\n"
            "\r\n"
            "Hello, World!";

        static const char* response_close =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Hello, World!";

        const char* response = should_close ? response_close : response_keepalive;
        size_t response_len = should_close ? 103 : 136;

        conn.write_buffer.clear();
        conn.write_buffer.append(std::string_view(response, response_len));

        conn.has_active_request = false;
        conn.arena_guard = bounded_arena_pool::scoped_arena();
        conn.request_arena = nullptr;

        conn.start_response();

        auto write_span = conn.write_buffer.readable_span();
        scatter_gather_write sg_write;
        sg_write.add_buffer(write_span);

        auto write_result = write_vectored(fd_val, sg_write);
        if (!write_result) {
            if (write_result.error().value() == EAGAIN ||
                write_result.error().value() == EWOULDBLOCK) {
                conn.writing_response = true;
                conn.should_close_after_write = should_close;
                return;
            }
            conn.safe_close();
            return;
        }

        size_t written = *write_result;
        if (written > 0) {
            conn.write_buffer.consume(written);
        }

        if (conn.write_buffer.empty()) {
            conn.finish_response();

            if (should_close) {
                conn.safe_close();
                return;
            }
        } else {
            conn.writing_response = true;
            conn.should_close_after_write = should_close;
            return;
        }
    }
}

void accept_connections(reactor_pool& pool, size_t reactor_idx, int32_t listener_fd) {
    size_t accepts_this_call = 0;
    auto& r = pool.get_reactor(reactor_idx);

    while (accepts_this_call < MAX_ACCEPTS_PER_TICK) {
        if (active_connections.load(std::memory_order_relaxed) >= MAX_CONNECTIONS) {
            return;
        }

        if (!accept_limiter.should_accept()) {
            return;
        }

        sockaddr_in client_addr{};
        socklen_t addr_len = sizeof(client_addr);

        int32_t client_fd = accept4(listener_fd,
                               static_cast<sockaddr*>(static_cast<void*>(&client_addr)),
                               &addr_len,
                               SOCK_NONBLOCK | SOCK_CLOEXEC);

        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return;
            }
            continue;
        }

        ++accepts_this_call;

        int32_t nodelay = 1;
        setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));

        active_connections.fetch_add(1, std::memory_order_relaxed);

        auto conn = std::make_shared<connection>(&r);
        conn->fd.store(client_fd, std::memory_order_relaxed);

        timeout_config timeouts{
            std::chrono::milliseconds(30000),
            std::chrono::milliseconds(30000),
            std::chrono::milliseconds(60000)
        };

        r.schedule([conn, &r, client_fd, timeouts]() {
            auto result = r.register_fd_with_timeout(
                client_fd,
                event_type::readable | event_type::edge_triggered,
                [conn, &r](event_type events) {
                    if (has_flag(events, event_type::readable)) {
                        handle_client(*conn);
                        int32_t refresh_fd = conn->fd.load(std::memory_order_relaxed);
                        if (refresh_fd >= 0) {
                            r.refresh_fd_timeout(refresh_fd);
                        }
                    }
                },
                timeouts
            );

            if (!result) {
                close(client_fd);
                active_connections.fetch_sub(1, std::memory_order_relaxed);
            }
        });
    }
}

int32_t main() {
    auto limits_result = system_limits::set_max_fds(65536);
    if (!limits_result) {
        std::cerr << "Failed to set max FDs: " << limits_result.error().message() << "\n";
    }

    std::cout << "Starting hello-world server on port " << PORT << "\n";

    reactor_pool pool;

    auto result = pool.start_listening(PORT, [&pool](int32_t listener_fd) {
        size_t reactor_idx = 0;
        for (size_t i = 0; i < pool.reactor_count(); ++i) {
            if (&pool.get_reactor(i) == &pool.get_reactor(reactor_idx)) {
                reactor_idx = i;
                break;
            }
        }

        for (auto& reactor : pool) {
            if (&reactor == &pool.get_reactor(reactor_idx)) {
                break;
            }
            ++reactor_idx;
        }

        accept_connections(pool, reactor_idx, listener_fd);
    });

    if (!result) {
        std::cerr << "Failed to start listening: " << result.error().message() << "\n";
        return 1;
    }

    pool.start();

    shutdown_manager::instance().setup_signal_handlers();
    shutdown_manager::instance().set_shutdown_callback([&pool]() {
        pool.graceful_stop(std::chrono::milliseconds(30000));
    });

    std::cout << "Server running with SO_REUSEPORT per-reactor. Press Ctrl+C to stop.\n";

    pool.wait();

    std::cout << "\nServer stopped\n";

    auto total_metrics = pool.aggregate_metrics();
    std::cout << "Total metrics:\n";
    std::cout << "  Tasks executed: " << total_metrics.tasks_executed << "\n";
    std::cout << "  FD events: " << total_metrics.fd_events_processed << "\n";
    std::cout << "  Timers fired: " << total_metrics.timers_fired << "\n";
    std::cout << "  Exceptions: " << total_metrics.exceptions_caught << "\n";
    std::cout << "  Total requests: " << total_requests.load() << "\n";
    std::cout << "  Keep-Alive reuses: " << keepalive_reuses.load() << "\n";

    return 0;
}
