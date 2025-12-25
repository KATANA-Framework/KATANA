// Products API Benchmark - Mixed CRUD Workload with Routing & Validation Tests
// Demonstrates comprehensive codegen benchmark scenarios

#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "katana/core/arena.hpp"
#include "katana/core/http.hpp"
#include "katana/core/problem.hpp"
#include "katana/core/router.hpp"

using namespace katana;
using namespace katana::http;

// Benchmark utilities
struct latency_stats {
    void add(int64_t ns) {
        samples.push_back(ns);
        sum_ns += ns;
    }

    void sort() { std::sort(samples.begin(), samples.end()); }

    [[nodiscard]] size_t count() const { return samples.size(); }

    [[nodiscard]] double percentile(double p) const {
        if (samples.empty())
            return 0.0;
        if (samples.size() == 1)
            return static_cast<double>(samples.front()) / 1e6;

        double rank = (p / 100.0) * static_cast<double>(samples.size() - 1);
        size_t lower_index = static_cast<size_t>(std::floor(rank));
        size_t upper_index = static_cast<size_t>(std::ceil(rank));
        double weight = rank - static_cast<double>(lower_index);
        double lower = static_cast<double>(samples[lower_index]);
        double upper = static_cast<double>(samples[upper_index]);
        double interpolated = lower + (upper - lower) * weight;
        return interpolated / 1e6;
    }

    [[nodiscard]] double avg() const {
        if (samples.empty())
            return 0.0;
        return static_cast<double>(sum_ns) / static_cast<double>(samples.size()) / 1e6;
    }

    std::vector<int64_t> samples;
    int64_t sum_ns = 0;
};

// Minimal Product DTO for benchmark (without full codegen)
struct minimal_product {
    int64_t id;
    std::string sku;
    std::string name;
    double price;
    int32_t stock;
};

// Simulate product store
class product_store {
public:
    std::optional<minimal_product> get(int64_t id) {
        auto it = products_.find(id);
        if (it == products_.end())
            return std::nullopt;
        return it->second;
    }

    int64_t create(const std::string& sku, const std::string& name, double price, int32_t stock) {
        int64_t id = next_id_++;
        products_[id] = {id, sku, name, price, stock};
        return id;
    }

    bool update(int64_t id, double price, int32_t stock) {
        auto it = products_.find(id);
        if (it == products_.end())
            return false;
        it->second.price = price;
        it->second.stock = stock;
        return true;
    }

    bool remove(int64_t id) { return products_.erase(id) > 0; }

    void clear() {
        products_.clear();
        next_id_ = 1;
    }

private:
    std::unordered_map<int64_t, minimal_product> products_;
    std::atomic<int64_t> next_id_{1};
};

product_store global_store;

