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
using katana::http_utils::format_validation_error_into;
using katana::http_utils::hash_string;
using katana::http_utils::content_type_info;

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
    constexpr std::string_view response_content_type = "application/json";
    auto accept_header = req.headers.get(katana::http::field::accept);
    if (accept_header && !accept_header->empty() && *accept_header != "*/*" && *accept_header != response_content_type) {
        auto comma = accept_header->find(',');
        auto semicolon = accept_header->find(';');
        auto token_end = std::min(comma, semicolon);
        auto simple_accept = accept_header->substr(0, token_end);
        if (simple_accept != response_content_type) {
            out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
            return {};
        }
    }
    std::optional<std::string_view> p_status = std::nullopt;
    std::optional<std::string_view> p_priority = std::nullopt;
    std::optional<std::string_view> p_limit = std::nullopt;
    std::optional<std::string_view> p_offset = std::nullopt;
    auto query_view = req.uri;
    auto query_qpos = query_view.find('?');
    if (query_qpos != std::string_view::npos) {
        query_view = query_view.substr(query_qpos + 1);
        while (!query_view.empty()) {
            auto amp = query_view.find('&');
            auto part = query_view.substr(0, amp);
            auto eq = part.find('=');
            auto name = part.substr(0, eq);
            auto value = eq == std::string_view::npos ? std::string_view{} : part.substr(eq + 1);
            if (!p_status && name == "status") {
                p_status = value;
            }
            else if (!p_priority && name == "priority") {
                p_priority = value;
            }
            else if (!p_limit && name == "limit") {
                p_limit = value;
            }
            else if (!p_offset && name == "offset") {
                p_offset = value;
            }
            if (p_status && p_priority && p_limit && p_offset) {
                break;
            }
            if (amp == std::string_view::npos) {
                break;
            }
            query_view.remove_prefix(amp + 1);
        }
    }
    std::optional<std::string_view> status = std::nullopt;
    if (p_status) status = *p_status;
    std::optional<int64_t> priority;
    if (p_priority) {
        int64_t tmp = 0;
        auto [ptr, ec] = std::from_chars(p_priority->data(), p_priority->data() + p_priority->size(), tmp);
        if (ec != std::errc() || ptr != p_priority->data() + p_priority->size()) { out = katana::http::response::error(katana::problem_details::bad_request("invalid param priority")); return {}; }
        priority = tmp;
    }
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
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto handler_result = handler.list_tasks(status, priority, limit, offset, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (!out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", response_content_type);
    }
    return {};
}

