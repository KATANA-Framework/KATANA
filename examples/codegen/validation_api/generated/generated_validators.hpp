// layer: flat
#pragma once

#include "generated_dtos.hpp"
#include "katana/core/validation.hpp"
#include <cctype>
#include <cmath>
#include <optional>
#include <string>
#include <string_view>

#include <regex>
#include <unordered_set>

using katana::validation_error;
using katana::validation_error_code;

inline constexpr std::string_view to_string(validation_error_code code) noexcept {
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

inline bool is_valid_email(std::string_view v) {
    auto at = v.find('@');
    if (at == std::string_view::npos || at == 0 || at + 1 >= v.size())
        return false;
    auto domain = v.substr(at + 1);
    auto dot = domain.find('.');
    if (dot == std::string_view::npos || dot == 0 || dot + 1 >= domain.size())
        return false;
    return true;
}

inline bool is_valid_uuid(std::string_view v) {
    if (v.size() != 36)
        return false;
    auto is_hex = [](char c) { return std::isxdigit(static_cast<unsigned char>(c)) != 0; };
    for (size_t i = 0; i < v.size(); ++i) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            if (v[i] != '-')
                return false;
        } else if (!is_hex(v[i])) {
            return false;
        }
    }
    return true;
}

inline bool is_valid_datetime(std::string_view v) {
    auto is_digit = [](char c) { return std::isdigit(static_cast<unsigned char>(c)) != 0; };
    if (v.size() < 20)
        return false;
    for (size_t i : {0u, 1u, 2u, 3u, 5u, 6u, 8u, 9u, 11u, 12u, 14u, 15u, 17u, 18u}) {
        if (!is_digit(v[i]))
            return false;
    }
    if (v[4] != '-' || v[7] != '-' || v[10] != 'T' || v[13] != ':' || v[16] != ':')
        return false;
    size_t pos = 19;
    if (pos < v.size() && v[pos] == '.') {
        ++pos;
        if (pos >= v.size())
            return false;
        while (pos < v.size() && is_digit(v[pos]))
            ++pos;
    }
    if (pos >= v.size())
        return false;
    if (v[pos] == 'Z')
        return pos + 1 == v.size();
    if (v[pos] == '+' || v[pos] == '-') {
        if (pos + 5 >= v.size())
            return false;
        if (!is_digit(v[pos + 1]) || !is_digit(v[pos + 2]))
            return false;
        if (v[pos + 3] != ':')
            return false;
        if (!is_digit(v[pos + 4]) || !is_digit(v[pos + 5]))
            return false;
        return pos + 6 == v.size();
    }
    return false;
}

inline std::optional<validation_error>
validate_RegisterUserRequest(const RegisterUserRequest& obj) {
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
        obj.password.size() < RegisterUserRequest::metadata::PASSWORD_MIN_LENGTH) {
        return validation_error{"password",
                                validation_error_code::string_too_short,
                                RegisterUserRequest::metadata::PASSWORD_MIN_LENGTH};
    }
    if (obj.password.size() > RegisterUserRequest::metadata::PASSWORD_MAX_LENGTH) {
        return validation_error{"password",
                                validation_error_code::string_too_long,
                                RegisterUserRequest::metadata::PASSWORD_MAX_LENGTH};
    }
    if (obj.age && static_cast<double>(*obj.age) < RegisterUserRequest::metadata::AGE_MINIMUM) {
        return validation_error{"age",
                                validation_error_code::value_too_small,
                                RegisterUserRequest::metadata::AGE_MINIMUM};
    }
    if (obj.age && static_cast<double>(*obj.age) > RegisterUserRequest::metadata::AGE_MAXIMUM) {
        return validation_error{"age",
                                validation_error_code::value_too_large,
                                RegisterUserRequest::metadata::AGE_MAXIMUM};
    }
    return std::nullopt;
}