// Benchmark: Routing-only mode (no business logic)
void bench_routing_only(size_t iterations) {
    route_entry routes[] = {
        {method::get,
         path_pattern::from_literal<"/products">(),
         handler_fn([](const request&, request_context&) { return response::ok("[]"); })},
        {method::post,
         path_pattern::from_literal<"/products">(),
         handler_fn([](const request&, request_context&) {
             return response{}.with_status(201).with_body("{}");
         })},
        {method::get,
         path_pattern::from_literal<"/products/{id}">(),
         handler_fn([](const request&, request_context&) { return response::ok("{}"); })},
        {method::put,
         path_pattern::from_literal<"/products/{id}">(),
         handler_fn([](const request&, request_context&) { return response::ok("{}"); })},
        {method::del,
         path_pattern::from_literal<"/products/{id}">(),
         handler_fn([](const request&, request_context&) {
             return response{}.with_status(204).with_body("");
         })},
    };

    router r(routes);
    latency_stats stats;

    // Warmup
    for (size_t i = 0; i < 10000; ++i) {
        monotonic_arena arena;
        request_context ctx{arena};
        request req;
        req.http_method = method::get;
        req.uri = "/products/123";
        [[maybe_unused]] auto resp = dispatch_or_problem(r, req, ctx);
    }

    // Measurement
    for (size_t i = 0; i < iterations; ++i) {
        monotonic_arena arena;
        request_context ctx{arena};
        request req;

        // Mix of routes
        switch (i % 5) {
        case 0:
            req.http_method = method::get;
            req.uri = "/products";
            break;
        case 1:
            req.http_method = method::post;
            req.uri = "/products";
            break;
        case 2:
            req.http_method = method::get;
            req.uri = "/products/123";
            break;
        case 3:
            req.http_method = method::put;
            req.uri = "/products/123";
            break;
        case 4:
            req.http_method = method::del;
            req.uri = "/products/123";
            break;
        }

        auto start = std::chrono::steady_clock::now();
        [[maybe_unused]] auto resp = dispatch_or_problem(r, req, ctx);
        auto end = std::chrono::steady_clock::now();

        stats.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    stats.sort();
    double duration_ms = static_cast<double>(stats.sum_ns) / 1e6;
    double ops_per_sec = static_cast<double>(iterations) / (duration_ms / 1000.0);

    std::cout << "\n=== Routing-Only Mode (No Business Logic) ===\n";
    std::cout << "  Operations: " << iterations << "\n";
    std::cout << "  Duration:   " << std::fixed << std::setprecision(2) << duration_ms << " ms\n";
    std::cout << "  Throughput: " << std::fixed << std::setprecision(2) << ops_per_sec / 1e6
              << " M ops/s\n";
    std::cout << "  p50:        " << std::fixed << std::setprecision(3) << stats.percentile(50.0)
              << " ms\n";
    std::cout << "  p99:        " << std::fixed << std::setprecision(3) << stats.percentile(99.0)
              << " ms\n";
    std::cout << "  p999:       " << std::fixed << std::setprecision(3) << stats.percentile(99.9)
              << " ms\n";
}

// Benchmark: Validation-heavy mode (simulate validation overhead)
void bench_validation_heavy(size_t iterations) {
    auto validate_sku = [](std::string_view sku) -> bool {
        if (sku.length() < 3 || sku.length() > 20)
            return false;
        for (char c : sku) {
            if (!std::isalnum(c) && c != '-')
                return false;
        }
        return true;
    };

    auto validate_price = [](double price) -> bool { return price >= 0.0; };

    auto validate_stock = [](int32_t stock) -> bool { return stock >= 0 && stock <= 1000000; };

    latency_stats stats_valid;
    latency_stats stats_invalid;

    // Warmup
    for (size_t i = 0; i < 10000; ++i) {
        [[maybe_unused]] bool v1 = validate_sku("VALID-SKU-123");
        [[maybe_unused]] bool v2 = validate_price(99.99);
        [[maybe_unused]] bool v3 = validate_stock(100);
    }

    // Measurement - valid requests
    for (size_t i = 0; i < iterations / 2; ++i) {
        auto start = std::chrono::steady_clock::now();
        bool valid = validate_sku("PROD-001") && validate_price(199.99) && validate_stock(50);
        auto end = std::chrono::steady_clock::now();

        if (!valid)
            std::cerr << "Validation failed unexpectedly\n";
        stats_valid.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    // Measurement - invalid requests (various failures)
    for (size_t i = 0; i < iterations / 2; ++i) {
        std::string sku;
        double price;
        int32_t stock;

        switch (i % 3) {
        case 0:
            sku = "X";
            price = 99.99;
            stock = 50;
            break; // Invalid SKU
        case 1:
            sku = "PROD-001";
            price = -10.0;
            stock = 50;
            break; // Invalid price
        case 2:
            sku = "PROD-001";
            price = 99.99;
            stock = -5;
            break; // Invalid stock
        }

        auto start = std::chrono::steady_clock::now();
        [[maybe_unused]] bool valid =
            validate_sku(sku) && validate_price(price) && validate_stock(stock);
        auto end = std::chrono::steady_clock::now();

        stats_invalid.add(
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    stats_valid.sort();
    stats_invalid.sort();

    double duration_valid_ms = static_cast<double>(stats_valid.sum_ns) / 1e6;
    double duration_invalid_ms = static_cast<double>(stats_invalid.sum_ns) / 1e6;
    double ops_per_sec_valid = static_cast<double>(iterations / 2) / (duration_valid_ms / 1000.0);
    double ops_per_sec_invalid =
        static_cast<double>(iterations / 2) / (duration_invalid_ms / 1000.0);

    std::cout << "\n=== Validation-Heavy Mode ===\n";
    std::cout << "\n  Valid Requests:\n";
    std::cout << "    Operations: " << iterations / 2 << "\n";
    std::cout << "    Throughput: " << std::fixed << std::setprecision(2) << ops_per_sec_valid / 1e6
              << " M ops/s\n";
    std::cout << "    p50:        " << std::fixed << std::setprecision(3)
              << stats_valid.percentile(50.0) << " ms\n";
    std::cout << "    p99:        " << std::fixed << std::setprecision(3)
              << stats_valid.percentile(99.0) << " ms\n";

    std::cout << "\n  Invalid Requests:\n";
    std::cout << "    Operations: " << iterations / 2 << "\n";
    std::cout << "    Throughput: " << std::fixed << std::setprecision(2)
              << ops_per_sec_invalid / 1e6 << " M ops/s\n";
    std::cout << "    p50:        " << std::fixed << std::setprecision(3)
              << stats_invalid.percentile(50.0) << " ms\n";
    std::cout << "    p99:        " << std::fixed << std::setprecision(3)
              << stats_invalid.percentile(99.0) << " ms\n";
}

// Benchmark: Mixed CRUD workload
void bench_mixed_crud(size_t iterations) {
    route_entry routes[] = {
        {method::get,
         path_pattern::from_literal<"/products">(),
         handler_fn([](const request&, request_context&) {
             // Simulate list operation
             return response::ok("{\"items\":[],\"total\":0}");
         })},
        {method::post,
         path_pattern::from_literal<"/products">(),
         handler_fn([](const request&, request_context&) {
             // Simulate create
             int64_t id = global_store.create("PROD-001", "Product", 99.99, 100);
             return response{}.with_status(201).with_body("{\"id\":" + std::to_string(id) + "}");
         })},
        {method::get,
         path_pattern::from_literal<"/products/{id}">(),
         handler_fn([](const request&, request_context& ctx) {
             // Simulate get by ID
             auto id_str = ctx.params.get("id").value_or("0");
             int64_t id = std::strtoll(id_str.data(), nullptr, 10);
             auto product = global_store.get(id);
             if (!product) {
                 return response{}.with_status(404).with_body("{\"error\":\"not found\"}");
             }
             return response::ok("{\"id\":" + std::to_string(product->id) + "}");
         })},
        {method::put,
         path_pattern::from_literal<"/products/{id}">(),
         handler_fn([](const request&, request_context& ctx) {
             // Simulate update
             auto id_str = ctx.params.get("id").value_or("0");
             int64_t id = std::strtoll(id_str.data(), nullptr, 10);
             bool updated = global_store.update(id, 89.99, 90);
             if (!updated) {
                 return response{}.with_status(404).with_body("{\"error\":\"not found\"}");
             }
             return response::ok("{\"id\":" + std::to_string(id) + "}");
         })},
        {method::del,
         path_pattern::from_literal<"/products/{id}">(),
         handler_fn([](const request&, request_context& ctx) {
             // Simulate delete
             auto id_str = ctx.params.get("id").value_or("0");
             int64_t id = std::strtoll(id_str.data(), nullptr, 10);
             [[maybe_unused]] bool deleted = global_store.remove(id);
             return response{}.with_status(204).with_body("");
         })},
    };

    router r(routes);
    latency_stats stats;

    // Pre-populate store with 100 products
    global_store.clear();
    for (int i = 1; i <= 100; ++i) {
        global_store.create("SKU-" + std::to_string(i), "Product " + std::to_string(i), 99.99, 100);
    }

    // Warmup
    for (size_t i = 0; i < 10000; ++i) {
        monotonic_arena arena;
        request_context ctx{arena};
        request req;
        req.http_method = method::get;
        req.uri = "/products/50";
        [[maybe_unused]] auto resp = dispatch_or_problem(r, req, ctx);
    }

    // Measurement - Mixed workload: 40% GET list, 30% GET id, 15% POST, 10% PUT, 5% DELETE
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> op_dist(1, 100);
    std::uniform_int_distribution<int> id_dist(1, 100);

    for (size_t i = 0; i < iterations; ++i) {
        monotonic_arena arena;
        request_context ctx{arena};
        request req;
        arena_string<char> uri_storage{arena_allocator<char>(&arena)};

        int op = op_dist(rng);
        if (op <= 40) {
            // GET list (40%)
            req.http_method = method::get;
            req.uri = "/products";
        } else if (op <= 70) {
            // GET by ID (30%)
            req.http_method = method::get;
            int id = id_dist(rng);
            uri_storage = "/products/" + std::to_string(id);
            req.uri = uri_storage;
        } else if (op <= 85) {
            // POST create (15%)
            req.http_method = method::post;
            req.uri = "/products";
            req.body = "{\"sku\":\"NEW-PROD\",\"name\":\"New\",\"price\":99.99,\"stock\":50}";
        } else if (op <= 95) {
            // PUT update (10%)
            req.http_method = method::put;
            int id = id_dist(rng);
            uri_storage = "/products/" + std::to_string(id);
            req.uri = uri_storage;
            req.body = "{\"price\":79.99,\"stock\":40}";
        } else {
            // DELETE (5%)
            req.http_method = method::del;
            int id = id_dist(rng);
            uri_storage = "/products/" + std::to_string(id);
            req.uri = uri_storage;
        }

        auto start = std::chrono::steady_clock::now();
        [[maybe_unused]] auto resp = dispatch_or_problem(r, req, ctx);
        auto end = std::chrono::steady_clock::now();

        stats.add(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    stats.sort();
    double duration_ms = static_cast<double>(stats.sum_ns) / 1e6;
    double ops_per_sec = static_cast<double>(iterations) / (duration_ms / 1000.0);

    std::cout << "\n=== Mixed CRUD Workload (40% GET list, 30% GET id, 15% POST, 10% PUT, 5% "
                 "DELETE) ===\n";
    std::cout << "  Operations: " << iterations << "\n";
    std::cout << "  Duration:   " << std::fixed << std::setprecision(2) << duration_ms << " ms\n";
    std::cout << "  Throughput: " << std::fixed << std::setprecision(2) << ops_per_sec / 1e6
              << " M ops/s\n";
    std::cout << "  p50:        " << std::fixed << std::setprecision(3) << stats.percentile(50.0)
              << " ms\n";
    std::cout << "  p99:        " << std::fixed << std::setprecision(3) << stats.percentile(99.0)
              << " ms\n";
    std::cout << "  p999:       " << std::fixed << std::setprecision(3) << stats.percentile(99.9)
              << " ms\n";
}

int main() {
    std::cout << "KATANA Products API Benchmark Suite\n";
    std::cout << "====================================\n";

    constexpr size_t iterations = 200000;

    bench_routing_only(iterations);
    bench_validation_heavy(iterations);
    bench_mixed_crud(iterations);

    std::cout << "\n✓ All benchmarks completed\n";
    return 0;
}
