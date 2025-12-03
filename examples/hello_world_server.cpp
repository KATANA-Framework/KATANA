#include "katana/core/arena.hpp"
#include "katana/core/http.hpp"
#include "katana/core/http_headers.hpp"
#include "katana/core/io_buffer.hpp"
#include "katana/core/reactor_pool.hpp"
#include "katana/core/router.hpp"
#include "katana/core/shutdown.hpp"
#include "katana/core/system_limits.hpp"

#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string_view>
#include <sys/socket.h>
#include <unistd.h>

using namespace katana;
using katana::http::ci_equal;

constexpr uint16_t DEFAULT_PORT = 18080;
constexpr size_t BUFFER_SIZE = 16384;
constexpr size_t ARENA_BLOCK_SIZE = 8192;
constexpr size_t MAX_CONNECTIONS = 10000;

static std::atomic<size_t> active_connections{0};
static std::atomic<size_t> total_requests{0};
static std::atomic<size_t> keepalive_reuses{0};

uint16_t server_port() {
    if (const char* env = std::getenv("HELLO_PORT")) {
        int v = std::atoi(env);
        if (v > 0 && v <= 65535) {
            return static_cast<uint16_t>(v);
        }
    }
    return DEFAULT_PORT;
}

int32_t create_listener(uint16_t port) {
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
    addr.sin_port = htons(port);

    if (bind(sockfd, static_cast<sockaddr*>(static_cast<void*>(&addr)), sizeof(addr)) < 0) {
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, 8192) < 0) {
        close(sockfd);
        return -1;
    }

    return sockfd;
}

static constexpr std::string_view RESPONSE_KEEPALIVE = "HTTP/1.1 200 OK\r\n"
                                                       "Content-Type: text/plain\r\n"
                                                       "Content-Length: 13\r\n"
                                                       "Connection: keep-alive\r\n"
                                                       "Keep-Alive: timeout=60, max=1000\r\n"
                                                       "\r\n"
                                                       "Hello, World!";

static constexpr std::string_view RESPONSE_CLOSE = "HTTP/1.1 200 OK\r\n"
                                                   "Content-Type: text/plain\r\n"
                                                   "Content-Length: 13\r\n"
                                                   "Connection: close\r\n"
                                                   "\r\n"
                                                   "Hello, World!";

void build_response_header(std::string& out,
                           size_t body_len,
                           bool close_after,
                           bool include_keepalive) {
    constexpr std::string_view status = "HTTP/1.1 200 OK\r\n";
    constexpr std::string_view content_type = "Content-Type: text/plain\r\n";
    constexpr std::string_view conn_keep = "Connection: keep-alive\r\n";
    constexpr std::string_view conn_close = "Connection: close\r\n";
    constexpr std::string_view keepalive_hdr = "Keep-Alive: timeout=60, max=1000\r\n";

    char len_buf[16];
    auto [ptr, ec] = std::to_chars(len_buf, len_buf + sizeof(len_buf), body_len);
    const size_t len_len = static_cast<size_t>(ptr - len_buf);

    out.clear();
    out.reserve(status.size() + content_type.size() + len_len + 64 + body_len);

    out.append(status);
    out.append("Content-Length: ");
    out.append(len_buf, len_len);
    out.append("\r\n");
    out.append(content_type);
    out.append(close_after ? conn_close : conn_keep);
    if (!close_after && include_keepalive) {
        out.append(keepalive_hdr);
    }
    out.append("\r\n");
}

void build_simple_response(std::string& out,
                           std::string_view body,
                           bool close_after,
                           bool include_keepalive) {
    build_response_header(out, body.size(), close_after, include_keepalive);
    out.append(body);
}

struct connection {
    std::atomic<int32_t> fd{-1};
    monotonic_arena arena;
    http::parser parser;
    std::vector<uint8_t> read_buffer;
    std::string active_response;
    size_t write_pos = 0;
    size_t requests_on_connection = 0;
    bool writing_response = false;
    bool should_close_after_write = false;
    reactor* reactor_ptr = nullptr;

