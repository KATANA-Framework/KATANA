#include "katana/core/router.hpp"

#include "katana/core/http.hpp"
#include "support/http_handler_harness.hpp"

#include <gtest/gtest.h>

#include <array>
#include <optional>
#include <string>
#include <vector>

using namespace katana;
using namespace katana::http;

namespace {

request make_request(method m, std::string_view uri) {
    request req;
    req.http_method = m;
    req.uri = uri;
    req.headers = headers_map(nullptr);
    return req;
}

handler_fn make_handler(std::string body) {
    return [body = std::move(body)](const request&, request_context&) {
        return response::ok(body, "text/plain");
    };
}

} // namespace

TEST(Router, PrefersStaticOverParams) {
    route_entry routes[] = {
        route_entry{method::get, path_pattern::from_literal<"/users/me">(), make_handler("me")},
        route_entry{
            method::get, path_pattern::from_literal<"/users/{id}">(), make_handler("param")},
    };

    router r(routes);
    monotonic_arena arena;

    request_context ctx_static{arena};
    auto res_static = r.dispatch(make_request(method::get, "/users/me"), ctx_static);
    ASSERT_TRUE(res_static);
    EXPECT_EQ(res_static->body, "me");
    EXPECT_EQ(ctx_static.params.size(), 0);

    request_context ctx_param{arena};
    auto res_param = r.dispatch(make_request(method::get, "/users/42"), ctx_param);
    ASSERT_TRUE(res_param);
    EXPECT_EQ(res_param->body, "param");
    ASSERT_EQ(ctx_param.params.size(), 1);
    EXPECT_EQ(ctx_param.params.get("id"), std::optional<std::string_view>("42"));
}

TEST(Router, MethodNotAllowed) {
    route_entry routes[] = {
        route_entry{method::get, path_pattern::from_literal<"/items/{id}">(), make_handler("get")},
    };

    router r(routes);
    monotonic_arena arena;
    request_context ctx{arena};

    auto res = r.dispatch(make_request(method::post, "/items/1"), ctx);
    ASSERT_FALSE(res);
    EXPECT_EQ(res.error(), make_error_code(error_code::method_not_allowed));
}

TEST(Router, MethodNotAllowedResponseHasAllowHeader) {
    route_entry routes[] = {
        route_entry{method::get, path_pattern::from_literal<"/items/{id}">(), make_handler("get")},
        route_entry{
            method::post, path_pattern::from_literal<"/items/{id}">(), make_handler("post")},
    };

    router r(routes);
    monotonic_arena arena;
    request_context ctx{arena};

    request req = make_request(method::put, "/items/1");
    auto resp = dispatch_or_problem(r, req, ctx);
    EXPECT_EQ(resp.status, 405);
    auto allow = resp.headers.get("Allow");
    ASSERT_TRUE(allow.has_value());
    EXPECT_EQ(*allow, "GET, POST");
}

TEST(Router, NotFound) {
    route_entry routes[] = {
        route_entry{method::get, path_pattern::from_literal<"/items/{id}">(), make_handler("get")},
    };

    router r(routes);
    monotonic_arena arena;
    request_context ctx{arena};

    auto res = r.dispatch(make_request(method::get, "/missing"), ctx);
    ASSERT_FALSE(res);
    EXPECT_EQ(res.error(), make_error_code(error_code::not_found));
}

TEST(Router, MiddlewareOrderAndShortCircuit) {
    std::vector<std::string> trace;

    std::array<middleware_fn, 2> middleware{
        middleware_fn([&](const request&, request_context&, next_fn next) {
            trace.push_back("m1-before");
            auto result = next();
            trace.push_back("m1-after");
            return result;
        }),
        middleware_fn([&](const request&, request_context&, next_fn next) {
            trace.push_back("m2-before");
            auto result = next();
            trace.push_back("m2-after");
            return result;
        }),
    };

    route_entry routes[] = {
        route_entry{method::get,
                    path_pattern::from_literal<"/chain">(),
                    make_handler("ok"),
                    make_middleware_chain(middleware)},
    };

    router r(routes);
    monotonic_arena arena;
    request_context ctx{arena};

    auto res = r.dispatch(make_request(method::get, "/chain"), ctx);
    ASSERT_TRUE(res);
    EXPECT_EQ(res->body, "ok");

    std::vector<std::string> expected{
        "m1-before",
        "m2-before",
        "m2-after",
        "m1-after",
    };
    EXPECT_EQ(trace, expected);
}

TEST(Router, CapturesMultipleParamsAndStripsQuery) {
    route_entry routes[] = {
        route_entry{method::get,
                    path_pattern::from_literal<"/orders/{orderId}/items/{itemId}">(),
                    make_handler("ok")},
    };

    router r(routes);
    monotonic_arena arena;
    request_context ctx{arena};

    auto res = r.dispatch(make_request(method::get, "/orders/abc/items/99?foo=bar"), ctx);
    ASSERT_TRUE(res);
    EXPECT_EQ(res->body, "ok");
    ASSERT_EQ(ctx.params.size(), 2);
    EXPECT_EQ(ctx.params.get("orderId"), std::optional<std::string_view>("abc"));
    EXPECT_EQ(ctx.params.get("itemId"), std::optional<std::string_view>("99"));
}

TEST(Router, HarnessIntegrationAndProblemDetails) {
    route_entry routes[] = {
        route_entry{method::get,
                    path_pattern::from_literal<"/hello/{name}">(),
                    handler_fn([](const request&, request_context& ctx) {
                        auto name = ctx.params.get("name").value_or("anonymous");
                        return response::ok(std::string{name});
                    })},
    };

    router r(routes);
    http::router_handler handler(r);
    katana::test_support::HttpHandlerHarness harness(
        [&](const request& req, monotonic_arena& arena) { return handler(req, arena); });

    auto ok_resp = harness.run_raw("GET /hello/world HTTP/1.1\r\nHost: test\r\n\r\n");
    EXPECT_EQ(ok_resp.status, 200);
    EXPECT_EQ(ok_resp.body, "world");

    auto nf_resp = harness.run_raw("GET /missing HTTP/1.1\r\nHost: test\r\n\r\n");
    EXPECT_EQ(nf_resp.status, 404);
}
