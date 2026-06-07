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

[[nodiscard]] inline std::optional<validation_error> validate_SumRequest(const SumRequest&);
[[nodiscard]] inline std::optional<validation_error> validate_SumRequest_Values_t(const SumRequest_Values_t&);
[[nodiscard]] inline std::optional<validation_error> validate_SumResponse(const SumResponse&);
[[nodiscard]] inline std::optional<validation_error> validate_StatsRequest(const StatsRequest&);
[[nodiscard]] inline std::optional<validation_error> validate_StatsRequest_Values_t(const StatsRequest_Values_t&);
[[nodiscard]] inline std::optional<validation_error> validate_StatsResponse(const StatsResponse&);
[[nodiscard]] inline std::optional<validation_error> validate_RegisterRequest(const RegisterRequest&);
[[nodiscard]] inline std::optional<validation_error> validate_RegisterRequest_Tags_t(const RegisterRequest_Tags_t&);
[[nodiscard]] inline std::optional<validation_error> validate_UserResponse(const UserResponse&);
[[nodiscard]] inline std::optional<validation_error> validate_Item(const Item&);
[[nodiscard]] inline std::optional<validation_error> validate_Item_Tags_t(const Item_Tags_t&);
[[nodiscard]] inline std::optional<validation_error> validate_ItemMetadata(const ItemMetadata&);
[[nodiscard]] inline std::optional<validation_error> validate_CreateItemRequest(const CreateItemRequest&);
[[nodiscard]] inline std::optional<validation_error> validate_CreateItemRequest_Tags_t(const CreateItemRequest_Tags_t&);
[[nodiscard]] inline std::optional<validation_error> validate_UpdateItemRequest(const UpdateItemRequest&);
[[nodiscard]] inline std::optional<validation_error> validate_UpdateItemRequest_Tags_t(const UpdateItemRequest_Tags_t&);
[[nodiscard]] inline std::optional<validation_error> validate_ItemList(const ItemList&);
[[nodiscard]] inline std::optional<validation_error> validate_ItemList_Items_t(const ItemList_Items_t&);
[[nodiscard]] inline std::optional<validation_error> validate_EchoRequest(const EchoRequest&);
[[nodiscard]] inline std::optional<validation_error> validate_EchoResponse(const EchoResponse&);
[[nodiscard]] inline std::optional<validation_error> validate_health_check_response(const health_check_response&);

// validate SumRequest — object, 1 field(s)  ← api.yaml:221
[[nodiscard]] inline std::optional<validation_error> validate_SumRequest(const SumRequest& obj) {
    if (obj.values.empty()) {
        return validation_error{"values", validation_error_code::required_field_missing};
    }
    if (!obj.values.empty() && obj.values.size() < SumRequest::field_constraints::VALUES_MIN_ITEMS) {
        return validation_error{"values", validation_error_code::array_too_small, SumRequest::field_constraints::VALUES_MIN_ITEMS};
    }
    if (obj.values.size() > SumRequest::field_constraints::VALUES_MAX_ITEMS) {
        return validation_error{"values", validation_error_code::array_too_large, SumRequest::field_constraints::VALUES_MAX_ITEMS};
    }
    return std::nullopt;
}

// validate SumRequest_Values_t — array, field SumRequest.values  ← api.yaml:225
[[nodiscard]] inline std::optional<validation_error> validate_SumRequest_Values_t(const SumRequest_Values_t& arr) {
    if (arr.size() < 1) return validation_error{"", validation_error_code::array_too_small, 1};
    if (arr.size() > 10000) return validation_error{"", validation_error_code::array_too_large, 10000};
    return std::nullopt;
}

// validate SumResponse — object, 2 field(s)  ← api.yaml:233
[[nodiscard]] inline std::optional<validation_error> validate_SumResponse(const SumResponse& obj) {
    (void)obj;
    return std::nullopt;
}

// validate StatsRequest — object, 2 field(s)  ← api.yaml:243
[[nodiscard]] inline std::optional<validation_error> validate_StatsRequest(const StatsRequest& obj) {
    if (obj.values.empty()) {
        return validation_error{"values", validation_error_code::required_field_missing};
    }
    if (!obj.values.empty() && obj.values.size() < StatsRequest::field_constraints::VALUES_MIN_ITEMS) {
        return validation_error{"values", validation_error_code::array_too_small, StatsRequest::field_constraints::VALUES_MIN_ITEMS};
    }
    if (obj.values.size() > StatsRequest::field_constraints::VALUES_MAX_ITEMS) {
        return validation_error{"values", validation_error_code::array_too_large, StatsRequest::field_constraints::VALUES_MAX_ITEMS};
    }
    return std::nullopt;
}

