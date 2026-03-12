#pragma once

#include "katana/core/http.hpp"
#include "katana/core/problem.hpp"
#include "katana/core/serde.hpp"
#include "katana/core/validation.hpp"

#include <array>
#include <cctype>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace katana::http_utils {

namespace detail {

[[nodiscard]] inline bool ascii_iequals(std::string_view lhs, std::string_view rhs) noexcept {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (size_t i = 0; i < lhs.size(); ++i) {
        unsigned char lc = static_cast<unsigned char>(lhs[i]);
        unsigned char rc = static_cast<unsigned char>(rhs[i]);
        if (std::tolower(lc) != std::tolower(rc)) {
            return false;
        }
    }
    return true;
}

[[nodiscard]] inline std::string_view media_type_token(std::string_view value) noexcept {
    value = katana::serde::trim_view(value);
    auto semicolon = value.find(';');
    if (semicolon != std::string_view::npos) {
        value = value.substr(0, semicolon);
    }
    return katana::serde::trim_view(value);
}

} // namespace detail

struct content_type_info {
    std::string_view mime_type;
};

struct named_param_target {
    std::string_view name;
    std::optional<std::string_view>* value;
};

inline std::optional<std::string_view> query_param(std::string_view uri,
                                                   std::string_view key) noexcept {
    auto qpos = uri.find('?');
    if (qpos == std::string_view::npos)
        return std::nullopt;
    auto query = uri.substr(qpos + 1);
    while (!query.empty()) {
        auto amp = query.find('&');
        auto part = query.substr(0, amp);
        auto eq = part.find('=');
        auto name = part.substr(0, eq);
        if (name == key) {
            if (eq == std::string_view::npos)
                return std::string_view{};
            return part.substr(eq + 1);
        }
        if (amp == std::string_view::npos)
            break;
        query.remove_prefix(amp + 1);
    }
    return std::nullopt;
}

template <size_t N>
inline void extract_query_params(std::string_view uri,
                                 const std::array<named_param_target, N>& targets) noexcept {
    if constexpr (N == 0) {
        return;
    }

    size_t remaining = 0;
    for (const auto& target : targets) {
        if (target.value != nullptr) {
            *target.value = std::nullopt;
            ++remaining;
        }
    }
    if (remaining == 0) {
        return;
    }

    auto qpos = uri.find('?');
    if (qpos == std::string_view::npos) {
        return;
    }

    auto query = uri.substr(qpos + 1);
    while (!query.empty() && remaining != 0) {
        auto amp = query.find('&');
        auto part = query.substr(0, amp);
        auto eq = part.find('=');
        auto name = part.substr(0, eq);
        auto value = eq == std::string_view::npos ? std::string_view{} : part.substr(eq + 1);

        for (const auto& target : targets) {
            if (target.value == nullptr || target.value->has_value()) {
                continue;
            }
            if (target.name == name) {
                *target.value = value;
                --remaining;
                break;
            }
        }

        if (amp == std::string_view::npos) {
            break;
        }
        query.remove_prefix(amp + 1);
    }
}

inline std::optional<std::string_view> cookie_param(const katana::http::request& req,
                                                    std::string_view key) noexcept {
    auto cookie = req.headers.get(katana::http::field::cookie);
    if (!cookie)
        return std::nullopt;
    std::string_view rest = *cookie;
    while (!rest.empty()) {
        auto sep = rest.find(';');
        auto token = rest.substr(0, sep);
        if (sep != std::string_view::npos)
            rest.remove_prefix(sep + 1);
        auto eq = token.find('=');
        if (eq == std::string_view::npos) {
            if (sep == std::string_view::npos)
                break;
            continue;
        }
        auto name = katana::serde::trim_view(token.substr(0, eq));
        auto val = katana::serde::trim_view(token.substr(eq + 1));
        if (name == key)
            return val;
        if (sep == std::string_view::npos)
            break;
    }
    return std::nullopt;
}

