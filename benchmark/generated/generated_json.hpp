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

[[nodiscard]] inline std::optional<UserInput> parse_UserInput(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<UserInput> parse_UserInput(katana::serde::json_cursor& cur, monotonic_arena* arena);



// ============================================================
// JSON Parse Functions
// ============================================================

// parse UserInput — object, 3 field(s)  ← test_api.yaml:52
[[nodiscard]] inline std::optional<UserInput> parse_UserInput(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    UserInput obj(arena);
    bool has_name = false;
    bool has_email = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "name") {
            if (auto v = cur.string()) {
                obj.name = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                has_name = true;
            } else { cur.skip_value(); }
        } else         if (*key == "email") {
            if (auto v = cur.string()) {
                obj.email = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                has_email = true;
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
    if (!has_name) return std::nullopt;
    if (!has_email) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<UserInput> parse_UserInput(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_UserInput(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// ============================================================
// JSON Serialize Functions
// ============================================================

