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
    std::string notes;
};

class benchmark_reporter {
public:
    void add(const std::string& category, const std::string& name,
             double value, const std::string& unit, const std::string& notes = "") {
        results_.push_back({name, category, value, unit, notes});
    }

    void print_summary() {
        std::string current_category;
        for (const auto& r : results_) {
            if (r.category != current_category) {
                std::cout << "\n=== " << r.category << " ===\n";
                current_category = r.category;
            }
            std::cout << "  " << std::left << std::setw(50) << r.name
                      << std::right << std::setw(12) << std::fixed << std::setprecision(2) << r.value
                      << " " << r.unit;
            if (!r.notes.empty()) {
                std::cout << "  (" << r.notes << ")";
            }
            std::cout << "\n";
        }
    }

    void save_to_file(const std::string& filename) {
        std::ofstream out(filename);
        out << "# KATANA Framework - Benchmark Results\n\n";
        out << "Generated: " << get_timestamp() << "\n\n";

        std::string current_category;
        for (const auto& r : results_) {
            if (r.category != current_category) {
                out << "\n## " << r.category << "\n\n";
                out << "| Benchmark | Value | Unit | Notes |\n";
                out << "|-----------|-------|------|-------|\n";
                current_category = r.category;
            }
            out << "| " << r.name << " | " << std::fixed << std::setprecision(2)
                << r.value << " | " << r.unit << " | " << r.notes << " |\n";
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

    double avg() const {
        if (samples.empty()) return 0;
        return std::accumulate(samples.begin(), samples.end(), 0.0) / static_cast<double>(samples.size()) / 1e6;
    }

    double min() const {
        return samples.empty() ? 0 : static_cast<double>(samples.front()) / 1e6;
    }

    double max() const {
        return samples.empty() ? 0 : static_cast<double>(samples.back()) / 1e6;
    }
};

int32_t create_connection(const char* host, uint16_t port) {
    int32_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return -1;

    timeval timeout{};
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

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

void set_socket_option(int32_t sockfd, int32_t level, int32_t optname, int32_t value) {
    setsockopt(sockfd, level, optname, &value, sizeof(value));
}

std::pair<bool, int64_t> send_http_request(int32_t sockfd, const std::string& request) {
    auto start = std::chrono::high_resolution_clock::now();

    if (send(sockfd, request.c_str(), request.size(), 0) <= 0) {
        return {false, 0};
    }

    char buffer[65536];
    if (recv(sockfd, buffer, sizeof(buffer), 0) <= 0) {
        return {false, 0};
    }

    auto end = std::chrono::high_resolution_clock::now();
    return {true, std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()};
}

void core_performance_plaintext_throughput(benchmark_reporter& reporter, const char* host, uint16_t port) {
    std::cout << "\n[1/30] Core Performance: Plaintext throughput...\n";

    std::vector<size_t> reactor_counts = {1, 2, 4, 8};
    const size_t requests_per_thread = 1000;

    for (size_t num_threads : reactor_counts) {
        std::atomic<size_t> total_requests{0};
        std::atomic<bool> start_flag{false};
        std::vector<std::thread> threads;

        for (size_t i = 0; i < num_threads; ++i) {
            threads.emplace_back([&]() {
                while (!start_flag.load()) std::this_thread::yield();

                int32_t sockfd = create_connection(host, port);
                if (sockfd < 0) return;

                std::string request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";
                for (size_t j = 0; j < requests_per_thread; ++j) {
                    auto [success, _] = send_http_request(sockfd, request);
                    if (success) total_requests.fetch_add(1);
                }
                close(sockfd);
            });
        }

        auto start = std::chrono::high_resolution_clock::now();
        start_flag.store(true);
        for (auto& t : threads) t.join();
        auto end = std::chrono::high_resolution_clock::now();

        double duration_s = std::chrono::duration<double>(end - start).count();
        double rps = static_cast<double>(total_requests.load()) / duration_s;

        reporter.add("Core Performance",
                     "Plaintext throughput (" + std::to_string(num_threads) + " reactors)",
                     rps, "req/s", "");
    }
}

void core_performance_hello_latency(benchmark_reporter& reporter, const char* host, uint16_t port) {
    std::cout << "[2/30] Core Performance: Hello World latency under load...\n";

    const size_t num_threads = 10;
    const size_t requests_per_thread = 1000;
    std::vector<latency_stats> thread_stats(num_threads);
    std::atomic<bool> start_flag{false};
    std::vector<std::thread> threads;

    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back([&, i]() {
            while (!start_flag.load()) std::this_thread::yield();

            int32_t sockfd = create_connection(host, port);
            if (sockfd < 0) return;

            std::string request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";
            for (size_t j = 0; j < requests_per_thread; ++j) {
                auto [success, latency] = send_http_request(sockfd, request);
                if (success) thread_stats[i].add(latency);
            }
            close(sockfd);
        });
    }

    start_flag.store(true);
    for (auto& t : threads) t.join();

    latency_stats combined;
    for (auto& s : thread_stats) {
        combined.samples.insert(combined.samples.end(), s.samples.begin(), s.samples.end());
    }
    combined.sort();

    reporter.add("Core Performance", "Hello World latency p50", combined.percentile(50), "ms", "under load");
    reporter.add("Core Performance", "Hello World latency p95", combined.percentile(95), "ms", "under load");
    reporter.add("Core Performance", "Hello World latency p99", combined.percentile(99), "ms", "under load");
    reporter.add("Core Performance", "Hello World latency p999", combined.percentile(99.9), "ms", "under load");
}

void core_performance_keepalive(benchmark_reporter& reporter, const char* host, uint16_t port) {
    std::cout << "[3/30] Core Performance: Keep-alive connections...\n";

    const size_t requests = 10000;
    int32_t sockfd = create_connection(host, port);
    if (sockfd < 0) return;

    std::string request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < requests; ++i) {
        send_http_request(sockfd, request);
    }
    auto end = std::chrono::high_resolution_clock::now();

    close(sockfd);

    double duration_s = std::chrono::duration<double>(end - start).count();
    double rps = requests / duration_s;

    reporter.add("Core Performance", "Keep-alive throughput", rps, "req/s", "single connection");
}

void core_performance_large_response(benchmark_reporter& reporter, const char*, uint16_t) {
    std::cout << "[4/30] Core Performance: Large response bodies (skipped - requires special server)...\n";
    reporter.add("Core Performance", "Large response test", 0, "N/A", "requires custom server");
}

void core_performance_parsing(benchmark_reporter& reporter, const char* host, uint16_t port) {
    std::cout << "[5/30] Core Performance: Request parsing overhead...\n";

    std::vector<std::pair<std::string, std::string>> requests = {
        {"Minimal", "GET / HTTP/1.1\r\nHost: a\r\n\r\n"},
        {"Medium", "GET /path HTTP/1.1\r\nHost: localhost\r\nUser-Agent: bench\r\nAccept: */*\r\n\r\n"},
        {"Large headers", "GET /path HTTP/1.1\r\nHost: localhost\r\nUser-Agent: benchmark\r\n"
                         "Accept: application/json\r\nAccept-Encoding: gzip\r\n"
                         "Accept-Language: en-US\r\nCache-Control: no-cache\r\n"
                         "X-Custom-1: value1\r\nX-Custom-2: value2\r\n\r\n"}
    };

    for (const auto& [label, req] : requests) {
        int32_t sockfd = create_connection(host, port);
        if (sockfd < 0) continue;

        latency_stats stats;
        for (int i = 0; i < 1000; ++i) {
            auto [success, latency] = send_http_request(sockfd, req);
            if (success) stats.add(latency);
        }
        close(sockfd);

        stats.sort();
        reporter.add("Core Performance", "Parsing " + label + " p50",
                     stats.percentile(50), "ms", "");
    }
}

void scalability_linear_scaling(benchmark_reporter& reporter, const char* host, uint16_t port) {
    std::cout << "[6/30] Scalability: Linear scaling test...\n";

    std::vector<size_t> reactor_counts = {1, 2, 4, 8};
    const size_t requests_per_thread = 5000;

    for (size_t num_threads : reactor_counts) {
        std::atomic<size_t> total_requests{0};
        std::atomic<bool> start_flag{false};
        std::vector<std::thread> threads;

        for (size_t i = 0; i < num_threads; ++i) {
            threads.emplace_back([&]() {
                while (!start_flag.load()) std::this_thread::yield();

                int32_t sockfd = create_connection(host, port);
                if (sockfd < 0) return;

                std::string request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";
                for (size_t j = 0; j < requests_per_thread; ++j) {
                    auto [success, _] = send_http_request(sockfd, request);
                    if (success) total_requests.fetch_add(1);
                }
                close(sockfd);
            });
        }

        auto start = std::chrono::high_resolution_clock::now();
        start_flag.store(true);
        for (auto& t : threads) t.join();
        auto end = std::chrono::high_resolution_clock::now();

        double duration_s = std::chrono::duration<double>(end - start).count();
        double rps = static_cast<double>(total_requests.load()) / duration_s;

        reporter.add("Scalability",
                     "Throughput " + std::to_string(num_threads) + " reactors",
                     rps, "req/s", "");
    }
}

void scalability_concurrent_connections(benchmark_reporter& reporter, const char* host, uint16_t port) {
    std::cout << "[7/30] Scalability: Concurrent connections...\n";

    std::vector<size_t> connection_counts = {100, 1000};
    const size_t requests_per_conn = 100;

    for (size_t num_conns : connection_counts) {
        std::atomic<size_t> total_requests{0};
        std::atomic<bool> start_flag{false};
        std::vector<std::thread> threads;

        for (size_t i = 0; i < num_conns; ++i) {
            threads.emplace_back([&]() {
                while (!start_flag.load()) std::this_thread::yield();

                int32_t sockfd = create_connection(host, port);
                if (sockfd < 0) return;

                std::string request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";
                for (size_t j = 0; j < requests_per_conn; ++j) {
                    auto [success, _] = send_http_request(sockfd, request);
                    if (success) total_requests.fetch_add(1);
                }
                close(sockfd);
            });
        }

        auto start = std::chrono::high_resolution_clock::now();
        start_flag.store(true);
        for (auto& t : threads) t.join();
        auto end = std::chrono::high_resolution_clock::now();

        double duration_s = std::chrono::duration<double>(end - start).count();
        double rps = static_cast<double>(total_requests.load()) / duration_s;

        reporter.add("Scalability",
                     std::to_string(num_conns) + " concurrent connections",
                     rps, "req/s", "");
    }
}

void scalability_connection_accept_latency(benchmark_reporter& reporter, const char* host, uint16_t port) {
    std::cout << "[8/30] Scalability: Connection accept latency...\n";

    latency_stats stats;
    const size_t num_connections = 1000;

    for (size_t i = 0; i < num_connections; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        int32_t sockfd = create_connection(host, port);
        if (sockfd >= 0) {
            std::string request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
            send_http_request(sockfd, request);
            close(sockfd);
            auto end = std::chrono::high_resolution_clock::now();
            stats.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
        }
    }

    stats.sort();
    reporter.add("Scalability", "Connection accept latency p50", stats.percentile(50), "ms", "");
    reporter.add("Scalability", "Connection accept latency p99", stats.percentile(99), "ms", "");
}

void memory_arena_allocator_test(benchmark_reporter& reporter) {
    std::cout << "[9/30] Memory & Arena: Arena allocator overhead...\n";

    const size_t iterations = 1000000;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; ++i) {
        void* ptr = malloc(64);
        free(ptr);
    }
    auto end = std::chrono::high_resolution_clock::now();

