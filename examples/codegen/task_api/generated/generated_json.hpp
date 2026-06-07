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

[[nodiscard]] inline std::optional<Task> parse_Task(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<User> parse_User(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateTaskRequest> parse_CreateTaskRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest> parse_UpdateTaskRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateRequest> parse_BatchCreateRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateResponse> parse_BatchCreateResponse(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest> parse_SearchRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TaskList> parse_TaskList(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<HealthResponse> parse_HealthResponse(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ProblemDetails> parse_ProblemDetails(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<listTasks_param_status> parse_listTasks_param_status(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<Task> parse_Task(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Status_t> parse_Task_Status_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Metadata_t> parse_Task_Metadata_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<User> parse_User(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateTaskRequest> parse_CreateTaskRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest> parse_UpdateTaskRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest_Status_t> parse_UpdateTaskRequest_Status_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateRequest> parse_BatchCreateRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateResponse> parse_BatchCreateResponse(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateResponse_Item_t_1> parse_BatchCreateResponse_Item_t_1(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest> parse_SearchRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Item_t> parse_SearchRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TaskList> parse_TaskList(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<HealthResponse> parse_HealthResponse(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<HealthResponse_Status_t> parse_HealthResponse_Status_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ProblemDetails> parse_ProblemDetails(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<listTasks_param_status> parse_listTasks_param_status(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_Task_into(const Task& obj, std::string& out);
inline void serialize_Task_Status_t_into(const Task_Status_t& obj, std::string& out);
inline void serialize_Task_Metadata_t_into(const Task_Metadata_t& obj, std::string& out);
inline void serialize_User_into(const User& obj, std::string& out);
inline void serialize_CreateTaskRequest_into(const CreateTaskRequest& obj, std::string& out);
inline void serialize_UpdateTaskRequest_into(const UpdateTaskRequest& obj, std::string& out);
inline void serialize_UpdateTaskRequest_Status_t_into(const UpdateTaskRequest_Status_t& obj, std::string& out);
inline void serialize_BatchCreateRequest_into(const BatchCreateRequest& obj, std::string& out);
inline void serialize_BatchCreateResponse_into(const BatchCreateResponse& obj, std::string& out);
inline void serialize_BatchCreateResponse_Item_t_1_into(const BatchCreateResponse_Item_t_1& obj, std::string& out);
inline void serialize_SearchRequest_into(const SearchRequest& obj, std::string& out);
inline void serialize_SearchRequest_Item_t_into(const SearchRequest_Item_t& obj, std::string& out);
inline void serialize_TaskList_into(const TaskList& obj, std::string& out);
inline void serialize_HealthResponse_into(const HealthResponse& obj, std::string& out);
inline void serialize_HealthResponse_Status_t_into(const HealthResponse_Status_t& obj, std::string& out);
inline void serialize_ProblemDetails_into(const ProblemDetails& obj, std::string& out);
inline void serialize_listTasks_param_status_into(const listTasks_param_status& obj, std::string& out);

inline std::string serialize_Task(const Task& obj);
inline std::string serialize_Task_Status_t(const Task_Status_t& obj);
inline std::string serialize_Task_Metadata_t(const Task_Metadata_t& obj);
inline std::string serialize_User(const User& obj);
inline std::string serialize_CreateTaskRequest(const CreateTaskRequest& obj);
inline std::string serialize_UpdateTaskRequest(const UpdateTaskRequest& obj);
inline std::string serialize_UpdateTaskRequest_Status_t(const UpdateTaskRequest_Status_t& obj);
inline std::string serialize_BatchCreateRequest(const BatchCreateRequest& obj);
inline std::string serialize_BatchCreateResponse(const BatchCreateResponse& obj);
inline std::string serialize_BatchCreateResponse_Item_t_1(const BatchCreateResponse_Item_t_1& obj);
inline std::string serialize_SearchRequest(const SearchRequest& obj);
inline std::string serialize_SearchRequest_Item_t(const SearchRequest_Item_t& obj);
inline std::string serialize_TaskList(const TaskList& obj);
inline std::string serialize_HealthResponse(const HealthResponse& obj);
inline std::string serialize_HealthResponse_Status_t(const HealthResponse_Status_t& obj);
inline std::string serialize_ProblemDetails(const ProblemDetails& obj);
inline std::string serialize_listTasks_param_status(const listTasks_param_status& obj);

// ============================================================
// JSON Parse Functions
// ============================================================

// parse Task — object, 11 field(s)  ← api.yaml:206
[[nodiscard]] inline std::optional<Task> parse_Task(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    Task obj(arena);
    bool has_id = false;
    bool has_title = false;
    bool has_status = false;
    bool has_priority = false;
    bool has_created_at = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 2:
            if (*key == "id") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.id = *v;
                    has_id = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 4:
            if (*key == "tags") {
                if (cur.try_array_start()) {
                    obj.tags.emplace(arena);
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end()) break;
                        if (auto v = cur.string()) {
                            (*obj.tags).push_back(katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena)));
                        } else { cur.skip_value(); }
                        cur.try_comma();
                    }
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 5:
            if (*key == "title") {
                if (auto v = cur.string()) {
                    obj.title = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_title = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 6:
            if (*key == "status") {
                if (auto v = cur.string()) {
                    std::string ev_scratch_;
                    auto enum_val = Task_Status_t_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
                    if (enum_val) { obj.status = *enum_val;
                    has_status = true;
                    }
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "priority") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.priority = *v;
                    has_priority = true;
                } else { cur.skip_value(); }
            } else if (*key == "assignee") {
                if (auto nested = parse_User(cur, arena)) {
                    obj.assignee = std::move(*nested);
                } else { cur.skip_value(); }
            } else if (*key == "due_date") {
                if (auto v = cur.string()) {
                    obj.due_date = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else if (*key == "metadata") {
                if (auto nested = parse_Task_Metadata_t(cur, arena)) {
                    obj.metadata = std::move(*nested);
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 10:
            if (*key == "created_at") {
                if (auto v = cur.string()) {
                    obj.created_at = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_created_at = true;
                } else { cur.skip_value(); }
            } else if (*key == "updated_at") {
                if (auto v = cur.string()) {
                    obj.updated_at = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 11:
            if (*key == "description") {
                if (auto v = cur.string()) {
                    obj.description = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_id) return std::nullopt;
    if (!has_title) return std::nullopt;
    if (!has_status) return std::nullopt;
    if (!has_priority) return std::nullopt;
    if (!has_created_at) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<Task> parse_Task(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_Task(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse Task_Status_t — enum, field Task.status  ← api.yaml:226
[[nodiscard]] inline std::optional<Task_Status_t> parse_Task_Status_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = cur.string()) {
        std::string ev_scratch_;
        return Task_Status_t_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
    }
    return std::nullopt;
}

// parse Task_Metadata_t — object, field Task.metadata  ← api.yaml:254
[[nodiscard]] inline std::optional<Task_Metadata_t> parse_Task_Metadata_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    cur.skip_ws();
    const char* ff_start_ = cur.ptr;
    cur.skip_value();
    return Task_Metadata_t(ff_start_, cur.ptr, arena_allocator<char>(arena));
}

// parse User — object, 3 field(s)  ← api.yaml:258
[[nodiscard]] inline std::optional<User> parse_User(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    User obj(arena);
    bool has_id = false;
    bool has_email = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "id") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.id = *v;
                has_id = true;
            } else { cur.skip_value(); }
        } else         if (*key == "email") {
            if (auto v = cur.string()) {
                obj.email = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                has_email = true;
            } else { cur.skip_value(); }
        } else         if (*key == "name") {
            if (auto v = cur.string()) {
                obj.name = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_id) return std::nullopt;
    if (!has_email) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<User> parse_User(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_User(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse CreateTaskRequest — object, 6 field(s)  ← api.yaml:278
[[nodiscard]] inline std::optional<CreateTaskRequest> parse_CreateTaskRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    CreateTaskRequest obj(arena);
    bool has_title = false;
    bool has_priority = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 4:
            if (*key == "tags") {
                if (cur.try_array_start()) {
                    obj.tags.emplace(arena);
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end()) break;
                        if (auto v = cur.string()) {
                            (*obj.tags).push_back(katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena)));
                        } else { cur.skip_value(); }
                        cur.try_comma();
                    }
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 5:
            if (*key == "title") {
                if (auto v = cur.string()) {
                    obj.title = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_title = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "priority") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.priority = *v;
                    has_priority = true;
                } else { cur.skip_value(); }
            } else if (*key == "due_date") {
                if (auto v = cur.string()) {
                    obj.due_date = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 11:
            if (*key == "description") {
                if (auto v = cur.string()) {
                    obj.description = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else if (*key == "assignee_id") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.assignee_id = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_title) return std::nullopt;
    if (!has_priority) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<CreateTaskRequest> parse_CreateTaskRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_CreateTaskRequest(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse UpdateTaskRequest — object, 7 field(s)  ← api.yaml:313
[[nodiscard]] inline std::optional<UpdateTaskRequest> parse_UpdateTaskRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    UpdateTaskRequest obj(arena);

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 4:
            if (*key == "tags") {
                if (cur.try_array_start()) {
                    obj.tags.emplace(arena);
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end()) break;
                        if (auto v = cur.string()) {
                            (*obj.tags).push_back(katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena)));
                        } else { cur.skip_value(); }
                        cur.try_comma();
                    }
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 5:
            if (*key == "title") {
                if (auto v = cur.string()) {
                    obj.title = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 6:
            if (*key == "status") {
                if (auto v = cur.string()) {
                    std::string ev_scratch_;
                    auto enum_val = UpdateTaskRequest_Status_t_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
                    if (enum_val) { obj.status = *enum_val;
                    }
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "priority") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.priority = *v;
                } else { cur.skip_value(); }
            } else if (*key == "due_date") {
                if (auto v = cur.string()) {
                    obj.due_date = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 11:
            if (*key == "description") {
                if (auto v = cur.string()) {
                    obj.description = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else if (*key == "assignee_id") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.assignee_id = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<UpdateTaskRequest> parse_UpdateTaskRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_UpdateTaskRequest(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse UpdateTaskRequest_Status_t — enum, field UpdateTaskRequest.status  ← api.yaml:323
[[nodiscard]] inline std::optional<UpdateTaskRequest_Status_t> parse_UpdateTaskRequest_Status_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = cur.string()) {
        std::string ev_scratch_;
        return UpdateTaskRequest_Status_t_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
    }
    return std::nullopt;
}

// parse BatchCreateRequest — object, 1 field(s)  ← api.yaml:348
[[nodiscard]] inline std::optional<BatchCreateRequest> parse_BatchCreateRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    BatchCreateRequest obj(arena);
    bool has_tasks = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "tasks") {
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) break;
                    if (auto nested = parse_CreateTaskRequest(cur, arena)) { obj.tasks.push_back(std::move(*nested)); }
                    else { cur.skip_value(); }
                    cur.try_comma();
                }
                has_tasks = true;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_tasks) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<BatchCreateRequest> parse_BatchCreateRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_BatchCreateRequest(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse BatchCreateResponse — object, 2 field(s)  ← api.yaml:360
[[nodiscard]] inline std::optional<BatchCreateResponse> parse_BatchCreateResponse(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    BatchCreateResponse obj(arena);
    bool has_created = false;
    bool has_failed = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "created") {
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) break;
                    if (auto nested = parse_Task(cur, arena)) { obj.created.push_back(std::move(*nested)); }
                    else { cur.skip_value(); }
                    cur.try_comma();
                }
                has_created = true;
            } else { cur.skip_value(); }
        } else         if (*key == "failed") {
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) break;
                    if (auto nested = parse_BatchCreateResponse_Item_t_1(cur, arena)) { obj.failed.push_back(std::move(*nested)); }
                    else { cur.skip_value(); }
                    cur.try_comma();
                }
                has_failed = true;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_created) return std::nullopt;
    if (!has_failed) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<BatchCreateResponse> parse_BatchCreateResponse(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_BatchCreateResponse(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse BatchCreateResponse_Item_t_1 — object, field BatchCreateResponse.item  ← api.yaml:372
[[nodiscard]] inline std::optional<BatchCreateResponse_Item_t_1> parse_BatchCreateResponse_Item_t_1(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    BatchCreateResponse_Item_t_1 obj(arena);
    bool has_index = false;
    bool has_error = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "index") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.index = *v;
                has_index = true;
            } else { cur.skip_value(); }
        } else         if (*key == "error") {
            if (auto v = cur.string()) {
                obj.error = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                has_error = true;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_index) return std::nullopt;
    if (!has_error) return std::nullopt;
    return obj;
}

// parse SearchRequest — object, 8 field(s)  ← api.yaml:383
[[nodiscard]] inline std::optional<SearchRequest> parse_SearchRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    SearchRequest obj(arena);

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 4:
            if (*key == "tags") {
                if (cur.try_array_start()) {
                    obj.tags.emplace(arena);
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end()) break;
                        if (auto v = cur.string()) {
                            (*obj.tags).push_back(katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena)));
                        } else { cur.skip_value(); }
                        cur.try_comma();
                    }
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "statuses") {
                if (cur.try_array_start()) {
                    obj.statuses.emplace(arena);
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end()) break;
                        if (auto v = cur.string()) {
                            std::string ev_scratch_;
                            auto enum_val = SearchRequest_Item_t_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
                            if (enum_val) (*obj.statuses).push_back(*enum_val);
                        } else { cur.skip_value(); }
                        cur.try_comma();
                    }
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 12:
            if (*key == "min_priority") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.min_priority = *v;
                } else { cur.skip_value(); }
            } else if (*key == "max_priority") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.max_priority = *v;
                } else { cur.skip_value(); }
            } else if (*key == "has_assignee") {
                if (auto v = katana::serde::parse_bool(cur)) {
                    obj.has_assignee = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 13:
            if (*key == "created_after") {
                if (auto v = cur.string()) {
                    obj.created_after = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 14:
            if (*key == "title_contains") {
                if (auto v = cur.string()) {
                    obj.title_contains = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else if (*key == "created_before") {
                if (auto v = cur.string()) {
                    obj.created_before = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<SearchRequest> parse_SearchRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_SearchRequest(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse SearchRequest_Item_t — enum, field SearchRequest.item  ← api.yaml:392
[[nodiscard]] inline std::optional<SearchRequest_Item_t> parse_SearchRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = cur.string()) {
        std::string ev_scratch_;
        return SearchRequest_Item_t_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
    }
    return std::nullopt;
}

// parse TaskList — object, 3 field(s)  ← api.yaml:420
[[nodiscard]] inline std::optional<TaskList> parse_TaskList(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    TaskList obj(arena);
    bool has_tasks = false;
    bool has_total = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "tasks") {
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) break;
                    if (auto nested = parse_Task(cur, arena)) { obj.tasks.push_back(std::move(*nested)); }
                    else { cur.skip_value(); }
                    cur.try_comma();
                }
                has_tasks = true;
            } else { cur.skip_value(); }
        } else         if (*key == "total") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.total = *v;
                has_total = true;
            } else { cur.skip_value(); }
        } else         if (*key == "has_more") {
            if (auto v = katana::serde::parse_bool(cur)) {
                obj.has_more = *v;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_tasks) return std::nullopt;
    if (!has_total) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<TaskList> parse_TaskList(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_TaskList(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse HealthResponse — object, 4 field(s)  ← api.yaml:436
[[nodiscard]] inline std::optional<HealthResponse> parse_HealthResponse(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    HealthResponse obj(arena);
    bool has_status = false;
    bool has_timestamp = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 6:
            if (*key == "status") {
                if (auto v = cur.string()) {
                    std::string ev_scratch_;
                    auto enum_val = HealthResponse_Status_t_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
                    if (enum_val) { obj.status = *enum_val;
                    has_status = true;
                    }
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 9:
            if (*key == "timestamp") {
                if (auto v = cur.string()) {
                    obj.timestamp = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_timestamp = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 14:
            if (*key == "uptime_seconds") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.uptime_seconds = *v;
                } else { cur.skip_value(); }
            } else if (*key == "total_requests") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.total_requests = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_status) return std::nullopt;
    if (!has_timestamp) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<HealthResponse> parse_HealthResponse(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_HealthResponse(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse HealthResponse_Status_t — enum, field HealthResponse.status  ← api.yaml:442
[[nodiscard]] inline std::optional<HealthResponse_Status_t> parse_HealthResponse_Status_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = cur.string()) {
        std::string ev_scratch_;
        return HealthResponse_Status_t_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
    }
    return std::nullopt;
}

// parse ProblemDetails — object, 5 field(s)  ← api.yaml:455
[[nodiscard]] inline std::optional<ProblemDetails> parse_ProblemDetails(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    ProblemDetails obj(arena);
    bool has_type = false;
    bool has_title = false;
    bool has_status = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 4:
            if (*key == "type") {
                if (auto v = cur.string()) {
                    obj.type = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_type = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 5:
            if (*key == "title") {
                if (auto v = cur.string()) {
                    obj.title = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_title = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 6:
            if (*key == "status") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.status = *v;
                    has_status = true;
                } else { cur.skip_value(); }
            } else if (*key == "detail") {
                if (auto v = cur.string()) {
                    obj.detail = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "instance") {
                if (auto v = cur.string()) {
                    obj.instance = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_type) return std::nullopt;
    if (!has_title) return std::nullopt;
    if (!has_status) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<ProblemDetails> parse_ProblemDetails(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_ProblemDetails(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse listTasks_param_status — enum  ← api.yaml:22
[[nodiscard]] inline std::optional<listTasks_param_status> parse_listTasks_param_status(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = cur.string()) {
        std::string ev_scratch_;
        return listTasks_param_status_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<listTasks_param_status> parse_listTasks_param_status(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_listTasks_param_status(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// ============================================================
// JSON Serialize Functions
// ============================================================

// serialize Task — object, 11 field(s)  ← api.yaml:206
inline void serialize_Task_into(const Task& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"id\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.id);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"title\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.title, json);
    json.push_back('"');
    if (obj.description) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"description\":");
    if (obj.description) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.description, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"status\":");
    json.push_back('"');
    json.append(to_string(obj.status));
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"priority\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.priority);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (obj.tags) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"tags\":");
    if (!obj.tags) {
        json.append("null");
    } else {
    json.push_back('[');
    for (size_t i = 0; i < obj.tags->size(); ++i) {
        if (i > 0) json.push_back(',');
        json.push_back('"');
        katana::serde::escape_json_string_into((*obj.tags)[i], json);
        json.push_back('"');
    }
    json.push_back(']');
    }
    }
    if (obj.assignee) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"assignee\":");
    if (obj.assignee) {
        serialize_User_into(*obj.assignee, json);
    } else {
        json.append("null");
    }
    }
    if (obj.due_date) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"due_date\":");
    if (obj.due_date) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.due_date, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"created_at\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.created_at, json);
    json.push_back('"');
    if (obj.updated_at) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"updated_at\":");
    if (obj.updated_at) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.updated_at, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    if (obj.metadata) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"metadata\":");
    if (obj.metadata) {
        serialize_Task_Metadata_t_into(*obj.metadata, json);
    } else {
        json.append("null");
    }
    }
    json.push_back('}');
}

inline std::string serialize_Task(const Task& obj) {
    std::string json;
    size_t reserve_estimate = 678;
    reserve_estimate += obj.title.size();
    if (obj.description) reserve_estimate += obj.description->size();
    if (obj.tags) reserve_estimate += obj.tags->size() * 32;
    if (obj.due_date) reserve_estimate += obj.due_date->size();
    reserve_estimate += obj.created_at.size();
    if (obj.updated_at) reserve_estimate += obj.updated_at->size();
    json.reserve(reserve_estimate);
    serialize_Task_into(obj, json);
    return json;
}

// serialize Task_Status_t — enum, field Task.status  ← api.yaml:226
inline void serialize_Task_Status_t_into(const Task_Status_t& obj, std::string& json) {
    auto str = to_string(obj);
    json.push_back('"');
    json.append(str);
    json.push_back('"');
}

inline std::string serialize_Task_Status_t(const Task_Status_t& obj) {
    std::string json;
    auto str = to_string(obj);
    json.reserve(str.size() + 2);
    serialize_Task_Status_t_into(obj, json);
    return json;
}

// serialize Task_Metadata_t — object, field Task.metadata  ← api.yaml:254
inline void serialize_Task_Metadata_t_into(const Task_Metadata_t& obj, std::string& json) {
    if (obj.empty()) { json.append("null"); }
    else { json.append(obj.data(), obj.size()); }
}

inline std::string serialize_Task_Metadata_t(const Task_Metadata_t& obj) {
    std::string json;
    json.reserve(obj.size() + 4);
    serialize_Task_Metadata_t_into(obj, json);
    return json;
}

// serialize User — object, 3 field(s)  ← api.yaml:258
inline void serialize_User_into(const User& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"id\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.id);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"email\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.email, json);
    json.push_back('"');
    if (obj.name) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"name\":");
    if (obj.name) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.name, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    json.push_back('}');
}

inline std::string serialize_User(const User& obj) {
    std::string json;
    size_t reserve_estimate = 109;
    reserve_estimate += obj.email.size();
    if (obj.name) reserve_estimate += obj.name->size();
    json.reserve(reserve_estimate);
    serialize_User_into(obj, json);
    return json;
}

// serialize CreateTaskRequest — object, 6 field(s)  ← api.yaml:278
inline void serialize_CreateTaskRequest_into(const CreateTaskRequest& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"title\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.title, json);
    json.push_back('"');
    if (obj.description) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"description\":");
    if (obj.description) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.description, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"priority\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.priority);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (obj.tags) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"tags\":");
    if (!obj.tags) {
        json.append("null");
    } else {
    json.push_back('[');
    for (size_t i = 0; i < obj.tags->size(); ++i) {
        if (i > 0) json.push_back(',');
        json.push_back('"');
        katana::serde::escape_json_string_into((*obj.tags)[i], json);
        json.push_back('"');
    }
    json.push_back(']');
    }
    }
    if (obj.assignee_id) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"assignee_id\":");
    {
        if (!obj.assignee_id) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.assignee_id);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    if (obj.due_date) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"due_date\":");
    if (obj.due_date) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.due_date, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    json.push_back('}');
}

inline std::string serialize_CreateTaskRequest(const CreateTaskRequest& obj) {
    std::string json;
    size_t reserve_estimate = 273;
    reserve_estimate += obj.title.size();
    if (obj.description) reserve_estimate += obj.description->size();
    if (obj.tags) reserve_estimate += obj.tags->size() * 32;
    if (obj.due_date) reserve_estimate += obj.due_date->size();
    json.reserve(reserve_estimate);
    serialize_CreateTaskRequest_into(obj, json);
    return json;
}

// serialize UpdateTaskRequest — object, 7 field(s)  ← api.yaml:313
inline void serialize_UpdateTaskRequest_into(const UpdateTaskRequest& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (obj.title) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"title\":");
    if (obj.title) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.title, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    if (obj.description) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"description\":");
    if (obj.description) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.description, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    if (obj.status) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"status\":");
    if (obj.status) {
        json.push_back('"');
        json.append(to_string(*obj.status));
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    if (obj.priority) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"priority\":");
    {
        if (!obj.priority) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.priority);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    if (obj.tags) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"tags\":");
    if (!obj.tags) {
        json.append("null");
    } else {
    json.push_back('[');
    for (size_t i = 0; i < obj.tags->size(); ++i) {
        if (i > 0) json.push_back(',');
        json.push_back('"');
        katana::serde::escape_json_string_into((*obj.tags)[i], json);
        json.push_back('"');
    }
    json.push_back(']');
    }
    }
    if (obj.assignee_id) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"assignee_id\":");
    {
        if (!obj.assignee_id) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.assignee_id);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    if (obj.due_date) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"due_date\":");
    if (obj.due_date) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.due_date, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    json.push_back('}');
}

inline std::string serialize_UpdateTaskRequest(const UpdateTaskRequest& obj) {
    std::string json;
    size_t reserve_estimate = 315;
    if (obj.title) reserve_estimate += obj.title->size();
    if (obj.description) reserve_estimate += obj.description->size();
    if (obj.tags) reserve_estimate += obj.tags->size() * 32;
    if (obj.due_date) reserve_estimate += obj.due_date->size();
    json.reserve(reserve_estimate);
    serialize_UpdateTaskRequest_into(obj, json);
    return json;
}

// serialize UpdateTaskRequest_Status_t — enum, field UpdateTaskRequest.status  ← api.yaml:323
inline void serialize_UpdateTaskRequest_Status_t_into(const UpdateTaskRequest_Status_t& obj, std::string& json) {
    auto str = to_string(obj);
    json.push_back('"');
    json.append(str);
    json.push_back('"');
}

inline std::string serialize_UpdateTaskRequest_Status_t(const UpdateTaskRequest_Status_t& obj) {
    std::string json;
    auto str = to_string(obj);
    json.reserve(str.size() + 2);
    serialize_UpdateTaskRequest_Status_t_into(obj, json);
    return json;
}

// serialize BatchCreateRequest — object, 1 field(s)  ← api.yaml:348
inline void serialize_BatchCreateRequest_into(const BatchCreateRequest& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"tasks\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.tasks.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateTaskRequest_into(obj.tasks[i], json);
    }
    json.push_back(']');
    json.push_back('}');
}

inline std::string serialize_BatchCreateRequest(const BatchCreateRequest& obj) {
    std::string json;
    size_t reserve_estimate = 75;
    reserve_estimate += obj.tasks.size() * 273;
    json.reserve(reserve_estimate);
    serialize_BatchCreateRequest_into(obj, json);
    return json;
}

// serialize BatchCreateResponse — object, 2 field(s)  ← api.yaml:360
inline void serialize_BatchCreateResponse_into(const BatchCreateResponse& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"created\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.created.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Task_into(obj.created[i], json);
    }
    json.push_back(']');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"failed\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.failed.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_BatchCreateResponse_Item_t_1_into(obj.failed[i], json);
    }
    json.push_back(']');
    json.push_back('}');
}

