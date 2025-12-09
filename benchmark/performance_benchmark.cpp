#include "katana/core/arena.hpp"
#include "katana/core/circular_buffer.hpp"
#include "katana/core/http.hpp"
#include "katana/core/reactor_pool.hpp"
#include "katana/core/ring_buffer_queue.hpp"
#include "katana/core/simd_utils.hpp"
#include "katana/core/tcp_listener.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <thread>
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

double percentile(const std::vector<double>& sorted_values, double pct) {
    if (sorted_values.empty()) {
        return 0.0;
    }

    double clamped = std::min(1.0, std::max(0.0, pct));
    size_t idx = static_cast<size_t>(clamped * static_cast<double>(sorted_values.size() - 1));
    return sorted_values[idx];
}

void print_result(const benchmark_result& result) {
    std::cout << "\n=== " << result.name << " ===\n";
    std::cout << "Operations: " << result.operations << "\n";
    std::cout << "Duration: " << result.duration_ms << " ms\n";
    std::cout << "Throughput: " << std::fixed << std::setprecision(2) << result.throughput
              << " ops/sec\n";
    std::cout << "Latency p50: " << std::fixed << std::setprecision(3) << result.latency_p50
              << " us\n";
    std::cout << "Latency p99: " << std::fixed << std::setprecision(3) << result.latency_p99
              << " us\n";
    std::cout << "Latency p999: " << std::fixed << std::setprecision(3) << result.latency_p999
              << " us\n";
}

benchmark_result benchmark_ring_buffer_queue() {
    const size_t num_operations = 1000000;
    const size_t sample_rate = 128;
    ring_buffer_queue<int> queue(1024);
    std::vector<double> latencies;
    latencies.reserve(num_operations / sample_rate + 2);

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations;) {
        size_t batch = std::min(sample_rate, num_operations - i);
        auto op_start = steady_clock::now();
        size_t batch_end = i + batch;
        for (; i < batch_end; ++i) {
            queue.try_push(static_cast<int>(i));
            int val;
            queue.try_pop(val);
        }
        auto op_end = steady_clock::now();

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) /
            (1000.0 * static_cast<double>(batch));
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
    result.latency_p50 = percentile(latencies, 0.50);
    result.latency_p99 = percentile(latencies, 0.99);
    result.latency_p999 = percentile(latencies, 0.999);

    return result;
}

benchmark_result benchmark_ring_buffer_concurrent() {
    const size_t num_operations = 1000000;
    const int num_threads = 4;
    ring_buffer_queue<int> queue(4096, /*enable_spsc_fast_path=*/false);
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

    for (auto& t : producers)
        t.join();
    for (auto& t : consumers)
        t.join();

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

benchmark_result benchmark_ring_buffer_high_contention() {
    const size_t num_operations = 1000000;
    const int producers = 8;
    const int consumers = 8;
    ring_buffer_queue<int> queue(2048, /*enable_spsc_fast_path=*/false);
    std::atomic<size_t> total_done{0};

    auto start = steady_clock::now();

    std::vector<std::thread> prod_threads;
    prod_threads.reserve(static_cast<size_t>(producers));
    for (int p = 0; p < producers; ++p) {
        prod_threads.emplace_back([&, p] {
            for (size_t i = 0; i < num_operations / static_cast<size_t>(producers); ++i) {
                const int val = static_cast<int>(static_cast<size_t>(p) * 1000000 + i);
                while (!queue.try_push(val)) {
                    std::this_thread::yield();
                }
            }
        });
    }

    std::vector<std::thread> cons_threads;
    cons_threads.reserve(static_cast<size_t>(consumers));
    for (int c = 0; c < consumers; ++c) {
        cons_threads.emplace_back([&] {
            while (total_done.load(std::memory_order_relaxed) < num_operations) {
                int val;
                if (queue.try_pop(val)) {
                    total_done.fetch_add(1, std::memory_order_relaxed);
                } else {
                    std::this_thread::yield();
                }
            }
        });
    }

    for (auto& t : prod_threads)
        t.join();
    for (auto& t : cons_threads)
        t.join();

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    benchmark_result result;
    result.name = "Ring Buffer Queue (High Contention 8x8)";
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
    const size_t sample_rate = 100;
    circular_buffer buf(4096);
    std::vector<double> latencies;
    latencies.reserve(num_operations / sample_rate + 2);

    std::vector<uint8_t> write_data(64, 'A');
    std::vector<uint8_t> read_data(64);

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations;) {
        size_t batch = std::min(sample_rate, num_operations - i);
        auto op_start = steady_clock::now();
        size_t batch_end = i + batch;
        for (; i < batch_end; ++i) {
            [[maybe_unused]] auto write_count = buf.write(std::span(write_data));
            [[maybe_unused]] auto read_count = buf.read(std::span(read_data));
        }
        auto op_end = steady_clock::now();

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) /
            (1000.0 * static_cast<double>(batch));
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
    result.latency_p50 = percentile(latencies, 0.50);
    result.latency_p99 = percentile(latencies, 0.99);
    result.latency_p999 = percentile(latencies, 0.999);

    return result;
}

