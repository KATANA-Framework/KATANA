#pragma once

#include <algorithm>
#include <atomic>
#include <chrono>
#include <functional>
#include <iterator>
#include <new>
#include <optional>
#include <thread>
#include <vector>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
#include <immintrin.h>
#endif

namespace katana {

template <typename T> class ring_buffer_queue {
public:
    explicit ring_buffer_queue(size_t capacity = 1024, bool enable_spsc_fast_path = true) {
        size_t actual_capacity = next_power_of_two(capacity);
        mask_ = actual_capacity - 1;

        buffer_ = static_cast<slot*>(
            ::operator new(actual_capacity * sizeof(slot), std::align_val_t(alignof(slot))));
        for (size_t i = 0; i < actual_capacity; ++i) {
            new (&buffer_[i]) slot();
            buffer_[i].sequence.store(i, std::memory_order_relaxed);
        }
        capacity_ = actual_capacity;

        // Some workloads (e.g., multi-producer benchmarks) hit edge cases when the queue
        // opportunistically switches between SPSC and MPMC. Allow callers to force the
        // safer MPMC path from the start.
        if (!enable_spsc_fast_path) {
            multi_producer_seen_.store(true, std::memory_order_relaxed);
            multi_consumer_seen_.store(true, std::memory_order_relaxed);
        }
    }

    ~ring_buffer_queue() {
        T temp;
        while (try_pop(temp)) {
        }

        if (buffer_) {
            for (size_t i = 0; i < capacity_; ++i) {
                buffer_[i].~slot();
            }
            ::operator delete(buffer_, std::align_val_t(alignof(slot)));
        }
    }

    ring_buffer_queue(const ring_buffer_queue&) = delete;
    ring_buffer_queue& operator=(const ring_buffer_queue&) = delete;

    bool try_push(T&& value) {
        if (multi_producer_seen_.load(std::memory_order_relaxed) ||
            multi_consumer_seen_.load(std::memory_order_relaxed)) {
            return try_push_mpmc(std::move(value));
        }

        const uint64_t current_producer = current_thread_id();
        mark_producer(current_producer);

        if (spsc_push_available(current_producer)) {
            return try_push_spsc(std::move(value));
        }

        return try_push_mpmc(std::move(value));
    }

    bool try_push(const T& value) {
        T copy = value;
        return try_push(std::move(copy));
    }

    bool try_pop(T& value) {
        if (multi_producer_seen_.load(std::memory_order_relaxed) ||
            multi_consumer_seen_.load(std::memory_order_relaxed)) {
            return try_pop_mpmc(value);
        }

        const uint64_t current_consumer = current_thread_id();
        mark_consumer(current_consumer);

        if (spsc_pop_available(current_consumer)) {
            return try_pop_spsc(value);
        }

        return try_pop_mpmc(value);
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
            tail_.waiters.fetch_add(1, std::memory_order_relaxed);
            tail_.value.wait(observed_tail, std::memory_order_relaxed);
            tail_.waiters.fetch_sub(1, std::memory_order_relaxed);
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
            head_.waiters.fetch_add(1, std::memory_order_relaxed);
            head_.value.wait(observed_head, std::memory_order_relaxed);
            head_.waiters.fetch_sub(1, std::memory_order_relaxed);
        }
    }

    std::optional<T> pop_wait() {
        T value;
        if (pop_wait(value)) {
            return std::optional<T>(std::move(value));
        }
        return std::nullopt;
    }

    template <typename Iterator> size_t push_batch(Iterator begin, Iterator end) noexcept {
        size_t count = static_cast<size_t>(std::distance(begin, end));
        if (count == 0) {
            return 0;
        }

        size_t head = head_.value.load(std::memory_order_relaxed);

        for (;;) {
            size_t tail = tail_.value.load(std::memory_order_acquire);
            size_t available = capacity_ - (head - tail);
            size_t to_push = std::min(count, available);

            if (to_push == 0) {
                return 0;
            }

            if (head_.value.compare_exchange_weak(
                    head, head + to_push, std::memory_order_acq_rel)) {
                for (size_t i = 0; i < to_push; ++i, ++begin) {
                    slot& s = buffer_[(head + i) & mask_];
                    new (&s.storage) T(std::move(*begin));
                    s.sequence.store(head + i + 1, std::memory_order_release);
                }

                maybe_notify(head_, head_notify_pending_);
                return to_push;
            }
        }
    }

