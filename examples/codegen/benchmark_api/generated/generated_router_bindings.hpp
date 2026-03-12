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
#include <chrono>
#include <functional>
#include <optional>
#include <variant>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace generated {

using katana::http_utils::query_param;
using katana::http_utils::cookie_param;
using katana::http_utils::extract_query_params;
using katana::http_utils::extract_cookie_params;
using katana::http_utils::find_content_type;
using katana::http_utils::negotiate_response_type;
using katana::http_utils::format_validation_error;
using katana::http_utils::format_validation_error_into;
using katana::http_utils::hash_string;
using katana::http_utils::content_type_info;
using katana::http_utils::named_param_target;

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
inline katana::result<void> dispatch_compute_sum(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    auto negotiated_content_type = negotiate_response_type(req, route_0_produces);
    if (!negotiated_content_type) {
        out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
        return {};
    }
    std::string_view response_content_type = *negotiated_content_type;
    auto content_type_index = find_content_type(req.headers.get(katana::http::field::content_type), route_0_consumes);
    if (!content_type_index) { out.assign_error(katana::problem_details::unsupported_media_type("unsupported Content-Type")); return {}; }
    auto parsed_body = parse_SumRequest(req.body, &ctx.arena);
    if (!parsed_body) {
        out.assign_error(katana::problem_details::bad_request("invalid request body")); return {};
    }

    // Automatic validation (optimized: single allocation)
    if (auto validation_error = validate_SumRequest(*parsed_body)) {
        format_validation_error_into(out, *validation_error);
        return {};
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto handler_result = handler.compute_sum(*parsed_body, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (out.status != 204 && !out.body.empty() && !out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", response_content_type);
    }
    return {};
}

// Dispatch for /compute/stats
inline katana::result<void> dispatch_compute_stats(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    auto negotiated_content_type = negotiate_response_type(req, route_1_produces);
    if (!negotiated_content_type) {
        out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
        return {};
    }
    std::string_view response_content_type = *negotiated_content_type;
    auto content_type_index = find_content_type(req.headers.get(katana::http::field::content_type), route_1_consumes);
    if (!content_type_index) { out.assign_error(katana::problem_details::unsupported_media_type("unsupported Content-Type")); return {}; }
    auto parsed_body = parse_StatsRequest(req.body, &ctx.arena);
    if (!parsed_body) {
        out.assign_error(katana::problem_details::bad_request("invalid request body")); return {};
    }

    // Automatic validation (optimized: single allocation)
    if (auto validation_error = validate_StatsRequest(*parsed_body)) {
        format_validation_error_into(out, *validation_error);
        return {};
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto handler_result = handler.compute_stats(*parsed_body, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (out.status != 204 && !out.body.empty() && !out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", response_content_type);
    }
    return {};
}

// Dispatch for /users/register
inline katana::result<void> dispatch_register_user(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    auto negotiated_content_type = negotiate_response_type(req, route_2_produces);
    if (!negotiated_content_type) {
        out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
        return {};
    }
    std::string_view response_content_type = *negotiated_content_type;
    auto content_type_index = find_content_type(req.headers.get(katana::http::field::content_type), route_2_consumes);
    if (!content_type_index) { out.assign_error(katana::problem_details::unsupported_media_type("unsupported Content-Type")); return {}; }
    auto parsed_body = parse_RegisterRequest(req.body, &ctx.arena);
    if (!parsed_body) {
        out.assign_error(katana::problem_details::bad_request("invalid request body")); return {};
    }

    // Automatic validation (optimized: single allocation)
    if (auto validation_error = validate_RegisterRequest(*parsed_body)) {
        format_validation_error_into(out, *validation_error);
        return {};
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto handler_result = handler.register_user(*parsed_body, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (out.status != 204 && !out.body.empty() && !out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", response_content_type);
    }
    return {};
}

// Dispatch for /items
inline katana::result<void> dispatch_list_items(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    auto negotiated_content_type = negotiate_response_type(req, route_3_produces);
    if (!negotiated_content_type) {
        out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
        return {};
    }
    std::string_view response_content_type = *negotiated_content_type;
    std::optional<std::string_view> p_limit = std::nullopt;
    std::optional<std::string_view> p_offset = std::nullopt;
    std::optional<std::string_view> p_category = std::nullopt;
    extract_query_params(req.uri, std::array{
        named_param_target{"limit", &p_limit},
        named_param_target{"offset", &p_offset},
        named_param_target{"category", &p_category},
    });
    std::optional<int64_t> limit;
    if (p_limit) {
        int64_t tmp = 0;
        auto [ptr, ec] = std::from_chars(p_limit->data(), p_limit->data() + p_limit->size(), tmp);
        if (ec != std::errc() || ptr != p_limit->data() + p_limit->size()) { out = katana::http::response::error(katana::problem_details::bad_request("invalid param limit")); return {}; }
        limit = tmp;
    }
    std::optional<int64_t> offset;
    if (p_offset) {
        int64_t tmp = 0;
        auto [ptr, ec] = std::from_chars(p_offset->data(), p_offset->data() + p_offset->size(), tmp);
        if (ec != std::errc() || ptr != p_offset->data() + p_offset->size()) { out = katana::http::response::error(katana::problem_details::bad_request("invalid param offset")); return {}; }
        offset = tmp;
    }
    std::optional<std::string_view> category = std::nullopt;
    if (p_category) category = *p_category;
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto handler_result = handler.list_items(limit, offset, category, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (out.status != 204 && !out.body.empty() && !out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", response_content_type);
    }
    return {};
}

// Dispatch for /items
inline katana::result<void> dispatch_create_item(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    auto negotiated_content_type = negotiate_response_type(req, route_4_produces);
    if (!negotiated_content_type) {
        out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
        return {};
    }
    std::string_view response_content_type = *negotiated_content_type;
    auto p_X_Request_Id = req.headers.get("X-Request-Id");
    if (!p_X_Request_Id) { out = katana::http::response::error(katana::problem_details::bad_request("missing param X-Request-Id")); return {}; }
    auto X_Request_Id = p_X_Request_Id ? *p_X_Request_Id : std::string_view{};
    auto p_session = cookie_param(req, "session");
    std::optional<std::string_view> session = std::nullopt;
    if (p_session) session = *p_session;
    auto content_type_index = find_content_type(req.headers.get(katana::http::field::content_type), route_4_consumes);
    if (!content_type_index) { out.assign_error(katana::problem_details::unsupported_media_type("unsupported Content-Type")); return {}; }
    auto parsed_body = parse_CreateItemRequest(req.body, &ctx.arena);
    if (!parsed_body) {
        out.assign_error(katana::problem_details::bad_request("invalid request body")); return {};
    }

    // Automatic validation (optimized: single allocation)
    if (auto validation_error = validate_CreateItemRequest(*parsed_body)) {
        format_validation_error_into(out, *validation_error);
        return {};
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto handler_result = handler.create_item(X_Request_Id, session, *parsed_body, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (out.status != 204 && !out.body.empty() && !out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", response_content_type);
    }
    return {};
}

// Dispatch for /items/{id}
inline katana::result<void> dispatch_get_item(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    auto negotiated_content_type = negotiate_response_type(req, route_5_produces);
    if (!negotiated_content_type) {
        out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
        return {};
    }
    std::string_view response_content_type = *negotiated_content_type;
    auto p_id = ctx.params.get("id");
    if (!p_id) { out = katana::http::response::error(katana::problem_details::bad_request("missing path param id")); return {}; }
    int64_t id = 0;
    {
        auto [ptr, ec] = std::from_chars(p_id->data(), p_id->data() + p_id->size(), id);
        if (ec != std::errc() || ptr != p_id->data() + p_id->size()) { out = katana::http::response::error(katana::problem_details::bad_request("invalid path param id")); return {}; }
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto handler_result = handler.get_item(id, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (out.status != 204 && !out.body.empty() && !out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", response_content_type);
    }
    return {};
}

// Dispatch for /items/{id}
inline katana::result<void> dispatch_update_item(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    auto negotiated_content_type = negotiate_response_type(req, route_6_produces);
    if (!negotiated_content_type) {
        out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
        return {};
    }
    std::string_view response_content_type = *negotiated_content_type;
    auto p_id = ctx.params.get("id");
    if (!p_id) { out = katana::http::response::error(katana::problem_details::bad_request("missing path param id")); return {}; }
    int64_t id = 0;
    {
        auto [ptr, ec] = std::from_chars(p_id->data(), p_id->data() + p_id->size(), id);
        if (ec != std::errc() || ptr != p_id->data() + p_id->size()) { out = katana::http::response::error(katana::problem_details::bad_request("invalid path param id")); return {}; }
    }
    auto content_type_index = find_content_type(req.headers.get(katana::http::field::content_type), route_6_consumes);
    if (!content_type_index) { out.assign_error(katana::problem_details::unsupported_media_type("unsupported Content-Type")); return {}; }
    auto parsed_body = parse_UpdateItemRequest(req.body, &ctx.arena);
    if (!parsed_body) {
        out.assign_error(katana::problem_details::bad_request("invalid request body")); return {};
    }

    // Automatic validation (optimized: single allocation)
    if (auto validation_error = validate_UpdateItemRequest(*parsed_body)) {
        format_validation_error_into(out, *validation_error);
        return {};
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto handler_result = handler.update_item(id, *parsed_body, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (out.status != 204 && !out.body.empty() && !out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", response_content_type);
    }
    return {};
}

// Dispatch for /items/{id}
inline katana::result<void> dispatch_delete_item(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    auto p_id = ctx.params.get("id");
    if (!p_id) { out = katana::http::response::error(katana::problem_details::bad_request("missing path param id")); return {}; }
    int64_t id = 0;
    {
        auto [ptr, ec] = std::from_chars(p_id->data(), p_id->data() + p_id->size(), id);
        if (ec != std::errc() || ptr != p_id->data() + p_id->size()) { out = katana::http::response::error(katana::problem_details::bad_request("invalid path param id")); return {}; }
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto handler_result = handler.delete_item(id, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    return {};
}

// Dispatch for /echo
inline katana::result<void> dispatch_echo(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    auto negotiated_content_type = negotiate_response_type(req, route_8_produces);
    if (!negotiated_content_type) {
        out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
        return {};
    }
    std::string_view response_content_type = *negotiated_content_type;
    auto content_type_index = find_content_type(req.headers.get(katana::http::field::content_type), route_8_consumes);
    if (!content_type_index) { out.assign_error(katana::problem_details::unsupported_media_type("unsupported Content-Type")); return {}; }
    auto parsed_body = parse_EchoRequest(req.body, &ctx.arena);
    if (!parsed_body) {
        out.assign_error(katana::problem_details::bad_request("invalid request body")); return {};
    }

    // Automatic validation (optimized: single allocation)
    if (auto validation_error = validate_EchoRequest(*parsed_body)) {
        format_validation_error_into(out, *validation_error);
        return {};
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto handler_result = handler.echo(*parsed_body, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (out.status != 204 && !out.body.empty() && !out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", response_content_type);
    }
    return {};
}

// Dispatch for /health
inline katana::result<void> dispatch_health_check(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    auto negotiated_content_type = negotiate_response_type(req, route_9_produces);
    if (!negotiated_content_type) {
        out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
        return {};
    }
    std::string_view response_content_type = *negotiated_content_type;
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto handler_result = handler.health_check(out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (out.status != 204 && !out.body.empty() && !out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", response_content_type);
    }
    return {};
}

// ============================================================
// Router Configuration
// ============================================================

class generated_router {
public:
    explicit generated_router(api_handler& handler)
        : route_entries_{
        katana::http::route_entry{katana::http::method::post,
                   katana::http::path_pattern::from_literal<"/compute/sum">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_compute_sum(req, ctx, *handler_ptr, out);
                   })
        },
        katana::http::route_entry{katana::http::method::post,
                   katana::http::path_pattern::from_literal<"/compute/stats">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_compute_stats(req, ctx, *handler_ptr, out);
                   })
        },
        katana::http::route_entry{katana::http::method::post,
                   katana::http::path_pattern::from_literal<"/users/register">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_register_user(req, ctx, *handler_ptr, out);
                   })
        },
        katana::http::route_entry{katana::http::method::get,
                   katana::http::path_pattern::from_literal<"/items">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_list_items(req, ctx, *handler_ptr, out);
                   })
        },
        katana::http::route_entry{katana::http::method::post,
                   katana::http::path_pattern::from_literal<"/items">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_create_item(req, ctx, *handler_ptr, out);
                   })
        },
        katana::http::route_entry{katana::http::method::get,
                   katana::http::path_pattern::from_literal<"/items/{id}">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_get_item(req, ctx, *handler_ptr, out);
                   })
        },
        katana::http::route_entry{katana::http::method::put,
                   katana::http::path_pattern::from_literal<"/items/{id}">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_update_item(req, ctx, *handler_ptr, out);
                   })
        },
        katana::http::route_entry{katana::http::method::del,
                   katana::http::path_pattern::from_literal<"/items/{id}">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_delete_item(req, ctx, *handler_ptr, out);
                   })
        },
        katana::http::route_entry{katana::http::method::post,
                   katana::http::path_pattern::from_literal<"/echo">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_echo(req, ctx, *handler_ptr, out);
                   })
        },
        katana::http::route_entry{katana::http::method::get,
                   katana::http::path_pattern::from_literal<"/health">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_health_check(req, ctx, *handler_ptr, out);
                   })
        },
        } {
        router_.emplace(route_entries_);
    }

    generated_router(const generated_router&) = delete;
    generated_router& operator=(const generated_router&) = delete;
    generated_router(generated_router&&) = delete;
    generated_router& operator=(generated_router&&) = delete;

    [[nodiscard]] const katana::http::router& router() const noexcept { return *router_; }
    [[nodiscard]] katana::http::router& router() noexcept { return *router_; }
    [[nodiscard]] operator const katana::http::router&() const noexcept { return *router_; }
    [[nodiscard]] operator katana::http::router&() noexcept { return *router_; }

