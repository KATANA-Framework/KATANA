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

[[nodiscard]] inline std::optional<validation_error> validate_CreateOrderRequest(const CreateOrderRequest&);

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