    connection() : arena(ARENA_BLOCK_SIZE), parser(&arena) {
        read_buffer.resize(BUFFER_SIZE);
        active_response.reserve(512);
    }

    void safe_close() {
        int32_t expected_fd = fd.exchange(-1, std::memory_order_acq_rel);
        if (expected_fd >= 0) {
            if (reactor_ptr) {
                reactor_ptr->unregister_fd(expected_fd);
            }
            close(expected_fd);
            active_connections.fetch_sub(1, std::memory_order_relaxed);
        }
    }
};

void write_active_response(connection& conn) {
    int32_t fd_val = conn.fd.load(std::memory_order_relaxed);
    if (fd_val < 0) {
        return;
    }

    while (conn.write_pos < conn.active_response.size()) {
        scatter_gather_write sg_write;
        sg_write.add_buffer(std::span<const uint8_t>(
            reinterpret_cast<const uint8_t*>(conn.active_response.data()) + conn.write_pos,
            conn.active_response.size() - conn.write_pos));

        auto write_result = write_vectored(fd_val, sg_write);
        if (!write_result) {
            if (write_result.error().value() == EAGAIN ||
                write_result.error().value() == EWOULDBLOCK) {
                conn.writing_response = true;
                return;
            }
            conn.safe_close();
            return;
        }

        size_t written = *write_result;
        if (written == 0) {
            conn.writing_response = true;
            return;
        }

        conn.write_pos += written;
    }

    conn.writing_response = false;
}

const http::router& hello_router() {
    // Simple router: GET /hello/{name?}
    static const http::route_entry routes[] = {
        {http::method::get,
         http::path_pattern::from_literal<"/">(),
         http::handler_fn([](const http::request&, http::request_context&) {
             return http::response::ok("Hello, World!");
         })},
        {http::method::get,
         http::path_pattern::from_literal<"/hello/{name}">(),
         http::handler_fn([](const http::request&, http::request_context& ctx) {
             auto name = ctx.params.get("name").value_or("world");
             std::string body;
             body.reserve(6 + name.size());
             body.append("Hello ");
             body.append(name);
             body.push_back('!');
             return http::response::ok(std::move(body));
         })},
    };
    static const http::router r(routes);
    return r;
}

http::response dispatch_request(const http::request& req, monotonic_arena& arena) {
    http::request_context ctx{arena};
    return http::dispatch_or_problem(hello_router(), req, ctx);
}