private:
    std::array<katana::http::route_entry, route_count> route_entries_;
    std::optional<katana::http::router> router_;
};

inline generated_router make_router(api_handler& handler) {
    return generated_router(handler);
}

// Optimized router with hash-based O(1) dispatch for static routes
class fast_router {
public:
    explicit fast_router(api_handler& handler, const katana::http::router& fallback)
        : handler_(handler), fallback_router_(fallback) {}

    katana::result<void> dispatch_to(
        const katana::http::request& req,
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
                    if (req.http_method == katana::http::method::post)
                        { return dispatch_compute_sum(req, ctx, handler_, out); }
                }
                break;
            case HASH_COMPUTE_STATS:
                if (path == "/compute/stats") {
                    if (req.http_method == katana::http::method::post)
                        { return dispatch_compute_stats(req, ctx, handler_, out); }
                }
                break;
            case HASH_REGISTER_USER:
                if (path == "/users/register") {
                    if (req.http_method == katana::http::method::post)
                        { return dispatch_register_user(req, ctx, handler_, out); }
                }
                break;
            case HASH_LIST_ITEMS:
                if (path == "/items") {
                    if (req.http_method == katana::http::method::get)
                        { return dispatch_list_items(req, ctx, handler_, out); }
                    if (req.http_method == katana::http::method::post)
                        { return dispatch_create_item(req, ctx, handler_, out); }
                }
                break;
            case HASH_ECHO:
                if (path == "/echo") {
                    if (req.http_method == katana::http::method::post)
                        { return dispatch_echo(req, ctx, handler_, out); }
                }
                break;
            case HASH_HEALTH_CHECK:
                if (path == "/health") {
                    if (req.http_method == katana::http::method::get)
                        { return dispatch_health_check(req, ctx, handler_, out); }
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

    katana::result<katana::http::response> operator()(
        const katana::http::request& req,
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
class generated_fast_router {
public:
    explicit generated_fast_router(api_handler& handler)
        : router_bundle_(handler), fast_router_(handler, router_bundle_.router()) {}

    generated_fast_router(const generated_fast_router&) = delete;
    generated_fast_router& operator=(const generated_fast_router&) = delete;
    generated_fast_router(generated_fast_router&&) = delete;
    generated_fast_router& operator=(generated_fast_router&&) = delete;

    katana::result<void> dispatch_to(
        const katana::http::request& req,
        katana::http::request_context& ctx,
        katana::http::response& out) const {
        return fast_router_.dispatch_to(req, ctx, out);
    }

    katana::result<katana::http::response> operator()(
        const katana::http::request& req,
        katana::http::request_context& ctx) const {
        return fast_router_(req, ctx);
    }

    [[nodiscard]] const generated_router& bundle() const noexcept { return router_bundle_; }

private:
    generated_router router_bundle_;
    fast_router fast_router_;
};

inline generated_fast_router make_fast_router(api_handler& handler) {
    return generated_fast_router(handler);
}

// Zero-boilerplate server creation
// Usage: return generated::serve<MyHandler>(8080);
template<typename Handler>
class generated_server {
public:
    template<typename... Args>
    explicit generated_server(Args&&... args)
        : handler_(std::forward<Args>(args)...),
          router_bundle_(handler_),
          server_(router_bundle_.router()) {}

    generated_server(const generated_server&) = delete;
    generated_server& operator=(const generated_server&) = delete;
    generated_server(generated_server&&) = delete;
    generated_server& operator=(generated_server&&) = delete;

    generated_server& bind(const std::string& host, uint16_t port) {
        server_.bind(host, port);
        return *this;
    }
    generated_server& listen(uint16_t port) {
        server_.listen(port);
        return *this;
    }
    generated_server& workers(size_t count) {
        server_.workers(count);
        return *this;
    }
    generated_server& backlog(int32_t size) {
        server_.backlog(size);
        return *this;
    }
    generated_server& reuseport(bool enable = true) {
        server_.reuseport(enable);
        return *this;
    }
    generated_server& graceful_shutdown(std::chrono::milliseconds timeout) {
        server_.graceful_shutdown(timeout);
        return *this;
    }
    generated_server& on_start(std::function<void()> callback) {
        server_.on_start(std::move(callback));
        return *this;
    }
    generated_server& on_stop(std::function<void()> callback) {
        server_.on_stop(std::move(callback));
        return *this;
    }
    generated_server& on_request(std::function<void(const request&, const response&)> callback) {
        server_.on_request(std::move(callback));
        return *this;
    }
    [[nodiscard]] Handler& handler() noexcept { return handler_; }
    [[nodiscard]] const Handler& handler() const noexcept { return handler_; }
    [[nodiscard]] katana::http::server& server() noexcept { return server_; }
    [[nodiscard]] const katana::http::server& server() const noexcept { return server_; }
    int run() { return server_.run(); }

private:
    Handler handler_;
    generated_router router_bundle_;
    katana::http::server server_;
};

template<typename Handler, typename... Args>
inline generated_server<Handler> make_server(Args&&... args) {
    return generated_server<Handler>(std::forward<Args>(args)...);
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
