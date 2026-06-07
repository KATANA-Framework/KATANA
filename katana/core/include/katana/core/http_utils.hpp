#pragma once

#include "katana/core/arena.hpp"
#include "katana/core/http.hpp"
#include "katana/core/media_type_registry.hpp"
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

using katana::http::detail::ascii_iequals;
using katana::http::detail::media_type_token;

} // namespace detail

struct content_type_info {
    std::string_view mime_type;
    katana::http::media_format format;

    constexpr content_type_info(
        std::string_view mime,
        katana::http::media_format fmt = katana::http::media_format::unknown) noexcept
        : mime_type(katana::http::detail::media_type_token(mime)),
          format(fmt == katana::http::media_format::unknown
                     ? katana::http::infer_media_format(mime_type)
                     : fmt) {}
};

struct named_param_target {
    std::string_view name;
    std::optional<std::string_view>* value;
};

// Percent-decode a query/path parameter value (and `+` → space for query strings). Returns
// the input unchanged (zero-copy) when there is nothing to decode; otherwise decodes into
// the request arena (the decoded form is never longer than the input). Without this, a value
// like `home%20goods` reaches the handler literally, and an encoded integer id (`%31%32`)
// fails to parse.
[[nodiscard]] inline std::string_view percent_decode_view(std::string_view raw,
                                                          katana::monotonic_arena& arena) {
    bool needs = false;
    for (char c : raw) {
        if (c == '%' || c == '+') {
            needs = true;
            break;
        }
    }
    if (!needs) {
        return raw;
    }
    char* buf = static_cast<char*>(arena.allocate(raw.size(), 1));
    if (buf == nullptr) {
        return raw; // out of arena space: fall back to the raw value
    }
    auto hexval = [](char c) -> int {
        if (c >= '0' && c <= '9') {
            return c - '0';
        }
        if (c >= 'a' && c <= 'f') {
            return c - 'a' + 10;
        }
        if (c >= 'A' && c <= 'F') {
            return c - 'A' + 10;
        }
        return -1;
    };
    size_t out = 0;
    for (size_t i = 0; i < raw.size();) {
        const char c = raw[i];
        if (c == '+') {
            buf[out++] = ' ';
            ++i;
        } else if (c == '%' && i + 2 < raw.size()) {
            const int hi = hexval(raw[i + 1]);
            const int lo = hexval(raw[i + 2]);
            if (hi >= 0 && lo >= 0) {
                buf[out++] = static_cast<char>((hi << 4) | lo);
                i += 3;
            } else {
                buf[out++] = c;
                ++i;
            }
        } else {
            buf[out++] = c;
            ++i;
        }
    }
    return std::string_view(buf, out);
}

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

inline const katana::http::media_type_entry*
find_content_type(std::optional<std::string_view> header,
                  const katana::http::media_type_registry& registry) noexcept {
    if (!header) {
        return nullptr;
    }
    return registry.find(*header);
}

inline std::optional<std::string_view>
negotiate_response_type(const katana::http::request& req,
                        std::span<const content_type_info> produces) noexcept {
    if (produces.empty())
        return std::nullopt;
    auto accept = req.headers.get(katana::http::field::accept);
    auto* negotiated = katana::http::detail::negotiate_accept(
        accept.value_or(std::string_view{}), produces, [](const content_type_info& info) {
            return info.mime_type;
        });
    if (negotiated == nullptr) {
        return std::nullopt;
    }
    return negotiated->mime_type;
}

inline std::optional<std::string_view>
negotiate_response_type(const katana::http::request& req,
                        const katana::http::media_type_registry& registry) noexcept {
    if (registry.all().empty()) {
        return std::nullopt;
    }
    auto accept = req.headers.get(katana::http::field::accept);
    auto* negotiated = registry.negotiate(accept.value_or(std::string_view{}));
    if (negotiated == nullptr) {
        return std::nullopt;
    }
    return negotiated->mime_type;
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