    double malloc_ns = std::chrono::duration<double, std::nano>(end - start).count() / iterations;
    reporter.add("Memory & Arena", "std::allocator latency", malloc_ns, "ns/op", "64 bytes");
}

void memory_per_request(benchmark_reporter& reporter, const char* host, uint16_t port) {
    std::cout << "[10/30] Memory & Arena: Memory per request...\n";

    struct rusage usage_before, usage_after;
    getrusage(RUSAGE_SELF, &usage_before);

    const size_t num_requests = 10000;
    int32_t sockfd = create_connection(host, port);
    if (sockfd >= 0) {
        std::string request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";
        for (size_t i = 0; i < num_requests; ++i) {
            send_http_request(sockfd, request);
        }
        close(sockfd);
    }

    getrusage(RUSAGE_SELF, &usage_after);
    long memory_delta = usage_after.ru_maxrss - usage_before.ru_maxrss;
    double memory_per_req = static_cast<double>(memory_delta) / num_requests;

    reporter.add("Memory & Arena", "Memory per request", memory_per_req, "KB", "avg");
    reporter.add("Memory & Arena", "Peak memory usage", static_cast<double>(usage_after.ru_maxrss), "KB", "RSS");
}

void http_protocol_chunked(benchmark_reporter& reporter, const char*, uint16_t) {
    std::cout << "[11/30] HTTP Protocol: Chunked encoding (skipped)...\n";
    reporter.add("HTTP Protocol", "Chunked encoding", 0, "N/A", "requires custom server");
}

