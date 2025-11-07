#pragma once

#include <cstdint>
#include <atomic>

namespace katana {

struct reactor_metrics {
    std::atomic<uint64_t> tasks_executed{0};
    std::atomic<uint64_t> tasks_scheduled{0};
    std::atomic<uint64_t> fd_events_processed{0};
    std::atomic<uint64_t> exceptions_caught{0};
    std::atomic<uint64_t> timers_fired{0};

    void reset() {
        tasks_executed.store(0, std::memory_order_relaxed);
        tasks_scheduled.store(0, std::memory_order_relaxed);
        fd_events_processed.store(0, std::memory_order_relaxed);
        exceptions_caught.store(0, std::memory_order_relaxed);
        timers_fired.store(0, std::memory_order_relaxed);
    }
};

} // namespace katana