// validate StatsRequest_Values_t — array, field StatsRequest.values  ← api.yaml:247
[[nodiscard]] inline std::optional<validation_error> validate_StatsRequest_Values_t(const StatsRequest_Values_t& arr) {
    if (arr.size() < 1) return validation_error{"", validation_error_code::array_too_small, 1};
    if (arr.size() > 10000) return validation_error{"", validation_error_code::array_too_large, 10000};
    return std::nullopt;
}

// validate StatsResponse — object, 6 field(s)  ← api.yaml:257
[[nodiscard]] inline std::optional<validation_error> validate_StatsResponse(const StatsResponse& obj) {
    (void)obj;
    return std::nullopt;
}

// validate RegisterRequest — object, 6 field(s)  ← api.yaml:279
[[nodiscard]] inline std::optional<validation_error> validate_RegisterRequest(const RegisterRequest& obj) {
    if (obj.username.empty()) {
        return validation_error{"username", validation_error_code::required_field_missing};
    }
    if (!obj.username.empty() && katana::utf8_length(obj.username) < RegisterRequest::field_constraints::USERNAME_MIN_LENGTH) {
        return validation_error{"username", validation_error_code::string_too_short, RegisterRequest::field_constraints::USERNAME_MIN_LENGTH};
    }
    if (katana::utf8_length(obj.username) > RegisterRequest::field_constraints::USERNAME_MAX_LENGTH) {
        return validation_error{"username", validation_error_code::string_too_long, RegisterRequest::field_constraints::USERNAME_MAX_LENGTH};
    }
    if (!obj.username.empty()) {
        const auto& pv_ = obj.username;
        bool pattern_ok_ = true;
        for (char ch_ : pv_) {
            if (!std::isalnum(static_cast<unsigned char>(ch_)) && ch_ != '_' && ch_ != '-') { pattern_ok_ = false; break; }
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
    if (!obj.password.empty() && katana::utf8_length(obj.password) < RegisterRequest::field_constraints::PASSWORD_MIN_LENGTH) {
        return validation_error{"password", validation_error_code::string_too_short, RegisterRequest::field_constraints::PASSWORD_MIN_LENGTH};
    }
    if (katana::utf8_length(obj.password) > RegisterRequest::field_constraints::PASSWORD_MAX_LENGTH) {
        return validation_error{"password", validation_error_code::string_too_long, RegisterRequest::field_constraints::PASSWORD_MAX_LENGTH};
    }
    if (obj.age && static_cast<double>((*obj.age)) < RegisterRequest::field_constraints::AGE_MINIMUM) {
        return validation_error{"age", validation_error_code::value_too_small, RegisterRequest::field_constraints::AGE_MINIMUM};
    }
    if (obj.age && static_cast<double>((*obj.age)) > RegisterRequest::field_constraints::AGE_MAXIMUM) {
        return validation_error{"age", validation_error_code::value_too_large, RegisterRequest::field_constraints::AGE_MAXIMUM};
    }
    if (obj.tags && obj.tags->size() > RegisterRequest::field_constraints::TAGS_MAX_ITEMS) {
        return validation_error{"tags", validation_error_code::array_too_large, RegisterRequest::field_constraints::TAGS_MAX_ITEMS};
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
    return std::nullopt;
}

// validate RegisterRequest_Tags_t — array, field RegisterRequest.tags  ← api.yaml:301
[[nodiscard]] inline std::optional<validation_error> validate_RegisterRequest_Tags_t(const RegisterRequest_Tags_t& arr) {
    if (arr.size() > 10) return validation_error{"", validation_error_code::array_too_large, 10};
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

// validate UserResponse — object, 5 field(s)  ← api.yaml:312
[[nodiscard]] inline std::optional<validation_error> validate_UserResponse(const UserResponse& obj) {
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

// validate Item — object, 8 field(s)  ← api.yaml:334
[[nodiscard]] inline std::optional<validation_error> validate_Item(const Item& obj) {
    if (obj.name.empty()) {
        return validation_error{"name", validation_error_code::required_field_missing};
    }
    if (obj.metadata) { if (auto e_ = validate_ItemMetadata(*obj.metadata)) return e_; }
    return std::nullopt;
}

// validate Item_Tags_t — array, field Item.tags  ← api.yaml:353
[[nodiscard]] inline std::optional<validation_error> validate_Item_Tags_t(const Item_Tags_t& arr) {
    (void)arr;
    return std::nullopt;
}

// validate ItemMetadata — object, 4 field(s)  ← api.yaml:360
[[nodiscard]] inline std::optional<validation_error> validate_ItemMetadata(const ItemMetadata& obj) {
    (void)obj;
    return std::nullopt;
}

// validate CreateItemRequest — object, 7 field(s)  ← api.yaml:373
[[nodiscard]] inline std::optional<validation_error> validate_CreateItemRequest(const CreateItemRequest& obj) {
    if (obj.name.empty()) {
        return validation_error{"name", validation_error_code::required_field_missing};
    }
    if (!obj.name.empty() && katana::utf8_length(obj.name) < CreateItemRequest::field_constraints::NAME_MIN_LENGTH) {
        return validation_error{"name", validation_error_code::string_too_short, CreateItemRequest::field_constraints::NAME_MIN_LENGTH};
    }
    if (katana::utf8_length(obj.name) > CreateItemRequest::field_constraints::NAME_MAX_LENGTH) {
        return validation_error{"name", validation_error_code::string_too_long, CreateItemRequest::field_constraints::NAME_MAX_LENGTH};
    }
    if (obj.description && katana::utf8_length((*obj.description)) > CreateItemRequest::field_constraints::DESCRIPTION_MAX_LENGTH) {
        return validation_error{"description", validation_error_code::string_too_long, CreateItemRequest::field_constraints::DESCRIPTION_MAX_LENGTH};
    }
    if (static_cast<double>(obj.price) < CreateItemRequest::field_constraints::PRICE_MINIMUM) {
        return validation_error{"price", validation_error_code::value_too_small, CreateItemRequest::field_constraints::PRICE_MINIMUM};
    }
    if (static_cast<double>(obj.price) >= CreateItemRequest::field_constraints::PRICE_EXCLUSIVE_MAXIMUM) {
        return validation_error{"price", validation_error_code::value_above_exclusive_maximum, CreateItemRequest::field_constraints::PRICE_EXCLUSIVE_MAXIMUM};
    }
    if (obj.stock && static_cast<double>((*obj.stock)) < CreateItemRequest::field_constraints::STOCK_MINIMUM) {
        return validation_error{"stock", validation_error_code::value_too_small, CreateItemRequest::field_constraints::STOCK_MINIMUM};
    }
    if (obj.stock && static_cast<double>((*obj.stock)) > CreateItemRequest::field_constraints::STOCK_MAXIMUM) {
        return validation_error{"stock", validation_error_code::value_too_large, CreateItemRequest::field_constraints::STOCK_MAXIMUM};
    }
    if (obj.tags && obj.tags->size() > CreateItemRequest::field_constraints::TAGS_MAX_ITEMS) {
        return validation_error{"tags", validation_error_code::array_too_large, CreateItemRequest::field_constraints::TAGS_MAX_ITEMS};
    }
    if (obj.metadata) { if (auto e_ = validate_ItemMetadata(*obj.metadata)) return e_; }
    return std::nullopt;
}

// validate CreateItemRequest_Tags_t — array, field CreateItemRequest.tags  ← api.yaml:396
[[nodiscard]] inline std::optional<validation_error> validate_CreateItemRequest_Tags_t(const CreateItemRequest_Tags_t& arr) {
    if (arr.size() > 20) return validation_error{"", validation_error_code::array_too_large, 20};
    return std::nullopt;
}

// validate UpdateItemRequest — object, 6 field(s)  ← api.yaml:405
[[nodiscard]] inline std::optional<validation_error> validate_UpdateItemRequest(const UpdateItemRequest& obj) {
    if (obj.name && !obj.name->empty() && katana::utf8_length((*obj.name)) < UpdateItemRequest::field_constraints::NAME_MIN_LENGTH) {
        return validation_error{"name", validation_error_code::string_too_short, UpdateItemRequest::field_constraints::NAME_MIN_LENGTH};
    }
    if (obj.name && katana::utf8_length((*obj.name)) > UpdateItemRequest::field_constraints::NAME_MAX_LENGTH) {
        return validation_error{"name", validation_error_code::string_too_long, UpdateItemRequest::field_constraints::NAME_MAX_LENGTH};
    }
    if (obj.description && katana::utf8_length((*obj.description)) > UpdateItemRequest::field_constraints::DESCRIPTION_MAX_LENGTH) {
        return validation_error{"description", validation_error_code::string_too_long, UpdateItemRequest::field_constraints::DESCRIPTION_MAX_LENGTH};
    }
    if (obj.price && static_cast<double>((*obj.price)) < UpdateItemRequest::field_constraints::PRICE_MINIMUM) {
        return validation_error{"price", validation_error_code::value_too_small, UpdateItemRequest::field_constraints::PRICE_MINIMUM};
    }
    if (obj.stock && static_cast<double>((*obj.stock)) < UpdateItemRequest::field_constraints::STOCK_MINIMUM) {
        return validation_error{"stock", validation_error_code::value_too_small, UpdateItemRequest::field_constraints::STOCK_MINIMUM};
    }
    if (obj.tags && obj.tags->size() > UpdateItemRequest::field_constraints::TAGS_MAX_ITEMS) {
        return validation_error{"tags", validation_error_code::array_too_large, UpdateItemRequest::field_constraints::TAGS_MAX_ITEMS};
    }
    return std::nullopt;
}

// validate UpdateItemRequest_Tags_t — array, field UpdateItemRequest.tags  ← api.yaml:425
[[nodiscard]] inline std::optional<validation_error> validate_UpdateItemRequest_Tags_t(const UpdateItemRequest_Tags_t& arr) {
    if (arr.size() > 20) return validation_error{"", validation_error_code::array_too_large, 20};
    return std::nullopt;
}

// validate ItemList — object, 4 field(s)  ← api.yaml:431
[[nodiscard]] inline std::optional<validation_error> validate_ItemList(const ItemList& obj) {
    (void)obj;
    for (const auto& it_ : obj.items) { if (auto e_ = validate_Item(it_)) return e_; }
    return std::nullopt;
}

// validate ItemList_Items_t — array, field ItemList.items  ← api.yaml:435
[[nodiscard]] inline std::optional<validation_error> validate_ItemList_Items_t(const ItemList_Items_t& arr) {
    (void)arr;
    return std::nullopt;
}

// validate EchoRequest — object, 3 field(s)  ← api.yaml:448
[[nodiscard]] inline std::optional<validation_error> validate_EchoRequest(const EchoRequest& obj) {
    if (obj.message.empty()) {
        return validation_error{"message", validation_error_code::required_field_missing};
    }
    if (katana::utf8_length(obj.message) > EchoRequest::field_constraints::MESSAGE_MAX_LENGTH) {
        return validation_error{"message", validation_error_code::string_too_long, EchoRequest::field_constraints::MESSAGE_MAX_LENGTH};
    }
    if (obj.repeat && static_cast<double>((*obj.repeat)) < EchoRequest::field_constraints::REPEAT_MINIMUM) {
        return validation_error{"repeat", validation_error_code::value_too_small, EchoRequest::field_constraints::REPEAT_MINIMUM};
    }
    if (obj.repeat && static_cast<double>((*obj.repeat)) > EchoRequest::field_constraints::REPEAT_MAXIMUM) {
        return validation_error{"repeat", validation_error_code::value_too_large, EchoRequest::field_constraints::REPEAT_MAXIMUM};
    }
    return std::nullopt;
}

// validate EchoResponse — object, 2 field(s)  ← api.yaml:462
[[nodiscard]] inline std::optional<validation_error> validate_EchoResponse(const EchoResponse& obj) {
    if (obj.message.empty()) {
        return validation_error{"message", validation_error_code::required_field_missing};
    }
    return std::nullopt;
}

// validate health_check_response — object, 2 field(s)  ← api.yaml:209
[[nodiscard]] inline std::optional<validation_error> validate_health_check_response(const health_check_response& obj) {
    (void)obj;
    return std::nullopt;
}

