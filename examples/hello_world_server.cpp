#include "katana/core/reactor_pool.hpp"
#include "katana/core/http.hpp"
#include "katana/core/http_headers.hpp"
#include "katana/core/system_limits.hpp"
#include "katana/core/shutdown.hpp"
#include "katana/core/arena.hpp"
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
constexpr size_t BUFFER_SIZE = 4096;
constexpr size_t ARENA_BLOCK_SIZE = 8192;
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

int32_t create_listener() {
    int32_t sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (sockfd < 0) {
        return -1;
    }

    int32_t opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(sockfd, static_cast<sockaddr*>(static_cast<void*>(&addr)), sizeof(addr)) < 0) {
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, 1024) < 0) {
        close(sockfd);
        return -1;
    }

    return sockfd;
}

struct connection {
    std::atomic<int32_t> fd{-1};
    monotonic_arena arena;
    http::parser parser;
    std::pmr::vector<uint8_t> read_buffer;
    std::pmr::vector<uint8_t> write_buffer;
    size_t write_pos = 0;
    size_t requests_on_connection = 0;
    bool writing_response = false;
    bool should_close_after_write = false;

    connection()
        : arena(ARENA_BLOCK_SIZE)
        , read_buffer(&arena)
        , write_buffer(&arena)
    {
        read_buffer.resize(BUFFER_SIZE);
    }

    void safe_close() {
        int32_t expected_fd = fd.exchange(-1, std::memory_order_acq_rel);
        if (expected_fd >= 0) {
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
        while (conn.write_pos < conn.write_buffer.size()) {
            scatter_gather_write sg_write;
            sg_write.add_buffer(std::span<const uint8_t>(
                conn.write_buffer.data() + conn.write_pos,
                conn.write_buffer.size() - conn.write_pos
            ));

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
            conn.write_pos += written;
        }

        conn.writing_response = false;
        if (conn.should_close_after_write) {
            conn.safe_close();
            return;
        }

        conn.parser = http::parser();
        conn.arena.reset();
        conn.write_pos = 0;
    }

    while (true) {
        scatter_gather_read sg_read;
        sg_read.add_buffer(std::span<uint8_t>(conn.read_buffer.data(), conn.read_buffer.size()));

        auto read_result = read_vectored(fd_val, sg_read);
        if (!read_result) {
            if (read_result.error().value() != EAGAIN && read_result.error().value() != EWOULDBLOCK) {
                conn.safe_close();
            }
            return;
        }

        size_t n = *read_result;
        if (n == 0) {
            conn.safe_close();
            return;
        }

        auto result = conn.parser.parse(std::span<const uint8_t>(conn.read_buffer.data(), n));

        if (!result) {
            auto resp = http::response::error(problem_details::bad_request("Invalid HTTP request"));
            std::string serialized = resp.serialize();

            scatter_gather_write sg_write;
            sg_write.add_buffer(std::span<const uint8_t>(
                reinterpret_cast<const uint8_t*>(serialized.data()),
                serialized.size()
            ));

            write_vectored(fd_val, sg_write);
            conn.safe_close();
            return;
        }

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
        if (connection_header && (*connection_header == "close" ||
            ci_equal(*connection_header, "close"))) {
            should_close = true;
        } else if (req.version == "HTTP/1.0") {
            should_close = !connection_header ||
                          !ci_equal(*connection_header, "keep-alive");
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

        conn.write_buffer.assign(
            reinterpret_cast<const uint8_t*>(response),
            reinterpret_cast<const uint8_t*>(response) + response_len
        );
        conn.write_pos = 0;

        while (conn.write_pos < conn.write_buffer.size()) {
            scatter_gather_write sg_write;
            sg_write.add_buffer(std::span<const uint8_t>(
                conn.write_buffer.data() + conn.write_pos,
                conn.write_buffer.size() - conn.write_pos
            ));

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
            if (written == 0) {
                conn.writing_response = true;
                conn.should_close_after_write = should_close;
                return;
            }

            conn.write_pos += written;
        }

        if (should_close) {
            conn.safe_close();
            return;
        }

        conn.parser = http::parser();
        conn.arena.reset();
        conn.write_pos = 0;
    }
}

void accept_connections(reactor_pool& pool, int32_t listener_fd) {
    size_t accepts_this_call = 0;

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

        auto conn = std::make_shared<connection>();
        conn->fd.store(client_fd, std::memory_order_relaxed);

        size_t reactor_idx = pool.select_reactor();
        auto& r = pool.get_reactor(reactor_idx);

        timeout_config timeouts{
            std::chrono::milliseconds(30000),
            std::chrono::milliseconds(30000),
            std::chrono::milliseconds(60000)
        };

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
    }
}

int32_t main() {
    auto limits_result = system_limits::set_max_fds(65536);
    if (!limits_result) {
        std::cerr << "Failed to set max FDs: " << limits_result.error().message() << "\n";
    }

    int32_t listener_fd = create_listener();
    if (listener_fd < 0) {
        std::cerr << "Failed to create listener socket\n";
        return 1;
    }

    std::cout << "Starting hello-world server on port " << PORT << "\n";

    reactor_pool pool;
    pool.start();

    size_t main_reactor_idx = pool.select_reactor();
    auto& main_reactor = pool.get_reactor(main_reactor_idx);

    auto result = main_reactor.register_fd(
        listener_fd,
        event_type::readable | event_type::edge_triggered,
        [&pool, listener_fd](event_type events) {
            if (has_flag(events, event_type::readable)) {
                accept_connections(pool, listener_fd);
            }
        }
    );

    if (!result) {
        std::cerr << "Failed to register listener: " << result.error().message() << "\n";
        close(listener_fd);
        return 1;
    }

    shutdown_manager::instance().setup_signal_handlers();
    shutdown_manager::instance().set_shutdown_callback([&pool]() {
        pool.graceful_stop(std::chrono::milliseconds(30000));
    });

    std::cout << "Server running. Press Ctrl+C to stop.\n";

    pool.wait();
    close(listener_fd);

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
