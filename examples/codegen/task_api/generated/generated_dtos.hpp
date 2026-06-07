// layer: flat
// Auto-generated DTOs (Data Transfer Objects) from OpenAPI specification
//
// This file contains:
//   - Type definitions for request/response bodies
//   - Enum types with string conversion functions
//   - Compile-time metadata for validation constraints
//   - Zero-copy arena allocators for high performance
//
// All types include metadata structs with validation constraints:
//   - Required/optional flags
//   - String length constraints (min_length, max_length)
//   - Numeric constraints (minimum, maximum, exclusive bounds)
//   - Array constraints (min_items, max_items, uniqueness)
//
#pragma once

#include "katana/core/arena.hpp"
using katana::arena_allocator;
using katana::arena_string;
using katana::arena_vector;
using katana::monotonic_arena;

#include <optional>
#include <string_view>
#include <cctype>

#include <variant>

// ============================================================
// Enum Types
// ============================================================

/// Enum with 4 possible values
enum class Task_Status_t_enum {
    pending,
    in_progress,
    completed,
    cancelled
};

inline std::string_view to_string(Task_Status_t_enum e) {
    switch (e) {
    case Task_Status_t_enum::pending: return "pending";
    case Task_Status_t_enum::in_progress: return "in_progress";
    case Task_Status_t_enum::completed: return "completed";
    case Task_Status_t_enum::cancelled: return "cancelled";
    }
    return "";
}

inline std::optional<Task_Status_t_enum> Task_Status_t_enum_from_string(std::string_view s) {
    if (s == "pending") return Task_Status_t_enum::pending;
    if (s == "in_progress") return Task_Status_t_enum::in_progress;
    if (s == "completed") return Task_Status_t_enum::completed;
    if (s == "cancelled") return Task_Status_t_enum::cancelled;
    return std::nullopt;
}

/// Alias of Task_Status_t_enum (identical value set)
using UpdateTaskRequest_Status_t_enum = Task_Status_t_enum;
inline std::optional<UpdateTaskRequest_Status_t_enum> UpdateTaskRequest_Status_t_enum_from_string(std::string_view s) { return Task_Status_t_enum_from_string(s); }

/// Alias of Task_Status_t_enum (identical value set)
using SearchRequest_Item_t_enum = Task_Status_t_enum;
inline std::optional<SearchRequest_Item_t_enum> SearchRequest_Item_t_enum_from_string(std::string_view s) { return Task_Status_t_enum_from_string(s); }

/// Enum with 3 possible values
enum class HealthResponse_Status_t_enum {
    healthy,
    degraded,
    unhealthy
};

inline std::string_view to_string(HealthResponse_Status_t_enum e) {
    switch (e) {
    case HealthResponse_Status_t_enum::healthy: return "healthy";
    case HealthResponse_Status_t_enum::degraded: return "degraded";
    case HealthResponse_Status_t_enum::unhealthy: return "unhealthy";
    }
    return "";
}

inline std::optional<HealthResponse_Status_t_enum> HealthResponse_Status_t_enum_from_string(std::string_view s) {
    if (s == "healthy") return HealthResponse_Status_t_enum::healthy;
    if (s == "degraded") return HealthResponse_Status_t_enum::degraded;
    if (s == "unhealthy") return HealthResponse_Status_t_enum::unhealthy;
    return std::nullopt;
}

/// Alias of Task_Status_t_enum (identical value set)
using listTasks_param_status_enum = Task_Status_t_enum;
inline std::optional<listTasks_param_status_enum> listTasks_param_status_enum_from_string(std::string_view s) { return Task_Status_t_enum_from_string(s); }

// ============================================================
// Data Transfer Objects (DTOs)
// ============================================================

// Task_Id_t — integer, field Task.id  ← api.yaml:215
using Task_Id_t = int64_t;

// Task_Title_t — string, field Task.title  ← api.yaml:219
using Task_Title_t = arena_string<>;

