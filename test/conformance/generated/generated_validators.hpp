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

[[nodiscard]] inline std::optional<validation_error> validate_PetCreateRequest(const PetCreateRequest&);

// validate PetCreateRequest — object, 3 field(s)
[[nodiscard]] inline std::optional<validation_error> validate_PetCreateRequest(const PetCreateRequest& obj) {
    if (obj.name.empty()) {
        return validation_error{"name", validation_error_code::required_field_missing};
    }
    if (!obj.name.empty() && katana::utf8_length(obj.name) < PetCreateRequest::field_constraints::NAME_MIN_LENGTH) {
        return validation_error{"name", validation_error_code::string_too_short, PetCreateRequest::field_constraints::NAME_MIN_LENGTH};
    }
    if (static_cast<double>(obj.age) < PetCreateRequest::field_constraints::AGE_MINIMUM) {
        return validation_error{"age", validation_error_code::value_too_small, PetCreateRequest::field_constraints::AGE_MINIMUM};
    }
    if (static_cast<double>(obj.age) > PetCreateRequest::field_constraints::AGE_MAXIMUM) {
        return validation_error{"age", validation_error_code::value_too_large, PetCreateRequest::field_constraints::AGE_MAXIMUM};
    }
    if (obj.ownerEmail.empty()) {
        return validation_error{"ownerEmail", validation_error_code::required_field_missing};
    }
    if (!obj.ownerEmail.empty() && !is_valid_email(obj.ownerEmail)) {
        return validation_error{"ownerEmail", validation_error_code::invalid_email_format};
    }
    return std::nullopt;
}

