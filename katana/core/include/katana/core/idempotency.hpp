#pragma once

#include "policy_storage.hpp"
#include "problem.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace katana::http {

class idempotency_executor final : public route_policy_executor {
public:
    using clock = policy_clock;

    struct options {
        std::string key_header = "Idempotency-Key";
        std::chrono::seconds in_progress_ttl{30};
        std::chrono::minutes completed_ttl{10};
        size_t cleanup_interval = 256;
        std::string missing_key_detail = "Missing Idempotency-Key header";
        std::string request_in_progress_detail = "Idempotent request is already in progress";
        std::string fingerprint_mismatch_detail =
            "Idempotency-Key reuse with different request payload";
        idempotency_store* store = nullptr;
    };

    idempotency_executor() : idempotency_executor(options{}) {}

    explicit idempotency_executor(options opts)
        : owned_store_(idempotency_store_options{opts.cleanup_interval}),
          store_(opts.store != nullptr ? *opts.store : static_cast<idempotency_store&>(owned_store_)),
          options_(std::move(opts)) {}

    result<route_policy_resolution>
    before_dispatch(const request& req, request_context& ctx, response& out) override {
        auto state = prepare_state(req, ctx);
        if (!state.should_apply) {
            return route_policy_resolution::continue_request;
        }
        if (state.missing_required_key) {
            out.assign_error(problem_details::bad_request(options_.missing_key_detail));
            return route_policy_resolution::short_circuit;
        }
        // No key resolved (e.g. enabled-mode request without the header): nothing to dedupe —
        // never dereference a missing key below.
        if (!state.idempotency_key) {
            return route_policy_resolution::continue_request;
        }

        const auto begin = store_.begin(ctx.route_policy,
                                        *state.idempotency_key,
                                        state.fingerprint,
                                        clock::now(),
                                        options_.in_progress_ttl);
        switch (begin.status) {
        case idempotency_begin_status::started:
            return route_policy_resolution::continue_request;
        case idempotency_begin_status::replay:
            if (begin.snapshot.has_value()) {
                restore_response(*begin.snapshot, out);
            }
            out.set_header("Idempotency-Replayed", "true");
            return route_policy_resolution::short_circuit;
        case idempotency_begin_status::in_progress:
            out.assign_error(problem_details::conflict(options_.request_in_progress_detail));
            return route_policy_resolution::short_circuit;
        case idempotency_begin_status::fingerprint_mismatch:
            out.assign_error(problem_details::conflict(options_.fingerprint_mismatch_detail));
            return route_policy_resolution::short_circuit;
        }

        return route_policy_resolution::continue_request;
    }

    result<void> after_dispatch(const request& req, request_context& ctx, response& out) override {
        auto state = prepare_state(req, ctx);
        if (!state.should_apply || state.missing_required_key || !state.idempotency_key) {
            return {};
        }

        if (out.status >= 500) {
            store_.abandon(ctx.route_policy, *state.idempotency_key);
            return {};
        }

        store_.complete(ctx.route_policy,
                        *state.idempotency_key,
                        state.fingerprint,
                        snapshot_response(out),
                        clock::now(),
                        options_.completed_ttl);
        return {};
    }

    [[nodiscard]] idempotency_store& store() noexcept { return store_; }

private:
    struct prepared_state {
        bool should_apply = false;
        bool missing_required_key = false;
        std::optional<std::string_view> idempotency_key;
        idempotency_request_fingerprint fingerprint{};
    };

    static uint64_t fnv1a_hash(std::string_view value) noexcept {
        uint64_t hash = 14695981039346656037ULL;
        for (unsigned char ch : value) {
            hash ^= ch;
            hash *= 1099511628211ULL;
        }
        return hash;
    }

    static bool is_required_mode(std::string_view mode) noexcept { return mode == "required"; }

    prepared_state prepare_state(const request& req, const request_context& ctx) const {
        prepared_state state;
        if (ctx.route_policy == nullptr || !ctx.route_policy->idempotency.enabled()) {
            return state;
        }

        state.should_apply = true;
        const auto key = req.headers.get(options_.key_header);
        if (!key.has_value()) {
            // No key: only `required` mode rejects with 400; every other configuration simply
            // skips idempotency (leaving idempotency_key unset, so it must never be dereferenced).
            if (ctx.route_policy->idempotency.kind == route_idempotency_policy_kind::mode &&
                is_required_mode(ctx.route_policy->idempotency.value)) {
                state.missing_required_key = true;
            } else {
                state.should_apply = false;
            }
            return state;
        }

        state.idempotency_key = *key;
        state.fingerprint = make_fingerprint(req);
        return state;
    }

    static idempotency_request_fingerprint make_fingerprint(const request& req) {
        idempotency_request_fingerprint fingerprint;
        fingerprint.http_method = req.http_method;
        fingerprint.uri = std::string(req.uri);
        if (auto content_type = req.headers.get(field::content_type)) {
            fingerprint.content_type = std::string(*content_type);
        }
        fingerprint.body_size = req.body.size();
        fingerprint.body_hash = fnv1a_hash(req.body);
        return fingerprint;
    }

    in_memory_idempotency_store owned_store_;
    idempotency_store& store_;
    options options_;
};

// Back-compat alias: the executor is store-agnostic; 'in_memory_idempotency_executor' kept for existing callers.
using in_memory_idempotency_executor = idempotency_executor;

} // namespace katana::http
