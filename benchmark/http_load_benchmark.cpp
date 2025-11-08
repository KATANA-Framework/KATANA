#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <thread>
#include <atomic>
#include <iomanip>
#include <cmath>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

using clock_type = std::chrono::high_resolution_clock;
using duration_ns = std::chrono::nanoseconds;

struct sample {
    duration_ns latency;
    bool success;
    clock_type::time_point timestamp;
};

class histogram {
public:
    histogram() : buckets_(100, 0) {}

    void record(int64_t value_ns) {
        double value_ms = static_cast<double>(value_ns) / 1'000'000.0;

        size_t bucket_idx;
        if (value_ms < 1.0) {
            bucket_idx = static_cast<size_t>(value_ms * 10);
        } else if (value_ms < 10.0) {
            bucket_idx = 10 + static_cast<size_t>((value_ms - 1.0) * 9);
        } else if (value_ms < 100.0) {
            bucket_idx = 91 + static_cast<size_t>((value_ms - 10.0) / 10);
        } else {
            bucket_idx = 99;
        }

        if (bucket_idx < buckets_.size()) {
            buckets_[bucket_idx]++;
        }
    }

    void print() const {
        std::cout << "\n=== Latency Histogram ===\n";

        size_t max_count = *std::max_element(buckets_.begin(), buckets_.end());
        if (max_count == 0) return;

        constexpr size_t bar_width = 50;

        for (size_t i = 0; i < buckets_.size(); ++i) {
            if (buckets_[i] == 0) continue;

            double range_start, range_end;
            if (i < 10) {
                range_start = static_cast<double>(i) * 0.1;
                range_end = static_cast<double>(i + 1) * 0.1;
            } else if (i < 91) {
                range_start = 1.0 + static_cast<double>(i - 10) / 9.0;
                range_end = 1.0 + static_cast<double>(i - 9) / 9.0;
            } else {
                range_start = 10.0 + static_cast<double>(i - 91) * 10.0;
                range_end = 10.0 + static_cast<double>(i - 90) * 10.0;
            }

            size_t bar_len = (buckets_[i] * bar_width) / max_count;

            std::cout << std::fixed << std::setprecision(2) << std::setw(7)
                      << range_start << "-" << std::setw(7) << range_end << " ms [";

            for (size_t j = 0; j < bar_width; ++j) {
                std::cout << (j < bar_len ? '#' : ' ');
            }

            std::cout << "] " << buckets_[i] << "\n";
        }
    }

private:
    std::vector<size_t> buckets_;
};

class stats_tracker {
public:
    void record(duration_ns latency, bool success, clock_type::time_point ts) {
        std::lock_guard<std::mutex> lock(mutex_);
        samples_.push_back({latency, success, ts});
    }

    void print_stats() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (samples_.empty()) {
            std::cout << "No samples recorded\n";
            return;
        }

        std::vector<int64_t> latencies;
        size_t successful = 0;
        histogram hist;

        for (const auto& s : samples_) {
            if (s.success) {
                int64_t ns = s.latency.count();
                latencies.push_back(ns);
                hist.record(ns);
                ++successful;
            }
        }

        if (latencies.empty()) {
            std::cout << "No successful requests\n";
            return;
        }

        std::sort(latencies.begin(), latencies.end());

        auto p50 = latencies[latencies.size() * 50 / 100];
        auto p75 = latencies[latencies.size() * 75 / 100];
        auto p90 = latencies[latencies.size() * 90 / 100];
        auto p95 = latencies[latencies.size() * 95 / 100];
        auto p99 = latencies[latencies.size() * 99 / 100];
        auto p999 = latencies[latencies.size() * 999 / 1000];

        int64_t sum = 0;
        for (auto l : latencies) sum += l;
        auto avg = sum / static_cast<int64_t>(latencies.size());

        double sum_sq_diff = 0.0;
        for (auto l : latencies) {
            double diff = static_cast<double>(l - avg);
            sum_sq_diff += diff * diff;
        }
        auto stddev = std::sqrt(sum_sq_diff / static_cast<double>(latencies.size()));

        std::cout << "\n=== Latency Statistics ===\n";
        std::cout << "Total requests:  " << samples_.size() << "\n";
        std::cout << "Successful:      " << successful << "\n";
        std::cout << "Failed:          " << (samples_.size() - successful) << "\n";
        std::cout << "Success rate:    " << std::fixed << std::setprecision(2)
                  << (100.0 * static_cast<double>(successful) / static_cast<double>(samples_.size())) << "%\n";

        std::cout << "\nLatency (ms):\n";
        std::cout << "  Min:     " << std::setw(8) << to_ms(latencies.front()) << " ms\n";
        std::cout << "  Avg:     " << std::setw(8) << to_ms(avg) << " ms\n";
        std::cout << "  Stddev:  " << std::setw(8) << (stddev / 1'000'000.0) << " ms\n";
        std::cout << "  p50:     " << std::setw(8) << to_ms(p50) << " ms\n";
        std::cout << "  p75:     " << std::setw(8) << to_ms(p75) << " ms\n";
        std::cout << "  p90:     " << std::setw(8) << to_ms(p90) << " ms\n";
        std::cout << "  p95:     " << std::setw(8) << to_ms(p95) << " ms\n";
        std::cout << "  p99:     " << std::setw(8) << to_ms(p99) << " ms\n";
        std::cout << "  p99.9:   " << std::setw(8) << to_ms(p999) << " ms\n";
        std::cout << "  Max:     " << std::setw(8) << to_ms(latencies.back()) << " ms\n";

