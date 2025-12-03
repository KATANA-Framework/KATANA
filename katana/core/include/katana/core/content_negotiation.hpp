#pragma once

#include "http.hpp"
#include "problem.hpp"
#include "result.hpp"
#include "router.hpp"

#include <span>
#include <string_view>

namespace katana::http {

struct content_type_info {
    std::string_view mime_type;
};

// Extract media type from Content-Type header, ignoring parameters (e.g., "; charset=utf-8")
inline std::string_view extract_media_type(std::string_view content_type) noexcept {
    auto semicolon_pos = content_type.find(';');
    if (semicolon_pos != std::string_view::npos) {
        content_type = content_type.substr(0, semicolon_pos);
    }
    // Trim whitespace
    while (!content_type.empty() && content_type.front() == ' ') {
        content_type.remove_prefix(1);
    }
    while (!content_type.empty() && content_type.back() == ' ') {
        content_type.remove_suffix(1);
    }
    return content_type;
}

// Check if request Content-Type is acceptable for this route
inline bool validate_content_type(const request& req,
                                  std::span<const content_type_info> accepted_types) noexcept {
    if (accepted_types.empty()) {
        return true; // No content type restrictions
    }

    auto content_type_header = req.header("Content-Type");
    if (!content_type_header) {
        return false; // Request must have Content-Type if route specifies accepted types
    }

    auto media_type = extract_media_type(*content_type_header);
    for (const auto& accepted : accepted_types) {
        if (media_type == accepted.mime_type) {
            return true;
        }
    }

    return false;
}

// Check if response can satisfy Accept header
inline bool validate_accept(const request& req,
                            std::span<const content_type_info> available_types) noexcept {
    if (available_types.empty()) {
        return true; // No restrictions
    }

    auto accept_header = req.header("Accept");
    if (!accept_header || accept_header->empty() || *accept_header == "*/*") {
        return true; // Client accepts anything
    }

    // Simple Accept header parsing (ignoring q-values and parameters)
    std::string_view remaining = *accept_header;
    while (!remaining.empty()) {
        // Find next comma
        auto comma_pos = remaining.find(',');
        auto part =
            comma_pos != std::string_view::npos ? remaining.substr(0, comma_pos) : remaining;

        auto media_type = extract_media_type(part);

        // Check wildcards
        if (media_type == "*/*") {
            return true;
        }

        // Check type/* wildcards (e.g., "application/*")
        if (media_type.size() >= 2 && media_type.substr(media_type.size() - 2) == "/*") {
            auto type_prefix = media_type.substr(0, media_type.size() - 2);
            for (const auto& available : available_types) {
                if (available.mime_type.starts_with(type_prefix)) {
                    return true;
                }
            }
        } else {
            // Exact match
            for (const auto& available : available_types) {
                if (media_type == available.mime_type) {
                    return true;
                }
            }
        }

        if (comma_pos == std::string_view::npos) {
            break;
        }
        remaining = remaining.substr(comma_pos + 1);
    }

    return false;
}

// Middleware factory for content negotiation
inline middleware_fn
make_content_negotiation_middleware(std::span<const content_type_info> consumes,
                                    std::span<const content_type_info> produces) {
    return [consumes,
            produces](const request& req, request_context& ctx, next_fn next) -> result<response> {
        // Validate Content-Type (415 Unsupported Media Type)
        if (!validate_content_type(req, consumes)) {
            auto problem = problem_details::unsupported_media_type();
            return response::error(problem);
        }

        // Validate Accept (406 Not Acceptable)
        if (!validate_accept(req, produces)) {
            auto problem = problem_details::not_acceptable();
            return response::error(problem);
        }

        return next();
    };
}

} // namespace katana::http
