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
    {"/notes", katana::http::method::post, "create_note", content_types_0, content_types_0},
    {"/notes", katana::http::method::get, "list_notes", {}, content_types_0},
    {"/notes/{id}", katana::http::method::get, "get_note", {}, content_types_0},
};

inline constexpr size_t route_count = sizeof(routes) / sizeof(routes[0]);

// Compile-time route metadata for type safety
namespace route_metadata {
    // create_note: POST /notes
    struct create_note_metadata {
        static constexpr std::string_view path = "/notes";
        static constexpr katana::http::method method = katana::http::method::post;
        static constexpr std::string_view operation_id = "create_note";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = true;
    };

    // list_notes: GET /notes
    struct list_notes_metadata {
        static constexpr std::string_view path = "/notes";
        static constexpr katana::http::method method = katana::http::method::get;
        static constexpr std::string_view operation_id = "list_notes";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = false;
    };

    // get_note: GET /notes/{id}
    struct get_note_metadata {
        static constexpr std::string_view path = "/notes/{id}";
        static constexpr katana::http::method method = katana::http::method::get;
        static constexpr std::string_view operation_id = "get_note";
        static constexpr size_t path_param_count = 1;
        static constexpr bool has_request_body = false;
    };

} // namespace route_metadata

// Compile-time validations
static_assert(route_count > 0, "At least one route must be defined");
} // namespace generated