// Task_Description_t — string, field Task.description  ← api.yaml:223
using Task_Description_t = arena_string<>;

// Task_Status_t — enum, field Task.status  ← api.yaml:226
using Task_Status_t = Task_Status_t_enum;

// Task_Priority_t — integer, field Task.priority  ← api.yaml:229
using Task_Priority_t = int64_t;

// Task_Tags_t — array, field Task.tags  ← api.yaml:233
using Task_Tags_t = arena_vector<arena_string<>>;

// Task_Item_t — string, field Task.item  ← api.yaml:235
using Task_Item_t = arena_string<>;

// Task_Due_date_t — string, field Task.due_date  ← api.yaml:243
using Task_Due_date_t = std::optional<arena_string<>>;

// Task_Created_at_t — string, field Task.created_at  ← api.yaml:247
using Task_Created_at_t = arena_string<>;

// Task_Updated_at_t — string, field Task.updated_at  ← api.yaml:250
using Task_Updated_at_t = std::optional<arena_string<>>;

// Task_Metadata_t — object, field Task.metadata  ← api.yaml:254
using Task_Metadata_t = std::monostate;

// User — object, 3 field(s)  ← api.yaml:258
/// Data type with 3 fields
struct User {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool ID_REQUIRED = true;
        static constexpr double ID_MINIMUM = 1;
        static constexpr bool EMAIL_REQUIRED = true;
        static constexpr size_t EMAIL_MIN_LENGTH = 3;
        static constexpr size_t EMAIL_MAX_LENGTH = 100;
        static constexpr bool NAME_REQUIRED = false;
        static constexpr size_t NAME_MIN_LENGTH = 1;
        static constexpr size_t NAME_MAX_LENGTH = 100;
    };

    static_assert(metadata::EMAIL_MIN_LENGTH <= metadata::EMAIL_MAX_LENGTH, "email: min_length must be <= max_length");
    static_assert(metadata::NAME_MIN_LENGTH <= metadata::NAME_MAX_LENGTH, "name: min_length must be <= max_length");

    explicit User(monotonic_arena* arena = nullptr)
        : arena_(arena),
          email(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    int64_t id;
    arena_string<> email;
    /// Optional field
    std::optional<arena_string<>> name;
};

// User_Id_t — integer, field User.id  ← api.yaml:264
using User_Id_t = int64_t;

// User_Email_t — string, field User.email  ← api.yaml:268
using User_Email_t = arena_string<>;

// User_Name_t — string, field User.name  ← api.yaml:273
using User_Name_t = arena_string<>;

// CreateTaskRequest — object, 6 field(s)  ← api.yaml:278
/// Data type with 6 fields
struct CreateTaskRequest {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool TITLE_REQUIRED = true;
        static constexpr size_t TITLE_MIN_LENGTH = 1;
        static constexpr size_t TITLE_MAX_LENGTH = 200;
        static constexpr bool DESCRIPTION_REQUIRED = false;
        static constexpr size_t DESCRIPTION_MAX_LENGTH = 2000;
        static constexpr bool PRIORITY_REQUIRED = true;
        static constexpr double PRIORITY_MINIMUM = 1;
        static constexpr double PRIORITY_MAXIMUM = 5;
        static constexpr bool TAGS_REQUIRED = false;
        static constexpr size_t TAGS_MAX_ITEMS = 20;
        static constexpr bool TAGS_UNIQUE_ITEMS = true;
        static constexpr bool ASSIGNEE_ID_REQUIRED = false;
        static constexpr double ASSIGNEE_ID_MINIMUM = 1;
        static constexpr bool DUE_DATE_REQUIRED = false;
    };

    static_assert(metadata::TITLE_MIN_LENGTH <= metadata::TITLE_MAX_LENGTH, "title: min_length must be <= max_length");
    static_assert(metadata::PRIORITY_MINIMUM <= metadata::PRIORITY_MAXIMUM, "priority: minimum must be <= maximum");

    explicit CreateTaskRequest(monotonic_arena* arena = nullptr)
        : arena_(arena),
          title(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> title;
    /// Optional field
    std::optional<arena_string<>> description;
    int64_t priority;
    /// Optional field
    std::optional<arena_vector<arena_string<>>> tags;
    /// Optional field
    std::optional<int64_t> assignee_id;
    /// Optional field
    std::optional<arena_string<>> due_date;
};

