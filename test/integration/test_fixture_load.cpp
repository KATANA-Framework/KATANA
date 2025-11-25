#include "support/virtual_event_loop.hpp"

#include <algorithm>
#include <chrono>
#include <gtest/gtest.h>
#include <vector>

using namespace std::chrono_literals;
using katana::test_support::VirtualEventLoop;

namespace {

std::chrono::milliseconds percentile_ms(const std::vector<std::chrono::milliseconds>& samples,
                                        double p) {
    if (samples.empty()) {
        return 0ms;
    }
    std::vector<std::chrono::milliseconds> sorted = samples;
    std::sort(sorted.begin(), sorted.end());
    const double idx = (p / 100.0) * (static_cast<double>(sorted.size() - 1));
    const auto lower = static_cast<size_t>(idx);
    const auto upper = std::min(sorted.size() - 1, lower + 1);
    const double frac = idx - static_cast<double>(lower);
    auto blended = sorted[lower].count() +
                   static_cast<long long>(
                       frac * static_cast<double>(sorted[upper].count() - sorted[lower].count()));
    return std::chrono::milliseconds(blended);
}

} // namespace

// Fixture-style deterministic "load" test: N tasks with known latencies, assert p99 budget.
TEST(FixtureLoad, DeterministicP99Budget) {
    VirtualEventLoop loop;
    std::vector<std::chrono::milliseconds> latencies = {1ms, 1ms, 2ms, 3ms, 4ms, 5ms, 6ms,
                                                        2ms, 3ms, 4ms, 1ms, 2ms, 2ms, 3ms,
                                                        4ms, 5ms, 6ms, 6ms, 2ms, 3ms};

    std::vector<std::chrono::milliseconds> durations;
    durations.reserve(latencies.size());

    for (auto latency : latencies) {
        auto start = loop.now();
        loop.post_after(latency, [&, start, latency] {
            auto elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(loop.now() - start);
            durations.push_back(elapsed);
            // Ensure deterministic completion time
            ASSERT_LE(elapsed, latency);
        });
    }

    loop.run_all();
    ASSERT_EQ(durations.size(), latencies.size());

    auto p95 = percentile_ms(durations, 95.0);
    auto p99 = percentile_ms(durations, 99.0);

    EXPECT_LE(p95.count(), 6);
    EXPECT_LE(p99.count(), 6);
}
