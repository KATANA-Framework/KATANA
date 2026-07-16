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
    {"/text/uppercase", katana::http::method::post, "text_uppercase", content_types_0, content_types_0},
    {"/text/lowercase", katana::http::method::post, "text_lowercase", content_types_0, content_types_0},
    {"/text/reverse", katana::http::method::post, "text_reverse", content_types_0, content_types_0},
    {"/text/stats", katana::http::method::post, "text_stats", content_types_0, content_types_0},
    {"/text/transform", katana::http::method::post, "text_transform", content_types_0, content_types_0},
};

inline constexpr size_t route_count = sizeof(routes) / sizeof(routes[0]);

// Compile-time route metadata for type safety
namespace route_metadata {
    // text_uppercase: POST /text/uppercase
    struct text_uppercase_metadata {
        static constexpr std::string_view path = "/text/uppercase";
        static constexpr katana::http::method method = katana::http::method::post;
        static constexpr std::string_view operation_id = "text_uppercase";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = true;
    };

    // text_lowercase: POST /text/lowercase
    struct text_lowercase_metadata {
        static constexpr std::string_view path = "/text/lowercase";
        static constexpr katana::http::method method = katana::http::method::post;
        static constexpr std::string_view operation_id = "text_lowercase";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = true;
    };

    // text_reverse: POST /text/reverse
    struct text_reverse_metadata {
        static constexpr std::string_view path = "/text/reverse";
        static constexpr katana::http::method method = katana::http::method::post;
        static constexpr std::string_view operation_id = "text_reverse";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = true;
    };

    // text_stats: POST /text/stats
    struct text_stats_metadata {
        static constexpr std::string_view path = "/text/stats";
        static constexpr katana::http::method method = katana::http::method::post;
        static constexpr std::string_view operation_id = "text_stats";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = true;
    };

    // text_transform: POST /text/transform
    struct text_transform_metadata {
        static constexpr std::string_view path = "/text/transform";
        static constexpr katana::http::method method = katana::http::method::post;
        static constexpr std::string_view operation_id = "text_transform";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = true;
    };

} // namespace route_metadata

// Compile-time validations
static_assert(route_count > 0, "At least one route must be defined");
} // namespace generated
