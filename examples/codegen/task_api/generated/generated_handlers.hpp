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

#include "generated_dtos.hpp"
#include "katana/core/http.hpp"
#include "katana/core/router.hpp"
#include <optional>
#include <string_view>
#include <variant>

using katana::http::request;
using katana::http::request_context;
using katana::http::response;

namespace generated {

// Base handler interface for all API operations
// Implement these methods to handle requests - validation is automatic!
struct api_handler {
    virtual ~api_handler() = default;

    // GET /tasks
    // List all tasks with optional filtering
    virtual katana::result<void> list_tasks(std::optional<std::string_view> status,
                                            std::optional<int64_t> priority,
                                            std::optional<int64_t> limit,
                                            std::optional<int64_t> offset,
                                            response& out) = 0;

    // POST /tasks
    // Create a new task
    virtual katana::result<void> create_task(const CreateTaskRequest& body, response& out) = 0;

    // GET /tasks/{id}
    // Get a specific task by ID
    virtual katana::result<void> get_task(int64_t id, response& out) = 0;

    // PUT /tasks/{id}
    // Update a task
    virtual katana::result<void>
    update_task(int64_t id, const UpdateTaskRequest& body, response& out) = 0;

    // DELETE /tasks/{id}
    // Delete a task
    virtual katana::result<void> delete_task(int64_t id, response& out) = 0;

    // POST /tasks/batch
    // Create multiple tasks in a single request
    virtual katana::result<void> batch_create_tasks(const BatchCreateRequest& body,
                                                    response& out) = 0;

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
//             out.assign_error(katana::problem_details::internal_server_error("failed to create
//             item")); return {};
//         }
//         respond::into(out).created_json(serialize_item(*item));
//         return {};
//     }
//
//     // Example 4: Enum handling
//     katana::result<void> transform_text(const text_transform_request& req, response& out)
//     override {
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
