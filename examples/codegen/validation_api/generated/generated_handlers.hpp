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

    // POST /user/register
    // Register user with strict validation
    virtual response register_user(const RegisterUserRequest& body) = 0;
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
//     response register_user(const RegisterUserRequest& req) override {
//         // Access request fields
//         auto input = req.text;
//
//         // Create response using arena allocator
//         register_user_response resp(&katana::http::arena());
//
//         // Process and set response fields
//         // resp.result = ...your logic here...
//
//         // Serialize and return
//         return response::json(serialize_register_user_response(resp));
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
