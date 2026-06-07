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

[[nodiscard]] inline std::optional<text_uppercase_request> parse_text_uppercase_request(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_uppercase_response> parse_text_uppercase_response(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_lowercase_request> parse_text_lowercase_request(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_lowercase_response> parse_text_lowercase_response(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_reverse_request> parse_text_reverse_request(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_reverse_response> parse_text_reverse_response(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_stats_request> parse_text_stats_request(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_stats_response> parse_text_stats_response(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_request> parse_text_transform_request(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_response> parse_text_transform_response(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<text_uppercase_request> parse_text_uppercase_request(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_uppercase_response> parse_text_uppercase_response(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_lowercase_request> parse_text_lowercase_request(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_lowercase_response> parse_text_lowercase_response(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_reverse_request> parse_text_reverse_request(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_reverse_response> parse_text_reverse_response(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_stats_request> parse_text_stats_request(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_stats_response> parse_text_stats_response(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_request> parse_text_transform_request(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_operation> parse_text_transform_operation(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_response> parse_text_transform_response(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_text_uppercase_request_into(const text_uppercase_request& obj, std::string& out);
inline void serialize_text_uppercase_response_into(const text_uppercase_response& obj, std::string& out);
inline void serialize_text_lowercase_request_into(const text_lowercase_request& obj, std::string& out);
inline void serialize_text_lowercase_response_into(const text_lowercase_response& obj, std::string& out);
inline void serialize_text_reverse_request_into(const text_reverse_request& obj, std::string& out);
inline void serialize_text_reverse_response_into(const text_reverse_response& obj, std::string& out);
inline void serialize_text_stats_request_into(const text_stats_request& obj, std::string& out);
inline void serialize_text_stats_response_into(const text_stats_response& obj, std::string& out);
inline void serialize_text_transform_request_into(const text_transform_request& obj, std::string& out);
inline void serialize_text_transform_operation_into(const text_transform_operation& obj, std::string& out);
inline void serialize_text_transform_response_into(const text_transform_response& obj, std::string& out);

inline std::string serialize_text_uppercase_request(const text_uppercase_request& obj);
inline std::string serialize_text_uppercase_response(const text_uppercase_response& obj);
inline std::string serialize_text_lowercase_request(const text_lowercase_request& obj);
inline std::string serialize_text_lowercase_response(const text_lowercase_response& obj);
inline std::string serialize_text_reverse_request(const text_reverse_request& obj);
inline std::string serialize_text_reverse_response(const text_reverse_response& obj);
inline std::string serialize_text_stats_request(const text_stats_request& obj);
inline std::string serialize_text_stats_response(const text_stats_response& obj);
inline std::string serialize_text_transform_request(const text_transform_request& obj);
inline std::string serialize_text_transform_operation(const text_transform_operation& obj);
inline std::string serialize_text_transform_response(const text_transform_response& obj);

[[nodiscard]] inline std::optional<std::vector<text_uppercase_request>> parse_text_uppercase_request_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_uppercase_response>> parse_text_uppercase_response_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_lowercase_request>> parse_text_lowercase_request_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_lowercase_response>> parse_text_lowercase_response_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_reverse_request>> parse_text_reverse_request_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_reverse_response>> parse_text_reverse_response_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_stats_request>> parse_text_stats_request_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_stats_response>> parse_text_stats_response_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_request>> parse_text_transform_request_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_response>> parse_text_transform_response_array(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<std::vector<text_uppercase_request>> parse_text_uppercase_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_uppercase_response>> parse_text_uppercase_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_lowercase_request>> parse_text_lowercase_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_lowercase_response>> parse_text_lowercase_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_reverse_request>> parse_text_reverse_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_reverse_response>> parse_text_reverse_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_stats_request>> parse_text_stats_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_stats_response>> parse_text_stats_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_request>> parse_text_transform_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_operation>> parse_text_transform_operation_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_response>> parse_text_transform_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_text_uppercase_request_array_into(const std::vector<text_uppercase_request>& arr, std::string& out);
inline void serialize_text_uppercase_request_array_into(const arena_vector<text_uppercase_request>& arr, std::string& out);
inline void serialize_text_uppercase_response_array_into(const std::vector<text_uppercase_response>& arr, std::string& out);
inline void serialize_text_uppercase_response_array_into(const arena_vector<text_uppercase_response>& arr, std::string& out);
inline void serialize_text_lowercase_request_array_into(const std::vector<text_lowercase_request>& arr, std::string& out);
inline void serialize_text_lowercase_request_array_into(const arena_vector<text_lowercase_request>& arr, std::string& out);
inline void serialize_text_lowercase_response_array_into(const std::vector<text_lowercase_response>& arr, std::string& out);
inline void serialize_text_lowercase_response_array_into(const arena_vector<text_lowercase_response>& arr, std::string& out);
inline void serialize_text_reverse_request_array_into(const std::vector<text_reverse_request>& arr, std::string& out);
inline void serialize_text_reverse_request_array_into(const arena_vector<text_reverse_request>& arr, std::string& out);
inline void serialize_text_reverse_response_array_into(const std::vector<text_reverse_response>& arr, std::string& out);
inline void serialize_text_reverse_response_array_into(const arena_vector<text_reverse_response>& arr, std::string& out);
inline void serialize_text_stats_request_array_into(const std::vector<text_stats_request>& arr, std::string& out);
inline void serialize_text_stats_request_array_into(const arena_vector<text_stats_request>& arr, std::string& out);
inline void serialize_text_stats_response_array_into(const std::vector<text_stats_response>& arr, std::string& out);
inline void serialize_text_stats_response_array_into(const arena_vector<text_stats_response>& arr, std::string& out);
inline void serialize_text_transform_request_array_into(const std::vector<text_transform_request>& arr, std::string& out);
inline void serialize_text_transform_request_array_into(const arena_vector<text_transform_request>& arr, std::string& out);
inline void serialize_text_transform_operation_array_into(const std::vector<text_transform_operation>& arr, std::string& out);
inline void serialize_text_transform_operation_array_into(const arena_vector<text_transform_operation>& arr, std::string& out);
inline void serialize_text_transform_response_array_into(const std::vector<text_transform_response>& arr, std::string& out);
inline void serialize_text_transform_response_array_into(const arena_vector<text_transform_response>& arr, std::string& out);

inline std::string serialize_text_uppercase_request_array(const std::vector<text_uppercase_request>& arr);
inline std::string serialize_text_uppercase_request_array(const arena_vector<text_uppercase_request>& arr);
inline std::string serialize_text_uppercase_response_array(const std::vector<text_uppercase_response>& arr);
inline std::string serialize_text_uppercase_response_array(const arena_vector<text_uppercase_response>& arr);
inline std::string serialize_text_lowercase_request_array(const std::vector<text_lowercase_request>& arr);
inline std::string serialize_text_lowercase_request_array(const arena_vector<text_lowercase_request>& arr);
inline std::string serialize_text_lowercase_response_array(const std::vector<text_lowercase_response>& arr);
inline std::string serialize_text_lowercase_response_array(const arena_vector<text_lowercase_response>& arr);
inline std::string serialize_text_reverse_request_array(const std::vector<text_reverse_request>& arr);
inline std::string serialize_text_reverse_request_array(const arena_vector<text_reverse_request>& arr);
inline std::string serialize_text_reverse_response_array(const std::vector<text_reverse_response>& arr);
inline std::string serialize_text_reverse_response_array(const arena_vector<text_reverse_response>& arr);
inline std::string serialize_text_stats_request_array(const std::vector<text_stats_request>& arr);
inline std::string serialize_text_stats_request_array(const arena_vector<text_stats_request>& arr);
inline std::string serialize_text_stats_response_array(const std::vector<text_stats_response>& arr);
inline std::string serialize_text_stats_response_array(const arena_vector<text_stats_response>& arr);
inline std::string serialize_text_transform_request_array(const std::vector<text_transform_request>& arr);
inline std::string serialize_text_transform_request_array(const arena_vector<text_transform_request>& arr);
inline std::string serialize_text_transform_operation_array(const std::vector<text_transform_operation>& arr);
inline std::string serialize_text_transform_operation_array(const arena_vector<text_transform_operation>& arr);
inline std::string serialize_text_transform_response_array(const std::vector<text_transform_response>& arr);
inline std::string serialize_text_transform_response_array(const arena_vector<text_transform_response>& arr);

// ============================================================
// JSON Parse Functions
// ============================================================

// parse text_uppercase_request — object, 1 field(s)  ← api.yaml:18
[[nodiscard]] inline std::optional<text_uppercase_request> parse_text_uppercase_request(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_uppercase_request obj(arena);
    bool has_text = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "text") {
            if (auto v = cur.string()) {
                obj.text = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                has_text = true;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_text) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<text_uppercase_request> parse_text_uppercase_request(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_text_uppercase_request(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse text_uppercase_response — object, 1 field(s)  ← api.yaml:31
[[nodiscard]] inline std::optional<text_uppercase_response> parse_text_uppercase_response(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_uppercase_response obj(arena);

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "result") {
            if (auto v = cur.string()) {
                obj.result = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<text_uppercase_response> parse_text_uppercase_response(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_text_uppercase_response(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse text_lowercase_request — object, 1 field(s)  ← api.yaml:45
[[nodiscard]] inline std::optional<text_lowercase_request> parse_text_lowercase_request(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_lowercase_request obj(arena);
    bool has_text = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "text") {
            if (auto v = cur.string()) {
                obj.text = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                has_text = true;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_text) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<text_lowercase_request> parse_text_lowercase_request(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_text_lowercase_request(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse text_lowercase_response — object, 1 field(s)  ← api.yaml:58
[[nodiscard]] inline std::optional<text_lowercase_response> parse_text_lowercase_response(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_lowercase_response obj(arena);

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "result") {
            if (auto v = cur.string()) {
                obj.result = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<text_lowercase_response> parse_text_lowercase_response(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_text_lowercase_response(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse text_reverse_request — object, 1 field(s)  ← api.yaml:72
[[nodiscard]] inline std::optional<text_reverse_request> parse_text_reverse_request(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_reverse_request obj(arena);
    bool has_text = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "text") {
            if (auto v = cur.string()) {
                obj.text = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                has_text = true;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_text) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<text_reverse_request> parse_text_reverse_request(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_text_reverse_request(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse text_reverse_response — object, 1 field(s)  ← api.yaml:85
[[nodiscard]] inline std::optional<text_reverse_response> parse_text_reverse_response(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_reverse_response obj(arena);

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "result") {
            if (auto v = cur.string()) {
                obj.result = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<text_reverse_response> parse_text_reverse_response(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_text_reverse_response(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse text_stats_request — object, 1 field(s)  ← api.yaml:100
[[nodiscard]] inline std::optional<text_stats_request> parse_text_stats_request(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_stats_request obj(arena);
    bool has_text = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "text") {
            if (auto v = cur.string()) {
                obj.text = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                has_text = true;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_text) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<text_stats_request> parse_text_stats_request(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_text_stats_request(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse text_stats_response — object, 3 field(s)  ← api.yaml:112
[[nodiscard]] inline std::optional<text_stats_response> parse_text_stats_response(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_stats_response obj(arena);

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "chars") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.chars = *v;
            } else { cur.skip_value(); }
        } else         if (*key == "words") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.words = *v;
            } else { cur.skip_value(); }
        } else         if (*key == "lines") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.lines = *v;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<text_stats_response> parse_text_stats_response(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_text_stats_response(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse text_transform_request — object, 3 field(s)  ← api.yaml:134
[[nodiscard]] inline std::optional<text_transform_request> parse_text_transform_request(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_transform_request obj(arena);
    bool has_text = false;
    bool has_operation = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "text") {
            if (auto v = cur.string()) {
                obj.text = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                has_text = true;
            } else { cur.skip_value(); }
        } else         if (*key == "operation") {
            if (auto v = cur.string()) {
                std::string ev_scratch_;
                auto enum_val = text_transform_operation_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
                if (enum_val) { obj.operation = *enum_val;
                has_operation = true;
                }
            } else { cur.skip_value(); }
        } else         if (*key == "trim") {
            if (auto v = katana::serde::parse_bool(cur)) {
                obj.trim = *v;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_text) return std::nullopt;
    if (!has_operation) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<text_transform_request> parse_text_transform_request(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_text_transform_request(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse text_transform_operation — enum, field text_transform_request.operation  ← api.yaml:142
[[nodiscard]] inline std::optional<text_transform_operation> parse_text_transform_operation(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = cur.string()) {
        std::string ev_scratch_;
        return text_transform_operation_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
    }
    return std::nullopt;
}

// parse text_transform_response — object, 3 field(s)  ← api.yaml:153
[[nodiscard]] inline std::optional<text_transform_response> parse_text_transform_response(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_transform_response obj(arena);

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "original_length") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.original_length = *v;
            } else { cur.skip_value(); }
        } else         if (*key == "result") {
            if (auto v = cur.string()) {
                obj.result = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else         if (*key == "operation_applied") {
            if (auto v = cur.string()) {
                obj.operation_applied = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<text_transform_response> parse_text_transform_response(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_text_transform_response(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// ============================================================
// JSON Serialize Functions
// ============================================================

// serialize text_uppercase_request — object, 1 field(s)  ← api.yaml:18
inline void serialize_text_uppercase_request_into(const text_uppercase_request& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"text\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.text, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_text_uppercase_request(const text_uppercase_request& obj) {
    std::string json;
    size_t reserve_estimate = 42;
    reserve_estimate += obj.text.size();
    json.reserve(reserve_estimate);
    serialize_text_uppercase_request_into(obj, json);
    return json;
}

// serialize text_uppercase_response — object, 1 field(s)  ← api.yaml:31
inline void serialize_text_uppercase_response_into(const text_uppercase_response& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (obj.result) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"result\":");
    if (obj.result) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.result, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    json.push_back('}');
}

inline std::string serialize_text_uppercase_response(const text_uppercase_response& obj) {
    std::string json;
    size_t reserve_estimate = 44;
    if (obj.result) reserve_estimate += obj.result->size();
    json.reserve(reserve_estimate);
    serialize_text_uppercase_response_into(obj, json);
    return json;
}

// serialize text_lowercase_request — object, 1 field(s)  ← api.yaml:45
inline void serialize_text_lowercase_request_into(const text_lowercase_request& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"text\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.text, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_text_lowercase_request(const text_lowercase_request& obj) {
    std::string json;
    size_t reserve_estimate = 42;
    reserve_estimate += obj.text.size();
    json.reserve(reserve_estimate);
    serialize_text_lowercase_request_into(obj, json);
    return json;
}

// serialize text_lowercase_response — object, 1 field(s)  ← api.yaml:58
inline void serialize_text_lowercase_response_into(const text_lowercase_response& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (obj.result) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"result\":");
    if (obj.result) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.result, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    json.push_back('}');
}

inline std::string serialize_text_lowercase_response(const text_lowercase_response& obj) {
    std::string json;
    size_t reserve_estimate = 44;
    if (obj.result) reserve_estimate += obj.result->size();
    json.reserve(reserve_estimate);
    serialize_text_lowercase_response_into(obj, json);
    return json;
}

// serialize text_reverse_request — object, 1 field(s)  ← api.yaml:72
inline void serialize_text_reverse_request_into(const text_reverse_request& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"text\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.text, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_text_reverse_request(const text_reverse_request& obj) {
    std::string json;
    size_t reserve_estimate = 42;
    reserve_estimate += obj.text.size();
    json.reserve(reserve_estimate);
    serialize_text_reverse_request_into(obj, json);
    return json;
}

// serialize text_reverse_response — object, 1 field(s)  ← api.yaml:85
inline void serialize_text_reverse_response_into(const text_reverse_response& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (obj.result) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"result\":");
    if (obj.result) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.result, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    json.push_back('}');
}

inline std::string serialize_text_reverse_response(const text_reverse_response& obj) {
    std::string json;
    size_t reserve_estimate = 44;
    if (obj.result) reserve_estimate += obj.result->size();
    json.reserve(reserve_estimate);
    serialize_text_reverse_response_into(obj, json);
    return json;
}

// serialize text_stats_request — object, 1 field(s)  ← api.yaml:100
inline void serialize_text_stats_request_into(const text_stats_request& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"text\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.text, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_text_stats_request(const text_stats_request& obj) {
    std::string json;
    size_t reserve_estimate = 42;
    reserve_estimate += obj.text.size();
    json.reserve(reserve_estimate);
    serialize_text_stats_request_into(obj, json);
    return json;
}

// serialize text_stats_response — object, 3 field(s)  ← api.yaml:112
inline void serialize_text_stats_response_into(const text_stats_response& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (obj.chars) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"chars\":");
    {
        if (!obj.chars) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.chars);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    if (obj.words) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"words\":");
    {
        if (!obj.words) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.words);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    if (obj.lines) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"lines\":");
    {
        if (!obj.lines) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.lines);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    json.push_back('}');
}

inline std::string serialize_text_stats_response(const text_stats_response& obj) {
    std::string json;
    size_t reserve_estimate = 89;
    json.reserve(reserve_estimate);
    serialize_text_stats_response_into(obj, json);
    return json;
}

// serialize text_transform_request — object, 3 field(s)  ← api.yaml:134
inline void serialize_text_transform_request_into(const text_transform_request& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"text\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.text, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"operation\":");
    json.push_back('"');
    json.append(to_string(obj.operation));
    json.push_back('"');
    if (obj.trim) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"trim\":");
    if (!obj.trim) {
        json.append("null");
    } else {
        json.append(*obj.trim ? "true" : "false");
    }
    }
    json.push_back('}');
}

inline std::string serialize_text_transform_request(const text_transform_request& obj) {
    std::string json;
    size_t reserve_estimate = 100;
    reserve_estimate += obj.text.size();
    json.reserve(reserve_estimate);
    serialize_text_transform_request_into(obj, json);
    return json;
}

// serialize text_transform_operation — enum, field text_transform_request.operation  ← api.yaml:142
inline void serialize_text_transform_operation_into(const text_transform_operation& obj, std::string& json) {
    auto str = to_string(obj);
    json.push_back('"');
    json.append(str);
    json.push_back('"');
}

inline std::string serialize_text_transform_operation(const text_transform_operation& obj) {
    std::string json;
    auto str = to_string(obj);
    json.reserve(str.size() + 2);
    serialize_text_transform_operation_into(obj, json);
    return json;
}

// serialize text_transform_response — object, 3 field(s)  ← api.yaml:153
inline void serialize_text_transform_response_into(const text_transform_response& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (obj.original_length) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"original_length\":");
    {
        if (!obj.original_length) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.original_length);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    if (obj.result) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"result\":");
    if (obj.result) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.result, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    if (obj.operation_applied) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"operation_applied\":");
    if (obj.operation_applied) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.operation_applied, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    json.push_back('}');
}

inline std::string serialize_text_transform_response(const text_transform_response& obj) {
    std::string json;
    size_t reserve_estimate = 136;
    if (obj.result) reserve_estimate += obj.result->size();
    if (obj.operation_applied) reserve_estimate += obj.operation_applied->size();
    json.reserve(reserve_estimate);
    serialize_text_transform_response_into(obj, json);
    return json;
}

// ============================================================
// Array Parse Functions
// ============================================================

// parse_array text_uppercase_request — object, 1 field(s)  ← api.yaml:18
[[nodiscard]] inline std::optional<std::vector<text_uppercase_request>> parse_text_uppercase_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_uppercase_request> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_uppercase_request(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_uppercase_request>> parse_text_uppercase_request_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_uppercase_request_array(cur, arena);
}

// parse_array text_uppercase_response — object, 1 field(s)  ← api.yaml:31
[[nodiscard]] inline std::optional<std::vector<text_uppercase_response>> parse_text_uppercase_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_uppercase_response> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_uppercase_response(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_uppercase_response>> parse_text_uppercase_response_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_uppercase_response_array(cur, arena);
}

// parse_array text_lowercase_request — object, 1 field(s)  ← api.yaml:45
[[nodiscard]] inline std::optional<std::vector<text_lowercase_request>> parse_text_lowercase_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_lowercase_request> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_lowercase_request(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_lowercase_request>> parse_text_lowercase_request_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_lowercase_request_array(cur, arena);
}

// parse_array text_lowercase_response — object, 1 field(s)  ← api.yaml:58
[[nodiscard]] inline std::optional<std::vector<text_lowercase_response>> parse_text_lowercase_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_lowercase_response> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_lowercase_response(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_lowercase_response>> parse_text_lowercase_response_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_lowercase_response_array(cur, arena);
}

// parse_array text_reverse_request — object, 1 field(s)  ← api.yaml:72
[[nodiscard]] inline std::optional<std::vector<text_reverse_request>> parse_text_reverse_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_reverse_request> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_reverse_request(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_reverse_request>> parse_text_reverse_request_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_reverse_request_array(cur, arena);
}

// parse_array text_reverse_response — object, 1 field(s)  ← api.yaml:85
[[nodiscard]] inline std::optional<std::vector<text_reverse_response>> parse_text_reverse_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_reverse_response> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_reverse_response(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_reverse_response>> parse_text_reverse_response_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_reverse_response_array(cur, arena);
}

// parse_array text_stats_request — object, 1 field(s)  ← api.yaml:100
[[nodiscard]] inline std::optional<std::vector<text_stats_request>> parse_text_stats_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_stats_request> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_stats_request(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_stats_request>> parse_text_stats_request_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_stats_request_array(cur, arena);
}

// parse_array text_stats_response — object, 3 field(s)  ← api.yaml:112
[[nodiscard]] inline std::optional<std::vector<text_stats_response>> parse_text_stats_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_stats_response> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_stats_response(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_stats_response>> parse_text_stats_response_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_stats_response_array(cur, arena);
}

// parse_array text_transform_request — object, 3 field(s)  ← api.yaml:134
[[nodiscard]] inline std::optional<std::vector<text_transform_request>> parse_text_transform_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_transform_request> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_transform_request(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_transform_request>> parse_text_transform_request_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_transform_request_array(cur, arena);
}

// parse_array text_transform_operation — enum, field text_transform_request.operation  ← api.yaml:142
[[nodiscard]] inline std::optional<std::vector<text_transform_operation>> parse_text_transform_operation_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_transform_operation> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_transform_operation(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

// parse_array text_transform_response — object, 3 field(s)  ← api.yaml:153
[[nodiscard]] inline std::optional<std::vector<text_transform_response>> parse_text_transform_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_transform_response> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_transform_response(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_transform_response>> parse_text_transform_response_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_transform_response_array(cur, arena);
}

// ============================================================
// Array Serialize Functions
// ============================================================

// serialize_array text_uppercase_request — object, 1 field(s)  ← api.yaml:18
inline void serialize_text_uppercase_request_array_into(const std::vector<text_uppercase_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_uppercase_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_uppercase_request_array(const std::vector<text_uppercase_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 42 + 2);
    serialize_text_uppercase_request_array_into(arr, json);
    return json;
}

inline void serialize_text_uppercase_request_array_into(const arena_vector<text_uppercase_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_uppercase_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_uppercase_request_array(const arena_vector<text_uppercase_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 42 + 2);
    serialize_text_uppercase_request_array_into(arr, json);
    return json;
}

// serialize_array text_uppercase_response — object, 1 field(s)  ← api.yaml:31
inline void serialize_text_uppercase_response_array_into(const std::vector<text_uppercase_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_uppercase_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_uppercase_response_array(const std::vector<text_uppercase_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 44 + 2);
    serialize_text_uppercase_response_array_into(arr, json);
    return json;
}

inline void serialize_text_uppercase_response_array_into(const arena_vector<text_uppercase_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_uppercase_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_uppercase_response_array(const arena_vector<text_uppercase_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 44 + 2);
    serialize_text_uppercase_response_array_into(arr, json);
    return json;
}

// serialize_array text_lowercase_request — object, 1 field(s)  ← api.yaml:45
inline void serialize_text_lowercase_request_array_into(const std::vector<text_lowercase_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_lowercase_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_lowercase_request_array(const std::vector<text_lowercase_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 42 + 2);
    serialize_text_lowercase_request_array_into(arr, json);
    return json;
}

inline void serialize_text_lowercase_request_array_into(const arena_vector<text_lowercase_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_lowercase_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_lowercase_request_array(const arena_vector<text_lowercase_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 42 + 2);
    serialize_text_lowercase_request_array_into(arr, json);
    return json;
}

// serialize_array text_lowercase_response — object, 1 field(s)  ← api.yaml:58
inline void serialize_text_lowercase_response_array_into(const std::vector<text_lowercase_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_lowercase_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_lowercase_response_array(const std::vector<text_lowercase_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 44 + 2);
    serialize_text_lowercase_response_array_into(arr, json);
    return json;
}

inline void serialize_text_lowercase_response_array_into(const arena_vector<text_lowercase_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_lowercase_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_lowercase_response_array(const arena_vector<text_lowercase_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 44 + 2);
    serialize_text_lowercase_response_array_into(arr, json);
    return json;
}

// serialize_array text_reverse_request — object, 1 field(s)  ← api.yaml:72
inline void serialize_text_reverse_request_array_into(const std::vector<text_reverse_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_reverse_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_reverse_request_array(const std::vector<text_reverse_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 42 + 2);
    serialize_text_reverse_request_array_into(arr, json);
    return json;
}

inline void serialize_text_reverse_request_array_into(const arena_vector<text_reverse_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_reverse_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_reverse_request_array(const arena_vector<text_reverse_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 42 + 2);
    serialize_text_reverse_request_array_into(arr, json);
    return json;
}

// serialize_array text_reverse_response — object, 1 field(s)  ← api.yaml:85
inline void serialize_text_reverse_response_array_into(const std::vector<text_reverse_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_reverse_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_reverse_response_array(const std::vector<text_reverse_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 44 + 2);
    serialize_text_reverse_response_array_into(arr, json);
    return json;
}

inline void serialize_text_reverse_response_array_into(const arena_vector<text_reverse_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_reverse_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_reverse_response_array(const arena_vector<text_reverse_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 44 + 2);
    serialize_text_reverse_response_array_into(arr, json);
    return json;
}

// serialize_array text_stats_request — object, 1 field(s)  ← api.yaml:100
inline void serialize_text_stats_request_array_into(const std::vector<text_stats_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_stats_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_stats_request_array(const std::vector<text_stats_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 42 + 2);
    serialize_text_stats_request_array_into(arr, json);
    return json;
}

inline void serialize_text_stats_request_array_into(const arena_vector<text_stats_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_stats_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_stats_request_array(const arena_vector<text_stats_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 42 + 2);
    serialize_text_stats_request_array_into(arr, json);
    return json;
}

// serialize_array text_stats_response — object, 3 field(s)  ← api.yaml:112
inline void serialize_text_stats_response_array_into(const std::vector<text_stats_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_stats_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_stats_response_array(const std::vector<text_stats_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 89 + 2);
    serialize_text_stats_response_array_into(arr, json);
    return json;
}

inline void serialize_text_stats_response_array_into(const arena_vector<text_stats_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_stats_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_stats_response_array(const arena_vector<text_stats_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 89 + 2);
    serialize_text_stats_response_array_into(arr, json);
    return json;
}

// serialize_array text_transform_request — object, 3 field(s)  ← api.yaml:134
inline void serialize_text_transform_request_array_into(const std::vector<text_transform_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_request_array(const std::vector<text_transform_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 100 + 2);
    serialize_text_transform_request_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_request_array_into(const arena_vector<text_transform_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_request_array(const arena_vector<text_transform_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 100 + 2);
    serialize_text_transform_request_array_into(arr, json);
    return json;
}

// serialize_array text_transform_operation — enum, field text_transform_request.operation  ← api.yaml:142
inline void serialize_text_transform_operation_array_into(const std::vector<text_transform_operation>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_operation_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_operation_array(const std::vector<text_transform_operation>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_transform_operation_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_operation_array_into(const arena_vector<text_transform_operation>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_operation_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_operation_array(const arena_vector<text_transform_operation>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_transform_operation_array_into(arr, json);
    return json;
}

// serialize_array text_transform_response — object, 3 field(s)  ← api.yaml:153
inline void serialize_text_transform_response_array_into(const std::vector<text_transform_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_response_array(const std::vector<text_transform_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 136 + 2);
    serialize_text_transform_response_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_response_array_into(const arena_vector<text_transform_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_response_array(const arena_vector<text_transform_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 136 + 2);
    serialize_text_transform_response_array_into(arr, json);
    return json;
}

