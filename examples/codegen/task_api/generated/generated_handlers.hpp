// layer: flat
// Auto-generated handler interfaces from OpenAPI specification
// 
// Zero-boilerplate design:
//   - Clean signatures: result<void> method(params, response& out)
//   - Automatic validation: schema constraints checked before handler call
//   - Auto parameter binding: path/query/header/body → typed arguments
//   - Context access: use katana::http::req(), ctx(), arena() for access
// 
// Example:
//   katana::result<void> get_user(int64_t id, response& out) override {
//       auto user = db.find(id, &arena());  // arena() from context
//       respond::into(out).json(serialize_User(user));
//       return {};
//   }
#pragma once

#include "katana/core/http.hpp"
#include "katana/core/problem.hpp"
#include "katana/core/router.hpp"
#include "generated_dtos.hpp"
#include <string_view>
#include <optional>
#include <variant>

using katana::http::request;
using katana::http::response;
using katana::http::request_context;

namespace generated {

// Base handler interface for all API operations
// Implement these methods to handle requests - validation is automatic!
struct api_handler {
    virtual ~api_handler() = default;

    // GET /tasks
    // List all tasks with optional filtering
    virtual katana::result<void> list_tasks(std::optional<std::string_view> status, std::optional<int64_t> priority, std::optional<int64_t> limit, std::optional<int64_t> offset, response& out) = 0;

    // POST /tasks
    // Create a new task
    virtual katana::result<void> create_task(const CreateTaskRequest& body, response& out) = 0;

    // GET /tasks/{id}
    // Get a specific task by ID
    virtual katana::result<void> get_task(int64_t id, response& out) = 0;

    // PUT /tasks/{id}
    // Update a task
    virtual katana::result<void> update_task(int64_t id, const UpdateTaskRequest& body, response& out) = 0;

    // DELETE /tasks/{id}
    // Delete a task
    virtual katana::result<void> delete_task(int64_t id, response& out) = 0;

    // POST /tasks/batch
    // Create multiple tasks in a single request
    virtual katana::result<void> batch_create_tasks(const BatchCreateRequest& body, response& out) = 0;

    // POST /tasks/search
    // Complex task search with multiple criteria
    virtual katana::result<void> search_tasks(const SearchRequest& body, response& out) = 0;

    // GET /health
    // Health check endpoint
    virtual katana::result<void> health_check(response& out) = 0;

};

// Optional async handler interface for generated routers.
// Implement only operations that should own deferred HTTP completion.
// Returning false falls back to the synchronous api_handler method.
struct async_api_handler {
    virtual ~async_api_handler() = default;

    virtual bool list_tasks_async(std::optional<std::string_view> status, std::optional<int64_t> priority, std::optional<int64_t> limit, std::optional<int64_t> offset, katana::http::async_response_writer out) {
        (void)status;
        (void)priority;
        (void)limit;
        (void)offset;
        (void)out;
        return false;
    }

    virtual bool create_task_async(const CreateTaskRequest& body, katana::http::async_response_writer out) {
        (void)body;
        (void)out;
        return false;
    }

    virtual bool get_task_async(int64_t id, katana::http::async_response_writer out) {
        (void)id;
        (void)out;
        return false;
    }

    virtual bool update_task_async(int64_t id, const UpdateTaskRequest& body, katana::http::async_response_writer out) {
        (void)id;
        (void)body;
        (void)out;
        return false;
    }

    virtual bool delete_task_async(int64_t id, katana::http::async_response_writer out) {
        (void)id;
        (void)out;
        return false;
    }

    virtual bool batch_create_tasks_async(const BatchCreateRequest& body, katana::http::async_response_writer out) {
        (void)body;
        (void)out;
        return false;
    }

    virtual bool search_tasks_async(const SearchRequest& body, katana::http::async_response_writer out) {
        (void)body;
        (void)out;
        return false;
    }