inline std::string serialize_BatchCreateResponse(const BatchCreateResponse& obj) {
    std::string json;
    size_t reserve_estimate = 151;
    reserve_estimate += obj.created.size() * 678;
    reserve_estimate += obj.failed.size() * 72;
    json.reserve(reserve_estimate);
    serialize_BatchCreateResponse_into(obj, json);
    return json;
}

// serialize BatchCreateResponse_Item_t_1 — object, field BatchCreateResponse.item  ← api.yaml:372
inline void serialize_BatchCreateResponse_Item_t_1_into(const BatchCreateResponse_Item_t_1& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"index\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.index);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"error\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.error, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_BatchCreateResponse_Item_t_1(const BatchCreateResponse_Item_t_1& obj) {
    std::string json;
    size_t reserve_estimate = 72;
    reserve_estimate += obj.error.size();
    json.reserve(reserve_estimate);
    serialize_BatchCreateResponse_Item_t_1_into(obj, json);
    return json;
}

// serialize SearchRequest — object, 8 field(s)  ← api.yaml:383
inline void serialize_SearchRequest_into(const SearchRequest& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (obj.title_contains) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"title_contains\":");
    if (obj.title_contains) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.title_contains, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    if (obj.statuses) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"statuses\":");
    if (!obj.statuses) {
        json.append("null");
    } else {
    json.push_back('[');
    for (size_t i = 0; i < obj.statuses->size(); ++i) {
        if (i > 0) json.push_back(',');
        json.push_back('"');
        json.append(to_string((*obj.statuses)[i]));
        json.push_back('"');
    }
    json.push_back(']');
    }
    }
    if (obj.min_priority) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"min_priority\":");
    {
        if (!obj.min_priority) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.min_priority);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    if (obj.max_priority) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"max_priority\":");
    {
        if (!obj.max_priority) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.max_priority);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    if (obj.tags) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"tags\":");
    if (!obj.tags) {
        json.append("null");
    } else {
    json.push_back('[');
    for (size_t i = 0; i < obj.tags->size(); ++i) {
        if (i > 0) json.push_back(',');
        json.push_back('"');
        katana::serde::escape_json_string_into((*obj.tags)[i], json);
        json.push_back('"');
    }
    json.push_back(']');
    }
    }
    if (obj.created_after) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"created_after\":");
    if (obj.created_after) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.created_after, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    if (obj.created_before) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"created_before\":");
    if (obj.created_before) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.created_before, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    if (obj.has_assignee) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"has_assignee\":");
    if (!obj.has_assignee) {
        json.append("null");
    } else {
        json.append(*obj.has_assignee ? "true" : "false");
    }
    }
    json.push_back('}');
}

