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

    for (auto& b : blocks_) {
        uintptr_t addr = std::bit_cast<uintptr_t>(b.data.get()) + b.used;
        uintptr_t aligned_addr = (addr + alignment - 1) & ~(alignment - 1);
        size_t padding = aligned_addr - addr;
        size_t total = padding + bytes;

        if (b.used + total <= b.size) {
            b.used += total;
            bytes_allocated_ += bytes;
            return std::bit_cast<void*>(aligned_addr);
        }
    }

    size_t block_size = std::max(block_size_, bytes + alignof(std::max_align_t));
    allocate_new_block(block_size);

    auto& b = blocks_.back();
    uintptr_t addr = std::bit_cast<uintptr_t>(b.data.get());
    uintptr_t aligned_addr = (addr + alignment - 1) & ~(alignment - 1);
    size_t padding = aligned_addr - addr;
    size_t total = padding + bytes;

    b.used = total;
    bytes_allocated_ += bytes;

    return std::bit_cast<void*>(aligned_addr);
}

void monotonic_arena::do_deallocate(void*, size_t, size_t) {
    // Monotonic arena doesn't support individual deallocation
}

bool monotonic_arena::do_is_equal(const memory_resource& other) const noexcept {
    return this == &other;
}

void monotonic_arena::allocate_new_block(size_t min_size) {
    auto data = std::make_unique<uint8_t[]>(min_size);
    blocks_.push_back(block{std::move(data), min_size, 0});
    total_capacity_ += min_size;
}

} // namespace katana
