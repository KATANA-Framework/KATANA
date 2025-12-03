#include "katana/core/arena.hpp"
#include "katana/core/fd_watch.hpp"
#include "katana/core/http.hpp"
#include "katana/core/io_buffer.hpp"
#include "katana/core/reactor_pool.hpp"
#include "katana/core/router.hpp"
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

// Domain model
struct user {
    int id;
    std::string name;
    std::string email;
};

// Simple in-memory repository
class user_repository {
public:
    user_repository() {
        users_[1] = {1, "Alice", "alice@example.com"};
        users_[2] = {2, "Bob", "bob@example.com"};
        next_id_ = 3;
    }

    std::vector<user> find_all() const {
        std::vector<user> result;
        result.reserve(users_.size());
        for (const auto& [id, u] : users_) {
            result.push_back(u);
        }
        return result;
    }

    std::optional<user> find_by_id(int id) const {
        auto it = users_.find(id);
        if (it == users_.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    user create(std::string name, std::string email) {
        int new_id = next_id_++;
        user u{new_id, std::move(name), std::move(email)};
        users_[new_id] = u;
        return u;
    }

    bool update(int id, std::string name, std::string email) {
        auto it = users_.find(id);
        if (it == users_.end()) {
            return false;
        }
        it->second.name = std::move(name);
        it->second.email = std::move(email);
        return true;
    }

    bool remove(int id) {
        auto it = users_.find(id);
        if (it == users_.end()) {
            return false;
        }
        users_.erase(it);
        return true;
    }

private:
    std::map<int, user> users_;
    int next_id_;
};

// Helper to serialize user to JSON
std::string user_to_json(const user& u) {
    std::ostringstream json;
    json << "{\"id\":" << u.id << ",\"name\":\"" << u.name << "\",\"email\":\"" << u.email << "\"}";
    return json.str();
}

// Helper to parse JSON body (naive, for demo purposes)
struct user_dto {
    std::string name;
    std::string email;
};

std::optional<user_dto> parse_user_dto(std::string_view body) {
    user_dto dto;

    auto name_pos = body.find("\"name\":\"");
    auto email_pos = body.find("\"email\":\"");

    if (name_pos != std::string_view::npos) {
        name_pos += 8;
        auto name_end = body.find("\"", name_pos);
        if (name_end != std::string_view::npos) {
            dto.name = std::string(body.substr(name_pos, name_end - name_pos));
        }
    }

    if (email_pos != std::string_view::npos) {
        email_pos += 9;
        auto email_end = body.find("\"", email_pos);
        if (email_end != std::string_view::npos) {
            dto.email = std::string(body.substr(email_pos, email_end - email_pos));
        }
    }

    if (dto.name.empty() || dto.email.empty()) {
        return std::nullopt;
    }

    return dto;
}

// Handlers
class user_handlers {
public:
    explicit user_handlers(user_repository& repo) : repo_(repo) {}

    // GET /api/users
    result<response> list_users(const request&, request_context&) {
        auto users = repo_.find_all();
        std::ostringstream json;
        json << "[";
        bool first = true;
        for (const auto& u : users) {
            if (!first)
                json << ",";
            json << user_to_json(u);
            first = false;
        }
        json << "]";
        return response::json(json.str());
    }

    // GET /api/users/{id}
    result<response> get_user(const request&, request_context& ctx) {
        auto id_str = ctx.params.get("id");
        if (!id_str) {
            return response::error(problem_details::bad_request("Missing user ID"));
        }

        int id = 0;
        auto [ptr, ec] = std::from_chars(id_str->data(), id_str->data() + id_str->size(), id);
        if (ec != std::errc{}) {
            return response::error(problem_details::bad_request("Invalid user ID"));
        }

        auto user = repo_.find_by_id(id);
        if (!user) {
            return response::error(problem_details::not_found("User not found"));
        }

        return response::json(user_to_json(*user));
    }

    // POST /api/users
    result<response> create_user(const request& req, request_context&) {
        auto dto = parse_user_dto(req.body);
        if (!dto) {
            return response::error(problem_details::bad_request("Name and email required"));
        }

        auto user = repo_.create(std::move(dto->name), std::move(dto->email));

        auto resp = response::json(user_to_json(user));
        resp.status = 201;
        resp.reason = "Created";
        return resp;
    }

    // PUT /api/users/{id}
    result<response> update_user(const request& req, request_context& ctx) {
        auto id_str = ctx.params.get("id");
        if (!id_str) {
            return response::error(problem_details::bad_request("Missing user ID"));
        }

        int id = 0;
        auto [ptr, ec] = std::from_chars(id_str->data(), id_str->data() + id_str->size(), id);
        if (ec != std::errc{}) {
            return response::error(problem_details::bad_request("Invalid user ID"));
        }

        auto dto = parse_user_dto(req.body);
        if (!dto) {
            return response::error(problem_details::bad_request("Name and email required"));
        }

        if (!repo_.update(id, std::move(dto->name), std::move(dto->email))) {
            return response::error(problem_details::not_found("User not found"));
        }

        auto user = repo_.find_by_id(id);
        return response::json(user_to_json(*user));
    }

    // DELETE /api/users/{id}
    result<response> delete_user(const request&, request_context& ctx) {
        auto id_str = ctx.params.get("id");
        if (!id_str) {
            return response::error(problem_details::bad_request("Missing user ID"));
        }

        int id = 0;
        auto [ptr, ec] = std::from_chars(id_str->data(), id_str->data() + id_str->size(), id);
        if (ec != std::errc{}) {
            return response::error(problem_details::bad_request("Invalid user ID"));
        }

        if (!repo_.remove(id)) {
            return response::error(problem_details::not_found("User not found"));
        }

        response resp;
        resp.status = 204;
        resp.reason = "No Content";
        return resp;
    }

    // GET /api/health
    result<response> health_check(const request&, request_context&) {
        return response::json("{\"status\":\"healthy\"}");
    }

private:
    user_repository& repo_;
};

// Connection handling (same as before)
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

void handle_connection(connection_state& state,
                       [[maybe_unused]] reactor& r,
                       const router& api_router) {
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
        auto resp = dispatch_or_problem(api_router, req, ctx);

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
                       const router& api_router) {
    auto accept_result = listener.accept();
    if (!accept_result) {
        return;
    }

    auto state = std::make_unique<connection_state>(std::move(*accept_result));
    int32_t fd = state->socket.native_handle();

    auto* state_ptr = state.get();
    state->watch = std::make_unique<fd_watch>(
        r, fd, event_type::readable, [state_ptr, &r, &api_router](event_type) {
            handle_connection(*state_ptr, r, api_router);
        });

    connections.push_back(std::move(state));
}

int main() {
    // Setup repository
    user_repository repo;
    user_handlers handlers(repo);

    // Define routes using the new router
    route_entry routes[] = {
        {method::get,
         path_pattern::from_literal<"/api/users">(),
         handler_fn([&handlers](const request& req, request_context& ctx) {
             return handlers.list_users(req, ctx);
         })},
        {method::get,
         path_pattern::from_literal<"/api/users/{id}">(),
         handler_fn([&handlers](const request& req, request_context& ctx) {
             return handlers.get_user(req, ctx);
         })},
        {method::post,
         path_pattern::from_literal<"/api/users">(),
         handler_fn([&handlers](const request& req, request_context& ctx) {
             return handlers.create_user(req, ctx);
         })},
        {method::put,
         path_pattern::from_literal<"/api/users/{id}">(),
         handler_fn([&handlers](const request& req, request_context& ctx) {
             return handlers.update_user(req, ctx);
         })},
        {method::del,
         path_pattern::from_literal<"/api/users/{id}">(),
         handler_fn([&handlers](const request& req, request_context& ctx) {
             return handlers.delete_user(req, ctx);
         })},
        {method::get,
         path_pattern::from_literal<"/api/health">(),
         handler_fn([&handlers](const request& req, request_context& ctx) {
             return handlers.health_check(req, ctx);
         })},
    };

    router api_router(routes);

    // Setup TCP listener
    tcp_listener listener(8080);
    if (!listener) {
        std::cerr << "Failed to create listener on port 8080\n";
        return 1;
    }

    listener.set_reuseport(true).set_backlog(1024);

    reactor_pool_config config;
    config.reactor_count = 1;
    reactor_pool pool(config);

    std::vector<std::unique_ptr<connection_state>> connections;
    std::unique_ptr<fd_watch> accept_watch;

    auto& reactor = pool.get_reactor(0);
    accept_watch = std::make_unique<fd_watch>(
        reactor,
        listener.native_handle(),
        event_type::readable,
        [&reactor, &listener, &connections, &api_router](event_type) {
            accept_connection(reactor, listener, connections, api_router);
        });

    std::cout << "REST API Server with Router listening on http://localhost:8080\n";
    std::cout << "\nEndpoints:\n";
    std::cout << "  GET    /api/users       - List all users\n";
    std::cout << "  GET    /api/users/:id   - Get user by ID\n";
    std::cout << "  POST   /api/users       - Create new user\n";
    std::cout << "  PUT    /api/users/:id   - Update user\n";
    std::cout << "  DELETE /api/users/:id   - Delete user\n";
    std::cout << "  GET    /api/health      - Health check\n";
    std::cout << "\nExamples:\n";
    std::cout << "  curl http://localhost:8080/api/users\n";
    std::cout << "  curl http://localhost:8080/api/users/1\n";
    std::cout << "  curl -X POST http://localhost:8080/api/users -d "
                 "'{\"name\":\"Charlie\",\"email\":\"charlie@example.com\"}'\n";
    std::cout << "  curl -X PUT http://localhost:8080/api/users/1 -d "
                 "'{\"name\":\"Alice Updated\",\"email\":\"alice.new@example.com\"}'\n";
    std::cout << "  curl -X DELETE http://localhost:8080/api/users/1\n\n";

    shutdown_manager::instance().setup_signal_handlers();
    shutdown_manager::instance().set_shutdown_callback(
        [&pool]() { pool.graceful_stop(std::chrono::milliseconds(5000)); });

    pool.start();
    pool.wait();

    std::cout << "Server stopped\n";
    return 0;
}
