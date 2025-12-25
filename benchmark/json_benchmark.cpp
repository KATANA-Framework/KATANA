// JSON Encoder/Decoder Microbenchmark
// Tests katana::serde performance with various payload sizes

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
            return static_cast<double>(samples.front()) / 1e6;

        double rank = (p / 100.0) * static_cast<double>(samples.size() - 1);
        size_t lower = static_cast<size_t>(std::floor(rank));
        size_t upper = static_cast<size_t>(std::ceil(rank));
        double weight = rank - static_cast<double>(lower);
        double interpolated =
            static_cast<double>(samples[lower]) +
            (static_cast<double>(samples[upper]) - static_cast<double>(samples[lower])) * weight;
        return interpolated / 1e6;
    }

    [[nodiscard]] double avg() const {
        if (samples.empty())
            return 0.0;
        return static_cast<double>(sum_ns) / static_cast<double>(samples.size()) / 1e6;
    }

    std::vector<int64_t> samples;
    int64_t sum_ns = 0;
};

// Test: JSON string encoding
void bench_json_string_encode(size_t iterations) {
    latency_stats stats_small;
    latency_stats stats_medium;
    latency_stats stats_large;

    std::string small = "Hello";
    std::string medium =
        "The quick brown fox jumps over the lazy dog multiple times to make this longer";
    std::string large(1000, 'x');

    // Warmup
    for (size_t i = 0; i < 10000; ++i) {
        [[maybe_unused]] auto encoded = serde::escape_json_string(small);
    }

    // Benchmark small strings
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();
        [[maybe_unused]] auto encoded = serde::escape_json_string(small);
        auto end = std::chrono::steady_clock::now();
        stats_small.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    // Benchmark medium strings
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();
        [[maybe_unused]] auto encoded = serde::escape_json_string(medium);
        auto end = std::chrono::steady_clock::now();
        stats_medium.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    // Benchmark large strings
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();
        [[maybe_unused]] auto encoded = serde::escape_json_string(large);
        auto end = std::chrono::steady_clock::now();
        stats_large.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    stats_small.sort();
    stats_medium.sort();
    stats_large.sort();

    std::cout << "\n=== JSON String Encoding ===\n";

    auto print_stats = [](const char* label, const latency_stats& stats, size_t ops) {
        double duration_ms = static_cast<double>(stats.sum_ns) / 1e6;
        double ops_per_sec = static_cast<double>(ops) / (duration_ms / 1000.0);
        std::cout << "  " << label << ":\n";
        std::cout << "    Throughput: " << std::fixed << std::setprecision(2) << ops_per_sec / 1e6
                  << " M ops/s\n";
        std::cout << "    p50:        " << std::fixed << std::setprecision(3)
                  << stats.percentile(50.0) << " ms\n";
        std::cout << "    p99:        " << std::fixed << std::setprecision(3)
                  << stats.percentile(99.0) << " ms\n";
        std::cout << "    p999:       " << std::fixed << std::setprecision(3)
                  << stats.percentile(99.9) << " ms\n";
    };

    print_stats("Small (5 bytes)", stats_small, iterations);
    print_stats("Medium (80 bytes)", stats_medium, iterations);
    print_stats("Large (1000 bytes)", stats_large, iterations);
}

// Test: JSON object serialization
void bench_json_object(size_t iterations) {
    latency_stats stats;

    // Simulated object serialization
    struct test_object {
        int64_t id;
        std::string name;
        double value;
        bool active;
    };

    test_object obj{42, "test_object", 3.14159, true};

    // Warmup
    for (size_t i = 0; i < 10000; ++i) {
        std::string json = "{\"id\":";
        json += std::to_string(obj.id);
        json += ",\"name\":\"" + obj.name + "\"";
        json += ",\"value\":" + std::to_string(obj.value);
        json += ",\"active\":" + std::string(obj.active ? "true" : "false");
        json += "}";
    }

    // Benchmark
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        std::string json = "{\"id\":";
        json += std::to_string(obj.id);
        json += ",\"name\":\"" + obj.name + "\"";
        json += ",\"value\":" + std::to_string(obj.value);
        json += ",\"active\":" + std::string(obj.active ? "true" : "false");
        json += "}";

        auto end = std::chrono::steady_clock::now();
        stats.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    stats.sort();
    double duration_ms = static_cast<double>(stats.sum_ns) / 1e6;
    double ops_per_sec = static_cast<double>(iterations) / (duration_ms / 1000.0);

    std::cout << "\n=== JSON Object Serialization ===\n";
    std::cout << "  Operations: " << iterations << "\n";
    std::cout << "  Throughput: " << std::fixed << std::setprecision(2) << ops_per_sec / 1e6
              << " M ops/s\n";
    std::cout << "  p50:        " << std::fixed << std::setprecision(3) << stats.percentile(50.0)
              << " ms\n";
    std::cout << "  p99:        " << std::fixed << std::setprecision(3) << stats.percentile(99.0)
              << " ms\n";
    std::cout << "  p999:       " << std::fixed << std::setprecision(3) << stats.percentile(99.9)
              << " ms\n";
}

