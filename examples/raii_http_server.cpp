#include "katana/core/fd_watch.hpp"
#include "katana/core/io_buffer.hpp"
#include "katana/core/reactor_pool.hpp"
#include "katana/core/tcp_listener.hpp"
#include "katana/core/tcp_socket.hpp"

#include <expected>
#include <iostream>
#include <memory>
#include <string_view>
#include <vector>

using namespace katana;

class http_connection {
public:
    explicit http_connection(tcp_socket sock)
        : socket_(std::move(sock)), read_buffer_(8192), write_buffer_(8192) {}

    result<std::string_view> read_request() {
        auto buf = read_buffer_.writable_span(4096);
        return socket_.read(buf).and_then([this](auto span) -> result<std::string_view> {
            if (span.empty()) {
                return std::string_view{};
            }
            read_buffer_.commit(span.size());
            auto readable = read_buffer_.readable_span();
            return std::string_view{reinterpret_cast<const char*>(readable.data()),
                                    readable.size()};
        });
    }

    result<void> write_response(std::string_view response) {
        write_buffer_.append(response);
        return flush();
    }

    result<void> flush() {
        while (!write_buffer_.empty()) {
            auto data = write_buffer_.readable_span();
            auto res = socket_.write(data);

            if (!res) {
                return std::unexpected(res.error());
            }

            if (res.value() == 0) {
                break;
            }

            write_buffer_.consume(res.value());
        }
        return {};
    }

    void consume_request(size_t bytes) { read_buffer_.consume(bytes); }

    [[nodiscard]] int32_t native_handle() const noexcept { return socket_.native_handle(); }

    [[nodiscard]] bool has_pending_write() const noexcept { return !write_buffer_.empty(); }

private:
    tcp_socket socket_;
    io_buffer read_buffer_;
    io_buffer write_buffer_;
};

struct connection_state {
    http_connection conn;
    std::unique_ptr<fd_watch> watch;

    explicit connection_state(tcp_socket sock) : conn(std::move(sock)) {}
};

result<std::string_view> process_request(std::string_view request) {
    constexpr std::string_view ok_response = "HTTP/1.1 200 OK\r\n"
                                             "Content-Type: text/plain\r\n"
                                             "Content-Length: 14\r\n"
                                             "Connection: close\r\n"
                                             "\r\n"
                                             "Hello, World!\n";

    if (request.find("GET") != std::string_view::npos) {
        return ok_response;
    }

    constexpr std::string_view bad_request = "HTTP/1.1 400 Bad Request\r\n"
                                             "Content-Length: 0\r\n"
                                             "Connection: close\r\n"
                                             "\r\n";

    return bad_request;
}

void handle_connection(connection_state& state, [[maybe_unused]] reactor& r) {
    state.conn.read_request()
        .and_then([](std::string_view req) { return process_request(req); })
        .and_then([&state](std::string_view resp) { return state.conn.write_response(resp); })
        .or_else([](std::error_code err) -> result<void> {
            if (err != make_error_code(error_code::ok)) {
                std::cerr << "Connection error: " << err.message() << "\n";
            }
            return {};
        });

    if (!state.conn.has_pending_write()) {
        state.watch.reset();
    }
}

void accept_connection(reactor& r,
                       tcp_listener& listener,
                       std::vector<std::unique_ptr<connection_state>>& connections) {
    listener.accept()
        .and_then([&](tcp_socket sock) -> result<void> {
            auto state = std::make_unique<connection_state>(std::move(sock));
            int32_t fd = state->conn.native_handle();

            auto* state_ptr = state.get();
            state->watch = std::make_unique<fd_watch>(
                r, fd, event_type::readable, [state_ptr, &r](event_type) {
                    handle_connection(*state_ptr, r);
                });

            connections.push_back(std::move(state));
            return {};
        })
        .or_else([](std::error_code err) -> result<void> {
            if (err != make_error_code(error_code::ok)) {
                std::cerr << "Accept error: " << err.message() << "\n";
            }
            return {};
        });
}

int main(int argc, char* argv[]) {
    uint16_t port = 8080;
    if (argc > 1) {
        port = static_cast<uint16_t>(std::stoul(argv[1]));
    }

    tcp_listener listener(port);
    if (!listener) {
        std::cerr << "Failed to create listener on port " << port << "\n";
        return 1;
    }

    listener.set_reuseport(true).set_backlog(1024);

    reactor_pool_config config;
    config.reactor_count = std::thread::hardware_concurrency();
    config.enable_thread_pinning = false;
    reactor_pool pool(config);

    std::vector<std::unique_ptr<connection_state>> connections;
    std::vector<std::unique_ptr<fd_watch>> accept_watches;

    for (auto& reactor : pool) {
        auto watch =
            std::make_unique<fd_watch>(reactor,
                                       listener.native_handle(),
                                       event_type::readable,
                                       [&reactor, &listener, &connections](event_type) {
                                           accept_connection(reactor, listener, connections);
                                       });
        accept_watches.push_back(std::move(watch));
    }

    std::cout << "RAII HTTP Server listening on port " << port << "\n";
    std::cout << "STL-style API with monadic composition\n";
    std::cout << "Using " << pool.size() << " reactor threads\n";

    pool.start();
    pool.wait();

    std::cout << "Server stopped\n";
    return 0;
}