// Dispatch for /tasks
inline katana::result<void> dispatch_create_task(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    constexpr std::string_view response_content_type = "application/json";
    auto accept_header = req.headers.get(katana::http::field::accept);
    if (accept_header && !accept_header->empty() && *accept_header != "*/*" && *accept_header != response_content_type) {
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
    auto handler_result = handler.create_task(*parsed_body, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (!out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", response_content_type);
    }
    return {};
}

// Dispatch for /tasks/{id}
inline katana::result<void> dispatch_get_task(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    constexpr std::string_view response_content_type = "application/json";
    auto accept_header = req.headers.get(katana::http::field::accept);
    if (accept_header && !accept_header->empty() && *accept_header != "*/*" && *accept_header != response_content_type) {
        auto comma = accept_header->find(',');
        auto semicolon = accept_header->find(';');
        auto token_end = std::min(comma, semicolon);
        auto simple_accept = accept_header->substr(0, token_end);
        if (simple_accept != response_content_type) {
            out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
            return {};
        }
    }
    auto p_id = ctx.params.get("id");
    if (!p_id) { out = katana::http::response::error(katana::problem_details::bad_request("missing path param id")); return {}; }
    int64_t id = 0;
    {
        auto [ptr, ec] = std::from_chars(p_id->data(), p_id->data() + p_id->size(), id);
        if (ec != std::errc() || ptr != p_id->data() + p_id->size()) { out = katana::http::response::error(katana::problem_details::bad_request("invalid path param id")); return {}; }
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto handler_result = handler.get_task(id, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (!out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", response_content_type);
    }
    return {};
}

// Dispatch for /tasks/{id}
inline katana::result<void> dispatch_update_task(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    constexpr std::string_view response_content_type = "application/json";
    auto accept_header = req.headers.get(katana::http::field::accept);
    if (accept_header && !accept_header->empty() && *accept_header != "*/*" && *accept_header != response_content_type) {
        auto comma = accept_header->find(',');
        auto semicolon = accept_header->find(';');
        auto token_end = std::min(comma, semicolon);
        auto simple_accept = accept_header->substr(0, token_end);
        if (simple_accept != response_content_type) {
            out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
            return {};
        }
    }
    auto p_id = ctx.params.get("id");
    if (!p_id) { out = katana::http::response::error(katana::problem_details::bad_request("missing path param id")); return {}; }
    int64_t id = 0;
    {
        auto [ptr, ec] = std::from_chars(p_id->data(), p_id->data() + p_id->size(), id);
        if (ec != std::errc() || ptr != p_id->data() + p_id->size()) { out = katana::http::response::error(katana::problem_details::bad_request("invalid path param id")); return {}; }
    }
    auto content_type = req.headers.get(katana::http::field::content_type);
    if (!content_type || content_type->substr(0, 16) != "application/json") {
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
    auto handler_result = handler.update_task(id, *parsed_body, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (!out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", response_content_type);
    }
    return {};
}

// Dispatch for /tasks/{id}
inline katana::result<void> dispatch_delete_task(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    constexpr std::string_view response_content_type = "application/json";
    auto accept_header = req.headers.get(katana::http::field::accept);
    if (accept_header && !accept_header->empty() && *accept_header != "*/*" && *accept_header != response_content_type) {
        auto comma = accept_header->find(',');
        auto semicolon = accept_header->find(';');
        auto token_end = std::min(comma, semicolon);
        auto simple_accept = accept_header->substr(0, token_end);
        if (simple_accept != response_content_type) {
            out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
            return {};
        }
    }
    auto p_id = ctx.params.get("id");
    if (!p_id) { out = katana::http::response::error(katana::problem_details::bad_request("missing path param id")); return {}; }
    int64_t id = 0;
    {
        auto [ptr, ec] = std::from_chars(p_id->data(), p_id->data() + p_id->size(), id);
        if (ec != std::errc() || ptr != p_id->data() + p_id->size()) { out = katana::http::response::error(katana::problem_details::bad_request("invalid path param id")); return {}; }
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto handler_result = handler.delete_task(id, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (!out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", response_content_type);
    }
    return {};
}

// Dispatch for /tasks/batch
inline katana::result<void> dispatch_batch_create_tasks(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    constexpr std::string_view response_content_type = "application/json";
    auto accept_header = req.headers.get(katana::http::field::accept);
    if (accept_header && !accept_header->empty() && *accept_header != "*/*" && *accept_header != response_content_type) {
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
    auto handler_result = handler.batch_create_tasks(*parsed_body, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (!out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", response_content_type);
    }
    return {};
}

// Dispatch for /tasks/search
inline katana::result<void> dispatch_search_tasks(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    constexpr std::string_view response_content_type = "application/json";
    auto accept_header = req.headers.get(katana::http::field::accept);
    if (accept_header && !accept_header->empty() && *accept_header != "*/*" && *accept_header != response_content_type) {
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
    auto handler_result = handler.search_tasks(*parsed_body, out);
    if (!handler_result) {
        return std::unexpected(handler_result.error());
    }
    if (!out.headers.get(katana::http::field::content_type)) {
        out.set_header("Content-Type", response_content_type);
    }
    return {};
}

// Dispatch for /health
inline katana::result<void> dispatch_health_check(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
    constexpr std::string_view response_content_type = "application/json";
    auto accept_header = req.headers.get(katana::http::field::accept);
    if (accept_header && !accept_header->empty() && *accept_header != "*/*" && *accept_header != response_content_type) {
        auto comma = accept_header->find(',');
        auto semicolon = accept_header->find(';');
        auto token_end = std::min(comma, semicolon);
        auto simple_accept = accept_header->substr(0, token_end);
        if (simple_accept != response_content_type) {
            out.assign_error(katana::problem_details::not_acceptable("unsupported Accept header"));
            return {};
        }
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    auto handler_result = handler.health_check(out);
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
    using katana::http::route_entry;
    using katana::http::path_pattern;
    using katana::http::handler_fn;
    static std::array<route_entry, route_count> route_entries = {
        route_entry{katana::http::method::get,
                   katana::http::path_pattern::from_literal<"/tasks">(),
                   handler_fn([&handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_list_tasks(req, ctx, handler, out);
                   })
        },
        route_entry{katana::http::method::post,
                   katana::http::path_pattern::from_literal<"/tasks">(),
                   handler_fn([&handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_create_task(req, ctx, handler, out);
                   })
        },
        route_entry{katana::http::method::get,
                   katana::http::path_pattern::from_literal<"/tasks/{id}">(),
                   handler_fn([&handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_get_task(req, ctx, handler, out);
                   })
        },
        route_entry{katana::http::method::put,
                   katana::http::path_pattern::from_literal<"/tasks/{id}">(),
                   handler_fn([&handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_update_task(req, ctx, handler, out);
                   })
        },
        route_entry{katana::http::method::del,
                   katana::http::path_pattern::from_literal<"/tasks/{id}">(),
                   handler_fn([&handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_delete_task(req, ctx, handler, out);
                   })
        },
        route_entry{katana::http::method::post,
                   katana::http::path_pattern::from_literal<"/tasks/batch">(),
                   handler_fn([&handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_batch_create_tasks(req, ctx, handler, out);
                   })
        },
        route_entry{katana::http::method::post,
                   katana::http::path_pattern::from_literal<"/tasks/search">(),
                   handler_fn([&handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_search_tasks(req, ctx, handler, out);
                   })
        },
        route_entry{katana::http::method::get,
                   katana::http::path_pattern::from_literal<"/health">(),
                   handler_fn([&handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_health_check(req, ctx, handler, out);
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
            case HASH_LIST_TASKS:
                if (path == "/tasks") {
                    if (req.http_method == katana::http::method::get)
                        { return dispatch_list_tasks(req, ctx, handler_, out); }
                    if (req.http_method == katana::http::method::post)
                        { return dispatch_create_task(req, ctx, handler_, out); }
                }
                break;
            case HASH_BATCH_CREATE_TASKS:
                if (path == "/tasks/batch") {
                    if (req.http_method == katana::http::method::post)
                        { return dispatch_batch_create_tasks(req, ctx, handler_, out); }
                }
                break;
            case HASH_SEARCH_TASKS:
                if (path == "/tasks/search") {
                    if (req.http_method == katana::http::method::post)
                        { return dispatch_search_tasks(req, ctx, handler_, out); }
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
