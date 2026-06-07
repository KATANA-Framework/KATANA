#include "katana/core/router.hpp"
#include "katana/core/cache.hpp"
#include "katana/core/contract_policies.hpp"
#include "katana/core/idempotency.hpp"
#include "katana/core/rate_limit.hpp"

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
    return [body = std::move(body)](const request&, request_context&, response& out) {
        out = response::ok(body, "text/plain");
        return result<void>{};
    };
}

class recording_policy_executor final : public route_policy_executor {
public:
    result<route_policy_resolution>
    before_dispatch(const request&, request_context&, response& out) override {
        ++calls;
        if (short_circuit) {
            out = response::ok("policy-short-circuit", "text/plain");
            return route_policy_resolution::short_circuit;
        }
        return route_policy_resolution::continue_request;
    }

    int calls = 0;
    bool short_circuit = false;
};

class replaying_cache_store final : public response_cache_store {
public:
    std::optional<response_snapshot>
    lookup(const route_policy_view*, std::string_view, policy_clock::time_point) override {
        ++lookup_calls;
        return snapshot;
    }

    void store(const route_policy_view*,
               std::string_view,
               response_snapshot,
               policy_clock::duration,
               policy_clock::time_point) override {
        ++store_calls;
    }

    int lookup_calls = 0;
    int store_calls = 0;
    response_snapshot snapshot{
        200,
        "OK",
        {{"Content-Type", "text/plain"}},
        "external-cache",
        false,
    };
};

class denying_rate_limit_store final : public rate_limit_store {
public:
    rate_limit_decision consume(const route_policy_view*,
                                std::string_view,
                                size_t,
                                policy_clock::duration,
                                policy_clock::time_point) override {
        ++consume_calls;
        return rate_limit_decision{false, 7};
    }

    int consume_calls = 0;
};

class replaying_idempotency_store final : public idempotency_store {
public:
    idempotency_begin_result begin(const route_policy_view*,
                                   std::string_view,
                                   const idempotency_request_fingerprint&,
                                   policy_clock::time_point,
                                   policy_clock::duration) override {
        ++begin_calls;
        return idempotency_begin_result{
            idempotency_begin_status::replay,
            response_snapshot{201,
                              "Created",
                              {{"Content-Type", "text/plain"}},
                              "external-idempotent",
                              false},
        };
    }

    void complete(const route_policy_view*,
                  std::string_view,
                  const idempotency_request_fingerprint&,
                  response_snapshot,
                  policy_clock::time_point,
                  policy_clock::duration) override {
        ++complete_calls;
    }

    void abandon(const route_policy_view*, std::string_view) override { ++abandon_calls; }

