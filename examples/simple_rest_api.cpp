#include "katana/core/arena.hpp"
#include "katana/core/fd_watch.hpp"
#include "katana/core/http.hpp"
#include "katana/core/io_buffer.hpp"
#include "katana/core/reactor_pool.hpp"
#include "katana/core/shutdown.hpp"
#include "katana/core/tcp_listener.hpp"
#include "katana/core/tcp_socket.hpp"

#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace katana;
using namespace katana::http;

struct user {
    int id;
    std::string name;
    std::string email;
};

class simple_rest_api {
public:
    simple_rest_api() {
        users_[1] = {1, "Alice", "alice@example.com"};
        users_[2] = {2, "Bob", "bob@example.com"};
        next_id_ = 3;
    }

    response handle_request(const request& req) {
        if (req.uri == "/api/users" && req.http_method == method::get) {
            return get_all_users();
        }

        if (req.uri.starts_with("/api/users/") && req.http_method == method::get) {
            auto id_str = req.uri.substr(11);
            try {
                int id = std::stoi(std::string(id_str));
                return get_user(id);
            } catch (...) {
                return response::error(problem_details::bad_request("Invalid user ID"));
            }
        }

        if (req.uri == "/api/users" && req.http_method == method::post) {
            return create_user(req);
        }

        if (req.uri.starts_with("/api/users/") && req.http_method == method::put) {
            auto id_str = req.uri.substr(11);
            try {
                int id = std::stoi(std::string(id_str));
                return update_user(id, req);
            } catch (...) {
                return response::error(problem_details::bad_request("Invalid user ID"));
            }
        }

        if (req.uri.starts_with("/api/users/") && req.http_method == method::del) {
            auto id_str = req.uri.substr(11);
            try {
                int id = std::stoi(std::string(id_str));
                return delete_user(id);
            } catch (...) {
                return response::error(problem_details::bad_request("Invalid user ID"));
            }
        }

        if (req.uri == "/api/health" && req.http_method == method::get) {
            return response::json("{\"status\":\"healthy\"}");
        }

        problem_details not_found;
        not_found.status = 404;
        not_found.title = "Not Found";
        not_found.detail = "The requested resource was not found";
        return response::error(not_found);
    }

private:
    response get_all_users() {
        std::ostringstream json;
        json << "[";
        bool first = true;
        for (const auto& [id, user] : users_) {
            if (!first)
                json << ",";
            json << "{\"id\":" << user.id << ",\"name\":\"" << user.name << "\""
                 << ",\"email\":\"" << user.email << "\"}";
            first = false;
        }
        json << "]";
        return response::json(json.str());
    }

    response get_user(int id) {
        auto it = users_.find(id);
        if (it == users_.end()) {
            problem_details not_found;
            not_found.status = 404;
            not_found.title = "User Not Found";
            not_found.detail = "User with ID " + std::to_string(id) + " not found";
            return response::error(not_found);
        }

        const auto& user = it->second;
        std::ostringstream json;
        json << "{\"id\":" << user.id << ",\"name\":\"" << user.name << "\""
             << ",\"email\":\"" << user.email << "\"}";
        return response::json(json.str());
    }

    response create_user(const request& req) {
        std::string name;
        std::string email;

        auto name_pos = req.body.find("\"name\":\"");
        auto email_pos = req.body.find("\"email\":\"");

        if (name_pos != std::string::npos) {
            name_pos += 8;
            auto name_end = req.body.find("\"", name_pos);
            if (name_end != std::string::npos) {
                name = req.body.substr(name_pos, name_end - name_pos);
            }
        }

        if (email_pos != std::string::npos) {
            email_pos += 9;
            auto email_end = req.body.find("\"", email_pos);
            if (email_end != std::string::npos) {
                email = req.body.substr(email_pos, email_end - email_pos);
            }
        }

        if (name.empty() || email.empty()) {
            return response::error(problem_details::bad_request("Name and email required"));
        }

        int new_id = next_id_++;
        users_[new_id] = {new_id, name, email};

        std::ostringstream json;
        json << "{\"id\":" << new_id << ",\"name\":\"" << name << "\""
             << ",\"email\":\"" << email << "\"}";

        response resp = response::json(json.str());
        resp.status = 201;
        resp.reason = "Created";
        return resp;
    }

