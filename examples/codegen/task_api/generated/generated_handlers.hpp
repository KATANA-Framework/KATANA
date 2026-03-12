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
//       out = response::json(serialize_User(user));
//       return {};
//   }
#pragma once

#include "katana/core/http.hpp"
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
//     response handle_request(const some_request& req) override {
//         if (req.value < 0) {
//             return response::bad_request("value must be positive");
//         }
//         // ... normal processing ...
//         return response::json(serialize_some_response(resp));
//     }
//
//     // Example 3: Different response status codes
//     response create_item(const create_request& req) override {
//         auto item = db.create(req, &katana::http::arena());
//         if (!item) {
//             return response::internal_error("failed to create item");
//         }
//         return response::created(serialize_item(*item));
//     }
//
//     // Example 4: Enum handling
//     response transform_text(const text_transform_request& req) override {
//         std::string result;
//         switch (req.operation) {
//             case text_transform_operation::upper:
//                 result = to_upper(req.text);
//                 break;
//             case text_transform_operation::lower:
//                 result = to_lower(req.text);
//                 break;
//             // ... other cases ...
//         }
//         text_transform_response resp(&katana::http::arena());
//         resp.result = result;
//         return response::json(serialize_text_transform_response(resp));
//     }
// };
//
// Available response helpers:
//   - respond::into(out).text(...)
//   - respond::into(out).json(...)
//   - respond::into(out).created_json(...)
//   - respond::into(out).no_content()
//   - out = response::bad_request(message)
//   - out = response::unauthorized(message)
//   - out = response::forbidden(message)
//   - out = response::not_found(message)
//   - out = response::internal_error(message)
//
// Context access functions (available in handler methods):
//   - katana::http::req()    - Get current request
//   - katana::http::ctx()    - Get request context
//   - katana::http::arena()  - Get arena allocator for zero-copy strings
//
} // namespace generated
