#include "katana/core/arena.hpp"
#include "katana/core/http_headers.hpp"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std::chrono;
using namespace katana;

struct benchmark_result {
    std::string name;
    double throughput;
    double latency_p50;
    double latency_p99;
    double latency_p999;
    uint64_t operations;
    uint64_t duration_ms;
};

void print_result(const benchmark_result& result) {
    std::cout << "\n=== " << result.name << " ===\n";
    std::cout << "Operations: " << result.operations << "\n";
    std::cout << "Duration: " << result.duration_ms << " ms\n";
    std::cout << "Throughput: " << std::fixed << std::setprecision(2) << result.throughput
              << " ops/sec\n";
    if (result.latency_p50 > 0.0) {
        std::cout << "Latency p50: " << std::fixed << std::setprecision(3) << result.latency_p50
                  << " us\n";
        std::cout << "Latency p99: " << std::fixed << std::setprecision(3) << result.latency_p99
                  << " us\n";
        std::cout << "Latency p999: " << std::fixed << std::setprecision(3) << result.latency_p999
                  << " us\n";
    }
}

benchmark_result benchmark_headers_set() {
    const size_t num_operations = 100000;
    std::vector<double> latencies;
    latencies.reserve(num_operations);

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations; ++i) {
        auto op_start = steady_clock::now();

        monotonic_arena arena;
        http::headers_map headers(&arena);
        headers.set(http::field::host, "localhost");
        headers.set(http::field::user_agent, "benchmark/1.0");
        headers.set(http::field::accept, "application/json");
        headers.set(http::field::content_type, "text/html");
        headers.set(http::field::connection, "keep-alive");

        auto op_end = steady_clock::now();

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) / 1000.0;
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = "Headers Set (5 standard fields)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = latencies[num_operations / 2];
    result.latency_p99 = latencies[num_operations * 99 / 100];
    result.latency_p999 = latencies[num_operations * 999 / 1000];

    return result;
}

benchmark_result benchmark_headers_get() {
    const size_t num_operations = 200000;
    std::vector<double> latencies;
    latencies.reserve(num_operations);

    monotonic_arena arena;
    http::headers_map headers(&arena);
    headers.set(http::field::host, "localhost");
    headers.set(http::field::user_agent, "benchmark/1.0");
    headers.set(http::field::accept, "application/json");
    headers.set(http::field::content_type, "text/html");
    headers.set(http::field::connection, "keep-alive");

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations; ++i) {
        auto op_start = steady_clock::now();

        [[maybe_unused]] auto host = headers.get(http::field::host);
        [[maybe_unused]] auto agent = headers.get(http::field::user_agent);
        [[maybe_unused]] auto accept = headers.get(http::field::accept);

        auto op_end = steady_clock::now();

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) / 1000.0;
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = "Headers Get (3 lookups)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = latencies[num_operations / 2];
    result.latency_p99 = latencies[num_operations * 99 / 100];
    result.latency_p999 = latencies[num_operations * 999 / 1000];

    return result;
}

benchmark_result benchmark_headers_custom() {
    const size_t num_operations = 50000;
    std::vector<double> latencies;
    latencies.reserve(num_operations);

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations; ++i) {
        auto op_start = steady_clock::now();

        monotonic_arena arena;
        http::headers_map headers(&arena);
        headers.set_view("X-Custom-Header-1", "value1");
        headers.set_view("X-Custom-Header-2", "value2");
        headers.set_view("X-Custom-Header-3", "value3");
        headers.set_view("X-Request-ID", "12345");

        auto op_end = steady_clock::now();

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) / 1000.0;
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = "Headers Set (4 custom fields)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = latencies[num_operations / 2];
    result.latency_p99 = latencies[num_operations * 99 / 100];
    result.latency_p999 = latencies[num_operations * 999 / 1000];

    return result;
}

benchmark_result benchmark_headers_ci_equal() {
    const size_t num_operations = 500000;
    std::vector<double> latencies;
    latencies.reserve(num_operations);

    const std::string_view str1 = "Content-Type";
    const std::string_view str2 = "content-type";

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations; ++i) {
        auto op_start = steady_clock::now();

        [[maybe_unused]] bool result = http::ci_equal(str1, str2);

        auto op_end = steady_clock::now();

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) / 1000.0;
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = "Case-Insensitive Compare";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = latencies[num_operations / 2];
    result.latency_p99 = latencies[num_operations * 99 / 100];
    result.latency_p999 = latencies[num_operations * 999 / 1000];

    return result;
}

benchmark_result benchmark_headers_iteration() {
    const size_t num_operations = 100000;
    std::vector<double> latencies;
    latencies.reserve(num_operations);

    monotonic_arena arena;
    http::headers_map headers(&arena);
    headers.set(http::field::host, "localhost");
    headers.set(http::field::user_agent, "benchmark/1.0");
    headers.set(http::field::accept, "application/json");
    headers.set_view("X-Custom-1", "value1");
    headers.set_view("X-Custom-2", "value2");

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations; ++i) {
        auto op_start = steady_clock::now();

        size_t count = 0;
        for (auto [name, value] : headers) {
            count += name.size() + value.size();
        }

        auto op_end = steady_clock::now();

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) / 1000.0;
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = "Headers Iteration (5 fields)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = latencies[num_operations / 2];
    result.latency_p99 = latencies[num_operations * 99 / 100];
    result.latency_p999 = latencies[num_operations * 999 / 1000];

    return result;
}

int main() {
    std::cout << "========================================\n";
    std::cout << "   KATANA HTTP Headers Benchmarks\n";
    std::cout << "========================================\n";

    std::vector<benchmark_result> results;

    std::cout << "\n[1/5] Benchmarking headers set (standard)...\n";
    results.push_back(benchmark_headers_set());
    print_result(results.back());

    std::cout << "\n[2/5] Benchmarking headers get...\n";
    results.push_back(benchmark_headers_get());
    print_result(results.back());

    std::cout << "\n[3/5] Benchmarking headers set (custom)...\n";
    results.push_back(benchmark_headers_custom());
    print_result(results.back());

    std::cout << "\n[4/5] Benchmarking case-insensitive compare...\n";
    results.push_back(benchmark_headers_ci_equal());
    print_result(results.back());

    std::cout << "\n[5/5] Benchmarking headers iteration...\n";
    results.push_back(benchmark_headers_iteration());
    print_result(results.back());

    std::cout << "\n========================================\n";
    std::cout << "         Benchmark Summary\n";
    std::cout << "========================================\n";

    for (const auto& result : results) {
        std::cout << std::left << std::setw(35) << result.name << ": " << std::fixed
                  << std::setprecision(0) << result.throughput << " ops/sec\n";
    }

    std::cout << "\nAll benchmarks completed successfully!\n";

    return 0;
}