// CreateTaskRequest_Title_t — string, field CreateTaskRequest.title  ← api.yaml:284
using CreateTaskRequest_Title_t = arena_string<>;

// CreateTaskRequest_Description_t — string, field CreateTaskRequest.description  ← api.yaml:288
using CreateTaskRequest_Description_t = arena_string<>;

// CreateTaskRequest_Priority_t — integer, field CreateTaskRequest.priority  ← api.yaml:291
using CreateTaskRequest_Priority_t = int64_t;

// CreateTaskRequest_Tags_t — array, field CreateTaskRequest.tags  ← api.yaml:295
using CreateTaskRequest_Tags_t = arena_vector<arena_string<>>;

// CreateTaskRequest_Item_t — string, field CreateTaskRequest.item  ← api.yaml:297
using CreateTaskRequest_Item_t = arena_string<>;

// CreateTaskRequest_Assignee_id_t — integer, field CreateTaskRequest.assignee_id  ← api.yaml:303
using CreateTaskRequest_Assignee_id_t = std::optional<int64_t>;

// CreateTaskRequest_Due_date_t — string, field CreateTaskRequest.due_date  ← api.yaml:308
using CreateTaskRequest_Due_date_t = std::optional<arena_string<>>;

// UpdateTaskRequest — object, 7 field(s)  ← api.yaml:313
/// Data type with 7 fields
struct UpdateTaskRequest {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool TITLE_REQUIRED = false;
        static constexpr size_t TITLE_MIN_LENGTH = 1;
        static constexpr size_t TITLE_MAX_LENGTH = 200;
        static constexpr bool DESCRIPTION_REQUIRED = false;
        static constexpr size_t DESCRIPTION_MAX_LENGTH = 2000;
        static constexpr bool STATUS_REQUIRED = false;
        static constexpr bool PRIORITY_REQUIRED = false;
        static constexpr double PRIORITY_MINIMUM = 1;
        static constexpr double PRIORITY_MAXIMUM = 5;
        static constexpr bool TAGS_REQUIRED = false;
        static constexpr size_t TAGS_MAX_ITEMS = 20;
        static constexpr bool TAGS_UNIQUE_ITEMS = true;
        static constexpr bool ASSIGNEE_ID_REQUIRED = false;
        static constexpr double ASSIGNEE_ID_MINIMUM = 1;
        static constexpr bool DUE_DATE_REQUIRED = false;
    };

    static_assert(metadata::TITLE_MIN_LENGTH <= metadata::TITLE_MAX_LENGTH, "title: min_length must be <= max_length");
    static_assert(metadata::PRIORITY_MINIMUM <= metadata::PRIORITY_MAXIMUM, "priority: minimum must be <= maximum");

    explicit UpdateTaskRequest(monotonic_arena* arena = nullptr)
        : arena_(arena) {}

    monotonic_arena* arena_;
    /// Optional field
    std::optional<arena_string<>> title;
    /// Optional field
    std::optional<arena_string<>> description;
    /// Optional field
    std::optional<UpdateTaskRequest_Status_t_enum> status;
    /// Optional field
    std::optional<int64_t> priority;
    /// Optional field
    std::optional<arena_vector<arena_string<>>> tags;
    /// Optional field
    std::optional<int64_t> assignee_id;
    /// Optional field
    std::optional<arena_string<>> due_date;
};

// UpdateTaskRequest_Title_t — string, field UpdateTaskRequest.title  ← api.yaml:316
using UpdateTaskRequest_Title_t = arena_string<>;

