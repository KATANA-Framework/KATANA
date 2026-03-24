#include "demo_service.hpp"
#include "katana/core/http_server.hpp"

#include <algorithm>
#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <thread>

using namespace katana;

#ifdef KATANA_HAS_LIBPQ

namespace {

uint16_t read_port() {
    if (const char* v = std::getenv("PORT")) {
        int p = std::atoi(v);
        if (p > 0 && p < 65536) {
            return static_cast<uint16_t>(p);
        }
    }
    return 9090;
}

std::size_t read_size_env(const char* name, std::size_t fallback) {
    const char* raw = std::getenv(name);
    if (raw == nullptr || *raw == '\0') {
        return fallback;
    }
    std::size_t parsed = 0;
    const auto* begin = raw;
    const auto* end = raw + std::char_traits<char>::length(raw);
    const auto [ptr, ec] = std::from_chars(begin, end, parsed);
    if (ec != std::errc{} || ptr != end) {
        return fallback;
    }
    return parsed;
}

bool read_bool_env(const char* name, bool fallback) {
    const char* raw = std::getenv(name);
    if (raw == nullptr || *raw == '\0') {
        return fallback;
    }
    const std::string_view value(raw);
    if (value == "1" || value == "true" || value == "yes") {
        return true;
    }
    if (value == "0" || value == "false" || value == "no") {
        return false;
    }
    return fallback;
}

const char* postgres_dsn() {
    if (const char* demo = std::getenv("KATANA_BENCHMARK_API_POSTGRES_DSN")) {
        if (*demo != '\0') {
            return demo;
        }
    }
    if (const char* test = std::getenv("KATANA_TEST_POSTGRES_DSN")) {
        if (*test != '\0') {
            return test;
        }
    }
    return nullptr;
}

} // namespace

int main() {
    const char* dsn = postgres_dsn();
    if (dsn == nullptr) {
        std::cerr << "benchmark_api_sql_server requires PostgreSQL DSN\n";
        return 1;
    }

    const uint16_t port = read_port();
    const std::size_t default_workers =
        std::min<std::size_t>(std::max<std::size_t>(1, std::thread::hardware_concurrency()), 64);
    const std::size_t executor_count = read_size_env("KATANA_BENCHMARK_API_EXECUTORS", default_workers);
    const uint16_t workers = static_cast<uint16_t>(
        std::min<std::size_t>(read_size_env("KATANA_BENCHMARK_API_WORKERS", executor_count), 64));

    katana::benchmark_api_demo::service service({
        .connection_string = dsn,
        .executor_count = executor_count,
        .eager_connect = true,
        .bootstrap_schema = read_bool_env("KATANA_BENCHMARK_API_BOOTSTRAP", true),
        .reset_data_on_start = read_bool_env("KATANA_BENCHMARK_API_RESET", true),
        .seed_item_count = read_size_env("KATANA_BENCHMARK_API_SEED_COUNT", 4096),
    });

    auto started = service.start();
    if (!started) {
        std::cerr << "benchmark_api_sql_server failed to initialize SQL demo service\n";
        return 1;
    }

    auto router = generated::make_router(service.handler());
    return http::server(router).listen(port).workers(workers).run();
}

#else

int main() {
    std::cerr << "benchmark_api_sql_server requires PostgreSQL/libpq support\n";
    return 1;
}

#endif
