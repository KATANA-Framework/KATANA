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

[[nodiscard]] inline std::optional<validation_error> validate_CustomerRevenue(const CustomerRevenue& obj) {
    if (obj.name.empty()) {
        return validation_error{"name", validation_error_code::required_field_missing};
    }
    if (obj.country.empty()) {
        return validation_error{"country", validation_error_code::required_field_missing};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_CustomerRevenueList(const CustomerRevenueList& obj) {
    (void)obj;
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_CustomerRevenueList_Items_t(const CustomerRevenueList_Items_t& arr) {
    (void)arr;
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_TopProduct(const TopProduct& obj) {
    if (obj.name.empty()) {
        return validation_error{"name", validation_error_code::required_field_missing};
    }
    if (obj.category.empty()) {
        return validation_error{"category", validation_error_code::required_field_missing};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_TopProductList(const TopProductList& obj) {
    (void)obj;
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_TopProductList_Items_t(const TopProductList_Items_t& arr) {
    (void)arr;
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_CategoryStat(const CategoryStat& obj) {
    if (obj.category.empty()) {
        return validation_error{"category", validation_error_code::required_field_missing};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_CategoryStatList(const CategoryStatList& obj) {
    (void)obj;
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_CategoryStatList_Items_t(const CategoryStatList_Items_t& arr) {
    (void)arr;
    return std::nullopt;
}

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

[[nodiscard]] inline std::optional<validation_error> validate_CreateOrderRequest(const CreateOrderRequest& obj) {
    if (static_cast<double>(obj.customer_id) < CreateOrderRequest::metadata::CUSTOMER_ID_MINIMUM) {
        return validation_error{"customer_id", validation_error_code::value_too_small, CreateOrderRequest::metadata::CUSTOMER_ID_MINIMUM};
    }
    if (obj.status.empty()) {
        return validation_error{"status", validation_error_code::required_field_missing};
    }
    if (!obj.status.empty() && obj.status.size() < CreateOrderRequest::metadata::STATUS_MIN_LENGTH) {
        return validation_error{"status", validation_error_code::string_too_short, CreateOrderRequest::metadata::STATUS_MIN_LENGTH};
    }
    if (obj.status.size() > CreateOrderRequest::metadata::STATUS_MAX_LENGTH) {
        return validation_error{"status", validation_error_code::string_too_long, CreateOrderRequest::metadata::STATUS_MAX_LENGTH};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<validation_error> validate_CreateOrderResponse(const CreateOrderResponse& obj) {
    (void)obj;
    return std::nullopt;
}

