#pragma once

// Optional, non-breaking date/time helpers. Date-time fields stay plain strings on the DTO
// (so the wire contract is unchanged); a handler that needs a typed value calls these on the
// string. Parses the RFC 3339 / ISO 8601 subset OpenAPI's `format: date-time` and `date` use.

#include <charconv>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string_view>

namespace katana {

namespace detail {

[[nodiscard]] inline bool parse_uint(std::string_view s, int& out) noexcept {
    if (s.empty()) {
        return false;
    }
    int value = 0;
    const auto* begin = s.data();
    const auto* end = s.data() + s.size();
    const auto res = std::from_chars(begin, end, value);
    if (res.ec != std::errc() || res.ptr != end) {
        return false;
    }
    out = value;
    return true;
}

// Days from civil 1970-01-01 to y-m-d (Howard Hinnant's algorithm). Valid for any proleptic
// Gregorian date; no time-zone or leap-second handling beyond what the offset provides.
[[nodiscard]] inline int64_t days_from_civil(int y, unsigned m, unsigned d) noexcept {
    y -= m <= 2;
    const int64_t era = (y >= 0 ? y : y - 399) / 400;
    const auto yoe = static_cast<unsigned>(y - era * 400);
    const unsigned doy = (153U * (m > 2 ? m - 3 : m + 9) + 2U) / 5U + d - 1U;
    const unsigned doe = yoe * 365U + yoe / 4U - yoe / 100U + doy;
    return era * 146097LL + static_cast<int64_t>(doe) - 719468LL;
}

} // namespace detail

// Parse a `date` (YYYY-MM-DD) into a time_point at 00:00:00 UTC. Returns nullopt on malformed
// input. Does not validate calendar limits beyond field ranges.
[[nodiscard]] inline std::optional<std::chrono::system_clock::time_point>
parse_date(std::string_view s) noexcept {
    if (s.size() != 10 || s[4] != '-' || s[7] != '-') {
        return std::nullopt;
    }
    int y = 0;
    int mo = 0;
    int d = 0;
    if (!detail::parse_uint(s.substr(0, 4), y) || !detail::parse_uint(s.substr(5, 2), mo) ||
        !detail::parse_uint(s.substr(8, 2), d)) {
        return std::nullopt;
    }
    if (mo < 1 || mo > 12 || d < 1 || d > 31) {
        return std::nullopt;
    }
    const int64_t days = detail::days_from_civil(y, static_cast<unsigned>(mo), static_cast<unsigned>(d));
    return std::chrono::system_clock::time_point{std::chrono::seconds{days * 86400LL}};
}

// Parse a `date-time` (RFC 3339, e.g. 2026-06-07T13:45:00Z or 2026-06-07T13:45:00.123+02:00)
// into a UTC time_point. Fractional seconds are accepted and dropped; the offset is applied.
// Returns nullopt on malformed input.
[[nodiscard]] inline std::optional<std::chrono::system_clock::time_point>
parse_rfc3339(std::string_view s) noexcept {
    if (s.size() < 19 || (s[10] != 'T' && s[10] != 't' && s[10] != ' ')) {
        return std::nullopt;
    }
    int y = 0;
    int mo = 0;
    int d = 0;
    int hh = 0;
    int mm = 0;
    int ss = 0;
    if (s[4] != '-' || s[7] != '-' || s[13] != ':' || s[16] != ':') {
        return std::nullopt;
    }
    if (!detail::parse_uint(s.substr(0, 4), y) || !detail::parse_uint(s.substr(5, 2), mo) ||
        !detail::parse_uint(s.substr(8, 2), d) || !detail::parse_uint(s.substr(11, 2), hh) ||
        !detail::parse_uint(s.substr(14, 2), mm) || !detail::parse_uint(s.substr(17, 2), ss)) {
        return std::nullopt;
    }
    if (mo < 1 || mo > 12 || d < 1 || d > 31 || hh > 23 || mm > 59 || ss > 60) {
        return std::nullopt;
    }

    std::string_view rest = s.substr(19);
    // Optional fractional seconds: .digits
    if (!rest.empty() && rest.front() == '.') {
        size_t i = 1;
        while (i < rest.size() && rest[i] >= '0' && rest[i] <= '9') {
            ++i;
        }
        if (i == 1) {
            return std::nullopt; // '.' with no digits
        }
        rest.remove_prefix(i);
    }

    int offset_seconds = 0;
    if (rest.empty()) {
        return std::nullopt; // RFC 3339 requires an explicit offset or 'Z'
    } else if (rest == "Z" || rest == "z") {
        offset_seconds = 0;
    } else if ((rest.front() == '+' || rest.front() == '-') && rest.size() == 6 && rest[3] == ':') {
        int oh = 0;
        int om = 0;
        if (!detail::parse_uint(rest.substr(1, 2), oh) || !detail::parse_uint(rest.substr(4, 2), om) ||
            oh > 23 || om > 59) {
            return std::nullopt;
        }
        offset_seconds = (oh * 3600 + om * 60) * (rest.front() == '-' ? -1 : 1);
    } else {
        return std::nullopt;
    }

    const int64_t days = detail::days_from_civil(y, static_cast<unsigned>(mo), static_cast<unsigned>(d));
    const int64_t secs = days * 86400LL + hh * 3600LL + mm * 60LL + ss - offset_seconds;
    return std::chrono::system_clock::time_point{std::chrono::seconds{secs}};
}

} // namespace katana
