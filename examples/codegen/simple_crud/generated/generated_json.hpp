#pragma once

#include "katana/core/arena.hpp"
#include "katana/core/serde.hpp"
#include <charconv>
#include <optional>
#include <string>
#include <vector>

using katana::monotonic_arena;

inline std::optional<Task> parse_Task(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_object_start())
        return std::nullopt;

    Task obj(arena);
    bool has_id = false;
    bool has_title = false;
    bool has_completed = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "id") {
            has_id = true;
            if (auto v = katana::serde::parse_size(cur)) {
                obj.id = static_cast<int64_t>(*v);
            } else {
                cur.skip_value();
            }
        } else if (*key == "title") {
            has_title = true;
            if (auto v = cur.string()) {
                obj.title = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else if (*key == "description") {
            if (auto v = cur.string()) {
                obj.description =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else if (*key == "completed") {
            has_completed = true;
            if (auto v = katana::serde::parse_bool(cur)) {
                obj.completed = *v;
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_id)
        return std::nullopt;
    if (!has_title)
        return std::nullopt;
    if (!has_completed)
        return std::nullopt;
    return obj;
}

inline std::optional<CreateTaskRequest> parse_CreateTaskRequest(std::string_view json,
                                                                monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_object_start())
        return std::nullopt;

    CreateTaskRequest obj(arena);
    bool has_title = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "title") {
            has_title = true;
            if (auto v = cur.string()) {
                obj.title = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else if (*key == "description") {
            if (auto v = cur.string()) {
                obj.description =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else if (*key == "completed") {
            if (auto v = katana::serde::parse_bool(cur)) {
                obj.completed = *v;
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_title)
        return std::nullopt;
    return obj;
}

inline std::optional<UpdateTaskRequest> parse_UpdateTaskRequest(std::string_view json,
                                                                monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_object_start())
        return std::nullopt;

    UpdateTaskRequest obj(arena);

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "title") {
            if (auto v = cur.string()) {
                obj.title = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else if (*key == "description") {
            if (auto v = cur.string()) {
                obj.description =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else if (*key == "completed") {
            if (auto v = katana::serde::parse_bool(cur)) {
                obj.completed = *v;
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return obj;
}

inline std::string serialize_Task(const Task& obj) {
    std::string json;
    json.reserve(256);
    json.push_back('{');
    bool first = true;

    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"id\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.id);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"title\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.title));
    json.push_back('"');
    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"description\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.description));
    json.push_back('"');
    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"completed\":");
    json.append(obj.completed ? "true" : "false");
    json.push_back('}');
    return json;
}

inline std::string serialize_CreateTaskRequest(const CreateTaskRequest& obj) {
    std::string json;
    json.reserve(256);
    json.push_back('{');
    bool first = true;

    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"title\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.title));
    json.push_back('"');
    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"description\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.description));
    json.push_back('"');
    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"completed\":");
    json.append(obj.completed ? "true" : "false");
    json.push_back('}');
    return json;
}

inline std::string serialize_UpdateTaskRequest(const UpdateTaskRequest& obj) {
    std::string json;
    json.reserve(256);
    json.push_back('{');
    bool first = true;

    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"title\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.title));
    json.push_back('"');
    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"description\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.description));
    json.push_back('"');
    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"completed\":");
    json.append(obj.completed ? "true" : "false");
    json.push_back('}');
    return json;
}

inline std::optional<std::vector<Task>> parse_Task_array(std::string_view json,
                                                         monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<Task> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_Task(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<CreateTaskRequest>>
parse_CreateTaskRequest_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<CreateTaskRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_CreateTaskRequest(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<UpdateTaskRequest>>
parse_UpdateTaskRequest_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<UpdateTaskRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_UpdateTaskRequest(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::string serialize_Task_array(const std::vector<Task>& arr) {
    std::string json = "[";
    for (size_t i = 0; i < arr.size(); ++i) {
        json += serialize_Task(arr[i]);
        if (i < arr.size() - 1)
            json += ",";
    }
    json += "]";
    return json;
}

inline std::string serialize_CreateTaskRequest_array(const std::vector<CreateTaskRequest>& arr) {
    std::string json = "[";
    for (size_t i = 0; i < arr.size(); ++i) {
        json += serialize_CreateTaskRequest(arr[i]);
        if (i < arr.size() - 1)
            json += ",";
    }
    json += "]";
    return json;
}

inline std::string serialize_UpdateTaskRequest_array(const std::vector<UpdateTaskRequest>& arr) {
    std::string json = "[";
    for (size_t i = 0; i < arr.size(); ++i) {
        json += serialize_UpdateTaskRequest(arr[i]);
        if (i < arr.size() - 1)
            json += ",";
    }
    json += "]";
    return json;
}
