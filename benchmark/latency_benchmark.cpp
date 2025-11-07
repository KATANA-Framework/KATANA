#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <thread>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

struct latency_sample {
    std::chrono::nanoseconds duration;
    bool success;
};

class latency_tracker {
public:
    void record(std::chrono::nanoseconds latency, bool success) {
        std::lock_guard<std::mutex> lock(mutex_);
        samples_.push_back({latency, success});
    }

    void print_stats() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (samples_.empty()) {
            std::cout << "No samples recorded\n";
            return;
        }

        std::vector<int64_t> latencies;
        size_t successful = 0;

        for (const auto& sample : samples_) {
            if (sample.success) {
                latencies.push_back(sample.duration.count());
                ++successful;
            }
        }

        if (latencies.empty()) {
            std::cout << "No successful requests\n";
            return;
        }

        std::sort(latencies.begin(), latencies.end());

        auto p50 = latencies[latencies.size() * 50 / 100];
        auto p90 = latencies[latencies.size() * 90 / 100];
        auto p95 = latencies[latencies.size() * 95 / 100];
        auto p99 = latencies[latencies.size() * 99 / 100];
        auto p999 = latencies[latencies.size() * 999 / 1000];

        int64_t sum = 0;
        for (auto l : latencies) {
            sum += l;
        }
        auto avg = sum / static_cast<int64_t>(latencies.size());

        std::cout << "\n=== Latency Statistics ===\n";
        std::cout << "Total requests: " << samples_.size() << "\n";
        std::cout << "Successful: " << successful << "\n";
        std::cout << "Failed: " << (samples_.size() - successful) << "\n";
        std::cout << "\nLatency (ms):\n";
        std::cout << "  Min:    " << (static_cast<double>(latencies.front()) / 1000000.0) << " ms\n";
        std::cout << "  Avg:    " << (static_cast<double>(avg) / 1000000.0) << " ms\n";
        std::cout << "  p50:    " << (static_cast<double>(p50) / 1000000.0) << " ms\n";
        std::cout << "  p90:    " << (static_cast<double>(p90) / 1000000.0) << " ms\n";
        std::cout << "  p95:    " << (static_cast<double>(p95) / 1000000.0) << " ms\n";
        std::cout << "  p99:    " << (static_cast<double>(p99) / 1000000.0) << " ms\n";
        std::cout << "  p99.9:  " << (static_cast<double>(p999) / 1000000.0) << " ms\n";
        std::cout << "  Max:    " << (static_cast<double>(latencies.back()) / 1000000.0) << " ms\n";

        // Stage 1 requirement: p99 < 1.5-2.0 ms
        if (static_cast<double>(p99) / 1000000.0 < 2.0) {
            std::cout << "\n✅ PASS: p99 latency < 2.0 ms (Stage 1 requirement)\n";
        } else {
            std::cout << "\n❌ FAIL: p99 latency >= 2.0 ms (Stage 1 requirement)\n";
        }
    }

private:
    std::vector<latency_sample> samples_;
    std::mutex mutex_;
};

int32_t create_connection(const char* host, uint16_t port) {
    int32_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        return -1;
    }

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

bool send_request(int32_t sockfd) {
    const char* request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";
    size_t total_sent = 0;
    size_t request_len = strlen(request);

    while (total_sent < request_len) {
        ssize_t sent = send(sockfd, request + total_sent, request_len - total_sent, 0);
        if (sent <= 0) {
            return false;
        }
        total_sent += sent;
    }

    char buffer[4096];
    ssize_t received = recv(sockfd, buffer, sizeof(buffer), 0);
    return received > 0;
}

void worker_thread(latency_tracker& tracker, const char* host, uint16_t port,
                  size_t requests_per_thread, std::atomic<bool>& start_flag) {
    while (!start_flag.load()) {
        std::this_thread::yield();
    }

    int32_t sockfd = create_connection(host, port);
    if (sockfd < 0) {
        std::cerr << "Failed to connect\n";
        return;
    }

    for (size_t i = 0; i < requests_per_thread; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        bool success = send_request(sockfd);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        tracker.record(duration, success);
    }

    close(sockfd);
}

int32_t main(int32_t argc, char* argv[]) {
    const char* host = "127.0.0.1";
    uint16_t port = 8080;
    size_t total_requests = 10000;
    size_t num_threads = 10;

    if (argc > 1) {
        total_requests = std::stoull(argv[1]);
    }
    if (argc > 2) {
        num_threads = std::stoull(argv[2]);
    }

    size_t requests_per_thread = total_requests / num_threads;

    std::cout << "=== HTTP Latency Benchmark ===\n";
    std::cout << "Target: " << host << ":" << port << "\n";
    std::cout << "Total requests: " << total_requests << "\n";
    std::cout << "Threads: " << num_threads << "\n";
    std::cout << "Requests per thread: " << requests_per_thread << "\n";
    std::cout << "\nStarting benchmark...\n";

    latency_tracker tracker;
    std::vector<std::thread> threads;
    std::atomic<bool> start_flag{false};

    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker_thread, std::ref(tracker), host, port,
                           requests_per_thread, std::ref(start_flag));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto benchmark_start = std::chrono::high_resolution_clock::now();
    start_flag.store(true);

    for (auto& t : threads) {
        t.join();
    }

    auto benchmark_end = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        benchmark_end - benchmark_start);

    tracker.print_stats();

    std::cout << "\nBenchmark duration: " << total_duration.count() << " ms\n";
    std::cout << "Throughput: " << (static_cast<double>(total_requests) * 1000.0 / static_cast<double>(total_duration.count()))
              << " req/s\n";

    return 0;
}
