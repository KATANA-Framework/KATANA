#pragma once

#include <memory_resource>
#include <memory>
#include <vector>
#include <string_view>
#include <string>
#include <cstdint>
#include <cstddef>

namespace katana {

class monotonic_arena : public std::pmr::memory_resource {
public:
    explicit monotonic_arena(size_t block_size = 64 * 1024);
    ~monotonic_arena() override;

    monotonic_arena(const monotonic_arena&) = delete;
    monotonic_arena& operator=(const monotonic_arena&) = delete;

    void reset() noexcept;

    size_t bytes_allocated() const noexcept { return bytes_allocated_; }
    size_t total_capacity() const noexcept { return total_capacity_; }

protected:
    void* do_allocate(size_t bytes, size_t alignment) override;
    void do_deallocate(void* p, size_t bytes, size_t alignment) override;
    bool do_is_equal(const memory_resource& other) const noexcept override;

private:
    struct block {
        std::unique_ptr<uint8_t[]> data;
        size_t size;
        size_t used;
    };

    void allocate_new_block(size_t min_size);

    std::vector<block> blocks_;
    size_t block_size_;
    size_t bytes_allocated_ = 0;
    size_t total_capacity_ = 0;
};

template <typename T>
using pmr_vector = std::pmr::vector<T>;

template <typename T>
using pmr_string = std::pmr::basic_string<T>;

using pmr_string_view = std::string_view;

} // namespace katana
