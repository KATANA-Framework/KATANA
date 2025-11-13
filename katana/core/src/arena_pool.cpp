#include "katana/core/arena_pool.hpp"

#include <algorithm>

namespace katana {

bounded_arena_pool::bounded_arena_pool(size_t max_memory_bytes)
    : max_memory_(max_memory_bytes)
{
    free_list_.reserve(MAX_POOL_SIZE);
}

bounded_arena_pool::~bounded_arena_pool() noexcept {
    for (auto* arena : free_list_) {
        delete arena;
    }
    free_list_.clear();
}

monotonic_arena* bounded_arena_pool::acquire() noexcept {
    total_acquires_.fetch_add(1, std::memory_order_relaxed);

    if (!free_list_.empty()) {
        auto* arena = free_list_.back();
        free_list_.pop_back();
        return arena;
    }

    // Check memory budget before creating new arena
    size_t current = current_memory_.load(std::memory_order_relaxed);
    if (current + DEFAULT_ARENA_SIZE > max_memory_) {
        oom_count_.fetch_add(1, std::memory_order_relaxed);
        return nullptr;
    }

    // Create new arena
    auto* arena = new (std::nothrow) monotonic_arena(DEFAULT_ARENA_SIZE);
    if (!arena) {
        oom_count_.fetch_add(1, std::memory_order_relaxed);
        return nullptr;
    }

    // Update memory tracking
    size_t new_current = current_memory_.fetch_add(DEFAULT_ARENA_SIZE, std::memory_order_relaxed) + DEFAULT_ARENA_SIZE;

    // Update peak if needed
    size_t current_peak = peak_memory_.load(std::memory_order_relaxed);
    while (new_current > current_peak) {
        if (peak_memory_.compare_exchange_weak(current_peak, new_current, std::memory_order_relaxed)) {
            break;
        }
    }

    return arena;
}

void bounded_arena_pool::release(monotonic_arena* arena) noexcept {
    if (!arena) {
        return;
    }

    total_releases_.fetch_add(1, std::memory_order_relaxed);

    arena->reset();

    // Always shrink to DEFAULT_ARENA_SIZE to maintain budget consistency
    if (arena->total_capacity() > DEFAULT_ARENA_SIZE) {
        arena->shrink_to(DEFAULT_ARENA_SIZE);
    }

    // Return to pool or delete if pool is full
    if (free_list_.size() < MAX_POOL_SIZE) {
        free_list_.push_back(arena);
    } else {
        // Each arena is counted as DEFAULT_ARENA_SIZE in budget (conservative model)
        current_memory_.fetch_sub(DEFAULT_ARENA_SIZE, std::memory_order_relaxed);
        delete arena;
    }
}

bounded_arena_pool::scoped_arena bounded_arena_pool::acquire_scoped() noexcept {
    return scoped_arena(this, acquire());
}

// scoped_arena implementation

bounded_arena_pool::scoped_arena::scoped_arena(bounded_arena_pool* pool, monotonic_arena* arena) noexcept
    : arena_(arena)
    , pool_(pool)
{
}

bounded_arena_pool::scoped_arena::~scoped_arena() noexcept {
    if (pool_ && arena_) {
        pool_->release(arena_);
    }
}

bounded_arena_pool::scoped_arena::scoped_arena(scoped_arena&& other) noexcept
    : arena_(other.arena_)
    , pool_(other.pool_)
{
    other.arena_ = nullptr;
    other.pool_ = nullptr;
}

bounded_arena_pool::scoped_arena& bounded_arena_pool::scoped_arena::operator=(scoped_arena&& other) noexcept {
    if (this != &other) {
        if (pool_ && arena_) {
            pool_->release(arena_);
        }
        arena_ = other.arena_;
        pool_ = other.pool_;
        other.arena_ = nullptr;
        other.pool_ = nullptr;
    }
    return *this;
}

} // namespace katana