template <size_t N>
inline void extract_cookie_params(const katana::http::request& req,
                                  const std::array<named_param_target, N>& targets) noexcept {
    if constexpr (N == 0) {
        return;
    }

    size_t remaining = 0;
    for (const auto& target : targets) {
        if (target.value != nullptr) {
            *target.value = std::nullopt;
            ++remaining;
        }
    }
    if (remaining == 0) {
        return;
    }

    auto cookie = req.headers.get(katana::http::field::cookie);
    if (!cookie) {
        return;
    }

    std::string_view rest = *cookie;
    while (!rest.empty() && remaining != 0) {
        auto sep = rest.find(';');
        auto token = rest.substr(0, sep);
        auto eq = token.find('=');

        if (eq != std::string_view::npos) {
            auto name = katana::serde::trim_view(token.substr(0, eq));
            auto value = katana::serde::trim_view(token.substr(eq + 1));

            for (const auto& target : targets) {
                if (target.value == nullptr || target.value->has_value()) {
                    continue;
                }
                if (target.name == name) {
                    *target.value = value;
                    --remaining;
                    break;
                }
            }
        }

        if (sep == std::string_view::npos) {
            break;
        }
        rest.remove_prefix(sep + 1);
    }
}

inline std::optional<size_t>
find_content_type(std::optional<std::string_view> header,
                  std::span<const content_type_info> allowed) noexcept {
    if (allowed.empty())
        return std::nullopt;
    if (!header)
        return std::nullopt;
    auto requested = detail::media_type_token(*header);
    if (requested.empty()) {
        return std::nullopt;
    }
    for (size_t i = 0; i < allowed.size(); ++i) {
        auto& ct = allowed[i];
        if (detail::ascii_iequals(requested, ct.mime_type))
            return i;
    }
    return std::nullopt;
}

inline std::optional<std::string_view>
negotiate_response_type(const katana::http::request& req,
                        std::span<const content_type_info> produces) noexcept {
    if (produces.empty())
        return std::nullopt;
    auto accept = req.headers.get(katana::http::field::accept);
    // Fast path: no Accept header or */*, return first
    if (!accept || accept->empty() || *accept == "*/*") {
        return produces.front().mime_type;
    }
    // Fast path: exact match with first content type (common case)
    if (produces.size() == 1 && *accept == produces.front().mime_type) {
        return produces.front().mime_type;
    }
    // Fast path: common exact matches without quality values
    if (accept->find(',') == std::string_view::npos &&
        accept->find(';') == std::string_view::npos) {
        for (auto& ct : produces) {
            if (ct.mime_type == *accept)
                return ct.mime_type;
        }
    }
    // Slow path: full parsing with quality values and wildcards
    std::string_view remaining = *accept;
    while (!remaining.empty()) {
        auto comma = remaining.find(',');
        auto token = comma == std::string_view::npos ? remaining : remaining.substr(0, comma);
        if (comma == std::string_view::npos)
            remaining = {};
        else
            remaining = remaining.substr(comma + 1);
        token = katana::serde::trim_view(token);
        if (token.empty())
            continue;
        auto semicolon = token.find(';');
        if (semicolon != std::string_view::npos)
            token = katana::serde::trim_view(token.substr(0, semicolon));
        if (token == "*/*")
            return produces.front().mime_type;
        if (token.size() > 2 && token.substr(token.size() - 2) == "/*") {
            auto prefix = token.substr(0, token.size() - 1);
            for (auto& ct : produces) {
                if (ct.mime_type.starts_with(prefix)) {
                    return ct.mime_type;
                }
            }
        } else {
            for (auto& ct : produces) {
                if (ct.mime_type == token)
                    return ct.mime_type;
            }
        }
    }
    return std::nullopt;
}

inline katana::http::response format_validation_error(const katana::validation_error& err) {
    std::string error_msg;
    error_msg.reserve(err.field.size() + err.message().size() + 2);
    error_msg.append(err.field);
    error_msg.append(": ");
    error_msg.append(err.message());
    return katana::http::response::error(
        katana::problem_details::bad_request(std::move(error_msg)));
}

inline void format_validation_error_into(katana::http::response& out,
                                         const katana::validation_error& err) {
    std::string error_msg;
    error_msg.reserve(err.field.size() + err.message().size() + 2);
    error_msg.append(err.field);
    error_msg.append(": ");
    error_msg.append(err.message());
    out.assign_error(katana::problem_details::bad_request(std::move(error_msg)));
}

// Hash-based routing optimization (FNV-1a)
constexpr uint64_t hash_string(std::string_view str) noexcept {
    uint64_t hash = 14695981039346656037ull;
    for (char c : str) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ull;
    }
    return hash;
}

} // namespace katana::http_utils