    virtual bool health_check_async(katana::http::async_response_writer out) {
        (void)out;
        return false;
    }

};

// Convenience base for async-first services.
// Override *_async methods only; synchronous fallbacks return 501.
struct async_api_handler_base : api_handler, async_api_handler {
    virtual ~async_api_handler_base() = default;

    katana::result<void> list_tasks(std::optional<std::string_view> status, std::optional<int64_t> priority, std::optional<int64_t> limit, std::optional<int64_t> offset, response& out) override {
        (void)status;
        (void)priority;
        (void)limit;
        (void)offset;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("list_tasks requires an async override or sync implementation"));
        return {};
    }

    katana::result<void> create_task(const CreateTaskRequest& body, response& out) override {
        (void)body;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("create_task requires an async override or sync implementation"));
        return {};
    }

    katana::result<void> get_task(int64_t id, response& out) override {
        (void)id;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("get_task requires an async override or sync implementation"));
        return {};
    }

    katana::result<void> update_task(int64_t id, const UpdateTaskRequest& body, response& out) override {
        (void)id;
        (void)body;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("update_task requires an async override or sync implementation"));
        return {};
    }

    katana::result<void> delete_task(int64_t id, response& out) override {
        (void)id;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("delete_task requires an async override or sync implementation"));
        return {};
    }

    katana::result<void> batch_create_tasks(const BatchCreateRequest& body, response& out) override {
        (void)body;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("batch_create_tasks requires an async override or sync implementation"));
        return {};
    }

    katana::result<void> search_tasks(const SearchRequest& body, response& out) override {
        (void)body;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("search_tasks requires an async override or sync implementation"));
        return {};
    }

    katana::result<void> health_check(response& out) override {
        out = katana::http::response::error(
            katana::problem_details::not_implemented("health_check requires an async override or sync implementation"));
        return {};
    }

};

// ============================================================================
// USAGE EXAMPLES
// ============================================================================
//
// Example implementation of api_handler:
//
// class my_api : public generated::api_handler {
// public:
//     // Example 1: Simple request/response with arena allocator
//     // Example 2: Error handling
//     katana::result<void> handle_request(const some_request& req, response& out) override {
//         if (req.value < 0) {
//             out.assign_error(katana::problem_details::bad_request("value must be positive"));
//             return {};
//         }
//         // ... normal processing ...
//         respond::into(out).json(serialize_some_response(resp));
//         return {};
//     }
//
//     // Example 3: Different response status codes
//     katana::result<void> create_item(const create_request& req, response& out) override {
//         auto item = db.create(req, &katana::http::arena());
//         if (!item) {
//             out.assign_error(katana::problem_details::internal_server_error("failed to create item"));
//             return {};
//         }
//         respond::into(out).created_json(serialize_item(*item));
//         return {};
//     }
//
//     // Example 4: Enum handling
//     katana::result<void> transform_text(const text_transform_request& req, response& out) override {
//         std::string result;
//         switch (req.operation) {
//             case text_transform_request_operation_enum::upper:
//                 result = to_upper(req.text);
//                 break;
//             case text_transform_request_operation_enum::lower:
//                 result = to_lower(req.text);
//                 break;
//             // ... other cases ...
//         }
//         text_transform_response resp(&katana::http::arena());
//         resp.result = result;
//         respond::into(out).json(serialize_text_transform_response(resp));
//         return {};
//     }
// };
//
// Available response helpers:
//   - respond::into(out).text(...)
//   - respond::into(out).json(...)
//   - respond::into(out).created_json(...)
//   - respond::into(out).no_content()
//   - out.assign_error(katana::problem_details::bad_request(message))
//   - out.assign_error(katana::problem_details::unauthorized(message))
//   - out.assign_error(katana::problem_details::forbidden(message))
//   - out.assign_error(katana::problem_details::not_found(message))
//   - out.assign_error(katana::problem_details::internal_server_error(message))
//
// Context access functions (available in handler methods):
//   - katana::http::req()    - Get current request
//   - katana::http::ctx()    - Get request context
//   - katana::http::arena()  - Get arena allocator for zero-copy strings
//
} // namespace generated
