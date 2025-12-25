// Benchmark for generated JSON parsing (Level 3 optimization)
// Tests the actual generated parse functions

#include "../examples/codegen/compute_api/generated/generated_dtos.hpp"
#include "../examples/codegen/compute_api/generated/generated_json.hpp"
#include "katana/core/arena.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
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

// Test: Parse compute_sum_body_0 (array of doubles)
void bench_parse_compute_sum_body(size_t iterations) {
    latency_stats stats_small;
    latency_stats stats_medium;
    latency_stats stats_large;

    std::string small_array = "[1.0, 2.0, 3.0, 4.0, 5.0]";
    std::string medium_array = "[1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, "
                               "11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0]";

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
        [[maybe_unused]] auto result = parse_compute_sum_body_0(small_array, &arena);
        arena.reset();
    }

    // Benchmark small array (5 elements)
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        [[maybe_unused]] auto result = parse_compute_sum_body_0(small_array, &arena);

        auto end = std::chrono::steady_clock::now();
        stats_small.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());

        arena.reset();
    }

    // Benchmark medium array (20 elements)
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        [[maybe_unused]] auto result = parse_compute_sum_body_0(medium_array, &arena);

        auto end = std::chrono::steady_clock::now();
        stats_medium.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());

        arena.reset();
    }

    // Benchmark large array (100 elements)
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        [[maybe_unused]] auto result = parse_compute_sum_body_0(large_array, &arena);

        auto end = std::chrono::steady_clock::now();
        stats_large.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());

        arena.reset();
    }

    stats_small.sort();
    stats_medium.sort();
    stats_large.sort();

    std::cout << "\n=== Parse compute_sum_body_0 (Generated Code) ===\n";

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
    print_stats("Medium (20 elements)", stats_medium, 20);
    print_stats("Large (100 elements)", stats_large, 100);
}

int main() {
    std::cout << "KATANA Generated JSON Parsing Benchmark\n";
    std::cout << "========================================\n";
    std::cout << "Testing Level 3 optimization (direct parsing, no intermediate string_view)\n";

    constexpr size_t iterations = 200000;

    bench_parse_compute_sum_body(iterations);

    std::cout << "\n✓ All benchmarks completed\n";
    std::cout << "\nOptimization: Arrays of primitives now parse directly without\n";
    std::cout << "creating intermediate string_view (eliminates double parsing).\n";

    return 0;
}
