// layer: flat
#pragma once

#include "katana/core/arena.hpp"
#include "katana/core/serde.hpp"
#include <charconv>
#include <optional>
#include <string>
#include <vector>

using katana::monotonic_arena;

inline std::optional<RegisterUserRequest> parse_RegisterUserRequest(std::string_view json,
                                                                    monotonic_arena* arena);
inline std::optional<RegisterUserRequest_Email_t>
parse_RegisterUserRequest_Email_t(std::string_view json, monotonic_arena* arena);
inline std::optional<RegisterUserRequest_Password_t>
parse_RegisterUserRequest_Password_t(std::string_view json, monotonic_arena* arena);
inline std::optional<RegisterUserRequest_Age_t>
parse_RegisterUserRequest_Age_t(std::string_view json, monotonic_arena* arena);
inline std::optional<register_user_resp_200_0>
parse_register_user_resp_200_0(std::string_view json, monotonic_arena* arena);

inline std::string serialize_RegisterUserRequest(const RegisterUserRequest& obj);
inline std::string serialize_RegisterUserRequest_Email_t(const RegisterUserRequest_Email_t& obj);
inline std::string
serialize_RegisterUserRequest_Password_t(const RegisterUserRequest_Password_t& obj);
inline std::string serialize_RegisterUserRequest_Age_t(const RegisterUserRequest_Age_t& obj);
inline std::string serialize_register_user_resp_200_0(const register_user_resp_200_0& obj);

inline std::optional<std::vector<RegisterUserRequest>>
parse_RegisterUserRequest_array(std::string_view json, monotonic_arena* arena);
inline std::optional<std::vector<RegisterUserRequest_Email_t>>
parse_RegisterUserRequest_Email_t_array(std::string_view json, monotonic_arena* arena);
inline std::optional<std::vector<RegisterUserRequest_Password_t>>
parse_RegisterUserRequest_Password_t_array(std::string_view json, monotonic_arena* arena);
inline std::optional<std::vector<RegisterUserRequest_Age_t>>
parse_RegisterUserRequest_Age_t_array(std::string_view json, monotonic_arena* arena);
inline std::optional<std::vector<register_user_resp_200_0>>
parse_register_user_resp_200_0_array(std::string_view json, monotonic_arena* arena);

inline std::string serialize_RegisterUserRequest_array(const std::vector<RegisterUserRequest>& arr);
inline std::string
serialize_RegisterUserRequest_array(const arena_vector<RegisterUserRequest>& arr);
inline std::string
serialize_RegisterUserRequest_Email_t_array(const std::vector<RegisterUserRequest_Email_t>& arr);
inline std::string
serialize_RegisterUserRequest_Email_t_array(const arena_vector<RegisterUserRequest_Email_t>& arr);
inline std::string serialize_RegisterUserRequest_Password_t_array(
    const std::vector<RegisterUserRequest_Password_t>& arr);
inline std::string serialize_RegisterUserRequest_Password_t_array(
    const arena_vector<RegisterUserRequest_Password_t>& arr);
inline std::string
serialize_RegisterUserRequest_Age_t_array(const std::vector<RegisterUserRequest_Age_t>& arr);
inline std::string
serialize_RegisterUserRequest_Age_t_array(const arena_vector<RegisterUserRequest_Age_t>& arr);
inline std::string
serialize_register_user_resp_200_0_array(const std::vector<register_user_resp_200_0>& arr);
inline std::string
serialize_register_user_resp_200_0_array(const arena_vector<register_user_resp_200_0>& arr);

