#pragma once

#include "generated_handlers.hpp"
#include "generated_json.hpp"
#include "generated_routes.hpp"
#include "katana/core/problem.hpp"
#include "katana/core/router.hpp"
#include <array>
#include <charconv>
#include <string_view>

namespace generated {

inline katana::http::router make_router(api_handler& handler) {
    using katana::http::handler_fn;
    using katana::http::path_pattern;
    using katana::http::route_entry;
    static std::array<route_entry, route_count> route_entries = {
        route_entry{katana::http::method::get,
                    katana::http::path_pattern::from_literal<"/health">(),
                    handler_fn([&handler](const katana::http::request& req,
                                          katana::http::request_context& ctx)
                                   -> katana::result<katana::http::response> {
                        return handler.health(req, ctx);
                    })},
        route_entry{katana::http::method::get,
                    katana::http::path_pattern::from_literal<"/users">(),
                    handler_fn([&handler](const katana::http::request& req,
                                          katana::http::request_context& ctx)
                                   -> katana::result<katana::http::response> {
                        return handler.list_users(req, ctx);
                    })},
        route_entry{katana::http::method::post,
                    katana::http::path_pattern::from_literal<"/users">(),
                    handler_fn([&handler](const katana::http::request& req,
                                          katana::http::request_context& ctx)
                                   -> katana::result<katana::http::response> {
                        auto parsed_body = parse_UserInput(req.body, &ctx.arena);
                        if (!parsed_body)
                            return katana::http::response::error(
                                katana::problem_details::bad_request("invalid request body"));
                        return handler.create_user(req, ctx, *parsed_body);
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
                        auto id = *p_id;
                        return handler.get_user(req, ctx, id);
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
                        auto id = *p_id;
                        auto parsed_body = parse_UserInput(req.body, &ctx.arena);
                        if (!parsed_body)
                            return katana::http::response::error(
                                katana::problem_details::bad_request("invalid request body"));
                        return handler.update_user(req, ctx, id, *parsed_body);
                    })},
    };
    return katana::http::router(route_entries);
}

} // namespace generated
