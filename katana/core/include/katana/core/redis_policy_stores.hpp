#pragma once

#include "policy_storage.hpp"

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_map>
#include <utility>

namespace katana::http {

class redis_kv_client {
public:
    virtual ~redis_kv_client() = default;

    virtual std::optional<std::string> get(std::string_view key) = 0;
    virtual void set(std::string_view key, std::string_view value, policy_clock::duration ttl) = 0;
    virtual bool set_if_absent(std::string_view key,
                               std::string_view value,
                               policy_clock::duration ttl) = 0;
    virtual void erase(std::string_view key) = 0;
    virtual int64_t increment(std::string_view key) = 0;
    virtual void expire(std::string_view key, policy_clock::duration ttl) = 0;
    virtual std::optional<policy_clock::duration> ttl(std::string_view key) = 0;
};

class in_memory_redis_kv_client final : public redis_kv_client {
public:
    std::optional<std::string> get(std::string_view key) override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto now = policy_clock::now();
        auto* cached_entry = get_entry_locked(key, now);
        if (cached_entry == nullptr) {
            return std::nullopt;
        }
        return cached_entry->value;
    }

    void set(std::string_view key, std::string_view value, policy_clock::duration ttl_value) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& stored_entry = entries_[std::string(key)];
        stored_entry.value.assign(value.data(), value.size());
        stored_entry.expires_at = policy_clock::now() + ttl_value;
    }

    bool set_if_absent(std::string_view key,
                       std::string_view value,
                       policy_clock::duration ttl_value) override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto now = policy_clock::now();
        auto it = entries_.find(std::string(key));
        if (it != entries_.end()) {
            if (it->second.expires_at != policy_clock::time_point{} && it->second.expires_at <= now) {
                entries_.erase(it);
            } else {
                return false;
            }
        }

        entry stored;
        stored.value.assign(value.data(), value.size());
        stored.expires_at = now + ttl_value;
        entries_.emplace(std::string(key), std::move(stored));
        return true;
    }

    void erase(std::string_view key) override {
        std::lock_guard<std::mutex> lock(mutex_);
        entries_.erase(std::string(key));
    }

    int64_t increment(std::string_view key) override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto now = policy_clock::now();
        auto& stored_entry = entries_[std::string(key)];
        if (stored_entry.expires_at != policy_clock::time_point{} &&
            stored_entry.expires_at <= now) {
            stored_entry.value = "0";
            stored_entry.expires_at = policy_clock::time_point{};
        }

        int64_t current = 0;
        if (!stored_entry.value.empty()) {
            const auto* first = stored_entry.value.data();
            const auto* last = stored_entry.value.data() + stored_entry.value.size();
            auto [end, ec] = std::from_chars(first, last, current);
            if (ec != std::errc() || end != last) {
                current = 0;
            }
        }
        ++current;
        stored_entry.value = std::to_string(current);
        return current;
    }

    void expire(std::string_view key, policy_clock::duration ttl_value) override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto now = policy_clock::now();
        auto* cached_entry = get_entry_locked(key, now);
        if (cached_entry == nullptr) {
            return;
        }
        cached_entry->expires_at = now + ttl_value;
    }

    std::optional<policy_clock::duration> ttl(std::string_view key) override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto now = policy_clock::now();
        auto* cached_entry = get_entry_locked(key, now);
        if (cached_entry == nullptr || cached_entry->expires_at == policy_clock::time_point{}) {
            return std::nullopt;
        }
        return cached_entry->expires_at - now;
    }

private:
    struct entry {
        std::string value;
        policy_clock::time_point expires_at{};
    };

    entry* get_entry_locked(std::string_view key, policy_clock::time_point now) {
        auto it = entries_.find(std::string(key));
        if (it == entries_.end()) {
            return nullptr;
        }
        if (it->second.expires_at != policy_clock::time_point{} && it->second.expires_at <= now) {
            entries_.erase(it);
            return nullptr;
        }
        return &it->second;
    }

    std::mutex mutex_;
    std::unordered_map<std::string, entry> entries_;
};

struct redis_store_options {
    std::string prefix = "katana:policy";
};

