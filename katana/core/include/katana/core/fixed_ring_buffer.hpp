#pragma once

#include <span>
#include <cstdint>
#include <cstring>
#include <array>

namespace katana {

template<size_t SIZE = 65536>
class fixed_ring_buffer {
public:
    fixed_ring_buffer() = default;

    [[nodiscard]] std::span<uint8_t> writable_span() noexcept {
        if (write_pos_ >= read_pos_) {
            return {data_.data() + write_pos_, SIZE - write_pos_};
        }
        return {data_.data() + write_pos_, read_pos_ - write_pos_};
    }

    void commit_write(size_t n) noexcept {
        write_pos_ = (write_pos_ + n) % SIZE;
        size_ = std::min(size_ + n, SIZE);
    }

    [[nodiscard]] std::span<const uint8_t> readable_span() const noexcept {
        if (read_pos_ <= write_pos_) {
            return {data_.data() + read_pos_, write_pos_ - read_pos_};
        }
        return {data_.data() + read_pos_, SIZE - read_pos_};
    }

    void consume(size_t n) noexcept {
        read_pos_ = (read_pos_ + n) % SIZE;
        size_ = (size_ >= n) ? (size_ - n) : 0;
    }

    [[nodiscard]] size_t size() const noexcept { return size_; }
    [[nodiscard]] size_t capacity() const noexcept { return SIZE; }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] bool full() const noexcept { return size_ == SIZE; }

    void clear() noexcept {
        read_pos_ = 0;
        write_pos_ = 0;
        size_ = 0;
    }

    void append(std::span<const uint8_t> data) {
        size_t remaining = data.size();
        size_t offset = 0;

        while (remaining > 0) {
            auto writable = writable_span();
            if (writable.empty()) {
                break;
            }

            size_t to_copy = std::min(remaining, writable.size());
            std::memcpy(writable.data(), data.data() + offset, to_copy);
            commit_write(to_copy);

            remaining -= to_copy;
            offset += to_copy;
        }
    }

private:
    alignas(64) std::array<uint8_t, SIZE> data_{};
    size_t read_pos_ = 0;
    size_t write_pos_ = 0;
    size_t size_ = 0;
};

} // namespace katana
