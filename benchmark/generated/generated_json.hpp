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
[[nodiscard]] inline std::optional<UserInput_Name_t> parse_UserInput_Name_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserInput_Email_t> parse_UserInput_Email_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserInput_Age_t> parse_UserInput_Age_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema> parse_schema(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<getUser_param_id> parse_getUser_param_id(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_1> parse_schema_1(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<UserInput> parse_UserInput(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserInput_Name_t> parse_UserInput_Name_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserInput_Email_t> parse_UserInput_Email_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserInput_Age_t> parse_UserInput_Age_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema> parse_schema(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<getUser_param_id> parse_getUser_param_id(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_1> parse_schema_1(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_UserInput_into(const UserInput& obj, std::string& out);
inline void serialize_UserInput_Name_t_into(const UserInput_Name_t& obj, std::string& out);
inline void serialize_UserInput_Email_t_into(const UserInput_Email_t& obj, std::string& out);
inline void serialize_UserInput_Age_t_into(const UserInput_Age_t& obj, std::string& out);
inline void serialize_schema_into(const schema& obj, std::string& out);
inline void serialize_getUser_param_id_into(const getUser_param_id& obj, std::string& out);
inline void serialize_schema_1_into(const schema_1& obj, std::string& out);

inline std::string serialize_UserInput(const UserInput& obj);
inline std::string serialize_UserInput_Name_t(const UserInput_Name_t& obj);
inline std::string serialize_UserInput_Email_t(const UserInput_Email_t& obj);
inline std::string serialize_UserInput_Age_t(const UserInput_Age_t& obj);
inline std::string serialize_schema(const schema& obj);
inline std::string serialize_getUser_param_id(const getUser_param_id& obj);
inline std::string serialize_schema_1(const schema_1& obj);

[[nodiscard]] inline std::optional<std::vector<UserInput>> parse_UserInput_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserInput_Name_t>> parse_UserInput_Name_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserInput_Email_t>> parse_UserInput_Email_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserInput_Age_t>> parse_UserInput_Age_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<getUser_param_id>> parse_getUser_param_id_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_1>> parse_schema_1_array(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<std::vector<UserInput>> parse_UserInput_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserInput_Name_t>> parse_UserInput_Name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserInput_Email_t>> parse_UserInput_Email_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserInput_Age_t>> parse_UserInput_Age_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<getUser_param_id>> parse_getUser_param_id_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_1>> parse_schema_1_array(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_UserInput_array_into(const std::vector<UserInput>& arr, std::string& out);
inline void serialize_UserInput_array_into(const arena_vector<UserInput>& arr, std::string& out);
inline void serialize_UserInput_Name_t_array_into(const std::vector<UserInput_Name_t>& arr, std::string& out);
inline void serialize_UserInput_Name_t_array_into(const arena_vector<UserInput_Name_t>& arr, std::string& out);
inline void serialize_UserInput_Email_t_array_into(const std::vector<UserInput_Email_t>& arr, std::string& out);
inline void serialize_UserInput_Email_t_array_into(const arena_vector<UserInput_Email_t>& arr, std::string& out);
inline void serialize_UserInput_Age_t_array_into(const std::vector<UserInput_Age_t>& arr, std::string& out);
inline void serialize_UserInput_Age_t_array_into(const arena_vector<UserInput_Age_t>& arr, std::string& out);
inline void serialize_schema_array_into(const std::vector<schema>& arr, std::string& out);
inline void serialize_schema_array_into(const arena_vector<schema>& arr, std::string& out);
inline void serialize_getUser_param_id_array_into(const std::vector<getUser_param_id>& arr, std::string& out);
inline void serialize_getUser_param_id_array_into(const arena_vector<getUser_param_id>& arr, std::string& out);
inline void serialize_schema_1_array_into(const std::vector<schema_1>& arr, std::string& out);
inline void serialize_schema_1_array_into(const arena_vector<schema_1>& arr, std::string& out);

inline std::string serialize_UserInput_array(const std::vector<UserInput>& arr);
inline std::string serialize_UserInput_array(const arena_vector<UserInput>& arr);
inline std::string serialize_UserInput_Name_t_array(const std::vector<UserInput_Name_t>& arr);
inline std::string serialize_UserInput_Name_t_array(const arena_vector<UserInput_Name_t>& arr);
inline std::string serialize_UserInput_Email_t_array(const std::vector<UserInput_Email_t>& arr);
inline std::string serialize_UserInput_Email_t_array(const arena_vector<UserInput_Email_t>& arr);
inline std::string serialize_UserInput_Age_t_array(const std::vector<UserInput_Age_t>& arr);
inline std::string serialize_UserInput_Age_t_array(const arena_vector<UserInput_Age_t>& arr);
inline std::string serialize_schema_array(const std::vector<schema>& arr);
inline std::string serialize_schema_array(const arena_vector<schema>& arr);
inline std::string serialize_getUser_param_id_array(const std::vector<getUser_param_id>& arr);
inline std::string serialize_getUser_param_id_array(const arena_vector<getUser_param_id>& arr);
inline std::string serialize_schema_1_array(const std::vector<schema_1>& arr);
inline std::string serialize_schema_1_array(const arena_vector<schema_1>& arr);

// ============================================================
// JSON Parse Functions
// ============================================================

[[nodiscard]] inline std::optional<UserInput> parse_UserInput(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    UserInput obj(arena);
    bool has_name = false;
    bool has_email = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "name") {
            has_name = true;
            if (auto v = cur.string()) {
                obj.name = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else         if (*key == "email") {
            has_email = true;
            if (auto v = cur.string()) {
                obj.email = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
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
    if (!has_name) return std::nullopt;
    if (!has_email) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<UserInput> parse_UserInput(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserInput(cur, arena);
}

[[nodiscard]] inline std::optional<UserInput_Name_t> parse_UserInput_Name_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return UserInput_Name_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UserInput_Name_t> parse_UserInput_Name_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserInput_Name_t(cur, arena);
}

[[nodiscard]] inline std::optional<UserInput_Email_t> parse_UserInput_Email_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return UserInput_Email_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UserInput_Email_t> parse_UserInput_Email_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserInput_Email_t(cur, arena);
}

[[nodiscard]] inline std::optional<UserInput_Age_t> parse_UserInput_Age_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return UserInput_Age_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UserInput_Age_t> parse_UserInput_Age_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserInput_Age_t(cur, arena);
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

[[nodiscard]] inline std::optional<getUser_param_id> parse_getUser_param_id(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return getUser_param_id{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<getUser_param_id> parse_getUser_param_id(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_getUser_param_id(cur, arena);
}

[[nodiscard]] inline std::optional<schema_1> parse_schema_1(katana::serde::json_cursor& cur, monotonic_arena* arena) {
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
    return schema_1{};
}

[[nodiscard]] inline std::optional<schema_1> parse_schema_1(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_1(cur, arena);
}

// ============================================================
// JSON Serialize Functions
// ============================================================

inline void serialize_UserInput_into(const UserInput& obj, std::string& json) {
    json.push_back('{');
    json.append("\"name\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.name, json);
    json.push_back('"');
    json.append(",\"email\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.email, json);
    json.push_back('"');
    json.append(",\"age\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.age);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.push_back('}');
}

inline std::string serialize_UserInput(const UserInput& obj) {
    std::string json;
    size_t reserve_estimate = 110;
    reserve_estimate += obj.name.size();
    reserve_estimate += obj.email.size();
    json.reserve(reserve_estimate);
    serialize_UserInput_into(obj, json);
    return json;
}

inline void serialize_UserInput_Name_t_into(const UserInput_Name_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_UserInput_Name_t(const UserInput_Name_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_UserInput_Name_t_into(obj, json);
    return json;
}

inline void serialize_UserInput_Email_t_into(const UserInput_Email_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_UserInput_Email_t(const UserInput_Email_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_UserInput_Email_t_into(obj, json);
    return json;
}

inline void serialize_UserInput_Age_t_into(const UserInput_Age_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_UserInput_Age_t(const UserInput_Age_t& obj) {
    std::string json;
    serialize_UserInput_Age_t_into(obj, json);
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

inline void serialize_getUser_param_id_into(const getUser_param_id& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_getUser_param_id(const getUser_param_id& obj) {
    std::string json;
    serialize_getUser_param_id_into(obj, json);
    return json;
}

inline void serialize_schema_1_into(const schema_1& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_1(const schema_1& obj) {
    (void)obj;
    return std::string("{}");
}

// ============================================================
// Array Parse Functions
// ============================================================

[[nodiscard]] inline std::optional<std::vector<UserInput>> parse_UserInput_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UserInput> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UserInput(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UserInput>> parse_UserInput_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserInput_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UserInput_Name_t>> parse_UserInput_Name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UserInput_Name_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UserInput_Name_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UserInput_Name_t>> parse_UserInput_Name_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserInput_Name_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UserInput_Email_t>> parse_UserInput_Email_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UserInput_Email_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UserInput_Email_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UserInput_Email_t>> parse_UserInput_Email_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserInput_Email_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UserInput_Age_t>> parse_UserInput_Age_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UserInput_Age_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UserInput_Age_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UserInput_Age_t>> parse_UserInput_Age_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserInput_Age_t_array(cur, arena);
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

[[nodiscard]] inline std::optional<std::vector<getUser_param_id>> parse_getUser_param_id_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<getUser_param_id> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_getUser_param_id(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<getUser_param_id>> parse_getUser_param_id_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_getUser_param_id_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_1>> parse_schema_1_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_1> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_1(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_1>> parse_schema_1_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_1_array(cur, arena);
}

// ============================================================
// Array Serialize Functions
// ============================================================

inline void serialize_UserInput_array_into(const std::vector<UserInput>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserInput_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserInput_array(const std::vector<UserInput>& arr) {
    std::string json;
    json.reserve(arr.size() * 110 + 2);
    serialize_UserInput_array_into(arr, json);
    return json;
}

inline void serialize_UserInput_array_into(const arena_vector<UserInput>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserInput_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserInput_array(const arena_vector<UserInput>& arr) {
    std::string json;
    json.reserve(arr.size() * 110 + 2);
    serialize_UserInput_array_into(arr, json);
    return json;
}

inline void serialize_UserInput_Name_t_array_into(const std::vector<UserInput_Name_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserInput_Name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserInput_Name_t_array(const std::vector<UserInput_Name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UserInput_Name_t_array_into(arr, json);
    return json;
}

inline void serialize_UserInput_Name_t_array_into(const arena_vector<UserInput_Name_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserInput_Name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserInput_Name_t_array(const arena_vector<UserInput_Name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UserInput_Name_t_array_into(arr, json);
    return json;
}

inline void serialize_UserInput_Email_t_array_into(const std::vector<UserInput_Email_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserInput_Email_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserInput_Email_t_array(const std::vector<UserInput_Email_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UserInput_Email_t_array_into(arr, json);
    return json;
}

inline void serialize_UserInput_Email_t_array_into(const arena_vector<UserInput_Email_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserInput_Email_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserInput_Email_t_array(const arena_vector<UserInput_Email_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UserInput_Email_t_array_into(arr, json);
    return json;
}

inline void serialize_UserInput_Age_t_array_into(const std::vector<UserInput_Age_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserInput_Age_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserInput_Age_t_array(const std::vector<UserInput_Age_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_UserInput_Age_t_array_into(arr, json);
    return json;
}

inline void serialize_UserInput_Age_t_array_into(const arena_vector<UserInput_Age_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserInput_Age_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserInput_Age_t_array(const arena_vector<UserInput_Age_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_UserInput_Age_t_array_into(arr, json);
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

inline void serialize_getUser_param_id_array_into(const std::vector<getUser_param_id>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_getUser_param_id_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_getUser_param_id_array(const std::vector<getUser_param_id>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_getUser_param_id_array_into(arr, json);
    return json;
}

inline void serialize_getUser_param_id_array_into(const arena_vector<getUser_param_id>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_getUser_param_id_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_getUser_param_id_array(const arena_vector<getUser_param_id>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_getUser_param_id_array_into(arr, json);
    return json;
}

inline void serialize_schema_1_array_into(const std::vector<schema_1>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_1_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_1_array(const std::vector<schema_1>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_1_array_into(arr, json);
    return json;
}

inline void serialize_schema_1_array_into(const arena_vector<schema_1>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_1_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_1_array(const arena_vector<schema_1>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_1_array_into(arr, json);
    return json;
}

