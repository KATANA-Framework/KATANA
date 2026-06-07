#pragma once

#include "cache.hpp"
#include "idempotency.hpp"
#include "rate_limit.hpp"
#include "redis_client.hpp"

namespace katana::http {

class in_memory_contract_policy_executor final : public route_policy_executor {
public:
    struct options {
        bool enable_cache = true;
        bool enable_idempotency = true;
        bool enable_rate_limit = true;
        response_cache_executor::options cache{};
        idempotency_executor::options idempotency{};
        rate_limit_executor::options rate_limit{};
    };

    in_memory_contract_policy_executor() : in_memory_contract_policy_executor(options{}) {}

    explicit in_memory_contract_policy_executor(options opts)
        : cache_(std::move(opts.cache)), idempotency_(std::move(opts.idempotency)),
          rate_limit_(std::move(opts.rate_limit)) {
        if (opts.enable_cache) {
            chain_.add(cache_);
        }
        if (opts.enable_idempotency) {
            chain_.add(idempotency_);
        }
        if (opts.enable_rate_limit) {
            chain_.add(rate_limit_);
        }
    }

    result<route_policy_resolution>
    before_dispatch(const request& req, request_context& ctx, response& out) override {
        return chain_.before_dispatch(req, ctx, out);
    }

    result<void> after_dispatch(const request& req, request_context& ctx, response& out) override {
        return chain_.after_dispatch(req, ctx, out);
    }

    [[nodiscard]] response_cache_executor& cache() noexcept { return cache_; }
    [[nodiscard]] idempotency_executor& idempotency() noexcept { return idempotency_; }
    [[nodiscard]] rate_limit_executor& rate_limit() noexcept { return rate_limit_; }

private:
    response_cache_executor cache_;
    idempotency_executor idempotency_;
    rate_limit_executor rate_limit_;
    route_policy_executor_chain chain_;
};

// Same as in_memory_contract_policy_executor, but backed by Redis so cache /
// rate-limit / idempotency state is shared across server instances. Wire it in one
// line: construct it from a connected redis_kv_client and pass it to
// server.policy_executor(...). It owns the Redis-backed stores and the executors.
class redis_contract_policy_executor final : public route_policy_executor {
public:
    struct options {
        bool enable_cache = true;
        bool enable_idempotency = true;
        bool enable_rate_limit = true;
        redis_store_options store{};
        response_cache_executor::options cache{};
        idempotency_executor::options idempotency{};
        rate_limit_executor::options rate_limit{};
        // In-process L1 cache in front of Redis: a cache HIT is served without a Redis
        // round-trip. L1 entries are capped to this TTL, so a cross-instance cache
        // invalidation goes stale on other instances for at most this long. Set to 0 to
        // disable (go straight to Redis on every lookup).
        policy_clock::duration l1_cache_ttl = std::chrono::seconds(1);
    };

    // Recommended: back the policies with a per-reactor connection pool, so each
    // worker thread uses its own Redis connection (parallel and thread-safe).
    explicit redis_contract_policy_executor(redis_pool& pool)
        : redis_contract_policy_executor(pool, options{}) {}

    redis_contract_policy_executor(redis_pool& pool, options opts)
        : pool_adapter_(std::make_unique<redis_pool_client>(pool)),
          cache_store_(*pool_adapter_, opts.store),
          idempotency_store_(*pool_adapter_, opts.store),
          rate_limit_store_(*pool_adapter_, opts.store), l1_cache_store_(),
          tiered_cache_store_(l1_cache_store_, cache_store_, opts.l1_cache_ttl),
          cache_((opts.cache.store = select_cache_store(opts.l1_cache_ttl),
                  std::move(opts.cache))),
          idempotency_((opts.idempotency.store = &idempotency_store_, std::move(opts.idempotency))),
          rate_limit_((opts.rate_limit.store = &rate_limit_store_, std::move(opts.rate_limit))) {
        build_chain(opts.enable_cache, opts.enable_idempotency, opts.enable_rate_limit);
    }

    // Single shared connection. Fine for one worker / tests; under multiple reactor
    // threads prefer the redis_pool constructor above.
    explicit redis_contract_policy_executor(redis_kv_client& client)
        : redis_contract_policy_executor(client, options{}) {}

    redis_contract_policy_executor(redis_kv_client& client, options opts)
        : cache_store_(client, opts.store), idempotency_store_(client, opts.store),
          rate_limit_store_(client, opts.store), l1_cache_store_(),
          tiered_cache_store_(l1_cache_store_, cache_store_, opts.l1_cache_ttl),
          cache_((opts.cache.store = select_cache_store(opts.l1_cache_ttl),
                  std::move(opts.cache))),
          idempotency_((opts.idempotency.store = &idempotency_store_, std::move(opts.idempotency))),
          rate_limit_((opts.rate_limit.store = &rate_limit_store_, std::move(opts.rate_limit))) {
        build_chain(opts.enable_cache, opts.enable_idempotency, opts.enable_rate_limit);
    }

    result<route_policy_resolution>
    before_dispatch(const request& req, request_context& ctx, response& out) override {
        return chain_.before_dispatch(req, ctx, out);
    }

    result<void> after_dispatch(const request& req, request_context& ctx, response& out) override {
        return chain_.after_dispatch(req, ctx, out);
    }

private:
    // The cache executor reads through the L1+L2 tier when an L1 TTL is set, otherwise
    // straight from the Redis store. Both members are already constructed by the time the
    // cache executor is initialized (declaration order), so this is safe in the init list.
    response_cache_store* select_cache_store(policy_clock::duration l1_ttl) noexcept {
        return l1_ttl.count() > 0 ? static_cast<response_cache_store*>(&tiered_cache_store_)
                                  : static_cast<response_cache_store*>(&cache_store_);
    }

    void build_chain(bool enable_cache, bool enable_idempotency, bool enable_rate_limit) {
        if (enable_cache) {
            chain_.add(cache_);
        }
        if (enable_idempotency) {
            chain_.add(idempotency_);
        }
        if (enable_rate_limit) {
            chain_.add(rate_limit_);
        }
    }

    std::unique_ptr<redis_pool_client> pool_adapter_; // non-null only for the pool ctor
    redis_response_cache_store cache_store_;
    redis_idempotency_store idempotency_store_;
    redis_rate_limit_store rate_limit_store_;
    in_memory_response_cache_store l1_cache_store_;       // L1 in front of the Redis cache store
    tiered_response_cache_store tiered_cache_store_;      // L1 + L2(Redis), used when l1 TTL > 0
    response_cache_executor cache_;
    idempotency_executor idempotency_;
    rate_limit_executor rate_limit_;
    route_policy_executor_chain chain_;
};

} // namespace katana::http