// UpdateTaskRequest_Description_t — string, field UpdateTaskRequest.description  ← api.yaml:320
using UpdateTaskRequest_Description_t = arena_string<>;

// UpdateTaskRequest_Status_t — enum, field UpdateTaskRequest.status  ← api.yaml:323
using UpdateTaskRequest_Status_t = UpdateTaskRequest_Status_t_enum;

// UpdateTaskRequest_Priority_t — integer, field UpdateTaskRequest.priority  ← api.yaml:326
using UpdateTaskRequest_Priority_t = int64_t;

// UpdateTaskRequest_Tags_t — array, field UpdateTaskRequest.tags  ← api.yaml:330
using UpdateTaskRequest_Tags_t = arena_vector<arena_string<>>;

// UpdateTaskRequest_Item_t — string, field UpdateTaskRequest.item  ← api.yaml:332
using UpdateTaskRequest_Item_t = arena_string<>;

// UpdateTaskRequest_Assignee_id_t — integer, field UpdateTaskRequest.assignee_id  ← api.yaml:338
using UpdateTaskRequest_Assignee_id_t = std::optional<int64_t>;

// UpdateTaskRequest_Due_date_t — string, field UpdateTaskRequest.due_date  ← api.yaml:343
using UpdateTaskRequest_Due_date_t = std::optional<arena_string<>>;

// BatchCreateResponse_Item_t_1 — object, field BatchCreateResponse.item  ← api.yaml:372
/// Data type with 2 fields
struct BatchCreateResponse_Item_t_1 {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool INDEX_REQUIRED = true;
        static constexpr bool ERROR_REQUIRED = true;
    };


    explicit BatchCreateResponse_Item_t_1(monotonic_arena* arena = nullptr)
        : arena_(arena),
          error(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    int64_t index;
    arena_string<> error;
};

// BatchCreateResponse_Index_t — integer, field BatchCreateResponse.index  ← api.yaml:378
using BatchCreateResponse_Index_t = int64_t;

// BatchCreateResponse_Error_t — string, field BatchCreateResponse.error  ← api.yaml:380
using BatchCreateResponse_Error_t = arena_string<>;

// SearchRequest — object, 8 field(s)  ← api.yaml:383
/// Data type with 8 fields
struct SearchRequest {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool TITLE_CONTAINS_REQUIRED = false;
        static constexpr size_t TITLE_CONTAINS_MIN_LENGTH = 1;
        static constexpr size_t TITLE_CONTAINS_MAX_LENGTH = 100;
        static constexpr bool STATUSES_REQUIRED = false;
        static constexpr size_t STATUSES_MIN_ITEMS = 1;
        static constexpr size_t STATUSES_MAX_ITEMS = 4;
        static constexpr bool STATUSES_UNIQUE_ITEMS = true;
        static constexpr bool MIN_PRIORITY_REQUIRED = false;
        static constexpr double MIN_PRIORITY_MINIMUM = 1;
        static constexpr double MIN_PRIORITY_MAXIMUM = 5;
        static constexpr bool MAX_PRIORITY_REQUIRED = false;
        static constexpr double MAX_PRIORITY_MINIMUM = 1;
        static constexpr double MAX_PRIORITY_MAXIMUM = 5;
        static constexpr bool TAGS_REQUIRED = false;
        static constexpr size_t TAGS_MAX_ITEMS = 10;
        static constexpr bool CREATED_AFTER_REQUIRED = false;
        static constexpr bool CREATED_BEFORE_REQUIRED = false;
        static constexpr bool HAS_ASSIGNEE_REQUIRED = false;
    };

    static_assert(metadata::TITLE_CONTAINS_MIN_LENGTH <= metadata::TITLE_CONTAINS_MAX_LENGTH, "title_contains: min_length must be <= max_length");
    static_assert(metadata::STATUSES_MIN_ITEMS <= metadata::STATUSES_MAX_ITEMS, "statuses: min_items must be <= max_items");
    static_assert(metadata::MIN_PRIORITY_MINIMUM <= metadata::MIN_PRIORITY_MAXIMUM, "min_priority: minimum must be <= maximum");
    static_assert(metadata::MAX_PRIORITY_MINIMUM <= metadata::MAX_PRIORITY_MAXIMUM, "max_priority: minimum must be <= maximum");

    explicit SearchRequest(monotonic_arena* arena = nullptr)
        : arena_(arena) {}

    monotonic_arena* arena_;
    /// Optional field
    std::optional<arena_string<>> title_contains;
    /// Optional field
    std::optional<arena_vector<SearchRequest_Item_t_enum>> statuses;
    /// Optional field
    std::optional<int64_t> min_priority;
    /// Optional field
    std::optional<int64_t> max_priority;
    /// Optional field
    std::optional<arena_vector<arena_string<>>> tags;
    /// Optional field
    std::optional<arena_string<>> created_after;
    /// Optional field
    std::optional<arena_string<>> created_before;
    /// Optional field
    std::optional<bool> has_assignee;
};

