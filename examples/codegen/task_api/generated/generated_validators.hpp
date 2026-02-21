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
#include <regex>
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

[[nodiscard]] inline std::optional<validation_error> validate_Task(const Task& obj) {
    if (static_cast<double>(obj.id) < Task::metadata::ID_MINIMUM) {
        return validation_error{"id", validation_error_code::value_too_small, Task::metadata::ID_MINIMUM};
    }
    if (obj.title.empty()) {
        return validation_error{"title", validation_error_code::required_field_missing};
    }
    if (!obj.title.empty() && obj.title.size() < Task::metadata::TITLE_MIN_LENGTH) {
        return validation_error{"title", validation_error_code::string_too_short, Task::metadata::TITLE_MIN_LENGTH};
    }
    if (obj.title.size() > Task::metadata::TITLE_MAX_LENGTH) {
        return validation_error{"title", validation_error_code::string_too_long, Task::metadata::TITLE_MAX_LENGTH};
    }
    if (obj.description.size() > Task::metadata::DESCRIPTION_MAX_LENGTH) {
        return validation_error{"description", validation_error_code::string_too_long, Task::metadata::DESCRIPTION_MAX_LENGTH};
    }
    if (static_cast<double>(obj.priority) < Task::metadata::PRIORITY_MINIMUM) {
        return validation_error{"priority", validation_error_code::value_too_small, Task::metadata::PRIORITY_MINIMUM};
    }
    if (static_cast<double>(obj.priority) > Task::metadata::PRIORITY_MAXIMUM) {
        return validation_error{"priority", validation_error_code::value_too_large, Task::metadata::PRIORITY_MAXIMUM};
    }
    if (obj.tags.size() > Task::metadata::TAGS_MAX_ITEMS) {
        return validation_error{"tags", validation_error_code::array_too_large, Task::metadata::TAGS_MAX_ITEMS};
    }
    {
        if (obj.tags.size() <= 64) {
            // Small array: sort a copy (stack-friendly, no heap)
            std::vector<std::string_view> tmp_(obj.tags.begin(), obj.tags.end());
            std::sort(tmp_.begin(), tmp_.end());
            if (std::adjacent_find(tmp_.begin(), tmp_.end()) != tmp_.end()) {
                return validation_error{"tags", validation_error_code::array_items_not_unique};
            }
        } else {
            // Large array: hash set
            std::unordered_set<std::string_view> seen_;
            for (const auto& v : obj.tags) {
                if (!seen_.insert(v).second) {
                    return validation_error{"tags", validation_error_code::array_items_not_unique};
                }
            }
        }
    }
    if (obj.due_date && !obj.due_date->empty() && !is_valid_datetime(*obj.due_date)) {
        return validation_error{"due_date", validation_error_code::invalid_datetime_format};
    }
    if (obj.created_at.empty()) {
        return validation_error{"created_at", validation_error_code::required_field_missing};
    }
    if (!obj.created_at.empty() && !is_valid_datetime(obj.created_at)) {
        return validation_error{"created_at", validation_error_code::invalid_datetime_format};
    }
    if (obj.updated_at && !obj.updated_at->empty() && !is_valid_datetime(*obj.updated_at)) {
        return validation_error{"updated_at", validation_error_code::invalid_datetime_format};
    }
    return std::nullopt;
}

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

