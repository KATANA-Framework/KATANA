// layer: flat
#pragma once

#include "katana/core/arena.hpp"
#include "katana/core/serde.hpp"
#include <charconv>
#include <optional>
#include <string>
#include <vector>

using katana::monotonic_arena;

inline std::optional<compute_sum_body_0> parse_compute_sum_body_0(std::string_view json,
                                                                  monotonic_arena* arena);
inline std::optional<schema> parse_schema(std::string_view json, monotonic_arena* arena);
inline std::optional<compute_sum_resp_200_0> parse_compute_sum_resp_200_0(std::string_view json,
                                                                          monotonic_arena* arena);

inline std::string serialize_compute_sum_body_0(const compute_sum_body_0& obj);
inline std::string serialize_schema(const schema& obj);
inline std::string serialize_compute_sum_resp_200_0(const compute_sum_resp_200_0& obj);

inline std::optional<std::vector<compute_sum_body_0>>
parse_compute_sum_body_0_array(std::string_view json, monotonic_arena* arena);
inline std::optional<std::vector<schema>> parse_schema_array(std::string_view json,
                                                             monotonic_arena* arena);
inline std::optional<std::vector<compute_sum_resp_200_0>>
parse_compute_sum_resp_200_0_array(std::string_view json, monotonic_arena* arena);

inline std::string serialize_compute_sum_body_0_array(const std::vector<compute_sum_body_0>& arr);
inline std::string serialize_compute_sum_body_0_array(const arena_vector<compute_sum_body_0>& arr);
inline std::string serialize_schema_array(const std::vector<schema>& arr);
inline std::string serialize_schema_array(const arena_vector<schema>& arr);
inline std::string
serialize_compute_sum_resp_200_0_array(const std::vector<compute_sum_resp_200_0>& arr);
inline std::string
serialize_compute_sum_resp_200_0_array(const arena_vector<compute_sum_resp_200_0>& arr);

inline std::optional<compute_sum_body_0> parse_compute_sum_body_0(std::string_view json,
                                                                  monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;
    compute_sum_body_0 result{arena_allocator<schema>(arena)};
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

inline std::optional<schema> parse_schema(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    (void)arena;
    if (auto v = katana::serde::parse_double(cur))
        return schema{*v};
    return std::nullopt;
}

inline std::optional<compute_sum_resp_200_0> parse_compute_sum_resp_200_0(std::string_view json,
                                                                          monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    (void)arena;
    if (auto v = katana::serde::parse_double(cur))
        return compute_sum_resp_200_0{*v};
    return std::nullopt;
}

inline std::string serialize_compute_sum_body_0(const compute_sum_body_0& obj) {
    const auto& arr = obj;
    std::string json;
    json.reserve(arr.size() * 16 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        json.append(serialize_schema(arr[i]));
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema(const schema& obj) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc())
        return std::string(buf, static_cast<size_t>(res.ptr - buf));
    return {};
}

inline std::string serialize_compute_sum_resp_200_0(const compute_sum_resp_200_0& obj) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc())
        return std::string(buf, static_cast<size_t>(res.ptr - buf));
    return {};
}

inline std::optional<std::vector<compute_sum_body_0>>
parse_compute_sum_body_0_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<compute_sum_body_0> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_compute_sum_body_0(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<schema>> parse_schema_array(std::string_view json,
                                                             monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_schema(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<compute_sum_resp_200_0>>
parse_compute_sum_resp_200_0_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<compute_sum_resp_200_0> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_compute_sum_resp_200_0(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::string serialize_compute_sum_body_0_array(const std::vector<compute_sum_body_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_compute_sum_body_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_compute_sum_body_0_array(const arena_vector<compute_sum_body_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_compute_sum_body_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_array(const std::vector<schema>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_array(const arena_vector<schema>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_compute_sum_resp_200_0_array(const std::vector<compute_sum_resp_200_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_compute_sum_resp_200_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_compute_sum_resp_200_0_array(const arena_vector<compute_sum_resp_200_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_compute_sum_resp_200_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}
