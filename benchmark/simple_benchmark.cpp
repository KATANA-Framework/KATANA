#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <thread>
#include <atomic>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <numeric>
#include <random>
#include <sys/socket.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

struct benchmark_result {
    std::string name;
    std::string category;
    double value;
    std::string unit;
};

class benchmark_reporter {
public:
    void add(const std::string& category, const std::string& name,
             double value, const std::string& unit) {
        results_.push_back({name, category, value, unit});
    }

    void print_summary() {
        std::string current_category;
        for (const auto& r : results_) {
            if (r.category != current_category) {
                std::cout << "\n=== " << r.category << " ===\n";
                current_category = r.category;
            }
            std::cout << "  " << std::left << std::setw(40) << r.name
                      << std::right << std::setw(10) << std::fixed << std::setprecision(2) << r.value
                      << " " << r.unit << "\n";
        }
    }

    void save_to_file(const std::string& filename) {
        std::ofstream out(filename);
        out << "# KATANA Framework - Benchmark Results\n\n";
        out << "Generated: " << get_timestamp() << "\n\n";
        out << "**Note**: Benchmarks test the reactor-per-core architecture with arena allocators,\n";
        out << "vectored I/O, and edge-triggered epoll for optimal performance.\n\n";

        std::string current_category;
        for (const auto& r : results_) {
            if (r.category != current_category) {
                out << "\n## " << r.category << "\n\n";
                out << "| Benchmark | Value | Unit |\n";
                out << "|-----------|-------|------|\n";
                current_category = r.category;
            }
            out << "| " << r.name << " | " << std::fixed << std::setprecision(2)
                << r.value << " | " << r.unit << " |\n";
        }
    }

private:
    std::vector<benchmark_result> results_;

    std::string get_timestamp() {
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};

struct latency_stats {
    std::vector<int64_t> samples;

    void add(int64_t ns) {
        samples.push_back(ns);
    }

    void sort() {
        std::sort(samples.begin(), samples.end());
    }

    double percentile(double p) const {
        if (samples.empty()) return 0;
        size_t idx = static_cast<size_t>(static_cast<double>(samples.size()) * p / 100.0);
        if (idx >= samples.size()) idx = samples.size() - 1;
        return static_cast<double>(samples[idx]) / 1e6;
    }

    double min() const {
        return samples.empty() ? 0 : static_cast<double>(samples.front()) / 1e6;
    }

    double max() const {
        return samples.empty() ? 0 : static_cast<double>(samples.back()) / 1e6;
    }

    double avg() const {
        if (samples.empty()) return 0;
        return std::accumulate(samples.begin(), samples.end(), 0.0) / static_cast<double>(samples.size()) / 1e6;
    }
};

int32_t create_connection(const char* host, uint16_t port) {
    int32_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return -1;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &addr.sin_addr);

    if (connect(sockfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        close(sockfd);
        return -1;
    }
    return sockfd;
}

std::pair<bool, int64_t> send_http_request(int32_t sockfd) {
    const char* request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";

    auto start = std::chrono::high_resolution_clock::now();

    if (send(sockfd, request, strlen(request), 0) <= 0) {
        return {false, 0};
    }

    char buffer[4096];
    if (recv(sockfd, buffer, sizeof(buffer), 0) <= 0) {
        return {false, 0};
    }

    auto end = std::chrono::high_resolution_clock::now();
    return {true, std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()};
}

void test_throughput(benchmark_reporter& reporter, const char* host, uint16_t port,
                     size_t num_threads, size_t requests_per_thread) {
    std::atomic<size_t> total_requests{0};
    std::atomic<bool> start_flag{false};
    std::vector<std::thread> threads;

    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back([&]() {
            while (!start_flag.load(std::memory_order_relaxed)) std::this_thread::yield();

            int32_t sockfd = create_connection(host, port);
            if (sockfd < 0) return;

            for (size_t j = 0; j < requests_per_thread; ++j) {
                auto [success, _] = send_http_request(sockfd);
                if (success) total_requests.fetch_add(1, std::memory_order_relaxed);
            }
            close(sockfd);
        });
    }

    auto start = std::chrono::high_resolution_clock::now();
    start_flag.store(true, std::memory_order_relaxed);
    for (auto& t : threads) t.join();
    auto end = std::chrono::high_resolution_clock::now();

    double duration_s = std::chrono::duration<double>(end - start).count();
    double rps = static_cast<double>(total_requests.load()) / duration_s;

    reporter.add("Scalability",
                 "Throughput with " + std::to_string(num_threads) + " threads",
                 rps, "req/s");
}

