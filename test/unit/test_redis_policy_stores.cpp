#include "katana/core/redis_policy_stores.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <optional>
#include <string_view>

using namespace katana::http;

namespace {

uint64_t stable_hash(std::string_view value) {
    uint64_t hash = 14695981039346656037ULL;
    for (unsigned char ch : value) {
        hash ^= ch;
        hash *= 1099511628211ULL;
    }
    return hash;
}

idempotency_request_fingerprint make_fingerprint(std::string_view uri, std::string_view body) {
    idempotency_request_fingerprint fingerprint;
    fingerprint.http_method = method::post;
    fingerprint.uri = std::string(uri);
    fingerprint.content_type = "application/json";
    fingerprint.body_size = body.size();
    fingerprint.body_hash = stable_hash(body);
    return fingerprint;
}

response_snapshot make_snapshot(int status, std::string_view body) {
    return response_snapshot{
        status,
        status == 201 ? "Created" : "OK",
        {{"Content-Type", "text/plain"}},
        std::string(body),
        false,
    };
}

} // namespace

TEST(RedisPolicyStores, ResponseCacheStoreRoundTripsSnapshot) {
    in_memory_redis_kv_client client;
    redis_response_cache_store store(client);

    const route_policy_view policy{
        route_cache_policy_view{route_cache_policy_kind::enabled, "true"},
        {},
        {},
        {},
        "listJobs",
    };

    store.store(&policy,
                "GET /jobs|accept=text/plain",
                make_snapshot(200, "cached-body"),
                std::chrono::seconds(10),
                policy_clock::now());

    auto loaded = store.lookup(&policy, "GET /jobs|accept=text/plain", policy_clock::now());
    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->status, 200);
    EXPECT_EQ(loaded->body, "cached-body");
    ASSERT_EQ(loaded->headers.size(), 1u);
    EXPECT_EQ(loaded->headers[0].first, "Content-Type");
}

TEST(RedisPolicyStores, RateLimitStoreUsesScopeToIsolateRoutes) {
    in_memory_redis_kv_client client;
    redis_rate_limit_store store(client);

    const route_policy_view jobs_policy{{}, {}, {true, "2/s", std::optional<size_t>{2U},
                                                 route_rate_limit_unit::second},
                                        {},
                                        "listJobs"};
    const route_policy_view reports_policy{{}, {}, {true, "2/s", std::optional<size_t>{2U},
                                                    route_rate_limit_unit::second},
                                           {},
                                           "listReports"};

    auto first = store.consume(&jobs_policy, "client-a", 2, std::chrono::seconds(1),
                               policy_clock::now());
    auto second = store.consume(&jobs_policy, "client-a", 2, std::chrono::seconds(1),
                                policy_clock::now());
    auto denied = store.consume(&jobs_policy, "client-a", 2, std::chrono::seconds(1),
                                policy_clock::now());
    auto other_route = store.consume(&reports_policy, "client-a", 2, std::chrono::seconds(1),
                                     policy_clock::now());

    EXPECT_TRUE(first.allowed);
    EXPECT_TRUE(second.allowed);
    EXPECT_FALSE(denied.allowed);
    EXPECT_GE(denied.retry_after_seconds, 1u);
    EXPECT_TRUE(other_route.allowed);
}

TEST(RedisPolicyStores, IdempotencyStoreReplaysCompletedResponseAndRejectsMismatch) {
    in_memory_redis_kv_client client;
    redis_idempotency_store store(client);

    const route_policy_view policy{
        {},
        {},
        {},
        route_idempotency_policy_view{route_idempotency_policy_kind::enabled, "true"},
        "createJob",
    };

    const auto fingerprint = make_fingerprint("/jobs", R"({"name":"a"})");
    const auto mismatch = make_fingerprint("/jobs", R"({"name":"b"})");

    auto started = store.begin(&policy, "key-1", fingerprint, policy_clock::now(),
                               std::chrono::seconds(30));
    EXPECT_EQ(started.status, idempotency_begin_status::started);

    store.complete(&policy,
                   "key-1",
                   fingerprint,
                   make_snapshot(201, "created-1"),
                   policy_clock::now(),
                   std::chrono::minutes(10));

    auto replay =
        store.begin(&policy, "key-1", fingerprint, policy_clock::now(), std::chrono::seconds(30));
    ASSERT_EQ(replay.status, idempotency_begin_status::replay);
    ASSERT_TRUE(replay.snapshot.has_value());
    EXPECT_EQ(replay.snapshot->status, 201);
    EXPECT_EQ(replay.snapshot->body, "created-1");

    auto conflict =
        store.begin(&policy, "key-1", mismatch, policy_clock::now(), std::chrono::seconds(30));
    EXPECT_EQ(conflict.status, idempotency_begin_status::fingerprint_mismatch);
}
