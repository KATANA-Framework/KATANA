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
#include "katana/core/format_validators.hpp"
#include "katana/core/validation.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

using katana::validation_error;
using katana::validation_error_code;

// ============================================================
// Format Validators (from framework)
// ============================================================

using katana::format_validators::is_valid_datetime;
using katana::format_validators::is_valid_email;
using katana::format_validators::is_valid_uuid;

// ============================================================
// Validation Functions
// ============================================================

[[nodiscard]] inline std::optional<validation_error> validate_SumRequest(const SumRequest& obj) {
    if (obj.values.empty()) {
        return validation_error{"values", validation_error_code::required_field_missing};
    }
    if (!obj.values.empty() && obj.values.size() < SumRequest::metadata::VALUES_MIN_ITEMS) {
        return validation_error{"values",
                                validation_error_code::array_too_small,
                                SumRequest::metadata::VALUES_MIN_ITEMS};
    }
    if (obj.values.size() > SumRequest::metadata::VALUES_MAX_ITEMS) {
        return validation_error{"values",
                                validation_error_code::array_too_large,
                                SumRequest::metadata::VALUES_MAX_ITEMS};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error>
validate_SumRequest_Values_t(const SumRequest_Values_t& arr) {
    if (arr.size() < 1)
        return validation_error{"", validation_error_code::array_too_small, 1};
    if (arr.size() > 10000)
        return validation_error{"", validation_error_code::array_too_large, 10000};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_SumResponse(const SumResponse& obj) {
    (void)obj;
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error>
validate_StatsRequest(const StatsRequest& obj) {
    if (obj.values.empty()) {
        return validation_error{"values", validation_error_code::required_field_missing};
    }
    if (!obj.values.empty() && obj.values.size() < StatsRequest::metadata::VALUES_MIN_ITEMS) {
        return validation_error{"values",
                                validation_error_code::array_too_small,
                                StatsRequest::metadata::VALUES_MIN_ITEMS};
    }
    if (obj.values.size() > StatsRequest::metadata::VALUES_MAX_ITEMS) {
        return validation_error{"values",
                                validation_error_code::array_too_large,
                                StatsRequest::metadata::VALUES_MAX_ITEMS};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error>
validate_StatsRequest_Values_t(const StatsRequest_Values_t& arr) {
    if (arr.size() < 1)
        return validation_error{"", validation_error_code::array_too_small, 1};
    if (arr.size() > 10000)
        return validation_error{"", validation_error_code::array_too_large, 10000};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error>
validate_StatsResponse(const StatsResponse& obj) {
    (void)obj;
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error>
validate_RegisterRequest(const RegisterRequest& obj) {
    if (obj.username.empty()) {
        return validation_error{"username", validation_error_code::required_field_missing};
    }
    if (!obj.username.empty() &&
        obj.username.size() < RegisterRequest::metadata::USERNAME_MIN_LENGTH) {
        return validation_error{"username",
                                validation_error_code::string_too_short,
                                RegisterRequest::metadata::USERNAME_MIN_LENGTH};
    }
    if (obj.username.size() > RegisterRequest::metadata::USERNAME_MAX_LENGTH) {
        return validation_error{"username",
                                validation_error_code::string_too_long,
                                RegisterRequest::metadata::USERNAME_MAX_LENGTH};
    }
    if (!obj.username.empty()) {
        const auto& pv_ = obj.username;
        bool pattern_ok_ = true;
        for (unsigned char ch_ : pv_) {
            if (!std::isalnum(ch_) && ch_ != '_' && ch_ != '-') {
                pattern_ok_ = false;
                break;
            }
        }
        if (!pattern_ok_) {
            return validation_error{"username", validation_error_code::pattern_mismatch};
        }
    }
    if (obj.email.empty()) {
        return validation_error{"email", validation_error_code::required_field_missing};
    }
    if (!obj.email.empty() && !is_valid_email(obj.email)) {
        return validation_error{"email", validation_error_code::invalid_email_format};
    }
    if (obj.password.empty()) {
        return validation_error{"password", validation_error_code::required_field_missing};
    }
    if (!obj.password.empty() &&
        obj.password.size() < RegisterRequest::metadata::PASSWORD_MIN_LENGTH) {
        return validation_error{"password",
                                validation_error_code::string_too_short,
                                RegisterRequest::metadata::PASSWORD_MIN_LENGTH};
    }
    if (obj.password.size() > RegisterRequest::metadata::PASSWORD_MAX_LENGTH) {
        return validation_error{"password",
                                validation_error_code::string_too_long,
                                RegisterRequest::metadata::PASSWORD_MAX_LENGTH};
    }
    if (obj.age && static_cast<double>((*obj.age)) < RegisterRequest::metadata::AGE_MINIMUM) {
        return validation_error{
            "age", validation_error_code::value_too_small, RegisterRequest::metadata::AGE_MINIMUM};
    }
    if (obj.age && static_cast<double>((*obj.age)) > RegisterRequest::metadata::AGE_MAXIMUM) {
        return validation_error{
            "age", validation_error_code::value_too_large, RegisterRequest::metadata::AGE_MAXIMUM};
    }
    if (obj.tags && obj.tags->size() > RegisterRequest::metadata::TAGS_MAX_ITEMS) {
        return validation_error{"tags",
                                validation_error_code::array_too_large,
                                RegisterRequest::metadata::TAGS_MAX_ITEMS};
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
                        return validation_error{"tags",
                                                validation_error_code::array_items_not_unique};
                    }
                }
            }
        }
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error>
validate_RegisterRequest_Tags_t(const RegisterRequest_Tags_t& arr) {
    if (arr.size() > 10)
        return validation_error{"", validation_error_code::array_too_large, 10};
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

[[nodiscard]] inline std::optional<validation_error>
validate_UserResponse(const UserResponse& obj) {
    if (obj.id.empty()) {
        return validation_error{"id", validation_error_code::required_field_missing};
    }
    if (!obj.id.empty() && !is_valid_uuid(obj.id)) {
        return validation_error{"id", validation_error_code::invalid_uuid_format};
    }
    if (obj.username.empty()) {
        return validation_error{"username", validation_error_code::required_field_missing};
    }
    if (obj.email.empty()) {
        return validation_error{"email", validation_error_code::required_field_missing};
    }
    if (obj.created_at.empty()) {
        return validation_error{"created_at", validation_error_code::required_field_missing};
    }
    if (!obj.created_at.empty() && !is_valid_datetime(obj.created_at)) {
        return validation_error{"created_at", validation_error_code::invalid_datetime_format};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_Item(const Item& obj) {
    if (obj.name.empty()) {
        return validation_error{"name", validation_error_code::required_field_missing};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_Item_Tags_t(const Item_Tags_t& arr) {
    (void)arr;
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error>
validate_ItemMetadata(const ItemMetadata& obj) {
    (void)obj;
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error>
validate_CreateItemRequest(const CreateItemRequest& obj) {
    if (obj.name.empty()) {
        return validation_error{"name", validation_error_code::required_field_missing};
    }
    if (!obj.name.empty() && obj.name.size() < CreateItemRequest::metadata::NAME_MIN_LENGTH) {
        return validation_error{"name",
                                validation_error_code::string_too_short,
                                CreateItemRequest::metadata::NAME_MIN_LENGTH};
    }
    if (obj.name.size() > CreateItemRequest::metadata::NAME_MAX_LENGTH) {
        return validation_error{"name",
                                validation_error_code::string_too_long,
                                CreateItemRequest::metadata::NAME_MAX_LENGTH};
    }
    if (obj.description &&
        obj.description->size() > CreateItemRequest::metadata::DESCRIPTION_MAX_LENGTH) {
        return validation_error{"description",
                                validation_error_code::string_too_long,
                                CreateItemRequest::metadata::DESCRIPTION_MAX_LENGTH};
    }
    if (static_cast<double>(obj.price) < CreateItemRequest::metadata::PRICE_MINIMUM) {
        return validation_error{"price",
                                validation_error_code::value_too_small,
                                CreateItemRequest::metadata::PRICE_MINIMUM};
    }
    if (static_cast<double>(obj.price) >= CreateItemRequest::metadata::PRICE_EXCLUSIVE_MAXIMUM) {
        return validation_error{"price",
                                validation_error_code::value_above_exclusive_maximum,
                                CreateItemRequest::metadata::PRICE_EXCLUSIVE_MAXIMUM};
    }
    if (obj.stock &&
        static_cast<double>((*obj.stock)) < CreateItemRequest::metadata::STOCK_MINIMUM) {
        return validation_error{"stock",
                                validation_error_code::value_too_small,
                                CreateItemRequest::metadata::STOCK_MINIMUM};
    }
    if (obj.stock &&
        static_cast<double>((*obj.stock)) > CreateItemRequest::metadata::STOCK_MAXIMUM) {
        return validation_error{"stock",
                                validation_error_code::value_too_large,
                                CreateItemRequest::metadata::STOCK_MAXIMUM};
    }
    if (obj.tags && obj.tags->size() > CreateItemRequest::metadata::TAGS_MAX_ITEMS) {
        return validation_error{"tags",
                                validation_error_code::array_too_large,
                                CreateItemRequest::metadata::TAGS_MAX_ITEMS};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error>
validate_CreateItemRequest_Tags_t(const CreateItemRequest_Tags_t& arr) {
    if (arr.size() > 20)
        return validation_error{"", validation_error_code::array_too_large, 20};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error>
validate_UpdateItemRequest(const UpdateItemRequest& obj) {
    if (obj.name && !obj.name->empty() &&
        obj.name->size() < UpdateItemRequest::metadata::NAME_MIN_LENGTH) {
        return validation_error{"name",
                                validation_error_code::string_too_short,
                                UpdateItemRequest::metadata::NAME_MIN_LENGTH};
    }
    if (obj.name && obj.name->size() > UpdateItemRequest::metadata::NAME_MAX_LENGTH) {
        return validation_error{"name",
                                validation_error_code::string_too_long,
                                UpdateItemRequest::metadata::NAME_MAX_LENGTH};
    }
    if (obj.description &&
        obj.description->size() > UpdateItemRequest::metadata::DESCRIPTION_MAX_LENGTH) {
        return validation_error{"description",
                                validation_error_code::string_too_long,
                                UpdateItemRequest::metadata::DESCRIPTION_MAX_LENGTH};
    }
    if (obj.price &&
        static_cast<double>((*obj.price)) < UpdateItemRequest::metadata::PRICE_MINIMUM) {
        return validation_error{"price",
                                validation_error_code::value_too_small,
                                UpdateItemRequest::metadata::PRICE_MINIMUM};
    }
    if (obj.stock &&
        static_cast<double>((*obj.stock)) < UpdateItemRequest::metadata::STOCK_MINIMUM) {
        return validation_error{"stock",
                                validation_error_code::value_too_small,
                                UpdateItemRequest::metadata::STOCK_MINIMUM};
    }
    if (obj.tags && obj.tags->size() > UpdateItemRequest::metadata::TAGS_MAX_ITEMS) {
        return validation_error{"tags",
                                validation_error_code::array_too_large,
                                UpdateItemRequest::metadata::TAGS_MAX_ITEMS};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error>
validate_UpdateItemRequest_Tags_t(const UpdateItemRequest_Tags_t& arr) {
    if (arr.size() > 20)
        return validation_error{"", validation_error_code::array_too_large, 20};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_ItemList(const ItemList& obj) {
    (void)obj;
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error>
validate_ItemList_Items_t(const ItemList_Items_t& arr) {
    (void)arr;
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_EchoRequest(const EchoRequest& obj) {
    if (obj.message.empty()) {
        return validation_error{"message", validation_error_code::required_field_missing};
    }
    if (obj.message.size() > EchoRequest::metadata::MESSAGE_MAX_LENGTH) {
        return validation_error{"message",
                                validation_error_code::string_too_long,
                                EchoRequest::metadata::MESSAGE_MAX_LENGTH};
    }
    if (obj.repeat && static_cast<double>((*obj.repeat)) < EchoRequest::metadata::REPEAT_MINIMUM) {
        return validation_error{"repeat",
                                validation_error_code::value_too_small,
                                EchoRequest::metadata::REPEAT_MINIMUM};
    }
    if (obj.repeat && static_cast<double>((*obj.repeat)) > EchoRequest::metadata::REPEAT_MAXIMUM) {
        return validation_error{"repeat",
                                validation_error_code::value_too_large,
                                EchoRequest::metadata::REPEAT_MAXIMUM};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error>
validate_EchoResponse(const EchoResponse& obj) {
    if (obj.message.empty()) {
        return validation_error{"message", validation_error_code::required_field_missing};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error>
validate_health_check_response(const health_check_response& obj) {
    (void)obj;
    return std::nullopt;
}
