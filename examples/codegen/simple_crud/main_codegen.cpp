// Simple CRUD API example with OpenAPI codegen
//
// This example demonstrates:
// 1. Using generated DTOs, JSON parsers, and validators
// 2. Implementing generated::api_handler interface
// 3. Using generated::make_router for automatic route binding
//
// Build and run:
//   cmake --preset examples
//   cmake --build --preset examples --target simple_crud_codegen
//   ./build/examples/examples/codegen/simple_crud/simple_crud_codegen

#include "katana/core/arena.hpp"
#include "katana/core/fd_watch.hpp"
#include "katana/core/http.hpp"
#include "katana/core/io_buffer.hpp"
#include "katana/core/reactor_pool.hpp"
#include "katana/core/router.hpp"
#include "katana/core/shutdown.hpp"
#include "katana/core/tcp_listener.hpp"
#include "katana/core/tcp_socket.hpp"

// Generated code
#include "generated/generated_dtos.hpp"
#include "generated/generated_handlers.hpp"
#include "generated/generated_json.hpp"
#include "generated/generated_router_bindings.hpp"
#include "generated/generated_routes.hpp"
#include "generated/generated_validators.hpp"

#include <cerrno>
#include <charconv>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>

using namespace katana;
using namespace katana::http;

// ============================================================================
// In-memory repository
// ============================================================================

class TaskRepository {
public:
    TaskRepository() : next_id_(1) {}

    std::vector<Task> list_all(monotonic_arena* arena) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Task> result;
        result.reserve(tasks_.size());
        for (const auto& [id, task_data] : tasks_) {
            Task task(arena);
            task.id = task_data.id;
            task.title = task_data.title;
            task.description = task_data.description;
            task.completed = task_data.completed;
            result.push_back(std::move(task));
        }
        return result;
    }

    std::optional<Task> get_by_id(int64_t id, monotonic_arena* arena) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = tasks_.find(id);
        if (it == tasks_.end()) {
            return std::nullopt;
        }

        Task task(arena);
        task.id = it->second.id;
        task.title = it->second.title;
        task.description = it->second.description;
        task.completed = it->second.completed;
        return task;
    }

    Task create(const CreateTaskRequest& req, monotonic_arena* arena) {
        std::lock_guard<std::mutex> lock(mutex_);
        int64_t id = next_id_++;

        TaskData data;
        data.id = id;
        // arena_string can be converted to std::string
        data.title = std::string(req.title);
        if (!req.description.empty()) {
            data.description = std::string(req.description);
        }
        data.completed = req.completed;

        tasks_[id] = data;

        Task task(arena);
        task.id = data.id;
        task.title = data.title;
        task.description = data.description;
        task.completed = data.completed;
        return task;
    }

    std::optional<Task> update(int64_t id, const UpdateTaskRequest& req, monotonic_arena* arena) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = tasks_.find(id);
        if (it == tasks_.end()) {
            return std::nullopt;
        }

        if (!req.title.empty()) {
            it->second.title = std::string(req.title);
        }
        if (!req.description.empty()) {
            it->second.description = std::string(req.description);
        }
        // Note: completed is always set in UpdateTaskRequest
        it->second.completed = req.completed;

        Task task(arena);
        task.id = it->second.id;
        task.title = it->second.title;
        task.description = it->second.description;
        task.completed = it->second.completed;
        return task;
    }

    bool remove(int64_t id) {
        std::lock_guard<std::mutex> lock(mutex_);
        return tasks_.erase(id) > 0;
    }

private:
    struct TaskData {
        int64_t id;
        std::string title;
        std::string description;
        bool completed;
    };

    mutable std::mutex mutex_;
    std::map<int64_t, TaskData> tasks_;
    int64_t next_id_;
};

// ============================================================================
// API Handler Implementation (using generated interface)
// ============================================================================

class TaskApiHandler : public generated::api_handler {
public:
    explicit TaskApiHandler(TaskRepository& repo) : repo_(repo) {}

    // GET /tasks
    response list_tasks(const request&, request_context& ctx) override {
        auto tasks = repo_.list_all(&ctx.arena);
        std::string json = serialize_Task_array(tasks);
        return response::json(json);
    }

    // POST /tasks
    response
    create_task(const request&, request_context& ctx, const CreateTaskRequest& body) override {
        // Validate using generated validator
        if (auto err = validate_CreateTaskRequest(body)) {
            std::string msg = std::string(err->field) + ": " + std::string(err->message);
            return response::error(problem_details::bad_request(msg));
        }

        auto task = repo_.create(body, &ctx.arena);
        std::string json = serialize_Task(task);

        response resp = response::json(json);
        resp.status = 201;
        return resp;
    }

