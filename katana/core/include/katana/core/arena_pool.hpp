#pragma once

#include "arena.hpp"

#include <vector>
#include <cstddef>
#include <cstdint>

namespace katana {

class bounded_arena_pool {
public:
    explicit bounded_arena_pool(size_t max_memory_bytes);
    ~bounded_arena_pool();

    bounded_arena_pool(const bounded_arena_pool&) = delete;
    bounded_arena_pool& operator=(const bounded_arena_pool&) = delete;
    bounded_arena_pool(bounded_arena_pool&&) = delete;
    bounded_arena_pool& operator=(bounded_arena_pool&&) = delete;

    [[nodiscard]] monotonic_arena* acquire();
    void release(monotonic_arena* arena);

    class scoped_arena {
    public:
        scoped_arena() = default;
        scoped_arena(scoped_arena&& other) noexcept;
        scoped_arena& operator=(scoped_arena&& other) noexcept;
        ~scoped_arena();

        scoped_arena(const scoped_arena&) = delete;
        scoped_arena& operator=(const scoped_arena&) = delete;

        [[nodiscard]] monotonic_arena* get() const noexcept { return arena_; }
        [[nodiscard]] monotonic_arena* operator->() const noexcept { return arena_; }

    private:
        friend class bounded_arena_pool;
        scoped_arena(bounded_arena_pool* pool, monotonic_arena* arena);

        bounded_arena_pool* pool_ = nullptr;
        monotonic_arena* arena_ = nullptr;
    };

    [[nodiscard]] scoped_arena acquire_scoped();

    [[nodiscard]] size_t current_memory() const noexcept { return current_memory_; }
    [[nodiscard]] size_t peak_memory() const noexcept { return peak_memory_; }
    [[nodiscard]] size_t pool_size() const noexcept { return free_list_.size(); }
    [[nodiscard]] size_t total_acquires() const noexcept { return total_acquires_; }
    [[nodiscard]] size_t total_releases() const noexcept { return total_releases_; }

private:
    std::vector<monotonic_arena*> free_list_;
    size_t max_memory_;
    size_t current_memory_ = 0;
    size_t peak_memory_ = 0;
    size_t total_acquires_ = 0;
    size_t total_releases_ = 0;

    static constexpr size_t DEFAULT_ARENA_SIZE = 64 * 1024;
    static constexpr size_t SHRINK_THRESHOLD = 256 * 1024;
    static constexpr size_t MAX_POOL_SIZE = 200;
};

} // namespace katana