[[nodiscard]] inline std::optional<validation_error> validate_User(const User& obj) {
    if (static_cast<double>(obj.id) < User::metadata::ID_MINIMUM) {
        return validation_error{"id", validation_error_code::value_too_small, User::metadata::ID_MINIMUM};
    }
    if (obj.email.empty()) {
        return validation_error{"email", validation_error_code::required_field_missing};
    }
    if (!obj.email.empty() && obj.email.size() < User::metadata::EMAIL_MIN_LENGTH) {
        return validation_error{"email", validation_error_code::string_too_short, User::metadata::EMAIL_MIN_LENGTH};
    }
    if (obj.email.size() > User::metadata::EMAIL_MAX_LENGTH) {
        return validation_error{"email", validation_error_code::string_too_long, User::metadata::EMAIL_MAX_LENGTH};
    }
    if (!obj.email.empty() && !is_valid_email(obj.email)) {
        return validation_error{"email", validation_error_code::invalid_email_format};
    }
    if (!obj.name.empty() && obj.name.size() < User::metadata::NAME_MIN_LENGTH) {
        return validation_error{"name", validation_error_code::string_too_short, User::metadata::NAME_MIN_LENGTH};
    }
    if (obj.name.size() > User::metadata::NAME_MAX_LENGTH) {
        return validation_error{"name", validation_error_code::string_too_long, User::metadata::NAME_MAX_LENGTH};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_CreateTaskRequest(const CreateTaskRequest& obj) {
    if (obj.title.empty()) {
        return validation_error{"title", validation_error_code::required_field_missing};
    }
    if (!obj.title.empty() && obj.title.size() < CreateTaskRequest::metadata::TITLE_MIN_LENGTH) {
        return validation_error{"title", validation_error_code::string_too_short, CreateTaskRequest::metadata::TITLE_MIN_LENGTH};
    }
    if (obj.title.size() > CreateTaskRequest::metadata::TITLE_MAX_LENGTH) {
        return validation_error{"title", validation_error_code::string_too_long, CreateTaskRequest::metadata::TITLE_MAX_LENGTH};
    }
    if (obj.description.size() > CreateTaskRequest::metadata::DESCRIPTION_MAX_LENGTH) {
        return validation_error{"description", validation_error_code::string_too_long, CreateTaskRequest::metadata::DESCRIPTION_MAX_LENGTH};
    }
    if (static_cast<double>(obj.priority) < CreateTaskRequest::metadata::PRIORITY_MINIMUM) {
        return validation_error{"priority", validation_error_code::value_too_small, CreateTaskRequest::metadata::PRIORITY_MINIMUM};
    }
    if (static_cast<double>(obj.priority) > CreateTaskRequest::metadata::PRIORITY_MAXIMUM) {
        return validation_error{"priority", validation_error_code::value_too_large, CreateTaskRequest::metadata::PRIORITY_MAXIMUM};
    }
    if (obj.tags.size() > CreateTaskRequest::metadata::TAGS_MAX_ITEMS) {
        return validation_error{"tags", validation_error_code::array_too_large, CreateTaskRequest::metadata::TAGS_MAX_ITEMS};
    }
    {
        if (obj.tags.size() <= 64) {
            // Small array: sort a copy (stack-friendly, no heap)
            std::vector<std::string_view> tmp_(obj.tags.begin(), obj.tags.end());
            std::sort(tmp_.begin(), tmp_.end());
            if (std::adjacent_find(tmp_.begin(), tmp_.end()) != tmp_.end()) {
                return validation_error{"tags", validation_error_code::array_items_not_unique};
            }
        } else {
            // Large array: hash set
            std::unordered_set<std::string_view> seen_;
            for (const auto& v : obj.tags) {
                if (!seen_.insert(v).second) {
                    return validation_error{"tags", validation_error_code::array_items_not_unique};
                }
            }
        }
    }
    if (obj.assignee_id && static_cast<double>(*obj.assignee_id) < CreateTaskRequest::metadata::ASSIGNEE_ID_MINIMUM) {
        return validation_error{"assignee_id", validation_error_code::value_too_small, CreateTaskRequest::metadata::ASSIGNEE_ID_MINIMUM};
    }
    if (obj.due_date && !obj.due_date->empty() && !is_valid_datetime(*obj.due_date)) {
        return validation_error{"due_date", validation_error_code::invalid_datetime_format};
    }
    return std::nullopt;
}

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

[[nodiscard]] inline std::optional<validation_error> validate_UpdateTaskRequest(const UpdateTaskRequest& obj) {
    if (!obj.title.empty() && obj.title.size() < UpdateTaskRequest::metadata::TITLE_MIN_LENGTH) {
        return validation_error{"title", validation_error_code::string_too_short, UpdateTaskRequest::metadata::TITLE_MIN_LENGTH};
    }
    if (obj.title.size() > UpdateTaskRequest::metadata::TITLE_MAX_LENGTH) {
        return validation_error{"title", validation_error_code::string_too_long, UpdateTaskRequest::metadata::TITLE_MAX_LENGTH};
    }
    if (obj.description.size() > UpdateTaskRequest::metadata::DESCRIPTION_MAX_LENGTH) {
        return validation_error{"description", validation_error_code::string_too_long, UpdateTaskRequest::metadata::DESCRIPTION_MAX_LENGTH};
    }
    if (static_cast<double>(obj.priority) < UpdateTaskRequest::metadata::PRIORITY_MINIMUM) {
        return validation_error{"priority", validation_error_code::value_too_small, UpdateTaskRequest::metadata::PRIORITY_MINIMUM};
    }
    if (static_cast<double>(obj.priority) > UpdateTaskRequest::metadata::PRIORITY_MAXIMUM) {
        return validation_error{"priority", validation_error_code::value_too_large, UpdateTaskRequest::metadata::PRIORITY_MAXIMUM};
    }
    if (obj.tags.size() > UpdateTaskRequest::metadata::TAGS_MAX_ITEMS) {
        return validation_error{"tags", validation_error_code::array_too_large, UpdateTaskRequest::metadata::TAGS_MAX_ITEMS};
    }
    {
        if (obj.tags.size() <= 64) {
            // Small array: sort a copy (stack-friendly, no heap)
            std::vector<std::string_view> tmp_(obj.tags.begin(), obj.tags.end());
            std::sort(tmp_.begin(), tmp_.end());
            if (std::adjacent_find(tmp_.begin(), tmp_.end()) != tmp_.end()) {
                return validation_error{"tags", validation_error_code::array_items_not_unique};
            }
        } else {
            // Large array: hash set
            std::unordered_set<std::string_view> seen_;
            for (const auto& v : obj.tags) {
                if (!seen_.insert(v).second) {
                    return validation_error{"tags", validation_error_code::array_items_not_unique};
                }
            }
        }
    }
    if (obj.assignee_id && static_cast<double>(*obj.assignee_id) < UpdateTaskRequest::metadata::ASSIGNEE_ID_MINIMUM) {
        return validation_error{"assignee_id", validation_error_code::value_too_small, UpdateTaskRequest::metadata::ASSIGNEE_ID_MINIMUM};
    }
    if (obj.due_date && !obj.due_date->empty() && !is_valid_datetime(*obj.due_date)) {
        return validation_error{"due_date", validation_error_code::invalid_datetime_format};
    }
    return std::nullopt;
}

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

[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateRequest(const BatchCreateRequest& obj) {
    if (obj.tasks.empty()) {
        return validation_error{"tasks", validation_error_code::required_field_missing};
    }
    if (!obj.tasks.empty() && obj.tasks.size() < BatchCreateRequest::metadata::TASKS_MIN_ITEMS) {
        return validation_error{"tasks", validation_error_code::array_too_small, BatchCreateRequest::metadata::TASKS_MIN_ITEMS};
    }
    if (obj.tasks.size() > BatchCreateRequest::metadata::TASKS_MAX_ITEMS) {
        return validation_error{"tasks", validation_error_code::array_too_large, BatchCreateRequest::metadata::TASKS_MAX_ITEMS};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateRequest_Tasks_t(const BatchCreateRequest_Tasks_t& arr) {
    if (arr.size() < 1) return validation_error{"", validation_error_code::array_too_small, 1};
    if (arr.size() > 100) return validation_error{"", validation_error_code::array_too_large, 100};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateResponse(const BatchCreateResponse& obj) {
    (void)obj;
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateResponse_Created_t(const BatchCreateResponse_Created_t& arr) {
    (void)arr;
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateResponse_Failed_t(const BatchCreateResponse_Failed_t& arr) {
    (void)arr;
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateResponse_Item_t_1(const BatchCreateResponse_Item_t_1& obj) {
    if (obj.error.empty()) {
        return validation_error{"error", validation_error_code::required_field_missing};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_SearchRequest(const SearchRequest& obj) {
    if (!obj.title_contains.empty() && obj.title_contains.size() < SearchRequest::metadata::TITLE_CONTAINS_MIN_LENGTH) {
        return validation_error{"title_contains", validation_error_code::string_too_short, SearchRequest::metadata::TITLE_CONTAINS_MIN_LENGTH};
    }
    if (obj.title_contains.size() > SearchRequest::metadata::TITLE_CONTAINS_MAX_LENGTH) {
        return validation_error{"title_contains", validation_error_code::string_too_long, SearchRequest::metadata::TITLE_CONTAINS_MAX_LENGTH};
    }
    if (!obj.statuses.empty() && obj.statuses.size() < SearchRequest::metadata::STATUSES_MIN_ITEMS) {
        return validation_error{"statuses", validation_error_code::array_too_small, SearchRequest::metadata::STATUSES_MIN_ITEMS};
    }
    if (obj.statuses.size() > SearchRequest::metadata::STATUSES_MAX_ITEMS) {
        return validation_error{"statuses", validation_error_code::array_too_large, SearchRequest::metadata::STATUSES_MAX_ITEMS};
    }
    {
        if (obj.statuses.size() <= 64) {
            // Small array: sort a copy (stack-friendly, no heap)
            auto tmp_ = obj.statuses;
            std::sort(tmp_.begin(), tmp_.end());
            if (std::adjacent_find(tmp_.begin(), tmp_.end()) != tmp_.end()) {
                return validation_error{"statuses", validation_error_code::array_items_not_unique};
            }
        } else {
            // Large array: hash set
            std::unordered_set<SearchRequest_Item_t_enum> seen_;
            for (const auto& v : obj.statuses) {
                if (!seen_.insert(v).second) {
                    return validation_error{"statuses", validation_error_code::array_items_not_unique};
                }
            }
        }
    }
    if (static_cast<double>(obj.min_priority) < SearchRequest::metadata::MIN_PRIORITY_MINIMUM) {
        return validation_error{"min_priority", validation_error_code::value_too_small, SearchRequest::metadata::MIN_PRIORITY_MINIMUM};
    }
    if (static_cast<double>(obj.min_priority) > SearchRequest::metadata::MIN_PRIORITY_MAXIMUM) {
        return validation_error{"min_priority", validation_error_code::value_too_large, SearchRequest::metadata::MIN_PRIORITY_MAXIMUM};
    }
    if (static_cast<double>(obj.max_priority) < SearchRequest::metadata::MAX_PRIORITY_MINIMUM) {
        return validation_error{"max_priority", validation_error_code::value_too_small, SearchRequest::metadata::MAX_PRIORITY_MINIMUM};
    }
    if (static_cast<double>(obj.max_priority) > SearchRequest::metadata::MAX_PRIORITY_MAXIMUM) {
        return validation_error{"max_priority", validation_error_code::value_too_large, SearchRequest::metadata::MAX_PRIORITY_MAXIMUM};
    }
    if (obj.tags.size() > SearchRequest::metadata::TAGS_MAX_ITEMS) {
        return validation_error{"tags", validation_error_code::array_too_large, SearchRequest::metadata::TAGS_MAX_ITEMS};
    }
    if (!obj.created_after.empty() && !is_valid_datetime(obj.created_after)) {
        return validation_error{"created_after", validation_error_code::invalid_datetime_format};
    }
    if (!obj.created_before.empty() && !is_valid_datetime(obj.created_before)) {
        return validation_error{"created_before", validation_error_code::invalid_datetime_format};
    }
    return std::nullopt;
}

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

[[nodiscard]] inline std::optional<validation_error> validate_SearchRequest_Tags_t(const SearchRequest_Tags_t& arr) {
    if (arr.size() > 10) return validation_error{"", validation_error_code::array_too_large, 10};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_TaskList(const TaskList& obj) {
    if (static_cast<double>(obj.total) < TaskList::metadata::TOTAL_MINIMUM) {
        return validation_error{"total", validation_error_code::value_too_small, TaskList::metadata::TOTAL_MINIMUM};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_TaskList_Tasks_t(const TaskList_Tasks_t& arr) {
    (void)arr;
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_HealthResponse(const HealthResponse& obj) {
    if (obj.timestamp.empty()) {
        return validation_error{"timestamp", validation_error_code::required_field_missing};
    }
    if (!obj.timestamp.empty() && !is_valid_datetime(obj.timestamp)) {
        return validation_error{"timestamp", validation_error_code::invalid_datetime_format};
    }
    if (static_cast<double>(obj.uptime_seconds) < HealthResponse::metadata::UPTIME_SECONDS_MINIMUM) {
        return validation_error{"uptime_seconds", validation_error_code::value_too_small, HealthResponse::metadata::UPTIME_SECONDS_MINIMUM};
    }
    if (static_cast<double>(obj.total_requests) < HealthResponse::metadata::TOTAL_REQUESTS_MINIMUM) {
        return validation_error{"total_requests", validation_error_code::value_too_small, HealthResponse::metadata::TOTAL_REQUESTS_MINIMUM};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_ProblemDetails(const ProblemDetails& obj) {
    if (obj.type.empty()) {
        return validation_error{"type", validation_error_code::required_field_missing};
    }
    if (obj.title.empty()) {
        return validation_error{"title", validation_error_code::required_field_missing};
    }
    return std::nullopt;
}

