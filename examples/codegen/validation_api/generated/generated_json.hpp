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
[[nodiscard]] inline std::optional<RegisterUserRequest_Email_t> parse_RegisterUserRequest_Email_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterUserRequest_Password_t> parse_RegisterUserRequest_Password_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterUserRequest_Age_t> parse_RegisterUserRequest_Age_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema> parse_schema(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<register_user_response> parse_register_user_response(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<RegisterUserRequest> parse_RegisterUserRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterUserRequest_Email_t> parse_RegisterUserRequest_Email_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterUserRequest_Password_t> parse_RegisterUserRequest_Password_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterUserRequest_Age_t> parse_RegisterUserRequest_Age_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema> parse_schema(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<register_user_response> parse_register_user_response(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_RegisterUserRequest_into(const RegisterUserRequest& obj, std::string& out);
inline void serialize_RegisterUserRequest_Email_t_into(const RegisterUserRequest_Email_t& obj, std::string& out);
inline void serialize_RegisterUserRequest_Password_t_into(const RegisterUserRequest_Password_t& obj, std::string& out);
inline void serialize_RegisterUserRequest_Age_t_into(const RegisterUserRequest_Age_t& obj, std::string& out);
inline void serialize_schema_into(const schema& obj, std::string& out);
inline void serialize_register_user_response_into(const register_user_response& obj, std::string& out);

inline std::string serialize_RegisterUserRequest(const RegisterUserRequest& obj);
inline std::string serialize_RegisterUserRequest_Email_t(const RegisterUserRequest_Email_t& obj);
inline std::string serialize_RegisterUserRequest_Password_t(const RegisterUserRequest_Password_t& obj);
inline std::string serialize_RegisterUserRequest_Age_t(const RegisterUserRequest_Age_t& obj);
inline std::string serialize_schema(const schema& obj);
inline std::string serialize_register_user_response(const register_user_response& obj);

[[nodiscard]] inline std::optional<std::vector<RegisterUserRequest>> parse_RegisterUserRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterUserRequest_Email_t>> parse_RegisterUserRequest_Email_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterUserRequest_Password_t>> parse_RegisterUserRequest_Password_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterUserRequest_Age_t>> parse_RegisterUserRequest_Age_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<register_user_response>> parse_register_user_response_array(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<std::vector<RegisterUserRequest>> parse_RegisterUserRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterUserRequest_Email_t>> parse_RegisterUserRequest_Email_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterUserRequest_Password_t>> parse_RegisterUserRequest_Password_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterUserRequest_Age_t>> parse_RegisterUserRequest_Age_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<register_user_response>> parse_register_user_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_RegisterUserRequest_array_into(const std::vector<RegisterUserRequest>& arr, std::string& out);
inline void serialize_RegisterUserRequest_array_into(const arena_vector<RegisterUserRequest>& arr, std::string& out);
inline void serialize_RegisterUserRequest_Email_t_array_into(const std::vector<RegisterUserRequest_Email_t>& arr, std::string& out);
inline void serialize_RegisterUserRequest_Email_t_array_into(const arena_vector<RegisterUserRequest_Email_t>& arr, std::string& out);
inline void serialize_RegisterUserRequest_Password_t_array_into(const std::vector<RegisterUserRequest_Password_t>& arr, std::string& out);
inline void serialize_RegisterUserRequest_Password_t_array_into(const arena_vector<RegisterUserRequest_Password_t>& arr, std::string& out);
inline void serialize_RegisterUserRequest_Age_t_array_into(const std::vector<RegisterUserRequest_Age_t>& arr, std::string& out);
inline void serialize_RegisterUserRequest_Age_t_array_into(const arena_vector<RegisterUserRequest_Age_t>& arr, std::string& out);
inline void serialize_schema_array_into(const std::vector<schema>& arr, std::string& out);
inline void serialize_schema_array_into(const arena_vector<schema>& arr, std::string& out);
inline void serialize_register_user_response_array_into(const std::vector<register_user_response>& arr, std::string& out);
inline void serialize_register_user_response_array_into(const arena_vector<register_user_response>& arr, std::string& out);

inline std::string serialize_RegisterUserRequest_array(const std::vector<RegisterUserRequest>& arr);
inline std::string serialize_RegisterUserRequest_array(const arena_vector<RegisterUserRequest>& arr);
inline std::string serialize_RegisterUserRequest_Email_t_array(const std::vector<RegisterUserRequest_Email_t>& arr);
inline std::string serialize_RegisterUserRequest_Email_t_array(const arena_vector<RegisterUserRequest_Email_t>& arr);
inline std::string serialize_RegisterUserRequest_Password_t_array(const std::vector<RegisterUserRequest_Password_t>& arr);
inline std::string serialize_RegisterUserRequest_Password_t_array(const arena_vector<RegisterUserRequest_Password_t>& arr);
inline std::string serialize_RegisterUserRequest_Age_t_array(const std::vector<RegisterUserRequest_Age_t>& arr);
inline std::string serialize_RegisterUserRequest_Age_t_array(const arena_vector<RegisterUserRequest_Age_t>& arr);
inline std::string serialize_schema_array(const std::vector<schema>& arr);
inline std::string serialize_schema_array(const arena_vector<schema>& arr);
inline std::string serialize_register_user_response_array(const std::vector<register_user_response>& arr);
inline std::string serialize_register_user_response_array(const arena_vector<register_user_response>& arr);

// ============================================================
// JSON Parse Functions
// ============================================================

[[nodiscard]] inline std::optional<RegisterUserRequest> parse_RegisterUserRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    RegisterUserRequest obj(arena);
    bool has_email = false;
    bool has_password = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "email") {
            has_email = true;
            if (auto v = cur.string()) {
                obj.email = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else         if (*key == "password") {
            has_password = true;
            if (auto v = cur.string()) {
                obj.password = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
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
    if (!has_email) return std::nullopt;
    if (!has_password) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<RegisterUserRequest> parse_RegisterUserRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterUserRequest(cur, arena);
}

[[nodiscard]] inline std::optional<RegisterUserRequest_Email_t> parse_RegisterUserRequest_Email_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return RegisterUserRequest_Email_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<RegisterUserRequest_Email_t> parse_RegisterUserRequest_Email_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterUserRequest_Email_t(cur, arena);
}

[[nodiscard]] inline std::optional<RegisterUserRequest_Password_t> parse_RegisterUserRequest_Password_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return RegisterUserRequest_Password_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<RegisterUserRequest_Password_t> parse_RegisterUserRequest_Password_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterUserRequest_Password_t(cur, arena);
}

[[nodiscard]] inline std::optional<RegisterUserRequest_Age_t> parse_RegisterUserRequest_Age_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return RegisterUserRequest_Age_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<RegisterUserRequest_Age_t> parse_RegisterUserRequest_Age_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterUserRequest_Age_t(cur, arena);
}

[[nodiscard]] inline std::optional<schema> parse_schema(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema{};
}

[[nodiscard]] inline std::optional<schema> parse_schema(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema(cur, arena);
}

[[nodiscard]] inline std::optional<register_user_response> parse_register_user_response(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return register_user_response{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<register_user_response> parse_register_user_response(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_register_user_response(cur, arena);
}

// ============================================================
// JSON Serialize Functions
// ============================================================

inline void serialize_RegisterUserRequest_into(const RegisterUserRequest& obj, std::string& json) {
    json.push_back('{');
    json.append("\"email\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.email, json);
    json.push_back('"');
    json.append(",\"password\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.password, json);
    json.push_back('"');
    json.append(",\"age\":");
    {
        if (!obj.age) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.age);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    json.push_back('}');
}

inline std::string serialize_RegisterUserRequest(const RegisterUserRequest& obj) {
    std::string json;
    size_t reserve_estimate = 114;
    reserve_estimate += obj.email.size();
    reserve_estimate += obj.password.size();
    json.reserve(reserve_estimate);
    serialize_RegisterUserRequest_into(obj, json);
    return json;
}

inline void serialize_RegisterUserRequest_Email_t_into(const RegisterUserRequest_Email_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_RegisterUserRequest_Email_t(const RegisterUserRequest_Email_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_RegisterUserRequest_Email_t_into(obj, json);
    return json;
}

inline void serialize_RegisterUserRequest_Password_t_into(const RegisterUserRequest_Password_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_RegisterUserRequest_Password_t(const RegisterUserRequest_Password_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_RegisterUserRequest_Password_t_into(obj, json);
    return json;
}

inline void serialize_RegisterUserRequest_Age_t_into(const RegisterUserRequest_Age_t& obj, std::string& json) {
    if (!obj) { json.append("null"); return; }
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_RegisterUserRequest_Age_t(const RegisterUserRequest_Age_t& obj) {
    if (!obj) return std::string("null");
    std::string json;
    serialize_RegisterUserRequest_Age_t_into(obj, json);
    return json;
}

inline void serialize_schema_into(const schema& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema(const schema& obj) {
    (void)obj;
    return std::string("{}");
}

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

// ============================================================
// Array Parse Functions
// ============================================================

[[nodiscard]] inline std::optional<std::vector<RegisterUserRequest>> parse_RegisterUserRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<RegisterUserRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_RegisterUserRequest(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<RegisterUserRequest>> parse_RegisterUserRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterUserRequest_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<RegisterUserRequest_Email_t>> parse_RegisterUserRequest_Email_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<RegisterUserRequest_Email_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_RegisterUserRequest_Email_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<RegisterUserRequest_Email_t>> parse_RegisterUserRequest_Email_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterUserRequest_Email_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<RegisterUserRequest_Password_t>> parse_RegisterUserRequest_Password_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<RegisterUserRequest_Password_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_RegisterUserRequest_Password_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<RegisterUserRequest_Password_t>> parse_RegisterUserRequest_Password_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterUserRequest_Password_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<RegisterUserRequest_Age_t>> parse_RegisterUserRequest_Age_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<RegisterUserRequest_Age_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_RegisterUserRequest_Age_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<RegisterUserRequest_Age_t>> parse_RegisterUserRequest_Age_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterUserRequest_Age_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<register_user_response>> parse_register_user_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<register_user_response> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_register_user_response(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<register_user_response>> parse_register_user_response_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_register_user_response_array(cur, arena);
}

// ============================================================
// Array Serialize Functions
// ============================================================

inline void serialize_RegisterUserRequest_array_into(const std::vector<RegisterUserRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterUserRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterUserRequest_array(const std::vector<RegisterUserRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 114 + 2);
    serialize_RegisterUserRequest_array_into(arr, json);
    return json;
}

inline void serialize_RegisterUserRequest_array_into(const arena_vector<RegisterUserRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterUserRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterUserRequest_array(const arena_vector<RegisterUserRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 114 + 2);
    serialize_RegisterUserRequest_array_into(arr, json);
    return json;
}

inline void serialize_RegisterUserRequest_Email_t_array_into(const std::vector<RegisterUserRequest_Email_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterUserRequest_Email_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterUserRequest_Email_t_array(const std::vector<RegisterUserRequest_Email_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_RegisterUserRequest_Email_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterUserRequest_Email_t_array_into(const arena_vector<RegisterUserRequest_Email_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterUserRequest_Email_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterUserRequest_Email_t_array(const arena_vector<RegisterUserRequest_Email_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_RegisterUserRequest_Email_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterUserRequest_Password_t_array_into(const std::vector<RegisterUserRequest_Password_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterUserRequest_Password_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterUserRequest_Password_t_array(const std::vector<RegisterUserRequest_Password_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_RegisterUserRequest_Password_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterUserRequest_Password_t_array_into(const arena_vector<RegisterUserRequest_Password_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterUserRequest_Password_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterUserRequest_Password_t_array(const arena_vector<RegisterUserRequest_Password_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_RegisterUserRequest_Password_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterUserRequest_Age_t_array_into(const std::vector<RegisterUserRequest_Age_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterUserRequest_Age_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterUserRequest_Age_t_array(const std::vector<RegisterUserRequest_Age_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_RegisterUserRequest_Age_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterUserRequest_Age_t_array_into(const arena_vector<RegisterUserRequest_Age_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterUserRequest_Age_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterUserRequest_Age_t_array(const arena_vector<RegisterUserRequest_Age_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_RegisterUserRequest_Age_t_array_into(arr, json);
    return json;
}

inline void serialize_schema_array_into(const std::vector<schema>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_array(const std::vector<schema>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_array_into(arr, json);
    return json;
}

inline void serialize_schema_array_into(const arena_vector<schema>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_array(const arena_vector<schema>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_array_into(arr, json);
    return json;
}

inline void serialize_register_user_response_array_into(const std::vector<register_user_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_register_user_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_register_user_response_array(const std::vector<register_user_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_register_user_response_array_into(arr, json);
    return json;
}

inline void serialize_register_user_response_array_into(const arena_vector<register_user_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_register_user_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_register_user_response_array(const arena_vector<register_user_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_register_user_response_array_into(arr, json);
    return json;
}