inline std::string serialize_SearchRequest(const SearchRequest& obj) {
    std::string json;
    size_t reserve_estimate = 392;
    if (obj.title_contains) reserve_estimate += obj.title_contains->size();
    if (obj.statuses) reserve_estimate += obj.statuses->size() * 32;
    if (obj.tags) reserve_estimate += obj.tags->size() * 32;
    if (obj.created_after) reserve_estimate += obj.created_after->size();
    if (obj.created_before) reserve_estimate += obj.created_before->size();
    json.reserve(reserve_estimate);
    serialize_SearchRequest_into(obj, json);
    return json;
}

// serialize SearchRequest_Item_t — enum, field SearchRequest.item  ← api.yaml:392
inline void serialize_SearchRequest_Item_t_into(const SearchRequest_Item_t& obj, std::string& json) {
    auto str = to_string(obj);
    json.push_back('"');
    json.append(str);
    json.push_back('"');
}

inline std::string serialize_SearchRequest_Item_t(const SearchRequest_Item_t& obj) {
    std::string json;
    auto str = to_string(obj);
    json.reserve(str.size() + 2);
    serialize_SearchRequest_Item_t_into(obj, json);
    return json;
}

// serialize TaskList — object, 3 field(s)  ← api.yaml:420
inline void serialize_TaskList_into(const TaskList& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"tasks\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.tasks.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Task_into(obj.tasks[i], json);
    }
    json.push_back(']');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"total\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.total);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (obj.has_more) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"has_more\":");
    if (!obj.has_more) {
        json.append("null");
    } else {
        json.append(*obj.has_more ? "true" : "false");
    }
    }
    json.push_back('}');
}

