#include "generated/generated_dtos.hpp"
#include "generated/generated_handlers.hpp"
#include "generated/generated_json.hpp"
#include "generated/generated_router_bindings.hpp"
#include "generated/generated_routes.hpp"
#include "generated/generated_validators.hpp"

#include "katana/core/arena.hpp"
#include "katana/core/http.hpp"
#include "katana/core/http_server.hpp"
#include "katana/core/reactor_pool.hpp"
#include "katana/core/router.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <map>
#include <mutex>
#include <vector>

using namespace katana;
using namespace katana::http;

// Simple in-memory storage for tasks
class TaskStorage {
public:
    TaskStorage()
        : next_id_(1), start_time_(std::chrono::steady_clock::now()), total_requests_(0) {}

    Task create_task(const CreateTaskRequest& req, monotonic_arena& arena) {
        std::lock_guard lock(mutex_);

        Task task;
        task.id = next_id_++;
        task.title = std::string(req.title);
        if (req.description) {
            task.description = std::string(*req.description);
        }
        task.status = std::string("pending");
        task.priority = req.priority;

        if (req.tags) {
            task.tags = std::vector<std::string>();
            for (const auto& tag : *req.tags) {
                task.tags->push_back(std::string(tag));
            }
        }

        if (req.assignee_id) {
            User user;
            user.id = *req.assignee_id;
            user.email = std::string("user@example.com");
            user.name = std::string("User " + std::to_string(*req.assignee_id));
            task.assignee = user;
        }

        task.due_date = req.due_date;

        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        task.created_at = std::string("2026-02-09T12:00:00Z");
        task.updated_at = {};

        tasks_[task.id] = task;
        return task;
    }