void test_latency(benchmark_reporter& reporter, const char* host, uint16_t port) {
    const size_t num_threads = 10;
    const size_t requests_per_thread = 1000;
    std::vector<latency_stats> thread_stats(num_threads);
    std::atomic<bool> start_flag{false};
    std::vector<std::thread> threads;

    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back([&, i]() {
            while (!start_flag.load(std::memory_order_relaxed)) std::this_thread::yield();

            int32_t sockfd = create_connection(host, port);
            if (sockfd < 0) return;

            for (size_t j = 0; j < requests_per_thread; ++j) {
                auto [success, latency] = send_http_request(sockfd);
                if (success) thread_stats[i].add(latency);
            }
            close(sockfd);
        });
    }

    start_flag.store(true, std::memory_order_relaxed);
    for (auto& t : threads) t.join();

    latency_stats combined;
    for (auto& s : thread_stats) {
        combined.samples.insert(combined.samples.end(), s.samples.begin(), s.samples.end());
    }
    combined.sort();

    reporter.add("Core Performance", "Latency p50", combined.percentile(50), "ms");
    reporter.add("Core Performance", "Latency p90", combined.percentile(90), "ms");
    reporter.add("Core Performance", "Latency p95", combined.percentile(95), "ms");
    reporter.add("Core Performance", "Latency p99", combined.percentile(99), "ms");
    reporter.add("Core Performance", "Latency p999", combined.percentile(99.9), "ms");
}

void test_keepalive(benchmark_reporter& reporter, const char* host, uint16_t port) {
    const size_t requests = 10000;
    int32_t sockfd = create_connection(host, port);
    if (sockfd < 0) return;

    auto start = std::chrono::high_resolution_clock::now();
    size_t successful = 0;
    for (size_t i = 0; i < requests; ++i) {
        auto [success, _] = send_http_request(sockfd);
        if (success) ++successful;
    }
    auto end = std::chrono::high_resolution_clock::now();
    close(sockfd);

    double duration_s = std::chrono::duration<double>(end - start).count();
    double rps = static_cast<double>(successful) / duration_s;

    reporter.add("Core Performance", "Keep-alive throughput", rps, "req/s");
}

void test_fd_limits(benchmark_reporter& reporter) {
    struct rlimit limit;
    getrlimit(RLIMIT_NOFILE, &limit);

    reporter.add("System Configuration", "FD soft limit", static_cast<double>(limit.rlim_cur), "fds");
    reporter.add("System Configuration", "FD hard limit", static_cast<double>(limit.rlim_max), "fds");
}

void test_concurrent_connections(benchmark_reporter& reporter, const char* host, uint16_t port) {
    std::vector<size_t> connection_counts = {100, 500, 1000};
    const size_t requests_per_conn = 50;

    for (size_t num_conns : connection_counts) {
        std::atomic<size_t> total_requests{0};
        std::atomic<bool> start_flag{false};
        std::vector<std::thread> threads;

        for (size_t i = 0; i < num_conns; ++i) {
            threads.emplace_back([&]() {
                while (!start_flag.load(std::memory_order_relaxed)) std::this_thread::yield();

                int32_t sockfd = create_connection(host, port);
                if (sockfd < 0) return;

                for (size_t j = 0; j < requests_per_conn; ++j) {
                    auto [success, _] = send_http_request(sockfd);
                    if (success) total_requests.fetch_add(1, std::memory_order_relaxed);
                }
                close(sockfd);
            });
        }

        auto start = std::chrono::high_resolution_clock::now();
        start_flag.store(true, std::memory_order_relaxed);
        for (auto& t : threads) t.join();
        auto end = std::chrono::high_resolution_clock::now();

        double duration_s = std::chrono::duration<double>(end - start).count();
        double rps = static_cast<double>(total_requests.load()) / duration_s;

        reporter.add("Scalability",
                     std::to_string(num_conns) + " concurrent connections",
                     rps, "req/s");
    }
}

