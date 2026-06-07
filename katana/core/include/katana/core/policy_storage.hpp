#pragma once

#include "http.hpp"
#include "router.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace katana::http {

using policy_clock = std::chrono::steady_clock;

struct response_snapshot {
    int32_t status = 200;
    std::string reason;
    std::vector<std::pair<std::string, std::string>> headers;
    std::string body;
    bool chunked = false;
};

[[nodiscard]] inline response_snapshot snapshot_response(const response& resp) {
    response_snapshot snapshot;
    snapshot.status = resp.status;
    snapshot.reason = resp.reason;
    snapshot.body = resp.body;
    snapshot.chunked = resp.chunked;
    snapshot.headers.reserve(resp.headers.size());
    for (const auto& [name, value] : resp.headers) {
        snapshot.headers.emplace_back(std::string(name), std::string(value));
    }
    return snapshot;
}

inline void restore_response(const response_snapshot& snapshot, response& out) {
    out.reset();
    out.status = snapshot.status;
    out.reason = snapshot.reason;
    out.body = snapshot.body;
    out.chunked = snapshot.chunked;
    for (const auto& [name, value] : snapshot.headers) {
        out.set_header(name, value);
    }
}

struct idempotency_request_fingerprint {
    method http_method = method::unknown;
    std::string uri;
    std::string content_type;
    size_t body_size = 0;
    uint64_t body_hash = 0;

    [[nodiscard]] bool operator==(const idempotency_request_fingerprint& other) const noexcept {
        return http_method == other.http_method && uri == other.uri &&
               content_type == other.content_type && body_size == other.body_size &&
               body_hash == other.body_hash;
    }
};

class response_cache_store {
public:
    virtual ~response_cache_store() = default;

    virtual std::optional<response_snapshot>
    lookup(const route_policy_view* route_policy, std::string_view cache_key,
           policy_clock::time_point now) = 0;

    virtual void store(const route_policy_view* route_policy,
                       std::string_view cache_key,
                       response_snapshot snapshot,
                       policy_clock::duration ttl,
                       policy_clock::time_point now) = 0;
};

struct response_cache_store_options {
    size_t cleanup_interval = 256;
};

class in_memory_response_cache_store final : public response_cache_store {
public:
    in_memory_response_cache_store()
        : in_memory_response_cache_store(response_cache_store_options{}) {}

    explicit in_memory_response_cache_store(response_cache_store_options opts)
        : cleanup_interval_(std::max<size_t>(1, opts.cleanup_interval)) {}

    std::optional<response_snapshot>
    lookup(const route_policy_view* route_policy,
           std::string_view cache_key,
           policy_clock::time_point now) override {
        std::lock_guard<std::mutex> lock(mutex_);
        maybe_cleanup_locked(now);

        const auto it = entries_.find(make_entry_key(route_policy, cache_key));
        if (it == entries_.end() || it->second.expires_at <= now) {
            return std::nullopt;
        }
        return it->second.snapshot;
    }

    void store(const route_policy_view* route_policy,
               std::string_view cache_key,
               response_snapshot snapshot,
               policy_clock::duration ttl,
               policy_clock::time_point now) override {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_entry entry;
        entry.snapshot = std::move(snapshot);
        entry.expires_at = now + ttl;
        entries_[make_entry_key(route_policy, cache_key)] = std::move(entry);
    }

private:
    struct entry_key {
        const route_policy_view* route_policy = nullptr;
        std::string cache_key;

        [[nodiscard]] bool operator==(const entry_key& other) const noexcept {
            return route_policy == other.route_policy && cache_key == other.cache_key;
        }
    };

    struct entry_key_hash {
        [[nodiscard]] size_t operator()(const entry_key& key) const noexcept {
            const auto route_hash = std::hash<const route_policy_view*>{}(key.route_policy);
            const auto cache_hash = std::hash<std::string>{}(key.cache_key);
            return route_hash ^ (cache_hash + 0x9e3779b97f4a7c15ULL + (route_hash << 6U) +
                                 (route_hash >> 2U));
        }
    };

    struct cache_entry {
        response_snapshot snapshot{};
        policy_clock::time_point expires_at{};
    };

    static entry_key make_entry_key(const route_policy_view* route_policy,
                                    std::string_view cache_key) {
        return entry_key{route_policy, std::string(cache_key)};
    }

    void maybe_cleanup_locked(policy_clock::time_point now) {
        ++requests_since_cleanup_;
        if (requests_since_cleanup_ < cleanup_interval_) {
            return;
        }
        requests_since_cleanup_ = 0;

        for (auto it = entries_.begin(); it != entries_.end();) {
            if (it->second.expires_at <= now) {
                it = entries_.erase(it);
            } else {
                ++it;
            }
        }
    }

