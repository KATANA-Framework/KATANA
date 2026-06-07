#pragma once

#include "policy_storage.hpp"
#include "problem.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <utility>

namespace katana::http {

class in_memory_rate_limit_executor final : public route_policy_executor {
public:
    using clock = policy_clock;
    using key_extractor_fn = std::function<std::string(const request&, const request_context&)>;

    struct options {
        key_extractor_fn key_extractor{};
        size_t cleanup_interval = 256;
        std::string rejection_detail = "Rate limit exceeded";
        rate_limit_store* store = nullptr;
    };

    in_memory_rate_limit_executor() : in_memory_rate_limit_executor(options{}) {}

    explicit in_memory_rate_limit_executor(options opts)
        : owned_store_(rate_limit_store_options{opts.cleanup_interval}),
          store_(opts.store != nullptr ? *opts.store : static_cast<rate_limit_store&>(owned_store_)),
          options_(std::move(opts)) {
        if (!options_.key_extractor) {
            options_.key_extractor = default_client_key;
        }
    }

    result<route_policy_resolution>
    before_dispatch(const request& req, request_context& ctx, response& out) override {
        if (ctx.route_policy == nullptr || !ctx.route_policy->rate_limit.parsed()) {
            return route_policy_resolution::continue_request;
        }

        const auto window = unit_to_window(ctx.route_policy->rate_limit.unit);
        if (window <= clock::duration::zero()) {
            return route_policy_resolution::continue_request;
        }

        const auto decision = store_.consume(ctx.route_policy,
                                             options_.key_extractor(req, ctx),
                                             *ctx.route_policy->rate_limit.count,
                                             window,
                                             clock::now());
        if (decision.allowed) {
            return route_policy_resolution::continue_request;
        }

        out.assign_error(problem_details::too_many_requests(options_.rejection_detail));
        out.set_header(field::retry_after, std::to_string(decision.retry_after_seconds));
        return route_policy_resolution::short_circuit;
    }

    [[nodiscard]] rate_limit_store& store() noexcept { return store_; }

    static std::string default_client_key(const request& req, const request_context&) {
        if (auto auth = req.headers.get(field::authorization)) {
            return std::string("auth:") + std::string(*auth);
        }
        if (auto forwarded_for = req.headers.get("X-Forwarded-For")) {
            return std::string("xff:") + std::string(first_forwarded_value(*forwarded_for));
        }
        if (auto real_ip = req.headers.get("X-Real-IP")) {
            return std::string("rip:") + std::string(trim_ascii(*real_ip));
        }
        return "global";
    }

private:
    static std::string_view trim_ascii(std::string_view value) noexcept {
        while (!value.empty() &&
               (value.front() == ' ' || value.front() == '\t' || value.front() == '\n' ||
                value.front() == '\r')) {
            value.remove_prefix(1);
        }
        while (!value.empty() &&
               (value.back() == ' ' || value.back() == '\t' || value.back() == '\n' ||
                value.back() == '\r')) {
            value.remove_suffix(1);
        }
        return value;
    }

    static std::string_view first_forwarded_value(std::string_view value) noexcept {
        const auto comma = value.find(',');
        if (comma == std::string_view::npos) {
            return trim_ascii(value);
        }
        return trim_ascii(value.substr(0, comma));
    }

    static clock::duration unit_to_window(route_rate_limit_unit unit) noexcept {
        using namespace std::chrono;
        switch (unit) {
        case route_rate_limit_unit::second:
            return seconds(1);
        case route_rate_limit_unit::minute:
            return minutes(1);
        case route_rate_limit_unit::hour:
            return hours(1);
        case route_rate_limit_unit::unknown:
            break;
        }
        return clock::duration::zero();
    }

    in_memory_rate_limit_store owned_store_;
    rate_limit_store& store_;
    options options_;
};

} // namespace katana::http