void test_parsing_overhead(benchmark_reporter& reporter, const char* host, uint16_t port) {
    std::vector<std::pair<std::string, std::string>> test_cases = {
        {"Minimal request", "GET / HTTP/1.1\r\nHost: a\r\n\r\n"},
        {"Medium request", "GET /api/users HTTP/1.1\r\nHost: localhost\r\n"
                          "User-Agent: bench/1.0\r\nAccept: */*\r\n\r\n"},
        {"Large headers", "GET /api/data HTTP/1.1\r\nHost: localhost\r\n"
                         "User-Agent: benchmark\r\nAccept: application/json\r\n"
                         "Accept-Encoding: gzip, deflate\r\n"
                         "Accept-Language: en-US,en;q=0.9\r\n"
                         "Cache-Control: no-cache\r\nPragma: no-cache\r\n"
                         "X-Custom-1: value1\r\nX-Custom-2: value2\r\n"
                         "X-Custom-3: value3\r\n\r\n"}
    };

    for (const auto& [label, request] : test_cases) {
        int32_t sockfd = create_connection(host, port);
        if (sockfd < 0) continue;

        latency_stats stats;
        for (size_t i = 0; i < 1000; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            if (send(sockfd, request.c_str(), request.size(), 0) > 0) {
                char buffer[4096];
                if (recv(sockfd, buffer, sizeof(buffer), 0) > 0) {
                    auto end = std::chrono::high_resolution_clock::now();
                    stats.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
                }
            }
        }
        close(sockfd);

        stats.sort();
        reporter.add("HTTP Parsing", label + " p50", stats.percentile(50), "ms");
        reporter.add("HTTP Parsing", label + " p99", stats.percentile(99), "ms");
    }
}

int32_t main(int32_t argc, char* argv[]) {
    const char* host = "127.0.0.1";
    uint16_t port = 8080;
    std::string output_file = "BENCHMARK_RESULTS.md";

    if (argc > 1) port = static_cast<uint16_t>(std::stoul(argv[1]));
    if (argc > 2) output_file = argv[2];

    std::cout << "=== KATANA Framework - Benchmark Suite ===\n";
    std::cout << "Target: " << host << ":" << port << "\n";
    std::cout << "Output: " << output_file << "\n";
    std::cout << "Hardware concurrency: " << std::thread::hardware_concurrency() << " cores\n\n";

    int32_t test_fd = create_connection(host, port);
    if (test_fd < 0) {
        std::cerr << "ERROR: Cannot connect to server at " << host << ":" << port << "\n";
        std::cerr << "Please start hello_world_server first\n";
        return 1;
    }
    close(test_fd);
    std::cout << "Connection successful!\n\n";

    benchmark_reporter reporter;

    std::cout << "[1/10] Testing latency under load (10 threads, 10k requests)...\n";
    test_latency(reporter, host, port);

    std::cout << "[2/10] Testing keep-alive throughput (10k requests)...\n";
    test_keepalive(reporter, host, port);

    std::cout << "[3/10] Testing HTTP parsing overhead...\n";
    test_parsing_overhead(reporter, host, port);

    std::cout << "[4/10] Testing throughput with 1 thread...\n";
    test_throughput(reporter, host, port, 1, 2000);

    std::cout << "[5/10] Testing throughput with 2 threads...\n";
    test_throughput(reporter, host, port, 2, 2000);

    std::cout << "[6/10] Testing throughput with 4 threads...\n";
    test_throughput(reporter, host, port, 4, 2000);

    std::cout << "[7/10] Testing throughput with 8 threads...\n";
    test_throughput(reporter, host, port, 8, 2000);

    std::cout << "[8/10] Testing concurrent connections...\n";
    test_concurrent_connections(reporter, host, port);

    std::cout << "[9/10] Checking system limits...\n";
    test_fd_limits(reporter);

    std::cout << "[10/10] Running stress test (5s duration)...\n";
    std::atomic<size_t> stress_requests{0};
    std::atomic<bool> stress_stop{false};
    std::vector<std::thread> stress_threads;

    for (size_t i = 0; i < 4; ++i) {
        stress_threads.emplace_back([&]() {
            int32_t sockfd = create_connection(host, port);
            if (sockfd < 0) return;

            while (!stress_stop.load(std::memory_order_relaxed)) {
                auto [success, _] = send_http_request(sockfd);
                if (success) stress_requests.fetch_add(1, std::memory_order_relaxed);
            }
            close(sockfd);
        });
    }

    auto stress_start = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    stress_stop.store(true, std::memory_order_relaxed);

    for (auto& t : stress_threads) t.join();
    auto stress_duration = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - stress_start).count();

    double stress_rps = static_cast<double>(stress_requests.load()) / stress_duration;
    reporter.add("Stability", "Sustained throughput (5s)", stress_rps, "req/s");
    reporter.add("Stability", "Total requests (5s)", static_cast<double>(stress_requests.load()), "requests");

    std::cout << "\n";
    reporter.print_summary();
    reporter.save_to_file(output_file);

    std::cout << "\n✅ Benchmark complete! Results saved to " << output_file << "\n";

    return 0;
}