    response update_user(int id, const request& req) {
        auto it = users_.find(id);
        if (it == users_.end()) {
            problem_details not_found;
            not_found.status = 404;
            not_found.title = "User Not Found";
            return response::error(not_found);
        }

        auto& user = it->second;

        auto name_pos = req.body.find("\"name\":\"");
        auto email_pos = req.body.find("\"email\":\"");

        if (name_pos != std::string::npos) {
            name_pos += 8;
            auto name_end = req.body.find("\"", name_pos);
            if (name_end != std::string::npos) {
                user.name = req.body.substr(name_pos, name_end - name_pos);
            }
        }

        if (email_pos != std::string::npos) {
            email_pos += 9;
            auto email_end = req.body.find("\"", email_pos);
            if (email_end != std::string::npos) {
                user.email = req.body.substr(email_pos, email_end - email_pos);
            }
        }

        std::ostringstream json;
        json << "{\"id\":" << user.id << ",\"name\":\"" << user.name << "\""
             << ",\"email\":\"" << user.email << "\"}";
        return response::json(json.str());
    }

    response delete_user(int id) {
        auto it = users_.find(id);
        if (it == users_.end()) {
            problem_details not_found;
            not_found.status = 404;
            not_found.title = "User Not Found";
            return response::error(not_found);
        }

        users_.erase(it);

        response resp;
        resp.status = 204;
        resp.reason = "No Content";
        return resp;
    }

    std::map<int, user> users_;
    int next_id_;
};

struct connection_state {
    tcp_socket socket;
    io_buffer read_buffer;
    io_buffer write_buffer;
    monotonic_arena arena;
    parser http_parser;
    std::unique_ptr<fd_watch> watch;

    explicit connection_state(tcp_socket sock)
        : socket(std::move(sock)), read_buffer(8192), write_buffer(8192), arena(8192),
          http_parser(&arena) {}
};

void handle_connection(connection_state& state, [[maybe_unused]] reactor& r, simple_rest_api& api) {
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
        auto resp = api.handle_request(req);

        std::cout << method_to_string(req.http_method) << " " << req.uri << " -> " << resp.status
                  << "\n";

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

void accept_connection(reactor& r,
                       tcp_listener& listener,
                       std::vector<std::unique_ptr<connection_state>>& connections,
                       simple_rest_api& api) {
    auto accept_result = listener.accept();
    if (!accept_result) {
        return;
    }

    auto state = std::make_unique<connection_state>(std::move(*accept_result));
    int32_t fd = state->socket.native_handle();

    auto* state_ptr = state.get();
    state->watch =
        std::make_unique<fd_watch>(r, fd, event_type::readable, [state_ptr, &r, &api](event_type) {
            handle_connection(*state_ptr, r, api);
        });

    connections.push_back(std::move(state));
}

int main() {
    tcp_listener listener(8080);
    if (!listener) {
        std::cerr << "Failed to create listener on port 8080\n";
        return 1;
    }

    listener.set_reuseport(true).set_backlog(1024);

    reactor_pool_config config;
    config.reactor_count = 1;
    reactor_pool pool(config);

    simple_rest_api api;

    std::vector<std::unique_ptr<connection_state>> connections;
    std::unique_ptr<fd_watch> accept_watch;

    auto& reactor = pool.get_reactor(0);
    accept_watch =
        std::make_unique<fd_watch>(reactor,
                                   listener.native_handle(),
                                   event_type::readable,
                                   [&reactor, &listener, &connections, &api](event_type) {
                                       accept_connection(reactor, listener, connections, api);
                                   });

    std::cout << "Simple REST API listening on http://localhost:8080\n";
    std::cout << "\nEndpoints:\n";
    std::cout << "  GET    /api/users       - List all users\n";
    std::cout << "  GET    /api/users/:id   - Get user by ID\n";
    std::cout << "  POST   /api/users       - Create new user\n";
    std::cout << "  PUT    /api/users/:id   - Update user\n";
    std::cout << "  DELETE /api/users/:id   - Delete user\n";
    std::cout << "  GET    /api/health      - Health check\n";
    std::cout << "\nExample:\n";
    std::cout << "  curl http://localhost:8080/api/users\n";
    std::cout << "  curl -X POST http://localhost:8080/api/users -d "
                 "'{\"name\":\"Charlie\",\"email\":\"charlie@example.com\"}'\n\n";

    shutdown_manager::instance().setup_signal_handlers();
    shutdown_manager::instance().set_shutdown_callback(
        [&pool]() { pool.graceful_stop(std::chrono::milliseconds(5000)); });

    pool.start();
    pool.wait();

    std::cout << "Server stopped\n";
    return 0;
}