inline std::string serialize_TaskList(const TaskList& obj) {
    std::string json;
    size_t reserve_estimate = 121;
    reserve_estimate += obj.tasks.size() * 678;
    json.reserve(reserve_estimate);
    serialize_TaskList_into(obj, json);
    return json;
}

// serialize HealthResponse — object, 4 field(s)  ← api.yaml:436
inline void serialize_HealthResponse_into(const HealthResponse& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"status\":");
    json.push_back('"');
    json.append(to_string(obj.status));
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"timestamp\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.timestamp, json);
    json.push_back('"');
    if (obj.uptime_seconds) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"uptime_seconds\":");
    {
        if (!obj.uptime_seconds) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.uptime_seconds);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    if (obj.total_requests) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"total_requests\":");
    {
        if (!obj.total_requests) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.total_requests);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    json.push_back('}');
}

inline std::string serialize_HealthResponse(const HealthResponse& obj) {
    std::string json;
    size_t reserve_estimate = 165;
    reserve_estimate += obj.timestamp.size();
    json.reserve(reserve_estimate);
    serialize_HealthResponse_into(obj, json);
    return json;
}

// serialize HealthResponse_Status_t — enum, field HealthResponse.status  ← api.yaml:442
inline void serialize_HealthResponse_Status_t_into(const HealthResponse_Status_t& obj, std::string& json) {
    auto str = to_string(obj);
    json.push_back('"');
    json.append(str);
    json.push_back('"');
}

