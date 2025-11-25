#include "katana/core/io_buffer.hpp"

#include <algorithm>
#include <chrono>
#include <cstring>
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

benchmark_result benchmark_buffer_append_small() {
    const size_t num_operations = 100000;
    std::vector<double> latencies;
    latencies.reserve(num_operations);

    std::vector<uint8_t> data(64, 'A');

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations; ++i) {
        auto op_start = steady_clock::now();

        io_buffer buffer;
        buffer.append(std::span(data));

        auto op_end = steady_clock::now();

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) / 1000.0;
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = "IO Buffer Append (64 bytes)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = latencies[num_operations / 2];
    result.latency_p99 = latencies[num_operations * 99 / 100];
    result.latency_p999 = latencies[num_operations * 999 / 1000];

    return result;
}

benchmark_result benchmark_buffer_append_large() {
    const size_t num_operations = 50000;
    std::vector<double> latencies;
    latencies.reserve(num_operations);

    std::vector<uint8_t> data(4096, 'B');

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations; ++i) {
        auto op_start = steady_clock::now();

        io_buffer buffer;
        buffer.append(std::span(data));

        auto op_end = steady_clock::now();

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) / 1000.0;
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = "IO Buffer Append (4KB)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = latencies[num_operations / 2];
    result.latency_p99 = latencies[num_operations * 99 / 100];
    result.latency_p999 = latencies[num_operations * 999 / 1000];

    return result;
}

benchmark_result benchmark_buffer_read_write() {
    const size_t num_operations = 100000;
    std::vector<double> latencies;
    latencies.reserve(num_operations);

    std::vector<uint8_t> write_data(256, 'C');

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations; ++i) {
        auto op_start = steady_clock::now();

        io_buffer buffer;
        buffer.append(std::span(write_data));
        auto readable = buffer.readable_span();
        buffer.consume(readable.size());

        auto op_end = steady_clock::now();

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) / 1000.0;
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = "IO Buffer Read/Write (256B)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = latencies[num_operations / 2];
    result.latency_p99 = latencies[num_operations * 99 / 100];
    result.latency_p999 = latencies[num_operations * 999 / 1000];

    return result;
}

benchmark_result benchmark_buffer_writable_commit() {
    const size_t num_operations = 100000;
    std::vector<double> latencies;
    latencies.reserve(num_operations);

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations; ++i) {
        auto op_start = steady_clock::now();

        io_buffer buffer;
        auto writable = buffer.writable_span(128);
        std::memset(writable.data(), 'D', writable.size());
        buffer.commit(writable.size());

        auto op_end = steady_clock::now();

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) / 1000.0;
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = "IO Buffer Writable/Commit (128B)";
    result.operations = num_operations;
    result.duration_ms = duration_ms;
    result.throughput = (num_operations * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = latencies[num_operations / 2];
    result.latency_p99 = latencies[num_operations * 99 / 100];
    result.latency_p999 = latencies[num_operations * 999 / 1000];

    return result;
}

benchmark_result benchmark_scatter_gather() {
    const size_t num_operations = 100000;
    std::vector<double> latencies;
    latencies.reserve(num_operations);

    std::vector<uint8_t> buf1(256);
    std::vector<uint8_t> buf2(512);
    std::vector<uint8_t> buf3(1024);

    auto start = steady_clock::now();

    for (size_t i = 0; i < num_operations; ++i) {
        auto op_start = steady_clock::now();

        scatter_gather_write sg;
        sg.add_buffer(std::span(buf1.data(), buf1.size()));
        sg.add_buffer(std::span(buf2.data(), buf2.size()));
        sg.add_buffer(std::span(buf3.data(), buf3.size()));

        auto op_end = steady_clock::now();

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(op_end - op_start).count()) / 1000.0;
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = "Scatter/Gather Write (3 buffers)";
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
    std::cout << "   KATANA IO Buffer Benchmarks\n";
    std::cout << "========================================\n";

    std::vector<benchmark_result> results;

    std::cout << "\n[1/5] Benchmarking buffer append (small)...\n";
    results.push_back(benchmark_buffer_append_small());
    print_result(results.back());

    std::cout << "\n[2/5] Benchmarking buffer append (large)...\n";
    results.push_back(benchmark_buffer_append_large());
    print_result(results.back());

    std::cout << "\n[3/5] Benchmarking buffer read/write...\n";
    results.push_back(benchmark_buffer_read_write());
    print_result(results.back());

    std::cout << "\n[4/5] Benchmarking writable/commit...\n";
    results.push_back(benchmark_buffer_writable_commit());
    print_result(results.back());

    std::cout << "\n[5/5] Benchmarking scatter/gather...\n";
    results.push_back(benchmark_scatter_gather());
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
