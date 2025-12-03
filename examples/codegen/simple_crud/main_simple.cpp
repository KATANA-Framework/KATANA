// Simple CRUD API with OpenAPI codegen - SIMPLIFIED VERSION
//
// This version uses katana::http::server abstraction to eliminate boilerplate!
//
// Build and run:
//   cmake --preset examples
//   cmake --build --preset examples --target simple_crud_simple
//   ./build/examples/examples/codegen/simple_crud/simple_crud_simple

#include "katana/core/http_server.hpp"
#include "katana/core/router.hpp"

// Generated code
#include "generated/generated_dtos.hpp"
#include "generated/generated_handlers.hpp"
#include "generated/generated_json.hpp"
#include "generated/generated_router_bindings.hpp"
#include "generated/generated_routes.hpp"
#include "generated/generated_validators.hpp"

#include <charconv>
#include <iostream>
#include <map>
#include <mutex>
#include <string>

using namespace katana;
using namespace katana::http;

// ============================================================================
// In-memory repository (same as before)
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
        if (it == tasks_.end())
            return std::nullopt;

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

        TaskData data{id, std::string(req.title), std::string(req.description), req.completed};
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
        if (it == tasks_.end())
            return std::nullopt;

        if (!req.title.empty())
            it->second.title = std::string(req.title);
        if (!req.description.empty())
            it->second.description = std::string(req.description);
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
// API Handler (using generated interface + fluent response API)
// ============================================================================

class TaskApiHandler : public generated::api_handler {
public:
    explicit TaskApiHandler(TaskRepository& repo) : repo_(repo) {}

    response list_tasks(const request&, request_context& ctx) override {
        auto tasks = repo_.list_all(&ctx.arena);
        return response::json(serialize_Task_array(tasks));
    }

    response
    create_task(const request&, request_context& ctx, const CreateTaskRequest& body) override {
        if (auto err = validate_CreateTaskRequest(body)) {
            return response::error(problem_details::bad_request(std::string(err->field) + ": " +
                                                                std::string(err->message)));
        }

        auto task = repo_.create(body, &ctx.arena);
        return response::json(serialize_Task(task)).with_status(201);
    }

    response get_task(const request&, request_context& ctx, std::string_view id_str) override {
        int64_t id = 0;
        auto [ptr, ec] = std::from_chars(id_str.data(), id_str.data() + id_str.size(), id);
        if (ec != std::errc{}) {
            return response::error(problem_details::bad_request("Invalid id"));
        }

        auto task = repo_.get_by_id(id, &ctx.arena);
        if (!task)
            return response::error(problem_details::not_found("Task not found"));

        return response::json(serialize_Task(*task));
    }

    response update_task(const request&,
                         request_context& ctx,
                         std::string_view id_str,
                         const UpdateTaskRequest& body) override {
        int64_t id = 0;
        auto [ptr, ec] = std::from_chars(id_str.data(), id_str.data() + id_str.size(), id);
        if (ec != std::errc{}) {
            return response::error(problem_details::bad_request("Invalid id"));
        }

        if (auto err = validate_UpdateTaskRequest(body)) {
            return response::error(problem_details::bad_request(std::string(err->field) + ": " +
                                                                std::string(err->message)));
        }

        auto task = repo_.update(id, body, &ctx.arena);
        if (!task)
            return response::error(problem_details::not_found("Task not found"));

        return response::json(serialize_Task(*task));
    }

    response delete_task(const request&, request_context&, std::string_view id_str) override {
        int64_t id = 0;
        auto [ptr, ec] = std::from_chars(id_str.data(), id_str.data() + id_str.size(), id);
        if (ec != std::errc{}) {
            return response::error(problem_details::bad_request("Invalid id"));
        }

        if (!repo_.remove(id)) {
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
// Main - ULTRA SIMPLIFIED!
// ============================================================================

int main() {
    std::cout << "Simple CRUD API with OpenAPI Codegen (Simplified)\n";
    std::cout << "=================================================\n\n";

    // Create repository and handler
    TaskRepository repo;
    TaskApiHandler handler(repo);

    // Create router from generated bindings
    const auto& api_router = generated::make_router(handler);

    // That's it! Server abstraction handles everything else!
    return http::server(api_router)
        .listen(8080)
        .workers(4)
        .on_start([]() {
            std::cout << "✨ Server started!\n";
            std::cout << "\nGenerated routes:\n";
            for (const auto& route : generated::routes) {
                std::cout << "  " << method_to_string(route.method) << " " << route.path << "\n";
            }
            std::cout << "\nTry:\n";
            std::cout << "  curl http://localhost:8080/tasks\n";
            std::cout << "  curl -X POST http://localhost:8080/tasks "
                         "-d '{\"title\":\"Test\"}'\n\n";
        })
        .on_request([](const request& req, const response& resp) {
            std::cout << method_to_string(req.http_method) << " " << req.uri << " -> "
                      << resp.status << "\n";
        })
        .run();
}
