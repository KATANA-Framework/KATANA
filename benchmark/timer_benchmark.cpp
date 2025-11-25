#include "katana/core/wheel_timer.hpp"

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

benchmark_result benchmark_timer_add() {
    const size_t num_operations = 100000;
    wheel_timer timer;
    std::vector<double> latencies;
    latencies.reserve(num_operations);

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations; ++i) {
        auto op_start = steady_clock::now();
        timer.add(milliseconds(100 + (i % 1000)), []() {});
        auto op_end = steady_clock::now();

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) / 1000.0;
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = "Wheel Timer (Add Operations)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = latencies[num_operations / 2];
    result.latency_p99 = latencies[num_operations * 99 / 100];
    result.latency_p999 = latencies[num_operations * 999 / 1000];

    return result;
}

benchmark_result benchmark_timer_cancel() {
    const size_t num_operations = 50000;
    wheel_timer timer;
    std::vector<double> latencies;
    latencies.reserve(num_operations);

    std::vector<wheel_timer<>::timeout_id> ids;
    ids.reserve(num_operations);

    for (size_t i = 0; i < num_operations; ++i) {
        ids.push_back(timer.add(milliseconds(1000), []() {}));
    }

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations; ++i) {
        auto op_start = steady_clock::now();
        (void)timer.cancel(ids[i]);
        auto op_end = steady_clock::now();

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) / 1000.0;
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = "Wheel Timer (Cancel Operations)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = latencies[num_operations / 2];
    result.latency_p99 = latencies[num_operations * 99 / 100];
    result.latency_p999 = latencies[num_operations * 999 / 1000];

    return result;
}

benchmark_result benchmark_timer_execution() {
    const size_t num_timers = 10000;
    wheel_timer timer;
    std::atomic<size_t> executed{0};

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_timers; ++i) {
        timer.add(milliseconds(100 + (i % 200)),
                  [&executed]() { executed.fetch_add(1, std::memory_order_relaxed); });
    }

    while (executed.load(std::memory_order_relaxed) < num_timers) {
        timer.tick();
        std::this_thread::sleep_for(milliseconds(10));
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    benchmark_result result;
    result.name = "Wheel Timer (Execution 10k)";
    result.operations = num_timers;
    result.duration_ms = duration_ms;
    result.throughput = (num_timers * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = 0.0;
    result.latency_p99 = 0.0;
    result.latency_p999 = 0.0;

    return result;
}

benchmark_result benchmark_timer_tick() {
    const size_t num_operations = 50000;
    wheel_timer timer;
    std::vector<double> latencies;
    latencies.reserve(num_operations);

    for (size_t i = 0; i < 100; ++i) {
        timer.add(milliseconds(10000), []() {});
    }

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations; ++i) {
        auto op_start = steady_clock::now();
        timer.tick(steady_clock::now());
        auto op_end = steady_clock::now();

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) / 1000.0;
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = "Wheel Timer (Tick Operations)";
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
    std::cout << "   KATANA Wheel Timer Benchmarks\n";
    std::cout << "========================================\n";

    std::vector<benchmark_result> results;

    std::cout << "\n[1/4] Benchmarking timer add operations...\n";
    results.push_back(benchmark_timer_add());
    print_result(results.back());

    std::cout << "\n[2/4] Benchmarking timer cancel operations...\n";
    results.push_back(benchmark_timer_cancel());
    print_result(results.back());

    std::cout << "\n[3/4] Benchmarking timer tick operations...\n";
    results.push_back(benchmark_timer_tick());
    print_result(results.back());

    std::cout << "\n[4/4] Benchmarking timer execution...\n";
    results.push_back(benchmark_timer_execution());
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
