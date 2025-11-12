#pragma once

#include "ring_buffer_queue.hpp"
#include <memory>
#include <vector>
#include <atomic>
#include <type_traits>

namespace katana {

template <typename Connection>
class connection_pool {
public:
    explicit connection_pool(size_t initial_capacity = 1024)
        : free_connections_(initial_capacity)
        , allocated_count_(0)
        , reuse_count_(0)
        , fallback_count_(0)
    {
        preallocate(initial_capacity);
    }

    ~connection_pool() = default;

    connection_pool(const connection_pool&) = delete;
    connection_pool& operator=(const connection_pool&) = delete;

    std::shared_ptr<Connection> acquire() {
        Connection* raw_conn = nullptr;
        if (free_connections_.try_pop(raw_conn)) {
            reuse_count_.fetch_add(1, std::memory_order_relaxed);
            return std::shared_ptr<Connection>(raw_conn, [this](Connection* conn) {
                reset_connection(conn);
                free_connections_.push(conn);
            });
        }

        fallback_count_.fetch_add(1, std::memory_order_relaxed);
        allocated_count_.fetch_add(1, std::memory_order_relaxed);

        auto* new_conn = new Connection();
        return std::shared_ptr<Connection>(new_conn, [this](Connection* conn) {
            reset_connection(conn);
            if (!free_connections_.try_push(conn)) {
                delete conn;
                allocated_count_.fetch_sub(1, std::memory_order_relaxed);
            }
        });
    }

    [[nodiscard]] size_t allocated_count() const noexcept {
        return allocated_count_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] size_t reuse_count() const noexcept {
        return reuse_count_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] size_t fallback_count() const noexcept {
        return fallback_count_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] size_t available_count() const noexcept {
        return free_connections_.size();
    }

private:
    void preallocate(size_t count) {
        for (size_t i = 0; i < count; ++i) {
            auto* conn = new Connection();
            free_connections_.push(conn);
            allocated_count_.fetch_add(1, std::memory_order_relaxed);
        }
    }

    void reset_connection(Connection* conn) {
        if constexpr (requires { conn->reset(); }) {
            conn->reset();
        }
    }

    ring_buffer_queue<Connection*> free_connections_;
    std::atomic<size_t> allocated_count_;
    std::atomic<size_t> reuse_count_;
    std::atomic<size_t> fallback_count_;
};

} // namespace katana
