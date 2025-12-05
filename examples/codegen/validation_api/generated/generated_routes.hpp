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

inline constexpr content_type_info route_0_consumes[] = {
    {"application/json"},
};

inline constexpr content_type_info route_0_produces[] = {
    {"application/json"},
};

inline constexpr route_entry routes[] = {
    {"/user/register",
     katana::http::method::post,
     "register_user",
     route_0_consumes,
     route_0_produces},
};

inline constexpr size_t route_count = sizeof(routes) / sizeof(routes[0]);

// Compile-time route metadata for type safety
namespace route_metadata {
// register_user: POST /user/register
struct register_user_metadata {
    static constexpr std::string_view path = "/user/register";
    static constexpr katana::http::method method = katana::http::method::post;
    static constexpr std::string_view operation_id = "register_user";
    static constexpr size_t path_param_count = 0;
    static constexpr bool has_request_body = true;
};

} // namespace route_metadata

// Compile-time validations
static_assert(route_count > 0, "At least one route must be defined");
} // namespace generated