// Test: JSON array serialization
void bench_json_array(size_t iterations) {
    latency_stats stats_small;
    latency_stats stats_large;

    std::vector<int> small_array = {1, 2, 3, 4, 5};
    std::vector<int> large_array(100);
    std::iota(large_array.begin(), large_array.end(), 1);

    // Warmup
    for (size_t i = 0; i < 10000; ++i) {
        std::string json = "[";
        for (size_t j = 0; j < small_array.size(); ++j) {
            if (j > 0)
                json += ",";
            json += std::to_string(small_array[j]);
        }
        json += "]";
    }

    // Benchmark small arrays
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        std::string json = "[";
        for (size_t j = 0; j < small_array.size(); ++j) {
            if (j > 0)
                json += ",";
            json += std::to_string(small_array[j]);
        }
        json += "]";

        auto end = std::chrono::steady_clock::now();
        stats_small.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    // Benchmark large arrays
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        std::string json = "[";
        for (size_t j = 0; j < large_array.size(); ++j) {
            if (j > 0)
                json += ",";
            json += std::to_string(large_array[j]);
        }
        json += "]";

        auto end = std::chrono::steady_clock::now();
        stats_large.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    stats_small.sort();
    stats_large.sort();

    std::cout << "\n=== JSON Array Serialization ===\n";

    auto print_stats = [](const char* label, const latency_stats& stats, size_t ops) {
        double duration_ms = static_cast<double>(stats.sum_ns) / 1e6;
        double ops_per_sec = static_cast<double>(ops) / (duration_ms / 1000.0);
        std::cout << "  " << label << ":\n";
        std::cout << "    Throughput: " << std::fixed << std::setprecision(2) << ops_per_sec / 1e6
                  << " M ops/s\n";
        std::cout << "    p50:        " << std::fixed << std::setprecision(3)
                  << stats.percentile(50.0) << " ms\n";
        std::cout << "    p99:        " << std::fixed << std::setprecision(3)
                  << stats.percentile(99.0) << " ms\n";
    };

    print_stats("Small array (5 elements)", stats_small, iterations);
    print_stats("Large array (100 elements)", stats_large, iterations);
}

// Test: Number to string conversion (common in JSON serialization)
void bench_number_conversion(size_t iterations) {
    latency_stats stats;

    int64_t test_number = 1234567890;

    // Warmup
    for (size_t i = 0; i < 10000; ++i) {
        [[maybe_unused]] auto str = std::to_string(test_number);
    }

    // Benchmark
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();
        [[maybe_unused]] auto str = std::to_string(test_number);
        auto end = std::chrono::steady_clock::now();
        stats.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    stats.sort();
    double duration_ms = static_cast<double>(stats.sum_ns) / 1e6;
    double ops_per_sec = static_cast<double>(iterations) / (duration_ms / 1000.0);

    std::cout << "\n=== Number to String Conversion ===\n";
    std::cout << "  Operations: " << iterations << "\n";
    std::cout << "  Throughput: " << std::fixed << std::setprecision(2) << ops_per_sec / 1e6
              << " M ops/s\n";
    std::cout << "  p50:        " << std::fixed << std::setprecision(3) << stats.percentile(50.0)
              << " ms\n";
    std::cout << "  p99:        " << std::fixed << std::setprecision(3) << stats.percentile(99.0)
              << " ms\n";
    std::cout << "  p999:       " << std::fixed << std::setprecision(3) << stats.percentile(99.9)
              << " ms\n";
}

int main() {
    std::cout << "KATANA JSON Encoder/Decoder Microbenchmark\n";
    std::cout << "==========================================\n";

    constexpr size_t iterations = 200000;

    bench_json_string_encode(iterations);
    bench_json_object(iterations);
    bench_json_array(iterations);
    bench_number_conversion(iterations);

    std::cout << "\n✓ All JSON benchmarks completed\n";
    return 0;
}
