// layer: flat
// Auto-generated handler interfaces from OpenAPI specification
//
// Zero-boilerplate design:
//   - Clean signatures: response method(params) - no request& or context&
//   - Automatic validation: schema constraints checked before handler call
//   - Auto parameter binding: path/query/header/body → typed arguments
//   - Context access: use katana::http::req(), ctx(), arena() for access
//
// Example:
//   response get_user(int64_t id) override {
//       auto user = db.find(id, &arena());  // arena() from context
//       return response::json(serialize_User(user));
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

    // POST /compute/sum
    // Sum array of numbers
    virtual response compute_sum(const SumRequest& body) = 0;

    // POST /compute/stats
    // Compute statistics for array of numbers
    virtual response compute_stats(const StatsRequest& body) = 0;

    // POST /users/register
    // Register user with strict validation
    virtual response register_user(const RegisterRequest& body) = 0;

    // GET /items
    // List items with pagination
    virtual response list_items(std::optional<int64_t> limit,
                                std::optional<int64_t> offset,
                                std::optional<std::string_view> category) = 0;

    // POST /items
    // Create a new item
    virtual response create_item(std::string_view X_Request_Id,
                                 std::optional<std::string_view> session,
                                 const CreateItemRequest& body) = 0;

    // GET /items/{id}
    // Get item by ID
    virtual response get_item(int64_t id) = 0;

    // PUT /items/{id}
    // Update item
    virtual response update_item(int64_t id, const UpdateItemRequest& body) = 0;

    // DELETE /items/{id}
    // Delete item
    virtual response delete_item(int64_t id) = 0;

    // POST /echo
    // Echo back the request body
    virtual response echo(const EchoRequest& body) = 0;

    // GET /health
    // Health check
    virtual response health_check() = 0;
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
//     response compute_sum(const SumRequest& req) override {
//         // Access request fields
//         auto input = req.text;
//
//         // Create response using arena allocator
//         SumResponse resp(&katana::http::arena());
//
//         // Process and set response fields
//         // resp.result = ...your logic here...
//
//         // Serialize and return
//         return response::json(serialize_SumResponse(resp));
//     }
//
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
//   - response::ok(body, content_type = "text/plain")
//   - response::json(json_string)
//   - response::created(body, location = "")
//   - response::no_content()
//   - response::bad_request(message)
//   - response::unauthorized(message)
//   - response::forbidden(message)
//   - response::not_found(message)
//   - response::internal_error(message)
//
// Context access functions (available in handler methods):
//   - katana::http::req()    - Get current request
//   - katana::http::ctx()    - Get request context
//   - katana::http::arena()  - Get arena allocator for zero-copy strings
//
} // namespace generated
