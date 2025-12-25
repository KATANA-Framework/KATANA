// JSON Parsing Microbenchmark
// Tests parse_double and skip_ws optimizations

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "katana/core/arena.hpp"
#include "katana/core/serde.hpp"

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

// Test: Parse single double
void bench_parse_double(size_t iterations) {
    latency_stats stats_simple;
    latency_stats stats_scientific;
    latency_stats stats_negative;

    std::string simple = "123.456";
    std::string scientific = "1.23456e10";
    std::string negative = "-987.654";

    monotonic_arena arena(64 * 1024);

    // Warmup
    for (size_t i = 0; i < 10000; ++i) {
        serde::json_cursor cur{simple.data(), simple.data() + simple.size()};
        [[maybe_unused]] auto val = serde::parse_double(cur);
    }

    // Benchmark simple numbers
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        serde::json_cursor cur{simple.data(), simple.data() + simple.size()};
        [[maybe_unused]] auto val = serde::parse_double(cur);

        auto end = std::chrono::steady_clock::now();
        stats_simple.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    // Benchmark scientific notation
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        serde::json_cursor cur{scientific.data(), scientific.data() + scientific.size()};
        [[maybe_unused]] auto val = serde::parse_double(cur);

        auto end = std::chrono::steady_clock::now();
        stats_scientific.add(
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    // Benchmark negative numbers
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        serde::json_cursor cur{negative.data(), negative.data() + negative.size()};
        [[maybe_unused]] auto val = serde::parse_double(cur);

        auto end = std::chrono::steady_clock::now();
        stats_negative.add(
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    stats_simple.sort();
    stats_scientific.sort();
    stats_negative.sort();

    std::cout << "\n=== Parse Double ===\n";

    auto print_stats = [](const char* label, const latency_stats& stats) {
        std::cout << "  " << label << ":\n";
        std::cout << "    avg: " << std::fixed << std::setprecision(1) << stats.avg() << " ns\n";
        std::cout << "    p50: " << std::fixed << std::setprecision(1) << stats.percentile(50.0)
                  << " ns\n";
        std::cout << "    p99: " << std::fixed << std::setprecision(1) << stats.percentile(99.0)
                  << " ns\n";
    };

    print_stats("Simple (123.456)", stats_simple);
    print_stats("Scientific (1.23e10)", stats_scientific);
    print_stats("Negative (-987.654)", stats_negative);
}

// Test: Parse array of doubles
void bench_parse_double_array(size_t iterations) {
    latency_stats stats_small;
    latency_stats stats_large;

    std::string small_array = "[1.0, 2.0, 3.0, 4.0, 5.0]";

    // Large array with 100 doubles
    std::string large_array = "[";
    for (int i = 0; i < 100; ++i) {
        if (i > 0)
            large_array += ", ";
        large_array += std::to_string(i * 1.5);
    }
    large_array += "]";

    monotonic_arena arena(64 * 1024);

    // Warmup
    for (size_t i = 0; i < 10000; ++i) {
        serde::json_cursor cur{small_array.data(), small_array.data() + small_array.size()};
        if (cur.try_array_start()) {
            while (!cur.eof()) {
                cur.skip_ws();
                if (cur.try_array_end())
                    break;
                [[maybe_unused]] auto val = serde::parse_double(cur);
                cur.try_comma();
            }
        }
    }

    // Benchmark small array
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        serde::json_cursor cur{small_array.data(), small_array.data() + small_array.size()};
        int count = 0;
        if (cur.try_array_start()) {
            while (!cur.eof()) {
                cur.skip_ws();
                if (cur.try_array_end())
                    break;
                [[maybe_unused]] auto val = serde::parse_double(cur);
                count++;
                cur.try_comma();
            }
        }

        auto end = std::chrono::steady_clock::now();
        stats_small.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    // Benchmark large array
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        serde::json_cursor cur{large_array.data(), large_array.data() + large_array.size()};
        int count = 0;
        if (cur.try_array_start()) {
            while (!cur.eof()) {
                cur.skip_ws();
                if (cur.try_array_end())
                    break;
                [[maybe_unused]] auto val = serde::parse_double(cur);
                count++;
                cur.try_comma();
            }
        }

        auto end = std::chrono::steady_clock::now();
        stats_large.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    stats_small.sort();
    stats_large.sort();

    std::cout << "\n=== Parse Double Array ===\n";

    auto print_stats = [](const char* label, const latency_stats& stats, size_t elements) {
        std::cout << "  " << label << ":\n";
        std::cout << "    total avg: " << std::fixed << std::setprecision(1) << stats.avg()
                  << " ns\n";
        std::cout << "    per element: " << std::fixed << std::setprecision(1)
                  << stats.avg() / static_cast<double>(elements) << " ns\n";
        std::cout << "    p50: " << std::fixed << std::setprecision(1) << stats.percentile(50.0)
                  << " ns\n";
        std::cout << "    p99: " << std::fixed << std::setprecision(1) << stats.percentile(99.0)
                  << " ns\n";
    };

    print_stats("Small (5 elements)", stats_small, 5);
    print_stats("Large (100 elements)", stats_large, 100);
}

// Test: Whitespace skipping
void bench_skip_ws(size_t iterations) {
    latency_stats stats_none;
    latency_stats stats_some;
    latency_stats stats_lots;

    std::string none = "123";
    std::string some = "   \t  123";
    std::string lots = "                    123";

    // Warmup
    for (size_t i = 0; i < 10000; ++i) {
        serde::json_cursor cur{some.data(), some.data() + some.size()};
        cur.skip_ws();
    }

    // Benchmark no whitespace
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        serde::json_cursor cur{none.data(), none.data() + none.size()};
        cur.skip_ws();

        auto end = std::chrono::steady_clock::now();
        stats_none.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    // Benchmark some whitespace
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        serde::json_cursor cur{some.data(), some.data() + some.size()};
        cur.skip_ws();

        auto end = std::chrono::steady_clock::now();
        stats_some.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    // Benchmark lots of whitespace
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        serde::json_cursor cur{lots.data(), lots.data() + lots.size()};
        cur.skip_ws();

        auto end = std::chrono::steady_clock::now();
        stats_lots.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    stats_none.sort();
    stats_some.sort();
    stats_lots.sort();

    std::cout << "\n=== Skip Whitespace ===\n";

    auto print_stats = [](const char* label, const latency_stats& stats) {
        std::cout << "  " << label << ":\n";
        std::cout << "    avg: " << std::fixed << std::setprecision(1) << stats.avg() << " ns\n";
        std::cout << "    p50: " << std::fixed << std::setprecision(1) << stats.percentile(50.0)
                  << " ns\n";
        std::cout << "    p99: " << std::fixed << std::setprecision(1) << stats.percentile(99.0)
                  << " ns\n";
    };

    print_stats("No whitespace", stats_none);
    print_stats("Some whitespace (6 chars)", stats_some);
    print_stats("Lots of whitespace (20 chars)", stats_lots);
}

int main() {
    std::cout << "KATANA JSON Parsing Microbenchmark\n";
    std::cout << "===================================\n";
    std::cout << "Testing parse_double and skip_ws optimizations\n";

    constexpr size_t iterations = 200000;

    bench_parse_double(iterations);
    bench_parse_double_array(iterations);
    bench_skip_ws(iterations);

    std::cout << "\n✓ All parsing benchmarks completed\n";
    return 0;
}