        hist.print();

        if (to_ms(p99) < 2.0) {
            std::cout << "\n✓ PASS: p99 < 2.0 ms\n";
        } else {
            std::cout << "\n✗ FAIL: p99 >= 2.0 ms\n";
        }
    }

    void print_throughput(duration_ns total_duration) {
        std::lock_guard<std::mutex> lock(mutex_);

        if (samples_.empty()) return;

        size_t successful = 0;
        for (const auto& s : samples_) {
            if (s.success) ++successful;
        }

        double duration_sec = static_cast<double>(total_duration.count()) / 1'000'000'000.0;
        double rps = static_cast<double>(successful) / duration_sec;

        std::cout << "\n=== Throughput ===\n";
        std::cout << "Duration:        " << std::fixed << std::setprecision(3)
                  << duration_sec << " seconds\n";
        std::cout << "Requests/sec:    " << std::fixed << std::setprecision(0)
                  << rps << " req/s\n";
    }

private:
    double to_ms(int64_t ns) const {
        return static_cast<double>(ns) / 1'000'000.0;
    }

    std::vector<sample> samples_;
    std::mutex mutex_;
};

int32_t create_connection(const char* host, uint16_t port) {
    int32_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return -1;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        close(sockfd);
        return -1;
    }

    return sockfd;
}

bool send_request(int32_t sockfd, const char* request) {
    size_t total_sent = 0;
    size_t request_len = std::strlen(request);

    while (total_sent < request_len) {
        ssize_t sent = send(sockfd, request + total_sent, request_len - total_sent, 0);
        if (sent <= 0) return false;
        total_sent += static_cast<size_t>(sent);
    }

    char buffer[4096];
    ssize_t received = recv(sockfd, buffer, sizeof(buffer), 0);
    return received > 0;
}

void worker_thread(stats_tracker& tracker, const char* host, uint16_t port,
                  const char* request, size_t requests_per_thread,
                  std::atomic<bool>& start_flag, bool keep_alive) {
    while (!start_flag.load(std::memory_order_acquire)) {
        std::this_thread::yield();
    }

    if (keep_alive) {
        int32_t sockfd = create_connection(host, port);
        if (sockfd < 0) {
            std::cerr << "Failed to connect\n";
            return;
        }

        for (size_t i = 0; i < requests_per_thread; ++i) {
            auto start = clock_type::now();
            bool success = send_request(sockfd, request);
            auto end = clock_type::now();

            auto latency = std::chrono::duration_cast<duration_ns>(end - start);
            tracker.record(latency, success, end);
        }

        close(sockfd);
    } else {
        for (size_t i = 0; i < requests_per_thread; ++i) {
            int32_t sockfd = create_connection(host, port);
            if (sockfd < 0) {
                tracker.record(duration_ns(0), false, clock_type::now());
                continue;
            }

            auto start = clock_type::now();
            bool success = send_request(sockfd, request);
            auto end = clock_type::now();

            auto latency = std::chrono::duration_cast<duration_ns>(end - start);
            tracker.record(latency, success, end);

            close(sockfd);
        }
    }
}

int main(int argc, char* argv[]) {
    const char* host = "127.0.0.1";
    uint16_t port = 8080;
    size_t total_requests = 100000;
    size_t num_threads = 10;
    bool keep_alive = true;

    if (argc > 1) total_requests = std::stoull(argv[1]);
    if (argc > 2) num_threads = std::stoull(argv[2]);
    if (argc > 3) keep_alive = std::string(argv[3]) == "1";

    const char* request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";
    size_t requests_per_thread = total_requests / num_threads;

    std::cout << "=== HTTP Load Benchmark ===\n";
    std::cout << "Target:              " << host << ":" << port << "\n";
    std::cout << "Total requests:      " << total_requests << "\n";
    std::cout << "Threads:             " << num_threads << "\n";
    std::cout << "Requests/thread:     " << requests_per_thread << "\n";
    std::cout << "Keep-alive:          " << (keep_alive ? "yes" : "no") << "\n";
    std::cout << "\nStarting benchmark...\n";

    stats_tracker tracker;
    std::vector<std::thread> threads;
    std::atomic<bool> start_flag{false};

    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker_thread, std::ref(tracker), host, port,
                           request, requests_per_thread, std::ref(start_flag), keep_alive);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto benchmark_start = clock_type::now();
    start_flag.store(true, std::memory_order_release);

    for (auto& t : threads) {
        t.join();
    }

    auto benchmark_end = clock_type::now();
    auto total_duration = std::chrono::duration_cast<duration_ns>(benchmark_end - benchmark_start);

    tracker.print_stats();
    tracker.print_throughput(total_duration);

    return 0;
}
