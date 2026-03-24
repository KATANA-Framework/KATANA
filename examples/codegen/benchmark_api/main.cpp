#include "demo_service.hpp"
#include "katana/core/http_server.hpp"

#include <algorithm>
#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string_view>
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
        std::cerr << "benchmark_api requires PostgreSQL DSN via "
                     "KATANA_BENCHMARK_API_POSTGRES_DSN or KATANA_TEST_POSTGRES_DSN\n";
        return 1;
    }

    const uint16_t port = read_port();
    const uint16_t workers = static_cast<uint16_t>(
        std::min<uint32_t>(std::max(1u, std::thread::hardware_concurrency()), 64));

    katana::benchmark_api_demo::service service({
        .connection_string = dsn,
        .executor_count = read_size_env("KATANA_BENCHMARK_API_EXECUTORS", workers),
        .eager_connect = true,
        .bootstrap_schema = read_bool_env("KATANA_BENCHMARK_API_BOOTSTRAP", true),
        .reset_data_on_start = read_bool_env("KATANA_BENCHMARK_API_RESET", true),
        .seed_item_count = read_size_env("KATANA_BENCHMARK_API_SEED_COUNT", 1024),
    });

    auto started = service.start();
    if (!started) {
        std::cerr << "benchmark_api failed to initialize SQL demo service\n";
        return 1;
    }

    auto router = generated::make_fast_router(service.handler());

    std::cout << "KATANA Benchmark API on :" << port << " (" << workers << " workers)\n"
              << "PostgreSQL-backed Stage 4 demo service enabled.\n"
              << "Endpoints:\n"
              << "  POST /compute/sum        - sum array of doubles\n"
              << "  POST /compute/stats      - min/max/mean/median\n"
              << "  POST /users/register     - validation-heavy path\n"
              << "  GET  /items              - SQL-backed list with pagination\n"
              << "  POST /items              - SQL-backed create (header: X-Request-Id)\n"
              << "  GET  /items/{id}         - SQL-backed get by id\n"
              << "  PUT  /items/{id}         - SQL-backed update\n"
              << "  DEL  /items/{id}         - SQL-backed delete\n"
              << "  POST /echo               - echo with optional transform\n"
              << "  GET  /health             - health check\n";

    return http::server(router)
        .listen(port)
        .workers(workers)
        .on_start([&]() { std::cout << "Server ready." << std::endl; })
        .run();
}

#else

int main() {
    std::cerr << "benchmark_api requires PostgreSQL/libpq support\n";
    return 1;
}

#endif
