#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <span>
#include <vector>

namespace katana {

class circular_buffer {
public:
    explicit circular_buffer(size_t capacity = 4096) {
        size_t actual_capacity = next_power_of_two(capacity);
        buffer_.resize(actual_capacity);
        mask_ = actual_capacity - 1;
    }

    circular_buffer(circular_buffer&&) noexcept = default;
    circular_buffer& operator=(circular_buffer&&) noexcept = default;
    circular_buffer(const circular_buffer&) = delete;
    circular_buffer& operator=(const circular_buffer&) = delete;

    [[nodiscard]] size_t write(std::span<const uint8_t> data) {
        size_t available = capacity() - size();
        size_t to_write = std::min(data.size(), available);

        if (to_write == 0)
            return 0;

        size_t tail_pos = tail_ & mask_;
        size_t first_part = std::min(to_write, buffer_.size() - tail_pos);

        std::memcpy(&buffer_[tail_pos], data.data(), first_part);

        if (to_write > first_part) {
            std::memcpy(&buffer_[0], data.data() + first_part, to_write - first_part);
        }

        tail_ += to_write;
        return to_write;
    }

    [[nodiscard]] size_t read(std::span<uint8_t> data) {
        size_t available = size();
        size_t to_read = std::min(data.size(), available);

        if (to_read == 0)
            return 0;

        size_t head_pos = head_ & mask_;
        size_t first_part = std::min(to_read, buffer_.size() - head_pos);

        std::memcpy(data.data(), &buffer_[head_pos], first_part);

        if (to_read > first_part) {
            std::memcpy(data.data() + first_part, &buffer_[0], to_read - first_part);
        }

        head_ += to_read;
        return to_read;
    }

    [[nodiscard]] std::span<const uint8_t> peek() const noexcept {
        if (empty())
            return {};

        size_t available = size();
        size_t head_pos = head_ & mask_;
        size_t contiguous = std::min(available, buffer_.size() - head_pos);

        return std::span<const uint8_t>(&buffer_[head_pos], contiguous);
    }

    void consume(size_t bytes) noexcept { head_ += std::min(bytes, size()); }

    [[nodiscard]] size_t size() const noexcept { return tail_ - head_; }

    [[nodiscard]] size_t capacity() const noexcept { return buffer_.size(); }

    [[nodiscard]] bool empty() const noexcept { return head_ == tail_; }

    void clear() noexcept {
        head_ = 0;
        tail_ = 0;
    }

    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity())
            return;

        size_t actual_capacity = next_power_of_two(new_capacity);
        std::vector<uint8_t> new_buffer(actual_capacity);

        size_t current_size = size();
        if (current_size > 0) {
            size_t head_pos = head_ & mask_;
            size_t first_part = std::min(current_size, buffer_.size() - head_pos);

            std::memcpy(&new_buffer[0], &buffer_[head_pos], first_part);
            if (current_size > first_part) {
                std::memcpy(&new_buffer[first_part], &buffer_[0], current_size - first_part);
            }
        }

        buffer_ = std::move(new_buffer);
        mask_ = actual_capacity - 1;
        head_ = 0;
        tail_ = current_size;
    }

private:
    static size_t next_power_of_two(size_t n) {
        if (n == 0)
            return 1;
        --n;
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        if constexpr (sizeof(size_t) > 4) {
            n |= n >> 32;
        }
        return n + 1;
    }

    std::vector<uint8_t> buffer_;
    size_t head_ = 0;
    size_t tail_ = 0;
    size_t mask_ = 0;
};

} // namespace katana