    int begin_calls = 0;
    int complete_calls = 0;
    int abandon_calls = 0;
};

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
        middleware_fn([&](const request&, request_context&, response& out, next_fn next) {
            trace.push_back("m1-before");
            auto result = next(out);
            trace.push_back("m1-after");
            return result;
        }),
        middleware_fn([&](const request&, request_context&, response& out, next_fn next) {
            trace.push_back("m2-before");
            auto result = next(out);
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

TEST(Router, ExposesRoutePolicyInRequestContext) {
    const route_policy_view policy{
        route_cache_policy_view{route_cache_policy_kind::ttl, "5m"},
        route_alloc_policy_view{
            route_alloc_policy_kind::bytes, "4096", std::optional<size_t>{4096U}},
        route_rate_limit_policy_view{
            true, "100/s", std::optional<size_t>{100U}, route_rate_limit_unit::second},
    };

    route_entry routes[] = {
        route_entry{method::get,
                    path_pattern::from_literal<"/policy">(),
                    handler_fn([](const request&, request_context& ctx, response& out) {
                        EXPECT_NE(ctx.route_policy, nullptr);
                        if (ctx.route_policy == nullptr) {
                            out = response::ok("policy");
                            return result<void>{};
                        }
                        EXPECT_FALSE(ctx.route_policy->empty());
                        EXPECT_EQ(ctx.route_policy->cache.kind, route_cache_policy_kind::ttl);
                        EXPECT_EQ(ctx.route_policy->cache.value, "5m");
                        EXPECT_TRUE(ctx.route_policy->alloc.bytes.has_value());
                        if (ctx.route_policy->alloc.bytes.has_value()) {
                            EXPECT_EQ(*ctx.route_policy->alloc.bytes, 4096U);
                        }
                        EXPECT_TRUE(ctx.route_policy->rate_limit.count.has_value());
                        if (ctx.route_policy->rate_limit.count.has_value()) {
                            EXPECT_EQ(*ctx.route_policy->rate_limit.count, 100U);
                        }
                        out = response::ok("policy");
                        return result<void>{};
                    }),
                    {},
                    &policy},
    };

    router r(routes);
    monotonic_arena arena;
    request_context ctx{arena};

    auto res = r.dispatch(make_request(method::get, "/policy"), ctx);
    ASSERT_TRUE(res);
    ASSERT_NE(ctx.route_policy, nullptr);
    EXPECT_EQ(ctx.route_policy->cache.value, "5m");
}

TEST(Router, InvokesRoutePolicyExecutorBeforeHandler) {
    const route_policy_view policy{
        route_cache_policy_view{route_cache_policy_kind::enabled, "true"},
        {},
        {},
    };

    recording_policy_executor executor;
    bool handler_called = false;
    route_entry routes[] = {
        route_entry{method::get,
                    path_pattern::from_literal<"/policy-exec">(),
                    handler_fn([&](const request&, request_context& ctx, response& out) {
                        handler_called = true;
                        EXPECT_EQ(ctx.policy_executor, &executor);
                        out = response::ok("handler");
                        return result<void>{};
                    }),
                    {},
                    &policy},
    };

    router r(routes);
    monotonic_arena arena;
    request_context ctx{arena};
    ctx.policy_executor = &executor;

    auto res = r.dispatch(make_request(method::get, "/policy-exec"), ctx);
    ASSERT_TRUE(res);
    EXPECT_EQ(executor.calls, 1);
    EXPECT_TRUE(handler_called);
    EXPECT_EQ(res->body, "handler");
}

TEST(Router, RoutePolicyExecutorCanShortCircuitHandler) {
    const route_policy_view policy{
        {},
        {},
        route_rate_limit_policy_view{
            true, "100/s", std::optional<size_t>{100U}, route_rate_limit_unit::second},
    };

    recording_policy_executor executor;
    executor.short_circuit = true;
    bool handler_called = false;
    route_entry routes[] = {
        route_entry{method::get,
                    path_pattern::from_literal<"/policy-stop">(),
                    handler_fn([&](const request&, request_context&, response& out) {
                        handler_called = true;
                        out = response::ok("handler");
                        return result<void>{};
                    }),
                    {},
                    &policy},
    };

    router r(routes);
    monotonic_arena arena;
    request_context ctx{arena};
    ctx.policy_executor = &executor;

    auto res = r.dispatch(make_request(method::get, "/policy-stop"), ctx);
    ASSERT_TRUE(res);
    EXPECT_EQ(executor.calls, 1);
    EXPECT_FALSE(handler_called);
    EXPECT_EQ(res->body, "policy-short-circuit");
}

TEST(Router, InMemoryRateLimitExecutorReturns429WithRetryAfter) {
    const route_policy_view policy{
        {},
        {},
        route_rate_limit_policy_view{
            true, "1/s", std::optional<size_t>{1U}, route_rate_limit_unit::second},
    };

    in_memory_rate_limit_executor executor;
    route_entry routes[] = {
        route_entry{method::get,
                    path_pattern::from_literal<"/limited">(),
                    make_handler("ok"),
                    {},
                    &policy},
    };

    router r(routes);
    monotonic_arena arena;
    request_context ctx_first{arena};
    ctx_first.policy_executor = &executor;

    auto first = r.dispatch(make_request(method::get, "/limited"), ctx_first);
    ASSERT_TRUE(first);
    EXPECT_EQ(first->status, 200);

    request_context ctx_second{arena};
    ctx_second.policy_executor = &executor;
    auto second = r.dispatch(make_request(method::get, "/limited"), ctx_second);
    ASSERT_TRUE(second);
    EXPECT_EQ(second->status, 429);
    EXPECT_EQ(second->reason, "Too Many Requests");
    auto retry_after = second->headers.get(field::retry_after);
    ASSERT_TRUE(retry_after.has_value());
    EXPECT_EQ(*retry_after, "1");
    EXPECT_NE(second->body.find("\"title\":\"Too Many Requests\""), std::string::npos);
}

TEST(Router, InMemoryRateLimitExecutorSeparatesClientsByDefaultKey) {
    const route_policy_view policy{
        {},
        {},
        route_rate_limit_policy_view{
            true, "1/s", std::optional<size_t>{1U}, route_rate_limit_unit::second},
    };

    in_memory_rate_limit_executor executor;
    route_entry routes[] = {
        route_entry{method::get,
                    path_pattern::from_literal<"/limited-clients">(),
                    make_handler("ok"),
                    {},
                    &policy},
    };

    router r(routes);
    monotonic_arena arena;

    auto first_req = make_request(method::get, "/limited-clients");
    first_req.headers.set_view("X-Forwarded-For", "10.0.0.1");
    request_context ctx_first{arena};
    ctx_first.policy_executor = &executor;
    auto first = r.dispatch(first_req, ctx_first);
    ASSERT_TRUE(first);
    EXPECT_EQ(first->status, 200);

    auto second_req = make_request(method::get, "/limited-clients");
    second_req.headers.set_view("X-Forwarded-For", "10.0.0.2");
    request_context ctx_second{arena};
    ctx_second.policy_executor = &executor;
    auto second = r.dispatch(second_req, ctx_second);
    ASSERT_TRUE(second);
    EXPECT_EQ(second->status, 200);
}

TEST(Router, InMemoryResponseCacheExecutorReplaysCachedResponse) {
    const route_policy_view policy{
        route_cache_policy_view{route_cache_policy_kind::enabled, "true"},
        {},
        {},
    };

    in_memory_response_cache_executor executor;
    int handler_calls = 0;
    route_entry routes[] = {
        route_entry{method::get,
                    path_pattern::from_literal<"/cached">(),
                    handler_fn([&](const request&, request_context&, response& out) {
                        ++handler_calls;
                        out.assign_text("cached-" + std::to_string(handler_calls));
                        return result<void>{};
                    }),
                    {},
                    &policy},
    };

    router r(routes);
    monotonic_arena arena;

    request_context first_ctx{arena};
    first_ctx.policy_executor = &executor;
    auto first = r.dispatch(make_request(method::get, "/cached"), first_ctx);
    ASSERT_TRUE(first);
    EXPECT_EQ(first->status, 200);
    EXPECT_EQ(first->body, "cached-1");

    request_context second_ctx{arena};
    second_ctx.policy_executor = &executor;
    auto second = r.dispatch(make_request(method::get, "/cached"), second_ctx);
    ASSERT_TRUE(second);
    EXPECT_EQ(second->status, 200);
    EXPECT_EQ(second->body, "cached-1");
    auto cache_header = second->headers.get("X-Katana-Cache");
    ASSERT_TRUE(cache_header.has_value());
    EXPECT_EQ(*cache_header, "HIT");
    EXPECT_EQ(handler_calls, 1);
}

TEST(Router, InMemoryResponseCacheExecutorSkipsNoStoreResponses) {
    const route_policy_view policy{
        route_cache_policy_view{route_cache_policy_kind::enabled, "true"},
        {},
        {},
    };

    in_memory_response_cache_executor executor;
    int handler_calls = 0;
    route_entry routes[] = {
        route_entry{method::get,
                    path_pattern::from_literal<"/cached-no-store">(),
                    handler_fn([&](const request&, request_context&, response& out) {
                        ++handler_calls;
                        out.assign_text("fresh-" + std::to_string(handler_calls));
                        out.set_header(field::cache_control, "no-store");
                        return result<void>{};
                    }),
                    {},
                    &policy},
    };

    router r(routes);
    monotonic_arena arena;

    request_context first_ctx{arena};
    first_ctx.policy_executor = &executor;
    auto first = r.dispatch(make_request(method::get, "/cached-no-store"), first_ctx);
    ASSERT_TRUE(first);
    EXPECT_EQ(first->body, "fresh-1");

    request_context second_ctx{arena};
    second_ctx.policy_executor = &executor;
    auto second = r.dispatch(make_request(method::get, "/cached-no-store"), second_ctx);
    ASSERT_TRUE(second);
    EXPECT_EQ(second->body, "fresh-2");
    EXPECT_FALSE(second->headers.get("X-Katana-Cache").has_value());
    EXPECT_EQ(handler_calls, 2);
}

TEST(Router, InMemoryResponseCacheExecutorSupportsInjectedStore) {
    const route_policy_view policy{
        route_cache_policy_view{route_cache_policy_kind::enabled, "true"},
        {},
        {},
    };

    replaying_cache_store store;
    in_memory_response_cache_executor::options opts;
    opts.store = &store;
    in_memory_response_cache_executor executor(std::move(opts));

    bool handler_called = false;
    route_entry routes[] = {
        route_entry{method::get,
                    path_pattern::from_literal<"/cached-external">(),
                    handler_fn([&](const request&, request_context&, response& out) {
                        handler_called = true;
                        out.assign_text("handler");
                        return result<void>{};
                    }),
                    {},
                    &policy},
    };

    router r(routes);
    monotonic_arena arena;
    request_context ctx{arena};
    ctx.policy_executor = &executor;

    auto res = r.dispatch(make_request(method::get, "/cached-external"), ctx);
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_EQ(res->body, "external-cache");
    EXPECT_EQ(res->headers.get("X-Katana-Cache"), std::optional<std::string_view>{"HIT"});
    EXPECT_FALSE(handler_called);
    EXPECT_EQ(store.lookup_calls, 1);
    EXPECT_EQ(store.store_calls, 0);
}

TEST(Router, InMemoryIdempotencyExecutorReplaysStoredResponse) {
    const route_policy_view policy{
        {},
        {},
        {},
        route_idempotency_policy_view{route_idempotency_policy_kind::enabled, "true"},
    };

    in_memory_idempotency_executor executor;
    int handler_calls = 0;
    route_entry routes[] = {
        route_entry{method::post,
                    path_pattern::from_literal<"/idempotent">(),
                    handler_fn([&](const request&, request_context&, response& out) {
                        ++handler_calls;
                        out.assign_text("created-" + std::to_string(handler_calls),
                                        "text/plain",
                                        201,
                                        "Created");
                        out.set_header("X-Call-Count", std::to_string(handler_calls));
                        return result<void>{};
                    }),
                    {},
                    &policy},
    };

    router r(routes);
    monotonic_arena arena;

    request first_req = make_request(method::post, "/idempotent");
    first_req.headers.set_view("Idempotency-Key", "key-1");
    request_context first_ctx{arena};
    first_ctx.policy_executor = &executor;
    auto first = r.dispatch(first_req, first_ctx);
    ASSERT_TRUE(first);
    EXPECT_EQ(first->status, 201);
    EXPECT_EQ(first->body, "created-1");

    request replay_req = make_request(method::post, "/idempotent");
    replay_req.headers.set_view("Idempotency-Key", "key-1");
    request_context replay_ctx{arena};
    replay_ctx.policy_executor = &executor;
    auto replay = r.dispatch(replay_req, replay_ctx);
    ASSERT_TRUE(replay);
    EXPECT_EQ(replay->status, 201);
    EXPECT_EQ(replay->body, "created-1");
    auto replayed = replay->headers.get("Idempotency-Replayed");
    ASSERT_TRUE(replayed.has_value());
    EXPECT_EQ(*replayed, "true");
    EXPECT_EQ(handler_calls, 1);
}

TEST(Router, InMemoryIdempotencyExecutorSupportsInjectedStore) {
    const route_policy_view policy{
        {},
        {},
        {},
        route_idempotency_policy_view{route_idempotency_policy_kind::enabled, "true"},
    };

    replaying_idempotency_store store;
    in_memory_idempotency_executor::options opts;
    opts.store = &store;
    in_memory_idempotency_executor executor(std::move(opts));

    bool handler_called = false;
    route_entry routes[] = {
        route_entry{method::post,
                    path_pattern::from_literal<"/idempotent-external">(),
                    handler_fn([&](const request&, request_context&, response& out) {
                        handler_called = true;
                        out.assign_text("handler");
                        return result<void>{};
                    }),
                    {},
                    &policy},
    };

    router r(routes);
    monotonic_arena arena;
    request req = make_request(method::post, "/idempotent-external");
    req.headers.set_view("Idempotency-Key", "external-key");
    request_context ctx{arena};
    ctx.policy_executor = &executor;

    auto res = r.dispatch(req, ctx);
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 201);
    EXPECT_EQ(res->body, "external-idempotent");
    EXPECT_EQ(res->headers.get("Idempotency-Replayed"), std::optional<std::string_view>{"true"});
    EXPECT_FALSE(handler_called);
    EXPECT_EQ(store.begin_calls, 1);
    EXPECT_EQ(store.complete_calls, 0);
    EXPECT_EQ(store.abandon_calls, 0);
}

TEST(Router, InMemoryIdempotencyExecutorRejectsFingerprintMismatch) {
    const route_policy_view policy{
        {},
        {},
        {},
        route_idempotency_policy_view{route_idempotency_policy_kind::enabled, "true"},
    };

    in_memory_idempotency_executor executor;
    route_entry routes[] = {
        route_entry{method::post,
                    path_pattern::from_literal<"/idempotent-conflict">(),
                    handler_fn([](const request&, request_context&, response& out) {
                        out.assign_text("ok");
                        return result<void>{};
                    }),
                    {},
                    &policy},
    };

    router r(routes);
    monotonic_arena arena;

    request first_req = make_request(method::post, "/idempotent-conflict?x=1");
    first_req.headers.set_view("Idempotency-Key", "dup-key");
    request_context first_ctx{arena};
    first_ctx.policy_executor = &executor;
    auto first = r.dispatch(first_req, first_ctx);
    ASSERT_TRUE(first);
    EXPECT_EQ(first->status, 200);

    request second_req = make_request(method::post, "/idempotent-conflict?x=2");
    second_req.headers.set_view("Idempotency-Key", "dup-key");
    request_context second_ctx{arena};
    second_ctx.policy_executor = &executor;
    auto second = r.dispatch(second_req, second_ctx);
    ASSERT_TRUE(second);
    EXPECT_EQ(second->status, 409);
    EXPECT_NE(second->body.find("different request payload"), std::string::npos);
}

TEST(Router, InMemoryIdempotencyExecutorRequiresKeyInRequiredMode) {
    const route_policy_view policy{
        {},
        {},
        {},
        route_idempotency_policy_view{route_idempotency_policy_kind::mode, "required"},
    };

    in_memory_idempotency_executor executor;
    bool handler_called = false;
    route_entry routes[] = {
        route_entry{method::post,
                    path_pattern::from_literal<"/idempotent-required">(),
                    handler_fn([&](const request&, request_context&, response& out) {
                        handler_called = true;
                        out.assign_text("ok");
                        return result<void>{};
                    }),
                    {},
                    &policy},
    };

    router r(routes);
    monotonic_arena arena;
    request_context ctx{arena};
    ctx.policy_executor = &executor;

    auto res = r.dispatch(make_request(method::post, "/idempotent-required"), ctx);
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 400);
    EXPECT_FALSE(handler_called);
}

