#include <algorithm>
#include <arpa/inet.h>
#include <atomic>
#include <barrier>
#include <cctype>
#include <cerrno>
#include <charconv>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <numeric>
#include <optional>
#include <sstream>
#include <string_view>
#include <sys/resource.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

namespace {

struct benchmark_result {
    std::string name;
    std::string category;
    double value;
    std::string unit;
};

class benchmark_reporter {
public:
    void add(const std::string& category,
             const std::string& name,
             double value,
             const std::string& unit) {
        results_.push_back({name, category, value, unit});
    }

    void print_summary() {
        std::string current_category;
        for (const auto& r : results_) {
            if (r.category != current_category) {
                std::cout << "\n=== " << r.category << " ===\n";
                current_category = r.category;
            }
            std::cout << "  " << std::left << std::setw(44) << r.name << std::right << std::setw(12)
                      << std::fixed << std::setprecision(3) << r.value << " " << r.unit << "\n";
        }
    }

    void save_to_file(const std::string& filename) {
        std::ofstream out(filename);
        out << "# KATANA Framework - Benchmark Results\n\n";
        out << "Generated: " << timestamp() << "\n\n";
        out << "**Note**: Measurements use time-boxed phases with warm-ups, steady-state sampling, "
               "and full response validation.\n\n";

        std::string current_category;
        for (const auto& r : results_) {
            if (r.category != current_category) {
                out << "\n## " << r.category << "\n\n";
                out << "| Benchmark | Value | Unit |\n";
                out << "|-----------|-------|------|\n";
                current_category = r.category;
            }
            out << "| " << r.name << " | " << std::fixed << std::setprecision(3) << r.value << " | "
                << r.unit << " |\n";
        }
    }

private:
    std::vector<benchmark_result> results_;

    static std::string timestamp() {
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};

struct latency_stats {
    void add(int64_t ns) {
        samples.push_back(ns);
        sum_ns += ns;
    }

    void sort() { std::sort(samples.begin(), samples.end()); }

    [[nodiscard]] size_t count() const { return samples.size(); }

    [[nodiscard]] double percentile(double p) const {
        if (samples.empty()) {
            return 0.0;
        }
        if (samples.size() == 1) {
            return static_cast<double>(samples.front()) / 1e6;
        }
        double rank = (p / 100.0) * static_cast<double>(samples.size() - 1);
        size_t lower_index = static_cast<size_t>(std::floor(rank));
        size_t upper_index = static_cast<size_t>(std::ceil(rank));
        double weight = rank - static_cast<double>(lower_index);
        double lower = static_cast<double>(samples[lower_index]);
        double upper = static_cast<double>(samples[upper_index]);
        double interpolated = lower + (upper - lower) * weight;
        return interpolated / 1e6;
    }

    [[nodiscard]] double min() const {
        return samples.empty() ? 0.0 : static_cast<double>(samples.front()) / 1e6;
    }

    [[nodiscard]] double max() const {
        return samples.empty() ? 0.0 : static_cast<double>(samples.back()) / 1e6;
    }

    [[nodiscard]] double avg() const {
        if (samples.empty()) {
            return 0.0;
        }
        return static_cast<double>(sum_ns) / static_cast<double>(samples.size()) / 1e6;
    }

    [[nodiscard]] double iqr() const {
        if (samples.size() < 4) {
            return 0.0;
        }
        return percentile(75.0) - percentile(25.0);
    }

    std::vector<int64_t> samples;
    int64_t sum_ns = 0;
};

constexpr std::string_view default_request() {
    return "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";
}

bool starts_with_case_insensitive(std::string_view text, std::string_view prefix) {
    if (text.size() < prefix.size()) {
        return false;
    }
    for (size_t i = 0; i < prefix.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(text[i])) !=
            std::tolower(static_cast<unsigned char>(prefix[i]))) {
            return false;
        }
    }
    return true;
}

std::optional<size_t> parse_content_length(std::string_view header) {
    size_t pos = 0;
    while (pos < header.size()) {
        size_t end = header.find("\r\n", pos);
        if (end == std::string_view::npos) {
            end = header.size();
        }
        std::string_view line = header.substr(pos, end - pos);
        pos = end + 2;

        if (!starts_with_case_insensitive(line, "content-length")) {
            continue;
        }

        size_t colon = line.find(':');
        if (colon == std::string_view::npos) {
            continue;
        }

        std::string_view value = line.substr(colon + 1);
        while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front()))) {
            value.remove_prefix(1);
        }

        size_t result = 0;
        auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), result);
        if (ec == std::errc()) {
            return result;
        }
    }

    return std::nullopt;
}

