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

inline const katana::http::router& make_router(api_handler& handler) {
    using katana::http::handler_fn;
    using katana::http::path_pattern;
    using katana::http::route_entry;
    static std::array<route_entry, route_count> route_entries = {
        route_entry{katana::http::method::get,
                    katana::http::path_pattern::from_literal<"/tasks">(),
                    handler_fn([&handler](const katana::http::request& req,
                                          katana::http::request_context& ctx)
                                   -> katana::result<katana::http::response> {
                        return handler.list_tasks(req, ctx);
                    })},
        route_entry{katana::http::method::post,
                    katana::http::path_pattern::from_literal<"/tasks">(),
                    handler_fn([&handler](const katana::http::request& req,
                                          katana::http::request_context& ctx)
                                   -> katana::result<katana::http::response> {
                        auto parsed_body = parse_CreateTaskRequest(req.body, &ctx.arena);
                        if (!parsed_body)
                            return katana::http::response::error(
                                katana::problem_details::bad_request("invalid request body"));
                        return handler.create_task(req, ctx, *parsed_body);
                    })},
        route_entry{katana::http::method::get,
                    katana::http::path_pattern::from_literal<"/tasks/{id}">(),
                    handler_fn([&handler](const katana::http::request& req,
                                          katana::http::request_context& ctx)
                                   -> katana::result<katana::http::response> {
                        auto p_id = ctx.params.get("id");
                        if (!p_id)
                            return katana::http::response::error(
                                katana::problem_details::bad_request("missing path param id"));
                        auto id = *p_id;
                        return handler.get_task(req, ctx, id);
                    })},
        route_entry{katana::http::method::put,
                    katana::http::path_pattern::from_literal<"/tasks/{id}">(),
                    handler_fn([&handler](const katana::http::request& req,
                                          katana::http::request_context& ctx)
                                   -> katana::result<katana::http::response> {
                        auto p_id = ctx.params.get("id");
                        if (!p_id)
                            return katana::http::response::error(
                                katana::problem_details::bad_request("missing path param id"));
                        auto id = *p_id;
                        auto parsed_body = parse_UpdateTaskRequest(req.body, &ctx.arena);
                        if (!parsed_body)
                            return katana::http::response::error(
                                katana::problem_details::bad_request("invalid request body"));
                        return handler.update_task(req, ctx, id, *parsed_body);
                    })},
        route_entry{katana::http::method::del,
                    katana::http::path_pattern::from_literal<"/tasks/{id}">(),
                    handler_fn([&handler](const katana::http::request& req,
                                          katana::http::request_context& ctx)
                                   -> katana::result<katana::http::response> {
                        auto p_id = ctx.params.get("id");
                        if (!p_id)
                            return katana::http::response::error(
                                katana::problem_details::bad_request("missing path param id"));
                        auto id = *p_id;
                        return handler.delete_task(req, ctx, id);
                    })},
    };
    static katana::http::router router_instance(route_entries);
    return router_instance;
}

} // namespace generated
