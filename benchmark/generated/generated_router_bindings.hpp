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

inline std::optional<std::string_view> query_param(std::string_view uri, std::string_view key) {
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

inline std::optional<std::string_view> cookie_param(const katana::http::request& req,
                                                    std::string_view key) {
    auto cookie = req.headers.get("Cookie");
    if (!cookie)
        return std::nullopt;
    std::string_view rest = *cookie;
    while (!rest.empty()) {
        auto sep = rest.find(';');
        auto token = rest.substr(0, sep);
        if (sep != std::string_view::npos)
            rest.remove_prefix(sep + 1);
        auto eq = token.find('=');
        if (eq == std::string_view::npos)
            continue;
        auto name = katana::serde::trim_view(token.substr(0, eq));
        auto val = katana::serde::trim_view(token.substr(eq + 1));
        if (name == key)
            return val;
        if (sep == std::string_view::npos)
            break;
    }
    return std::nullopt;
}

inline std::optional<size_t> find_content_type(std::optional<std::string_view> header,
                                               std::span<const content_type_info> allowed) {
    if (allowed.empty())
        return std::nullopt;
    if (!header)
        return std::nullopt;
    for (size_t i = 0; i < allowed.size(); ++i) {
        auto& ct = allowed[i];
        if (header->substr(0, ct.mime_type.size()) == ct.mime_type)
            return i;
    }
    return std::nullopt;
}

inline std::optional<std::string_view>
negotiate_response_type(const katana::http::request& req,
                        std::span<const content_type_info> produces) {
    if (produces.empty())
        return std::nullopt;
    auto accept = req.headers.get("Accept");
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
        // Single value without q-factor
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
            auto prefix = token.substr(0, token.size() - 1); // keep trailing '/'
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

// Helper to format validation errors into problem details
inline katana::http::response format_validation_error(const validation_error& err) {
    std::string error_msg;
    error_msg.reserve(err.field.size() + err.message().size() + 2);
    error_msg.append(err.field);
    error_msg.append(": ");
    error_msg.append(err.message());
    return katana::http::response::error(
        katana::problem_details::bad_request(std::move(error_msg)));
}

inline const katana::http::router& make_router(api_handler& handler) {
    using katana::http::handler_fn;
    using katana::http::path_pattern;
    using katana::http::route_entry;
    static std::array<route_entry, route_count> route_entries = {
        route_entry{katana::http::method::get,
                    katana::http::path_pattern::from_literal<"/health">(),
                    handler_fn([&handler](const katana::http::request& req,
                                          katana::http::request_context& ctx)
                                   -> katana::result<katana::http::response> {
                        // Set handler context for zero-boilerplate access
                        katana::http::handler_context::scope context_scope(req, ctx);
                        auto generated_response = handler.health();
                        return generated_response;
                    })},
        route_entry{katana::http::method::get,
                    katana::http::path_pattern::from_literal<"/users">(),
                    handler_fn([&handler](const katana::http::request& req,
                                          katana::http::request_context& ctx)
                                   -> katana::result<katana::http::response> {
                        // Set handler context for zero-boilerplate access
                        katana::http::handler_context::scope context_scope(req, ctx);
                        auto generated_response = handler.list_users();
                        return generated_response;
                    })},
        route_entry{katana::http::method::post,
                    katana::http::path_pattern::from_literal<"/users">(),
                    handler_fn([&handler](const katana::http::request& req,
                                          katana::http::request_context& ctx)
                                   -> katana::result<katana::http::response> {
                        auto matched_ct =
                            find_content_type(req.headers.get("Content-Type"), route_2_consumes);
                        if (!matched_ct)
                            return katana::http::response::error(
                                katana::problem_details::unsupported_media_type(
                                    "unsupported Content-Type"));
                        std::optional<UserInput> parsed_body;
                        switch (*matched_ct) {
                        case 0: {
                            auto candidate = parse_UserInput(req.body, &ctx.arena);
                            if (!candidate)
                                return katana::http::response::error(
                                    katana::problem_details::bad_request("invalid request body"));
                            parsed_body = std::move(*candidate);
                            break;
                        }
                        default:
                            return katana::http::response::error(
                                katana::problem_details::unsupported_media_type(
                                    "unsupported Content-Type"));
                        }
                        // Automatic validation (optimized: single allocation)
                        if (auto validation_error = validate_UserInput(*parsed_body)) {
                            return format_validation_error(*validation_error);
                        }
                        // Set handler context for zero-boilerplate access
                        katana::http::handler_context::scope context_scope(req, ctx);
                        auto generated_response = handler.create_user(*parsed_body);
                        return generated_response;
                    })},
        route_entry{katana::http::method::get,
                    katana::http::path_pattern::from_literal<"/users/{id}">(),
                    handler_fn([&handler](const katana::http::request& req,
                                          katana::http::request_context& ctx)
                                   -> katana::result<katana::http::response> {
                        auto p_id = ctx.params.get("id");
                        if (!p_id)
                            return katana::http::response::error(
                                katana::problem_details::bad_request("missing path param id"));
                        int64_t id = 0;
                        {
                            auto [ptr, ec] =
                                std::from_chars(p_id->data(), p_id->data() + p_id->size(), id);
                            if (ec != std::errc())
                                return katana::http::response::error(
                                    katana::problem_details::bad_request("invalid path param id"));
                        }
                        // Set handler context for zero-boilerplate access
                        katana::http::handler_context::scope context_scope(req, ctx);
                        auto generated_response = handler.get_user(id);
                        return generated_response;
                    })},
        route_entry{katana::http::method::put,
                    katana::http::path_pattern::from_literal<"/users/{id}">(),
                    handler_fn([&handler](const katana::http::request& req,
                                          katana::http::request_context& ctx)
                                   -> katana::result<katana::http::response> {
                        auto p_id = ctx.params.get("id");
                        if (!p_id)
                            return katana::http::response::error(
                                katana::problem_details::bad_request("missing path param id"));
                        int64_t id = 0;
                        {
                            auto [ptr, ec] =
                                std::from_chars(p_id->data(), p_id->data() + p_id->size(), id);
                            if (ec != std::errc())
                                return katana::http::response::error(
                                    katana::problem_details::bad_request("invalid path param id"));
                        }
                        auto matched_ct =
                            find_content_type(req.headers.get("Content-Type"), route_4_consumes);
                        if (!matched_ct)
                            return katana::http::response::error(
                                katana::problem_details::unsupported_media_type(
                                    "unsupported Content-Type"));
                        std::optional<UserInput> parsed_body;
                        switch (*matched_ct) {
                        case 0: {
                            auto candidate = parse_UserInput(req.body, &ctx.arena);
                            if (!candidate)
                                return katana::http::response::error(
                                    katana::problem_details::bad_request("invalid request body"));
                            parsed_body = std::move(*candidate);
                            break;
                        }
                        default:
                            return katana::http::response::error(
                                katana::problem_details::unsupported_media_type(
                                    "unsupported Content-Type"));
                        }
                        // Automatic validation (optimized: single allocation)
                        if (auto validation_error = validate_UserInput(*parsed_body)) {
                            return format_validation_error(*validation_error);
                        }
                        // Set handler context for zero-boilerplate access
                        katana::http::handler_context::scope context_scope(req, ctx);
                        auto generated_response = handler.update_user(id, *parsed_body);
                        return generated_response;
                    })},
    };
    static katana::http::router router_instance(route_entries);
    return router_instance;
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