inline std::optional<RegisterUserRequest> parse_RegisterUserRequest(std::string_view json,
                                                                    monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_object_start())
        return std::nullopt;

    RegisterUserRequest obj(arena);
    bool has_email = false;
    bool has_password = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "email") {
            has_email = true;
            if (auto v = cur.string()) {
                obj.email = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else if (*key == "password") {
            has_password = true;
            if (auto v = cur.string()) {
                obj.password = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else if (*key == "age") {
            if (auto v = katana::serde::parse_size(cur)) {
                obj.age = static_cast<int64_t>(*v);
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_email)
        return std::nullopt;
    if (!has_password)
        return std::nullopt;
    return obj;
}

inline std::optional<RegisterUserRequest_Email_t>
parse_RegisterUserRequest_Email_t(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (auto v = cur.string()) {
        return RegisterUserRequest_Email_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

inline std::optional<RegisterUserRequest_Password_t>
parse_RegisterUserRequest_Password_t(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (auto v = cur.string()) {
        return RegisterUserRequest_Password_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

inline std::optional<RegisterUserRequest_Age_t>
parse_RegisterUserRequest_Age_t(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    (void)arena;
    if (auto v = katana::serde::parse_size(cur))
        return RegisterUserRequest_Age_t{static_cast<int64_t>(*v)};
    return std::nullopt;
}

inline std::optional<register_user_resp_200_0>
parse_register_user_resp_200_0(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (auto v = cur.string()) {
        return register_user_resp_200_0{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

inline std::string serialize_RegisterUserRequest(const RegisterUserRequest& obj) {
    std::string json;
    json.reserve(256);
    json.push_back('{');
    bool first = true;

    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"email\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.email));
    json.push_back('"');
    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"password\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.password));
    json.push_back('"');
    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"age\":");
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
    return json;
}

inline std::string serialize_RegisterUserRequest_Email_t(const RegisterUserRequest_Email_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj));
    json.push_back('"');
    return json;
}

inline std::string
serialize_RegisterUserRequest_Password_t(const RegisterUserRequest_Password_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj));
    json.push_back('"');
    return json;
}

inline std::string serialize_RegisterUserRequest_Age_t(const RegisterUserRequest_Age_t& obj) {
    if (!obj)
        return std::string("null");
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj);
    return std::string(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_register_user_resp_200_0(const register_user_resp_200_0& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj));
    json.push_back('"');
    return json;
}

inline std::optional<std::vector<RegisterUserRequest>>
parse_RegisterUserRequest_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<RegisterUserRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_RegisterUserRequest(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<RegisterUserRequest_Email_t>>
parse_RegisterUserRequest_Email_t_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<RegisterUserRequest_Email_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_RegisterUserRequest_Email_t(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<RegisterUserRequest_Password_t>>
parse_RegisterUserRequest_Password_t_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<RegisterUserRequest_Password_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_RegisterUserRequest_Password_t(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<RegisterUserRequest_Age_t>>
parse_RegisterUserRequest_Age_t_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<RegisterUserRequest_Age_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_RegisterUserRequest_Age_t(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<register_user_resp_200_0>>
parse_register_user_resp_200_0_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<register_user_resp_200_0> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_register_user_resp_200_0(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::string
serialize_RegisterUserRequest_array(const std::vector<RegisterUserRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_RegisterUserRequest(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_RegisterUserRequest_array(const arena_vector<RegisterUserRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_RegisterUserRequest(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_RegisterUserRequest_Email_t_array(const std::vector<RegisterUserRequest_Email_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_RegisterUserRequest_Email_t(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_RegisterUserRequest_Email_t_array(const arena_vector<RegisterUserRequest_Email_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_RegisterUserRequest_Email_t(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_RegisterUserRequest_Password_t_array(
    const std::vector<RegisterUserRequest_Password_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_RegisterUserRequest_Password_t(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_RegisterUserRequest_Password_t_array(
    const arena_vector<RegisterUserRequest_Password_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_RegisterUserRequest_Password_t(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_RegisterUserRequest_Age_t_array(const std::vector<RegisterUserRequest_Age_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_RegisterUserRequest_Age_t(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_RegisterUserRequest_Age_t_array(const arena_vector<RegisterUserRequest_Age_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_RegisterUserRequest_Age_t(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_register_user_resp_200_0_array(const std::vector<register_user_resp_200_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_register_user_resp_200_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_register_user_resp_200_0_array(const arena_vector<register_user_resp_200_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_register_user_resp_200_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}
