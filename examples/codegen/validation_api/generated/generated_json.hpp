// layer: flat
// Auto-generated JSON parsers and serializers from OpenAPI specification
//
// This file contains:
//   - parse_<Type>() functions: JSON string → C++ struct
//   - serialize_<Type>() functions: C++ struct → JSON string
//
// Features:
//   - Zero-copy parsing using arena allocators
//   - Streaming JSON generation without intermediate buffers
//   - Type-safe enum conversion
//   - Automatic null handling for optional fields
//
// All parse functions return std::optional<T>:
//   - std::nullopt on parse error (invalid JSON, wrong type, etc.)
//   - Parsed object on success
//
#pragma once

#include "katana/core/arena.hpp"
#include "katana/core/serde.hpp"
#include <optional>
#include <string>
#include <charconv>
#include <vector>

using katana::monotonic_arena;

// ============================================================
// Forward Declarations
// ============================================================

[[nodiscard]] inline std::optional<RegisterUserRequest> parse_RegisterUserRequest(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<RegisterUserRequest> parse_RegisterUserRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_register_user_response_into(const register_user_response& obj, std::string& out);

inline std::string serialize_register_user_response(const register_user_response& obj);

// ============================================================
// JSON Parse Functions
// ============================================================

// parse RegisterUserRequest — object, 3 field(s)  ← api.yaml:25
[[nodiscard]] inline std::optional<RegisterUserRequest> parse_RegisterUserRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    RegisterUserRequest obj(arena);
    bool has_email = false;
    bool has_password = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "email") {
            if (auto v = cur.string()) {
                obj.email = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                has_email = true;
            } else { cur.skip_value(); }
        } else         if (*key == "password") {
            if (auto v = cur.string()) {
                obj.password = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                has_password = true;
            } else { cur.skip_value(); }
        } else         if (*key == "age") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.age = *v;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_email) return std::nullopt;
    if (!has_password) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<RegisterUserRequest> parse_RegisterUserRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_RegisterUserRequest(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// ============================================================
// JSON Serialize Functions
// ============================================================

// serialize register_user_response — string  ← api.yaml:21
inline void serialize_register_user_response_into(const register_user_response& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_register_user_response(const register_user_response& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_register_user_response_into(obj, json);
    return json;
}