// SearchRequest_Title_contains_t — string, field SearchRequest.title_contains  ← api.yaml:386
using SearchRequest_Title_contains_t = arena_string<>;

// SearchRequest_Statuses_t — array, field SearchRequest.statuses  ← api.yaml:390
using SearchRequest_Statuses_t = arena_vector<SearchRequest_Item_t_enum>;

// SearchRequest_Item_t — enum, field SearchRequest.item  ← api.yaml:392
using SearchRequest_Item_t = SearchRequest_Item_t_enum;

// SearchRequest_Min_priority_t — integer, field SearchRequest.min_priority  ← api.yaml:398
using SearchRequest_Min_priority_t = int64_t;

// SearchRequest_Max_priority_t — integer, field SearchRequest.max_priority  ← api.yaml:402
using SearchRequest_Max_priority_t = int64_t;

// SearchRequest_Tags_t — array, field SearchRequest.tags  ← api.yaml:406
using SearchRequest_Tags_t = arena_vector<arena_string<>>;

// SearchRequest_Item_t_1 — string, field SearchRequest.item  ← api.yaml:408
using SearchRequest_Item_t_1 = arena_string<>;

// SearchRequest_Created_after_t — string, field SearchRequest.created_after  ← api.yaml:411
using SearchRequest_Created_after_t = arena_string<>;

// SearchRequest_Created_before_t — string, field SearchRequest.created_before  ← api.yaml:414
using SearchRequest_Created_before_t = arena_string<>;

// SearchRequest_Has_assignee_t — boolean, field SearchRequest.has_assignee  ← api.yaml:417
using SearchRequest_Has_assignee_t = bool;

// TaskList_Total_t — integer, field TaskList.total  ← api.yaml:430
using TaskList_Total_t = int64_t;

// TaskList_Has_more_t — boolean, field TaskList.has_more  ← api.yaml:433
using TaskList_Has_more_t = bool;

// HealthResponse — object, 4 field(s)  ← api.yaml:436
/// Data type with 4 fields
struct HealthResponse {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool STATUS_REQUIRED = true;
        static constexpr bool TIMESTAMP_REQUIRED = true;
        static constexpr bool UPTIME_SECONDS_REQUIRED = false;
        static constexpr double UPTIME_SECONDS_MINIMUM = 0;
        static constexpr bool TOTAL_REQUESTS_REQUIRED = false;
        static constexpr double TOTAL_REQUESTS_MINIMUM = 0;
    };


    explicit HealthResponse(monotonic_arena* arena = nullptr)
        : arena_(arena),
          timestamp(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    HealthResponse_Status_t_enum status;
    arena_string<> timestamp;
    /// Optional field
    std::optional<int64_t> uptime_seconds;
    /// Optional field
    std::optional<int64_t> total_requests;
};

// HealthResponse_Status_t — enum, field HealthResponse.status  ← api.yaml:442
using HealthResponse_Status_t = HealthResponse_Status_t_enum;

