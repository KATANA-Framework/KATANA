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
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
#include <immintrin.h>
#endif

using namespace std::chrono;
using namespace katana;

namespace {
inline void cpu_pause() noexcept {
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
    _mm_pause();
#else
    std::this_thread::yield();
#endif
}

inline size_t work_items_for_worker(size_t total, int workers, int worker_index) noexcept {
    if (workers <= 0 || worker_index < 0 || worker_index >= workers) {
        return 0;
    }

    const size_t worker_count = static_cast<size_t>(workers);
    const size_t base = total / worker_count;
    const size_t remainder = total % worker_count;
    return base + (static_cast<size_t>(worker_index) < remainder ? 1 : 0);
}

inline double throughput_ops_per_sec(uint64_t operations, steady_clock::duration elapsed) noexcept {
    const auto duration_ns = duration_cast<nanoseconds>(elapsed).count();
    if (operations == 0 || duration_ns <= 0) {
        return 0.0;
    }

    return (static_cast<double>(operations) * 1'000'000'000.0) /
           static_cast<double>(duration_ns);
}
} // namespace

struct benchmark_result {
    std::string name;
    double throughput;
    double latency_p50 = -1.0;
    double latency_p99 = -1.0;
    double latency_p999 = -1.0;
    double retries_per_op_push = -1.0;
    double retries_per_op_pop = -1.0;
    double retries_per_op_total = -1.0;
    uint64_t producer_retries = 0;
    uint64_t consumer_retries = 0;
    int producers = 0;
    int consumers = 0;
    int hardware_threads = 0;
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
    if (result.latency_p50 >= 0.0 && result.latency_p99 >= 0.0 && result.latency_p999 >= 0.0) {
        std::cout << "Latency p50: " << std::fixed << std::setprecision(6) << result.latency_p50
                  << " us\n";
        std::cout << "Latency p99: " << std::fixed << std::setprecision(6) << result.latency_p99
                  << " us\n";
        std::cout << "Latency p999: " << std::fixed << std::setprecision(6)
                  << result.latency_p999 << " us\n";
    }
    if (result.retries_per_op_total >= 0.0) {
        std::cout << "Producer retries: " << result.producer_retries << "\n";
        std::cout << "Consumer retries: " << result.consumer_retries << "\n";
        std::cout << "Retries/op (push/pop/total): " << std::fixed << std::setprecision(6)
                  << result.retries_per_op_push << " / " << result.retries_per_op_pop << " / "
                  << result.retries_per_op_total << "\n";
        if (result.producers > 0 && result.consumers > 0 && result.hardware_threads > 0) {
            const double total_threads = static_cast<double>(result.producers + result.consumers);
            const double oversub = total_threads / static_cast<double>(result.hardware_threads);
            std::cout << "Thread topology: " << result.producers << "x" << result.consumers
                      << " (hw=" << result.hardware_threads
                      << ", oversubscription=" << std::setprecision(3) << oversub << "x)\n";
        }
    }
}

int detect_hardware_threads() {
    const unsigned hw = std::thread::hardware_concurrency();
    if (hw == 0U) {
        return 8;
    }
    return static_cast<int>(hw);
}

std::string ring_case_name(const std::string& base, int producers, int consumers) {
    std::ostringstream ss;
    ss << base << " " << producers << "x" << consumers << ")";
    return ss.str();
}

benchmark_result benchmark_ring_buffer_mpmc(const std::string& name,
                                            size_t num_operations,
                                            int producers,
                                            int consumers,
                                            size_t queue_capacity,
                                            size_t latency_sample_rate) {
    ring_buffer_queue<int> queue(queue_capacity, /*enable_spsc_fast_path=*/false);
    std::vector<std::vector<double>> consumer_latencies(static_cast<size_t>(consumers));
    std::vector<uint64_t> producer_retries_per_thread(static_cast<size_t>(producers), 0);
    std::vector<uint64_t> consumer_retries_per_thread(static_cast<size_t>(consumers), 0);
    const int hw_threads = detect_hardware_threads();

    auto start = steady_clock::now();

    std::vector<std::thread> producer_threads;
    producer_threads.reserve(static_cast<size_t>(producers));
    for (int p = 0; p < producers; ++p) {
        producer_threads.emplace_back([&, p] {
            const size_t ops_for_thread = work_items_for_worker(num_operations, producers, p);
            uint64_t local_retries = 0;
            for (size_t i = 0; i < ops_for_thread; ++i) {
                const int val = static_cast<int>(static_cast<size_t>(p) * 1000000 + i);
                while (!queue.try_push(val)) {
                    ++local_retries;
                    cpu_pause();
                }
            }
            producer_retries_per_thread[static_cast<size_t>(p)] = local_retries;
        });
    }

    std::vector<std::thread> consumer_threads;
    consumer_threads.reserve(static_cast<size_t>(consumers));
    for (int c = 0; c < consumers; ++c) {
        consumer_threads.emplace_back([&, c] {
            auto& samples = consumer_latencies[static_cast<size_t>(c)];
            const size_t target = work_items_for_worker(num_operations, consumers, c);
            samples.reserve(target / latency_sample_rate + 2);

            size_t consumed = 0;
            size_t sampled_ops = 0;
            uint64_t local_retries = 0;
            auto batch_start = steady_clock::now();
            while (consumed < target) {
                int val;
                if (queue.try_pop(val)) {
                    ++consumed;
                    ++sampled_ops;
                    if (sampled_ops >= latency_sample_rate) {
                        auto batch_end = steady_clock::now();
                        double latency_us = static_cast<double>(
                                                duration_cast<nanoseconds>(batch_end - batch_start)
                                                    .count()) /
                                            (1000.0 * static_cast<double>(sampled_ops));
                        samples.push_back(latency_us);
                        sampled_ops = 0;
                        batch_start = steady_clock::now();
                    }
                } else {
                    ++local_retries;
                    cpu_pause();
                }
            }

            if (sampled_ops > 0) {
                auto batch_end = steady_clock::now();
                double latency_us =
                    static_cast<double>(duration_cast<nanoseconds>(batch_end - batch_start).count()) /
                    (1000.0 * static_cast<double>(sampled_ops));
                samples.push_back(latency_us);
            }
            consumer_retries_per_thread[static_cast<size_t>(c)] = local_retries;
        });
    }

    for (auto& t : producer_threads)
        t.join();
    for (auto& t : consumer_threads)
        t.join();

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::vector<double> merged_latencies;
    for (const auto& local : consumer_latencies) {
        merged_latencies.insert(merged_latencies.end(), local.begin(), local.end());
    }
    std::sort(merged_latencies.begin(), merged_latencies.end());
    const uint64_t total_producer_retries =
        std::accumulate(producer_retries_per_thread.begin(), producer_retries_per_thread.end(), uint64_t{0});
    const uint64_t total_consumer_retries =
        std::accumulate(consumer_retries_per_thread.begin(), consumer_retries_per_thread.end(), uint64_t{0});

    benchmark_result result;
    result.name = name;
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = throughput_ops_per_sec(result.operations, end - start);
    result.producer_retries = total_producer_retries;
    result.consumer_retries = total_consumer_retries;
    result.producers = producers;
    result.consumers = consumers;
    result.hardware_threads = hw_threads;
    if (num_operations > 0) {
        result.retries_per_op_push =
            static_cast<double>(total_producer_retries) / static_cast<double>(num_operations);
        result.retries_per_op_pop =
            static_cast<double>(total_consumer_retries) / static_cast<double>(num_operations);
        result.retries_per_op_total =
            static_cast<double>(total_producer_retries + total_consumer_retries) /
            static_cast<double>(num_operations);
    }
    if (!merged_latencies.empty()) {
        result.latency_p50 = percentile(merged_latencies, 0.50);
        result.latency_p99 = percentile(merged_latencies, 0.99);
        result.latency_p999 = percentile(merged_latencies, 0.999);
    }
    return result;
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
    result.throughput = throughput_ops_per_sec(result.operations, end - start);
    result.latency_p50 = percentile(latencies, 0.50);
    result.latency_p99 = percentile(latencies, 0.99);
    result.latency_p999 = percentile(latencies, 0.999);

    return result;
}

benchmark_result benchmark_ring_buffer_concurrent() {
    return benchmark_ring_buffer_mpmc(
        "Ring Buffer Queue (Concurrent 4x4)",
        1000000,
        4,
        4,
        4096,
        1024);
}

benchmark_result benchmark_ring_buffer_high_contention() {
    return benchmark_ring_buffer_mpmc(
        "Ring Buffer Queue (High Contention 8x8)",
        1000000,
        8,
        8,
        2048,
        1024);
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
    result.throughput = throughput_ops_per_sec(result.operations, end - start);
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
    result.throughput = throughput_ops_per_sec(result.operations, end - start);
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
    result.throughput = throughput_ops_per_sec(result.operations, end - start);
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
    result.throughput = throughput_ops_per_sec(result.operations, end - start);

    if (!latencies.empty()) {
        result.latency_p50 = percentile(latencies, 0.50);
        result.latency_p99 = percentile(latencies, 0.99);
        result.latency_p999 = percentile(latencies, 0.999);
    } else {
        result.latency_p50 = -1.0;
        result.latency_p99 = -1.0;
        result.latency_p999 = -1.0;
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
    result.throughput = throughput_ops_per_sec(result.operations, end - start);
    result.latency_p50 = -1.0;
    result.latency_p99 = -1.0;
    result.latency_p999 = -1.0;

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
    result.throughput = throughput_ops_per_sec(result.operations, end - start);
    result.latency_p50 = -1.0;
    result.latency_p99 = -1.0;
    result.latency_p999 = -1.0;

    return result;
}

benchmark_result benchmark_simd_crlf_32kb() {
    const size_t num_operations = 30000;
    const size_t sample_rate = 15;
    std::vector<double> latencies;
    latencies.reserve(num_operations / sample_rate + 2);

    // 32KB buffer with CRLF near the end
    std::string test_data(32 * 1024, 'X');
    test_data.replace(test_data.size() - 4, 4, "AB\r\n");

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations;) {
        size_t batch = std::min(sample_rate, num_operations - i);
        auto op_start = steady_clock::now();
        size_t batch_end = i + batch;
        for (; i < batch_end; ++i) {
            const char* result = simd::find_crlf(test_data.data(), test_data.size());
            if (result == nullptr) {
                std::cerr << "CRLF search (32KB buffer) failed!\n";
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
    result.name = "SIMD CRLF Search (32KB buffer)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = throughput_ops_per_sec(result.operations, end - start);
    result.latency_p50 = percentile(latencies, 0.50);
    result.latency_p99 = percentile(latencies, 0.99);
    result.latency_p999 = percentile(latencies, 0.999);

    return result;
}

benchmark_result benchmark_simd_crlf_64kb() {
    const size_t num_operations = 20000;
    const size_t sample_rate = 10;
    std::vector<double> latencies;
    latencies.reserve(num_operations / sample_rate + 2);

    // 64KB buffer with CRLF near the end
    std::string test_data(64 * 1024, 'X');
    test_data.replace(test_data.size() - 4, 4, "AB\r\n");

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations;) {
        size_t batch = std::min(sample_rate, num_operations - i);
        auto op_start = steady_clock::now();
        size_t batch_end = i + batch;
        for (; i < batch_end; ++i) {
            const char* result = simd::find_crlf(test_data.data(), test_data.size());
            if (result == nullptr) {
                std::cerr << "CRLF search (64KB buffer) failed!\n";
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
    result.name = "SIMD CRLF Search (64KB buffer)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = throughput_ops_per_sec(result.operations, end - start);
    result.latency_p50 = percentile(latencies, 0.50);
    result.latency_p99 = percentile(latencies, 0.99);
    result.latency_p999 = percentile(latencies, 0.999);

    return result;
}

benchmark_result benchmark_simd_crlf_128kb() {
    const size_t num_operations = 10000;
    const size_t sample_rate = 5;
    std::vector<double> latencies;
    latencies.reserve(num_operations / sample_rate + 2);

    // 128KB buffer with CRLF near the end
    std::string test_data(128 * 1024, 'X');
    test_data.replace(test_data.size() - 4, 4, "AB\r\n");

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations;) {
        size_t batch = std::min(sample_rate, num_operations - i);
        auto op_start = steady_clock::now();
        size_t batch_end = i + batch;
        for (; i < batch_end; ++i) {
            const char* result = simd::find_crlf(test_data.data(), test_data.size());
            if (result == nullptr) {
                std::cerr << "CRLF search (128KB buffer) failed!\n";
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
    result.name = "SIMD CRLF Search (128KB buffer)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = throughput_ops_per_sec(result.operations, end - start);
    result.latency_p50 = percentile(latencies, 0.50);
    result.latency_p99 = percentile(latencies, 0.99);
    result.latency_p999 = percentile(latencies, 0.999);

    return result;
}

benchmark_result benchmark_ring_buffer_extreme_contention() {
    return benchmark_ring_buffer_mpmc(
        "Ring Buffer Queue (Extreme Contention 12x12)",
        1000000,
        12,
        12,
        4096,
        1024);
}

benchmark_result benchmark_ring_buffer_max_contention() {
    return benchmark_ring_buffer_mpmc(
        "Ring Buffer Queue (Max Contention 16x16)",
        1000000,
        16,
        16,
        8192,
        1024);
}

std::pair<int, int> derive_core_saturation_pair() {
    const int hw = detect_hardware_threads();
    int total_threads = std::max(4, hw);
    if (total_threads % 2 != 0) {
        --total_threads;
    }
    const int per_side = std::max(2, total_threads / 2);
    return {per_side, per_side};
}

std::pair<int, int> derive_oversubscribed_pair() {
    const int hw = detect_hardware_threads();
    const auto [sat_prod, sat_cons] = derive_core_saturation_pair();
    int per_side = std::max(std::max(sat_prod, sat_cons) + 2, hw + 2);
    per_side = std::min(per_side, 24); // keep benchmark runtime bounded on big machines
    return {per_side, per_side};
}

benchmark_result benchmark_ring_buffer_core_saturation() {
    const auto [producers, consumers] = derive_core_saturation_pair();
    return benchmark_ring_buffer_mpmc(
        ring_case_name("Ring Buffer Queue (Core Saturation", producers, consumers),
        1000000,
        producers,
        consumers,
        4096,
        1024);
}

benchmark_result benchmark_ring_buffer_oversubscribed() {
    const auto [producers, consumers] = derive_oversubscribed_pair();
    return benchmark_ring_buffer_mpmc(
        ring_case_name("Ring Buffer Queue (Oversubscribed", producers, consumers),
        1000000,
        producers,
        consumers,
        8192,
        1024);
}

benchmark_result benchmark_ring_buffer_2x2() {
    return benchmark_ring_buffer_mpmc(
        "Ring Buffer Queue (Concurrent 2x2)",
        1000000,
        2,
        2,
        4096,
        1024);
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
    result.throughput = throughput_ops_per_sec(result.operations, end - start);

    if (!latencies.empty()) {
        result.latency_p50 = percentile(latencies, 0.50);
        result.latency_p99 = percentile(latencies, 0.99);
        result.latency_p999 = percentile(latencies, 0.999);
    } else {
        result.latency_p50 = -1.0;
        result.latency_p99 = -1.0;
        result.latency_p999 = -1.0;
    }

    return result;
}

int main() {
    std::cout << "========================================\n";
    std::cout << "   KATANA Performance Benchmarks\n";
    std::cout << "========================================\n";

    std::vector<benchmark_result> results;

    std::cout << "\n--- Ring Buffer (Core-aware contention, <= HW threads) ---\n";
    std::cout << "\n[1/18] Benchmarking ring_buffer_queue (single thread)...\n";
    results.push_back(benchmark_ring_buffer_queue());
    print_result(results.back());

    std::cout << "\n[2/18] Benchmarking ring_buffer_queue (concurrent 2x2)...\n";
    results.push_back(benchmark_ring_buffer_2x2());
    print_result(results.back());

    std::cout << "\n[3/18] Benchmarking ring_buffer_queue (concurrent 4x4)...\n";
    results.push_back(benchmark_ring_buffer_concurrent());
    print_result(results.back());

    std::cout << "\n[4/18] Benchmarking ring_buffer_queue (core saturation)...\n";
    results.push_back(benchmark_ring_buffer_core_saturation());
    print_result(results.back());

    std::cout << "\n--- Ring Buffer (Oversubscribed contention, > HW threads) ---\n";

    std::cout << "\n[5/18] Benchmarking ring_buffer_queue (high contention 8x8)...\n";
    results.push_back(benchmark_ring_buffer_high_contention());
    print_result(results.back());

    std::cout << "\n[6/18] Benchmarking ring_buffer_queue (extreme contention 12x12)...\n";
    results.push_back(benchmark_ring_buffer_extreme_contention());
    print_result(results.back());

    std::cout << "\n[7/18] Benchmarking ring_buffer_queue (max contention 16x16)...\n";
    results.push_back(benchmark_ring_buffer_max_contention());
    print_result(results.back());

    std::cout << "\n[8/18] Benchmarking ring_buffer_queue (oversubscribed)...\n";
    results.push_back(benchmark_ring_buffer_oversubscribed());
    print_result(results.back());

    std::cout << "\n[9/18] Benchmarking circular_buffer...\n";
    results.push_back(benchmark_circular_buffer());
    print_result(results.back());

    std::cout << "\n[10/18] Benchmarking SIMD CRLF search (1.5KB)...\n";
    results.push_back(benchmark_simd_crlf_search());
    print_result(results.back());

    std::cout << "\n[11/18] Benchmarking SIMD CRLF search (16KB)...\n";
    results.push_back(benchmark_simd_crlf_large_buffer());
    print_result(results.back());

    std::cout << "\n[12/18] Benchmarking SIMD CRLF search (32KB)...\n";
    results.push_back(benchmark_simd_crlf_32kb());
    print_result(results.back());

    std::cout << "\n[13/18] Benchmarking SIMD CRLF search (64KB)...\n";
    results.push_back(benchmark_simd_crlf_64kb());
    print_result(results.back());

    std::cout << "\n[14/18] Benchmarking SIMD CRLF search (128KB)...\n";
    results.push_back(benchmark_simd_crlf_128kb());
    print_result(results.back());

    std::cout << "\n[15/18] Benchmarking HTTP parser (full message)...\n";
    results.push_back(benchmark_http_parser());
    print_result(results.back());

    std::cout << "\n[16/18] Benchmarking HTTP parser (fragmented)...\n";
    results.push_back(benchmark_http_parser_fragmented());
    print_result(results.back());

    std::cout << "\n[17/18] Benchmarking arena allocations...\n";
    results.push_back(benchmark_arena_small_allocs());
    print_result(results.back());

    std::cout << "\n[18/18] Benchmarking memory allocations...\n";
    results.push_back(benchmark_memory_allocations());
    print_result(results.back());

    std::cout << "\n========================================\n";
    std::cout << "         Benchmark Summary\n";
    std::cout << "========================================\n";

    for (const auto& result : results) {
        std::cout << std::left << std::setw(45) << result.name << ": " << std::fixed
                  << std::setprecision(0) << result.throughput << " ops/sec\n";
    }

    std::cout << "\nAll benchmarks completed successfully!\n";

    return 0;
}
