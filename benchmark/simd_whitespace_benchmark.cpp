// SIMD Whitespace Benchmark
// Tests SIMD skip_ws optimization with formatted JSON (lots of whitespace)

#include "katana/core/arena.hpp"
#include "katana/core/serde.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace katana;

struct latency_stats {
    void add(int64_t ns) {
        samples.push_back(ns);
        sum_ns += ns;
    }

    void sort() { std::sort(samples.begin(), samples.end()); }

    [[nodiscard]] double percentile(double p) const {
        if (samples.empty())
            return 0.0;
        if (samples.size() == 1)
            return static_cast<double>(samples.front());

        double rank = (p / 100.0) * static_cast<double>(samples.size() - 1);
        size_t lower = static_cast<size_t>(std::floor(rank));
        size_t upper = static_cast<size_t>(std::ceil(rank));
        double weight = rank - static_cast<double>(lower);
        double interpolated =
            static_cast<double>(samples[lower]) +
            (static_cast<double>(samples[upper]) - static_cast<double>(samples[lower])) * weight;
        return interpolated;
    }

    [[nodiscard]] double avg() const {
        if (samples.empty())
            return 0.0;
        return static_cast<double>(sum_ns) / static_cast<double>(samples.size());
    }

    std::vector<int64_t> samples;
    int64_t sum_ns = 0;
};

void bench_whitespace_skipping(size_t iterations) {
    latency_stats stats_compact;
    latency_stats stats_formatted;
    latency_stats stats_heavily_formatted;

    // Compact JSON - minimal whitespace
    std::string compact = "[1.0,2.0,3.0,4.0,5.0]";

    // Formatted JSON - moderate whitespace
    std::string formatted = R"([
  1.0,
  2.0,
  3.0,
  4.0,
  5.0
])";

    // Heavily formatted JSON - lots of whitespace
    std::string heavily_formatted = R"([
    1.0,
    2.0,
    3.0,
    4.0,
    5.0,
    6.0,
    7.0,
    8.0,
    9.0,
    10.0,
    11.0,
    12.0,
    13.0,
    14.0,
    15.0,
    16.0,
    17.0,
    18.0,
    19.0,
    20.0
])";

    // Warmup
    for (size_t i = 0; i < 10000; ++i) {
        serde::json_cursor cur{compact.data(), compact.data() + compact.size()};
        while (!cur.eof()) {
            cur.skip_ws();
            if (cur.try_array_start() || cur.try_array_end() || cur.try_comma())
                continue;
            [[maybe_unused]] auto v = serde::parse_double(cur);
        }
    }

    // Benchmark compact JSON
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        serde::json_cursor cur{compact.data(), compact.data() + compact.size()};
        int count = 0;
        while (!cur.eof()) {
            cur.skip_ws();
            if (cur.try_array_start() || cur.try_array_end() || cur.try_comma())
                continue;
            if (auto v = serde::parse_double(cur))
                count++;
        }

        auto end = std::chrono::steady_clock::now();
        stats_compact.add(
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    // Benchmark formatted JSON
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        serde::json_cursor cur{formatted.data(), formatted.data() + formatted.size()};
        int count = 0;
        while (!cur.eof()) {
            cur.skip_ws();
            if (cur.try_array_start() || cur.try_array_end() || cur.try_comma())
                continue;
            if (auto v = serde::parse_double(cur))
                count++;
        }

        auto end = std::chrono::steady_clock::now();
        stats_formatted.add(
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    // Benchmark heavily formatted JSON
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        serde::json_cursor cur{heavily_formatted.data(),
                               heavily_formatted.data() + heavily_formatted.size()};
        int count = 0;
        while (!cur.eof()) {
            cur.skip_ws();
            if (cur.try_array_start() || cur.try_array_end() || cur.try_comma())
                continue;
            if (auto v = serde::parse_double(cur))
                count++;
        }

        auto end = std::chrono::steady_clock::now();
        stats_heavily_formatted.add(
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    stats_compact.sort();
    stats_formatted.sort();
    stats_heavily_formatted.sort();

    std::cout << "\n=== Whitespace Skipping Performance ===\n";

    auto print_stats =
        [](const char* label, const latency_stats& stats, size_t chars, size_t numbers) {
            std::cout << "  " << label << ":\n";
            std::cout << "    total chars: " << chars << "\n";
            std::cout << "    total avg:   " << std::fixed << std::setprecision(1) << stats.avg()
                      << " ns\n";
            std::cout << "    per number:  " << std::fixed << std::setprecision(1)
                      << stats.avg() / static_cast<double>(numbers) << " ns\n";
            std::cout << "    p50:         " << std::fixed << std::setprecision(1)
                      << stats.percentile(50.0) << " ns\n";
            std::cout << "    p99:         " << std::fixed << std::setprecision(1)
                      << stats.percentile(99.0) << " ns\n";
        };

    print_stats("Compact JSON (minimal whitespace)", stats_compact, compact.size(), 5);
    print_stats("Formatted JSON (moderate whitespace)", stats_formatted, formatted.size(), 5);
    print_stats("Heavily Formatted JSON (lots of whitespace)",
                stats_heavily_formatted,
                heavily_formatted.size(),
                20);
}

int main() {
    std::cout << "KATANA SIMD Whitespace Benchmark\n";
    std::cout << "=================================\n";
    std::cout << "Testing SIMD skip_ws with formatted JSON\n";

    constexpr size_t iterations = 200000;

    bench_whitespace_skipping(iterations);

    std::cout << "\n✓ Benchmark completed\n";
    std::cout << "\nNote: SIMD activates for 8+ consecutive whitespace characters.\n";
    std::cout << "Compact JSON uses fast scalar path, formatted JSON uses SIMD.\n";

    return 0;
}