void handle_client(connection& conn) {
    int32_t fd_val = conn.fd.load(std::memory_order_relaxed);
    if (fd_val < 0) {
        return;
    }

    if (conn.writing_response) {
        write_active_response(conn);
        if (conn.writing_response) {
            return;
        }

        if (conn.should_close_after_write) {
            conn.safe_close();
            return;
        }

        conn.arena.reset();
        conn.parser = http::parser(&conn.arena);
        conn.write_pos = 0;
        conn.active_response = {};
        conn.should_close_after_write = false;
    }

    while (true) {
        scatter_gather_read sg_read;
        sg_read.add_buffer(std::span<uint8_t>(conn.read_buffer.data(), conn.read_buffer.size()));

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

        auto result = conn.parser.parse(std::span<const uint8_t>(conn.read_buffer.data(), n));

        if (!result) {
            auto resp = http::response::error(problem_details::bad_request("Invalid HTTP request"));
            std::string serialized = resp.serialize();

            scatter_gather_write sg_write;
            sg_write.add_buffer(std::span<const uint8_t>(
                reinterpret_cast<const uint8_t*>(serialized.data()), serialized.size()));

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
        if (connection_header && ci_equal(*connection_header, "close")) {
            should_close = true;
        }

        if (conn.requests_on_connection >= 1000) {
            should_close = true;
        }

        const bool has_conn_close = connection_header && ci_equal(*connection_header, "close");
        const bool final_close = should_close || has_conn_close;

        bool handled_fast_path = false;
        if (req.http_method == http::method::get) {
            if (req.uri == "/") {
                conn.active_response.clear();
                if (has_conn_close || final_close) {
                    conn.active_response.append(RESPONSE_CLOSE.data(), RESPONSE_CLOSE.size());
                } else {
                    conn.active_response.append(RESPONSE_KEEPALIVE.data(),
                                                RESPONSE_KEEPALIVE.size());
                }
                handled_fast_path = true;
            } else if (req.uri.size() > 7 && req.uri.rfind("/hello/", 0) == 0) {
                std::string_view name = req.uri.substr(7);
                if (!name.empty() && name.size() < 256) {
                    constexpr std::string_view prefix = "Hello ";
                    constexpr std::string_view suffix = "!";
                    const size_t body_len = prefix.size() + name.size() + suffix.size();
                    build_response_header(conn.active_response, body_len, final_close, true);
                    conn.active_response.append(prefix);
                    conn.active_response.append(name);
                    conn.active_response.append(suffix);
                    handled_fast_path = true;
                }
            }
        }

        if (!handled_fast_path) {
            http::response resp = dispatch_request(req, conn.arena);
            resp.set_header("Connection", has_conn_close ? "close" : "keep-alive");
            conn.active_response.clear();
            resp.serialize_into(conn.active_response);
        } else {
            conn.active_response.reserve(conn.active_response.size());
        }

        conn.write_pos = 0;
        conn.should_close_after_write = final_close || has_conn_close;

        write_active_response(conn);

        if (conn.writing_response) {
            return;
        }

        if (final_close) {
            conn.safe_close();
            return;
        }

        conn.arena.reset();
        conn.parser.reset(&conn.arena);
        conn.write_pos = 0;
        conn.active_response.clear();
    }
}

void accept_connections(reactor_pool& pool, int32_t listener_fd) {
    while (true) {
        if (active_connections.load(std::memory_order_relaxed) >= MAX_CONNECTIONS) {
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

        int32_t nodelay = 1;
        setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));

        active_connections.fetch_add(1, std::memory_order_relaxed);

        auto conn = std::make_shared<connection>();
        conn->fd.store(client_fd, std::memory_order_relaxed);

        size_t reactor_idx = pool.select_reactor();
        auto& r = pool.get_reactor(reactor_idx);
        conn->reactor_ptr = &r;

        timeout_config timeouts{std::chrono::milliseconds(30000),
                                std::chrono::milliseconds(30000),
                                std::chrono::milliseconds(60000)};

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
                timeouts);

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

    uint16_t port = server_port();
    int32_t listener_fd = create_listener(port);
    if (listener_fd < 0) {
        std::cerr << "Failed to create listener socket (errno=" << errno
                  << "): " << std::strerror(errno) << "\n";
        return 1;
    }

    std::cout << "Starting hello-world server on port " << port << "\n";

    reactor_pool pool;

    size_t main_reactor_idx = pool.select_reactor();
    auto& main_reactor = pool.get_reactor(main_reactor_idx);

    auto result = main_reactor.register_fd(listener_fd,
                                           event_type::readable | event_type::edge_triggered,
                                           [&pool, listener_fd](event_type events) {
                                               if (has_flag(events, event_type::readable)) {
                                                   accept_connections(pool, listener_fd);
                                               }
                                           });

    if (!result) {
        std::cerr << "Failed to register listener: " << result.error().message() << "\n";
        close(listener_fd);
        return 1;
    }

    pool.start();

    shutdown_manager::instance().setup_signal_handlers();
    shutdown_manager::instance().set_shutdown_callback(
        [&pool]() { pool.graceful_stop(std::chrono::milliseconds(30000)); });

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
