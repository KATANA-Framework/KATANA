#include "katana/core/arena_pool.hpp"

#include <algorithm>

namespace katana {

bounded_arena_pool::bounded_arena_pool(size_t max_memory_bytes)
    : max_memory_(max_memory_bytes)
{
}

bounded_arena_pool::~bounded_arena_pool() {
    for (auto* arena : free_list_) {
        delete arena;
    }
    free_list_.clear();
}

monotonic_arena* bounded_arena_pool::acquire() {
    ++total_acquires_;

    if (!free_list_.empty()) {
        auto* arena = free_list_.back();
        free_list_.pop_back();
        return arena;
    }

    if (current_memory_ + DEFAULT_ARENA_SIZE > max_memory_) {
        return nullptr;
    }

    auto* arena = new (std::nothrow) monotonic_arena(DEFAULT_ARENA_SIZE);
    if (!arena) {
        return nullptr;
    }

    current_memory_ += DEFAULT_ARENA_SIZE;
    if (current_memory_ > peak_memory_) {
        peak_memory_ = current_memory_;
    }

    return arena;
}

void bounded_arena_pool::release(monotonic_arena* arena) {
    if (!arena) {
        return;
    }

    ++total_releases_;

    arena->reset();

    size_t arena_capacity = arena->total_capacity();
    if (arena_capacity > SHRINK_THRESHOLD) {
        size_t old_capacity = arena_capacity;
        arena->shrink_to(DEFAULT_ARENA_SIZE);
        size_t new_capacity = arena->total_capacity();

        if (new_capacity < old_capacity) {
            current_memory_ -= (old_capacity - new_capacity);
        }
    }

    if (free_list_.size() >= MAX_POOL_SIZE) {
        size_t released_capacity = arena->total_capacity();
        current_memory_ -= released_capacity;
        delete arena;
        return;
    }

    free_list_.push_back(arena);
}

bounded_arena_pool::scoped_arena bounded_arena_pool::acquire_scoped() {
    return scoped_arena(this, acquire());
}

bounded_arena_pool::scoped_arena::scoped_arena(bounded_arena_pool* pool, monotonic_arena* arena)
    : pool_(pool), arena_(arena)
{
}

bounded_arena_pool::scoped_arena::scoped_arena(scoped_arena&& other) noexcept
    : pool_(other.pool_), arena_(other.arena_)
{
    other.pool_ = nullptr;
    other.arena_ = nullptr;
}

bounded_arena_pool::scoped_arena& bounded_arena_pool::scoped_arena::operator=(scoped_arena&& other) noexcept {
    if (this != &other) {
        if (arena_ && pool_) {
            pool_->release(arena_);
        }
        pool_ = other.pool_;
        arena_ = other.arena_;
        other.pool_ = nullptr;
        other.arena_ = nullptr;
    }
    return *this;
}

bounded_arena_pool::scoped_arena::~scoped_arena() {
    if (arena_ && pool_) {
        pool_->release(arena_);
    }
}

} // namespace katana