TEST(Router, RoutePolicyExecutorChainCombinesRateLimitAndIdempotency) {
    const route_policy_view policy{
        {},
        {},
        route_rate_limit_policy_view{
            true, "2/s", std::optional<size_t>{2U}, route_rate_limit_unit::second},
        route_idempotency_policy_view{route_idempotency_policy_kind::enabled, "true"},
    };

    in_memory_rate_limit_executor rate_limit;
    in_memory_idempotency_executor idempotency;
    route_policy_executor_chain chain;
    chain.add(idempotency).add(rate_limit);

    int handler_calls = 0;
    route_entry routes[] = {
        route_entry{method::post,
                    path_pattern::from_literal<"/combo">(),
                    handler_fn([&](const request&, request_context&, response& out) {
                        ++handler_calls;
                        out.assign_text("combo-" + std::to_string(handler_calls));
                        return result<void>{};
                    }),
                    {},
                    &policy},
    };

    router r(routes);
    monotonic_arena arena;

    request first_req = make_request(method::post, "/combo");
    first_req.headers.set_view("Idempotency-Key", "combo-key");
    request_context first_ctx{arena};
    first_ctx.policy_executor = &chain;
    auto first = r.dispatch(first_req, first_ctx);
    ASSERT_TRUE(first);
    EXPECT_EQ(first->status, 200);

    request replay_req = make_request(method::post, "/combo");
    replay_req.headers.set_view("Idempotency-Key", "combo-key");
    request_context replay_ctx{arena};
    replay_ctx.policy_executor = &chain;
    auto replay = r.dispatch(replay_req, replay_ctx);
    ASSERT_TRUE(replay);
    EXPECT_EQ(replay->status, 200);
    EXPECT_EQ(replay->body, "combo-1");
    EXPECT_EQ(handler_calls, 1);

    request another_req = make_request(method::post, "/combo");
    another_req.headers.set_view("Idempotency-Key", "combo-key-2");
    request_context another_ctx{arena};
    another_ctx.policy_executor = &chain;
    auto another = r.dispatch(another_req, another_ctx);
    ASSERT_TRUE(another);
    EXPECT_EQ(another->status, 200);

    request limited_req = make_request(method::post, "/combo");
    limited_req.headers.set_view("Idempotency-Key", "combo-key-3");
    request_context limited_ctx{arena};
    limited_ctx.policy_executor = &chain;
    auto limited = r.dispatch(limited_req, limited_ctx);
    ASSERT_TRUE(limited);
    EXPECT_EQ(limited->status, 429);
}

