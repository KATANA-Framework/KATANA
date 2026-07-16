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

[[nodiscard]] inline std::optional<validation_error> validate_UserInput(const UserInput&);

// validate UserInput — object, 3 field(s)  ← test_api.yaml:52
[[nodiscard]] inline std::optional<validation_error> validate_UserInput(const UserInput& obj) {
    if (obj.name.empty()) {
        return validation_error{"name", validation_error_code::required_field_missing};
    }
    if (!obj.name.empty() && katana::utf8_length(obj.name) < UserInput::field_constraints::NAME_MIN_LENGTH) {
        return validation_error{"name", validation_error_code::string_too_short, UserInput::field_constraints::NAME_MIN_LENGTH};
    }
    if (obj.email.empty()) {
        return validation_error{"email", validation_error_code::required_field_missing};
    }
    if (!obj.email.empty() && !is_valid_email(obj.email)) {
        return validation_error{"email", validation_error_code::invalid_email_format};
    }
    if (obj.age && static_cast<double>((*obj.age)) < UserInput::field_constraints::AGE_MINIMUM) {
        return validation_error{"age", validation_error_code::value_too_small, UserInput::field_constraints::AGE_MINIMUM};
    }
    return std::nullopt;
}