inline std::string serialize_HealthResponse_Status_t(const HealthResponse_Status_t& obj) {
    std::string json;
    auto str = to_string(obj);
    json.reserve(str.size() + 2);
    serialize_HealthResponse_Status_t_into(obj, json);
    return json;
}

// serialize ProblemDetails — object, 5 field(s)  ← api.yaml:455
inline void serialize_ProblemDetails_into(const ProblemDetails& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"type\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.type, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"title\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.title, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"status\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.status);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (obj.detail) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"detail\":");
    if (obj.detail) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.detail, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    if (obj.instance) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"instance\":");
    if (obj.instance) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.instance, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    json.push_back('}');
}

inline std::string serialize_ProblemDetails(const ProblemDetails& obj) {
    std::string json;
    size_t reserve_estimate = 199;
    reserve_estimate += obj.type.size();
    reserve_estimate += obj.title.size();
    if (obj.detail) reserve_estimate += obj.detail->size();
    if (obj.instance) reserve_estimate += obj.instance->size();
    json.reserve(reserve_estimate);
    serialize_ProblemDetails_into(obj, json);
    return json;
}

// serialize listTasks_param_status — enum  ← api.yaml:22
inline void serialize_listTasks_param_status_into(const listTasks_param_status& obj, std::string& json) {
    auto str = to_string(obj);
    json.push_back('"');
    json.append(str);
    json.push_back('"');
}

inline std::string serialize_listTasks_param_status(const listTasks_param_status& obj) {
    std::string json;
    auto str = to_string(obj);
    json.reserve(str.size() + 2);
    serialize_listTasks_param_status_into(obj, json);
    return json;
}