TEST(Router, InMemoryRateLimitExecutorSupportsInjectedStore) {
    const route_policy_view policy{
        {},
        {},
        route_rate_limit_policy_view{
            true, "1/s", std::optional<size_t>{1U}, route_rate_limit_unit::second},
    };

    denying_rate_limit_store store;
    in_memory_rate_limit_executor::options opts;
    opts.store = &store;
    in_memory_rate_limit_executor executor(std::move(opts));

    bool handler_called = false;
    route_entry routes[] = {
        route_entry{method::get,
                    path_pattern::from_literal<"/limited-external">(),
                    handler_fn([&](const request&, request_context&, response& out) {
                        handler_called = true;
                        out.assign_text("handler");
                        return result<void>{};
                    }),
                    {},
                    &policy},
    };

    router r(routes);
    monotonic_arena arena;
    request_context ctx{arena};
    ctx.policy_executor = &executor;

    auto res = r.dispatch(make_request(method::get, "/limited-external"), ctx);
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 429);
    EXPECT_EQ(res->headers.get(field::retry_after), std::optional<std::string_view>{"7"});
    EXPECT_FALSE(handler_called);
    EXPECT_EQ(store.consume_calls, 1);
}

TEST(Router, RoutePolicyExecutorChainAllowsCacheHitsWithoutConsumingRateLimitBudget) {
    const route_policy_view policy{
        route_cache_policy_view{route_cache_policy_kind::enabled, "true"},
        {},
        route_rate_limit_policy_view{
            true, "2/s", std::optional<size_t>{2U}, route_rate_limit_unit::second},
    };

    in_memory_response_cache_executor cache;
    in_memory_rate_limit_executor rate_limit;
    route_policy_executor_chain chain;
    chain.add(cache).add(rate_limit);

    int handler_calls = 0;
    route_entry routes[] = {
        route_entry{method::get,
                    path_pattern::from_literal<"/cache-rate-limit">(),
                    handler_fn([&](const request& req, request_context&, response& out) {
                        ++handler_calls;
                        std::string body = "body-" + std::to_string(handler_calls);
                        if (auto accept = req.headers.get(field::accept)) {
                            body.append("|");
                            body.append(*accept);
                        }
                        out.assign_text(std::move(body));
                        return result<void>{};
                    }),
                    {},
                    &policy},
    };

    router r(routes);
    monotonic_arena arena;

    request first_req = make_request(method::get, "/cache-rate-limit");
    request_context first_ctx{arena};
    first_ctx.policy_executor = &chain;
    auto first = r.dispatch(first_req, first_ctx);
    ASSERT_TRUE(first);
    EXPECT_EQ(first->status, 200);
    EXPECT_EQ(first->body, "body-1");

    request replay_req = make_request(method::get, "/cache-rate-limit");
    request_context replay_ctx{arena};
    replay_ctx.policy_executor = &chain;
    auto replay = r.dispatch(replay_req, replay_ctx);
    ASSERT_TRUE(replay);
    EXPECT_EQ(replay->status, 200);
    EXPECT_EQ(replay->body, "body-1");
    EXPECT_EQ(replay->headers.get("X-Katana-Cache"), std::optional<std::string_view>{"HIT"});

    request miss_req = make_request(method::get, "/cache-rate-limit");
    miss_req.headers.set_view("Accept", "application/json");
    request_context miss_ctx{arena};
    miss_ctx.policy_executor = &chain;
    auto miss = r.dispatch(miss_req, miss_ctx);
    ASSERT_TRUE(miss);
    EXPECT_EQ(miss->status, 200);
    EXPECT_EQ(miss->body, "body-2|application/json");

    request limited_req = make_request(method::get, "/cache-rate-limit");
    limited_req.headers.set_view("Accept", "text/html");
    request_context limited_ctx{arena};
    limited_ctx.policy_executor = &chain;
    auto limited = r.dispatch(limited_req, limited_ctx);
    ASSERT_TRUE(limited);
    EXPECT_EQ(limited->status, 429);
    EXPECT_EQ(handler_calls, 2);
}

