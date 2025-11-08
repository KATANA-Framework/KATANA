#include "katana/core/arena.hpp"

#include <algorithm>
#include <memory>
#include <cstdlib>
#include <bit>

namespace katana {

monotonic_arena::monotonic_arena(size_t block_size)
    : block_size_(block_size)
{
}

monotonic_arena::~monotonic_arena() = default;

void monotonic_arena::reset() noexcept {
    for (auto& b : blocks_) {
        b.used = 0;
    }
    bytes_allocated_ = 0;
}

void* monotonic_arena::do_allocate(size_t bytes, size_t alignment) {
    if (bytes == 0) {
        return nullptr;
    }

    if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
        throw std::bad_alloc();
    }

    for (auto& b : blocks_) {
        if (b.used >= b.size) {
            continue;
        }

        void* ptr = b.data.data() + b.used;
        size_t space = b.size - b.used;
        void* aligned_ptr = std::align(alignment, bytes, ptr, space);

        if (aligned_ptr && space >= bytes) {
            std::ptrdiff_t padding_diff = static_cast<uint8_t*>(aligned_ptr) - (b.data.data() + b.used);
            size_t padding = static_cast<size_t>(padding_diff);
            if (padding > SIZE_MAX - bytes || b.used > SIZE_MAX - (padding + bytes)) {
                throw std::bad_alloc();
            }
            b.used += padding + bytes;
            bytes_allocated_ += bytes;
            return aligned_ptr;
        }
    }

    size_t block_size = std::max(block_size_, bytes + alignment - 1);
    allocate_new_block(block_size);

    auto& b = blocks_.back();
    void* ptr = b.data.data();
    size_t space = b.size;
    void* aligned_ptr = std::align(alignment, bytes, ptr, space);

    if (!aligned_ptr || space < bytes) {
        throw std::bad_alloc();
    }

    std::ptrdiff_t padding_diff = static_cast<uint8_t*>(aligned_ptr) - b.data.data();
    size_t padding = static_cast<size_t>(padding_diff);
    if (padding > SIZE_MAX - bytes) {
        throw std::bad_alloc();
    }
    b.used = padding + bytes;
    bytes_allocated_ += bytes;

    return aligned_ptr;
}

void monotonic_arena::do_deallocate(void*, size_t, size_t) {
    // Monotonic arena doesn't support individual deallocation
}

bool monotonic_arena::do_is_equal(const memory_resource& other) const noexcept {
    return this == &other;
}

void monotonic_arena::allocate_new_block(size_t min_size) {
    blocks_.push_back(block{std::vector<uint8_t>(min_size), min_size, 0});
    total_capacity_ += min_size;
}

} // namespace katana
