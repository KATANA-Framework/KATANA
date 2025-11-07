#include "katana/core/reactor_pool.hpp"
#include "katana/core/http.hpp"
#include "katana/core/http_headers.hpp"
#include "katana/core/system_limits.hpp"
#include "katana/core/shutdown.hpp"
#include "katana/core/arena.hpp"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

using namespace katana;
using katana::http::ci_equal;

constexpr uint16_t PORT = 8080;
constexpr size_t BUFFER_SIZE = 4096;
constexpr size_t ARENA_BLOCK_SIZE = 8192;
constexpr size_t MAX_CONNECTIONS = 10000;

static std::atomic<size_t> active_connections{0};

int create_listener() {
    int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (sockfd < 0) {
        return -1;
    }

    int opt = 1;
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
    std::atomic<int> fd{-1};
    monotonic_arena arena;
    http::parser parser;
    std::pmr::vector<uint8_t> read_buffer;
    std::pmr::vector<uint8_t> write_buffer;
    size_t write_pos = 0;

    connection()
        : arena(ARENA_BLOCK_SIZE)
        , read_buffer(&arena)
        , write_buffer(&arena)
    {
        read_buffer.resize(BUFFER_SIZE);
    }

    void safe_close() {
        int expected_fd = fd.exchange(-1, std::memory_order_acq_rel);
        if (expected_fd >= 0) {
            close(expected_fd);
            active_connections.fetch_sub(1, std::memory_order_relaxed);
        }
    }
};

void handle_client(connection& conn) {
    while (true) {
        int fd_val = conn.fd.load(std::memory_order_relaxed);
        if (fd_val < 0) {
            return;
        }
        ssize_t n = recv(fd_val, conn.read_buffer.data(), conn.read_buffer.size(), 0);

        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return;
            }
            conn.safe_close();
            return;
        }

        if (n == 0) {
            conn.safe_close();
            return;
        }

        auto result = conn.parser.parse(std::span<const uint8_t>(conn.read_buffer.data(), static_cast<size_t>(n)));

        if (!result) {
            auto resp = http::response::error(problem_details::bad_request("Invalid HTTP request"));
            std::string serialized = resp.serialize();
            int write_fd = conn.fd.load(std::memory_order_relaxed);
            if (write_fd >= 0) {
                size_t total_written = 0;
                while (total_written < serialized.size()) {
                    ssize_t written = write(write_fd, serialized.data() + total_written,
                                          serialized.size() - total_written);
                    if (written <= 0) break;
                    total_written += static_cast<size_t>(written);
                }
            }
            conn.safe_close();
            return;
        }

        if (conn.parser.is_complete()) {
            auto& req = conn.parser.get_request();
            auto resp = http::response::ok("Hello, World!", "text/plain");

            bool should_close = false;
            auto connection_header = req.header("Connection");
            if (connection_header && (*connection_header == "close" ||
                ci_equal(*connection_header, "close"))) {
                should_close = true;
            } else if (req.version == "HTTP/1.0") {
                should_close = !connection_header ||
                              !ci_equal(*connection_header, "keep-alive");
            }

            if (should_close) {
                resp.set_header("Connection", "close");
            } else {
                resp.set_header("Connection", "keep-alive");
            }

            std::string serialized = resp.serialize();
            conn.write_buffer.assign(serialized.begin(), serialized.end());
            conn.write_pos = 0;

            while (conn.write_pos < conn.write_buffer.size()) {
                int send_fd = conn.fd.load(std::memory_order_relaxed);
                if (send_fd < 0) {
                    return;
                }
                ssize_t written = send(send_fd,
                                     conn.write_buffer.data() + conn.write_pos,
                                     conn.write_buffer.size() - conn.write_pos,
                                     0);

                if (written < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        break;
                    }
                    conn.safe_close();
                    return;
                }

                conn.write_pos += static_cast<size_t>(written);
            }

            if (conn.write_pos == conn.write_buffer.size()) {
                if (should_close) {
                    conn.safe_close();
                    return;
                }

                conn.parser = http::parser();
                conn.arena.reset();
                conn.write_pos = 0;
            }
        }
    }
}

void accept_connections(reactor_pool& pool, int listener_fd) {
    while (true) {
        if (active_connections.load(std::memory_order_relaxed) >= MAX_CONNECTIONS) {
            return;
        }

        sockaddr_in client_addr{};
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept4(listener_fd,
                               static_cast<sockaddr*>(static_cast<void*>(&client_addr)),
                               &addr_len,
                               SOCK_NONBLOCK | SOCK_CLOEXEC);

        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return;
            }
            continue;
        }

        int flag = 1;
        setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

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
                    int refresh_fd = conn->fd.load(std::memory_order_relaxed);
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

int main() {
    auto limits_result = system_limits::set_max_fds(65536);
    if (!limits_result) {
        std::cerr << "Failed to set max FDs: " << limits_result.error().message() << "\n";
    }

    int listener_fd = create_listener();
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

    return 0;
}