// HealthResponse_Timestamp_t — string, field HealthResponse.timestamp  ← api.yaml:445
using HealthResponse_Timestamp_t = arena_string<>;

// HealthResponse_Uptime_seconds_t — integer, field HealthResponse.uptime_seconds  ← api.yaml:448
using HealthResponse_Uptime_seconds_t = int64_t;

// HealthResponse_Total_requests_t — integer, field HealthResponse.total_requests  ← api.yaml:451
using HealthResponse_Total_requests_t = int64_t;

// ProblemDetails — object, 5 field(s)  ← api.yaml:455
/// Data type with 5 fields
struct ProblemDetails {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool TYPE_REQUIRED = true;
        static constexpr bool TITLE_REQUIRED = true;
        static constexpr bool STATUS_REQUIRED = true;
        static constexpr bool DETAIL_REQUIRED = false;
        static constexpr bool INSTANCE_REQUIRED = false;
    };


    explicit ProblemDetails(monotonic_arena* arena = nullptr)
        : arena_(arena),
          type(arena_allocator<char>(arena)),
          title(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> type;
    arena_string<> title;
    int64_t status;
    /// Optional field
    std::optional<arena_string<>> detail;
    /// Optional field
    std::optional<arena_string<>> instance;
};

// ProblemDetails_Type_t — string, field ProblemDetails.type  ← api.yaml:462
using ProblemDetails_Type_t = arena_string<>;

// ProblemDetails_Title_t — string, field ProblemDetails.title  ← api.yaml:464
using ProblemDetails_Title_t = arena_string<>;

// ProblemDetails_Status_t — integer, field ProblemDetails.status  ← api.yaml:466
using ProblemDetails_Status_t = int64_t;

// ProblemDetails_Detail_t — string, field ProblemDetails.detail  ← api.yaml:468
using ProblemDetails_Detail_t = arena_string<>;

// ProblemDetails_Instance_t — string, field ProblemDetails.instance  ← api.yaml:470
using ProblemDetails_Instance_t = arena_string<>;

// listTasks_param_status — enum  ← api.yaml:22
using listTasks_param_status = listTasks_param_status_enum;

// listTasks_param_priority — integer  ← api.yaml:28
using listTasks_param_priority = int64_t;

// listTasks_param_limit — integer  ← api.yaml:35
using listTasks_param_limit = int64_t;

// listTasks_param_offset — integer  ← api.yaml:43
using listTasks_param_offset = int64_t;

// getTask_param_id — integer  ← api.yaml:86
using getTask_param_id = int64_t;

// updateTask_param_id — integer  ← api.yaml:111
using updateTask_param_id = int64_t;

// deleteTask_param_id — integer  ← api.yaml:142
using deleteTask_param_id = int64_t;

// Task — object, 11 field(s)  ← api.yaml:206
/// Data type with 11 fields
struct Task {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool ID_REQUIRED = true;
        static constexpr double ID_MINIMUM = 1;
        static constexpr bool TITLE_REQUIRED = true;
        static constexpr size_t TITLE_MIN_LENGTH = 1;
        static constexpr size_t TITLE_MAX_LENGTH = 200;
        static constexpr bool DESCRIPTION_REQUIRED = false;
        static constexpr size_t DESCRIPTION_MAX_LENGTH = 2000;
        static constexpr bool STATUS_REQUIRED = true;
        static constexpr bool PRIORITY_REQUIRED = true;
        static constexpr double PRIORITY_MINIMUM = 1;
        static constexpr double PRIORITY_MAXIMUM = 5;
        static constexpr bool TAGS_REQUIRED = false;
        static constexpr size_t TAGS_MAX_ITEMS = 20;
        static constexpr bool TAGS_UNIQUE_ITEMS = true;
        static constexpr bool ASSIGNEE_REQUIRED = false;
        static constexpr bool DUE_DATE_REQUIRED = false;
        static constexpr bool CREATED_AT_REQUIRED = true;
        static constexpr bool UPDATED_AT_REQUIRED = false;
        static constexpr bool METADATA_REQUIRED = false;
    };

    static_assert(metadata::TITLE_MIN_LENGTH <= metadata::TITLE_MAX_LENGTH, "title: min_length must be <= max_length");
    static_assert(metadata::PRIORITY_MINIMUM <= metadata::PRIORITY_MAXIMUM, "priority: minimum must be <= maximum");

    explicit Task(monotonic_arena* arena = nullptr)
        : arena_(arena),
          title(arena_allocator<char>(arena)),
          created_at(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    int64_t id;
    arena_string<> title;
    /// Optional field
    std::optional<arena_string<>> description;
    Task_Status_t_enum status;
    int64_t priority;
    /// Optional field
    std::optional<arena_vector<arena_string<>>> tags;
    /// Optional field
    std::optional<User> assignee;
    /// Optional field
    std::optional<arena_string<>> due_date;
    arena_string<> created_at;
    /// Optional field
    std::optional<arena_string<>> updated_at;
    /// Optional field
    std::optional<std::monostate> metadata;
};

