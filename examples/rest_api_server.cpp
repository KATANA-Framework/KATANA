#include "katana/core/reactor_pool.hpp"
#include "katana/core/tcp_listener.hpp"
#include "katana/core/tcp_socket.hpp"
#include "katana/core/fd_watch.hpp"
#include "katana/core/io_buffer.hpp"
#include "katana/core/http.hpp"

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include <functional>
#include <sstream>

using namespace katana;
using namespace katana::http;

struct user {
    int32_t id;
    std::string name;
    std::string email;
};

class user_store {
public:
    user_store() {
        users_[1] = {1, "Alice", "alice@example.com"};
        users_[2] = {2, "Bob", "bob@example.com"};
        next_id_ = 3;
    }

    std::optional<user> get(int32_t id) const {
        auto it = users_.find(id);
        if (it != users_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::vector<user> list() const {
        std::vector<user> result;
        result.reserve(users_.size());
        for (const auto& [id, u] : users_) {
            result.push_back(u);
        }
        return result;
    }

    user create(std::string name, std::string email) {
        user u{next_id_++, std::move(name), std::move(email)};
        users_[u.id] = u;
        return u;
    }

    bool update(int32_t id, std::string name, std::string email) {
        auto it = users_.find(id);
        if (it == users_.end()) {
            return false;
        }
        it->second.name = std::move(name);
        it->second.email = std::move(email);
        return true;
    }

    bool remove(int32_t id) {
        return users_.erase(id) > 0;
    }

private:
    std::unordered_map<int32_t, user> users_;
    int32_t next_id_ = 1;
};

std::string user_to_json(const user& u) {
    std::ostringstream oss;
    oss << "{\"id\":" << u.id
        << ",\"name\":\"" << u.name << "\""
        << ",\"email\":\"" << u.email << "\"}";
    return oss.str();
}

std::string users_to_json(const std::vector<user>& users) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < users.size(); ++i) {
        if (i > 0) oss << ",";
        oss << user_to_json(users[i]);
    }
    oss << "]";
    return oss.str();
}

std::pair<std::string, std::string> parse_json_user(std::string_view json) {
    std::string name, email;

    auto name_pos = json.find("\"name\"");
    if (name_pos != std::string_view::npos) {
        auto start = json.find("\"", name_pos + 6);
        if (start != std::string_view::npos) {
            auto end = json.find("\"", start + 1);
            if (end != std::string_view::npos) {
                name = json.substr(start + 1, end - start - 1);
            }
        }
    }

    auto email_pos = json.find("\"email\"");
    if (email_pos != std::string_view::npos) {
        auto start = json.find("\"", email_pos + 7);
        if (start != std::string_view::npos) {
            auto end = json.find("\"", start + 1);
            if (end != std::string_view::npos) {
                email = json.substr(start + 1, end - start - 1);
            }
        }
    }

    return {std::move(name), std::move(email)};
}

int32_t extract_id(std::string_view path) {
    auto last_slash = path.rfind('/');
    if (last_slash != std::string_view::npos && last_slash + 1 < path.size()) {
        auto id_str = path.substr(last_slash + 1);
        try {
            return std::stoi(std::string(id_str));
        } catch (...) {
            return -1;
        }
    }
    return -1;
}

class http_connection {
public:
    explicit http_connection(tcp_socket sock)
        : socket_(std::move(sock))
        , read_buffer_(8192)
        , write_buffer_(8192)
    {}

    result<void> read_and_parse() {
        auto buf = read_buffer_.writable_span(4096);
        return socket_.read(buf)
            .and_then([this](auto span) -> result<void> {
                if (span.empty()) {
                    return {};
                }
                read_buffer_.commit(span.size());
                auto readable = read_buffer_.readable_span();
                return parser_.parse(readable)
                    .and_then([](auto state) -> result<void> {
                        (void)state;
                        return {};
                    });
            });
    }

    result<void> write_response(const response& resp) {
        auto serialized = resp.serialize();
        write_buffer_.append(serialized);
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

    [[nodiscard]] bool is_complete() const noexcept {
        return parser_.is_complete();
    }

    [[nodiscard]] request&& take_request() {
        return parser_.take_request();
    }

    [[nodiscard]] int32_t native_handle() const noexcept {
        return socket_.native_handle();
    }

    [[nodiscard]] bool has_pending_write() const noexcept {
        return !write_buffer_.empty();
    }

private:
    tcp_socket socket_;
    io_buffer read_buffer_;
    io_buffer write_buffer_;
    parser parser_;
};

struct connection_state {
    http_connection conn;
    std::unique_ptr<fd_watch> watch;