TEST(Router, InMemoryContractPolicyExecutorAppliesCanonicalCacheOrdering) {
    const route_policy_view policy{
        route_cache_policy_view{route_cache_policy_kind::enabled, "true"},
        {},
        route_rate_limit_policy_view{
            true, "2/s", std::optional<size_t>{2U}, route_rate_limit_unit::second},
    };

    in_memory_contract_policy_executor executor;
    int handler_calls = 0;
    route_entry routes[] = {
        route_entry{method::get,
                    path_pattern::from_literal<"/contract-cache">(),
                    handler_fn([&](const request& req, request_context&, response& out) {
                        ++handler_calls;
                        std::string body = "contract-" + std::to_string(handler_calls);
                        if (auto accept = req.headers.get(field::accept)) {
                            body.append("|");
                            body.append(*accept);
                        }
                        out.assign_text(std::move(body));
                        return result<void>{};
                    }),
                    {},
                    &policy},
    };

    router r(routes);
    monotonic_arena arena;

    request_context first_ctx{arena};
    first_ctx.policy_executor = &executor;
    auto first = r.dispatch(make_request(method::get, "/contract-cache"), first_ctx);
    ASSERT_TRUE(first);
    EXPECT_EQ(first->body, "contract-1");

    request_context replay_ctx{arena};
    replay_ctx.policy_executor = &executor;
    auto replay = r.dispatch(make_request(method::get, "/contract-cache"), replay_ctx);
    ASSERT_TRUE(replay);
    EXPECT_EQ(replay->body, "contract-1");
    EXPECT_EQ(replay->headers.get("X-Katana-Cache"), std::optional<std::string_view>{"HIT"});

    request miss_req = make_request(method::get, "/contract-cache");
    miss_req.headers.set_view("Accept", "application/json");
    request_context miss_ctx{arena};
    miss_ctx.policy_executor = &executor;
    auto miss = r.dispatch(miss_req, miss_ctx);
    ASSERT_TRUE(miss);
    EXPECT_EQ(miss->status, 200);
    EXPECT_EQ(handler_calls, 2);

    request limited_req = make_request(method::get, "/contract-cache");
    limited_req.headers.set_view("Accept", "text/html");
    request_context limited_ctx{arena};
    limited_ctx.policy_executor = &executor;
    auto limited = r.dispatch(limited_req, limited_ctx);
    ASSERT_TRUE(limited);
    EXPECT_EQ(limited->status, 429);
}

