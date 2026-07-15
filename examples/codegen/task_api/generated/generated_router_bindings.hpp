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
constexpr uint64_t HASH_LIST_TASKS = hash_string("/tasks");
constexpr uint64_t HASH_BATCH_CREATE_TASKS = hash_string("/tasks/batch");
constexpr uint64_t HASH_SEARCH_TASKS = hash_string("/tasks/search");
constexpr uint64_t HASH_HEALTH_CHECK = hash_string("/health");

// ============================================================
// Route Dispatch Functions
// ============================================================

// Dispatch for /tasks
inline katana::result<void> dispatch_list_tasks(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    constexpr std::string_view kJsonContentType = "application/json";
    auto accept = req.headers.get(katana::http::field::accept);
    if (accept && !accept->empty() && *accept != "*/*" && *accept != kJsonContentType) {
        out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
        return {};
    }
    std::optional<std::string_view> p_status = std::nullopt;
    std::optional<std::string_view> p_priority = std::nullopt;
    std::optional<std::string_view> p_limit = std::nullopt;
    std::optional<std::string_view> p_offset = std::nullopt;
    extract_query_params(req.uri, std::array{
        named_param_target{"status", &p_status},
        named_param_target{"priority", &p_priority},
        named_param_target{"limit", &p_limit},
        named_param_target{"offset", &p_offset},
    });
    if (p_status) *p_status = katana::http_utils::percent_decode_view(*p_status, ctx.arena);
    std::optional<std::string_view> status = std::nullopt;
    if (p_status) status = *p_status;
    if (p_priority) *p_priority = katana::http_utils::percent_decode_view(*p_priority, ctx.arena);
    std::optional<int64_t> priority;
    if (p_priority) {
        int64_t tmp = 0;
        auto [ptr, ec] = std::from_chars(p_priority->data(), p_priority->data() + p_priority->size(), tmp);
        if (ec != std::errc() || ptr != p_priority->data() + p_priority->size()) { out = katana::http::response::error(katana::problem_details::bad_request("invalid param priority")); return {}; }
        priority = tmp;
    }
    if (p_limit) *p_limit = katana::http_utils::percent_decode_view(*p_limit, ctx.arena);
    std::optional<int64_t> limit;
    if (p_limit) {
        int64_t tmp = 0;
        auto [ptr, ec] = std::from_chars(p_limit->data(), p_limit->data() + p_limit->size(), tmp);
        if (ec != std::errc() || ptr != p_limit->data() + p_limit->size()) { out = katana::http::response::error(katana::problem_details::bad_request("invalid param limit")); return {}; }
        limit = tmp;
    }
    if (p_offset) *p_offset = katana::http_utils::percent_decode_view(*p_offset, ctx.arena);
    std::optional<int64_t> offset;
    if (p_offset) {
        int64_t tmp = 0;
        auto [ptr, ec] = std::from_chars(p_offset->data(), p_offset->data() + p_offset->size(), tmp);
        if (ec != std::errc() || ptr != p_offset->data() + p_offset->size()) { out = katana::http::response::error(katana::problem_details::bad_request("invalid param offset")); return {}; }
        offset = tmp;
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    if (ctx.can_defer_response()) {
        if (auto* async_handler = dynamic_cast<async_api_handler*>(&handler)) {
            auto async_out = katana::http::async_response_writer(ctx.share_deferred_response(), kJsonContentType);
            if (async_out) {
                if (async_handler->list_tasks_async(status, priority, limit, offset, async_out)) {
                    return {};
                }
                async_out.disarm();
                ctx.reset_deferred_response();
            }
        }
    }
    auto handler_result = handler.list_tasks(status, priority, limit, offset, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (out.status != 204 && !out.body.empty() && !out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", kJsonContentType);
    }
    return {};
}

// Dispatch for /tasks
inline katana::result<void> dispatch_create_task(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    constexpr std::string_view kJsonContentType = "application/json";
    auto accept = req.headers.get(katana::http::field::accept);
    if (accept && !accept->empty() && *accept != "*/*" && *accept != kJsonContentType) {
        out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
        return {};
    }
    auto content_type = req.headers.get(katana::http::field::content_type);
    if (!content_type || !katana::http_utils::detail::ascii_iequals(
                             katana::http_utils::detail::media_type_token(*content_type),
                             kJsonContentType)) {
        out.assign_error(katana::problem_details::unsupported_media_type("unsupported Content-Type")); return {};
    }
    auto parsed_body = parse_CreateTaskRequest(req.body, &ctx.arena);
    if (!parsed_body) {
        out.assign_error(katana::problem_details::bad_request("invalid request body")); return {};
    }

    // Automatic validation (optimized: single allocation)
    if (auto validation_error = validate_CreateTaskRequest(*parsed_body)) {
        format_validation_error_into(out, *validation_error);
        return {};
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    if (ctx.can_defer_response()) {
        if (auto* async_handler = dynamic_cast<async_api_handler*>(&handler)) {
            auto async_out = katana::http::async_response_writer(ctx.share_deferred_response(), kJsonContentType);
            if (async_out) {
                if (async_handler->create_task_async(*parsed_body, async_out)) {
                    return {};
                }
                async_out.disarm();
                ctx.reset_deferred_response();
            }
        }
    }
    auto handler_result = handler.create_task(*parsed_body, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (out.status != 204 && !out.body.empty() && !out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", kJsonContentType);
    }
    return {};
}

// Dispatch for /tasks/{id}
inline katana::result<void> dispatch_get_task(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    constexpr std::string_view kJsonContentType = "application/json";
    auto accept = req.headers.get(katana::http::field::accept);
    if (accept && !accept->empty() && *accept != "*/*" && *accept != kJsonContentType) {
        out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
        return {};
    }
    auto p_id = ctx.params.get("id");
    if (!p_id) { out = katana::http::response::error(katana::problem_details::bad_request("missing path param id")); return {}; }
    *p_id = katana::http_utils::percent_decode_view(*p_id, ctx.arena);
    int64_t id = 0;
    {
        auto [ptr, ec] = std::from_chars(p_id->data(), p_id->data() + p_id->size(), id);
        if (ec != std::errc() || ptr != p_id->data() + p_id->size()) { out = katana::http::response::error(katana::problem_details::bad_request("invalid path param id")); return {}; }
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    if (ctx.can_defer_response()) {
        if (auto* async_handler = dynamic_cast<async_api_handler*>(&handler)) {
            auto async_out = katana::http::async_response_writer(ctx.share_deferred_response(), kJsonContentType);
            if (async_out) {
                if (async_handler->get_task_async(id, async_out)) {
                    return {};
                }
                async_out.disarm();
                ctx.reset_deferred_response();
            }
        }
    }
    auto handler_result = handler.get_task(id, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (out.status != 204 && !out.body.empty() && !out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", kJsonContentType);
    }
    return {};
}

// Dispatch for /tasks/{id}
inline katana::result<void> dispatch_update_task(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    constexpr std::string_view kJsonContentType = "application/json";
    auto accept = req.headers.get(katana::http::field::accept);
    if (accept && !accept->empty() && *accept != "*/*" && *accept != kJsonContentType) {
        out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
        return {};
    }
    auto p_id = ctx.params.get("id");
    if (!p_id) { out = katana::http::response::error(katana::problem_details::bad_request("missing path param id")); return {}; }
    *p_id = katana::http_utils::percent_decode_view(*p_id, ctx.arena);
    int64_t id = 0;
    {
        auto [ptr, ec] = std::from_chars(p_id->data(), p_id->data() + p_id->size(), id);
        if (ec != std::errc() || ptr != p_id->data() + p_id->size()) { out = katana::http::response::error(katana::problem_details::bad_request("invalid path param id")); return {}; }
    }
    auto content_type = req.headers.get(katana::http::field::content_type);
    if (!content_type || !katana::http_utils::detail::ascii_iequals(
                             katana::http_utils::detail::media_type_token(*content_type),
                             kJsonContentType)) {
        out.assign_error(katana::problem_details::unsupported_media_type("unsupported Content-Type")); return {};
    }
    auto parsed_body = parse_UpdateTaskRequest(req.body, &ctx.arena);
    if (!parsed_body) {
        out.assign_error(katana::problem_details::bad_request("invalid request body")); return {};
    }

    // Automatic validation (optimized: single allocation)
    if (auto validation_error = validate_UpdateTaskRequest(*parsed_body)) {
        format_validation_error_into(out, *validation_error);
        return {};
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    if (ctx.can_defer_response()) {
        if (auto* async_handler = dynamic_cast<async_api_handler*>(&handler)) {
            auto async_out = katana::http::async_response_writer(ctx.share_deferred_response(), kJsonContentType);
            if (async_out) {
                if (async_handler->update_task_async(id, *parsed_body, async_out)) {
                    return {};
                }
                async_out.disarm();
                ctx.reset_deferred_response();
            }
        }
    }
    auto handler_result = handler.update_task(id, *parsed_body, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (out.status != 204 && !out.body.empty() && !out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", kJsonContentType);
    }
    return {};
}

// Dispatch for /tasks/{id}
inline katana::result<void> dispatch_delete_task(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    constexpr std::string_view kJsonContentType = "application/json";
    auto accept = req.headers.get(katana::http::field::accept);
    if (accept && !accept->empty() && *accept != "*/*" && *accept != kJsonContentType) {
        out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
        return {};
    }
    auto p_id = ctx.params.get("id");
    if (!p_id) { out = katana::http::response::error(katana::problem_details::bad_request("missing path param id")); return {}; }
    *p_id = katana::http_utils::percent_decode_view(*p_id, ctx.arena);
    int64_t id = 0;
    {
        auto [ptr, ec] = std::from_chars(p_id->data(), p_id->data() + p_id->size(), id);
        if (ec != std::errc() || ptr != p_id->data() + p_id->size()) { out = katana::http::response::error(katana::problem_details::bad_request("invalid path param id")); return {}; }
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    if (ctx.can_defer_response()) {
        if (auto* async_handler = dynamic_cast<async_api_handler*>(&handler)) {
            auto async_out = katana::http::async_response_writer(ctx.share_deferred_response(), kJsonContentType);
            if (async_out) {
                if (async_handler->delete_task_async(id, async_out)) {
                    return {};
                }
                async_out.disarm();
                ctx.reset_deferred_response();
            }
        }
    }
    auto handler_result = handler.delete_task(id, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (out.status != 204 && !out.body.empty() && !out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", kJsonContentType);
    }
    return {};
}

// Dispatch for /tasks/batch
inline katana::result<void> dispatch_batch_create_tasks(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    constexpr std::string_view kJsonContentType = "application/json";
    auto accept = req.headers.get(katana::http::field::accept);
    if (accept && !accept->empty() && *accept != "*/*" && *accept != kJsonContentType) {
        out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
        return {};
    }
    auto content_type = req.headers.get(katana::http::field::content_type);
    if (!content_type || !katana::http_utils::detail::ascii_iequals(
                             katana::http_utils::detail::media_type_token(*content_type),
                             kJsonContentType)) {
        out.assign_error(katana::problem_details::unsupported_media_type("unsupported Content-Type")); return {};
    }
    auto parsed_body = parse_BatchCreateRequest(req.body, &ctx.arena);
    if (!parsed_body) {
        out.assign_error(katana::problem_details::bad_request("invalid request body")); return {};
    }

    // Automatic validation (optimized: single allocation)
    if (auto validation_error = validate_BatchCreateRequest(*parsed_body)) {
        format_validation_error_into(out, *validation_error);
        return {};
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    if (ctx.can_defer_response()) {
        if (auto* async_handler = dynamic_cast<async_api_handler*>(&handler)) {
            auto async_out = katana::http::async_response_writer(ctx.share_deferred_response(), kJsonContentType);
            if (async_out) {
                if (async_handler->batch_create_tasks_async(*parsed_body, async_out)) {
                    return {};
                }
                async_out.disarm();
                ctx.reset_deferred_response();
            }
        }
    }
    auto handler_result = handler.batch_create_tasks(*parsed_body, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (out.status != 204 && !out.body.empty() && !out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", kJsonContentType);
    }
    return {};
}

// Dispatch for /tasks/search
inline katana::result<void> dispatch_search_tasks(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    constexpr std::string_view kJsonContentType = "application/json";
    auto accept = req.headers.get(katana::http::field::accept);
    if (accept && !accept->empty() && *accept != "*/*" && *accept != kJsonContentType) {
        out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
        return {};
    }
    auto content_type = req.headers.get(katana::http::field::content_type);
    if (!content_type || !katana::http_utils::detail::ascii_iequals(
                             katana::http_utils::detail::media_type_token(*content_type),
                             kJsonContentType)) {
        out.assign_error(katana::problem_details::unsupported_media_type("unsupported Content-Type")); return {};
    }
    auto parsed_body = parse_SearchRequest(req.body, &ctx.arena);
    if (!parsed_body) {
        out.assign_error(katana::problem_details::bad_request("invalid request body")); return {};
    }

    // Automatic validation (optimized: single allocation)
    if (auto validation_error = validate_SearchRequest(*parsed_body)) {
        format_validation_error_into(out, *validation_error);
        return {};
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    if (ctx.can_defer_response()) {
        if (auto* async_handler = dynamic_cast<async_api_handler*>(&handler)) {
            auto async_out = katana::http::async_response_writer(ctx.share_deferred_response(), kJsonContentType);
            if (async_out) {
                if (async_handler->search_tasks_async(*parsed_body, async_out)) {
                    return {};
                }
                async_out.disarm();
                ctx.reset_deferred_response();
            }
        }
    }
    auto handler_result = handler.search_tasks(*parsed_body, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (out.status != 204 && !out.body.empty() && !out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", kJsonContentType);
    }
    return {};
}

// Dispatch for /health
inline katana::result<void> dispatch_health_check(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    constexpr std::string_view kJsonContentType = "application/json";
    auto accept = req.headers.get(katana::http::field::accept);
    if (accept && !accept->empty() && *accept != "*/*" && *accept != kJsonContentType) {
        out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
        return {};
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    if (ctx.can_defer_response()) {
        if (auto* async_handler = dynamic_cast<async_api_handler*>(&handler)) {
            auto async_out = katana::http::async_response_writer(ctx.share_deferred_response(), kJsonContentType);
            if (async_out) {
                if (async_handler->health_check_async(async_out)) {
                    return {};
                }
                async_out.disarm();
                ctx.reset_deferred_response();
            }
        }
    }
    auto handler_result = handler.health_check(out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (out.status != 204 && !out.body.empty() && !out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", kJsonContentType);
    }
    return {};
}

// ============================================================
// Router Configuration
// ============================================================

class generated_router {
public:
    explicit generated_router(api_handler& handler)
        : route_policies_{
        katana::http::route_policy_view{katana::http::route_cache_policy_view{katana::http::route_cache_policy_kind::none, std::string_view{}}, katana::http::route_alloc_policy_view{katana::http::route_alloc_policy_kind::none, std::string_view{}, std::nullopt}, katana::http::route_rate_limit_policy_view{false, std::string_view{}, std::nullopt, katana::http::route_rate_limit_unit::unknown}, katana::http::route_idempotency_policy_view{katana::http::route_idempotency_policy_kind::none, std::string_view{}}, "listTasks", false, std::string_view{}},
        katana::http::route_policy_view{katana::http::route_cache_policy_view{katana::http::route_cache_policy_kind::none, std::string_view{}}, katana::http::route_alloc_policy_view{katana::http::route_alloc_policy_kind::none, std::string_view{}, std::nullopt}, katana::http::route_rate_limit_policy_view{false, std::string_view{}, std::nullopt, katana::http::route_rate_limit_unit::unknown}, katana::http::route_idempotency_policy_view{katana::http::route_idempotency_policy_kind::none, std::string_view{}}, "createTask", false, std::string_view{}},
        katana::http::route_policy_view{katana::http::route_cache_policy_view{katana::http::route_cache_policy_kind::none, std::string_view{}}, katana::http::route_alloc_policy_view{katana::http::route_alloc_policy_kind::none, std::string_view{}, std::nullopt}, katana::http::route_rate_limit_policy_view{false, std::string_view{}, std::nullopt, katana::http::route_rate_limit_unit::unknown}, katana::http::route_idempotency_policy_view{katana::http::route_idempotency_policy_kind::none, std::string_view{}}, "getTask", false, std::string_view{}},
        katana::http::route_policy_view{katana::http::route_cache_policy_view{katana::http::route_cache_policy_kind::none, std::string_view{}}, katana::http::route_alloc_policy_view{katana::http::route_alloc_policy_kind::none, std::string_view{}, std::nullopt}, katana::http::route_rate_limit_policy_view{false, std::string_view{}, std::nullopt, katana::http::route_rate_limit_unit::unknown}, katana::http::route_idempotency_policy_view{katana::http::route_idempotency_policy_kind::none, std::string_view{}}, "updateTask", false, std::string_view{}},
        katana::http::route_policy_view{katana::http::route_cache_policy_view{katana::http::route_cache_policy_kind::none, std::string_view{}}, katana::http::route_alloc_policy_view{katana::http::route_alloc_policy_kind::none, std::string_view{}, std::nullopt}, katana::http::route_rate_limit_policy_view{false, std::string_view{}, std::nullopt, katana::http::route_rate_limit_unit::unknown}, katana::http::route_idempotency_policy_view{katana::http::route_idempotency_policy_kind::none, std::string_view{}}, "deleteTask", false, std::string_view{}},
        katana::http::route_policy_view{katana::http::route_cache_policy_view{katana::http::route_cache_policy_kind::none, std::string_view{}}, katana::http::route_alloc_policy_view{katana::http::route_alloc_policy_kind::none, std::string_view{}, std::nullopt}, katana::http::route_rate_limit_policy_view{false, std::string_view{}, std::nullopt, katana::http::route_rate_limit_unit::unknown}, katana::http::route_idempotency_policy_view{katana::http::route_idempotency_policy_kind::none, std::string_view{}}, "batchCreateTasks", false, std::string_view{}},
        katana::http::route_policy_view{katana::http::route_cache_policy_view{katana::http::route_cache_policy_kind::none, std::string_view{}}, katana::http::route_alloc_policy_view{katana::http::route_alloc_policy_kind::none, std::string_view{}, std::nullopt}, katana::http::route_rate_limit_policy_view{false, std::string_view{}, std::nullopt, katana::http::route_rate_limit_unit::unknown}, katana::http::route_idempotency_policy_view{katana::http::route_idempotency_policy_kind::none, std::string_view{}}, "searchTasks", false, std::string_view{}},
        katana::http::route_policy_view{katana::http::route_cache_policy_view{katana::http::route_cache_policy_kind::none, std::string_view{}}, katana::http::route_alloc_policy_view{katana::http::route_alloc_policy_kind::none, std::string_view{}, std::nullopt}, katana::http::route_rate_limit_policy_view{false, std::string_view{}, std::nullopt, katana::http::route_rate_limit_unit::unknown}, katana::http::route_idempotency_policy_view{katana::http::route_idempotency_policy_kind::none, std::string_view{}}, "healthCheck", false, std::string_view{}},
        }, route_entries_{
        katana::http::route_entry{katana::http::method::get,
                   katana::http::path_pattern::from_literal<"/tasks">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_list_tasks(req, ctx, *handler_ptr, out);
                   })
                   , katana::http::middleware_chain{}
                   , &route_policies_[0]
        },
        katana::http::route_entry{katana::http::method::post,
                   katana::http::path_pattern::from_literal<"/tasks">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_create_task(req, ctx, *handler_ptr, out);
                   })
                   , katana::http::middleware_chain{}
                   , &route_policies_[1]
        },
        katana::http::route_entry{katana::http::method::get,
                   katana::http::path_pattern::from_literal<"/tasks/{id}">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_get_task(req, ctx, *handler_ptr, out);
                   })
                   , katana::http::middleware_chain{}
                   , &route_policies_[2]
        },
        katana::http::route_entry{katana::http::method::put,
                   katana::http::path_pattern::from_literal<"/tasks/{id}">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_update_task(req, ctx, *handler_ptr, out);
                   })
                   , katana::http::middleware_chain{}
                   , &route_policies_[3]
        },
        katana::http::route_entry{katana::http::method::del,
                   katana::http::path_pattern::from_literal<"/tasks/{id}">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_delete_task(req, ctx, *handler_ptr, out);
                   })
                   , katana::http::middleware_chain{}
                   , &route_policies_[4]
        },
        katana::http::route_entry{katana::http::method::post,
                   katana::http::path_pattern::from_literal<"/tasks/batch">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_batch_create_tasks(req, ctx, *handler_ptr, out);
                   })
                   , katana::http::middleware_chain{}
                   , &route_policies_[5]
        },
        katana::http::route_entry{katana::http::method::post,
                   katana::http::path_pattern::from_literal<"/tasks/search">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_search_tasks(req, ctx, *handler_ptr, out);
                   })
                   , katana::http::middleware_chain{}
                   , &route_policies_[6]
        },
        katana::http::route_entry{katana::http::method::get,
                   katana::http::path_pattern::from_literal<"/health">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_health_check(req, ctx, *handler_ptr, out);
                   })
                   , katana::http::middleware_chain{}
                   , &route_policies_[7]
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
    [[nodiscard]] std::span<const katana::http::route_policy_view> route_policies() const noexcept {
        return std::span<const katana::http::route_policy_view>(route_policies_.data(), route_policies_.size());
    }
    [[nodiscard]] operator const katana::http::router&() const noexcept { return *router_; }
    [[nodiscard]] operator katana::http::router&() noexcept { return *router_; }

private:
    std::array<katana::http::route_policy_view, route_count> route_policies_;
    std::array<katana::http::route_entry, route_count> route_entries_;
    std::optional<katana::http::router> router_;
};

inline generated_router make_router(api_handler& handler) {
    return generated_router(handler);
}

// Optimized router with hash-based O(1) dispatch for static routes
class fast_router {
public:
    explicit fast_router(api_handler& handler,
                         const katana::http::router& fallback,
                         std::span<const katana::http::route_policy_view> route_policies)
        : handler_(handler), fallback_router_(fallback), route_policies_(route_policies) {}

    katana::result<void> dispatch_to(
        const katana::http::request& req,
        katana::http::request_context& ctx,
        katana::http::response& out) const {
        ctx.route_policy = nullptr;
        // Strip query string for matching
        std::string_view path = req.uri;
        auto query_pos = path.find('?');
        if (query_pos != std::string_view::npos) {
            path = path.substr(0, query_pos);
        }

        // Fast path: O(1) hash-based dispatch for static routes
        uint64_t path_hash = hash_string(path);
        switch (path_hash) {
            case HASH_LIST_TASKS:
                if (path == "/tasks") {
                    if (req.http_method == katana::http::method::get)
                        { ctx.params.reset(); ctx.route_policy = &route_policies_[0]; auto policy_result = katana::http::apply_route_policy_executor(req, ctx, out); if (!policy_result) return std::unexpected(policy_result.error()); if (!*policy_result) return {}; auto dispatch_result = dispatch_list_tasks(req, ctx, handler_, out); if (!dispatch_result) return dispatch_result; auto after_result = katana::http::apply_route_policy_after_dispatch(req, ctx, out); if (!after_result) return after_result; return {}; }
                    if (req.http_method == katana::http::method::post)
                        { ctx.params.reset(); ctx.route_policy = &route_policies_[1]; auto policy_result = katana::http::apply_route_policy_executor(req, ctx, out); if (!policy_result) return std::unexpected(policy_result.error()); if (!*policy_result) return {}; auto dispatch_result = dispatch_create_task(req, ctx, handler_, out); if (!dispatch_result) return dispatch_result; auto after_result = katana::http::apply_route_policy_after_dispatch(req, ctx, out); if (!after_result) return after_result; return {}; }
                }
                break;
            case HASH_BATCH_CREATE_TASKS:
                if (path == "/tasks/batch") {
                    if (req.http_method == katana::http::method::post)
                        { ctx.params.reset(); ctx.route_policy = &route_policies_[5]; auto policy_result = katana::http::apply_route_policy_executor(req, ctx, out); if (!policy_result) return std::unexpected(policy_result.error()); if (!*policy_result) return {}; auto dispatch_result = dispatch_batch_create_tasks(req, ctx, handler_, out); if (!dispatch_result) return dispatch_result; auto after_result = katana::http::apply_route_policy_after_dispatch(req, ctx, out); if (!after_result) return after_result; return {}; }
                }
                break;
            case HASH_SEARCH_TASKS:
                if (path == "/tasks/search") {
                    if (req.http_method == katana::http::method::post)
                        { ctx.params.reset(); ctx.route_policy = &route_policies_[6]; auto policy_result = katana::http::apply_route_policy_executor(req, ctx, out); if (!policy_result) return std::unexpected(policy_result.error()); if (!*policy_result) return {}; auto dispatch_result = dispatch_search_tasks(req, ctx, handler_, out); if (!dispatch_result) return dispatch_result; auto after_result = katana::http::apply_route_policy_after_dispatch(req, ctx, out); if (!after_result) return after_result; return {}; }
                }
                break;
            case HASH_HEALTH_CHECK:
                if (path == "/health") {
                    if (req.http_method == katana::http::method::get)
                        { ctx.params.reset(); ctx.route_policy = &route_policies_[7]; auto policy_result = katana::http::apply_route_policy_executor(req, ctx, out); if (!policy_result) return std::unexpected(policy_result.error()); if (!*policy_result) return {}; auto dispatch_result = dispatch_health_check(req, ctx, handler_, out); if (!dispatch_result) return dispatch_result; auto after_result = katana::http::apply_route_policy_after_dispatch(req, ctx, out); if (!after_result) return after_result; return {}; }
                }
                break;
            default:
                break;
        }

        // Fallback to standard router for dynamic routes (path params), hash
        // collisions, and method mismatches. Use dispatch_with_info +
        // map_dispatch_error so a 405 keeps its Allow header (plain dispatch()
        // drops the allowed-methods mask, violating RFC 7231).
        auto fallback_info_ = fallback_router_.dispatch_with_info(req, ctx, out);
        if (fallback_info_.has_error) {
            katana::http::map_dispatch_error(fallback_info_, out);
        }
        return {};
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
    std::span<const katana::http::route_policy_view> route_policies_;
};

// Create optimized router (recommended for production)
class generated_fast_router {
public:
    explicit generated_fast_router(api_handler& handler)
        : router_bundle_(handler), fast_router_(handler, router_bundle_.router(), router_bundle_.route_policies()) {}

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
          server_(router_bundle_) {}

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
    generated_fast_router router_bundle_;
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
