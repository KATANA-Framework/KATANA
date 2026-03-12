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

    // POST /compute/sum
    // Sum array of numbers
    virtual katana::result<void> compute_sum(const SumRequest& body, response& out) = 0;

    // POST /compute/stats
    // Compute statistics for array of numbers
    virtual katana::result<void> compute_stats(const StatsRequest& body, response& out) = 0;

    // POST /users/register
    // Register user with strict validation
    virtual katana::result<void> register_user(const RegisterRequest& body, response& out) = 0;

    // GET /items
    // List items with pagination
    virtual katana::result<void> list_items(std::optional<int64_t> limit, std::optional<int64_t> offset, std::optional<std::string_view> category, response& out) = 0;

    // POST /items
    // Create a new item
    virtual katana::result<void> create_item(std::string_view X_Request_Id, std::optional<std::string_view> session, const CreateItemRequest& body, response& out) = 0;

    // GET /items/{id}
    // Get item by ID
    virtual katana::result<void> get_item(int64_t id, response& out) = 0;

    // PUT /items/{id}
    // Update item
    virtual katana::result<void> update_item(int64_t id, const UpdateItemRequest& body, response& out) = 0;

    // DELETE /items/{id}
    // Delete item
    virtual katana::result<void> delete_item(int64_t id, response& out) = 0;

    // POST /echo
    // Echo back the request body
    virtual katana::result<void> echo(const EchoRequest& body, response& out) = 0;

    // GET /health
    // Health check
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
//     katana::result<void> compute_sum(const SumRequest& body, response& out) override {
//         // Access request fields
//         auto input = body.text;
//
//         // Create response using arena allocator
//         SumResponse resp(&katana::http::arena());
//
//         // Process and set response fields
//         // resp.result = ...your logic here...
//
//         // Serialize into the provided response
//         respond::into(out).json(serialize_SumResponse(resp));
//         return {};
//     }
//
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