    size_t cleanup_interval_ = 1;
    size_t requests_since_cleanup_ = 0;
    std::mutex mutex_;
    std::unordered_map<entry_key, cache_entry, entry_key_hash> entries_;
};

struct rate_limit_decision {
    bool allowed = true;
    size_t retry_after_seconds = 0;
};

class rate_limit_store {
public:
    virtual ~rate_limit_store() = default;

    virtual rate_limit_decision consume(const route_policy_view* route_policy,
                                        std::string_view client_key,
                                        size_t limit,
                                        policy_clock::duration window,
                                        policy_clock::time_point now) = 0;
};

struct rate_limit_store_options {
    size_t cleanup_interval = 256;
};

class in_memory_rate_limit_store final : public rate_limit_store {
public:
    in_memory_rate_limit_store() : in_memory_rate_limit_store(rate_limit_store_options{}) {}

    explicit in_memory_rate_limit_store(rate_limit_store_options opts)
        : cleanup_interval_(std::max<size_t>(1, opts.cleanup_interval)) {}

    rate_limit_decision consume(const route_policy_view* route_policy,
                                std::string_view client_key,
                                size_t limit,
                                policy_clock::duration window,
                                policy_clock::time_point now) override {
        std::lock_guard<std::mutex> lock(mutex_);
        maybe_cleanup_locked(now);

        auto& bucket = buckets_[bucket_key{route_policy, std::string(client_key)}];
        if (bucket.window != window || now >= bucket.reset_at) {
            bucket.count = 0;
            bucket.window = window;
            bucket.reset_at = now + window;
        }

        if (bucket.count >= limit) {
            return rate_limit_decision{false, compute_retry_after_seconds(bucket.reset_at, now)};
        }

        ++bucket.count;
        return rate_limit_decision{};
    }

private:
    struct bucket_key {
        const route_policy_view* route_policy = nullptr;
        std::string client_key;

        [[nodiscard]] bool operator==(const bucket_key& other) const noexcept {
            return route_policy == other.route_policy && client_key == other.client_key;
        }
    };

    struct bucket_key_hash {
        [[nodiscard]] size_t operator()(const bucket_key& key) const noexcept {
            const auto route_hash = std::hash<const route_policy_view*>{}(key.route_policy);
            const auto client_hash = std::hash<std::string>{}(key.client_key);
            return route_hash ^ (client_hash + 0x9e3779b97f4a7c15ULL + (route_hash << 6U) +
                                 (route_hash >> 2U));
        }
    };

    struct bucket_state {
        size_t count = 0;
        policy_clock::duration window{};
        policy_clock::time_point reset_at{};
    };

    static size_t compute_retry_after_seconds(policy_clock::time_point reset_at,
                                              policy_clock::time_point now) noexcept {
        if (reset_at <= now) {
            return 1;
        }
        const auto remaining =
            std::chrono::duration_cast<std::chrono::milliseconds>(reset_at - now).count();
        const auto rounded = (remaining + 999LL) / 1000LL;
        return static_cast<size_t>(std::max<int64_t>(1, rounded));
    }

    void maybe_cleanup_locked(policy_clock::time_point now) {
        ++requests_since_cleanup_;
        if (requests_since_cleanup_ < cleanup_interval_) {
            return;
        }
        requests_since_cleanup_ = 0;

        for (auto it = buckets_.begin(); it != buckets_.end();) {
            const auto expiry_with_grace = it->second.reset_at + it->second.window;
            if (expiry_with_grace <= now) {
                it = buckets_.erase(it);
            } else {
                ++it;
            }
        }
    }

    size_t cleanup_interval_ = 1;
    size_t requests_since_cleanup_ = 0;
    std::mutex mutex_;
    std::unordered_map<bucket_key, bucket_state, bucket_key_hash> buckets_;
};

enum class idempotency_begin_status : uint8_t { started, replay, in_progress, fingerprint_mismatch };

struct idempotency_begin_result {
    idempotency_begin_status status{idempotency_begin_status::started};
    std::optional<response_snapshot> snapshot;
};

class idempotency_store {
public:
    virtual ~idempotency_store() = default;

    virtual idempotency_begin_result begin(const route_policy_view* route_policy,
                                           std::string_view idempotency_key,
                                           const idempotency_request_fingerprint& fingerprint,
                                           policy_clock::time_point now,
                                           policy_clock::duration in_progress_ttl) = 0;

    virtual void complete(const route_policy_view* route_policy,
                          std::string_view idempotency_key,
                          const idempotency_request_fingerprint& fingerprint,
                          response_snapshot snapshot,
                          policy_clock::time_point now,
                          policy_clock::duration completed_ttl) = 0;

