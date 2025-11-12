#include "katana/core/arena.hpp"

#include <algorithm>
#include <memory>
#include <cstdlib>
#include <new>

namespace katana {

monotonic_arena::block::block(size_t s) noexcept
    : data(nullptr), size(s), used(0)
{
    data = static_cast<uint8_t*>(std::aligned_alloc(64, s));
}

monotonic_arena::block::~block() noexcept {
    if (data) {
        std::free(data);
    }
}

monotonic_arena::block::block(block&& other) noexcept
    : data(other.data), size(other.size), used(other.used)
{
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

monotonic_arena::monotonic_arena(size_t block_size) noexcept
    : block_size_(block_size)
{
}

monotonic_arena::~monotonic_arena() noexcept = default;

monotonic_arena::monotonic_arena(monotonic_arena&& other) noexcept
    : blocks_(std::move(other.blocks_)),
      block_size_(other.block_size_),
      bytes_allocated_(other.bytes_allocated_),
      total_capacity_(other.total_capacity_)
{
    other.bytes_allocated_ = 0;
    other.total_capacity_ = 0;
}

monotonic_arena& monotonic_arena::operator=(monotonic_arena&& other) noexcept {
    if (this != &other) {
        blocks_ = std::move(other.blocks_);
        block_size_ = other.block_size_;
        bytes_allocated_ = other.bytes_allocated_;
        total_capacity_ = other.total_capacity_;
        other.bytes_allocated_ = 0;
        other.total_capacity_ = 0;
    }
    return *this;
}

void monotonic_arena::reset() noexcept {
    for (auto& b : blocks_) {
        b.used = 0;
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

    for (auto& b : blocks_) {
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

    auto& b = blocks_.back();
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
    blocks_.emplace_back(min_size);
    if (!blocks_.back().data) {
        blocks_.pop_back();
        return false;
    }
    total_capacity_ += min_size;
    return true;
}

} // namespace katana
