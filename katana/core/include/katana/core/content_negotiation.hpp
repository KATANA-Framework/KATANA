#pragma once

#include "http.hpp"
#include "http_utils.hpp"
#include "media_type_registry.hpp"
#include "problem.hpp"
#include "result.hpp"
#include "router.hpp"

namespace katana::http {

using katana::http_utils::content_type_info;

// Extract media type from Content-Type header, ignoring parameters (e.g., "; charset=utf-8")
inline std::string_view extract_media_type(std::string_view content_type) noexcept {
    return katana::http::detail::media_type_token(content_type);
}

// Check if request Content-Type is acceptable for this route
inline bool validate_content_type(const request& req,
                                  std::span<const content_type_info> accepted_types) noexcept {
    if (accepted_types.empty()) {
        return true; // No content type restrictions
    }

    return katana::http_utils::find_content_type(req.headers.get(field::content_type), accepted_types)
        .has_value();
}

inline bool validate_content_type(const request& req,
                                  const media_type_registry& registry) noexcept {
    if (!req.headers.get(field::content_type)) {
        return true;
    }
    return katana::http_utils::find_content_type(req.headers.get(field::content_type), registry) !=
           nullptr;
}

// Check if response can satisfy Accept header
inline bool validate_accept(const request& req,
                            std::span<const content_type_info> available_types) noexcept {
    if (available_types.empty()) {
        return true; // No restrictions
    }

    return katana::http_utils::negotiate_response_type(req, available_types).has_value();
}

inline bool validate_accept(const request& req, const media_type_registry& registry) noexcept {
    auto accept = req.headers.get(field::accept);
    if (!accept || accept->empty()) {
        return true;
    }
    return katana::http_utils::negotiate_response_type(req, registry).has_value();
}

// Middleware factory for registry-driven content negotiation
inline middleware_fn make_content_negotiation_middleware(const media_type_registry& registry) {
    return [&registry](
               const request& req, request_context& ctx, response& out, next_fn next)
               -> result<void> {
        (void)ctx;
        if (!validate_content_type(req, registry)) {
            respond::into(out).problem(problem_details::unsupported_media_type());
            return {};
        }

        if (!validate_accept(req, registry)) {
            respond::into(out).problem(problem_details::not_acceptable());
            return {};
        }

        return next(out);
    };
}

// Middleware factory for content negotiation
[[deprecated("use make_content_negotiation_middleware(const media_type_registry&)")]]
inline middleware_fn
make_content_negotiation_middleware(std::span<const content_type_info> consumes,
                                    std::span<const content_type_info> produces) {
    return
        [consumes, produces](
            const request& req, request_context& ctx, response& out, next_fn next) -> result<void> {
            (void)ctx;
            // Validate Content-Type (415 Unsupported Media Type)
            if (!validate_content_type(req, consumes)) {
                respond::into(out).problem(problem_details::unsupported_media_type());
                return {};
            }

            // Validate Accept (406 Not Acceptable)
            if (!validate_accept(req, produces)) {
                respond::into(out).problem(problem_details::not_acceptable());
                return {};
            }

            return next(out);
        };
}

} // namespace katana::http
