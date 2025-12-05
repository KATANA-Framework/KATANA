#pragma once

#include <cstdint>
#include <string_view>

namespace katana {

// Unified validation error codes used by generated validators and runtime JSON parsing.
enum class validation_error_code : uint8_t {
    required_field_missing,
    invalid_type,
    string_too_short,
    string_too_long,
    invalid_email_format,
    invalid_uuid_format,
    invalid_datetime_format,
    invalid_enum_value,
    pattern_mismatch,
    value_too_small,
    value_too_large,
    value_below_exclusive_minimum,
    value_above_exclusive_maximum,
    value_not_multiple_of,
    array_too_small,
    array_too_large,
    array_items_not_unique,
};

inline constexpr std::string_view validation_error_message(validation_error_code code) noexcept {
    switch (code) {
    case validation_error_code::required_field_missing:
        return "required field is missing";
    case validation_error_code::invalid_type:
        return "invalid type";
    case validation_error_code::string_too_short:
        return "string too short";
    case validation_error_code::string_too_long:
        return "string too long";
    case validation_error_code::invalid_email_format:
        return "invalid email format";
    case validation_error_code::invalid_uuid_format:
        return "invalid uuid format";
    case validation_error_code::invalid_datetime_format:
        return "invalid date-time format";
    case validation_error_code::invalid_enum_value:
        return "invalid enum value";
    case validation_error_code::pattern_mismatch:
        return "pattern mismatch";
    case validation_error_code::value_too_small:
        return "value too small";
    case validation_error_code::value_too_large:
        return "value too large";
    case validation_error_code::value_below_exclusive_minimum:
        return "value must be greater than minimum";
    case validation_error_code::value_above_exclusive_maximum:
        return "value must be less than maximum";
    case validation_error_code::value_not_multiple_of:
        return "value must be multiple of";
    case validation_error_code::array_too_small:
        return "array too small";
    case validation_error_code::array_too_large:
        return "array too large";
    case validation_error_code::array_items_not_unique:
        return "array items must be unique";
    }
    return "unknown error";
}

struct validation_error {
    std::string_view field;        // Field that failed validation
    validation_error_code code;    // Error code (type-safe)
    double constraint_value = 0.0; // Optional: constraint value for context (min/max/etc)

    [[nodiscard]] constexpr std::string_view message() const noexcept {
        return validation_error_message(code);
    }
};

} // namespace katana
