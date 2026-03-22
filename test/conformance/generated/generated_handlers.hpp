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

    // GET /pets
    virtual katana::result<void> list_pets(std::optional<int64_t> limit, std::string_view X_Trace, std::optional<std::string_view> session, response& out) = 0;

    // POST /pets
    virtual katana::result<void> create_pet(const PetCreateRequest& body, response& out) = 0;

    // GET /pets/{petId}
    virtual katana::result<void> get_pet(int64_t petId, response& out) = 0;

    // DELETE /pets/{petId}
    virtual katana::result<void> delete_pet(int64_t petId, response& out) = 0;

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
