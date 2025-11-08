#include "katana/core/ring_buffer_queue.hpp"
#include "katana/core/circular_buffer.hpp"
#include "katana/core/simd_utils.hpp"
#include "katana/core/http.hpp"
#include "katana/core/reactor_pool.hpp"
#include "katana/core/tcp_listener.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <iomanip>
#include <numeric>
#include <algorithm>

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
    std::cout << "Throughput: " << std::fixed << std::setprecision(2)
              << result.throughput << " ops/sec\n";
    std::cout << "Latency p50: " << std::fixed << std::setprecision(3)
              << result.latency_p50 << " us\n";
    std::cout << "Latency p99: " << std::fixed << std::setprecision(3)
              << result.latency_p99 << " us\n";
    std::cout << "Latency p999: " << std::fixed << std::setprecision(3)
              << result.latency_p999 << " us\n";
}

benchmark_result benchmark_ring_buffer_queue() {
    const size_t num_operations = 1000000;
    ring_buffer_queue<int> queue(1024);
    std::vector<double> latencies;
    latencies.reserve(num_operations);

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations; ++i) {
        auto op_start = steady_clock::now();
        queue.try_push(static_cast<int>(i));
        int val;
        queue.try_pop(val);
        auto op_end = steady_clock::now();

        double latency_us = static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) / 1000.0;
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = "Ring Buffer Queue (Single Thread)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = latencies[num_operations / 2];
    result.latency_p99 = latencies[num_operations * 99 / 100];
    result.latency_p999 = latencies[num_operations * 999 / 1000];

    return result;
}