    std::optional<Task> get_task(int64_t id) {
        std::lock_guard lock(mutex_);
        auto it = tasks_.find(id);
        if (it == tasks_.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    std::vector<Task> list_tasks(const std::optional<std::string>& status,
                                 const std::optional<int64_t>& priority,
                                 int64_t limit,
                                 int64_t offset) {
        std::lock_guard lock(mutex_);
        std::vector<Task> result;

        for (const auto& [id, task] : tasks_) {
            if (status && task.status != *status) {
                continue;
            }
            if (priority && task.priority != *priority) {
                continue;
            }
            result.push_back(task);
        }

        // Apply offset and limit
        if (offset > 0 && static_cast<size_t>(offset) < result.size()) {
            result.erase(result.begin(), result.begin() + offset);
        } else if (offset > 0) {
            result.clear();
        }

        if (limit > 0 && static_cast<size_t>(limit) < result.size()) {
            result.resize(limit);
        }

        return result;
    }

    std::optional<Task> update_task(int64_t id, const UpdateTaskRequest& req) {
        std::lock_guard lock(mutex_);
        auto it = tasks_.find(id);
        if (it == tasks_.end()) {
            return std::nullopt;
        }

        Task& task = it->second;

        if (req.title) {
            task.title = std::string(*req.title);
        }
        if (req.description) {
            task.description = std::string(*req.description);
        }
        if (req.status) {
            task.status = std::string(*req.status);
        }
        if (req.priority) {
            task.priority = *req.priority;
        }
        if (req.tags) {
            task.tags = std::vector<std::string>();
            for (const auto& tag : *req.tags) {
                task.tags->push_back(std::string(tag));
            }
        }
        if (req.assignee_id) {
            User user;
            user.id = *req.assignee_id;
            user.email = std::string("user@example.com");
            user.name = std::string("User " + std::to_string(*req.assignee_id));
            task.assignee = user;
        }

        task.updated_at = std::string("2026-02-09T12:00:00Z");

        return task;
    }

    bool delete_task(int64_t id) {
        std::lock_guard lock(mutex_);
        return tasks_.erase(id) > 0;
    }

    size_t count_tasks() {
        std::lock_guard lock(mutex_);
        return tasks_.size();
    }

    std::vector<Task> search_tasks(const SearchRequest& req) {
        std::lock_guard lock(mutex_);
        std::vector<Task> result;

        for (const auto& [id, task] : tasks_) {
            bool match = true;

            if (req.title_contains && task.title.find(*req.title_contains) == std::string::npos) {
                match = false;
            }

            if (match && req.statuses) {
                bool status_match = false;
                for (const auto& status : *req.statuses) {
                    if (task.status == status) {
                        status_match = true;
                        break;
                    }
                }
                if (!status_match) {
                    match = false;
                }
            }

            if (match && req.min_priority && task.priority < *req.min_priority) {
                match = false;
            }

            if (match && req.max_priority && task.priority > *req.max_priority) {
                match = false;
            }

            if (match) {
                result.push_back(task);
            }
        }

        return result;
    }

    int64_t uptime_seconds() {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(now - start_time_).count();
    }

    void increment_requests() { total_requests_.fetch_add(1, std::memory_order_relaxed); }

    int64_t total_requests() const { return total_requests_.load(std::memory_order_relaxed); }

private:
    std::mutex mutex_;
    std::map<int64_t, Task> tasks_;
    int64_t next_id_;
    std::chrono::steady_clock::time_point start_time_;
    std::atomic<int64_t> total_requests_;
};

// Implementation of the API handlers
class TaskAPIImpl : public TaskAPI {
public:
    explicit TaskAPIImpl(TaskStorage& storage) : storage_(storage) {}

    response listTasks(const listTasksRequest& req, request_context& ctx) override {
        storage_.increment_requests();

        auto tasks = storage_.list_tasks(
            req.status, req.priority, req.limit.value_or(10), req.offset.value_or(0));

        TaskList list;
        list.tasks = tasks;
        list.total = static_cast<int64_t>(storage_.count_tasks());
        list.has_more = (req.offset.value_or(0) + static_cast<int64_t>(tasks.size())) < list.total;

        return serialize_TaskList(list, ctx.arena);
    }

    response createTask(const CreateTaskRequest& req, request_context& ctx) override {
        storage_.increment_requests();

        auto task = storage_.create_task(req, ctx.arena);
        return serialize_Task(task, ctx.arena, 201);
    }

    response getTask(const getTaskRequest& req, request_context& ctx) override {
        storage_.increment_requests();

        auto task = storage_.get_task(req.id);
        if (!task) {
            return response::error(problem_details::not_found(
                "Task not found", "task.not_found", "Task with given ID does not exist"));
        }

        return serialize_Task(*task, ctx.arena);
    }

    response updateTask(const updateTaskRequest& req, request_context& ctx) override {
        storage_.increment_requests();

        auto task = storage_.update_task(req.id, req.body);
        if (!task) {
            return response::error(problem_details::not_found(
                "Task not found", "task.not_found", "Task with given ID does not exist"));
        }

        return serialize_Task(*task, ctx.arena);
    }

    response deleteTask(const deleteTaskRequest& req, request_context& ctx) override {
        storage_.increment_requests();

        if (!storage_.delete_task(req.id)) {
            return response::error(problem_details::not_found(
                "Task not found", "task.not_found", "Task with given ID does not exist"));
        }

        return response(204, {}, "");
    }

    response batchCreateTasks(const BatchCreateRequest& req, request_context& ctx) override {
        storage_.increment_requests();

        BatchCreateResponse resp;
        resp.created = std::vector<Task>();
        resp.failed = std::vector<batchCreateTasks_failed_item>();

        for (size_t i = 0; i < req.tasks.size(); ++i) {
            try {
                auto task = storage_.create_task(req.tasks[i], ctx.arena);
                resp.created->push_back(task);
            } catch (const std::exception& e) {
                batchCreateTasks_failed_item failed;
                failed.index = static_cast<int64_t>(i);
                failed.error = std::string(e.what());
                resp.failed->push_back(failed);
            }
        }

        return serialize_BatchCreateResponse(resp, ctx.arena);
    }

    response searchTasks(const SearchRequest& req, request_context& ctx) override {
        storage_.increment_requests();

        auto tasks = storage_.search_tasks(req);

        TaskList list;
        list.tasks = tasks;
        list.total = static_cast<int64_t>(tasks.size());
        list.has_more = false;

        return serialize_TaskList(list, ctx.arena);
    }

    response healthCheck(const healthCheckRequest&, request_context& ctx) override {
        storage_.increment_requests();

        HealthResponse health;
        health.status = std::string("healthy");
        health.timestamp = std::string("2026-02-09T12:00:00Z");
        health.uptime_seconds = storage_.uptime_seconds();
        health.total_requests = storage_.total_requests();

        return serialize_HealthResponse(health, ctx.arena);
    }

private:
    TaskStorage& storage_;
};

int main(int argc, char* argv[]) {
    uint16_t port = 18081;
    if (argc > 1) {
        port = static_cast<uint16_t>(std::atoi(argv[1]));
    }

    TaskStorage storage;
    TaskAPIImpl api_impl(storage);

    std::cout << "Task API server starting on port " << port << "...\n";
    std::cout << "Endpoints:\n";
    std::cout << "  GET    /tasks          - List tasks\n";
    std::cout << "  POST   /tasks          - Create task\n";
    std::cout << "  GET    /tasks/{id}     - Get task\n";
    std::cout << "  PUT    /tasks/{id}     - Update task\n";
    std::cout << "  DELETE /tasks/{id}     - Delete task\n";
    std::cout << "  POST   /tasks/batch    - Batch create\n";
    std::cout << "  POST   /tasks/search   - Search tasks\n";
    std::cout << "  GET    /health         - Health check\n";
    std::cout << std::flush;

    auto handler = create_TaskAPI_handler(api_impl);

    try {
        // Run with reactor pool
        reactor_pool pool;
        pool.run_server(port, handler);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
