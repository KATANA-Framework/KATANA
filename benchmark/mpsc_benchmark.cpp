#include "katana/core/mpsc_queue.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
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

benchmark_result benchmark_mpsc_single_producer() {
    const size_t num_operations = 1000000;
    mpsc_queue<int> queue;
    std::vector<double> latencies;
    latencies.reserve(num_operations);

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations; ++i) {
        auto op_start = steady_clock::now();
        queue.push(static_cast<int>(i));
        auto val = queue.pop();
        auto op_end = steady_clock::now();

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) / 1000.0;
        latencies.push_back(latency_us);

        if (!val) {
            std::cerr << "Pop failed!\n";
        }
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = "MPSC Queue (Single Producer)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = latencies[num_operations / 2];
    result.latency_p99 = latencies[num_operations * 99 / 100];
    result.latency_p999 = latencies[num_operations * 999 / 1000];

    return result;
}

benchmark_result benchmark_mpsc_multi_producer(int num_producers) {
    const size_t total_operations = 1000000;
    const size_t ops_per_producer = total_operations / num_producers;
    mpsc_queue<int> queue;
    std::atomic<size_t> total_pushed{0};
    std::atomic<size_t> total_popped{0};

    auto start = steady_clock::now();

    std::vector<std::thread> producers;
    producers.reserve(num_producers);

    for (int t = 0; t < num_producers; ++t) {
        producers.emplace_back([&, t] {
            for (size_t i = 0; i < ops_per_producer; ++i) {
                queue.push(static_cast<int>(t * ops_per_producer + i));
                total_pushed.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    std::thread consumer([&] {
        while (total_popped.load(std::memory_order_relaxed) < total_operations) {
            auto val = queue.pop();
            if (val) {
                total_popped.fetch_add(1, std::memory_order_relaxed);
            } else {
                std::this_thread::yield();
            }
        }
    });

    for (auto& t : producers) {
        t.join();
    }
    consumer.join();

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    benchmark_result result;
    result.name = "MPSC Queue (" + std::to_string(num_producers) + " Producers)";
    result.operations = total_operations;
    result.duration_ms = duration_ms;
    result.throughput = (total_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = 0.0;
    result.latency_p99 = 0.0;
    result.latency_p999 = 0.0;

    return result;
}

benchmark_result benchmark_mpsc_with_limit() {
    const size_t num_operations = 500000;
    const size_t queue_limit = 1024;
    mpsc_queue<int> queue(queue_limit);

    auto start = steady_clock::now();

    std::atomic<size_t> total_pushed{0};
    std::atomic<size_t> total_popped{0};

    std::thread producer([&] {
        for (size_t i = 0; i < num_operations; ++i) {
            while (!queue.try_push(static_cast<int>(i))) {
                std::this_thread::yield();
            }
            total_pushed.fetch_add(1, std::memory_order_relaxed);
        }
    });

    std::thread consumer([&] {
        while (total_popped.load(std::memory_order_relaxed) < num_operations) {
            auto val = queue.pop();
            if (val) {
                total_popped.fetch_add(1, std::memory_order_relaxed);
            } else {
                std::this_thread::yield();
            }
        }
    });

    producer.join();
    consumer.join();

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    benchmark_result result;
    result.name = "MPSC Queue (Bounded 1024)";
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
    std::cout << "   KATANA MPSC Queue Benchmarks\n";
    std::cout << "========================================\n";

    std::vector<benchmark_result> results;

    std::cout << "\n[1/5] Benchmarking MPSC queue (single producer)...\n";
    results.push_back(benchmark_mpsc_single_producer());
    print_result(results.back());

    std::cout << "\n[2/5] Benchmarking MPSC queue (2 producers)...\n";
    results.push_back(benchmark_mpsc_multi_producer(2));
    print_result(results.back());

    std::cout << "\n[3/5] Benchmarking MPSC queue (4 producers)...\n";
    results.push_back(benchmark_mpsc_multi_producer(4));
    print_result(results.back());

    std::cout << "\n[4/5] Benchmarking MPSC queue (8 producers)...\n";
    results.push_back(benchmark_mpsc_multi_producer(8));
    print_result(results.back());

    std::cout << "\n[5/5] Benchmarking MPSC queue (bounded)...\n";
    results.push_back(benchmark_mpsc_with_limit());
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