class http_client {
public:
    http_client(std::string host, uint16_t port) : host_(std::move(host)), port_(port) {
        read_buffer_.resize(8192);
    }

    ~http_client() { close(); }

    bool perform_request(std::string_view request, latency_stats* stats = nullptr) {
        if (!ensure_connection()) {
            return false;
        }

        auto start = std::chrono::steady_clock::now();
        if (!send_all(request)) {
            reset();
            return false;
        }

        if (!read_response()) {
            reset();
            return false;
        }

        if (stats) {
            auto end = std::chrono::steady_clock::now();
            stats->add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
        }

        return true;
    }

    bool perform_request(latency_stats* stats = nullptr) {
        return perform_request(default_request(), stats);
    }

    void close() { reset(); }

private:
    bool ensure_connection() {
        if (sockfd_ >= 0) {
            return true;
        }

        int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
        if (fd < 0) {
            return false;
        }

        int flag = 1;
        ::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(flag));
        ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        if (::inet_pton(AF_INET, host_.c_str(), &addr.sin_addr) != 1) {
            ::close(fd);
            return false;
        }

        while (true) {
            if (::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0) {
                break;
            }
            if (errno == EINTR) {
                continue;
            }
            ::close(fd);
            return false;
        }

        sockfd_ = fd;
        return true;
    }

    bool send_all(std::string_view data) {
        const char* ptr = data.data();
        size_t remaining = data.size();

        while (remaining > 0) {
            ssize_t sent = ::send(sockfd_, ptr, remaining, MSG_NOSIGNAL);
            if (sent < 0) {
                if (errno == EINTR) {
                    continue;
                }
                return false;
            }
            if (sent == 0) {
                return false;
            }
            ptr += static_cast<size_t>(sent);
            remaining -= static_cast<size_t>(sent);
        }

        return true;
    }

    bool read_response() {
        constexpr size_t max_bytes = 1 << 20;
        size_t total = 0;
        size_t header_end = std::numeric_limits<size_t>::max();
        std::optional<size_t> body_bytes_expected;

        while (true) {
            if (total >= read_buffer_.size()) {
                if (read_buffer_.size() >= max_bytes) {
                    return false;
                }
                read_buffer_.resize(std::min(read_buffer_.size() * 2, max_bytes));
            }

            ssize_t received =
                ::recv(sockfd_, read_buffer_.data() + total, read_buffer_.size() - total, 0);
            if (received < 0) {
                if (errno == EINTR) {
                    continue;
                }
                return false;
            }
            if (received == 0) {
                return false;
            }

            total += static_cast<size_t>(received);
            std::string_view data(read_buffer_.data(), total);

            if (header_end == std::numeric_limits<size_t>::max()) {
                size_t pos = data.find("\r\n\r\n");
                if (pos != std::string_view::npos) {
                    header_end = pos + 4;
                    body_bytes_expected = parse_content_length(data.substr(0, pos));
                    if (!body_bytes_expected && total > header_end) {
                        return true;
                    }
                }
            }

            if (header_end != std::numeric_limits<size_t>::max()) {
                size_t body = total - header_end;
                if (!body_bytes_expected) {
                    if (body > 0) {
                        return true;
                    }
                } else if (body >= *body_bytes_expected) {
                    return true;
                }
            }
        }
    }

    void reset() {
        if (sockfd_ >= 0) {
            ::close(sockfd_);
            sockfd_ = -1;
        }
    }

    std::string host_;
    uint16_t port_;
    int sockfd_ = -1;
    std::vector<char> read_buffer_;
};

using steady_clock = std::chrono::steady_clock;