// BatchCreateRequest — object, 1 field(s)  ← api.yaml:348
/// Data type with 1 fields
struct BatchCreateRequest {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool TASKS_REQUIRED = true;
        static constexpr size_t TASKS_MIN_ITEMS = 1;
        static constexpr size_t TASKS_MAX_ITEMS = 100;
    };

    static_assert(metadata::TASKS_MIN_ITEMS <= metadata::TASKS_MAX_ITEMS, "tasks: min_items must be <= max_items");

    explicit BatchCreateRequest(monotonic_arena* arena = nullptr)
        : arena_(arena),
          tasks(arena_allocator<CreateTaskRequest>(arena)) {}

    monotonic_arena* arena_;
    arena_vector<CreateTaskRequest> tasks;
};

// BatchCreateRequest_Tasks_t — array, field BatchCreateRequest.tasks  ← api.yaml:353
using BatchCreateRequest_Tasks_t = arena_vector<CreateTaskRequest>;

// BatchCreateResponse_Failed_t — array, field BatchCreateResponse.failed  ← api.yaml:370
using BatchCreateResponse_Failed_t = arena_vector<BatchCreateResponse_Item_t_1>;

// BatchCreateResponse — object, 2 field(s)  ← api.yaml:360
/// Data type with 2 fields
struct BatchCreateResponse {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool CREATED_REQUIRED = true;
        static constexpr bool FAILED_REQUIRED = true;
    };


    explicit BatchCreateResponse(monotonic_arena* arena = nullptr)
        : arena_(arena),
          created(arena_allocator<Task>(arena)),
          failed(arena_allocator<BatchCreateResponse_Item_t_1>(arena)) {}

    monotonic_arena* arena_;
    arena_vector<Task> created;
    arena_vector<BatchCreateResponse_Item_t_1> failed;
};

// BatchCreateResponse_Created_t — array, field BatchCreateResponse.created  ← api.yaml:366
using BatchCreateResponse_Created_t = arena_vector<Task>;

// TaskList — object, 3 field(s)  ← api.yaml:420
/// Data type with 3 fields
struct TaskList {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool TASKS_REQUIRED = true;
        static constexpr bool TOTAL_REQUIRED = true;
        static constexpr double TOTAL_MINIMUM = 0;
        static constexpr bool HAS_MORE_REQUIRED = false;
    };


    explicit TaskList(monotonic_arena* arena = nullptr)
        : arena_(arena),
          tasks(arena_allocator<Task>(arena)) {}

    monotonic_arena* arena_;
    arena_vector<Task> tasks;
    int64_t total;
    /// Optional field
    std::optional<bool> has_more;
};

// TaskList_Tasks_t — array, field TaskList.tasks  ← api.yaml:426
using TaskList_Tasks_t = arena_vector<Task>;