TEST(Router, InMemoryContractPolicyExecutorReplaysIdempotentResponsesBeforeRateLimit) {
    const route_policy_view policy{
        {},
        {},
        route_rate_limit_policy_view{
            true, "1/s", std::optional<size_t>{1U}, route_rate_limit_unit::second},
        route_idempotency_policy_view{route_idempotency_policy_kind::enabled, "true"},
    };

    in_memory_contract_policy_executor executor;
    int handler_calls = 0;
    route_entry routes[] = {
        route_entry{method::post,
                    path_pattern::from_literal<"/contract-idempotent">(),
                    handler_fn([&](const request&, request_context&, response& out) {
                        ++handler_calls;
                        out.assign_text("created-" + std::to_string(handler_calls),
                                        "text/plain",
                                        201,
                                        "Created");
                        return result<void>{};
                    }),
                    {},
                    &policy},
    };

    router r(routes);
    monotonic_arena arena;

    request first_req = make_request(method::post, "/contract-idempotent");
    first_req.headers.set_view("Idempotency-Key", "contract-key");
    request_context first_ctx{arena};
    first_ctx.policy_executor = &executor;
    auto first = r.dispatch(first_req, first_ctx);
    ASSERT_TRUE(first);
    EXPECT_EQ(first->status, 201);

    request replay_req = make_request(method::post, "/contract-idempotent");
    replay_req.headers.set_view("Idempotency-Key", "contract-key");
    request_context replay_ctx{arena};
    replay_ctx.policy_executor = &executor;
    auto replay = r.dispatch(replay_req, replay_ctx);
    ASSERT_TRUE(replay);
    EXPECT_EQ(replay->status, 201);
    EXPECT_EQ(replay->body, "created-1");
    EXPECT_EQ(replay->headers.get("Idempotency-Replayed"),
              std::optional<std::string_view>{"true"});
    EXPECT_EQ(handler_calls, 1);

    request limited_req = make_request(method::post, "/contract-idempotent");
    limited_req.headers.set_view("Idempotency-Key", "contract-key-2");
    request_context limited_ctx{arena};
    limited_ctx.policy_executor = &executor;
    auto limited = r.dispatch(limited_req, limited_ctx);
    ASSERT_TRUE(limited);
    EXPECT_EQ(limited->status, 429);
}