void test_latency(benchmark_reporter& reporter, const std::string& host, uint16_t port) {
    constexpr size_t num_threads = 8;
    const auto warmup = std::chrono::milliseconds(250);
    const auto duration = std::chrono::milliseconds(2000);

    std::vector<latency_stats> per_thread(num_threads);
    std::atomic<size_t> successful_samples{0};

    steady_clock::time_point warmup_end;
    steady_clock::time_point finish;
    std::barrier sync_point(static_cast<std::ptrdiff_t>(num_threads), [&]() {
        auto start = steady_clock::now();
        warmup_end = start + warmup;
        finish = warmup_end + duration;
    });

    std::vector<std::thread> workers;
    workers.reserve(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back([&, i]() {
            http_client client(host, port);
            sync_point.arrive_and_wait();

            auto now = steady_clock::now();
            while (now < warmup_end) {
                client.perform_request();
                now = steady_clock::now();
            }

            now = steady_clock::now();
            while (now < finish) {
                if (client.perform_request(&per_thread[i])) {
                    successful_samples.fetch_add(1, std::memory_order_relaxed);
                }
                now = steady_clock::now();
            }
        });
    }

    for (auto& t : workers) {
        t.join();
    }

    latency_stats combined;
    combined.samples.reserve(successful_samples.load(std::memory_order_relaxed));
    for (auto& s : per_thread) {
        combined.samples.insert(combined.samples.end(), s.samples.begin(), s.samples.end());
        combined.sum_ns += s.sum_ns;
    }
    combined.sort();

    reporter.add(
        "Core Performance", "Latency samples", static_cast<double>(combined.count()), "samples");
    reporter.add("Core Performance", "Latency avg", combined.avg(), "ms");
    reporter.add("Core Performance", "Latency p50", combined.percentile(50.0), "ms");
    reporter.add("Core Performance", "Latency p90", combined.percentile(90.0), "ms");
    reporter.add("Core Performance", "Latency p95", combined.percentile(95.0), "ms");
    reporter.add("Core Performance", "Latency p99", combined.percentile(99.0), "ms");
    reporter.add("Core Performance", "Latency p999", combined.percentile(99.9), "ms");
    reporter.add("Core Performance", "Latency IQR", combined.iqr(), "ms");
    reporter.add("Core Performance", "Latency max", combined.max(), "ms");
}

void test_keepalive(benchmark_reporter& reporter, const std::string& host, uint16_t port) {
    const size_t requests = 5000;
    http_client client(host, port);

    auto start = steady_clock::now();
    size_t success = 0;
    for (size_t i = 0; i < requests; ++i) {
        if (client.perform_request()) {
            ++success;
        }
    }
    auto end = steady_clock::now();

    double duration_s = std::chrono::duration<double>(end - start).count();
    double rps = static_cast<double>(success) / duration_s;

    reporter.add("Core Performance", "Keep-alive throughput", rps, "req/s");
    reporter.add(
        "Core Performance", "Keep-alive success", static_cast<double>(success), "requests");
}

void test_throughput(benchmark_reporter& reporter,
                     const std::string& host,
                     uint16_t port,
                     size_t num_threads,
                     std::chrono::milliseconds duration) {
    const auto warmup = std::chrono::milliseconds(300);

    std::atomic<size_t> total_requests{0};
    steady_clock::time_point warmup_end;
    steady_clock::time_point finish;

    std::barrier sync_point(static_cast<std::ptrdiff_t>(num_threads), [&]() {
        auto start = steady_clock::now();
        warmup_end = start + warmup;
        finish = warmup_end + duration;
    });

    std::vector<std::thread> workers;
    workers.reserve(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back([&]() {
            http_client client(host, port);
            sync_point.arrive_and_wait();

            auto now = steady_clock::now();
            while (now < warmup_end) {
                client.perform_request();
                now = steady_clock::now();
            }

            size_t local = 0;
            now = steady_clock::now();
            while (now < finish) {
                if (client.perform_request()) {
                    ++local;
                }
                now = steady_clock::now();
            }

            total_requests.fetch_add(local, std::memory_order_relaxed);
        });
    }

    for (auto& t : workers) {
        t.join();
    }

    double duration_s = std::chrono::duration<double>(duration).count();
    double rps = static_cast<double>(total_requests.load(std::memory_order_relaxed)) / duration_s;

    reporter.add(
        "Scalability", "Throughput with " + std::to_string(num_threads) + " threads", rps, "req/s");
}

void test_connection_churn(benchmark_reporter& reporter,
                           const std::string& host,
                           uint16_t port,
                           size_t num_threads,
                           std::chrono::milliseconds duration) {
    const auto warmup = std::chrono::milliseconds(300);

    std::atomic<size_t> total_requests{0};
    steady_clock::time_point warmup_end;
    steady_clock::time_point finish;

    std::barrier sync_point(static_cast<std::ptrdiff_t>(num_threads), [&]() {
        auto start = steady_clock::now();
        warmup_end = start + warmup;
        finish = warmup_end + duration;
    });

    std::vector<std::thread> workers;
    workers.reserve(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back([&]() {
            sync_point.arrive_and_wait();

            auto now = steady_clock::now();
            while (now < warmup_end) {
                http_client client(host, port);
                client.perform_request();
                now = steady_clock::now();
            }

            size_t local = 0;
            now = steady_clock::now();
            while (now < finish) {
                http_client client(host, port);
                if (client.perform_request()) {
                    ++local;
                }
                now = steady_clock::now();
            }

            total_requests.fetch_add(local, std::memory_order_relaxed);
        });
    }

    for (auto& t : workers) {
        t.join();
    }

    double duration_s = std::chrono::duration<double>(duration).count();
    double rps = static_cast<double>(total_requests.load(std::memory_order_relaxed)) / duration_s;

    reporter.add("Connection Churn",
                 "Close-after-each-request throughput (" + std::to_string(num_threads) +
                     " threads)",
                 rps,
                 "req/s");
}

