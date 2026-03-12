// layer: flat
// Auto-generated router bindings from OpenAPI specification
//
// Performance characteristics:
//   - Compile-time route parsing (constexpr path_pattern)
//   - Zero-copy parameter extraction (string_view)
//   - Fast paths for common Accept headers (3 levels)
//   - Single allocation for validation errors with reserve
//   - Arena-based JSON parsing (request-scoped memory)
//   - Thread-local handler context (reactor-per-core compatible)
//   - std::from_chars for fastest integer parsing
//   - Inplace functions (160 bytes SBO, no heap allocation)
//
// Hot path optimizations:
//   1. Content negotiation: O(1) for */*, single type, or exact match
//   2. Validation: Only on error path, single allocation
//   3. Parameter parsing: Zero-copy with std::from_chars
//   4. Handler context: RAII scope guard (zero-cost abstraction)
#pragma once

#include "generated_handlers.hpp"
#include "generated_json.hpp"
#include "generated_routes.hpp"
#include "generated_validators.hpp"
#include "katana/core/handler_context.hpp"
#include "katana/core/http_server.hpp"
#include "katana/core/http_utils.hpp"
#include "katana/core/problem.hpp"
#include "katana/core/router.hpp"
#include "katana/core/serde.hpp"
#include <array>
#include <charconv>
#include <optional>
#include <span>
#include <string_view>
#include <variant>

namespace generated {

using katana::http_utils::content_type_info;
using katana::http_utils::cookie_param;
using katana::http_utils::find_content_type;
using katana::http_utils::format_validation_error;
using katana::http_utils::format_validation_error_into;
using katana::http_utils::hash_string;
using katana::http_utils::negotiate_response_type;
using katana::http_utils::query_param;

// Pre-computed path hashes for static routes
constexpr uint64_t HASH_COMPUTE_SUM = hash_string("/compute/sum");

// ============================================================
// Route Dispatch Functions
// ============================================================

// Dispatch for /compute/sum
inline katana::result<void> dispatch_compute_sum(const katana::http::request& req,
                                                 katana::http::request_context& ctx,
                                                 api_handler& handler,
                                                 katana::http::response& out) {
    constexpr std::string_view response_content_type = "application/json";
    auto accept_header = req.headers.get(katana::http::field::accept);
    if (accept_header && !accept_header->empty() && *accept_header != "*/*" &&
        *accept_header != response_content_type) {
        auto comma = accept_header->find(',');
        auto semicolon = accept_header->find(';');
        auto token_end = std::min(comma, semicolon);
        auto simple_accept = accept_header->substr(0, token_end);
        if (simple_accept != response_content_type) {
            out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
            return {};
        }
    }
    auto content_type = req.headers.get(katana::http::field::content_type);
    if (!content_type || content_type->substr(0, 16) != "application/json") {
        out.assign_error(
            katana::problem_details::unsupported_media_type("unsupported Content-Type"));
        return {};
    }
    auto parsed_body = parse_compute_sum_request(req.body, &ctx.arena);
    if (!parsed_body) {
        out.assign_error(katana::problem_details::bad_request("invalid request body"));
        return {};
    }

    // Automatic validation (optimized: single allocation)
    if (auto validation_error = validate_compute_sum_request(*parsed_body)) {
        format_validation_error_into(out, *validation_error);
        return {};
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto handler_result = handler.compute_sum(*parsed_body, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (!out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", response_content_type);
    }
    return {};
}

// ============================================================
// Router Configuration
// ============================================================

inline const katana::http::router& make_router(api_handler& handler) {
    using katana::http::handler_fn;
    using katana::http::path_pattern;
    using katana::http::route_entry;
    static std::array<route_entry, route_count> route_entries = {
        route_entry{katana::http::method::post,
                    katana::http::path_pattern::from_literal<"/compute/sum">(),
                    handler_fn([&handler](const katana::http::request& req,
                                          katana::http::request_context& ctx,
                                          katana::http::response& out) -> katana::result<void> {
                        return dispatch_compute_sum(req, ctx, handler, out);
                    })},
    };
    static katana::http::router router_instance(route_entries);
    return router_instance;
}

// Optimized router with hash-based O(1) dispatch for static routes
class fast_router {
public:
    explicit fast_router(api_handler& handler, const katana::http::router& fallback)
        : handler_(handler), fallback_router_(fallback) {}

    katana::result<void> dispatch_to(const katana::http::request& req,
                                     katana::http::request_context& ctx,
                                     katana::http::response& out) const {
        // Strip query string for matching
        std::string_view path = req.uri;
        auto query_pos = path.find('?');
        if (query_pos != std::string_view::npos) {
            path = path.substr(0, query_pos);
        }

        // Fast path: O(1) hash-based dispatch for static routes
        uint64_t path_hash = hash_string(path);
        switch (path_hash) {
        case HASH_COMPUTE_SUM:
            if (path == "/compute/sum") {
                if (req.http_method == katana::http::method::post) {
                    return dispatch_compute_sum(req, ctx, handler_, out);
                }
            }
            break;
        default:
            break;
        }

        // Fallback to standard router for:
        // - Dynamic routes (with path parameters)
        // - Hash collisions
        // - Method mismatches
        return fallback_router_.dispatch(req, ctx, out);
    }

    katana::result<katana::http::response> operator()(const katana::http::request& req,
                                                      katana::http::request_context& ctx) const {
        katana::http::response out;
        auto status = dispatch_to(req, ctx, out);
        if (!status) {
            return std::unexpected(status.error());
        }
        return out;
    }

private:
    api_handler& handler_;
    const katana::http::router& fallback_router_;
};

// Create optimized router (recommended for production)
inline fast_router make_fast_router(api_handler& handler) {
    return fast_router(handler, make_router(handler));
}

// Zero-boilerplate server creation
// Usage: return generated::serve<MyHandler>(8080);
template <typename Handler, typename... Args> inline auto make_server(Args&&... args) {
    static Handler handler_instance{std::forward<Args>(args)...};
    const auto& router = make_router(handler_instance);
    return katana::http::server(router);
}

template <typename Handler, typename... Args> inline int serve(uint16_t port, Args&&... args) {
    return make_server<Handler>(std::forward<Args>(args)...)
        .listen(port)
        .workers(4)
        .backlog(1024)
        .reuseport(true)
        .run();
}

} // namespace generated
