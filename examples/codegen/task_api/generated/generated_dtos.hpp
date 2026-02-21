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

/// Enum with 4 possible values
enum class UpdateTaskRequest_Status_t_enum {
    pending,
    in_progress,
    completed,
    cancelled
};

inline std::string_view to_string(UpdateTaskRequest_Status_t_enum e) {
    switch (e) {
    case UpdateTaskRequest_Status_t_enum::pending: return "pending";
    case UpdateTaskRequest_Status_t_enum::in_progress: return "in_progress";
    case UpdateTaskRequest_Status_t_enum::completed: return "completed";
    case UpdateTaskRequest_Status_t_enum::cancelled: return "cancelled";
    }
    return "";
}

inline std::optional<UpdateTaskRequest_Status_t_enum> UpdateTaskRequest_Status_t_enum_from_string(std::string_view s) {
    if (s == "pending") return UpdateTaskRequest_Status_t_enum::pending;
    if (s == "in_progress") return UpdateTaskRequest_Status_t_enum::in_progress;
    if (s == "completed") return UpdateTaskRequest_Status_t_enum::completed;
    if (s == "cancelled") return UpdateTaskRequest_Status_t_enum::cancelled;
    return std::nullopt;
}

/// Enum with 4 possible values
enum class SearchRequest_Item_t_enum {
    pending,
    in_progress,
    completed,
    cancelled
};

inline std::string_view to_string(SearchRequest_Item_t_enum e) {
    switch (e) {
    case SearchRequest_Item_t_enum::pending: return "pending";
    case SearchRequest_Item_t_enum::in_progress: return "in_progress";
    case SearchRequest_Item_t_enum::completed: return "completed";
    case SearchRequest_Item_t_enum::cancelled: return "cancelled";
    }
    return "";
}

inline std::optional<SearchRequest_Item_t_enum> SearchRequest_Item_t_enum_from_string(std::string_view s) {
    if (s == "pending") return SearchRequest_Item_t_enum::pending;
    if (s == "in_progress") return SearchRequest_Item_t_enum::in_progress;
    if (s == "completed") return SearchRequest_Item_t_enum::completed;
    if (s == "cancelled") return SearchRequest_Item_t_enum::cancelled;
    return std::nullopt;
}

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

/// Enum with 4 possible values
enum class listTasks_param_status_enum {
    pending,
    in_progress,
    completed,
    cancelled
};

inline std::string_view to_string(listTasks_param_status_enum e) {
    switch (e) {
    case listTasks_param_status_enum::pending: return "pending";
    case listTasks_param_status_enum::in_progress: return "in_progress";
    case listTasks_param_status_enum::completed: return "completed";
    case listTasks_param_status_enum::cancelled: return "cancelled";
    }
    return "";
}

inline std::optional<listTasks_param_status_enum> listTasks_param_status_enum_from_string(std::string_view s) {
    if (s == "pending") return listTasks_param_status_enum::pending;
    if (s == "in_progress") return listTasks_param_status_enum::in_progress;
    if (s == "completed") return listTasks_param_status_enum::completed;
    if (s == "cancelled") return listTasks_param_status_enum::cancelled;
    return std::nullopt;
}

// ============================================================
// Data Transfer Objects (DTOs)
// ============================================================

using Task_Id_t = int64_t;

using Task_Title_t = arena_string<>;

using Task_Description_t = arena_string<>;

using Task_Status_t = Task_Status_t_enum;

using Task_Priority_t = int64_t;

using Task_Tags_t = arena_vector<arena_string<>>;

using Task_Item_t = arena_string<>;

using Task_Assignee_t = std::monostate;

using Task_Due_date_t = std::optional<arena_string<>>;

using Task_Created_at_t = arena_string<>;

using Task_Updated_at_t = std::optional<arena_string<>>;

using Task_Metadata_t = std::monostate;

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
          email(arena_allocator<char>(arena)),
          name(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    int64_t id;
    arena_string<> email;
    /// Optional field
    arena_string<> name;
};

using User_Id_t = int64_t;

using User_Email_t = arena_string<>;

using User_Name_t = arena_string<>;

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
          title(arena_allocator<char>(arena)),
          description(arena_allocator<char>(arena)),
          tags(arena_allocator<arena_string<>>(arena)),
          due_date(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> title;
    /// Optional field
    arena_string<> description;
    int64_t priority;
    /// Optional field
    arena_vector<arena_string<>> tags;
    /// Optional field
    std::optional<int64_t> assignee_id = {};
    /// Optional field
    std::optional<arena_string<>> due_date;
};

using CreateTaskRequest_Title_t = arena_string<>;

using CreateTaskRequest_Description_t = arena_string<>;

using CreateTaskRequest_Priority_t = int64_t;

using CreateTaskRequest_Tags_t = arena_vector<arena_string<>>;

using CreateTaskRequest_Item_t = arena_string<>;

using CreateTaskRequest_Assignee_id_t = std::optional<int64_t>;

using CreateTaskRequest_Due_date_t = std::optional<arena_string<>>;

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
        : arena_(arena),
          title(arena_allocator<char>(arena)),
          description(arena_allocator<char>(arena)),
          tags(arena_allocator<arena_string<>>(arena)),
          due_date(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    /// Optional field
    arena_string<> title;
    /// Optional field
    arena_string<> description;
    /// Optional field
    UpdateTaskRequest_Status_t_enum status = {};
    /// Optional field
    int64_t priority = {};
    /// Optional field
    arena_vector<arena_string<>> tags;
    /// Optional field
    std::optional<int64_t> assignee_id = {};
    /// Optional field
    std::optional<arena_string<>> due_date;
};