TEST(Router, HarnessIntegrationAndProblemDetails) {
    route_entry routes[] = {
        route_entry{method::get,
                    path_pattern::from_literal<"/hello/{name}">(),
                    handler_fn([](const request&, request_context& ctx, response& out) {
                        auto name = ctx.params.get("name").value_or("anonymous");
                        out = response::ok(std::string{name});
                        return result<void>{};
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

TEST(RequestContext, ScheduleReturnsFalseWithoutScheduler) {
    monotonic_arena arena;
    request_context ctx{arena};

    bool invoked = false;
    EXPECT_FALSE(ctx.can_schedule());
    EXPECT_FALSE(ctx.schedule([&invoked]() { invoked = true; }));
    EXPECT_FALSE(invoked);
}

TEST(RequestContext, ScheduleInvokesConfiguredScheduler) {
    monotonic_arena arena;
    request_context ctx{arena};

    bool scheduler_called = false;
    bool task_called = false;
    ctx.task_scheduler_user = &scheduler_called;
    ctx.task_scheduler = [](void* user, request_context::scheduled_task task) {
        auto* called = static_cast<bool*>(user);
        *called = true;
        task();
        return true;
    };

    EXPECT_TRUE(ctx.can_schedule());
    EXPECT_TRUE(ctx.schedule([&task_called]() { task_called = true; }));
    EXPECT_TRUE(scheduler_called);
    EXPECT_TRUE(task_called);
}

TEST(RequestContext, ShareDeferredResponseReturnsInvalidWithoutFactory) {
    monotonic_arena arena;
    request_context ctx{arena};

    auto deferred = ctx.share_deferred_response();
    EXPECT_FALSE(static_cast<bool>(deferred));
    EXPECT_FALSE(ctx.is_response_deferred());
}

namespace {

// Capture katana::log output produced while running `body`.
template <typename Fn> std::string capture_log(Fn&& body) {
    std::FILE* f = std::tmpfile();
    katana::log::set_sink(f);
    katana::log::set_min_level(katana::log::level::debug);
    std::forward<Fn>(body)();
    std::fflush(f);
    std::rewind(f);
    std::string out;
    char buf[2048];
    size_t n = 0;
    while ((n = std::fread(buf, 1, sizeof(buf), f)) > 0) {
        out.append(buf, n);
    }
    std::fclose(f);
    katana::log::set_sink(stderr);
    return out;
}

} // namespace

TEST(RequestContextLog, TagsHandlerLogsWithCorrelationAndTrace) {
    monotonic_arena arena;
    request_context ctx{arena};
    ctx.request_id = "req-abc";
    ctx.trace = katana::tracing::start_server_span(
        "00-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-01");

    const std::string out = capture_log([&] {
        ctx.log_info("charged card").field("amount", 1299);
    });

    EXPECT_NE(out.find("\"msg\":\"charged card\""), std::string::npos);
    EXPECT_NE(out.find("\"request_id\":\"req-abc\""), std::string::npos);
    EXPECT_NE(out.find("\"trace_id\":\"4bf92f3577b34da6a3ce929d0e0e4736\""), std::string::npos);
    EXPECT_NE(out.find("\"amount\":1299"), std::string::npos);
}

TEST(RequestContextLog, OmitsTraceWhenNotTraced) {
    monotonic_arena arena;
    request_context ctx{arena};
    ctx.request_id = "req-xyz";
    // No trace set -> trace.valid() is false.

    const std::string out = capture_log([&] { ctx.log_warn("retrying"); });

    EXPECT_NE(out.find("\"level\":\"warn\""), std::string::npos);
    EXPECT_NE(out.find("\"request_id\":\"req-xyz\""), std::string::npos);
    EXPECT_EQ(out.find("trace_id"), std::string::npos);
}