namespace detail {

inline void append_u8(std::string& out, uint8_t value) { out.push_back(static_cast<char>(value)); }

inline void append_u32(std::string& out, uint32_t value) {
    for (int shift = 24; shift >= 0; shift -= 8) {
        out.push_back(static_cast<char>((value >> shift) & 0xffU));
    }
}

inline void append_u64(std::string& out, uint64_t value) {
    for (int shift = 56; shift >= 0; shift -= 8) {
        out.push_back(static_cast<char>((value >> shift) & 0xffU));
    }
}

inline void append_string(std::string& out, std::string_view value) {
    append_u32(out, static_cast<uint32_t>(value.size()));
    out.append(value.data(), value.size());
}

inline bool read_u8(std::string_view& in, uint8_t& value) {
    if (in.empty()) {
        return false;
    }
    value = static_cast<uint8_t>(in.front());
    in.remove_prefix(1);
    return true;
}

inline bool read_u32(std::string_view& in, uint32_t& value) {
    if (in.size() < sizeof(uint32_t)) {
        return false;
    }
    value = 0;
    for (size_t i = 0; i < sizeof(uint32_t); ++i) {
        value = static_cast<uint32_t>((value << 8U) |
                                      static_cast<uint8_t>(in[i]));
    }
    in.remove_prefix(sizeof(uint32_t));
    return true;
}

inline bool read_u64(std::string_view& in, uint64_t& value) {
    if (in.size() < sizeof(uint64_t)) {
        return false;
    }
    value = 0;
    for (size_t i = 0; i < sizeof(uint64_t); ++i) {
        value = (value << 8U) | static_cast<uint8_t>(in[i]);
    }
    in.remove_prefix(sizeof(uint64_t));
    return true;
}

inline bool read_string(std::string_view& in, std::string& value) {
    uint32_t size = 0;
    if (!read_u32(in, size) || in.size() < size) {
        return false;
    }
    value.assign(in.data(), size);
    in.remove_prefix(size);
    return true;
}

inline std::string hex_encode(std::string_view value) {
    static constexpr char kHex[] = "0123456789abcdef";
    std::string encoded;
    encoded.resize(value.size() * 2);
    for (size_t i = 0; i < value.size(); ++i) {
        const auto byte = static_cast<unsigned char>(value[i]);
        encoded[i * 2] = kHex[byte >> 4U];
        encoded[i * 2 + 1] = kHex[byte & 0x0fU];
    }
    return encoded;
}

inline std::string route_scope_fragment(const route_policy_view* route_policy) {
    if (route_policy != nullptr && !route_policy->scope.empty()) {
        return hex_encode(route_policy->scope);
    }

    if (route_policy == nullptr) {
        return "global";
    }

    char buffer[2 + sizeof(uintptr_t) * 2]{};
    std::snprintf(buffer, sizeof(buffer), "%p", static_cast<const void*>(route_policy));
    return hex_encode(std::string_view(buffer, std::strlen(buffer)));
}

inline std::string encode_response_snapshot(const response_snapshot& snapshot) {
    std::string encoded;
    append_u32(encoded, static_cast<uint32_t>(snapshot.status));
    append_string(encoded, snapshot.reason);
    append_u32(encoded, static_cast<uint32_t>(snapshot.headers.size()));
    for (const auto& [name, value] : snapshot.headers) {
        append_string(encoded, name);
        append_string(encoded, value);
    }
    append_string(encoded, snapshot.body);
    append_u8(encoded, snapshot.chunked ? 1U : 0U);
    return encoded;
}

inline std::optional<response_snapshot> decode_response_snapshot(std::string_view encoded) {
    response_snapshot snapshot;
    uint32_t status = 0;
    if (!read_u32(encoded, status)) {
        return std::nullopt;
    }
    snapshot.status = static_cast<int32_t>(status);
    if (!read_string(encoded, snapshot.reason)) {
        return std::nullopt;
    }
    uint32_t header_count = 0;
    if (!read_u32(encoded, header_count)) {
        return std::nullopt;
    }
    snapshot.headers.reserve(header_count);
    for (uint32_t i = 0; i < header_count; ++i) {
        std::string name;
        std::string value;
        if (!read_string(encoded, name) || !read_string(encoded, value)) {
            return std::nullopt;
        }
        snapshot.headers.emplace_back(std::move(name), std::move(value));
    }
    if (!read_string(encoded, snapshot.body)) {
        return std::nullopt;
    }
    uint8_t chunked = 0;
    if (!read_u8(encoded, chunked) || !encoded.empty()) {
        return std::nullopt;
    }
    snapshot.chunked = chunked != 0;
    return snapshot;
}

struct redis_idempotency_record {
    bool completed = false;
    idempotency_request_fingerprint fingerprint{};
    std::optional<response_snapshot> snapshot;
};

inline std::string
encode_idempotency_record(const redis_idempotency_record& record) {
    std::string encoded;
    append_u8(encoded, record.completed ? 1U : 0U);
    append_u8(encoded, static_cast<uint8_t>(record.fingerprint.http_method));
    append_string(encoded, record.fingerprint.uri);
    append_string(encoded, record.fingerprint.content_type);
    append_u64(encoded, static_cast<uint64_t>(record.fingerprint.body_size));
    append_u64(encoded, record.fingerprint.body_hash);
    append_u8(encoded, record.snapshot.has_value() ? 1U : 0U);
    if (record.snapshot.has_value()) {
        append_string(encoded, encode_response_snapshot(*record.snapshot));
    }
    return encoded;
}

inline std::optional<redis_idempotency_record>
decode_idempotency_record(std::string_view encoded) {
    redis_idempotency_record record;
    uint8_t completed = 0;
    uint8_t http_method = 0;
    uint64_t body_size = 0;
    uint64_t body_hash = 0;
    uint8_t has_snapshot = 0;
    if (!read_u8(encoded, completed) || !read_u8(encoded, http_method)) {
        return std::nullopt;
    }
    record.completed = completed != 0;
    record.fingerprint.http_method = static_cast<method>(http_method);
    if (!read_string(encoded, record.fingerprint.uri) ||
        !read_string(encoded, record.fingerprint.content_type) ||
        !read_u64(encoded, body_size) || !read_u64(encoded, body_hash) ||
        !read_u8(encoded, has_snapshot)) {
        return std::nullopt;
    }
    record.fingerprint.body_size = static_cast<size_t>(body_size);
    record.fingerprint.body_hash = body_hash;
    if (has_snapshot != 0) {
        std::string snapshot_payload;
        if (!read_string(encoded, snapshot_payload)) {
            return std::nullopt;
        }
        record.snapshot = decode_response_snapshot(snapshot_payload);
        if (!record.snapshot.has_value()) {
            return std::nullopt;
        }
    }
    if (!encoded.empty()) {
        return std::nullopt;
    }
    return record;
}

inline std::string build_redis_key(std::string_view prefix,
                                   std::string_view category,
                                   const route_policy_view* route_policy,
                                   std::string_view value) {
    std::string key;
    key.reserve(prefix.size() + category.size() + value.size() * 2 + 32);
    key.append(prefix);
    key.push_back(':');
    key.append(category);
    key.push_back(':');
    key.append(route_scope_fragment(route_policy));
    key.push_back(':');
    key.append(hex_encode(value));
    return key;
}

inline size_t
duration_to_retry_after_seconds(policy_clock::duration duration) noexcept {
    const auto remaining =
        std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    const auto rounded = (remaining + 999LL) / 1000LL;
    return static_cast<size_t>(std::max<int64_t>(1, rounded));
}

} // namespace detail

