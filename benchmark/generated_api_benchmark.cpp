#include "generated_handlers.hpp"
#include "generated_router_bindings.hpp"
#include "katana/core/arena.hpp"
#include "katana/core/http.hpp"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <vector>

using namespace katana;
using namespace katana::http;
using namespace std::chrono;

struct bench_handler : generated::api_handler {
    result<void> health(response& out) override {
        out = response::ok("ok");
        return {};
    }

    result<void> list_users(response& out) override {
        out.assign_json(R"([{"id":1,"name":"Alice"}])");
        return {};
    }

    result<void> create_user(const UserInput& body, response& out) override {
        (void)body;
        out.assign_json(R"({"id":42})", 201, "Created");
        return {};
    }

    result<void> get_user(int64_t id, response& out) override {
        std::string payload = std::string("{\"id\":") + std::to_string(id) + ",\"name\":\"User\"}";
        out.assign_json(std::move(payload));
        return {};
    }

    result<void> update_user(int64_t id, const UserInput&, response& out) override {
        std::string payload =
            std::string("{\"id\":") + std::to_string(id) + ",\"status\":\"updated\"}";
        out.assign_json(std::move(payload));
        return {};
    }
};

struct bench_result {
    std::string name;
    double throughput;
    double latency_p50;
    double latency_p99;
    double latency_p999;
    uint64_t operations;
    uint64_t duration_ms;
    uint64_t errors;
};

void print_result(const bench_result& result) {
    std::cout << "\n=== " << result.name << " ===\n";
    std::cout << "Operations: " << result.operations << "\n";
    std::cout << "Duration: " << result.duration_ms << " ms\n";
    std::cout << "Throughput: " << std::fixed << std::setprecision(2) << result.throughput
              << " ops/sec\n";
    std::cout << "Errors: " << result.errors << "\n";
    std::cout << "Latency p50: " << std::fixed << std::setprecision(3) << result.latency_p50
              << " us\n";
    std::cout << "Latency p99: " << std::fixed << std::setprecision(3) << result.latency_p99
              << " us\n";
    std::cout << "Latency p999: " << std::fixed << std::setprecision(3) << result.latency_p999
              << " us\n";
}

request make_request(std::string_view uri, method m, std::string_view body = "") {
    request req;
    req.http_method = m;
    req.uri = uri;
    req.headers = headers_map(nullptr);
    req.headers.set(field::accept, "application/json");
    if (!body.empty()) {
        req.headers.set(field::content_type, "application/json");
    }
    req.body = body;
    return req;
}

template <typename DispatchFn>
bench_result bench_dispatch(const std::string& name,
                            DispatchFn&& dispatch,
                            const std::vector<request>& requests,
                            size_t iterations) {
    std::vector<double> latencies;
    latencies.reserve(iterations);

    uint64_t errors = 0;
    auto start = steady_clock::now();

    for (size_t i = 0; i < iterations; ++i) {
        monotonic_arena arena;
        request_context ctx{arena};
        const auto& req = requests[i % requests.size()];

        response final;
        auto t0 = steady_clock::now();
        auto status = dispatch(req, ctx, final);
        auto t1 = steady_clock::now();

        if (!status || final.status >= 400) {
            ++errors;
        }

        double latency_us =
            static_cast<double>(duration_cast<nanoseconds>(t1 - t0).count()) / 1000.0;
        latencies.push_back(latency_us);
    }

    auto end = steady_clock::now();
    auto duration_ms = static_cast<uint64_t>(duration_cast<milliseconds>(end - start).count());

    std::sort(latencies.begin(), latencies.end());

    bench_result result;
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
    bench_handler handler;
    auto r = generated::make_fast_router(handler);
    auto dispatch = [&](const request& req, request_context& ctx, response& out) -> result<void> {
        auto status = r.dispatch_to(req, ctx, out);
        if (!status) {
            out = response::error(problem_details::internal_server_error());
            return status;
        }
        return {};
    };

    // Requests: mix path params and JSON body
    std::vector<request> reqs;
    reqs.push_back(make_request("/health", method::get));
    reqs.push_back(make_request("/users", method::get));
    reqs.push_back(
        make_request("/users", method::post, R"({"name":"Alice","email":"a@b.com","age":30})"));
    reqs.push_back(make_request("/users/42", method::get));
    reqs.push_back(
        make_request("/users/99", method::put, R"({"name":"Bob","email":"b@c.com","age":25})"));

    const size_t iterations = 200000;

    auto result = bench_dispatch("Generated API dispatch+parse", dispatch, reqs, iterations);
    print_result(result);

    return 0;
}
