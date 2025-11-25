#include "katana/core/arena.hpp"

#include <algorithm>
#include <cstdlib>
#include <memory>
#include <new>

namespace katana {

monotonic_arena::block::block(size_t s) noexcept : data(nullptr), size(s), used(0) {
    constexpr size_t alignment = 64;
    size_t aligned_size = (s + alignment - 1) & ~(alignment - 1);
    data = static_cast<uint8_t*>(std::aligned_alloc(alignment, aligned_size));
}

monotonic_arena::block::~block() noexcept {
    if (data) {
        std::free(data);
    }
}

monotonic_arena::block::block(block&& other) noexcept
    : data(other.data), size(other.size), used(other.used) {
    other.data = nullptr;
    other.size = 0;
    other.used = 0;
}

monotonic_arena::block& monotonic_arena::block::operator=(block&& other) noexcept {
    if (this != &other) {
        if (data) {
            std::free(data);
        }
        data = other.data;
        size = other.size;
        used = other.used;
        other.data = nullptr;
        other.size = 0;
        other.used = 0;
    }
    return *this;
}

monotonic_arena::monotonic_arena(size_t block_size) noexcept : block_size_(block_size) {}

monotonic_arena::~monotonic_arena() noexcept = default;

monotonic_arena::monotonic_arena(monotonic_arena&& other) noexcept
    : blocks_(std::move(other.blocks_)), num_blocks_(other.num_blocks_),
      block_size_(other.block_size_), bytes_allocated_(other.bytes_allocated_),
      total_capacity_(other.total_capacity_) {
    other.num_blocks_ = 0;
    other.bytes_allocated_ = 0;
    other.total_capacity_ = 0;
}

monotonic_arena& monotonic_arena::operator=(monotonic_arena&& other) noexcept {
    if (this != &other) {
        blocks_ = std::move(other.blocks_);
        num_blocks_ = other.num_blocks_;
        block_size_ = other.block_size_;
        bytes_allocated_ = other.bytes_allocated_;
        total_capacity_ = other.total_capacity_;
        other.num_blocks_ = 0;
        other.bytes_allocated_ = 0;
        other.total_capacity_ = 0;
    }
    return *this;
}

void monotonic_arena::reset() noexcept {
    for (size_t i = 0; i < num_blocks_; ++i) {
        blocks_[i].used = 0;
    }
    bytes_allocated_ = 0;
}

void* monotonic_arena::allocate(size_t bytes, size_t alignment) noexcept {
    if (bytes == 0) {
        return nullptr;
    }

    if (alignment == 0 || (alignment & (alignment - 1)) != 0 || alignment > MAX_ALIGNMENT) {
        return nullptr;
    }

    for (size_t i = 0; i < num_blocks_; ++i) {
        auto& b = blocks_[i];
        if (b.used >= b.size || !b.data) {
            continue;
        }

        uintptr_t current = reinterpret_cast<uintptr_t>(b.data + b.used);
        uintptr_t aligned = align_up(current, alignment);
        size_t padding = aligned - current;

        if (b.used + padding + bytes <= b.size) {
            b.used += padding + bytes;
            bytes_allocated_ += bytes;
            return reinterpret_cast<void*>(aligned);
        }
    }

    size_t block_size = std::max(block_size_, bytes + MAX_ALIGNMENT);
    if (!allocate_new_block(block_size)) {
        return nullptr;
    }

    auto& b = blocks_[num_blocks_ - 1];
    if (!b.data) {
        return nullptr;
    }

    uintptr_t current = reinterpret_cast<uintptr_t>(b.data);
    uintptr_t aligned = align_up(current, alignment);
    size_t padding = aligned - current;

    if (padding + bytes > b.size) {
        return nullptr;
    }

    b.used = padding + bytes;
    bytes_allocated_ += bytes;

    return reinterpret_cast<void*>(aligned);
}

bool monotonic_arena::allocate_new_block(size_t min_size) noexcept {
    if (num_blocks_ >= MAX_BLOCKS) {
        return false;
    }

    blocks_[num_blocks_] = block(min_size);
    if (!blocks_[num_blocks_].data) {
        return false;
    }

    total_capacity_ += min_size;
    ++num_blocks_;
    return true;
}

} // namespace katana
