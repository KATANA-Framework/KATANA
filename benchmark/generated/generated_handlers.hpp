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

    // GET /health
    virtual response health() = 0;

    // GET /users
    virtual response list_users() = 0;

    // POST /users
    virtual response create_user(const UserInput& body) = 0;

    // GET /users/{id}
    virtual response get_user(int64_t id) = 0;

    // PUT /users/{id}
    virtual response update_user(int64_t id, const UserInput& body) = 0;
};

} // namespace generated