benchmark_result benchmark_simd_crlf_search() {
    const size_t num_operations = 100000;
    const size_t sample_rate = 50;
    std::vector<double> latencies;
    latencies.reserve(num_operations / sample_rate + 2);

    std::string test_data(1000, 'X');
    test_data += "\r\n";
    test_data += std::string(500, 'Y');

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations;) {
        size_t batch = std::min(sample_rate, num_operations - i);
        auto op_start = steady_clock::now();
        size_t batch_end = i + batch;
        for (; i < batch_end; ++i) {
            const char* result = simd::find_crlf(test_data.data(), test_data.size());
            if (result == nullptr) {
                std::cerr << "CRLF search failed!\n";
            }
        }
        auto op_end = steady_clock::now();

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) /
            (1000.0 * static_cast<double>(batch));
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
    result.latency_p50 = percentile(latencies, 0.50);
    result.latency_p99 = percentile(latencies, 0.99);
    result.latency_p999 = percentile(latencies, 0.999);

    return result;
}

benchmark_result benchmark_simd_crlf_large_buffer() {
    const size_t num_operations = 50000;
    const size_t sample_rate = 25;
    std::vector<double> latencies;
    latencies.reserve(num_operations / sample_rate + 2);

    std::string test_data(16 * 1024, 'Z');
    test_data.replace(test_data.size() - 4, 4, "AB\r\n");

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations;) {
        size_t batch = std::min(sample_rate, num_operations - i);
        auto op_start = steady_clock::now();
        size_t batch_end = i + batch;
        for (; i < batch_end; ++i) {
            const char* result = simd::find_crlf(test_data.data(), test_data.size());
            if (result == nullptr) {
                std::cerr << "CRLF search (large buffer) failed!\n";
            }
        }
        auto op_end = steady_clock::now();

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) /
            (1000.0 * static_cast<double>(batch));
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = "SIMD CRLF Search (16KB buffer)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = percentile(latencies, 0.50);
    result.latency_p99 = percentile(latencies, 0.99);
    result.latency_p999 = percentile(latencies, 0.999);

    return result;
}