void test_fd_limits(benchmark_reporter& reporter) {
    rlimit limit{};
    getrlimit(RLIMIT_NOFILE, &limit);
    reporter.add(
        "System Configuration", "FD soft limit", static_cast<double>(limit.rlim_cur), "fds");
    reporter.add(
        "System Configuration", "FD hard limit", static_cast<double>(limit.rlim_max), "fds");
}

void test_concurrent_connections(benchmark_reporter& reporter,
                                 const std::string& host,
                                 uint16_t port) {
    const std::vector<size_t> connection_counts{32, 64, 128, 256};
    const auto warmup = std::chrono::milliseconds(300);
    const auto duration = std::chrono::milliseconds(2500);

    for (size_t connections : connection_counts) {
        std::atomic<size_t> total_requests{0};
        steady_clock::time_point warmup_end;
        steady_clock::time_point finish;

        std::barrier sync_point(static_cast<std::ptrdiff_t>(connections), [&]() {
            auto start = steady_clock::now();
            warmup_end = start + warmup;
            finish = warmup_end + duration;
        });

        std::vector<std::thread> workers;
        workers.reserve(connections);

        for (size_t i = 0; i < connections; ++i) {
            workers.emplace_back([&]() {
                http_client client(host, port);
                sync_point.arrive_and_wait();

                auto now = steady_clock::now();
                while (now < warmup_end) {
                    client.perform_request();
                    now = steady_clock::now();
                }

                size_t local = 0;
                now = steady_clock::now();
                while (now < finish) {
                    if (client.perform_request()) {
                        ++local;
                    }
                    now = steady_clock::now();
                }

                total_requests.fetch_add(local, std::memory_order_relaxed);
            });
        }

        for (auto& t : workers) {
            t.join();
        }

        double duration_s = std::chrono::duration<double>(duration).count();
        double rps =
            static_cast<double>(total_requests.load(std::memory_order_relaxed)) / duration_s;

        reporter.add(
            "Scalability", std::to_string(connections) + " concurrent connections", rps, "req/s");
    }
}

void test_parsing_overhead(benchmark_reporter& reporter, const std::string& host, uint16_t port) {
    const std::vector<std::pair<std::string, std::string>> test_cases{
        {"Minimal request", "GET / HTTP/1.1\r\nHost: a\r\n\r\n"},
        {"Medium request",
         "GET /api/users HTTP/1.1\r\nHost: localhost\r\nUser-Agent: bench/1.0\r\nAccept: "
         "*/*\r\n\r\n"},
        {"Large headers",
         "GET /api/data HTTP/1.1\r\nHost: localhost\r\nUser-Agent: benchmark\r\nAccept: "
         "application/json\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: "
         "en-US,en;q=0.9\r\nCache-Control: no-cache\r\nPragma: no-cache\r\nX-Custom-1: "
         "value1\r\nX-Custom-2: value2\r\nX-Custom-3: value3\r\n\r\n"}};

    const size_t target_samples = 1500;

    for (const auto& [label, request] : test_cases) {
        http_client client(host, port);
        latency_stats stats;
        size_t attempts = 0;

        while (stats.count() < target_samples && attempts < target_samples * 3) {
            if (client.perform_request(request, &stats)) {
                continue;
            }
            ++attempts;
        }

        if (stats.count() == 0) {
            reporter.add("HTTP Parsing", label + " samples", 0.0, "samples");
            reporter.add("HTTP Parsing", label + " p50", 0.0, "ms");
            reporter.add("HTTP Parsing", label + " p99", 0.0, "ms");
            continue;
        }

        stats.sort();
        reporter.add(
            "HTTP Parsing", label + " samples", static_cast<double>(stats.count()), "samples");
        reporter.add("HTTP Parsing", label + " p50", stats.percentile(50.0), "ms");
        reporter.add("HTTP Parsing", label + " p99", stats.percentile(99.0), "ms");
    }
}

