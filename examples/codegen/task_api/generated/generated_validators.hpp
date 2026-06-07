// layer: flat
// Auto-generated validators from OpenAPI specification
//
// This file contains:
//   - Validation functions for all request/response types
//   - Format validators (email, UUID, date-time, etc.)
//   - Constraint validators (length, range, pattern, etc.)
//   - Enum value validators
//
// All validators return std::optional<validation_error>:
//   - std::nullopt on success
//   - validation_error with field path and error code on failure
//
// Validation is automatically called by router bindings before handler execution.
// Invalid requests return 400 Bad Request with error details.
//
#pragma once

#include "generated_dtos.hpp"
#include "katana/core/validation.hpp"
#include "katana/core/format_validators.hpp"
#include <algorithm>
#include <optional>
#include <string_view>
#include <string>
#include <cmath>
#include <cctype>
#include <unordered_set>
#include <vector>

using katana::validation_error;
using katana::validation_error_code;

// ============================================================
// Format Validators (from framework)
// ============================================================

using katana::format_validators::is_valid_email;
using katana::format_validators::is_valid_uuid;
using katana::format_validators::is_valid_datetime;

// ============================================================
// Validation Functions
// ============================================================

[[nodiscard]] inline std::optional<validation_error> validate_Task(const Task&);
[[nodiscard]] inline std::optional<validation_error> validate_Task_Tags_t(const Task_Tags_t&);
[[nodiscard]] inline std::optional<validation_error> validate_User(const User&);
[[nodiscard]] inline std::optional<validation_error> validate_CreateTaskRequest(const CreateTaskRequest&);
[[nodiscard]] inline std::optional<validation_error> validate_CreateTaskRequest_Tags_t(const CreateTaskRequest_Tags_t&);
[[nodiscard]] inline std::optional<validation_error> validate_UpdateTaskRequest(const UpdateTaskRequest&);
[[nodiscard]] inline std::optional<validation_error> validate_UpdateTaskRequest_Tags_t(const UpdateTaskRequest_Tags_t&);
[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateRequest(const BatchCreateRequest&);
[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateRequest_Tasks_t(const BatchCreateRequest_Tasks_t&);
[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateResponse(const BatchCreateResponse&);
[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateResponse_Created_t(const BatchCreateResponse_Created_t&);
[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateResponse_Failed_t(const BatchCreateResponse_Failed_t&);
[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateResponse_Item_t_1(const BatchCreateResponse_Item_t_1&);
[[nodiscard]] inline std::optional<validation_error> validate_SearchRequest(const SearchRequest&);
[[nodiscard]] inline std::optional<validation_error> validate_SearchRequest_Statuses_t(const SearchRequest_Statuses_t&);
[[nodiscard]] inline std::optional<validation_error> validate_SearchRequest_Tags_t(const SearchRequest_Tags_t&);
[[nodiscard]] inline std::optional<validation_error> validate_TaskList(const TaskList&);
[[nodiscard]] inline std::optional<validation_error> validate_TaskList_Tasks_t(const TaskList_Tasks_t&);
[[nodiscard]] inline std::optional<validation_error> validate_HealthResponse(const HealthResponse&);
[[nodiscard]] inline std::optional<validation_error> validate_ProblemDetails(const ProblemDetails&);

// validate Task — object, 11 field(s)  ← api.yaml:206
[[nodiscard]] inline std::optional<validation_error> validate_Task(const Task& obj) {
    if (static_cast<double>(obj.id) < Task::field_constraints::ID_MINIMUM) {
        return validation_error{"id", validation_error_code::value_too_small, Task::field_constraints::ID_MINIMUM};
    }
    if (obj.title.empty()) {
        return validation_error{"title", validation_error_code::required_field_missing};
    }
    if (!obj.title.empty() && katana::utf8_length(obj.title) < Task::field_constraints::TITLE_MIN_LENGTH) {
        return validation_error{"title", validation_error_code::string_too_short, Task::field_constraints::TITLE_MIN_LENGTH};
    }
    if (katana::utf8_length(obj.title) > Task::field_constraints::TITLE_MAX_LENGTH) {
        return validation_error{"title", validation_error_code::string_too_long, Task::field_constraints::TITLE_MAX_LENGTH};
    }
    if (obj.description && katana::utf8_length((*obj.description)) > Task::field_constraints::DESCRIPTION_MAX_LENGTH) {
        return validation_error{"description", validation_error_code::string_too_long, Task::field_constraints::DESCRIPTION_MAX_LENGTH};
    }
    if (static_cast<double>(obj.priority) < Task::field_constraints::PRIORITY_MINIMUM) {
        return validation_error{"priority", validation_error_code::value_too_small, Task::field_constraints::PRIORITY_MINIMUM};
    }
    if (static_cast<double>(obj.priority) > Task::field_constraints::PRIORITY_MAXIMUM) {
        return validation_error{"priority", validation_error_code::value_too_large, Task::field_constraints::PRIORITY_MAXIMUM};
    }
    if (obj.tags && obj.tags->size() > Task::field_constraints::TAGS_MAX_ITEMS) {
        return validation_error{"tags", validation_error_code::array_too_large, Task::field_constraints::TAGS_MAX_ITEMS};
    }
    {
        if (!obj.tags) {
            // nullable/omitted array: uniqueness check does not apply
        } else {
        if ((*obj.tags).size() <= 64) {
            // Small array: sort a copy (stack-friendly, no heap)
            std::vector<std::string_view> tmp_((*obj.tags).begin(), (*obj.tags).end());
            std::sort(tmp_.begin(), tmp_.end());
            if (std::adjacent_find(tmp_.begin(), tmp_.end()) != tmp_.end()) {
                return validation_error{"tags", validation_error_code::array_items_not_unique};
            }
        } else {
            // Large array: hash set
            std::unordered_set<std::string_view> seen_;
            for (const auto& v : (*obj.tags)) {
                if (!seen_.insert(v).second) {
                    return validation_error{"tags", validation_error_code::array_items_not_unique};
                }
            }
        }
        }
    }
    if (obj.due_date && !obj.due_date->empty() && !is_valid_datetime((*obj.due_date))) {
        return validation_error{"due_date", validation_error_code::invalid_datetime_format};
    }
    if (obj.created_at.empty()) {
        return validation_error{"created_at", validation_error_code::required_field_missing};
    }
    if (!obj.created_at.empty() && !is_valid_datetime(obj.created_at)) {
        return validation_error{"created_at", validation_error_code::invalid_datetime_format};
    }
    if (obj.updated_at && !obj.updated_at->empty() && !is_valid_datetime((*obj.updated_at))) {
        return validation_error{"updated_at", validation_error_code::invalid_datetime_format};
    }
    if (obj.assignee) { if (auto e_ = validate_User(*obj.assignee)) return e_; }
    return std::nullopt;
}

// validate Task_Tags_t — array, field Task.tags  ← api.yaml:233
[[nodiscard]] inline std::optional<validation_error> validate_Task_Tags_t(const Task_Tags_t& arr) {
    if (arr.size() > 20) return validation_error{"", validation_error_code::array_too_large, 20};
    {
        if (arr.size() <= 64) {
            // Small array: sort a copy (stack-friendly, no heap)
            std::vector<std::string_view> tmp_(arr.begin(), arr.end());
            std::sort(tmp_.begin(), tmp_.end());
            if (std::adjacent_find(tmp_.begin(), tmp_.end()) != tmp_.end()) {
                return validation_error{"", validation_error_code::array_items_not_unique};
            }
        } else {
            // Large array: hash set
            std::unordered_set<std::string_view> seen_;
            for (const auto& v : arr) {
                if (!seen_.insert(v).second) {
                    return validation_error{"", validation_error_code::array_items_not_unique};
                }
            }
        }
    }
    return std::nullopt;
}

// validate User — object, 3 field(s)  ← api.yaml:258
[[nodiscard]] inline std::optional<validation_error> validate_User(const User& obj) {
    if (static_cast<double>(obj.id) < User::field_constraints::ID_MINIMUM) {
        return validation_error{"id", validation_error_code::value_too_small, User::field_constraints::ID_MINIMUM};
    }
    if (obj.email.empty()) {
        return validation_error{"email", validation_error_code::required_field_missing};
    }
    if (!obj.email.empty() && katana::utf8_length(obj.email) < User::field_constraints::EMAIL_MIN_LENGTH) {
        return validation_error{"email", validation_error_code::string_too_short, User::field_constraints::EMAIL_MIN_LENGTH};
    }
    if (katana::utf8_length(obj.email) > User::field_constraints::EMAIL_MAX_LENGTH) {
        return validation_error{"email", validation_error_code::string_too_long, User::field_constraints::EMAIL_MAX_LENGTH};
    }
    if (!obj.email.empty() && !is_valid_email(obj.email)) {
        return validation_error{"email", validation_error_code::invalid_email_format};
    }
    if (obj.name && !obj.name->empty() && katana::utf8_length((*obj.name)) < User::field_constraints::NAME_MIN_LENGTH) {
        return validation_error{"name", validation_error_code::string_too_short, User::field_constraints::NAME_MIN_LENGTH};
    }
    if (obj.name && katana::utf8_length((*obj.name)) > User::field_constraints::NAME_MAX_LENGTH) {
        return validation_error{"name", validation_error_code::string_too_long, User::field_constraints::NAME_MAX_LENGTH};
    }
    return std::nullopt;
}

// validate CreateTaskRequest — object, 6 field(s)  ← api.yaml:278
[[nodiscard]] inline std::optional<validation_error> validate_CreateTaskRequest(const CreateTaskRequest& obj) {
    if (obj.title.empty()) {
        return validation_error{"title", validation_error_code::required_field_missing};
    }
    if (!obj.title.empty() && katana::utf8_length(obj.title) < CreateTaskRequest::field_constraints::TITLE_MIN_LENGTH) {
        return validation_error{"title", validation_error_code::string_too_short, CreateTaskRequest::field_constraints::TITLE_MIN_LENGTH};
    }
    if (katana::utf8_length(obj.title) > CreateTaskRequest::field_constraints::TITLE_MAX_LENGTH) {
        return validation_error{"title", validation_error_code::string_too_long, CreateTaskRequest::field_constraints::TITLE_MAX_LENGTH};
    }
    if (obj.description && katana::utf8_length((*obj.description)) > CreateTaskRequest::field_constraints::DESCRIPTION_MAX_LENGTH) {
        return validation_error{"description", validation_error_code::string_too_long, CreateTaskRequest::field_constraints::DESCRIPTION_MAX_LENGTH};
    }
    if (static_cast<double>(obj.priority) < CreateTaskRequest::field_constraints::PRIORITY_MINIMUM) {
        return validation_error{"priority", validation_error_code::value_too_small, CreateTaskRequest::field_constraints::PRIORITY_MINIMUM};
    }
    if (static_cast<double>(obj.priority) > CreateTaskRequest::field_constraints::PRIORITY_MAXIMUM) {
        return validation_error{"priority", validation_error_code::value_too_large, CreateTaskRequest::field_constraints::PRIORITY_MAXIMUM};
    }
    if (obj.tags && obj.tags->size() > CreateTaskRequest::field_constraints::TAGS_MAX_ITEMS) {
        return validation_error{"tags", validation_error_code::array_too_large, CreateTaskRequest::field_constraints::TAGS_MAX_ITEMS};
    }
    {
        if (!obj.tags) {
            // nullable/omitted array: uniqueness check does not apply
        } else {
        if ((*obj.tags).size() <= 64) {
            // Small array: sort a copy (stack-friendly, no heap)
            std::vector<std::string_view> tmp_((*obj.tags).begin(), (*obj.tags).end());
            std::sort(tmp_.begin(), tmp_.end());
            if (std::adjacent_find(tmp_.begin(), tmp_.end()) != tmp_.end()) {
                return validation_error{"tags", validation_error_code::array_items_not_unique};
            }
        } else {
            // Large array: hash set
            std::unordered_set<std::string_view> seen_;
            for (const auto& v : (*obj.tags)) {
                if (!seen_.insert(v).second) {
                    return validation_error{"tags", validation_error_code::array_items_not_unique};
                }
            }
        }
        }
    }
    if (obj.assignee_id && static_cast<double>((*obj.assignee_id)) < CreateTaskRequest::field_constraints::ASSIGNEE_ID_MINIMUM) {
        return validation_error{"assignee_id", validation_error_code::value_too_small, CreateTaskRequest::field_constraints::ASSIGNEE_ID_MINIMUM};
    }
    if (obj.due_date && !obj.due_date->empty() && !is_valid_datetime((*obj.due_date))) {
        return validation_error{"due_date", validation_error_code::invalid_datetime_format};
    }
    return std::nullopt;
}

// validate CreateTaskRequest_Tags_t — array, field CreateTaskRequest.tags  ← api.yaml:295
[[nodiscard]] inline std::optional<validation_error> validate_CreateTaskRequest_Tags_t(const CreateTaskRequest_Tags_t& arr) {
    if (arr.size() > 20) return validation_error{"", validation_error_code::array_too_large, 20};
    {
        if (arr.size() <= 64) {
            // Small array: sort a copy (stack-friendly, no heap)
            std::vector<std::string_view> tmp_(arr.begin(), arr.end());
            std::sort(tmp_.begin(), tmp_.end());
            if (std::adjacent_find(tmp_.begin(), tmp_.end()) != tmp_.end()) {
                return validation_error{"", validation_error_code::array_items_not_unique};
            }
        } else {
            // Large array: hash set
            std::unordered_set<std::string_view> seen_;
            for (const auto& v : arr) {
                if (!seen_.insert(v).second) {
                    return validation_error{"", validation_error_code::array_items_not_unique};
                }
            }
        }
    }
    return std::nullopt;
}

// validate UpdateTaskRequest — object, 7 field(s)  ← api.yaml:313
[[nodiscard]] inline std::optional<validation_error> validate_UpdateTaskRequest(const UpdateTaskRequest& obj) {
    if (obj.title && !obj.title->empty() && katana::utf8_length((*obj.title)) < UpdateTaskRequest::field_constraints::TITLE_MIN_LENGTH) {
        return validation_error{"title", validation_error_code::string_too_short, UpdateTaskRequest::field_constraints::TITLE_MIN_LENGTH};
    }
    if (obj.title && katana::utf8_length((*obj.title)) > UpdateTaskRequest::field_constraints::TITLE_MAX_LENGTH) {
        return validation_error{"title", validation_error_code::string_too_long, UpdateTaskRequest::field_constraints::TITLE_MAX_LENGTH};
    }
    if (obj.description && katana::utf8_length((*obj.description)) > UpdateTaskRequest::field_constraints::DESCRIPTION_MAX_LENGTH) {
        return validation_error{"description", validation_error_code::string_too_long, UpdateTaskRequest::field_constraints::DESCRIPTION_MAX_LENGTH};
    }
    if (obj.priority && static_cast<double>((*obj.priority)) < UpdateTaskRequest::field_constraints::PRIORITY_MINIMUM) {
        return validation_error{"priority", validation_error_code::value_too_small, UpdateTaskRequest::field_constraints::PRIORITY_MINIMUM};
    }
    if (obj.priority && static_cast<double>((*obj.priority)) > UpdateTaskRequest::field_constraints::PRIORITY_MAXIMUM) {
        return validation_error{"priority", validation_error_code::value_too_large, UpdateTaskRequest::field_constraints::PRIORITY_MAXIMUM};
    }
    if (obj.tags && obj.tags->size() > UpdateTaskRequest::field_constraints::TAGS_MAX_ITEMS) {
        return validation_error{"tags", validation_error_code::array_too_large, UpdateTaskRequest::field_constraints::TAGS_MAX_ITEMS};
    }
    {
        if (!obj.tags) {
            // nullable/omitted array: uniqueness check does not apply
        } else {
        if ((*obj.tags).size() <= 64) {
            // Small array: sort a copy (stack-friendly, no heap)
            std::vector<std::string_view> tmp_((*obj.tags).begin(), (*obj.tags).end());
            std::sort(tmp_.begin(), tmp_.end());
            if (std::adjacent_find(tmp_.begin(), tmp_.end()) != tmp_.end()) {
                return validation_error{"tags", validation_error_code::array_items_not_unique};
            }
        } else {
            // Large array: hash set
            std::unordered_set<std::string_view> seen_;
            for (const auto& v : (*obj.tags)) {
                if (!seen_.insert(v).second) {
                    return validation_error{"tags", validation_error_code::array_items_not_unique};
                }
            }
        }
        }
    }
    if (obj.assignee_id && static_cast<double>((*obj.assignee_id)) < UpdateTaskRequest::field_constraints::ASSIGNEE_ID_MINIMUM) {
        return validation_error{"assignee_id", validation_error_code::value_too_small, UpdateTaskRequest::field_constraints::ASSIGNEE_ID_MINIMUM};
    }
    if (obj.due_date && !obj.due_date->empty() && !is_valid_datetime((*obj.due_date))) {
        return validation_error{"due_date", validation_error_code::invalid_datetime_format};
    }
    return std::nullopt;
}

// validate UpdateTaskRequest_Tags_t — array, field UpdateTaskRequest.tags  ← api.yaml:330
[[nodiscard]] inline std::optional<validation_error> validate_UpdateTaskRequest_Tags_t(const UpdateTaskRequest_Tags_t& arr) {
    if (arr.size() > 20) return validation_error{"", validation_error_code::array_too_large, 20};
    {
        if (arr.size() <= 64) {
            // Small array: sort a copy (stack-friendly, no heap)
            std::vector<std::string_view> tmp_(arr.begin(), arr.end());
            std::sort(tmp_.begin(), tmp_.end());
            if (std::adjacent_find(tmp_.begin(), tmp_.end()) != tmp_.end()) {
                return validation_error{"", validation_error_code::array_items_not_unique};
            }
        } else {
            // Large array: hash set
            std::unordered_set<std::string_view> seen_;
            for (const auto& v : arr) {
                if (!seen_.insert(v).second) {
                    return validation_error{"", validation_error_code::array_items_not_unique};
                }
            }
        }
    }
    return std::nullopt;
}

// validate BatchCreateRequest — object, 1 field(s)  ← api.yaml:348
[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateRequest(const BatchCreateRequest& obj) {
    if (obj.tasks.empty()) {
        return validation_error{"tasks", validation_error_code::required_field_missing};
    }
    if (!obj.tasks.empty() && obj.tasks.size() < BatchCreateRequest::field_constraints::TASKS_MIN_ITEMS) {
        return validation_error{"tasks", validation_error_code::array_too_small, BatchCreateRequest::field_constraints::TASKS_MIN_ITEMS};
    }
    if (obj.tasks.size() > BatchCreateRequest::field_constraints::TASKS_MAX_ITEMS) {
        return validation_error{"tasks", validation_error_code::array_too_large, BatchCreateRequest::field_constraints::TASKS_MAX_ITEMS};
    }
    for (const auto& it_ : obj.tasks) { if (auto e_ = validate_CreateTaskRequest(it_)) return e_; }
    return std::nullopt;
}

// validate BatchCreateRequest_Tasks_t — array, field BatchCreateRequest.tasks  ← api.yaml:353
[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateRequest_Tasks_t(const BatchCreateRequest_Tasks_t& arr) {
    if (arr.size() < 1) return validation_error{"", validation_error_code::array_too_small, 1};
    if (arr.size() > 100) return validation_error{"", validation_error_code::array_too_large, 100};
    return std::nullopt;
}

// validate BatchCreateResponse — object, 2 field(s)  ← api.yaml:360
[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateResponse(const BatchCreateResponse& obj) {
    (void)obj;
    for (const auto& it_ : obj.created) { if (auto e_ = validate_Task(it_)) return e_; }
    for (const auto& it_ : obj.failed) { if (auto e_ = validate_BatchCreateResponse_Item_t_1(it_)) return e_; }
    return std::nullopt;
}

// validate BatchCreateResponse_Created_t — array, field BatchCreateResponse.created  ← api.yaml:366
[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateResponse_Created_t(const BatchCreateResponse_Created_t& arr) {
    (void)arr;
    return std::nullopt;
}

// validate BatchCreateResponse_Failed_t — array, field BatchCreateResponse.failed  ← api.yaml:370
[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateResponse_Failed_t(const BatchCreateResponse_Failed_t& arr) {
    (void)arr;
    return std::nullopt;
}

// validate BatchCreateResponse_Item_t_1 — object, field BatchCreateResponse.item  ← api.yaml:372
[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateResponse_Item_t_1(const BatchCreateResponse_Item_t_1& obj) {
    if (obj.error.empty()) {
        return validation_error{"error", validation_error_code::required_field_missing};
    }
    return std::nullopt;
}

// validate SearchRequest — object, 8 field(s)  ← api.yaml:383
[[nodiscard]] inline std::optional<validation_error> validate_SearchRequest(const SearchRequest& obj) {
    if (obj.title_contains && !obj.title_contains->empty() && katana::utf8_length((*obj.title_contains)) < SearchRequest::field_constraints::TITLE_CONTAINS_MIN_LENGTH) {
        return validation_error{"title_contains", validation_error_code::string_too_short, SearchRequest::field_constraints::TITLE_CONTAINS_MIN_LENGTH};
    }
    if (obj.title_contains && katana::utf8_length((*obj.title_contains)) > SearchRequest::field_constraints::TITLE_CONTAINS_MAX_LENGTH) {
        return validation_error{"title_contains", validation_error_code::string_too_long, SearchRequest::field_constraints::TITLE_CONTAINS_MAX_LENGTH};
    }
    if (obj.statuses && !obj.statuses->empty() && obj.statuses->size() < SearchRequest::field_constraints::STATUSES_MIN_ITEMS) {
        return validation_error{"statuses", validation_error_code::array_too_small, SearchRequest::field_constraints::STATUSES_MIN_ITEMS};
    }
    if (obj.statuses && obj.statuses->size() > SearchRequest::field_constraints::STATUSES_MAX_ITEMS) {
        return validation_error{"statuses", validation_error_code::array_too_large, SearchRequest::field_constraints::STATUSES_MAX_ITEMS};
    }
    {
        if (!obj.statuses) {
            // nullable/omitted array: uniqueness check does not apply
        } else {
        if ((*obj.statuses).size() <= 64) {
            // Small array: sort a copy (stack-friendly, no heap)
            auto tmp_ = (*obj.statuses);
            std::sort(tmp_.begin(), tmp_.end());
            if (std::adjacent_find(tmp_.begin(), tmp_.end()) != tmp_.end()) {
                return validation_error{"statuses", validation_error_code::array_items_not_unique};
            }
        } else {
            // Large array: hash set
            std::unordered_set<SearchRequest_Item_t_enum> seen_;
            for (const auto& v : (*obj.statuses)) {
                if (!seen_.insert(v).second) {
                    return validation_error{"statuses", validation_error_code::array_items_not_unique};
                }
            }
        }
        }
    }
    if (obj.min_priority && static_cast<double>((*obj.min_priority)) < SearchRequest::field_constraints::MIN_PRIORITY_MINIMUM) {
        return validation_error{"min_priority", validation_error_code::value_too_small, SearchRequest::field_constraints::MIN_PRIORITY_MINIMUM};
    }
    if (obj.min_priority && static_cast<double>((*obj.min_priority)) > SearchRequest::field_constraints::MIN_PRIORITY_MAXIMUM) {
        return validation_error{"min_priority", validation_error_code::value_too_large, SearchRequest::field_constraints::MIN_PRIORITY_MAXIMUM};
    }
    if (obj.max_priority && static_cast<double>((*obj.max_priority)) < SearchRequest::field_constraints::MAX_PRIORITY_MINIMUM) {
        return validation_error{"max_priority", validation_error_code::value_too_small, SearchRequest::field_constraints::MAX_PRIORITY_MINIMUM};
    }
    if (obj.max_priority && static_cast<double>((*obj.max_priority)) > SearchRequest::field_constraints::MAX_PRIORITY_MAXIMUM) {
        return validation_error{"max_priority", validation_error_code::value_too_large, SearchRequest::field_constraints::MAX_PRIORITY_MAXIMUM};
    }
    if (obj.tags && obj.tags->size() > SearchRequest::field_constraints::TAGS_MAX_ITEMS) {
        return validation_error{"tags", validation_error_code::array_too_large, SearchRequest::field_constraints::TAGS_MAX_ITEMS};
    }
    if (obj.created_after && !obj.created_after->empty() && !is_valid_datetime((*obj.created_after))) {
        return validation_error{"created_after", validation_error_code::invalid_datetime_format};
    }
    if (obj.created_before && !obj.created_before->empty() && !is_valid_datetime((*obj.created_before))) {
        return validation_error{"created_before", validation_error_code::invalid_datetime_format};
    }
    return std::nullopt;
}

// validate SearchRequest_Statuses_t — array, field SearchRequest.statuses  ← api.yaml:390
[[nodiscard]] inline std::optional<validation_error> validate_SearchRequest_Statuses_t(const SearchRequest_Statuses_t& arr) {
    if (arr.size() < 1) return validation_error{"", validation_error_code::array_too_small, 1};
    if (arr.size() > 4) return validation_error{"", validation_error_code::array_too_large, 4};
    {
        if (arr.size() <= 64) {
            // Small array: sort a copy (stack-friendly, no heap)
            auto tmp_ = arr;
            std::sort(tmp_.begin(), tmp_.end());
            if (std::adjacent_find(tmp_.begin(), tmp_.end()) != tmp_.end()) {
                return validation_error{"", validation_error_code::array_items_not_unique};
            }
        } else {
            // Large array: hash set
            std::unordered_set<SearchRequest_Item_t_enum> seen_;
            for (const auto& v : arr) {
                if (!seen_.insert(v).second) {
                    return validation_error{"", validation_error_code::array_items_not_unique};
                }
            }
        }
    }
    return std::nullopt;
}

// validate SearchRequest_Tags_t — array, field SearchRequest.tags  ← api.yaml:406
[[nodiscard]] inline std::optional<validation_error> validate_SearchRequest_Tags_t(const SearchRequest_Tags_t& arr) {
    if (arr.size() > 10) return validation_error{"", validation_error_code::array_too_large, 10};
    return std::nullopt;
}

// validate TaskList — object, 3 field(s)  ← api.yaml:420
[[nodiscard]] inline std::optional<validation_error> validate_TaskList(const TaskList& obj) {
    if (static_cast<double>(obj.total) < TaskList::field_constraints::TOTAL_MINIMUM) {
        return validation_error{"total", validation_error_code::value_too_small, TaskList::field_constraints::TOTAL_MINIMUM};
    }
    for (const auto& it_ : obj.tasks) { if (auto e_ = validate_Task(it_)) return e_; }
    return std::nullopt;
}

// validate TaskList_Tasks_t — array, field TaskList.tasks  ← api.yaml:426
[[nodiscard]] inline std::optional<validation_error> validate_TaskList_Tasks_t(const TaskList_Tasks_t& arr) {
    (void)arr;
    return std::nullopt;
}

// validate HealthResponse — object, 4 field(s)  ← api.yaml:436
[[nodiscard]] inline std::optional<validation_error> validate_HealthResponse(const HealthResponse& obj) {
    if (obj.timestamp.empty()) {
        return validation_error{"timestamp", validation_error_code::required_field_missing};
    }
    if (!obj.timestamp.empty() && !is_valid_datetime(obj.timestamp)) {
        return validation_error{"timestamp", validation_error_code::invalid_datetime_format};
    }
    if (obj.uptime_seconds && static_cast<double>((*obj.uptime_seconds)) < HealthResponse::field_constraints::UPTIME_SECONDS_MINIMUM) {
        return validation_error{"uptime_seconds", validation_error_code::value_too_small, HealthResponse::field_constraints::UPTIME_SECONDS_MINIMUM};
    }
    if (obj.total_requests && static_cast<double>((*obj.total_requests)) < HealthResponse::field_constraints::TOTAL_REQUESTS_MINIMUM) {
        return validation_error{"total_requests", validation_error_code::value_too_small, HealthResponse::field_constraints::TOTAL_REQUESTS_MINIMUM};
    }
    return std::nullopt;
}

// validate ProblemDetails — object, 5 field(s)  ← api.yaml:455
[[nodiscard]] inline std::optional<validation_error> validate_ProblemDetails(const ProblemDetails& obj) {
    if (obj.type.empty()) {
        return validation_error{"type", validation_error_code::required_field_missing};
    }
    if (obj.title.empty()) {
        return validation_error{"title", validation_error_code::required_field_missing};
    }
    return std::nullopt;
}