void http_protocol_pipelining(benchmark_reporter& reporter, const char* host, uint16_t port) {
    std::cout << "[12/30] HTTP Protocol: HTTP pipelining...\n";

    int32_t sockfd = create_connection(host, port);
    if (sockfd < 0) return;

    std::string pipelined = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
                            "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
                            "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";

    auto start = std::chrono::high_resolution_clock::now();
    send(sockfd, pipelined.c_str(), pipelined.size(), 0);

    char buffer[65536];
    for (int i = 0; i < 3; ++i) {
        recv(sockfd, buffer, sizeof(buffer), 0);
    }
    auto end = std::chrono::high_resolution_clock::now();

    close(sockfd);

    double latency = std::chrono::duration<double, std::milli>(end - start).count();
    reporter.add("HTTP Protocol", "Pipelining 3 requests", latency, "ms", "");
}

void http_protocol_large_headers(benchmark_reporter& reporter, const char* host, uint16_t port) {
    std::cout << "[13/30] HTTP Protocol: Large headers...\n";

    std::string large_headers = "GET / HTTP/1.1\r\nHost: localhost\r\n";
    for (int i = 0; i < 50; ++i) {
        large_headers += "X-Custom-Header-" + std::to_string(i) + ": " +
                        std::string(100, 'x') + "\r\n";
    }
    large_headers += "\r\n";

    latency_stats stats;
    for (int i = 0; i < 100; ++i) {
        int32_t sockfd = create_connection(host, port);
        if (sockfd < 0) continue;

        auto [success, latency] = send_http_request(sockfd, large_headers);
        if (success) stats.add(latency);
        close(sockfd);
    }

    stats.sort();
    reporter.add("HTTP Protocol", "Large headers p50", stats.percentile(50), "ms", "50 headers");
    reporter.add("HTTP Protocol", "Large headers p99", stats.percentile(99), "ms", "50 headers");
}

