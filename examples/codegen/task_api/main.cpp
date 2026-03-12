#include "generated/generated_dtos.hpp"
#include "generated/generated_handlers.hpp"
#include "generated/generated_json.hpp"
#include "generated/generated_router_bindings.hpp"
#include "generated/generated_routes.hpp"
#include "generated/generated_validators.hpp"

#include "katana/core/handler_context.hpp"
#include "katana/core/http_server.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

using namespace katana;
using namespace katana::http;

namespace {

struct stored_task {
    int64_t id = 0;
    std::string title;
    std::string description;
    Task_Status_t_enum status = Task_Status_t_enum::pending;
    int64_t priority = 1;
    std::vector<std::string> tags;
    std::optional<int64_t> assignee_id;
    std::optional<std::string> due_date;
    std::string created_at;
    std::optional<std::string> updated_at;
};

Task_Status_t_enum update_status_to_task_status(UpdateTaskRequest_Status_t_enum status) {
    switch (status) {
    case UpdateTaskRequest_Status_t_enum::pending:
        return Task_Status_t_enum::pending;
    case UpdateTaskRequest_Status_t_enum::in_progress:
        return Task_Status_t_enum::in_progress;
    case UpdateTaskRequest_Status_t_enum::completed:
        return Task_Status_t_enum::completed;
    case UpdateTaskRequest_Status_t_enum::cancelled:
        return Task_Status_t_enum::cancelled;
    }
    return Task_Status_t_enum::pending;
}

Task_Status_t_enum search_status_to_task_status(SearchRequest_Item_t_enum status) {
    switch (status) {
    case SearchRequest_Item_t_enum::pending:
        return Task_Status_t_enum::pending;
    case SearchRequest_Item_t_enum::in_progress:
        return Task_Status_t_enum::in_progress;
    case SearchRequest_Item_t_enum::completed:
        return Task_Status_t_enum::completed;
    case SearchRequest_Item_t_enum::cancelled:
        return Task_Status_t_enum::cancelled;
    }
    return Task_Status_t_enum::pending;
}

std::string fixed_timestamp() {
    return "2026-02-22T00:00:00Z";
}

Task to_dto(const stored_task& src, monotonic_arena& arena) {
    Task task(&arena);
    task.id = src.id;
    task.title = arena_string<>(src.title, arena_allocator<char>(&arena));
    if (!src.description.empty()) {
        task.description = arena_string<>(src.description, arena_allocator<char>(&arena));
    }
    task.status = src.status;
    task.priority = src.priority;
    if (!src.tags.empty()) {
        task.tags.emplace(arena_allocator<arena_string<>>(&arena));
        for (const auto& tag : src.tags) {
            task.tags->emplace_back(tag, arena_allocator<char>(&arena));
        }
    }

    if (src.assignee_id) {
        task.assignee.emplace(&arena);
        task.assignee->id = *src.assignee_id;
        task.assignee->email = arena_string<>("user@example.com", arena_allocator<char>(&arena));
        std::string name = "User " + std::to_string(*src.assignee_id);
        task.assignee->name = arena_string<>(name, arena_allocator<char>(&arena));
    }

    if (src.due_date) {
        task.due_date = arena_string<>(*src.due_date, arena_allocator<char>(&arena));
    }
    task.created_at = arena_string<>(src.created_at, arena_allocator<char>(&arena));
    if (src.updated_at) {
        task.updated_at = arena_string<>(*src.updated_at, arena_allocator<char>(&arena));
    }
    return task;
}

class task_storage {
public:
    task_storage() : start_time_(std::chrono::steady_clock::now()) {}

    stored_task create(const CreateTaskRequest& req) {
        std::lock_guard lock(mu_);

        stored_task task;
        task.id = next_id_++;
        task.title.assign(req.title.data(), req.title.size());
        if (req.description && !req.description->empty()) {
            task.description.assign(req.description->data(), req.description->size());
        }
        task.status = Task_Status_t_enum::pending;
        task.priority = req.priority;
        if (req.tags) {
            for (const auto& tag : *req.tags) {
                task.tags.emplace_back(tag.data(), tag.size());
            }
        }
        if (req.assignee_id) {
            task.assignee_id = *req.assignee_id;
        }
        if (req.due_date) {
            task.due_date = std::string(req.due_date->data(), req.due_date->size());
        }
        task.created_at = fixed_timestamp();

        auto id = task.id;
        tasks_[id] = task;
        return task;
    }

