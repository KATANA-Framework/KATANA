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

[[nodiscard]] inline std::optional<validation_error> validate_CreateTaskRequest(const CreateTaskRequest&);
[[nodiscard]] inline std::optional<validation_error> validate_CreateTaskRequest_Tags_t(const CreateTaskRequest_Tags_t&);
[[nodiscard]] inline std::optional<validation_error> validate_UpdateTaskRequest(const UpdateTaskRequest&);
[[nodiscard]] inline std::optional<validation_error> validate_UpdateTaskRequest_Tags_t(const UpdateTaskRequest_Tags_t&);
[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateRequest(const BatchCreateRequest&);
[[nodiscard]] inline std::optional<validation_error> validate_BatchCreateRequest_Tasks_t(const BatchCreateRequest_Tasks_t&);
[[nodiscard]] inline std::optional<validation_error> validate_SearchRequest(const SearchRequest&);
[[nodiscard]] inline std::optional<validation_error> validate_SearchRequest_Statuses_t(const SearchRequest_Statuses_t&);
[[nodiscard]] inline std::optional<validation_error> validate_SearchRequest_Tags_t(const SearchRequest_Tags_t&);

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

