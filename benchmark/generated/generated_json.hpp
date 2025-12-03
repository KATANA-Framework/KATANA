#pragma once

#include "generated_dtos.hpp"
#include "katana/core/arena.hpp"
#include "katana/core/serde.hpp"
#include <optional>
#include <string>

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
    std::string json = "{";
    bool first = true;

    if (!first)
        json += ",";
    first = false;
    json += "\"" + std::string("name") + "\":";
    json += "\"" + katana::serde::escape_json_string(obj.name) + "\"";

    if (!first)
        json += ",";
    first = false;
    json += "\"" + std::string("email") + "\":";
    json += "\"" + katana::serde::escape_json_string(obj.email) + "\"";

    if (!first)
        json += ",";
    first = false;
    json += "\"" + std::string("age") + "\":";
    json += std::to_string(obj.age);

    json += "}";
    return json;
}
