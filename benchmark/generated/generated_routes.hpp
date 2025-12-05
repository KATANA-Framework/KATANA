// layer: flat
#pragma once

#include "katana/core/http.hpp"
#include "katana/core/router.hpp"
#include <array>
#include <span>
#include <string_view>

namespace generated {

struct content_type_info {
    std::string_view mime_type;
};

struct route_entry {
    std::string_view path;
    katana::http::method method;
    std::string_view operation_id;
    std::span<const content_type_info> consumes;
    std::span<const content_type_info> produces;
};

inline constexpr content_type_info route_2_consumes[] = {
    {"application/json"},
};

inline constexpr content_type_info route_4_consumes[] = {
    {"application/json"},
};

inline constexpr route_entry routes[] = {
    {"/health", katana::http::method::get, "health", {}, {}},
    {"/users", katana::http::method::get, "listUsers", {}, {}},
    {"/users", katana::http::method::post, "createUser", route_2_consumes, {}},
    {"/users/{id}", katana::http::method::get, "getUser", {}, {}},
    {"/users/{id}", katana::http::method::put, "updateUser", route_4_consumes, {}},
};

inline constexpr size_t route_count = sizeof(routes) / sizeof(routes[0]);

// Compile-time route metadata for type safety
namespace route_metadata {
// health: GET /health
struct health_metadata {
    static constexpr std::string_view path = "/health";
    static constexpr katana::http::method method = katana::http::method::get;
    static constexpr std::string_view operation_id = "health";
    static constexpr size_t path_param_count = 0;
    static constexpr bool has_request_body = false;
};

// listUsers: GET /users
struct listUsers_metadata {
    static constexpr std::string_view path = "/users";
    static constexpr katana::http::method method = katana::http::method::get;
    static constexpr std::string_view operation_id = "listUsers";
    static constexpr size_t path_param_count = 0;
    static constexpr bool has_request_body = false;
};

// createUser: POST /users
struct createUser_metadata {
    static constexpr std::string_view path = "/users";
    static constexpr katana::http::method method = katana::http::method::post;
    static constexpr std::string_view operation_id = "createUser";
    static constexpr size_t path_param_count = 0;
    static constexpr bool has_request_body = true;
};

// getUser: GET /users/{id}
struct getUser_metadata {
    static constexpr std::string_view path = "/users/{id}";
    static constexpr katana::http::method method = katana::http::method::get;
    static constexpr std::string_view operation_id = "getUser";
    static constexpr size_t path_param_count = 1;
    static constexpr bool has_request_body = false;
};

// updateUser: PUT /users/{id}
struct updateUser_metadata {
    static constexpr std::string_view path = "/users/{id}";
    static constexpr katana::http::method method = katana::http::method::put;
    static constexpr std::string_view operation_id = "updateUser";
    static constexpr size_t path_param_count = 1;
    static constexpr bool has_request_body = true;
};

} // namespace route_metadata

// Compile-time validations
static_assert(route_count > 0, "At least one route must be defined");
} // namespace generated
