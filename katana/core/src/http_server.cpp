#include "katana/core/http_server.hpp"
#include "katana/core/problem.hpp"

#include <iostream>

namespace katana {
namespace http {

void server::handle_connection(connection_state& state, [[maybe_unused]] reactor& r) {
    while (true) {
        auto buf = state.read_buffer.writable_span(4096);
        auto read_result = state.socket.read(buf);

        if (!read_result) {
            if (read_result.error().value() == EAGAIN ||
                read_result.error().value() == EWOULDBLOCK) {
                break;
            }
            state.watch.reset();
            return;
        }

        if (read_result->empty()) {
            state.watch.reset();
            return;
        }

        state.read_buffer.commit(read_result->size());

        auto readable = state.read_buffer.readable_span();
        auto parse_result = state.http_parser.parse(readable);

        if (!parse_result) {
            auto resp = response::error(problem_details::bad_request("Invalid HTTP request"));
            state.write_buffer.append(resp.serialize());
            state.watch.reset();
            return;
        }

        if (!state.http_parser.is_complete()) {
            continue;
        }

        const auto& req = state.http_parser.get_request();
        request_context ctx{state.arena};
        auto resp = dispatch_or_problem(router_, req, ctx);

        // Call on_request callback if set
        if (on_request_callback_) {
            on_request_callback_(req, resp);
        }

        state.write_buffer.append(resp.serialize());

        while (!state.write_buffer.empty()) {
            auto data = state.write_buffer.readable_span();
            auto write_result = state.socket.write(data);

            if (!write_result) {
                if (write_result.error().value() == EAGAIN ||
                    write_result.error().value() == EWOULDBLOCK) {
                    break;
                }
                state.watch.reset();
                return;
            }

            if (write_result.value() == 0) {
                break;
            }

            state.write_buffer.consume(write_result.value());
        }

        if (state.write_buffer.empty()) {
            state.watch.reset();
        }

        return;
    }
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
    // Create TCP listener
    tcp_listener listener(port_);
    if (!listener) {
        std::cerr << "Failed to create listener on port " << port_ << "\n";
        return 1;
    }

    listener.set_reuseport(reuseport_).set_backlog(backlog_);

    // Setup reactor pool
    reactor_pool_config config;
    config.reactor_count = static_cast<uint32_t>(worker_count_);
    reactor_pool pool(config);

    std::vector<std::unique_ptr<connection_state>> connections;
    std::unique_ptr<fd_watch> accept_watch;

    auto& r = pool.get_reactor(0);
    accept_watch = std::make_unique<fd_watch>(r,
                                              listener.native_handle(),
                                              event_type::readable,
                                              [this, &r, &listener, &connections](event_type) {
                                                  accept_connection(r, listener, connections);
                                              });

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

    // Run the server
    pool.start();
    pool.wait();

    return 0;
}

} // namespace http
} // namespace katana