benchmark_result benchmark_http_parser() {
    const size_t num_operations = 50000;
    const size_t sample_rate = 20;
    std::vector<double> latencies;
    latencies.reserve(num_operations / sample_rate + 2);

    const std::string http_request = "GET /api/v1/users?id=123&name=test HTTP/1.1\r\n"
                                     "Host: example.com\r\n"
                                     "User-Agent: Mozilla/5.0\r\n"
                                     "Accept: application/json\r\n"
                                     "Connection: keep-alive\r\n"
                                     "Content-Length: 24\r\n"
                                     "\r\n"
                                     "{\"key\":\"value\",\"num\":42}";

    auto start = steady_clock::now();
    size_t successful_parses = 0;

    for (size_t i = 0; i < num_operations;) {
        size_t batch = std::min(sample_rate, num_operations - i);
        auto op_start = steady_clock::now();
        size_t batch_end = i + batch;
        size_t batch_success = 0;

        for (; i < batch_end; ++i) {
            monotonic_arena arena;
            http::parser parser(&arena);
            auto data_span = std::span(reinterpret_cast<const uint8_t*>(http_request.data()),
                                       http_request.size());

            auto parse_result = parser.parse(data_span);

            if (parse_result && *parse_result == http::parser::state::complete) {
                ++successful_parses;
                ++batch_success;
            }
        }

        auto op_end = steady_clock::now();
        if (batch_success > 0) {
            double latency_us =
                static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) /
                (1000.0 * static_cast<double>(batch_success));
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
    result.throughput =
        (static_cast<double>(successful_parses) * 1000.0) / static_cast<double>(duration_ms);

    if (!latencies.empty()) {
        result.latency_p50 = percentile(latencies, 0.50);
        result.latency_p99 = percentile(latencies, 0.99);
        result.latency_p999 = percentile(latencies, 0.999);
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

benchmark_result benchmark_arena_small_allocs() {
    const size_t num_operations = 500000;
    const size_t payload_size = 64;
    std::vector<std::vector<char>> payloads;
    payloads.reserve(num_operations);

    auto start = steady_clock::now();
    for (size_t i = 0; i < num_operations; ++i) {
        monotonic_arena arena;
        auto* data = static_cast<char*>(arena.allocate(payload_size, alignof(char)));
        std::fill_n(data, payload_size, static_cast<char>('A' + (i % 26)));
        payloads.emplace_back(data, data + payload_size);
    }
    auto end = steady_clock::now();

    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    benchmark_result result;
    result.name = "Arena Allocations (64B objects)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = 0.0;
    result.latency_p99 = 0.0;
    result.latency_p999 = 0.0;

    return result;
}

benchmark_result benchmark_http_parser_fragmented() {
    const size_t num_operations = 50000;
    const size_t sample_rate = 20;
    std::vector<double> latencies;
    latencies.reserve(num_operations / sample_rate + 2);

    const std::string body = R"({"payload":"fragmented"})";
    const std::string http_request = "POST /upload HTTP/1.1\r\n"
                                     "Host: example.com\r\n"
                                     "User-Agent: curl/8.0\r\n"
                                     "Content-Type: application/json\r\n"
                                     "Content-Length: " +
                                     std::to_string(body.size()) + "\r\n\r\n" + body;

    auto start = steady_clock::now();
    size_t successful_parses = 0;

    for (size_t i = 0; i < num_operations;) {
        size_t batch = std::min(sample_rate, num_operations - i);
        auto op_start = steady_clock::now();
        size_t batch_end = i + batch;
        size_t batch_success = 0;

        for (; i < batch_end; ++i) {
            monotonic_arena arena;
            http::parser parser(&arena);
            auto data_span = std::span(reinterpret_cast<const uint8_t*>(http_request.data()),
                                       http_request.size());

            // Feed the parser in two fragments to simulate TCP segmentation.
            auto first = data_span.first(data_span.size() / 2);
            auto second = data_span.last(data_span.size() - first.size());

            auto first_res = parser.parse(first);
            if (!first_res) {
                continue;
            }
            auto second_res = parser.parse(second);

            if (second_res && *second_res == http::parser::state::complete) {
                ++successful_parses;
                ++batch_success;
            }
        }

        auto op_end = steady_clock::now();
        if (batch_success > 0) {
            double latency_us =
                static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) /
                (1000.0 * static_cast<double>(batch_success));
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
    result.name = "HTTP Parser (Fragmented Request)";
    result.operations = successful_parses;
    result.duration_ms = duration_ms;
    result.throughput =
        (static_cast<double>(successful_parses) * 1000.0) / static_cast<double>(duration_ms);

    if (!latencies.empty()) {
        result.latency_p50 = percentile(latencies, 0.50);
        result.latency_p99 = percentile(latencies, 0.99);
        result.latency_p999 = percentile(latencies, 0.999);
    } else {
        result.latency_p50 = 0.0;
        result.latency_p99 = 0.0;
        result.latency_p999 = 0.0;
    }

    return result;
}

int main() {
    std::cout << "========================================\n";
    std::cout << "   KATANA Performance Benchmarks\n";
    std::cout << "========================================\n";

    std::vector<benchmark_result> results;

    std::cout << "\n[1/10] Benchmarking ring_buffer_queue (single thread)...\n";
    results.push_back(benchmark_ring_buffer_queue());
    print_result(results.back());

    std::cout << "\n[2/10] Benchmarking ring_buffer_queue (concurrent)...\n";
    results.push_back(benchmark_ring_buffer_concurrent());
    print_result(results.back());

    std::cout << "\n[3/10] Benchmarking ring_buffer_queue (high contention)...\n";
    results.push_back(benchmark_ring_buffer_high_contention());
    print_result(results.back());

    std::cout << "\n[4/10] Benchmarking circular_buffer...\n";
    results.push_back(benchmark_circular_buffer());
    print_result(results.back());

    std::cout << "\n[5/10] Benchmarking SIMD CRLF search (1.5KB)...\n";
    results.push_back(benchmark_simd_crlf_search());
    print_result(results.back());

    std::cout << "\n[6/10] Benchmarking SIMD CRLF search (16KB)...\n";
    results.push_back(benchmark_simd_crlf_large_buffer());
    print_result(results.back());

    std::cout << "\n[7/10] Benchmarking HTTP parser (full message)...\n";
    results.push_back(benchmark_http_parser());
    print_result(results.back());

    std::cout << "\n[8/10] Benchmarking HTTP parser (fragmented)...\n";
    results.push_back(benchmark_http_parser_fragmented());
    print_result(results.back());

    std::cout << "\n[9/10] Benchmarking arena allocations...\n";
    results.push_back(benchmark_arena_small_allocs());
    print_result(results.back());

    std::cout << "\n[10/10] Benchmarking memory allocations...\n";
    results.push_back(benchmark_memory_allocations());
    print_result(results.back());

    std::cout << "\n========================================\n";
    std::cout << "         Benchmark Summary\n";
    std::cout << "========================================\n";

    for (const auto& result : results) {
        std::cout << std::left << std::setw(40) << result.name << ": " << std::fixed
                  << std::setprecision(0) << result.throughput << " ops/sec\n";
    }

    std::cout << "\nAll benchmarks completed successfully!\n";

    return 0;
}
