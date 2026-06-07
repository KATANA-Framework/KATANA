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

[[nodiscard]] inline std::optional<validation_error> validate_CustomerRevenue(const CustomerRevenue&);
[[nodiscard]] inline std::optional<validation_error> validate_CustomerRevenueList(const CustomerRevenueList&);
[[nodiscard]] inline std::optional<validation_error> validate_CustomerRevenueList_Items_t(const CustomerRevenueList_Items_t&);
[[nodiscard]] inline std::optional<validation_error> validate_TopProduct(const TopProduct&);
[[nodiscard]] inline std::optional<validation_error> validate_TopProductList(const TopProductList&);
[[nodiscard]] inline std::optional<validation_error> validate_TopProductList_Items_t(const TopProductList_Items_t&);
[[nodiscard]] inline std::optional<validation_error> validate_CategoryStat(const CategoryStat&);
[[nodiscard]] inline std::optional<validation_error> validate_CategoryStatList(const CategoryStatList&);
[[nodiscard]] inline std::optional<validation_error> validate_CategoryStatList_Items_t(const CategoryStatList_Items_t&);
[[nodiscard]] inline std::optional<validation_error> validate_OrderDetail(const OrderDetail&);
[[nodiscard]] inline std::optional<validation_error> validate_CreateOrderRequest(const CreateOrderRequest&);
[[nodiscard]] inline std::optional<validation_error> validate_CreateOrderResponse(const CreateOrderResponse&);

// validate CustomerRevenue — object, 6 field(s)  ← api.yaml:94
[[nodiscard]] inline std::optional<validation_error> validate_CustomerRevenue(const CustomerRevenue& obj) {
    if (obj.name.empty()) {
        return validation_error{"name", validation_error_code::required_field_missing};
    }
    if (obj.country.empty()) {
        return validation_error{"country", validation_error_code::required_field_missing};
    }
    return std::nullopt;
}

// validate CustomerRevenueList — object, 1 field(s)  ← api.yaml:104
[[nodiscard]] inline std::optional<validation_error> validate_CustomerRevenueList(const CustomerRevenueList& obj) {
    (void)obj;
    for (const auto& it_ : obj.items) { if (auto e_ = validate_CustomerRevenue(it_)) return e_; }
    return std::nullopt;
}

// validate CustomerRevenueList_Items_t — array, field CustomerRevenueList.items  ← api.yaml:108
[[nodiscard]] inline std::optional<validation_error> validate_CustomerRevenueList_Items_t(const CustomerRevenueList_Items_t& arr) {
    (void)arr;
    return std::nullopt;
}

// validate TopProduct — object, 6 field(s)  ← api.yaml:109
[[nodiscard]] inline std::optional<validation_error> validate_TopProduct(const TopProduct& obj) {
    if (obj.name.empty()) {
        return validation_error{"name", validation_error_code::required_field_missing};
    }
    if (obj.category.empty()) {
        return validation_error{"category", validation_error_code::required_field_missing};
    }
    return std::nullopt;
}

// validate TopProductList — object, 1 field(s)  ← api.yaml:119
[[nodiscard]] inline std::optional<validation_error> validate_TopProductList(const TopProductList& obj) {
    (void)obj;
    for (const auto& it_ : obj.items) { if (auto e_ = validate_TopProduct(it_)) return e_; }
    return std::nullopt;
}

// validate TopProductList_Items_t — array, field TopProductList.items  ← api.yaml:123
[[nodiscard]] inline std::optional<validation_error> validate_TopProductList_Items_t(const TopProductList_Items_t& arr) {
    (void)arr;
    return std::nullopt;
}

// validate CategoryStat — object, 6 field(s)  ← api.yaml:124
[[nodiscard]] inline std::optional<validation_error> validate_CategoryStat(const CategoryStat& obj) {
    if (obj.category.empty()) {
        return validation_error{"category", validation_error_code::required_field_missing};
    }
    return std::nullopt;
}

// validate CategoryStatList — object, 1 field(s)  ← api.yaml:134
[[nodiscard]] inline std::optional<validation_error> validate_CategoryStatList(const CategoryStatList& obj) {
    (void)obj;
    for (const auto& it_ : obj.items) { if (auto e_ = validate_CategoryStat(it_)) return e_; }
    return std::nullopt;
}

// validate CategoryStatList_Items_t — array, field CategoryStatList.items  ← api.yaml:138
[[nodiscard]] inline std::optional<validation_error> validate_CategoryStatList_Items_t(const CategoryStatList_Items_t& arr) {
    (void)arr;
    return std::nullopt;
}

// validate OrderDetail — object, 7 field(s)  ← api.yaml:139
[[nodiscard]] inline std::optional<validation_error> validate_OrderDetail(const OrderDetail& obj) {
    if (obj.status.empty()) {
        return validation_error{"status", validation_error_code::required_field_missing};
    }
    if (obj.customer_name.empty()) {
        return validation_error{"customer_name", validation_error_code::required_field_missing};
    }
    if (obj.country.empty()) {
        return validation_error{"country", validation_error_code::required_field_missing};
    }
    return std::nullopt;
}

// validate CreateOrderRequest — object, 2 field(s)  ← api.yaml:150
[[nodiscard]] inline std::optional<validation_error> validate_CreateOrderRequest(const CreateOrderRequest& obj) {
    if (static_cast<double>(obj.customer_id) < CreateOrderRequest::field_constraints::CUSTOMER_ID_MINIMUM) {
        return validation_error{"customer_id", validation_error_code::value_too_small, CreateOrderRequest::field_constraints::CUSTOMER_ID_MINIMUM};
    }
    if (obj.status.empty()) {
        return validation_error{"status", validation_error_code::required_field_missing};
    }
    if (!obj.status.empty() && katana::utf8_length(obj.status) < CreateOrderRequest::field_constraints::STATUS_MIN_LENGTH) {
        return validation_error{"status", validation_error_code::string_too_short, CreateOrderRequest::field_constraints::STATUS_MIN_LENGTH};
    }
    if (katana::utf8_length(obj.status) > CreateOrderRequest::field_constraints::STATUS_MAX_LENGTH) {
        return validation_error{"status", validation_error_code::string_too_long, CreateOrderRequest::field_constraints::STATUS_MAX_LENGTH};
    }
    return std::nullopt;
}

// validate CreateOrderResponse — object, 1 field(s)  ← api.yaml:156
[[nodiscard]] inline std::optional<validation_error> validate_CreateOrderResponse(const CreateOrderResponse& obj) {
    (void)obj;
    return std::nullopt;
}

