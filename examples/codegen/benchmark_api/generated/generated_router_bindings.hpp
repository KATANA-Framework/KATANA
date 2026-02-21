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

#include "katana/core/router.hpp"
#include "katana/core/problem.hpp"
#include "katana/core/serde.hpp"
#include "katana/core/handler_context.hpp"
#include "katana/core/http_server.hpp"
#include "katana/core/http_utils.hpp"
#include "generated_routes.hpp"
#include "generated_handlers.hpp"
#include "generated_json.hpp"
#include "generated_validators.hpp"
#include <array>
#include <charconv>
#include <optional>
#include <variant>
#include <span>
#include <string_view>

namespace generated {

using katana::http_utils::query_param;
using katana::http_utils::cookie_param;
using katana::http_utils::find_content_type;
using katana::http_utils::negotiate_response_type;
using katana::http_utils::format_validation_error;
using katana::http_utils::hash_string;
using katana::http_utils::content_type_info;

// Pre-computed path hashes for static routes
constexpr uint64_t HASH_COMPUTE_SUM = hash_string("/compute/sum");
constexpr uint64_t HASH_COMPUTE_STATS = hash_string("/compute/stats");
constexpr uint64_t HASH_REGISTER_USER = hash_string("/users/register");
constexpr uint64_t HASH_LIST_ITEMS = hash_string("/items");
constexpr uint64_t HASH_ECHO = hash_string("/echo");
constexpr uint64_t HASH_HEALTH_CHECK = hash_string("/health");

// ============================================================
// Route Dispatch Functions
// ============================================================

// Dispatch for /compute/sum
inline katana::result<katana::http::response> dispatch_compute_sum(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler) {
    auto response_content_type = negotiate_response_type(req, route_0_produces);
    if (!response_content_type) {
        return katana::http::response::error(katana::problem_details::not_acceptable("unsupported Accept header"));
    }
    auto content_type_index = find_content_type(req.headers.get(katana::http::field::content_type), route_0_consumes);
    if (!content_type_index) return katana::http::response::error(katana::problem_details::unsupported_media_type("unsupported Content-Type"));
    std::optional<SumRequest> parsed_body;
    switch (*content_type_index) {
    case 0: {
        auto parsed_body_candidate = parse_SumRequest(req.body, &ctx.arena);
        if (!parsed_body_candidate) return katana::http::response::error(katana::problem_details::bad_request("invalid request body"));
        parsed_body = std::move(*parsed_body_candidate);
        break;
    }
    default:
        return katana::http::response::error(katana::problem_details::unsupported_media_type("unsupported Content-Type"));
    }

    // Automatic validation (optimized: single allocation)
    if (auto validation_error = validate_SumRequest(*parsed_body)) {
        return format_validation_error(*validation_error);
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto result = handler.compute_sum(*parsed_body);
    if (response_content_type && !result.headers.get(katana::http::field::content_type)) {
        result.set_header("Content-Type", *response_content_type);
    }
    return result;
}

// Dispatch for /compute/stats
inline katana::result<katana::http::response> dispatch_compute_stats(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler) {
    auto response_content_type = negotiate_response_type(req, route_1_produces);
    if (!response_content_type) {
        return katana::http::response::error(katana::problem_details::not_acceptable("unsupported Accept header"));
    }
    auto content_type_index = find_content_type(req.headers.get(katana::http::field::content_type), route_1_consumes);
    if (!content_type_index) return katana::http::response::error(katana::problem_details::unsupported_media_type("unsupported Content-Type"));
    std::optional<StatsRequest> parsed_body;
    switch (*content_type_index) {
    case 0: {
        auto parsed_body_candidate = parse_StatsRequest(req.body, &ctx.arena);
        if (!parsed_body_candidate) return katana::http::response::error(katana::problem_details::bad_request("invalid request body"));
        parsed_body = std::move(*parsed_body_candidate);
        break;
    }
    default:
        return katana::http::response::error(katana::problem_details::unsupported_media_type("unsupported Content-Type"));
    }

    // Automatic validation (optimized: single allocation)
    if (auto validation_error = validate_StatsRequest(*parsed_body)) {
        return format_validation_error(*validation_error);
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto result = handler.compute_stats(*parsed_body);
    if (response_content_type && !result.headers.get(katana::http::field::content_type)) {
        result.set_header("Content-Type", *response_content_type);
    }
    return result;
}

// Dispatch for /users/register
inline katana::result<katana::http::response> dispatch_register_user(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler) {
    auto response_content_type = negotiate_response_type(req, route_2_produces);
    if (!response_content_type) {
        return katana::http::response::error(katana::problem_details::not_acceptable("unsupported Accept header"));
    }
    auto content_type_index = find_content_type(req.headers.get(katana::http::field::content_type), route_2_consumes);
    if (!content_type_index) return katana::http::response::error(katana::problem_details::unsupported_media_type("unsupported Content-Type"));
    std::optional<RegisterRequest> parsed_body;
    switch (*content_type_index) {
    case 0: {
        auto parsed_body_candidate = parse_RegisterRequest(req.body, &ctx.arena);
        if (!parsed_body_candidate) return katana::http::response::error(katana::problem_details::bad_request("invalid request body"));
        parsed_body = std::move(*parsed_body_candidate);
        break;
    }
    default:
        return katana::http::response::error(katana::problem_details::unsupported_media_type("unsupported Content-Type"));
    }

    // Automatic validation (optimized: single allocation)
    if (auto validation_error = validate_RegisterRequest(*parsed_body)) {
        return format_validation_error(*validation_error);
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto result = handler.register_user(*parsed_body);
    if (response_content_type && !result.headers.get(katana::http::field::content_type)) {
        result.set_header("Content-Type", *response_content_type);
    }
    return result;
}

// Dispatch for /items
inline katana::result<katana::http::response> dispatch_list_items(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler) {
    auto response_content_type = negotiate_response_type(req, route_3_produces);
    if (!response_content_type) {
        return katana::http::response::error(katana::problem_details::not_acceptable("unsupported Accept header"));
    }
    auto p_limit = query_param(req.uri, "limit");
    std::optional<int64_t> limit;
    if (p_limit) {
        int64_t tmp = 0;
        auto [ptr, ec] = std::from_chars(p_limit->data(), p_limit->data() + p_limit->size(), tmp);
        if (ec != std::errc() || ptr != p_limit->data() + p_limit->size()) return katana::http::response::error(katana::problem_details::bad_request("invalid param limit"));
        limit = tmp;
    }
    auto p_offset = query_param(req.uri, "offset");
    std::optional<int64_t> offset;
    if (p_offset) {
        int64_t tmp = 0;
        auto [ptr, ec] = std::from_chars(p_offset->data(), p_offset->data() + p_offset->size(), tmp);
        if (ec != std::errc() || ptr != p_offset->data() + p_offset->size()) return katana::http::response::error(katana::problem_details::bad_request("invalid param offset"));
        offset = tmp;
    }
    auto p_category = query_param(req.uri, "category");
    std::optional<std::string_view> category = std::nullopt;
    if (p_category) category = *p_category;
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto result = handler.list_items(limit, offset, category);
    if (response_content_type && !result.headers.get(katana::http::field::content_type)) {
        result.set_header("Content-Type", *response_content_type);
    }
    return result;
}

// Dispatch for /items
inline katana::result<katana::http::response> dispatch_create_item(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler) {
    auto response_content_type = negotiate_response_type(req, route_4_produces);
    if (!response_content_type) {
        return katana::http::response::error(katana::problem_details::not_acceptable("unsupported Accept header"));
    }
    auto p_X_Request_Id = req.headers.get("X-Request-Id");
    if (!p_X_Request_Id) return katana::http::response::error(katana::problem_details::bad_request("missing param X-Request-Id"));
    auto X_Request_Id = p_X_Request_Id ? *p_X_Request_Id : std::string_view{};
    auto p_session = cookie_param(req, "session");
    std::optional<std::string_view> session = std::nullopt;
    if (p_session) session = *p_session;
    auto content_type_index = find_content_type(req.headers.get(katana::http::field::content_type), route_4_consumes);
    if (!content_type_index) return katana::http::response::error(katana::problem_details::unsupported_media_type("unsupported Content-Type"));
    std::optional<CreateItemRequest> parsed_body;
    switch (*content_type_index) {
    case 0: {
        auto parsed_body_candidate = parse_CreateItemRequest(req.body, &ctx.arena);
        if (!parsed_body_candidate) return katana::http::response::error(katana::problem_details::bad_request("invalid request body"));
        parsed_body = std::move(*parsed_body_candidate);
        break;
    }
    default:
        return katana::http::response::error(katana::problem_details::unsupported_media_type("unsupported Content-Type"));
    }

    // Automatic validation (optimized: single allocation)
    if (auto validation_error = validate_CreateItemRequest(*parsed_body)) {
        return format_validation_error(*validation_error);
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto result = handler.create_item(X_Request_Id, session, *parsed_body);
    if (response_content_type && !result.headers.get(katana::http::field::content_type)) {
        result.set_header("Content-Type", *response_content_type);
    }
    return result;
}

// Dispatch for /items/{id}
inline katana::result<katana::http::response> dispatch_get_item(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler) {
    auto response_content_type = negotiate_response_type(req, route_5_produces);
    if (!response_content_type) {
        return katana::http::response::error(katana::problem_details::not_acceptable("unsupported Accept header"));
    }
    auto p_id = ctx.params.get("id");
    if (!p_id) return katana::http::response::error(katana::problem_details::bad_request("missing path param id"));
    int64_t id = 0;
    {
        auto [ptr, ec] = std::from_chars(p_id->data(), p_id->data() + p_id->size(), id);
        if (ec != std::errc() || ptr != p_id->data() + p_id->size()) return katana::http::response::error(katana::problem_details::bad_request("invalid path param id"));
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto result = handler.get_item(id);
    if (response_content_type && !result.headers.get(katana::http::field::content_type)) {
        result.set_header("Content-Type", *response_content_type);
    }
    return result;
}

// Dispatch for /items/{id}
inline katana::result<katana::http::response> dispatch_update_item(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler) {
    auto response_content_type = negotiate_response_type(req, route_6_produces);
    if (!response_content_type) {
        return katana::http::response::error(katana::problem_details::not_acceptable("unsupported Accept header"));
    }
    auto p_id = ctx.params.get("id");
    if (!p_id) return katana::http::response::error(katana::problem_details::bad_request("missing path param id"));
    int64_t id = 0;
    {
        auto [ptr, ec] = std::from_chars(p_id->data(), p_id->data() + p_id->size(), id);
        if (ec != std::errc() || ptr != p_id->data() + p_id->size()) return katana::http::response::error(katana::problem_details::bad_request("invalid path param id"));
    }
    auto content_type_index = find_content_type(req.headers.get(katana::http::field::content_type), route_6_consumes);
    if (!content_type_index) return katana::http::response::error(katana::problem_details::unsupported_media_type("unsupported Content-Type"));
    std::optional<UpdateItemRequest> parsed_body;
    switch (*content_type_index) {
    case 0: {
        auto parsed_body_candidate = parse_UpdateItemRequest(req.body, &ctx.arena);
        if (!parsed_body_candidate) return katana::http::response::error(katana::problem_details::bad_request("invalid request body"));
        parsed_body = std::move(*parsed_body_candidate);
        break;
    }
    default:
        return katana::http::response::error(katana::problem_details::unsupported_media_type("unsupported Content-Type"));
    }

    // Automatic validation (optimized: single allocation)
    if (auto validation_error = validate_UpdateItemRequest(*parsed_body)) {
        return format_validation_error(*validation_error);
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto result = handler.update_item(id, *parsed_body);
    if (response_content_type && !result.headers.get(katana::http::field::content_type)) {
        result.set_header("Content-Type", *response_content_type);
    }
    return result;
}

// Dispatch for /items/{id}
inline katana::result<katana::http::response> dispatch_delete_item(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler) {
    auto p_id = ctx.params.get("id");
    if (!p_id) return katana::http::response::error(katana::problem_details::bad_request("missing path param id"));
    int64_t id = 0;
    {
        auto [ptr, ec] = std::from_chars(p_id->data(), p_id->data() + p_id->size(), id);
        if (ec != std::errc() || ptr != p_id->data() + p_id->size()) return katana::http::response::error(katana::problem_details::bad_request("invalid path param id"));
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto result = handler.delete_item(id);
    return result;
}

// Dispatch for /echo
inline katana::result<katana::http::response> dispatch_echo(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler) {
    auto response_content_type = negotiate_response_type(req, route_8_produces);
    if (!response_content_type) {
        return katana::http::response::error(katana::problem_details::not_acceptable("unsupported Accept header"));
    }
    auto content_type_index = find_content_type(req.headers.get(katana::http::field::content_type), route_8_consumes);
    if (!content_type_index) return katana::http::response::error(katana::problem_details::unsupported_media_type("unsupported Content-Type"));
    std::optional<EchoRequest> parsed_body;
    switch (*content_type_index) {
    case 0: {
        auto parsed_body_candidate = parse_EchoRequest(req.body, &ctx.arena);
        if (!parsed_body_candidate) return katana::http::response::error(katana::problem_details::bad_request("invalid request body"));
        parsed_body = std::move(*parsed_body_candidate);
        break;
    }
    default:
        return katana::http::response::error(katana::problem_details::unsupported_media_type("unsupported Content-Type"));
    }

    // Automatic validation (optimized: single allocation)
    if (auto validation_error = validate_EchoRequest(*parsed_body)) {
        return format_validation_error(*validation_error);
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto result = handler.echo(*parsed_body);
    if (response_content_type && !result.headers.get(katana::http::field::content_type)) {
        result.set_header("Content-Type", *response_content_type);
    }
    return result;
}

// Dispatch for /health
inline katana::result<katana::http::response> dispatch_health_check(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler) {
    auto response_content_type = negotiate_response_type(req, route_9_produces);
    if (!response_content_type) {
        return katana::http::response::error(katana::problem_details::not_acceptable("unsupported Accept header"));
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto result = handler.health_check();
    if (response_content_type && !result.headers.get(katana::http::field::content_type)) {
        result.set_header("Content-Type", *response_content_type);
    }
    return result;
}

// ============================================================
// Router Configuration
// ============================================================

inline const katana::http::router& make_router(api_handler& handler) {
    using katana::http::route_entry;
    using katana::http::path_pattern;
    using katana::http::handler_fn;
    static std::array<route_entry, route_count> route_entries = {
        route_entry{katana::http::method::post,
                   katana::http::path_pattern::from_literal<"/compute/sum">(),
                   handler_fn([&handler](const katana::http::request& req, katana::http::request_context& ctx) -> katana::result<katana::http::response> {
                       return dispatch_compute_sum(req, ctx, handler);
                   })
        },
        route_entry{katana::http::method::post,
                   katana::http::path_pattern::from_literal<"/compute/stats">(),
                   handler_fn([&handler](const katana::http::request& req, katana::http::request_context& ctx) -> katana::result<katana::http::response> {
                       return dispatch_compute_stats(req, ctx, handler);
                   })
        },
        route_entry{katana::http::method::post,
                   katana::http::path_pattern::from_literal<"/users/register">(),
                   handler_fn([&handler](const katana::http::request& req, katana::http::request_context& ctx) -> katana::result<katana::http::response> {
                       return dispatch_register_user(req, ctx, handler);
                   })
        },
        route_entry{katana::http::method::get,
                   katana::http::path_pattern::from_literal<"/items">(),
                   handler_fn([&handler](const katana::http::request& req, katana::http::request_context& ctx) -> katana::result<katana::http::response> {
                       return dispatch_list_items(req, ctx, handler);
                   })
        },
        route_entry{katana::http::method::post,
                   katana::http::path_pattern::from_literal<"/items">(),
                   handler_fn([&handler](const katana::http::request& req, katana::http::request_context& ctx) -> katana::result<katana::http::response> {
                       return dispatch_create_item(req, ctx, handler);
                   })
        },
        route_entry{katana::http::method::get,
                   katana::http::path_pattern::from_literal<"/items/{id}">(),
                   handler_fn([&handler](const katana::http::request& req, katana::http::request_context& ctx) -> katana::result<katana::http::response> {
                       return dispatch_get_item(req, ctx, handler);
                   })
        },
        route_entry{katana::http::method::put,
                   katana::http::path_pattern::from_literal<"/items/{id}">(),
                   handler_fn([&handler](const katana::http::request& req, katana::http::request_context& ctx) -> katana::result<katana::http::response> {
                       return dispatch_update_item(req, ctx, handler);
                   })
        },
        route_entry{katana::http::method::del,
                   katana::http::path_pattern::from_literal<"/items/{id}">(),
                   handler_fn([&handler](const katana::http::request& req, katana::http::request_context& ctx) -> katana::result<katana::http::response> {
                       return dispatch_delete_item(req, ctx, handler);
                   })
        },
        route_entry{katana::http::method::post,
                   katana::http::path_pattern::from_literal<"/echo">(),
                   handler_fn([&handler](const katana::http::request& req, katana::http::request_context& ctx) -> katana::result<katana::http::response> {
                       return dispatch_echo(req, ctx, handler);
                   })
        },
        route_entry{katana::http::method::get,
                   katana::http::path_pattern::from_literal<"/health">(),
                   handler_fn([&handler](const katana::http::request& req, katana::http::request_context& ctx) -> katana::result<katana::http::response> {
                       return dispatch_health_check(req, ctx, handler);
                   })
        },
    };
    static katana::http::router router_instance(route_entries);
    return router_instance;
}

// Optimized router with hash-based O(1) dispatch for static routes
class fast_router {
public:
    explicit fast_router(api_handler& handler, const katana::http::router& fallback)
        : handler_(handler), fallback_router_(fallback) {}

    katana::result<katana::http::response> operator()(
        const katana::http::request& req,
        katana::http::request_context& ctx) const {
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
                    if (req.http_method == katana::http::method::post)
                        return dispatch_compute_sum(req, ctx, handler_);
                }
                break;
            case HASH_COMPUTE_STATS:
                if (path == "/compute/stats") {
                    if (req.http_method == katana::http::method::post)
                        return dispatch_compute_stats(req, ctx, handler_);
                }
                break;
            case HASH_REGISTER_USER:
                if (path == "/users/register") {
                    if (req.http_method == katana::http::method::post)
                        return dispatch_register_user(req, ctx, handler_);
                }
                break;
            case HASH_LIST_ITEMS:
                if (path == "/items") {
                    if (req.http_method == katana::http::method::get)
                        return dispatch_list_items(req, ctx, handler_);
                    if (req.http_method == katana::http::method::post)
                        return dispatch_create_item(req, ctx, handler_);
                }
                break;
            case HASH_ECHO:
                if (path == "/echo") {
                    if (req.http_method == katana::http::method::post)
                        return dispatch_echo(req, ctx, handler_);
                }
                break;
            case HASH_HEALTH_CHECK:
                if (path == "/health") {
                    if (req.http_method == katana::http::method::get)
                        return dispatch_health_check(req, ctx, handler_);
                }
                break;
            default:
                break;
        }

        // Fallback to standard router for:
        // - Dynamic routes (with path parameters)
        // - Hash collisions
        // - Method mismatches
        return fallback_router_.dispatch(req, ctx);
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
template<typename Handler, typename... Args>
inline auto make_server(Args&&... args) {
    static Handler handler_instance{std::forward<Args>(args)...};
    const auto& router = make_router(handler_instance);
    return katana::http::server(router);
}

template<typename Handler, typename... Args>
inline int serve(uint16_t port, Args&&... args) {
    return make_server<Handler>(std::forward<Args>(args)...)
        .listen(port)
        .workers(4)
        .backlog(1024)
        .reuseport(true)
        .run();
}

} // namespace generated
