#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace katana {

class monotonic_arena {
public:
    static constexpr size_t DEFAULT_BLOCK_SIZE = 64UL * 1024UL;
    static constexpr size_t MAX_ALIGNMENT = 64;
    static constexpr size_t MAX_BLOCKS = 32;

    explicit monotonic_arena(size_t block_size = DEFAULT_BLOCK_SIZE) noexcept;
    ~monotonic_arena() noexcept;

    monotonic_arena(const monotonic_arena&) = delete;
    monotonic_arena& operator=(const monotonic_arena&) = delete;
    monotonic_arena(monotonic_arena&&) noexcept;
    monotonic_arena& operator=(monotonic_arena&&) noexcept;

    [[nodiscard]] void* allocate(size_t bytes,
                                 size_t alignment = alignof(std::max_align_t)) noexcept;

    template <typename T> [[nodiscard]] T* allocate_array(size_t count) noexcept {
        return static_cast<T*>(allocate(sizeof(T) * count, alignof(T)));
    }

    [[nodiscard]] char* allocate_string(std::string_view str) noexcept {
        char* ptr = static_cast<char*>(allocate(str.size() + 1, 1));
        if (ptr) {
            std::memcpy(ptr, str.data(), str.size());
            ptr[str.size()] = '\0';
        }
        return ptr;
    }

    void reset() noexcept;

    [[nodiscard]] size_t bytes_allocated() const noexcept { return bytes_allocated_; }
    [[nodiscard]] size_t total_capacity() const noexcept { return total_capacity_; }

private:
    struct block {
        uint8_t* data;
        size_t size;
        size_t used;

        block() noexcept : data(nullptr), size(0), used(0) {}
        block(size_t s) noexcept;
        ~block() noexcept;

        block(const block&) = delete;
        block& operator=(const block&) = delete;
        block(block&& other) noexcept;
        block& operator=(block&& other) noexcept;
    };

    [[nodiscard]] static constexpr size_t align_up(size_t n, size_t alignment) noexcept {
        return (n + alignment - 1) & ~(alignment - 1);
    }

    [[nodiscard]] bool allocate_new_block(size_t min_size) noexcept;

    std::array<block, MAX_BLOCKS> blocks_;
    size_t num_blocks_ = 0;
    size_t block_size_;
    size_t bytes_allocated_ = 0;
    size_t total_capacity_ = 0;
};

template <typename T> class arena_allocator {
public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    explicit arena_allocator(monotonic_arena* arena) noexcept : arena_(arena) {}

    template <typename U>
    arena_allocator(const arena_allocator<U>& other) noexcept : arena_(other.arena_) {}

    [[nodiscard]] T* allocate(size_t n) {
        return static_cast<T*>(arena_->allocate(n * sizeof(T), alignof(T)));
    }

    void deallocate(T*, size_t) noexcept {}

    template <typename U> bool operator==(const arena_allocator<U>& other) const noexcept {
        return arena_ == other.arena_;
    }

    template <typename U> bool operator!=(const arena_allocator<U>& other) const noexcept {
        return arena_ != other.arena_;
    }

    monotonic_arena* arena_;
};

template <typename T> using arena_vector = std::vector<T, arena_allocator<T>>;

template <typename CharT = char>
using arena_string = std::basic_string<CharT, std::char_traits<CharT>, arena_allocator<CharT>>;

using arena_string_view = std::string_view;

} // namespace katana
