#include "katana/core/arena.hpp"
#include "katana/core/router.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
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

enum class expected_response {
    success_2xx,
    not_found_404,
    method_not_allowed_405,
};

bool matches_expected_status(int status, expected_response expected) {
    switch (expected) {
    case expected_response::success_2xx:
        return status >= 200 && status < 300;
    case expected_response::not_found_404:
        return status == 404;
    case expected_response::method_not_allowed_405:
        return status == 405;
    }
    return false;
}

double percentile(const std::vector<double>& sorted_values, double pct) {
    if (sorted_values.empty()) {
        return 0.0;
    }
    if (sorted_values.size() == 1) {
        return sorted_values[0];
    }

    const double clamped = std::min(1.0, std::max(0.0, pct));
    const double pos = clamped * static_cast<double>(sorted_values.size() - 1);
    const size_t lo = static_cast<size_t>(std::floor(pos));
    const size_t hi = static_cast<size_t>(std::ceil(pos));
    if (lo == hi) {
        return sorted_values[lo];
    }
    const double frac = pos - static_cast<double>(lo);
    return sorted_values[lo] * (1.0 - frac) + sorted_values[hi] * frac;
}

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

benchmark_result bench_dispatch(const std::string& name,
                                const router& r,
                                const std::vector<std::string_view>& paths,
                                method m,
                                size_t iterations,
                                expected_response expected) {
    constexpr size_t sample_batch_size = 32;
    std::vector<double> latencies;
    latencies.reserve(iterations / sample_batch_size + 4);

    uint64_t errors = 0;

    // Pre-create arena outside the loop — reset per iteration to simulate fresh state.
    // This measures routing performance, not memory allocation overhead.
    monotonic_arena arena;

    const size_t warmup_iterations = std::min<size_t>(iterations / 20, 10'000);
    for (size_t i = 0; i < warmup_iterations; ++i) {
        arena.reset();
        request_context ctx{arena};
        const auto path = paths[i % paths.size()];

        request req;
        req.http_method = m;
        req.uri = path;
        req.headers = headers_map(&arena);

        auto res = dispatch_or_problem(r, req, ctx);
        (void)res;
    }

    auto start = steady_clock::now();

    size_t processed = 0;
    while (processed < iterations) {
        const size_t batch = std::min(sample_batch_size, iterations - processed);
        auto batch_start = steady_clock::now();

        for (size_t local = 0; local < batch; ++local, ++processed) {
            arena.reset();
            request_context ctx{arena};
            const auto path = paths[processed % paths.size()];

            // Construct a minimal request for routing (only method + uri matter)
            request req;
            req.http_method = m;
            req.uri = path;
            req.headers = headers_map(&arena);

            auto res = dispatch_or_problem(r, req, ctx);
            const int status = static_cast<int>(res.status);
            if (!matches_expected_status(status, expected)) {
                ++errors;
            }
        }

        auto batch_end = steady_clock::now();
        const double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(batch_end - batch_start).count()) /
            (1000.0 * static_cast<double>(batch));
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = name;
    result.operations = iterations;
    result.duration_ms = duration_ms;
    const auto elapsed_ns = duration_cast<nanoseconds>(end - start).count();
    result.throughput =
        elapsed_ns > 0
            ? (static_cast<double>(iterations) * 1'000'000'000.0) / static_cast<double>(elapsed_ns)
            : 0.0;
    result.latency_p50 = percentile(latencies, 0.50);
    result.latency_p99 = percentile(latencies, 0.99);
    result.latency_p999 = percentile(latencies, 0.999);
    result.errors = errors;
    return result;
}

// Build a large route table to test router performance at scale.
// Uses compile-time generated patterns for each route.
// This creates 50+ routes across various API namespaces to simulate realistic load.
std::array<route_entry, 64> make_large_route_table(const handler_fn& h) {
    return {{
        // Users API (10 routes)
        {method::get, path_pattern::from_literal<"/api/v1/users">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/users/{id}">(), h},
        {method::post, path_pattern::from_literal<"/api/v1/users">(), h},
        {method::put, path_pattern::from_literal<"/api/v1/users/{id}">(), h},
        {method::del, path_pattern::from_literal<"/api/v1/users/{id}">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/users/{id}/profile">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/users/{id}/settings">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/users/{id}/posts">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/users/{id}/comments">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/users/{id}/followers">(), h},
        // Posts API (10 routes)
        {method::get, path_pattern::from_literal<"/api/v1/posts">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/posts/{id}">(), h},
        {method::post, path_pattern::from_literal<"/api/v1/posts">(), h},
        {method::put, path_pattern::from_literal<"/api/v1/posts/{id}">(), h},
        {method::del, path_pattern::from_literal<"/api/v1/posts/{id}">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/posts/{id}/comments">(), h},
        {method::post, path_pattern::from_literal<"/api/v1/posts/{id}/comments">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/posts/{id}/likes">(), h},
        {method::post, path_pattern::from_literal<"/api/v1/posts/{id}/likes">(), h},
        {method::del, path_pattern::from_literal<"/api/v1/posts/{id}/likes">(), h},
        // Comments API (8 routes)
        {method::get, path_pattern::from_literal<"/api/v1/comments">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/comments/{id}">(), h},
        {method::put, path_pattern::from_literal<"/api/v1/comments/{id}">(), h},
        {method::del, path_pattern::from_literal<"/api/v1/comments/{id}">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/comments/{id}/replies">(), h},
        {method::post, path_pattern::from_literal<"/api/v1/comments/{id}/replies">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/comments/{id}/likes">(), h},
        {method::post, path_pattern::from_literal<"/api/v1/comments/{id}/likes">(), h},
        // Products API (10 routes)
        {method::get, path_pattern::from_literal<"/api/v1/products">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/products/{id}">(), h},
        {method::post, path_pattern::from_literal<"/api/v1/products">(), h},
        {method::put, path_pattern::from_literal<"/api/v1/products/{id}">(), h},
        {method::del, path_pattern::from_literal<"/api/v1/products/{id}">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/products/{id}/reviews">(), h},
        {method::post, path_pattern::from_literal<"/api/v1/products/{id}/reviews">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/products/{id}/images">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/products/{id}/variants">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/products/{id}/inventory">(), h},
        // Orders API (10 routes)
        {method::get, path_pattern::from_literal<"/api/v1/orders">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/orders/{id}">(), h},
        {method::post, path_pattern::from_literal<"/api/v1/orders">(), h},
        {method::put, path_pattern::from_literal<"/api/v1/orders/{id}">(), h},
        {method::del, path_pattern::from_literal<"/api/v1/orders/{id}">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/orders/{id}/items">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/orders/{id}/shipping">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/orders/{id}/payments">(), h},
        {method::post, path_pattern::from_literal<"/api/v1/orders/{id}/cancel">(), h},
        {method::post, path_pattern::from_literal<"/api/v1/orders/{id}/refund">(), h},
        // Auth API (6 routes)
        {method::post, path_pattern::from_literal<"/api/v1/auth/login">(), h},
        {method::post, path_pattern::from_literal<"/api/v1/auth/logout">(), h},
        {method::post, path_pattern::from_literal<"/api/v1/auth/refresh">(), h},
        {method::post, path_pattern::from_literal<"/api/v1/auth/register">(), h},
        {method::post, path_pattern::from_literal<"/api/v1/auth/reset-password">(), h},
        {method::get, path_pattern::from_literal<"/api/v1/auth/verify/{token}">(), h},
        // Static files (6 routes)
        {method::get, path_pattern::from_literal<"/static/css/{file}">(), h},
        {method::get, path_pattern::from_literal<"/static/js/{file}">(), h},
        {method::get, path_pattern::from_literal<"/static/images/{file}">(), h},
        {method::get, path_pattern::from_literal<"/static/fonts/{file}">(), h},
        {method::get, path_pattern::from_literal<"/favicon.ico">(), h},
        {method::get, path_pattern::from_literal<"/robots.txt">(), h},
        // Admin API (4 routes)
        {method::get, path_pattern::from_literal<"/admin/dashboard">(), h},
        {method::get, path_pattern::from_literal<"/admin/users">(), h},
        {method::get, path_pattern::from_literal<"/admin/logs">(), h},
        {method::get, path_pattern::from_literal<"/admin/settings">(), h},
    }};
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

    // Small route table (original)
    route_entry small_routes[] = {
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

    router small_router(small_routes);

    // Large route table (64+ routes)
    auto large_routes = make_large_route_table(ok_handler);
    router large_router(large_routes);

    std::vector<std::string_view> hit_paths = {
        "/",
        "/users/me",
        "/users/42",
        "/posts/10/comments/5",
        "/static/about",
    };

    std::vector<std::string_view> not_found_paths = {
        "/missing",
        "/unknown/path",
        "/users/42/profile",
        "/posts/10/comments/5/replies",
        "/static/unknown/page",
    };

    std::vector<std::string_view> method_mismatch_paths = {
        "/",
        "/users/me",
        "/users/42",
        "/posts/10/comments/5",
        "/static/about",
    };

    // Paths for large router that match routes at various positions
    std::vector<std::string_view> large_happy_paths = {
        "/api/v1/users",
        "/api/v1/users/42",
        "/api/v1/posts/100",
        "/api/v1/products/999",
        "/api/v1/orders/12345",
        "/api/v1/orders/12345/items",
        "/api/v1/auth/verify/abc123",
        "/static/css/main.css",
        "/static/js/app.js",
        "/admin/dashboard",
        "/robots.txt",
        "/favicon.ico",
    };

    // Paths for 405 testing on large router (wrong method)
    std::vector<std::string_view> large_method_mismatch_paths = {
        "/api/v1/users",      // GET route, test with POST
        "/api/v1/posts",      // GET+POST, test with DELETE
        "/api/v1/auth/login", // POST route, test with GET
        "/admin/dashboard",   // GET route, test with PUT
    };

    // Edge-case paths: very long paths, deep nesting
    std::vector<std::string_view> edge_case_paths = {
        "/api/v1/users/123/posts",
        "/api/v1/posts/456/comments",
        "/api/v1/products/789/reviews",
        "/api/v1/orders/101/shipping",
    };

    const size_t iterations = 200000;

    // Warmup
    auto warmup = bench_dispatch(
        "Warmup (small)", small_router, hit_paths, method::get, 10000, expected_response::success_2xx);
    (void)warmup;

    std::cout << "========================================\n";
    std::cout << "   Router Dispatch Benchmarks\n";
    std::cout << "========================================\n";

    std::cout << "\n--- Small Route Table (6 routes) ---\n";

    auto hit = bench_dispatch(
        "Router dispatch (hits)", small_router, hit_paths, method::get, iterations, expected_response::success_2xx);
    auto miss = bench_dispatch(
        "Router dispatch (not found)", small_router, not_found_paths, method::get, iterations, expected_response::not_found_404);
    auto method_na = bench_dispatch(
        "Router dispatch (405)", small_router, method_mismatch_paths, method::post, iterations, expected_response::method_not_allowed_405);

    print_result(hit);
    print_result(miss);
    print_result(method_na);

    std::cout << "\n--- Large Route Table (64 routes) ---\n";

    // Warmup large router
    auto warmup_large =
        bench_dispatch("Warmup (large)", large_router, large_happy_paths, method::get, 10000, expected_response::success_2xx);
    (void)warmup_large;

    auto large_hit = bench_dispatch(
        "Large router (hits)", large_router, large_happy_paths, method::get, iterations, expected_response::success_2xx);
    auto large_miss = bench_dispatch(
        "Large router (not found)", large_router, not_found_paths, method::get, iterations, expected_response::not_found_404);
    auto large_405 = bench_dispatch(
        "Large router (405)", large_router, large_method_mismatch_paths, method::del, iterations, expected_response::method_not_allowed_405);
    auto large_edge = bench_dispatch(
        "Large router (edge cases)", large_router, edge_case_paths, method::get, iterations, expected_response::success_2xx);

    print_result(large_hit);
    print_result(large_miss);
    print_result(large_405);
    print_result(large_edge);

    std::cout << "\n========================================\n";
    std::cout << "   Summary\n";
    std::cout << "========================================\n";
    std::cout << "Small router (6 routes):\n";
    std::cout << "  Hits: " << std::fixed << std::setprecision(0) << hit.throughput << " ops/sec\n";
    std::cout << "  405:  " << std::fixed << std::setprecision(0) << method_na.throughput
              << " ops/sec\n";
    std::cout << "Large router (64 routes):\n";
    std::cout << "  Hits: " << std::fixed << std::setprecision(0) << large_hit.throughput
              << " ops/sec\n";
    std::cout << "  405:  " << std::fixed << std::setprecision(0) << large_405.throughput
              << " ops/sec\n";

    return 0;
}
