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
    {"application/cbor"},
};

inline constexpr content_type_info content_types_1[] = {
    {"application/json"},
};

inline constexpr route_entry routes[] = {
    {"/pets", katana::http::method::get, "listPets", {}, content_types_0},
    {"/pets", katana::http::method::post, "createPet", content_types_0, content_types_1},
    {"/pets/{petId}", katana::http::method::get, "getPet", {}, content_types_0},
    {"/pets/{petId}", katana::http::method::del, "deletePet", {}, {}},
};

inline constexpr size_t route_count = sizeof(routes) / sizeof(routes[0]);

// Compile-time route metadata for type safety
namespace route_metadata {
    // listPets: GET /pets
    struct listPets_metadata {
        static constexpr std::string_view path = "/pets";
        static constexpr katana::http::method method = katana::http::method::get;
        static constexpr std::string_view operation_id = "listPets";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = false;
    };

    // createPet: POST /pets
    struct createPet_metadata {
        static constexpr std::string_view path = "/pets";
        static constexpr katana::http::method method = katana::http::method::post;
        static constexpr std::string_view operation_id = "createPet";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = true;
    };

    // getPet: GET /pets/{petId}
    struct getPet_metadata {
        static constexpr std::string_view path = "/pets/{petId}";
        static constexpr katana::http::method method = katana::http::method::get;
        static constexpr std::string_view operation_id = "getPet";
        static constexpr size_t path_param_count = 1;
        static constexpr bool has_request_body = false;
    };

    // deletePet: DELETE /pets/{petId}
    struct deletePet_metadata {
        static constexpr std::string_view path = "/pets/{petId}";
        static constexpr katana::http::method method = katana::http::method::del;
        static constexpr std::string_view operation_id = "deletePet";
        static constexpr size_t path_param_count = 1;
        static constexpr bool has_request_body = false;
    };

} // namespace route_metadata

// Compile-time validations
static_assert(route_count > 0, "At least one route must be defined");
} // namespace generated
