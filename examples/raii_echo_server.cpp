#include "katana/core/fd_watch.hpp"
#include "katana/core/io_buffer.hpp"
#include "katana/core/reactor_pool.hpp"
#include "katana/core/tcp_listener.hpp"
#include "katana/core/tcp_socket.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

using namespace katana;

struct connection_state {
    tcp_socket socket;
    io_buffer read_buffer;
    io_buffer write_buffer;
    std::unique_ptr<fd_watch> watch;

    explicit connection_state(tcp_socket sock)
        : socket(std::move(sock)), read_buffer(8192), write_buffer(8192) {}
};

void handle_read(connection_state& conn, reactor& r);
void handle_write(connection_state& conn, reactor& r);

void handle_read(connection_state& conn, [[maybe_unused]] reactor& r) {
    auto buf = conn.read_buffer.writable_span(4096);
    auto res = conn.socket.read(buf);

    if (!res) {
        return;
    }

    auto read_span = res.value();
    if (read_span.empty()) {
        return;
    }

    conn.read_buffer.commit(read_span.size());

    auto readable = conn.read_buffer.readable_span();
    conn.write_buffer.append(readable);
    conn.read_buffer.consume(readable.size());

    conn.watch->modify(event_type::writable);
}

void handle_write(connection_state& conn, [[maybe_unused]] reactor& r) {
    if (conn.write_buffer.empty()) {
        conn.watch->modify(event_type::readable);
        return;
    }

    auto data = conn.write_buffer.readable_span();
    auto res = conn.socket.write(data);

    if (!res) {
        return;
    }

    conn.write_buffer.consume(res.value());

    if (conn.write_buffer.empty()) {
        conn.watch->modify(event_type::readable);
    }
}

void accept_connection(reactor& r,
                       tcp_listener& listener,
                       std::vector<std::unique_ptr<connection_state>>& connections) {
    auto sock_res = listener.accept();
    if (!sock_res) {
        return;
    }

    auto conn = std::make_unique<connection_state>(std::move(sock_res.value()));
    int32_t fd = conn->socket.native_handle();

    auto* conn_ptr = conn.get();
    conn->watch =
        std::make_unique<fd_watch>(r, fd, event_type::readable, [conn_ptr, &r](event_type events) {
            if (has_flag(events, event_type::readable)) {
                handle_read(*conn_ptr, r);
            }
            if (has_flag(events, event_type::writable)) {
                handle_write(*conn_ptr, r);
            }
        });

    connections.push_back(std::move(conn));
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

    listener.set_reuseport(true);

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

    std::cout << "RAII Echo Server listening on port " << port << "\n";
    std::cout << "Using " << pool.size() << " reactor threads\n";
    std::cout << "Thread pinning: " << (config.enable_thread_pinning ? "enabled" : "disabled")
              << "\n";

    pool.start();
    pool.wait();

    std::cout << "Server stopped\n";
    return 0;
}
