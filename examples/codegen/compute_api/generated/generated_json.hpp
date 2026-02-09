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
#include <charconv>
#include <optional>
#include <string>
#include <vector>

using katana::monotonic_arena;

// ============================================================
// Forward Declarations
// ============================================================

[[nodiscard]] inline std::optional<compute_sum_request>
parse_compute_sum_request(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema> parse_schema(std::string_view json,
                                                        monotonic_arena* arena);
[[nodiscard]] inline std::optional<compute_sum_response>
parse_compute_sum_response(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<compute_sum_request>
parse_compute_sum_request(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema> parse_schema(katana::serde::json_cursor& cur,
                                                        monotonic_arena* arena);
[[nodiscard]] inline std::optional<compute_sum_response>
parse_compute_sum_response(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_compute_sum_request_into(const compute_sum_request& obj, std::string& out);
inline void serialize_schema_into(const schema& obj, std::string& out);
inline void serialize_compute_sum_response_into(const compute_sum_response& obj, std::string& out);

inline std::string serialize_compute_sum_request(const compute_sum_request& obj);
inline std::string serialize_schema(const schema& obj);
inline std::string serialize_compute_sum_response(const compute_sum_response& obj);

[[nodiscard]] inline std::optional<std::vector<compute_sum_request>>
parse_compute_sum_request_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(std::string_view json,
                                                                           monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<compute_sum_response>>
parse_compute_sum_response_array(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<std::vector<compute_sum_request>>
parse_compute_sum_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema>>
parse_schema_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<compute_sum_response>>
parse_compute_sum_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_compute_sum_request_array_into(const std::vector<compute_sum_request>& arr,
                                                     std::string& out);
inline void serialize_compute_sum_request_array_into(const arena_vector<compute_sum_request>& arr,
                                                     std::string& out);
inline void serialize_schema_array_into(const std::vector<schema>& arr, std::string& out);
inline void serialize_schema_array_into(const arena_vector<schema>& arr, std::string& out);
inline void serialize_compute_sum_response_array_into(const std::vector<compute_sum_response>& arr,
                                                      std::string& out);
inline void serialize_compute_sum_response_array_into(const arena_vector<compute_sum_response>& arr,
                                                      std::string& out);

inline std::string serialize_compute_sum_request_array(const std::vector<compute_sum_request>& arr);
inline std::string
serialize_compute_sum_request_array(const arena_vector<compute_sum_request>& arr);
inline std::string serialize_schema_array(const std::vector<schema>& arr);
inline std::string serialize_schema_array(const arena_vector<schema>& arr);
inline std::string
serialize_compute_sum_response_array(const std::vector<compute_sum_response>& arr);
inline std::string
serialize_compute_sum_response_array(const arena_vector<compute_sum_response>& arr);

// ============================================================
// JSON Parse Functions
// ============================================================

[[nodiscard]] inline std::optional<compute_sum_request>
parse_compute_sum_request(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;
    compute_sum_request result{arena_allocator<schema>(arena)};
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto v = katana::serde::parse_double(cur)) {
            result.push_back(*v);
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<compute_sum_request>
parse_compute_sum_request(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_compute_sum_request(cur, arena);
}

[[nodiscard]] inline std::optional<schema> parse_schema(katana::serde::json_cursor& cur,
                                                        monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur))
        return schema{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<schema> parse_schema(std::string_view json,
                                                        monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema(cur, arena);
}

[[nodiscard]] inline std::optional<compute_sum_response>
parse_compute_sum_response(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur))
        return compute_sum_response{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<compute_sum_response>
parse_compute_sum_response(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_compute_sum_response(cur, arena);
}

// ============================================================
// JSON Serialize Functions
// ============================================================

inline void serialize_compute_sum_request_into(const compute_sum_request& obj, std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_compute_sum_request(const compute_sum_request& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_compute_sum_request_into(obj, json);
    return json;
}

inline void serialize_schema_into(const schema& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc())
        json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_schema(const schema& obj) {
    std::string json;
    serialize_schema_into(obj, json);
    return json;
}

inline void serialize_compute_sum_response_into(const compute_sum_response& obj,
                                                std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc())
        json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_compute_sum_response(const compute_sum_response& obj) {
    std::string json;
    serialize_compute_sum_response_into(obj, json);
    return json;
}

// ============================================================
// Array Parse Functions
// ============================================================

[[nodiscard]] inline std::optional<std::vector<compute_sum_request>>
parse_compute_sum_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<compute_sum_request> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_compute_sum_request(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<compute_sum_request>>
parse_compute_sum_request_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_compute_sum_request_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema>>
parse_schema_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_schema(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(std::string_view json,
                                                                           monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<compute_sum_response>>
parse_compute_sum_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<compute_sum_response> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_compute_sum_response(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<compute_sum_response>>
parse_compute_sum_response_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_compute_sum_response_array(cur, arena);
}

// ============================================================
// Array Serialize Functions
// ============================================================

inline void serialize_compute_sum_request_array_into(const std::vector<compute_sum_request>& arr,
                                                     std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_compute_sum_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_compute_sum_request_array(const std::vector<compute_sum_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_compute_sum_request_array_into(arr, json);
    return json;
}

inline void serialize_compute_sum_request_array_into(const arena_vector<compute_sum_request>& arr,
                                                     std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_compute_sum_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_compute_sum_request_array(const arena_vector<compute_sum_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_compute_sum_request_array_into(arr, json);
    return json;
}

inline void serialize_schema_array_into(const std::vector<schema>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_array(const std::vector<schema>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_schema_array_into(arr, json);
    return json;
}

inline void serialize_schema_array_into(const arena_vector<schema>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_array(const arena_vector<schema>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_schema_array_into(arr, json);
    return json;
}

inline void serialize_compute_sum_response_array_into(const std::vector<compute_sum_response>& arr,
                                                      std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_compute_sum_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_compute_sum_response_array(const std::vector<compute_sum_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_compute_sum_response_array_into(arr, json);
    return json;
}

inline void serialize_compute_sum_response_array_into(const arena_vector<compute_sum_response>& arr,
                                                      std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_compute_sum_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_compute_sum_response_array(const arena_vector<compute_sum_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_compute_sum_response_array_into(arr, json);
    return json;
}