    template <typename OutputIt> size_t pop_batch(OutputIt out, size_t max_count) noexcept {
        size_t tail = tail_.value.load(std::memory_order_relaxed);

        for (;;) {
            size_t head = head_.value.load(std::memory_order_acquire);
            size_t available = head - tail;
            size_t to_pop = std::min(max_count, available);

            if (to_pop == 0) {
                return 0;
            }

            bool all_ready = true;
            for (size_t i = 0; i < to_pop && all_ready; ++i) {
                slot& s = buffer_[(tail + i) & mask_];
                size_t seq = s.sequence.load(std::memory_order_acquire);
                if (seq != tail + i + 1) {
                    all_ready = false;
                    to_pop = i;
                }
            }

            if (to_pop == 0) {
                continue;
            }

            if (tail_.value.compare_exchange_weak(tail, tail + to_pop, std::memory_order_acq_rel)) {
                for (size_t i = 0; i < to_pop; ++i) {
                    slot& s = buffer_[(tail + i) & mask_];
                    *out++ = std::move(*reinterpret_cast<T*>(&s.storage));
                    reinterpret_cast<T*>(&s.storage)->~T();
                    s.sequence.store(tail + i + mask_ + 1, std::memory_order_release);
                }

                maybe_notify(tail_, tail_notify_pending_);
                return to_pop;
            }
        }
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

    [[nodiscard]] size_t capacity() const noexcept { return capacity_; }

private:
    struct slot {
        slot() : sequence(0) {}

        alignas(64) std::atomic<size_t> sequence;
        alignas(alignof(T)) std::byte storage[sizeof(T)];
    };

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

    // Используем 128B паддинг, чтобы снизить ложное разделение линий в сценариАх
    // с высоким контеншеном (8×8 бенч); для hot-path это даёт более ровный
    // throughput на p99.
    static constexpr size_t cache_line_size = 128;

    struct alignas(cache_line_size) padded_atomic {
        std::atomic<size_t> value{0};
        std::atomic<size_t> waiters{0};
        static constexpr size_t padding_size =
            cache_line_size - sizeof(std::atomic<size_t>) - sizeof(std::atomic<size_t>);
        char padding[padding_size > 0 ? padding_size : 1]{};
    };

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
            for (int i = 0; i < 4; ++i)
                cpu_relax();
        } else if (spins < 64) {
            for (int i = 0; i < 8; ++i)
                cpu_relax();
        } else if (spins < 256) {
            std::this_thread::yield();
        }
    }

    padded_atomic head_{};
    padded_atomic tail_{};
    slot* buffer_ = nullptr;
    size_t mask_ = 0;
    size_t capacity_ = 0;

    alignas(cache_line_size) std::atomic<uint64_t> last_producer_{0};
    alignas(cache_line_size) std::atomic<uint64_t> last_consumer_{0};
    alignas(cache_line_size) std::atomic<bool> multi_producer_seen_{false};
    alignas(cache_line_size) std::atomic<bool> multi_consumer_seen_{false};
    alignas(cache_line_size) std::atomic<bool> head_notify_pending_{false};
    alignas(cache_line_size) std::atomic<bool> tail_notify_pending_{false};

    bool try_push_mpmc(T&& value) {
        size_t head = head_.value.load(std::memory_order_relaxed);
        size_t spins = 0;

        for (;;) {
            slot& s = buffer_[head & mask_];
            size_t seq = s.sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(head);

            if (diff == 0) {
                if (head_.value.compare_exchange_weak(head, head + 1, std::memory_order_acq_rel)) {
                    new (&s.storage) T(std::move(value));
                    s.sequence.store(head + 1, std::memory_order_release);
                    maybe_notify(head_, head_notify_pending_);
                    return true;
                }
            } else if (diff < 0) {
                return false;
            } else {
                head = head_.value.load(std::memory_order_relaxed);
                spins = 0;
            }

            adaptive_pause(spins++);
        }
    }

    bool try_pop_mpmc(T& value) {
        size_t tail = tail_.value.load(std::memory_order_relaxed);
        size_t spins = 0;

        for (;;) {
            slot& s = buffer_[tail & mask_];

#if defined(__GNUG__) || defined(__clang__)
            __builtin_prefetch(&buffer_[(tail + 1) & mask_], 0, 3);
#endif

            size_t seq = s.sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(tail + 1);

            if (diff == 0) {
                if (tail_.value.compare_exchange_weak(tail, tail + 1, std::memory_order_acq_rel)) {
                    value = std::move(*reinterpret_cast<T*>(&s.storage));
                    reinterpret_cast<T*>(&s.storage)->~T();
                    s.sequence.store(tail + mask_ + 1, std::memory_order_release);
                    maybe_notify(tail_, tail_notify_pending_);
                    return true;
                }
            } else if (diff < 0) {
                return false;
            } else {
                tail = tail_.value.load(std::memory_order_relaxed);
                spins = 0;
            }

            adaptive_pause(spins++);
        }
    }

    bool try_push_spsc(T&& value) noexcept {
        size_t head = head_.value.load(std::memory_order_relaxed);
        slot& s = buffer_[head & mask_];

        size_t seq = s.sequence.load(std::memory_order_acquire);
        if (seq != head) {
            return false;
        }

        new (&s.storage) T(std::move(value));
        s.sequence.store(head + 1, std::memory_order_release);
        head_.value.store(head + 1, std::memory_order_release);
        maybe_notify(head_, head_notify_pending_);
        return true;
    }

    bool try_pop_spsc(T& value) noexcept {
        size_t tail = tail_.value.load(std::memory_order_relaxed);
        slot& s = buffer_[tail & mask_];

        size_t seq = s.sequence.load(std::memory_order_acquire);
        if (seq != tail + 1) {
            return false;
        }

        value = std::move(*reinterpret_cast<T*>(&s.storage));
        reinterpret_cast<T*>(&s.storage)->~T();
        s.sequence.store(tail + mask_ + 1, std::memory_order_release);
        tail_.value.store(tail + 1, std::memory_order_release);
        maybe_notify(tail_, tail_notify_pending_);
        return true;
    }

    void mark_producer(uint64_t current) noexcept {
        if (multi_producer_seen_.load(std::memory_order_relaxed)) {
            return;
        }

        auto last = last_producer_.load(std::memory_order_relaxed);
        if (last == current) {
            return;
        }

        if (last == 0) {
            last_producer_.compare_exchange_strong(last, current, std::memory_order_relaxed);
            return;
        }

        multi_producer_seen_.store(true, std::memory_order_relaxed);
    }

    void mark_consumer(uint64_t current) noexcept {
        if (multi_consumer_seen_.load(std::memory_order_relaxed)) {
            return;
        }

        auto last = last_consumer_.load(std::memory_order_relaxed);
        if (last == current) {
            return;
        }

        if (last == 0) {
            last_consumer_.compare_exchange_strong(last, current, std::memory_order_relaxed);
            return;
        }

        multi_consumer_seen_.store(true, std::memory_order_relaxed);
    }

    [[nodiscard]] bool spsc_push_available(uint64_t current) const noexcept {
        return !multi_producer_seen_.load(std::memory_order_relaxed) &&
               !multi_consumer_seen_.load(std::memory_order_relaxed) &&
               last_producer_.load(std::memory_order_relaxed) == current &&
               last_consumer_.load(std::memory_order_relaxed) != 0;
    }

    [[nodiscard]] bool spsc_pop_available(uint64_t current) const noexcept {
        return !multi_producer_seen_.load(std::memory_order_relaxed) &&
               !multi_consumer_seen_.load(std::memory_order_relaxed) &&
               last_consumer_.load(std::memory_order_relaxed) == current &&
               last_producer_.load(std::memory_order_relaxed) != 0;
    }

    void maybe_notify(padded_atomic& state, std::atomic<bool>& notify_pending) noexcept {
        if (state.waiters.load(std::memory_order_relaxed) == 0) {
            return;
        }

        bool expected = false;
        if (notify_pending.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
            state.value.notify_one();
            notify_pending.store(false, std::memory_order_release);
        }
    }

    [[nodiscard]] static uint64_t current_thread_id() noexcept {
        thread_local const uint64_t id = [] {
            static std::atomic<uint64_t> next_id{1};
            uint64_t assigned = next_id.fetch_add(1, std::memory_order_relaxed);
            if (assigned == 0) {
                assigned = next_id.fetch_add(1, std::memory_order_relaxed);
            }
            return assigned;
        }();
        return id;
    }
};

} // namespace katana
