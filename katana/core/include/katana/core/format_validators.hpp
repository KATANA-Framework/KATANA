#pragma once

#include <cctype>
#include <string_view>

namespace katana::format_validators {

inline bool is_valid_email(std::string_view v) noexcept {
    auto at = v.find('@');
    if (at == std::string_view::npos || at == 0 || at + 1 >= v.size())
        return false;
    auto domain = v.substr(at + 1);
    auto dot = domain.find('.');
    if (dot == std::string_view::npos || dot == 0 || dot + 1 >= domain.size())
        return false;
    return true;
}

inline bool is_valid_uuid(std::string_view v) noexcept {
    if (v.size() != 36)
        return false;
    auto is_hex = [](char c) noexcept { return std::isxdigit(static_cast<unsigned char>(c)) != 0; };
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

inline bool is_valid_datetime(std::string_view v) noexcept {
    auto is_digit = [](char c) noexcept {
        return std::isdigit(static_cast<unsigned char>(c)) != 0;
    };
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

} // namespace katana::format_validators