    // GET /tasks/{id}
    response get_task(const request&, request_context& ctx, std::string_view id_str) override {
        int64_t id = 0;
        auto [ptr, ec] = std::from_chars(id_str.data(), id_str.data() + id_str.size(), id);

        if (ec != std::errc{}) {
            return response::error(problem_details::bad_request("Invalid id parameter"));
        }

        auto task = repo_.get_by_id(id, &ctx.arena);
        if (!task) {
            return response::error(problem_details::not_found("Task not found"));
        }

        std::string json = serialize_Task(*task);
        return response::json(json);
    }

    // PUT /tasks/{id}
    response update_task(const request&,
                         request_context& ctx,
                         std::string_view id_str,
                         const UpdateTaskRequest& body) override {
        int64_t id = 0;
        auto [ptr, ec] = std::from_chars(id_str.data(), id_str.data() + id_str.size(), id);

        if (ec != std::errc{}) {
            return response::error(problem_details::bad_request("Invalid id parameter"));
        }

        // Validate using generated validator
        if (auto err = validate_UpdateTaskRequest(body)) {
            std::string msg = std::string(err->field) + ": " + std::string(err->message);
            return response::error(problem_details::bad_request(msg));
        }

        auto task = repo_.update(id, body, &ctx.arena);
        if (!task) {
            return response::error(problem_details::not_found("Task not found"));
        }

        std::string json = serialize_Task(*task);
        return response::json(json);
    }

    // DELETE /tasks/{id}
    response delete_task(const request&, request_context&, std::string_view id_str) override {
        int64_t id = 0;
        auto [ptr, ec] = std::from_chars(id_str.data(), id_str.data() + id_str.size(), id);

        if (ec != std::errc{}) {
            return response::error(problem_details::bad_request("Invalid id parameter"));
        }

        bool deleted = repo_.remove(id);
        if (!deleted) {
            return response::error(problem_details::not_found("Task not found"));
        }

        response resp;
        resp.status = 204;
        resp.reason = "No Content";
        return resp;
    }

private:
    TaskRepository& repo_;
};

// ============================================================================
// Connection handling
// ============================================================================

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

// ============================================================================
// Main
// ============================================================================

int main() {
    std::cout << "Simple CRUD API with OpenAPI Codegen\n";
    std::cout << "====================================\n\n";

    // Initialize repository and handler
    TaskRepository repo;
    TaskApiHandler handler(repo);

    // Create router using generated bindings
    const auto& api_router = generated::make_router(handler);

    std::cout << "Generated routes:\n";
    for (const auto& route : generated::routes) {
        std::cout << "  " << method_to_string(route.method) << " " << route.path << " ("
                  << route.operation_id << ")\n";
    }
    std::cout << "\n";

    // Setup TCP listener
    tcp_listener listener(8080);
    if (!listener) {
        std::cerr << "Failed to create listener on port 8080\n";
        return 1;
    }

    listener.set_reuseport(true).set_backlog(1024);

    // Setup reactor pool
    reactor_pool_config config;
    config.reactor_count = 1;
    reactor_pool pool(config);

    std::vector<std::unique_ptr<connection_state>> connections;
    std::unique_ptr<fd_watch> accept_watch;

    auto& r = pool.get_reactor(0);
    accept_watch =
        std::make_unique<fd_watch>(r,
                                   listener.native_handle(),
                                   event_type::readable,
                                   [&r, &listener, &connections, &api_router](event_type) {
                                       accept_connection(r, listener, connections, api_router);
                                   });

    std::cout << "Server listening on http://0.0.0.0:8080\n\n";
    std::cout << "Try these commands:\n";
    std::cout << "  curl http://localhost:8080/tasks\n";
    std::cout << "  curl -X POST http://localhost:8080/tasks "
                 "-H 'Content-Type: application/json' -d '{\"title\":\"Buy milk\"}'\n";
    std::cout << "  curl http://localhost:8080/tasks/1\n";
    std::cout << "  curl -X PUT http://localhost:8080/tasks/1 "
                 "-H 'Content-Type: application/json' -d '{\"completed\":true}'\n";
    std::cout << "  curl -X DELETE http://localhost:8080/tasks/1\n";
    std::cout << "\nOr run: cd examples/codegen/simple_crud && ./test_requests.sh\n\n";

    shutdown_manager::instance().setup_signal_handlers();
    shutdown_manager::instance().set_shutdown_callback(
        [&pool]() { pool.graceful_stop(std::chrono::milliseconds(5000)); });

    pool.start();
    pool.wait();

    std::cout << "Server stopped\n";
    return 0;
}
