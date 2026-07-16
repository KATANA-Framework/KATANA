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

[[nodiscard]] inline std::optional<validation_error> validate_text_uppercase_request(const text_uppercase_request&);
[[nodiscard]] inline std::optional<validation_error> validate_text_lowercase_request(const text_lowercase_request&);
[[nodiscard]] inline std::optional<validation_error> validate_text_reverse_request(const text_reverse_request&);
[[nodiscard]] inline std::optional<validation_error> validate_text_stats_request(const text_stats_request&);
[[nodiscard]] inline std::optional<validation_error> validate_text_transform_request(const text_transform_request&);

// validate text_uppercase_request — object, 1 field(s)  ← api.yaml:18
[[nodiscard]] inline std::optional<validation_error> validate_text_uppercase_request(const text_uppercase_request& obj) {
    if (obj.text.empty()) {
        return validation_error{"text", validation_error_code::required_field_missing};
    }
    if (!obj.text.empty() && katana::utf8_length(obj.text) < text_uppercase_request::field_constraints::TEXT_MIN_LENGTH) {
        return validation_error{"text", validation_error_code::string_too_short, text_uppercase_request::field_constraints::TEXT_MIN_LENGTH};
    }
    if (katana::utf8_length(obj.text) > text_uppercase_request::field_constraints::TEXT_MAX_LENGTH) {
        return validation_error{"text", validation_error_code::string_too_long, text_uppercase_request::field_constraints::TEXT_MAX_LENGTH};
    }
    return std::nullopt;
}

// validate text_lowercase_request — object, 1 field(s)  ← api.yaml:45
[[nodiscard]] inline std::optional<validation_error> validate_text_lowercase_request(const text_lowercase_request& obj) {
    if (obj.text.empty()) {
        return validation_error{"text", validation_error_code::required_field_missing};
    }
    if (!obj.text.empty() && katana::utf8_length(obj.text) < text_lowercase_request::field_constraints::TEXT_MIN_LENGTH) {
        return validation_error{"text", validation_error_code::string_too_short, text_lowercase_request::field_constraints::TEXT_MIN_LENGTH};
    }
    if (katana::utf8_length(obj.text) > text_lowercase_request::field_constraints::TEXT_MAX_LENGTH) {
        return validation_error{"text", validation_error_code::string_too_long, text_lowercase_request::field_constraints::TEXT_MAX_LENGTH};
    }
    return std::nullopt;
}

// validate text_reverse_request — object, 1 field(s)  ← api.yaml:72
[[nodiscard]] inline std::optional<validation_error> validate_text_reverse_request(const text_reverse_request& obj) {
    if (obj.text.empty()) {
        return validation_error{"text", validation_error_code::required_field_missing};
    }
    if (!obj.text.empty() && katana::utf8_length(obj.text) < text_reverse_request::field_constraints::TEXT_MIN_LENGTH) {
        return validation_error{"text", validation_error_code::string_too_short, text_reverse_request::field_constraints::TEXT_MIN_LENGTH};
    }
    if (katana::utf8_length(obj.text) > text_reverse_request::field_constraints::TEXT_MAX_LENGTH) {
        return validation_error{"text", validation_error_code::string_too_long, text_reverse_request::field_constraints::TEXT_MAX_LENGTH};
    }
    return std::nullopt;
}

// validate text_stats_request — object, 1 field(s)  ← api.yaml:100
[[nodiscard]] inline std::optional<validation_error> validate_text_stats_request(const text_stats_request& obj) {
    if (obj.text.empty()) {
        return validation_error{"text", validation_error_code::required_field_missing};
    }
    if (katana::utf8_length(obj.text) > text_stats_request::field_constraints::TEXT_MAX_LENGTH) {
        return validation_error{"text", validation_error_code::string_too_long, text_stats_request::field_constraints::TEXT_MAX_LENGTH};
    }
    return std::nullopt;
}

// validate text_transform_request — object, 3 field(s)  ← api.yaml:134
[[nodiscard]] inline std::optional<validation_error> validate_text_transform_request(const text_transform_request& obj) {
    if (obj.text.empty()) {
        return validation_error{"text", validation_error_code::required_field_missing};
    }
    if (!obj.text.empty() && katana::utf8_length(obj.text) < text_transform_request::field_constraints::TEXT_MIN_LENGTH) {
        return validation_error{"text", validation_error_code::string_too_short, text_transform_request::field_constraints::TEXT_MIN_LENGTH};
    }
    if (katana::utf8_length(obj.text) > text_transform_request::field_constraints::TEXT_MAX_LENGTH) {
        return validation_error{"text", validation_error_code::string_too_long, text_transform_request::field_constraints::TEXT_MAX_LENGTH};
    }
    return std::nullopt;
}

