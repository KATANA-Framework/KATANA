#include "katana/core/arena.hpp"
#include "katana/core/router.hpp"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <vector>

using namespace std::chrono;
using namespace katana;
using namespace katana::http;

struct benchmark_result {
    std::string name;
    double throughput;
    double latency_p50;
    double latency_p99;
    double latency_p999;
    uint64_t operations;
    uint64_t duration_ms;
    uint64_t errors;
};

void print_result(const benchmark_result& result) {
    std::cout << "\n=== " << result.name << " ===\n";
    std::cout << "Operations: " << result.operations << "\n";
    std::cout << "Duration: " << result.duration_ms << " ms\n";
    std::cout << "Throughput: " << std::fixed << std::setprecision(2) << result.throughput
              << " ops/sec\n";
    std::cout << "Errors: " << result.errors << "\n";
    if (result.latency_p50 > 0.0) {
        std::cout << "Latency p50: " << std::fixed << std::setprecision(3) << result.latency_p50
                  << " us\n";
        std::cout << "Latency p99: " << std::fixed << std::setprecision(3) << result.latency_p99
                  << " us\n";
        std::cout << "Latency p999: " << std::fixed << std::setprecision(3) << result.latency_p999
                  << " us\n";
    }
}

request make_request(std::string_view uri, method m, monotonic_arena& arena) {
    request req;
    req.http_method = m;
    req.uri = uri;
    req.headers = headers_map(&arena);
    return req;
}

benchmark_result bench_dispatch(const std::string& name,
                                const router& r,
                                const std::vector<std::string_view>& paths,
                                method m,
                                size_t iterations) {
    std::vector<double> latencies;
    latencies.reserve(iterations);

    uint64_t errors = 0;
    auto start = steady_clock::now();

    for (size_t i = 0; i < iterations; ++i) {
        monotonic_arena arena;
        request_context ctx{arena};
        const auto path = paths[i % paths.size()];
        auto req = make_request(path, m, arena);

        auto t0 = steady_clock::now();
        auto res = dispatch_or_problem(r, req, ctx);
        auto t1 = steady_clock::now();

        if (res.status >= 400) {
            ++errors;
        }

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(t1 - t0).count()) / 1000.0;
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = name;
    result.operations = iterations;
    result.duration_ms = duration_ms;
    result.throughput =
        (static_cast<double>(iterations) * 1000.0) / static_cast<double>(duration_ms);
    result.latency_p50 = latencies[iterations / 2];
    result.latency_p99 = latencies[iterations * 99 / 100];
    result.latency_p999 = latencies[iterations * 999 / 1000];
    result.errors = errors;
    return result;
}

int main() {
    handler_fn ok_handler = [](const request&, request_context&) {
        return response::ok("ok", "text/plain");
    };

    std::array<middleware_fn, 1> middleware = {
        middleware_fn([](const request&, request_context&, next_fn next) {
            // cheap middleware to ensure chain overhead is measured
            return next();
        }),
    };

    route_entry routes[] = {
        {method::get, path_pattern::from_literal<"/">(), ok_handler},
        {method::get,
         path_pattern::from_literal<"/users/{id}">(),
         ok_handler,
         make_middleware_chain(middleware)},
        {method::get, path_pattern::from_literal<"/users/me">(), ok_handler},
        {method::get, path_pattern::from_literal<"/posts/{id}/comments/{cid}">(), ok_handler},
        {method::post, path_pattern::from_literal<"/posts">(), ok_handler},
        {method::get, path_pattern::from_literal<"/static/about">(), ok_handler},
    };

    router r(routes);

    std::vector<std::string_view> happy_paths = {
        "/",
        "/users/me",
        "/users/42",
        "/posts/10/comments/5",
        "/posts",
        "/static/about",
    };

    std::vector<std::string_view> not_found_paths = {
        "/missing",
        "/unknown/path",
        "/posts/10/comments",
        "/users/",
        "/static",
    };

    const size_t iterations = 200000;

    auto warmup = bench_dispatch("Warmup", r, happy_paths, method::get, 10000);
    (void)warmup;

    auto hit = bench_dispatch("Router dispatch (hits)", r, happy_paths, method::get, iterations);
    auto miss =
        bench_dispatch("Router dispatch (not found)", r, not_found_paths, method::get, iterations);
    auto method_na =
        bench_dispatch("Router dispatch (405)", r, happy_paths, method::post, iterations);

    print_result(hit);
    print_result(miss);
    print_result(method_na);

    return 0;
}