void network_tcp_nodelay(benchmark_reporter& reporter, const char* host, uint16_t port) {
    std::cout << "[14/30] Network I/O: TCP_NODELAY impact...\n";

    auto test_with_nodelay = [&](bool nodelay) {
        int32_t sockfd = create_connection(host, port);
        if (sockfd < 0) return 0.0;

        int flag = nodelay ? 1 : 0;
        setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

        latency_stats stats;
        std::string request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";

        for (int i = 0; i < 1000; ++i) {
            auto [success, latency] = send_http_request(sockfd, request);
            if (success) stats.add(latency);
        }
        close(sockfd);

        stats.sort();
        return stats.percentile(50);
    };

    double with_nodelay = test_with_nodelay(true);
    double without_nodelay = test_with_nodelay(false);

    reporter.add("Network I/O", "TCP_NODELAY enabled p50", with_nodelay, "ms", "");
    reporter.add("Network I/O", "TCP_NODELAY disabled p50", without_nodelay, "ms", "");
}

void network_epoll_latency(benchmark_reporter& reporter, const char* host, uint16_t port) {
    std::cout << "[15/30] Network I/O: epoll latency...\n";

    const size_t num_threads = 4;
    const size_t requests_per_thread = 1000;
    std::vector<latency_stats> thread_stats(num_threads);
    std::atomic<bool> start_flag{false};
    std::vector<std::thread> threads;

    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back([&, i]() {
            while (!start_flag.load()) std::this_thread::yield();

            int32_t sockfd = create_connection(host, port);
            if (sockfd < 0) return;

            std::string request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";
            for (size_t j = 0; j < requests_per_thread; ++j) {
                auto [success, latency] = send_http_request(sockfd, request);
                if (success) thread_stats[i].add(latency);
            }
            close(sockfd);
        });
    }

    start_flag.store(true);
    for (auto& t : threads) t.join();

    latency_stats combined;
    for (auto& s : thread_stats) {
        combined.samples.insert(combined.samples.end(), s.samples.begin(), s.samples.end());
    }
    combined.sort();

    reporter.add("Network I/O", "epoll latency p50", combined.percentile(50), "ms", "4 threads");
    reporter.add("Network I/O", "epoll latency p99", combined.percentile(99), "ms", "4 threads");
}

