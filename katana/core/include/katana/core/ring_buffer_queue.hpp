#pragma once

#include <atomic>
#include <optional>
#include <vector>
#include <new>

namespace katana {

template <typename T>
class ring_buffer_queue {
public:
    explicit ring_buffer_queue(size_t capacity = 1024) {
        size_t actual_capacity = next_power_of_two(capacity);
        mask_ = actual_capacity - 1;

        buffer_ = static_cast<slot*>(operator new(actual_capacity * sizeof(slot)));
        for (size_t i = 0; i < actual_capacity; ++i) {
            new (&buffer_[i]) slot();
            buffer_[i].sequence.store(i, std::memory_order_relaxed);
        }
        capacity_ = actual_capacity;
    }

    ~ring_buffer_queue() {
        T temp;
        while (try_pop(temp)) {}

        if (buffer_) {
            for (size_t i = 0; i < capacity_; ++i) {
                buffer_[i].~slot();
            }
            operator delete(buffer_);
        }
    }

    ring_buffer_queue(const ring_buffer_queue&) = delete;
    ring_buffer_queue& operator=(const ring_buffer_queue&) = delete;

    bool try_push(T&& value) {
        size_t head = head_.load(std::memory_order_relaxed);

        for (;;) {
            slot& s = buffer_[head & mask_];
            size_t seq = s.sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(head);

            if (diff == 0) {
                if (head_.compare_exchange_weak(head, head + 1, std::memory_order_relaxed)) {
                    new (&s.storage) T(std::move(value));
                    s.sequence.store(head + 1, std::memory_order_release);
                    return true;
                }
            } else if (diff < 0) {
                return false;
            } else {
                head = head_.load(std::memory_order_relaxed);
            }
        }
    }

    bool try_push(const T& value) {
        T copy = value;
        return try_push(std::move(copy));
    }

    bool try_pop(T& value) {
        size_t tail = tail_.load(std::memory_order_relaxed);

        for (;;) {
            slot& s = buffer_[tail & mask_];
            size_t seq = s.sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(tail + 1);

            if (diff == 0) {
                if (tail_.compare_exchange_weak(tail, tail + 1, std::memory_order_relaxed)) {
                    value = std::move(*reinterpret_cast<T*>(&s.storage));
                    reinterpret_cast<T*>(&s.storage)->~T();
                    s.sequence.store(tail + mask_ + 1, std::memory_order_release);
                    return true;
                }
            } else if (diff < 0) {
                return false;
            } else {
                tail = tail_.load(std::memory_order_relaxed);
            }
        }
    }

    std::optional<T> pop() {
        T value;
        if (try_pop(value)) {
            return std::optional<T>(std::move(value));
        }
        return std::nullopt;
    }

    void push(T value) {
        while (!try_push(std::move(value))) {
            // Busy wait
        }
    }

    [[nodiscard]] bool empty() const noexcept {
        size_t tail = tail_.load(std::memory_order_relaxed);
        size_t head = head_.load(std::memory_order_relaxed);
        return tail == head;
    }

    [[nodiscard]] size_t size() const noexcept {
        size_t head = head_.load(std::memory_order_relaxed);
        size_t tail = tail_.load(std::memory_order_relaxed);
        return head - tail;
    }

    [[nodiscard]] size_t capacity() const noexcept {
        return capacity_;
    }

private:
    struct slot {
        alignas(64) std::atomic<size_t> sequence;
        alignas(alignof(T)) std::byte storage[sizeof(T)];
    };

    static size_t next_power_of_two(size_t n) {
        if (n == 0) return 1;
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

    alignas(64) std::atomic<size_t> head_{0};
    alignas(64) std::atomic<size_t> tail_{0};
    slot* buffer_ = nullptr;
    size_t mask_;
    size_t capacity_;
};

} // namespace katana