void test_stress(benchmark_reporter& reporter, const std::string& host, uint16_t port) {
    const size_t stress_threads = 4;
    const auto duration = std::chrono::seconds(5);

    std::atomic<size_t> total_requests{0};
    std::atomic<bool> stop{false};

    std::vector<std::thread> workers;
    workers.reserve(stress_threads);

    for (size_t i = 0; i < stress_threads; ++i) {
        workers.emplace_back([&]() {
            http_client client(host, port);
            while (!stop.load(std::memory_order_relaxed)) {
                if (client.perform_request()) {
                    total_requests.fetch_add(1, std::memory_order_relaxed);
                    continue;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }

    auto start = steady_clock::now();
    std::this_thread::sleep_for(duration);
    stop.store(true, std::memory_order_relaxed);

    for (auto& t : workers) {
        t.join();
    }

    double elapsed = std::chrono::duration<double>(steady_clock::now() - start).count();
    double rps = static_cast<double>(total_requests.load(std::memory_order_relaxed)) / elapsed;

    reporter.add("Stability", "Sustained throughput", rps, "req/s");
    reporter.add("Stability",
                 "Total requests",
                 static_cast<double>(total_requests.load(std::memory_order_relaxed)),
                 "requests");
}

} // namespace

uint16_t default_port() {
    if (const char* env = std::getenv("HELLO_PORT")) {
        int v = std::atoi(env);
        if (v > 0 && v <= 65535) {
            return static_cast<uint16_t>(v);
        }
    }
    return 18080;
}

int32_t main(int32_t argc, char* argv[]) {
    std::signal(SIGPIPE, SIG_IGN);

    std::string host = "127.0.0.1";
    uint16_t port = default_port();
    std::string output_file = "BENCHMARK_RESULTS.md";

    if (argc > 1) {
        port = static_cast<uint16_t>(std::stoul(argv[1]));
    }
    if (argc > 2) {
        output_file = argv[2];
    }

    std::cout << "=== KATANA Framework - Benchmark Suite ===\n";
    std::cout << "Target: " << host << ":" << port << "\n";
    std::cout << "Output: " << output_file << "\n";
    std::cout << "Hardware concurrency: " << std::thread::hardware_concurrency() << " cores\n\n";

    http_client probe(host, port);
    if (!probe.perform_request()) {
        std::cerr
            << "ERROR: Unable to complete probe request. Ensure hello_world_server is running.\n";
        return 1;
    }

    benchmark_reporter reporter;

    auto hw = std::max<unsigned>(1u, std::thread::hardware_concurrency());
    std::vector<size_t> throughput_levels{1, 4, 8};
    if (hw >= 12) {
        throughput_levels.push_back(12);
    }
    if (hw >= 16) {
        throughput_levels.push_back(16);
    }

    const size_t total_steps = 3 + throughput_levels.size() + 3;
    size_t step = 1;

    std::cout << "[" << step++ << "/" << total_steps << "] Measuring latency distribution...\n";
    test_latency(reporter, host, port);

    std::cout << "[" << step++ << "/" << total_steps << "] Measuring keep-alive throughput...\n";
    test_keepalive(reporter, host, port);

    std::cout << "[" << step++ << "/" << total_steps << "] Evaluating HTTP parsing overhead...\n";
    test_parsing_overhead(reporter, host, port);

    for (size_t level : throughput_levels) {
        std::cout << "[" << step++ << "/" << total_steps << "] Measuring throughput at " << level
                  << " threads...\n";
        test_throughput(reporter, host, port, level, std::chrono::milliseconds(2000));
    }

    std::cout << "[" << step++ << "/" << total_steps
              << "] Measuring connection churn (close-after-each)...\n";
    test_connection_churn(reporter, host, port, 4, std::chrono::milliseconds(1500));

    std::cout << "[" << step++ << "/" << total_steps << "] Exploring connection fan-out...\n";
    test_concurrent_connections(reporter, host, port);

    std::cout << "[" << step++ << "/" << total_steps << "] Running sustained stress test...\n";
    test_stress(reporter, host, port);

    std::cout << "\n";
    test_fd_limits(reporter);

    reporter.print_summary();
    reporter.save_to_file(output_file);

    std::cout << "\n✅ Benchmark complete! Results saved to " << output_file << "\n";
    return 0;
}
