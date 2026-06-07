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

// ============================================================
// Route Dispatch Functions
// ============================================================

// Dispatch for /compute/sum
inline katana::result<void> dispatch_compute_sum(const katana::http::request& req, katana::http::request_context& ctx, api_handler& handler, katana::http::response& out) {
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
    auto parsed_body = parse_compute_sum_request(req.body, &ctx.arena);
    if (!parsed_body) {
        out.assign_error(katana::problem_details::bad_request("invalid request body")); return {};
    }

    // Automatic validation (optimized: single allocation)
    if (auto validation_error = validate_compute_sum_request(*parsed_body)) {
        format_validation_error_into(out, *validation_error);
        return {};
    }
    // Set handler context for zero-boilerplate access
    katana::http::handler_context::scope context_scope(req, ctx);
    if (ctx.can_defer_response()) {
        if (auto* async_handler = dynamic_cast<async_api_handler*>(&handler)) {
            auto async_out = katana::http::async_response_writer(ctx.share_deferred_response(), kJsonContentType);
            if (async_out) {
                if (async_handler->compute_sum_async(*parsed_body, async_out)) {
                    return {};
                }
                async_out.disarm();
                ctx.reset_deferred_response();
            }
        }
    }
    auto handler_result = handler.compute_sum(*parsed_body, out);
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
        : route_entries_{
        katana::http::route_entry{katana::http::method::post,
                   katana::http::path_pattern::from_literal<"/compute/sum">(),
                   katana::http::handler_fn([handler_ptr = &handler](const katana::http::request& req, katana::http::request_context& ctx, katana::http::response& out) -> katana::result<void> {
                       return dispatch_compute_sum(req, ctx, *handler_ptr, out);
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