benchmark_result benchmark_ring_buffer_concurrent() {
    const size_t num_operations = 1000000;
    const int num_threads = 4;
    ring_buffer_queue<int> queue(4096);
    std::atomic<size_t> total_ops{0};

    auto start = steady_clock::now();

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int t = 0; t < num_threads; ++t) {
        producers.emplace_back([&] {
            for (size_t i = 0; i < num_operations / num_threads; ++i) {
                while (!queue.try_push(static_cast<int>(i))) {
                    std::this_thread::yield();
                }
                total_ops.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (int t = 0; t < num_threads; ++t) {
        consumers.emplace_back([&] {
            size_t consumed = 0;
            while (consumed < num_operations / num_threads) {
                int val;
                if (queue.try_pop(val)) {
                    ++consumed;
                } else {
                    std::this_thread::yield();
                }
            }
        });
    }

    for (auto& t : producers) t.join();
    for (auto& t : consumers) t.join();

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    benchmark_result result;
    result.name = "Ring Buffer Queue (Concurrent 4x4)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = 0.0;
    result.latency_p99 = 0.0;
    result.latency_p999 = 0.0;

    return result;
}

benchmark_result benchmark_circular_buffer() {
    const size_t num_operations = 500000;
    circular_buffer buf(4096);
    std::vector<double> latencies;
    latencies.reserve(num_operations);

    std::vector<uint8_t> write_data(64, 'A');
    std::vector<uint8_t> read_data(64);

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations; ++i) {
        auto op_start = steady_clock::now();
        [[maybe_unused]] auto write_count = buf.write(std::span(write_data));
        [[maybe_unused]] auto read_count = buf.read(std::span(read_data));
        auto op_end = steady_clock::now();

        double latency_us = static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) / 1000.0;
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = "Circular Buffer";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = latencies[num_operations / 2];
    result.latency_p99 = latencies[num_operations * 99 / 100];
    result.latency_p999 = latencies[num_operations * 999 / 1000];

    return result;
}

benchmark_result benchmark_simd_crlf_search() {
    const size_t num_operations = 100000;
    std::vector<double> latencies;
    latencies.reserve(num_operations);

    std::string test_data(1000, 'X');
    test_data += "\r\n";
    test_data += std::string(500, 'Y');

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations; ++i) {
        auto op_start = steady_clock::now();
        const char* result = simd::find_crlf(test_data.data(), test_data.size());
        auto op_end = steady_clock::now();

        if (result == nullptr) {
            std::cerr << "CRLF search failed!\n";
        }

        double latency_us = static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) / 1000.0;
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = "SIMD CRLF Search (1.5KB buffer)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = latencies[num_operations / 2];
    result.latency_p99 = latencies[num_operations * 99 / 100];
    result.latency_p999 = latencies[num_operations * 999 / 1000];

    return result;
}

benchmark_result benchmark_http_parser() {
    const size_t num_operations = 50000;
    std::vector<double> latencies;
    latencies.reserve(num_operations);

    const std::string http_request =
        "GET /api/v1/users?id=123&name=test HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: Mozilla/5.0\r\n"
        "Accept: application/json\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: 24\r\n"
        "\r\n"
        "{\"key\":\"value\",\"num\":42}";

    auto start = steady_clock::now();
    size_t successful_parses = 0;

    for (size_t i = 0; i < num_operations; ++i) {
        auto op_start = steady_clock::now();

        http::parser parser;
        auto data_span = std::span(
            reinterpret_cast<const uint8_t*>(http_request.data()),
            http_request.size()
        );

        auto parse_result = parser.parse(data_span);

        auto op_end = steady_clock::now();

        if (parse_result && *parse_result == http::parser::state::complete) {
            ++successful_parses;
            double latency_us = static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) / 1000.0;
            latencies.push_back(latency_us);
        }
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    if (duration_ms == 0) {
        duration_ms = 1;
    }

    if (!latencies.empty()) {
        std::sort(latencies.begin(), latencies.end());
    }

    benchmark_result result;
    result.name = "HTTP Parser (Complete Request)";
    result.operations = successful_parses;
    result.duration_ms = duration_ms;
    result.throughput = (static_cast<double>(successful_parses) * 1000.0) / static_cast<double>(duration_ms);

    if (!latencies.empty()) {
        result.latency_p50 = latencies[latencies.size() / 2];
        result.latency_p99 = latencies[latencies.size() * 99 / 100];
        result.latency_p999 = latencies[latencies.size() * 999 / 1000];
    } else {
        result.latency_p50 = 0.0;
        result.latency_p99 = 0.0;
        result.latency_p999 = 0.0;
    }

    return result;
}

benchmark_result benchmark_memory_allocations() {
    const size_t num_operations = 100000;
    ring_buffer_queue<std::string> queue(1024);

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations; ++i) {
        queue.try_push(std::string("test_string_") + std::to_string(i));
        std::string val;
        queue.try_pop(val);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    benchmark_result result;
    result.name = "Memory Allocations (String Queue)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = 0.0;
    result.latency_p99 = 0.0;
    result.latency_p999 = 0.0;

    return result;
}

int main() {
    std::cout << "========================================\n";
    std::cout << "   KATANA Performance Benchmarks\n";
    std::cout << "========================================\n";

    std::vector<benchmark_result> results;

    std::cout << "\n[1/6] Benchmarking ring_buffer_queue (single thread)...\n";
    results.push_back(benchmark_ring_buffer_queue());
    print_result(results.back());

    std::cout << "\n[2/6] Benchmarking ring_buffer_queue (concurrent)...\n";
    results.push_back(benchmark_ring_buffer_concurrent());
    print_result(results.back());

    std::cout << "\n[3/6] Benchmarking circular_buffer...\n";
    results.push_back(benchmark_circular_buffer());
    print_result(results.back());

    std::cout << "\n[4/6] Benchmarking SIMD CRLF search...\n";
    results.push_back(benchmark_simd_crlf_search());
    print_result(results.back());

    std::cout << "\n[5/6] Benchmarking HTTP parser...\n";
    results.push_back(benchmark_http_parser());
    print_result(results.back());

    std::cout << "\n[6/6] Benchmarking memory allocations...\n";
    results.push_back(benchmark_memory_allocations());
    print_result(results.back());

    std::cout << "\n========================================\n";
    std::cout << "         Benchmark Summary\n";
    std::cout << "========================================\n";

    for (const auto& result : results) {
        std::cout << std::left << std::setw(40) << result.name << ": "
                  << std::fixed << std::setprecision(0) << result.throughput
                  << " ops/sec\n";
    }

    std::cout << "\nAll benchmarks completed successfully!\n";

    return 0;
}