    std::optional<stored_task> get(int64_t id) const {
        std::lock_guard lock(mu_);
        auto it = tasks_.find(id);
        if (it == tasks_.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    bool remove(int64_t id) {
        std::lock_guard lock(mu_);
        return tasks_.erase(id) > 0;
    }

    std::optional<stored_task> update(int64_t id, const UpdateTaskRequest& req) {
        std::lock_guard lock(mu_);
        auto it = tasks_.find(id);
        if (it == tasks_.end()) {
            return std::nullopt;
        }

        auto& task = it->second;
        if (req.title) {
            task.title.assign(req.title->data(), req.title->size());
        }
        if (req.description) {
            task.description.assign(req.description->data(), req.description->size());
        }
        if (req.priority) {
            task.priority = *req.priority;
        }
        if (req.tags) {
            task.tags.clear();
            for (const auto& tag : *req.tags) {
                task.tags.emplace_back(tag.data(), tag.size());
            }
        }

        if (req.status) {
            task.status = update_status_to_task_status(*req.status);
        }
        if (req.assignee_id) {
            task.assignee_id = *req.assignee_id;
        }
        if (req.due_date) {
            task.due_date = std::string(req.due_date->data(), req.due_date->size());
        }
        task.updated_at = fixed_timestamp();
        return task;
    }

    std::vector<stored_task> list(std::optional<std::string_view> status,
                                  std::optional<int64_t> priority,
                                  int64_t limit,
                                  int64_t offset) const {
        std::lock_guard lock(mu_);
        std::vector<stored_task> result;
        result.reserve(tasks_.size());

        std::optional<Task_Status_t_enum> expected_status;
        if (status) {
            expected_status = Task_Status_t_enum_from_string(*status);
        }

        for (const auto& [_, task] : tasks_) {
            if (expected_status && task.status != *expected_status) {
                continue;
            }
            if (priority && task.priority != *priority) {
                continue;
            }
            result.push_back(task);
        }

        if (offset > 0) {
            const auto off = static_cast<size_t>(offset);
            if (off >= result.size()) {
                return {};
            }
            result.erase(result.begin(), result.begin() + static_cast<std::ptrdiff_t>(off));
        }
        if (limit > 0 && static_cast<size_t>(limit) < result.size()) {
            result.resize(static_cast<size_t>(limit));
        }
        return result;
    }

    std::vector<stored_task> search(const SearchRequest& req) const {
        std::lock_guard lock(mu_);
        std::vector<stored_task> result;
        result.reserve(tasks_.size());

        for (const auto& [_, task] : tasks_) {
            if (req.title_contains) {
                std::string needle(req.title_contains->data(), req.title_contains->size());
                if (task.title.find(needle) == std::string::npos) {
                    continue;
                }
            }
            if (req.statuses && !req.statuses->empty()) {
                bool status_match = false;
                for (auto s : *req.statuses) {
                    if (task.status == search_status_to_task_status(s)) {
                        status_match = true;
                        break;
                    }
                }
                if (!status_match) {
                    continue;
                }
            }
            if (req.min_priority && task.priority < *req.min_priority) {
                continue;
            }
            if (req.max_priority && task.priority > *req.max_priority) {
                continue;
            }
            if (req.tags && !req.tags->empty()) {
                bool tag_match = false;
                for (const auto& filter_tag : *req.tags) {
                    auto it = std::find(task.tags.begin(),
                                        task.tags.end(),
                                        std::string(filter_tag.data(), filter_tag.size()));
                    if (it != task.tags.end()) {
                        tag_match = true;
                        break;
                    }
                }
                if (!tag_match) {
                    continue;
                }
            }
            if (req.has_assignee.value_or(false) && !task.assignee_id) {
                continue;
            }
            result.push_back(task);
        }
        return result;
    }

    int64_t count() const {
        std::lock_guard lock(mu_);
        return static_cast<int64_t>(tasks_.size());
    }

    int64_t uptime_seconds() const {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(now - start_time_).count();
    }

private:
    mutable std::mutex mu_;
    std::map<int64_t, stored_task> tasks_;
    int64_t next_id_ = 1;
    std::chrono::steady_clock::time_point start_time_;
};

class task_handler : public generated::api_handler {
public:
    explicit task_handler(task_storage& storage) : storage_(storage) {}

    result<void> list_tasks(std::optional<std::string_view> status,
                            std::optional<int64_t> priority,
                            std::optional<int64_t> limit,
                            std::optional<int64_t> offset,
                            response& out) override {
        request_count_.fetch_add(1, std::memory_order_relaxed);
        auto tasks = storage_.list(status, priority, limit.value_or(10), offset.value_or(0));

        auto& arena = handler_context::arena();
        TaskList list(&arena);
        for (const auto& task : tasks) {
            list.tasks.push_back(to_dto(task, arena));
        }
        list.total = storage_.count();
        list.has_more = offset.value_or(0) + static_cast<int64_t>(tasks.size()) < list.total;
        out = response::json(serialize_TaskList(list));
        return {};
    }

    result<void> create_task(const CreateTaskRequest& body, response& out) override {
        request_count_.fetch_add(1, std::memory_order_relaxed);
        auto task = storage_.create(body);
        auto& arena = handler_context::arena();
        respond::into(out).created_json(serialize_Task(to_dto(task, arena)));
        return {};
    }

    result<void> get_task(int64_t id, response& out) override {
        request_count_.fetch_add(1, std::memory_order_relaxed);
        auto task = storage_.get(id);
        if (!task) {
            out = response::error(problem_details::not_found("Task not found"));
            return {};
        }
        auto& arena = handler_context::arena();
        out = response::json(serialize_Task(to_dto(*task, arena)));
        return {};
    }

    result<void> update_task(int64_t id, const UpdateTaskRequest& body, response& out) override {
        request_count_.fetch_add(1, std::memory_order_relaxed);
        auto task = storage_.update(id, body);
        if (!task) {
            out = response::error(problem_details::not_found("Task not found"));
            return {};
        }
        auto& arena = handler_context::arena();
        out = response::json(serialize_Task(to_dto(*task, arena)));
        return {};
    }

    result<void> delete_task(int64_t id, response& out) override {
        request_count_.fetch_add(1, std::memory_order_relaxed);
        if (!storage_.remove(id)) {
            out = response::error(problem_details::not_found("Task not found"));
            return {};
        }
        respond::into(out).no_content();
        return {};
    }

    result<void> batch_create_tasks(const BatchCreateRequest& body, response& out) override {
        request_count_.fetch_add(1, std::memory_order_relaxed);
        auto& arena = handler_context::arena();
        BatchCreateResponse batch(&arena);

        for (size_t i = 0; i < body.tasks.size(); ++i) {
            try {
                auto created = storage_.create(body.tasks[i]);
                batch.created.push_back(to_dto(created, arena));
            } catch (const std::exception& e) {
                BatchCreateResponse_Item_t_1 failed(&arena);
                failed.index = static_cast<int64_t>(i);
                failed.error = arena_string<>(e.what(), arena_allocator<char>(&arena));
                batch.failed.push_back(std::move(failed));
            }
        }
        out = response::json(serialize_BatchCreateResponse(batch));
        return {};
    }

    result<void> search_tasks(const SearchRequest& body, response& out) override {
        request_count_.fetch_add(1, std::memory_order_relaxed);
        auto tasks = storage_.search(body);
        auto& arena = handler_context::arena();
        TaskList list(&arena);
        list.total = static_cast<int64_t>(tasks.size());
        list.has_more = false;
        for (const auto& task : tasks) {
            list.tasks.push_back(to_dto(task, arena));
        }
        out = response::json(serialize_TaskList(list));
        return {};
    }

    result<void> health_check(response& out) override {
        request_count_.fetch_add(1, std::memory_order_relaxed);
        auto& arena = handler_context::arena();
        HealthResponse health(&arena);
        health.status = HealthResponse_Status_t_enum::healthy;
        health.timestamp = arena_string<>(fixed_timestamp(), arena_allocator<char>(&arena));
        health.uptime_seconds = storage_.uptime_seconds();
        health.total_requests = request_count_.load(std::memory_order_relaxed);
        out = response::json(serialize_HealthResponse(health));
        return {};
    }

private:
    task_storage& storage_;
    std::atomic<int64_t> request_count_{0};
};

uint16_t read_port(int argc, char* argv[]) {
    if (argc > 1) {
        int parsed = std::atoi(argv[1]);
        if (parsed > 0 && parsed < 65536) {
            return static_cast<uint16_t>(parsed);
        }
    }
    if (const char* env_port = std::getenv("PORT")) {
        int parsed = std::atoi(env_port);
        if (parsed > 0 && parsed < 65536) {
            return static_cast<uint16_t>(parsed);
        }
    }
    return 18081;
}

uint16_t worker_count() {
    const uint32_t hw = std::max(1u, std::thread::hardware_concurrency());
    const uint32_t capped = std::min<uint32_t>(hw, 32);
    return static_cast<uint16_t>(capped);
}

} // namespace

int main(int argc, char* argv[]) {
    const uint16_t port = read_port(argc, argv);
    const uint16_t workers = worker_count();

    task_storage storage;
    task_handler handler(storage);
    const auto& router = generated::make_router(handler);

    return http::server(router)
        .listen(port)
        .workers(workers)
        .on_start([&]() {
            std::cout << "Task API running on :" << port << " with " << workers << " workers"
                      << std::endl;
        })
        .run();
}
