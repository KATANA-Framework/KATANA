#pragma once

#include <atomic>
#include <cstdint>

namespace katana {

struct metrics_snapshot {
    uint64_t tasks_executed = 0;
    uint64_t tasks_scheduled = 0;
    uint64_t fd_events_processed = 0;
    uint64_t exceptions_caught = 0;
    uint64_t timers_fired = 0;
    uint64_t tasks_rejected = 0; // Tasks rejected due to backpressure
    uint64_t fd_timeouts = 0;

    metrics_snapshot& operator+=(const metrics_snapshot& other) {
        tasks_executed += other.tasks_executed;
        tasks_scheduled += other.tasks_scheduled;
        fd_events_processed += other.fd_events_processed;
        exceptions_caught += other.exceptions_caught;
        timers_fired += other.timers_fired;
        tasks_rejected += other.tasks_rejected;
        fd_timeouts += other.fd_timeouts;
        return *this;
    }
};

struct reactor_metrics {
    std::atomic<uint64_t> tasks_executed{0};
    std::atomic<uint64_t> tasks_scheduled{0};
    std::atomic<uint64_t> fd_events_processed{0};
    std::atomic<uint64_t> exceptions_caught{0};
    std::atomic<uint64_t> timers_fired{0};
    std::atomic<uint64_t> tasks_rejected{0}; // Tasks rejected due to backpressure
    std::atomic<uint64_t> fd_timeouts{0};

    void reset() {
        tasks_executed.store(0, std::memory_order_relaxed);
        tasks_scheduled.store(0, std::memory_order_relaxed);
        fd_events_processed.store(0, std::memory_order_relaxed);
        exceptions_caught.store(0, std::memory_order_relaxed);
        timers_fired.store(0, std::memory_order_relaxed);
        tasks_rejected.store(0, std::memory_order_relaxed);
        fd_timeouts.store(0, std::memory_order_relaxed);
    }

    [[nodiscard]] metrics_snapshot snapshot() const {
        return metrics_snapshot{tasks_executed.load(std::memory_order_relaxed),
                                tasks_scheduled.load(std::memory_order_relaxed),
                                fd_events_processed.load(std::memory_order_relaxed),
                                exceptions_caught.load(std::memory_order_relaxed),
                                timers_fired.load(std::memory_order_relaxed),
                                tasks_rejected.load(std::memory_order_relaxed),
                                fd_timeouts.load(std::memory_order_relaxed)};
    }
};

} // namespace katana
