#pragma once

#include "log.hpp"
#include "policy_storage.hpp"

#include <algorithm>
#include <atomic>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

namespace katana::http {

class response_cache_executor final : public route_policy_executor {
public:
    using clock = policy_clock;
    using key_extractor_fn = std::function<std::string(const request&, const request_context&)>;

    struct options {
        std::chrono::seconds default_ttl{30};
        key_extractor_fn key_extractor{};
        size_t cleanup_interval = 256;
        response_cache_store* store = nullptr;
    };

    response_cache_executor() : response_cache_executor(options{}) {}

    explicit response_cache_executor(options opts)
        : owned_store_(response_cache_store_options{opts.cleanup_interval}),
          store_(opts.store != nullptr ? *opts.store : static_cast<response_cache_store&>(owned_store_)),
          options_(std::move(opts)) {
        if (!options_.key_extractor) {
            options_.key_extractor = default_cache_key;
        }
    }

    result<route_policy_resolution>
    before_dispatch(const request& req, request_context& ctx, response& out) override {
        auto ttl = resolve_ttl(ctx);
        if (!ttl.has_value() || !is_cacheable_method(req.http_method)) {
            return route_policy_resolution::continue_request;
        }

        auto snapshot =
            store_.lookup(ctx.route_policy, options_.key_extractor(req, ctx), clock::now());
        if (!snapshot.has_value()) {
            return route_policy_resolution::continue_request;
        }

        restore_response(*snapshot, out);
        out.set_header("X-Katana-Cache", "HIT");
        return route_policy_resolution::short_circuit;
    }

    result<void> after_dispatch(const request& req, request_context& ctx, response& out) override {
        auto ttl = resolve_ttl(ctx);
        if (!ttl.has_value() || !is_cacheable_method(req.http_method)) {
            return {};
        }

        // A deferred/async response is not populated yet at after_dispatch time — it completes later,
        // off the policy path — so caching `out` here would store an empty body (and serve it on the
        // next HIT). Skip the store and warn once: a route can be cached OR async, not both.
        if (ctx.is_response_deferred()) {
            static std::atomic<bool> warned{false};
            if (!warned.exchange(true, std::memory_order_relaxed)) {
                katana::log::warn("cache_skipped_deferred_response")
                    .field("hint", "x-katana-cache has no effect on an async/deferred handler");
            }
            return {};
        }

        if (!is_cacheable_response(out)) {
            return {};
        }

        store_.store(ctx.route_policy,
                     options_.key_extractor(req, ctx),
                     snapshot_response(out),
                     *ttl,
                     clock::now());
        return {};
    }

    [[nodiscard]] response_cache_store& store() noexcept { return store_; }

    static std::string default_cache_key(const request& req, const request_context&) {
        std::string key;
        key.reserve(req.uri.size() + 64);
        key.append(method_to_string(req.http_method));
        key.push_back(' ');
        key.append(req.uri);

        if (auto accept = req.headers.get(field::accept)) {
            key.append("|accept=");
            key.append(*accept);
        }
        if (auto auth = req.headers.get(field::authorization)) {
            key.append("|auth=");
            key.append(*auth);
        }
        return key;
    }

private:
    static bool is_cacheable_method(method http_method) noexcept {
        return http_method == method::get || http_method == method::head;
    }

    static bool is_cacheable_response(const response& out) noexcept {
        if (out.status < 200 || out.status >= 300) {
            return false;
        }
        if (out.headers.contains(field::set_cookie)) {
            return false;
        }
        if (auto cache_control = out.headers.get(field::cache_control)) {
            if (cache_control->find("no-store") != std::string_view::npos) {
                return false;
            }
        }
        return true;
    }

    std::optional<clock::duration> resolve_ttl(const request_context& ctx) const noexcept {
        if (ctx.route_policy == nullptr || !ctx.route_policy->cache.enabled()) {
            return std::nullopt;
        }

        if (ctx.route_policy->cache.kind == route_cache_policy_kind::enabled) {
            return options_.default_ttl;
        }
        if (ctx.route_policy->cache.kind != route_cache_policy_kind::ttl) {
            return std::nullopt;
        }
        return parse_ttl(ctx.route_policy->cache.value);
    }

    static std::optional<clock::duration> parse_ttl(std::string_view value) noexcept {
        if (value.size() < 2) {
            return std::nullopt;
        }

        const char suffix = value.back();
        value.remove_suffix(1);
        size_t amount = 0;
        const auto [end, ec] = std::from_chars(value.data(), value.data() + value.size(), amount);
        if (ec != std::errc() || end != value.data() + value.size() || amount == 0) {
            return std::nullopt;
        }

        using namespace std::chrono;
        switch (suffix) {
        case 's':
            return seconds(amount);
        case 'm':
            return minutes(amount);
        case 'h':
            return hours(amount);
        default:
            return std::nullopt;
        }
    }

    in_memory_response_cache_store owned_store_;
    response_cache_store& store_;
    options options_;
};

// Back-compat alias: the executor is store-agnostic; 'in_memory_response_cache_executor' kept for existing callers.
using in_memory_response_cache_executor = response_cache_executor;

} // namespace katana::http