class redis_response_cache_store final : public response_cache_store {
public:
    redis_response_cache_store(redis_kv_client& client, redis_store_options opts = {})
        : client_(client), options_(std::move(opts)) {}

    std::optional<response_snapshot>
    lookup(const route_policy_view* route_policy,
           std::string_view cache_key,
           policy_clock::time_point) override {
        auto value = client_.get(detail::build_redis_key(options_.prefix, "cache", route_policy,
                                                         cache_key));
        if (!value.has_value()) {
            return std::nullopt;
        }
        return detail::decode_response_snapshot(*value);
    }

    void store(const route_policy_view* route_policy,
               std::string_view cache_key,
               response_snapshot snapshot,
               policy_clock::duration ttl,
               policy_clock::time_point) override {
        client_.set(detail::build_redis_key(options_.prefix, "cache", route_policy, cache_key),
                    detail::encode_response_snapshot(snapshot),
                    ttl);
    }

private:
    redis_kv_client& client_;
    redis_store_options options_;
};

class redis_rate_limit_store final : public rate_limit_store {
public:
    redis_rate_limit_store(redis_kv_client& client, redis_store_options opts = {})
        : client_(client), options_(std::move(opts)) {}

    rate_limit_decision consume(const route_policy_view* route_policy,
                                std::string_view client_key,
                                size_t limit,
                                policy_clock::duration window,
                                policy_clock::time_point) override {
        const auto key =
            detail::build_redis_key(options_.prefix, "rate-limit", route_policy, client_key);
        const auto count = client_.increment(key);
        if (count == 1) {
            client_.expire(key, window);
        }
        if (count <= static_cast<int64_t>(limit)) {
            return {};
        }

        const auto ttl = client_.ttl(key).value_or(window);
        return rate_limit_decision{false, detail::duration_to_retry_after_seconds(ttl)};
    }

private:
    redis_kv_client& client_;
    redis_store_options options_;
};

