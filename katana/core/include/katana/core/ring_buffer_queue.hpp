#pragma once

#include <atomic>
#include <chrono>
#include <new>
#include <optional>
#include <thread>
#include <vector>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
    #include <immintrin.h>
#endif

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
        size_t head = head_.value.load(std::memory_order_relaxed);
        size_t spins = 0;

        for (;;) {
            slot& s = buffer_[head & mask_];
            size_t seq = s.sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(head);

            if (diff == 0) { // slot is free
                if (head_.value.compare_exchange_weak(head, head + 1, std::memory_order_relaxed)) {
                    new (&s.storage) T(std::move(value));
                    s.sequence.store(head + 1, std::memory_order_release);
                    if (head_.waiters.load(std::memory_order_acquire) > 0) {
                        head_.value.notify_one();
                    }
                    return true;
                }
            } else if (diff < 0) { // queue is full
                return false;
            } else {
                head = head_.value.load(std::memory_order_relaxed);
                spins = 0; // progress happened elsewhere; restart backoff
            }

            adaptive_pause(spins++);
        }
    }

    bool try_push(const T& value) {
        T copy = value;
        return try_push(std::move(copy));
    }

    bool try_pop(T& value) {
        size_t tail = tail_.value.load(std::memory_order_relaxed);
        size_t spins = 0;

        for (;;) {
            slot& s = buffer_[tail & mask_];
            size_t seq = s.sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(tail + 1);

            if (diff == 0) { // slot is full
                if (tail_.value.compare_exchange_weak(tail, tail + 1, std::memory_order_relaxed)) {
                    value = std::move(*reinterpret_cast<T*>(&s.storage));
                    reinterpret_cast<T*>(&s.storage)->~T();
                    s.sequence.store(tail + mask_ + 1, std::memory_order_release);
                    if (tail_.waiters.load(std::memory_order_acquire) > 0) {
                        tail_.value.notify_one();
                    }
                    return true;
                }
            } else if (diff < 0) { // queue is empty
                return false;
            } else {
                tail = tail_.value.load(std::memory_order_relaxed);
                spins = 0; // progress happened elsewhere; restart backoff
            }

            adaptive_pause(spins++);
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

    // Blocking variants based on C++20 atomic_wait/notify (futex on Linux).
    void push_wait(T value) {
        for (;;) {
            for (size_t spins = 0; spins < 64; ++spins) {
                if (try_push(std::move(value))) {
                    return;
                }
                adaptive_pause(spins);
            }

            size_t observed_tail = tail_.value.load(std::memory_order_acquire);
            tail_.waiters.fetch_add(1, std::memory_order_acquire);
            tail_.value.wait(observed_tail, std::memory_order_relaxed);
            tail_.waiters.fetch_sub(1, std::memory_order_release);
        }
    }

    bool pop_wait(T& value) {
        for (;;) {
            for (size_t spins = 0; spins < 64; ++spins) {
                if (try_pop(value)) {
                    return true;
                }
                adaptive_pause(spins);
            }

            size_t observed_head = head_.value.load(std::memory_order_acquire);
            head_.waiters.fetch_add(1, std::memory_order_acquire);
            head_.value.wait(observed_head, std::memory_order_relaxed);
            head_.waiters.fetch_sub(1, std::memory_order_release);
        }
    }

    std::optional<T> pop_wait() {
        T value;
        if (pop_wait(value)) {
            return std::optional<T>(std::move(value));
        }
        return std::nullopt;
    }

    [[nodiscard]] bool empty() const noexcept {
        size_t tail = tail_.value.load(std::memory_order_relaxed);
        size_t head = head_.value.load(std::memory_order_relaxed);
        return tail == head;
    }

    [[nodiscard]] size_t size() const noexcept {
        size_t head = head_.value.load(std::memory_order_relaxed);
        size_t tail = tail_.value.load(std::memory_order_relaxed);
        return head - tail;
    }

    [[nodiscard]] size_t capacity() const noexcept {
        return capacity_;
    }

private:
    struct slot {
        slot() : sequence(0) {}

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

    static constexpr size_t cache_line_size = 128;

    struct alignas(cache_line_size) padded_atomic {
        std::atomic<size_t> value{0};
        std::atomic<size_t> waiters{0};
        static constexpr size_t padding_size =
            cache_line_size - sizeof(std::atomic<size_t>) - sizeof(std::atomic<size_t>);
        static_assert(padding_size < cache_line_size, "padding calculation underflow");
        char padding[padding_size > 0 ? padding_size : 1]{};
    };
    static_assert(cache_line_size >= sizeof(std::atomic<size_t>) * 2, "cache line size too small for padding");

    static void cpu_relax() noexcept {
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
        _mm_pause();
#else
        std::this_thread::yield();
#endif
    }

    static void adaptive_pause(size_t spins) noexcept {
        if (spins < 4) {
            cpu_relax();
        } else if (spins < 16) {
            cpu_relax();
            cpu_relax();
        } else if (spins < 32) {
            for (int i = 0; i < 4; ++i) cpu_relax();
        } else if (spins < 64) {
            for (int i = 0; i < 8; ++i) cpu_relax();
        } else if (spins < 256) {
            std::this_thread::yield();
        }
    }

    padded_atomic head_{};
    padded_atomic tail_{};
    slot* buffer_ = nullptr;
    size_t mask_;
    size_t capacity_;
};

} // namespace katana