    explicit connection_state(tcp_socket sock)
        : conn(std::move(sock))
    {}
};

response handle_users_list(const request&, user_store& store) {
    auto users = store.list();
    auto json = users_to_json(users);
    return response::json(std::move(json));
}

response handle_user_get(const request& req, user_store& store) {
    auto id = extract_id(req.uri);
    if (id < 0) {
        return response::error(problem_details::bad_request("Invalid user ID"));
    }

    auto user_opt = store.get(id);
    if (!user_opt) {
        return response::error(problem_details::not_found("User not found"));
    }

    return response::json(user_to_json(*user_opt));
}

response handle_user_create(const request& req, user_store& store) {
    auto [name, email] = parse_json_user(req.body);
    if (name.empty() || email.empty()) {
        return response::error(problem_details::bad_request("Missing name or email"));
    }

    auto new_user = store.create(std::move(name), std::move(email));
    auto resp = response::json(user_to_json(new_user));
    resp.status = 201;
    resp.reason = "Created";
    return resp;
}

response handle_user_update(const request& req, user_store& store) {
    auto id = extract_id(req.uri);
    if (id < 0) {
        return response::error(problem_details::bad_request("Invalid user ID"));
    }

    auto [name, email] = parse_json_user(req.body);
    if (name.empty() || email.empty()) {
        return response::error(problem_details::bad_request("Missing name or email"));
    }

    if (!store.update(id, std::move(name), std::move(email))) {
        return response::error(problem_details::not_found("User not found"));
    }

    auto user_opt = store.get(id);
    return response::json(user_to_json(*user_opt));
}

response handle_user_delete(const request& req, user_store& store) {
    auto id = extract_id(req.uri);
    if (id < 0) {
        return response::error(problem_details::bad_request("Invalid user ID"));
    }

    if (!store.remove(id)) {
        return response::error(problem_details::not_found("User not found"));
    }

    response resp;
    resp.status = 204;
    resp.reason = "No Content";
    return resp;
}

response route_request(const request& req, user_store& store) {
    std::string_view path = req.uri;

    if (path == "/users" || path == "/users/") {
        if (req.http_method == method::GET) {
            return handle_users_list(req, store);
        } else if (req.http_method == method::POST) {
            return handle_user_create(req, store);
        }
    } else if (path.starts_with("/users/")) {
        if (req.http_method == method::GET) {
            return handle_user_get(req, store);
        } else if (req.http_method == method::PUT) {
            return handle_user_update(req, store);
        } else if (req.http_method == method::DELETE) {
            return handle_user_delete(req, store);
        }
    }

    return response::error(problem_details::not_found("Endpoint not found"));
}

void handle_connection(connection_state& state, [[maybe_unused]] reactor& r, user_store& store) {
    state.conn.read_and_parse()
        .and_then([&state, &store]() -> result<void> {
            if (!state.conn.is_complete()) {
                return {};
            }

            auto req = state.conn.take_request();
            auto resp = route_request(req, store);
            return state.conn.write_response(resp);
        })
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

void accept_connection(reactor& r, tcp_listener& listener,
                      std::vector<std::unique_ptr<connection_state>>& connections,
                      user_store& store) {
    listener.accept()
        .and_then([&](tcp_socket sock) -> result<void> {
            auto state = std::make_unique<connection_state>(std::move(sock));
            int32_t fd = state->conn.native_handle();

            auto* state_ptr = state.get();
            state->watch = std::make_unique<fd_watch>(
                r, fd, event_type::readable,
                [state_ptr, &r, &store](event_type) {
                    handle_connection(*state_ptr, r, store);
                }
            );

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

    user_store store;
    std::vector<std::unique_ptr<connection_state>> connections;
    std::vector<std::unique_ptr<fd_watch>> accept_watches;

    for (auto& reactor : pool) {
        auto watch = std::make_unique<fd_watch>(
            reactor, listener.native_handle(), event_type::readable,
            [&reactor, &listener, &connections, &store](event_type) {
                accept_connection(reactor, listener, connections, store);
            }
        );
        accept_watches.push_back(std::move(watch));
    }

    std::cout << "REST API Server listening on port " << port << "\n";
    std::cout << "Endpoints:\n";
    std::cout << "  GET    /users      - List all users\n";
    std::cout << "  POST   /users      - Create user (JSON: {\"name\":\"...\",\"email\":\"...\"})\n";
    std::cout << "  GET    /users/:id  - Get user by ID\n";
    std::cout << "  PUT    /users/:id  - Update user (JSON: {\"name\":\"...\",\"email\":\"...\"})\n";
    std::cout << "  DELETE /users/:id  - Delete user\n";
    std::cout << "Using " << pool.size() << " reactor threads\n";

    pool.start();
    pool.wait();

    return 0;
}