class redis_idempotency_store final : public idempotency_store {
public:
    redis_idempotency_store(redis_kv_client& client, redis_store_options opts = {})
        : client_(client), options_(std::move(opts)) {}

    idempotency_begin_result begin(const route_policy_view* route_policy,
                                   std::string_view idempotency_key,
                                   const idempotency_request_fingerprint& fingerprint,
                                   policy_clock::time_point,
                                   policy_clock::duration in_progress_ttl) override {
        const auto key =
            detail::build_redis_key(options_.prefix, "idempotency", route_policy, idempotency_key);
        const detail::redis_idempotency_record in_progress{
            false,
            fingerprint,
            std::nullopt,
        };
        const auto encoded = detail::encode_idempotency_record(in_progress);
        if (client_.set_if_absent(key, encoded, in_progress_ttl)) {
            return {};
        }

        auto existing = client_.get(key);
        if (!existing.has_value()) {
            if (client_.set_if_absent(key, encoded, in_progress_ttl)) {
                return {};
            }
            existing = client_.get(key);
            if (!existing.has_value()) {
                return idempotency_begin_result{idempotency_begin_status::in_progress,
                                                std::nullopt};
            }
        }

        const auto record = detail::decode_idempotency_record(*existing);
        if (!record.has_value()) {
            return idempotency_begin_result{idempotency_begin_status::in_progress, std::nullopt};
        }
        if (!(record->fingerprint == fingerprint)) {
            return idempotency_begin_result{idempotency_begin_status::fingerprint_mismatch,
                                            std::nullopt};
        }
        if (!record->completed || !record->snapshot.has_value()) {
            return idempotency_begin_result{idempotency_begin_status::in_progress, std::nullopt};
        }
        return idempotency_begin_result{idempotency_begin_status::replay, record->snapshot};
    }

    void complete(const route_policy_view* route_policy,
                  std::string_view idempotency_key,
                  const idempotency_request_fingerprint& fingerprint,
                  response_snapshot snapshot,
                  policy_clock::time_point,
                  policy_clock::duration completed_ttl) override {
        detail::redis_idempotency_record record;
        record.completed = true;
        record.fingerprint = fingerprint;
        record.snapshot = std::move(snapshot);
        const auto key =
            detail::build_redis_key(options_.prefix, "idempotency", route_policy, idempotency_key);

        client_.set(key, detail::encode_idempotency_record(record), completed_ttl);
    }

    void abandon(const route_policy_view* route_policy, std::string_view idempotency_key) override {
        client_.erase(
            detail::build_redis_key(options_.prefix, "idempotency", route_policy, idempotency_key));
    }

private:
    redis_kv_client& client_;
    redis_store_options options_;
};

} // namespace katana::http
