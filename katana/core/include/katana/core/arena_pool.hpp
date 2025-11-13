#pragma once

#include "arena.hpp"

#include <vector>
#include <atomic>
#include <cstddef>

namespace katana {

// Pool of monotonic arenas with bounded memory budget
// acquire/release must be called from single thread (reactor thread)
// Metrics can be read from any thread (atomics ensure visibility)
class bounded_arena_pool {
public:
    static constexpr size_t DEFAULT_ARENA_SIZE = 64 * 1024;
    static constexpr size_t SHRINK_THRESHOLD = 256 * 1024;
    static constexpr size_t MAX_POOL_SIZE = 200;
    static constexpr size_t DEFAULT_BUDGET = 100 * 1024 * 1024; // 100MB

    class scoped_arena {
    public:
        ~scoped_arena() noexcept;

        scoped_arena(const scoped_arena&) = delete;
        scoped_arena& operator=(const scoped_arena&) = delete;

        scoped_arena(scoped_arena&& other) noexcept;
        scoped_arena& operator=(scoped_arena&& other) noexcept;

        [[nodiscard]] monotonic_arena* get() const noexcept { return arena_; }
        [[nodiscard]] monotonic_arena* operator->() const noexcept { return arena_; }
        [[nodiscard]] explicit operator bool() const noexcept { return arena_ != nullptr; }

    private:
        friend class bounded_arena_pool;
        scoped_arena(bounded_arena_pool* pool, monotonic_arena* arena) noexcept;

        monotonic_arena* arena_;
        bounded_arena_pool* pool_;
    };

    explicit bounded_arena_pool(size_t max_memory_bytes = DEFAULT_BUDGET);
    ~bounded_arena_pool() noexcept;

    bounded_arena_pool(const bounded_arena_pool&) = delete;
    bounded_arena_pool& operator=(const bounded_arena_pool&) = delete;
    bounded_arena_pool(bounded_arena_pool&&) = delete;
    bounded_arena_pool& operator=(bounded_arena_pool&&) = delete;

    // Acquire arena from pool (nullptr if OOM)
    [[nodiscard]] monotonic_arena* acquire() noexcept;

    // Return arena to pool
    void release(monotonic_arena* arena) noexcept;

    // RAII wrapper for automatic release
    [[nodiscard]] scoped_arena acquire_scoped() noexcept;

    // Metrics
    [[nodiscard]] size_t current_memory() const noexcept {
        return current_memory_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] size_t peak_memory() const noexcept {
        return peak_memory_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] size_t pool_size() const noexcept {
        return free_list_.size();
    }

    [[nodiscard]] size_t total_acquires() const noexcept {
        return total_acquires_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] size_t total_releases() const noexcept {
        return total_releases_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] size_t oom_count() const noexcept {
        return oom_count_.load(std::memory_order_relaxed);
    }

private:
    std::vector<monotonic_arena*> free_list_;
    const size_t max_memory_;
    std::atomic<size_t> current_memory_{0};
    std::atomic<size_t> peak_memory_{0};
    std::atomic<size_t> total_acquires_{0};
    std::atomic<size_t> total_releases_{0};
    std::atomic<size_t> oom_count_{0};
};

} // namespace katana