    virtual void abandon(const route_policy_view* route_policy,
                         std::string_view idempotency_key) = 0;
};

struct idempotency_store_options {
    size_t cleanup_interval = 256;
};

class in_memory_idempotency_store final : public idempotency_store {
public:
    in_memory_idempotency_store() : in_memory_idempotency_store(idempotency_store_options{}) {}

    explicit in_memory_idempotency_store(idempotency_store_options opts)
        : cleanup_interval_(std::max<size_t>(1, opts.cleanup_interval)) {}

    idempotency_begin_result begin(const route_policy_view* route_policy,
                                   std::string_view idempotency_key,
                                   const idempotency_request_fingerprint& fingerprint,
                                   policy_clock::time_point now,
                                   policy_clock::duration in_progress_ttl) override {
        std::lock_guard<std::mutex> lock(mutex_);
        maybe_cleanup_locked(now);

        auto key = make_entry_key(route_policy, idempotency_key);
        auto it = entries_.find(key);
        if (it == entries_.end()) {
            idempotency_entry entry;
            entry.fingerprint = fingerprint;
            entry.status = entry_status::in_progress;
            entry.expires_at = now + in_progress_ttl;
            entries_.emplace(std::move(key), std::move(entry));
            return {};
        }

        if (!(it->second.fingerprint == fingerprint)) {
            return idempotency_begin_result{idempotency_begin_status::fingerprint_mismatch,
                                            std::nullopt};
        }

        if (it->second.status == entry_status::in_progress) {
            return idempotency_begin_result{idempotency_begin_status::in_progress, std::nullopt};
        }

        return idempotency_begin_result{idempotency_begin_status::replay, it->second.snapshot};
    }

    void complete(const route_policy_view* route_policy,
                  std::string_view idempotency_key,
                  const idempotency_request_fingerprint& fingerprint,
                  response_snapshot snapshot,
                  policy_clock::time_point now,
                  policy_clock::duration completed_ttl) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = entries_.find(make_entry_key(route_policy, idempotency_key));
        if (it == entries_.end()) {
            idempotency_entry entry;
            entry.fingerprint = fingerprint;
            entry.status = entry_status::completed;
            entry.snapshot = std::move(snapshot);
            entry.expires_at = now + completed_ttl;
            entries_.emplace(make_entry_key(route_policy, idempotency_key), std::move(entry));
            return;
        }

        it->second.status = entry_status::completed;
        it->second.fingerprint = fingerprint;
        it->second.snapshot = std::move(snapshot);
        it->second.expires_at = now + completed_ttl;
    }

    void abandon(const route_policy_view* route_policy, std::string_view idempotency_key) override {
        std::lock_guard<std::mutex> lock(mutex_);
        entries_.erase(make_entry_key(route_policy, idempotency_key));
    }

private:
    enum class entry_status : uint8_t { in_progress, completed };

    struct idempotency_entry {
        idempotency_request_fingerprint fingerprint;
        entry_status status{entry_status::in_progress};
        response_snapshot snapshot{};
        policy_clock::time_point expires_at{};
    };

    struct entry_key {
        const route_policy_view* route_policy = nullptr;
        std::string idempotency_key;

        [[nodiscard]] bool operator==(const entry_key& other) const noexcept {
            return route_policy == other.route_policy && idempotency_key == other.idempotency_key;
        }
    };

    struct entry_key_hash {
        [[nodiscard]] size_t operator()(const entry_key& key) const noexcept {
            const auto route_hash = std::hash<const route_policy_view*>{}(key.route_policy);
            const auto id_hash = std::hash<std::string>{}(key.idempotency_key);
            return route_hash ^ (id_hash + 0x9e3779b97f4a7c15ULL + (route_hash << 6U) +
                                 (route_hash >> 2U));
        }
    };

    static entry_key make_entry_key(const route_policy_view* route_policy,
                                    std::string_view idempotency_key) {
        return entry_key{route_policy, std::string(idempotency_key)};
    }

    void maybe_cleanup_locked(policy_clock::time_point now) {
        ++requests_since_cleanup_;
        if (requests_since_cleanup_ < cleanup_interval_) {
            return;
        }
        requests_since_cleanup_ = 0;

        for (auto it = entries_.begin(); it != entries_.end();) {
            if (it->second.expires_at <= now) {
                it = entries_.erase(it);
            } else {
                ++it;
            }
        }
    }

    size_t cleanup_interval_ = 1;
    size_t requests_since_cleanup_ = 0;
    std::mutex mutex_;
    std::unordered_map<entry_key, idempotency_entry, entry_key_hash> entries_;
};

} // namespace katana::http
