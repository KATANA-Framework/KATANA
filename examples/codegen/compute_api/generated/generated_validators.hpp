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

[[nodiscard]] inline std::optional<validation_error>
validate_compute_sum_request(const compute_sum_request& arr) {
    if (arr.size() < 1)
        return validation_error{"", validation_error_code::array_too_small, 1};
    if (arr.size() > 1024)
        return validation_error{"", validation_error_code::array_too_large, 1024};
    return std::nullopt;
}
