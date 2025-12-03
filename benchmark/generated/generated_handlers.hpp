#pragma once

#include "generated_dtos.hpp"
#include "katana/core/http.hpp"
#include "katana/core/router.hpp"
#include <optional>
#include <string_view>

using katana::http::request;
using katana::http::request_context;
using katana::http::response;

namespace generated {

// Base handler interface for all API operations
struct api_handler {
    virtual ~api_handler() = default;

    // GET /health
    virtual response health(const request& req, request_context& ctx) = 0;

    // GET /users
    virtual response list_users(const request& req, request_context& ctx) = 0;

    // POST /users
    virtual response
    create_user(const request& req, request_context& ctx, const UserInput& body) = 0;

    // GET /users/{id}
    virtual response get_user(const request& req, request_context& ctx, std::string_view id) = 0;

    // PUT /users/{id}
    virtual response update_user(const request& req,
                                 request_context& ctx,
                                 std::string_view id,
                                 const UserInput& body) = 0;
};

} // namespace generated
