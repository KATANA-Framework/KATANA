// layer: flat
#pragma once

#include "katana/core/arena.hpp"
#include "katana/core/serde.hpp"
#include <charconv>
#include <optional>
#include <string>
#include <vector>

using katana::monotonic_arena;

inline std::optional<UserInput> parse_UserInput(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_object_start())
        return std::nullopt;

    UserInput obj(arena);
    bool has_name = false;
    bool has_email = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "name") {
            has_name = true;
            if (auto v = cur.string()) {
                obj.name = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else if (*key == "email") {
            has_email = true;
            if (auto v = cur.string()) {
                obj.email = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
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
    if (!has_name)
        return std::nullopt;
    if (!has_email)
        return std::nullopt;
    return obj;
}

inline std::string serialize_UserInput(const UserInput& obj) {
    std::string json;
    json.reserve(256);
    json.push_back('{');
    bool first = true;

    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"name\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.name));
    json.push_back('"');
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
    json.append("\"age\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.age);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.push_back('}');
    return json;
}

inline std::optional<std::vector<UserInput>> parse_UserInput_array(std::string_view json,
                                                                   monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<UserInput> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_UserInput(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::string serialize_UserInput_array(const std::vector<UserInput>& arr) {
    std::string json = "[";
    for (size_t i = 0; i < arr.size(); ++i) {
        json += serialize_UserInput(arr[i]);
        if (i < arr.size() - 1)
            json += ",";
    }
    json += "]";
    return json;
}

inline std::string serialize_UserInput_array(const arena_vector<UserInput>& arr) {
    std::string json = "[";
    for (size_t i = 0; i < arr.size(); ++i) {
        json += serialize_UserInput(arr[i]);
        if (i < arr.size() - 1)
            json += ",";
    }
    json += "]";
    return json;
}