using UpdateTaskRequest_Title_t = arena_string<>;

using UpdateTaskRequest_Description_t = arena_string<>;

using UpdateTaskRequest_Status_t = UpdateTaskRequest_Status_t_enum;

using UpdateTaskRequest_Priority_t = int64_t;

using UpdateTaskRequest_Tags_t = arena_vector<arena_string<>>;

using UpdateTaskRequest_Item_t = arena_string<>;

using UpdateTaskRequest_Assignee_id_t = std::optional<int64_t>;

using UpdateTaskRequest_Due_date_t = std::optional<arena_string<>>;

using BatchCreateRequest_Item_t = std::monostate;

using BatchCreateResponse_Item_t = std::monostate;

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

using BatchCreateResponse_Index_t = int64_t;

using BatchCreateResponse_Error_t = arena_string<>;

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
        : arena_(arena),
          title_contains(arena_allocator<char>(arena)),
          statuses(arena_allocator<SearchRequest_Item_t_enum>(arena)),
          tags(arena_allocator<arena_string<>>(arena)),
          created_after(arena_allocator<char>(arena)),
          created_before(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    /// Optional field
    arena_string<> title_contains;
    /// Optional field
    arena_vector<SearchRequest_Item_t_enum> statuses;
    /// Optional field
    int64_t min_priority = {};
    /// Optional field
    int64_t max_priority = {};
    /// Optional field
    arena_vector<arena_string<>> tags;
    /// Optional field
    arena_string<> created_after;
    /// Optional field
    arena_string<> created_before;
    /// Optional field
    bool has_assignee = {};
};

using SearchRequest_Title_contains_t = arena_string<>;

using SearchRequest_Statuses_t = arena_vector<SearchRequest_Item_t_enum>;

using SearchRequest_Item_t = SearchRequest_Item_t_enum;

using SearchRequest_Min_priority_t = int64_t;

using SearchRequest_Max_priority_t = int64_t;

using SearchRequest_Tags_t = arena_vector<arena_string<>>;

using SearchRequest_Item_t_1 = arena_string<>;

using SearchRequest_Created_after_t = arena_string<>;

using SearchRequest_Created_before_t = arena_string<>;

using SearchRequest_Has_assignee_t = bool;

using TaskList_Item_t = std::monostate;

using TaskList_Total_t = int64_t;

using TaskList_Has_more_t = bool;

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
    int64_t uptime_seconds = {};
    /// Optional field
    int64_t total_requests = {};
};

using HealthResponse_Status_t = HealthResponse_Status_t_enum;

using HealthResponse_Timestamp_t = arena_string<>;

using HealthResponse_Uptime_seconds_t = int64_t;

using HealthResponse_Total_requests_t = int64_t;

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
          title(arena_allocator<char>(arena)),
          detail(arena_allocator<char>(arena)),
          instance(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> type;
    arena_string<> title;
    int64_t status;
    /// Optional field
    arena_string<> detail;
    /// Optional field
    arena_string<> instance;
};

using ProblemDetails_Type_t = arena_string<>;

using ProblemDetails_Title_t = arena_string<>;

using ProblemDetails_Status_t = int64_t;

using ProblemDetails_Detail_t = arena_string<>;

using ProblemDetails_Instance_t = arena_string<>;

using listTasks_param_status = listTasks_param_status_enum;

using listTasks_param_priority = int64_t;

using listTasks_param_limit = int64_t;

using listTasks_param_offset = int64_t;

using schema = std::monostate;

using schema_1 = std::monostate;

using schema_2 = std::monostate;

using schema_3 = std::monostate;

using getTask_param_id = int64_t;

using schema_4 = std::monostate;

using schema_5 = std::monostate;

using updateTask_param_id = int64_t;

using schema_6 = std::monostate;

using schema_7 = std::monostate;

using schema_8 = std::monostate;

using deleteTask_param_id = int64_t;

using schema_9 = std::monostate;

using schema_10 = std::monostate;

using schema_11 = std::monostate;

using schema_12 = std::monostate;

using schema_13 = std::monostate;

using schema_14 = std::monostate;

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
          description(arena_allocator<char>(arena)),
          tags(arena_allocator<arena_string<>>(arena)),
          due_date(arena_allocator<char>(arena)),
          created_at(arena_allocator<char>(arena)),
          updated_at(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    int64_t id;
    arena_string<> title;
    /// Optional field
    arena_string<> description;
    Task_Status_t_enum status;
    int64_t priority;
    /// Optional field
    arena_vector<arena_string<>> tags;
    /// Optional field
    User assignee;
    /// Optional field
    std::optional<arena_string<>> due_date;
    arena_string<> created_at;
    /// Optional field
    std::optional<arena_string<>> updated_at;
    /// Optional field
    std::monostate metadata = {};
};

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

using BatchCreateRequest_Tasks_t = arena_vector<CreateTaskRequest>;

using BatchCreateResponse_Failed_t = arena_vector<BatchCreateResponse_Item_t_1>;

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

using BatchCreateResponse_Created_t = arena_vector<Task>;

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
    bool has_more = {};
};

using TaskList_Tasks_t = arena_vector<Task>;

