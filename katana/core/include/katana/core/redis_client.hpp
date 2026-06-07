#pragma once

#include "redis_policy_stores.hpp"
#include "result.hpp"
#include "tcp_socket.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_map>
#include <vector>

namespace katana::http {

class redis_sync_client final : public redis_kv_client {
public:
    struct options {
        std::string host = "127.0.0.1";
        uint16_t port = 6379;
        std::chrono::milliseconds connect_timeout{500};
        std::chrono::milliseconds io_timeout{500};
    };

    redis_sync_client();
    explicit redis_sync_client(options opts);
    ~redis_sync_client() override;

    redis_sync_client(redis_sync_client&&) noexcept;
    redis_sync_client& operator=(redis_sync_client&&) noexcept;
    redis_sync_client(const redis_sync_client&) = delete;
    redis_sync_client& operator=(const redis_sync_client&) = delete;

    std::optional<std::string> get(std::string_view key) override;
    void set(std::string_view key, std::string_view value, policy_clock::duration ttl) override;
    bool set_if_absent(std::string_view key,
                       std::string_view value,
                       policy_clock::duration ttl) override;
    void erase(std::string_view key) override;
    int64_t increment(std::string_view key) override;
    void expire(std::string_view key, policy_clock::duration ttl) override;
    std::optional<policy_clock::duration> ttl(std::string_view key) override;

    [[nodiscard]] result<void> connect();
    void disconnect() noexcept;
    [[nodiscard]] bool connected() const noexcept;
    [[nodiscard]] std::error_code last_error() const noexcept { return last_error_; }

private:
    struct resp_reply {
        enum class kind : uint8_t { simple_string, error, integer, bulk_string, null_bulk };

        kind type{kind::simple_string};
        std::string text;
        int64_t integer = 0;
    };

    [[nodiscard]] result<resp_reply> execute(std::span<const std::string_view> args);
    [[nodiscard]] result<void> ensure_connected();
    [[nodiscard]] result<void> send_command(std::span<const std::string_view> args);
    [[nodiscard]] result<resp_reply> read_reply();
    [[nodiscard]] result<void> wait_until_ready(short events,
                                                std::chrono::milliseconds timeout) const;
    [[nodiscard]] result<void> fill_buffer();
    [[nodiscard]] result<std::string_view> read_line(size_t start);
    [[nodiscard]] result<resp_reply> parse_reply(size_t& cursor);
    void consume_buffer(size_t bytes);
    [[nodiscard]] static std::chrono::milliseconds
    to_milliseconds(policy_clock::duration duration) noexcept;

    options options_{};
    katana::tcp_socket socket_{};
    std::string read_buffer_{};
    std::error_code last_error_{};
};

// Per-reactor pool of Redis connections. One redis_sync_client is a single socket with
// no internal locking, so sharing one across reactor threads both serializes every
// request on one round-trip AND races the RESP stream. This pool hands each worker
// thread its own dedicated connection (so policy lookups run in parallel and are
// thread-safe), the same way postgres_pool::current_executor() does.
class redis_pool {
public:
    redis_pool(std::size_t size, redis_sync_client::options opts) {
        const std::size_t n = std::max<std::size_t>(size, 1);
        clients_.reserve(n);
        for (std::size_t i = 0; i < n; ++i) {
            clients_.push_back(std::make_unique<redis_sync_client>(opts));
        }
    }

    [[nodiscard]] result<void> connect_all() {
        for (auto& client : clients_) {
            if (auto connected = client->connect(); !connected) {
                return connected;
            }
        }
        return {};
    }

    // The calling thread's dedicated connection (stable for the thread's lifetime).
    [[nodiscard]] redis_sync_client& current() noexcept {
        static thread_local std::unordered_map<const redis_pool*, std::size_t> slots;
        if (const auto it = slots.find(this); it != slots.end()) {
            return *clients_[it->second];
        }
        const std::size_t slot = next_.fetch_add(1, std::memory_order_relaxed) % clients_.size();
        slots.emplace(this, slot);
        return *clients_[slot];
    }

    [[nodiscard]] redis_sync_client& for_reactor(std::size_t index) noexcept {
        return *clients_[index % clients_.size()];
    }

    [[nodiscard]] std::size_t size() const noexcept { return clients_.size(); }

private:
    std::vector<std::unique_ptr<redis_sync_client>> clients_;
    std::atomic<std::size_t> next_{0};
};

// redis_kv_client adapter that routes every call to the calling thread's pooled
// connection, so the existing policy stores work unchanged on top of redis_pool.
class redis_pool_client final : public redis_kv_client {
public:
    explicit redis_pool_client(redis_pool& pool) noexcept : pool_(pool) {}

    std::optional<std::string> get(std::string_view key) override {
        return pool_.current().get(key);
    }
    void set(std::string_view key, std::string_view value, policy_clock::duration ttl) override {
        pool_.current().set(key, value, ttl);
    }
    bool set_if_absent(std::string_view key, std::string_view value,
                       policy_clock::duration ttl) override {
        return pool_.current().set_if_absent(key, value, ttl);
    }
    void erase(std::string_view key) override { pool_.current().erase(key); }
    int64_t increment(std::string_view key) override { return pool_.current().increment(key); }
    void expire(std::string_view key, policy_clock::duration ttl) override {
        pool_.current().expire(key, ttl);
    }
    std::optional<policy_clock::duration> ttl(std::string_view key) override {
        return pool_.current().ttl(key);
    }

private:
    redis_pool& pool_;
};

} // namespace katana::http