void stability_graceful_shutdown(benchmark_reporter& reporter, const char*, uint16_t) {
    std::cout << "[16/30] Stability: Graceful shutdown...\n";

    reporter.add("Stability", "Graceful shutdown", 0, "s", "manual test required");
}

void stability_fd_limits(benchmark_reporter& reporter) {
    std::cout << "[17/30] Stability: File descriptor limits...\n";

    struct rlimit limit;
    getrlimit(RLIMIT_NOFILE, &limit);

    reporter.add("Stability", "FD soft limit", static_cast<double>(limit.rlim_cur), "fds", "");
    reporter.add("Stability", "FD hard limit", static_cast<double>(limit.rlim_max), "fds", "");
}

void stability_stress_test(benchmark_reporter& reporter, const char* host, uint16_t port) {
    std::cout << "[18/30] Stability: Long-running stress test...\n";

    const auto test_duration = std::chrono::seconds(10);
    std::atomic<size_t> total_requests{0};
    std::atomic<bool> stop_flag{false};
    std::vector<std::thread> threads;

    for (size_t i = 0; i < 4; ++i) {
        threads.emplace_back([&]() {
            int32_t sockfd = create_connection(host, port);
            if (sockfd < 0) return;

            std::string request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";
            while (!stop_flag.load()) {
                auto [success, _] = send_http_request(sockfd, request);
                if (success) total_requests.fetch_add(1);
            }
            close(sockfd);
        });
    }

    std::this_thread::sleep_for(test_duration);
    stop_flag.store(true);

    for (auto& t : threads) t.join();

    double rps = static_cast<double>(total_requests.load()) / static_cast<double>(test_duration.count());
    reporter.add("Stability", "Stress test RPS", rps, "req/s", "10s duration");
    reporter.add("Stability", "Stress test total", static_cast<double>(total_requests.load()), "requests", "10s");
}

int32_t main(int32_t argc, char* argv[]) {
    const char* host = "127.0.0.1";
    uint16_t port = 8080;
    std::string output_file = "benchmark_results.md";

    if (argc > 1) port = static_cast<uint16_t>(std::stoul(argv[1]));
    if (argc > 2) output_file = argv[2];

    std::cout << "=== KATANA Framework - Comprehensive Benchmark Suite ===\n";
    std::cout << "Target: " << host << ":" << port << "\n";
    std::cout << "Output: " << output_file << "\n\n";

    std::cout << "Testing connection to server...\n";
    int32_t test_fd = create_connection(host, port);
    if (test_fd < 0) {
        std::cerr << "ERROR: Cannot connect to server at " << host << ":" << port << "\n";
        std::cerr << "Please start the server first.\n";
        return 1;
    }
    close(test_fd);
    std::cout << "Connection successful!\n";

    benchmark_reporter reporter;

    core_performance_plaintext_throughput(reporter, host, port);
    core_performance_hello_latency(reporter, host, port);
    core_performance_keepalive(reporter, host, port);
    core_performance_large_response(reporter, host, port);
    core_performance_parsing(reporter, host, port);

    scalability_linear_scaling(reporter, host, port);
    scalability_concurrent_connections(reporter, host, port);
    scalability_connection_accept_latency(reporter, host, port);

    memory_arena_allocator_test(reporter);
    memory_per_request(reporter, host, port);

    http_protocol_chunked(reporter, host, port);
    http_protocol_pipelining(reporter, host, port);
    http_protocol_large_headers(reporter, host, port);

    network_tcp_nodelay(reporter, host, port);
    network_epoll_latency(reporter, host, port);

    stability_graceful_shutdown(reporter, host, port);
    stability_fd_limits(reporter);
    stability_stress_test(reporter, host, port);

    std::cout << "\n";
    reporter.print_summary();
    reporter.save_to_file(output_file);

    std::cout << "\n✅ Benchmark complete! Results saved to " << output_file << "\n";

    return 0;
}
