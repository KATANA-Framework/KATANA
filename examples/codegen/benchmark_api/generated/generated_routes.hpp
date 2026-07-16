// layer: flat
#pragma once

#include "katana/core/http.hpp"
#include "katana/core/http_utils.hpp"
#include "katana/core/router.hpp"
#include <array>
#include <span>
#include <string_view>

namespace generated {

using katana::http_utils::content_type_info;

struct route_entry {
    std::string_view path;
    katana::http::method method;
    std::string_view operation_id;
    std::span<const content_type_info> consumes;
    std::span<const content_type_info> produces;
};

inline constexpr content_type_info content_types_0[] = {
    {"application/json"},
};

inline constexpr route_entry routes[] = {
    {"/compute/sum", katana::http::method::post, "compute_sum", content_types_0, content_types_0},
    {"/compute/stats", katana::http::method::post, "compute_stats", content_types_0, content_types_0},
    {"/users/register", katana::http::method::post, "register_user", content_types_0, content_types_0},
    {"/items", katana::http::method::get, "list_items", {}, content_types_0},
    {"/items", katana::http::method::post, "create_item", content_types_0, content_types_0},
    {"/items/{id}", katana::http::method::get, "get_item", {}, content_types_0},
    {"/items/{id}", katana::http::method::put, "update_item", content_types_0, content_types_0},
    {"/items/{id}", katana::http::method::del, "delete_item", {}, {}},
    {"/json", katana::http::method::get, "json_message", {}, content_types_0},
    {"/echo", katana::http::method::post, "echo", content_types_0, content_types_0},
    {"/health", katana::http::method::get, "health_check", {}, content_types_0},
};

inline constexpr size_t route_count = sizeof(routes) / sizeof(routes[0]);

// Compile-time route metadata for type safety
namespace route_metadata {
    // compute_sum: POST /compute/sum
    struct compute_sum_metadata {
        static constexpr std::string_view path = "/compute/sum";
        static constexpr katana::http::method method = katana::http::method::post;
        static constexpr std::string_view operation_id = "compute_sum";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = true;
    };

    // compute_stats: POST /compute/stats
    struct compute_stats_metadata {
        static constexpr std::string_view path = "/compute/stats";
        static constexpr katana::http::method method = katana::http::method::post;
        static constexpr std::string_view operation_id = "compute_stats";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = true;
    };

    // register_user: POST /users/register
    struct register_user_metadata {
        static constexpr std::string_view path = "/users/register";
        static constexpr katana::http::method method = katana::http::method::post;
        static constexpr std::string_view operation_id = "register_user";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = true;
    };

    // list_items: GET /items
    struct list_items_metadata {
        static constexpr std::string_view path = "/items";
        static constexpr katana::http::method method = katana::http::method::get;
        static constexpr std::string_view operation_id = "list_items";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = false;
    };

    // create_item: POST /items
    struct create_item_metadata {
        static constexpr std::string_view path = "/items";
        static constexpr katana::http::method method = katana::http::method::post;
        static constexpr std::string_view operation_id = "create_item";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = true;
    };

    // get_item: GET /items/{id}
    struct get_item_metadata {
        static constexpr std::string_view path = "/items/{id}";
        static constexpr katana::http::method method = katana::http::method::get;
        static constexpr std::string_view operation_id = "get_item";
        static constexpr size_t path_param_count = 1;
        static constexpr bool has_request_body = false;
    };

    // update_item: PUT /items/{id}
    struct update_item_metadata {
        static constexpr std::string_view path = "/items/{id}";
        static constexpr katana::http::method method = katana::http::method::put;
        static constexpr std::string_view operation_id = "update_item";
        static constexpr size_t path_param_count = 1;
        static constexpr bool has_request_body = true;
    };

    // delete_item: DELETE /items/{id}
    struct delete_item_metadata {
        static constexpr std::string_view path = "/items/{id}";
        static constexpr katana::http::method method = katana::http::method::del;
        static constexpr std::string_view operation_id = "delete_item";
        static constexpr size_t path_param_count = 1;
        static constexpr bool has_request_body = false;
    };

    // json_message: GET /json
    struct json_message_metadata {
        static constexpr std::string_view path = "/json";
        static constexpr katana::http::method method = katana::http::method::get;
        static constexpr std::string_view operation_id = "json_message";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = false;
    };

    // echo: POST /echo
    struct echo_metadata {
        static constexpr std::string_view path = "/echo";
        static constexpr katana::http::method method = katana::http::method::post;
        static constexpr std::string_view operation_id = "echo";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = true;
    };

    // health_check: GET /health
    struct health_check_metadata {
        static constexpr std::string_view path = "/health";
        static constexpr katana::http::method method = katana::http::method::get;
        static constexpr std::string_view operation_id = "health_check";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = false;
    };

} // namespace route_metadata

// Compile-time validations
static_assert(route_count > 0, "At least one route must be defined");
} // namespace generated
