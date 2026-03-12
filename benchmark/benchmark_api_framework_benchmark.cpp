#include "bench_utils.hpp"
#include "generated_dtos.hpp"
#include "generated_handlers.hpp"
#include "generated_json.hpp"
#include "generated_router_bindings.hpp"
#include "katana/core/handler_context.hpp"
#include "katana/core/http.hpp"
#include "katana/core/problem.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <optional>
#include <string_view>
#include <vector>

using namespace katana;
using namespace katana::http;
using bench_util::do_not_optimize;

namespace {

class framework_handler : public generated::api_handler {
public:
    result<void> compute_sum(const SumRequest& body, response& out) override {
        auto& arena = handler_context::arena();
        SumResponse resp(&arena);
        resp.result = body.values.empty() ? 0.0 : body.values.front();
        resp.count = static_cast<int64_t>(body.values.size());
        out.assign_json(serialize_SumResponse(resp));
        return {};
    }

    result<void> compute_stats(const StatsRequest& body, response& out) override {
        auto& arena = handler_context::arena();
        StatsResponse resp(&arena);
        const double base = body.values.empty() ? 0.0 : body.values.front();
        resp.min = base;
        resp.max = base;
        resp.sum = base;
        resp.mean = base;
        resp.count = static_cast<int64_t>(body.values.size());
        if (body.include_median.value_or(false)) {
            resp.median = base;
        }
        out.assign_json(serialize_StatsResponse(resp));
        return {};
    }

    result<void> register_user(const RegisterRequest& body, response& out) override {
        auto& arena = handler_context::arena();
        UserResponse resp(&arena);
        resp.id =
            arena_string<>("550e8400-e29b-41d4-a716-446655440000", arena_allocator<char>(&arena));
        resp.username = arena_string<>(
            body.username.data(), body.username.size(), arena_allocator<char>(&arena));
        resp.email =
            arena_string<>(body.email.data(), body.email.size(), arena_allocator<char>(&arena));
        resp.role = body.role.value_or(UserRole_enum::user);
        resp.created_at = arena_string<>("2026-01-01T00:00:00Z", arena_allocator<char>(&arena));
        out.assign_json(serialize_UserResponse(resp), 201, "Created");
        return {};
    }

    result<void> list_items(std::optional<int64_t> limit,
                            std::optional<int64_t> offset,
                            std::optional<std::string_view> category,
                            response& out) override {
        auto& arena = handler_context::arena();
        ItemList resp(&arena);
        resp.total = 1;
        resp.limit = limit.value_or(20);
        resp.offset = offset.value_or(0);

        Item item(&arena);
        item.id = 42;
        item.name = arena_string<>("Benchmark Item", arena_allocator<char>(&arena));
        item.description =
            arena_string<>("Generated framework path", arena_allocator<char>(&arena));
        item.price = 99.0;
        item.stock = 7;
        item.category = category ? ItemCategory_enum::tools : ItemCategory_enum::other;
        item.tags.emplace(arena_allocator<arena_string<>>(&arena));
        item.tags->emplace_back("bench", arena_allocator<char>(&arena));
        resp.items.push_back(std::move(item));

        out.assign_json(serialize_ItemList(resp));
        return {};
    }

    result<void> create_item([[maybe_unused]] std::string_view X_Request_Id,
                             [[maybe_unused]] std::optional<std::string_view> session,
                             const CreateItemRequest& body,
                             response& out) override {
        auto& arena = handler_context::arena();
        Item item(&arena);
        item.id = 100;
        item.name =
            arena_string<>(body.name.data(), body.name.size(), arena_allocator<char>(&arena));
        if (body.description) {
            item.description = arena_string<>(
                body.description->data(), body.description->size(), arena_allocator<char>(&arena));
        }
        item.price = body.price;
        item.stock = body.stock.value_or(0);
        item.category = body.category;
        if (body.tags) {
            item.tags.emplace(arena_allocator<arena_string<>>(&arena));
            for (const auto& tag : *body.tags) {
                item.tags->emplace_back(tag.data(), tag.size(), arena_allocator<char>(&arena));
            }
        }
        out.assign_json(serialize_Item(item), 201, "Created");
        return {};
    }

    result<void> get_item(int64_t id, response& out) override {
        auto& arena = handler_context::arena();
        Item item(&arena);
        item.id = id;
        item.name = arena_string<>("Benchmark Item", arena_allocator<char>(&arena));
        item.description =
            arena_string<>("Generated framework path", arena_allocator<char>(&arena));
        item.price = 99.0;
        item.stock = 7;
        item.category = ItemCategory_enum::tools;
        item.tags.emplace(arena_allocator<arena_string<>>(&arena));
        item.tags->emplace_back("bench", arena_allocator<char>(&arena));
        out.assign_json(serialize_Item(item));
        return {};
    }

    result<void> update_item(int64_t id, const UpdateItemRequest& body, response& out) override {
        auto& arena = handler_context::arena();
        Item item(&arena);
        item.id = id;
        if (body.name) {
            item.name = arena_string<>(
                body.name->data(), body.name->size(), arena_allocator<char>(&arena));
        } else {
            item.name = arena_string<>("Updated Item", arena_allocator<char>(&arena));
        }
        if (body.description) {
            item.description = arena_string<>(
                body.description->data(), body.description->size(), arena_allocator<char>(&arena));
        }
        item.price = body.price.value_or(99.0);
        item.stock = body.stock.value_or(7);
        item.category = ItemCategory_enum::tools;
        if (body.tags) {
            item.tags.emplace(arena_allocator<arena_string<>>(&arena));
            for (const auto& tag : *body.tags) {
                item.tags->emplace_back(tag.data(), tag.size(), arena_allocator<char>(&arena));
            }
        }
        out.assign_json(serialize_Item(item));
        return {};
    }

    result<void> delete_item([[maybe_unused]] int64_t id, response& out) override {
        out.reset();
        out.status = 204;
        out.reason = "No Content";
        return {};
    }

    result<void> echo(const EchoRequest& body, response& out) override {
        auto& arena = handler_context::arena();
        EchoResponse resp(&arena);
        resp.message =
            arena_string<>(body.message.data(), body.message.size(), arena_allocator<char>(&arena));
        resp.length = static_cast<int64_t>(body.message.size());
        out.assign_json(serialize_EchoResponse(resp));
        return {};
    }

    result<void> health_check(response& out) override {
        out.assign_json(R"({"status":"ok","uptime_ms":1234})");
        return {};
    }
};

struct bench_result {
    const char* name = "";
    double ns_per_op = 0.0;
    double ops_per_sec = 0.0;
    double p50_us = 0.0;
    double p95_us = 0.0;
    double p99_us = 0.0;
    double p999_us = 0.0;
    uint64_t errors = 0;
    size_t iterations = 0;
};

static double percentile_ns(const std::vector<double>& sorted, double p) {
    if (sorted.empty()) {
        return 0.0;
    }
    if (sorted.size() == 1) {
        return sorted.front();
    }

    const double rank = (p / 100.0) * static_cast<double>(sorted.size() - 1);
    const size_t lo = static_cast<size_t>(std::floor(rank));
    const size_t hi = static_cast<size_t>(std::ceil(rank));
    const double t = rank - static_cast<double>(lo);
    return sorted[lo] + (sorted[hi] - sorted[lo]) * t;
}

template <typename DispatchFn>
static bench_result run_dispatch_bench(const char* name,
                                       DispatchFn&& dispatch,
                                       const std::vector<request>& requests,
                                       size_t iterations) {
    const size_t warmup = std::min<size_t>(iterations / 10, 20000);
    for (size_t i = 0; i < warmup; ++i) {
        monotonic_arena arena;
        request_context ctx{arena};
        const auto& req = requests[i % requests.size()];
        response resp;
        auto status = dispatch(req, ctx, resp);
        do_not_optimize(status.has_value());
        do_not_optimize(resp.status);
    }

    std::vector<double> latencies_ns;
    latencies_ns.reserve(iterations);
    uint64_t errors = 0;

    const auto run_start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < iterations; ++i) {
        monotonic_arena arena;
        request_context ctx{arena};
        const auto& req = requests[i % requests.size()];

        response resp;
        const auto t0 = std::chrono::steady_clock::now();
        auto status = dispatch(req, ctx, resp);
        const auto t1 = std::chrono::steady_clock::now();

        if (!status || resp.status >= 400) {
            ++errors;
        }
        latencies_ns.push_back(static_cast<double>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count()));
        do_not_optimize(status.has_value());
        do_not_optimize(resp.status);
    }
    const auto run_end = std::chrono::steady_clock::now();

    const double total_ns = static_cast<double>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(run_end - run_start).count());
    std::sort(latencies_ns.begin(), latencies_ns.end());

    bench_result out;
    out.name = name;
    out.iterations = iterations;
    out.errors = errors;
    out.ns_per_op = total_ns / static_cast<double>(iterations);
    out.ops_per_sec = (static_cast<double>(iterations) * 1e9) / total_ns;
    out.p50_us = percentile_ns(latencies_ns, 50.0) / 1000.0;
    out.p95_us = percentile_ns(latencies_ns, 95.0) / 1000.0;
    out.p99_us = percentile_ns(latencies_ns, 99.0) / 1000.0;
    out.p999_us = percentile_ns(latencies_ns, 99.9) / 1000.0;
    return out;
}

static void print_result(const bench_result& r) {
    if (r.ns_per_op < 1000.0) {
        std::printf("  %-45s %8.1f ns    %12.0f ops/sec\n", r.name, r.ns_per_op, r.ops_per_sec);
    } else {
        std::printf(
            "  %-45s %8.2f us    %12.0f ops/sec\n", r.name, r.ns_per_op / 1000.0, r.ops_per_sec);
    }

    const double error_pct =
        (r.iterations == 0)
            ? 0.0
            : (100.0 * static_cast<double>(r.errors) / static_cast<double>(r.iterations));
    std::printf("      p50: %.3f us | p95: %.3f us | p99: %.3f us | p999: %.3f us | errors: %.3f%% "
                "(%llu/%zu)\n",
                r.p50_us,
                r.p95_us,
                r.p99_us,
                r.p999_us,
                error_pct,
                static_cast<unsigned long long>(r.errors),
                r.iterations);
}

static request make_request(method m, std::string_view uri, std::string_view body = {}) {
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

} // namespace

int main() {
    std::printf("=== Benchmark API Framework Benchmark ===\n\n");

    framework_handler handler;
    auto r = generated::make_fast_router(handler);

    auto dispatch = [&](const request& req, request_context& ctx, response& out) -> result<void> {
        auto status = r.dispatch_to(req, ctx, out);
        if (!status) {
            out.assign_error(problem_details::internal_server_error());
            return status;
        }
        return {};
    };

    request req_create = make_request(
        method::post,
        "/items",
        R"({"name":"Drill","description":"Cordless drill","price":119.99,"stock":25,"category":"tools","tags":["power","workshop"]})");
    req_create.headers.set_view("X-Request-Id", "550e8400-e29b-41d4-a716-446655440001");
    req_create.headers.set(field::cookie, "session=bench-session");

    std::vector<request> valid_workload;
    valid_workload.reserve(9);
    valid_workload.push_back(
        make_request(method::post, "/compute/sum", R"({"values":[1,2,3,4,5,6,7,8,9,10,11,12]})"));
    valid_workload.push_back(
        make_request(method::post,
                     "/compute/stats",
                     R"({"values":[10,20,30,40,50,60,70,80,90,100],"include_median":true})"));
    valid_workload.push_back(make_request(
        method::post,
        "/users/register",
        R"({"username":"bench_user_1","email":"bench1@example.com","password":"StrongPass123","age":30,"role":"moderator","tags":["perf","stage2"]})"));
    valid_workload.push_back(std::move(req_create));
    valid_workload.push_back(make_request(method::get, "/items?limit=20&offset=0&category=tools"));
    valid_workload.push_back(make_request(method::get, "/items/42"));
    valid_workload.push_back(make_request(
        method::put,
        "/items/42",
        R"({"name":"Updated Drill","price":109.99,"stock":20,"tags":["updated","bench"]})"));
    valid_workload.push_back(make_request(
        method::post, "/echo", R"({"message":"benchmark","repeat":3,"uppercase":true})"));
    valid_workload.push_back(make_request(method::get, "/health"));

    request req_invalid_missing_header = make_request(
        method::post, "/items", R"({"name":"NoHeader","price":1.0,"category":"tools"})");
    request req_invalid_register = make_request(
        method::post, "/users/register", R"({"username":"x","email":"bad","password":"123"})");
    request req_invalid_echo =
        make_request(method::post, "/echo", R"({"message":"bad","repeat":1000,"uppercase":false})");
    request req_invalid_path = make_request(method::get, "/items/not-an-int");

    std::vector<request> error_workload;
    error_workload.reserve(4);
    error_workload.push_back(std::move(req_invalid_missing_header));
    error_workload.push_back(std::move(req_invalid_register));
    error_workload.push_back(std::move(req_invalid_echo));
    error_workload.push_back(std::move(req_invalid_path));

    constexpr size_t valid_iterations = 220000;
    constexpr size_t error_iterations = 120000;

    std::printf("--- Generated framework path: valid workload ---\n");
    auto valid_result = run_dispatch_bench(
        "benchmark_api_framework_valid_mix", dispatch, valid_workload, valid_iterations);
    print_result(valid_result);

    std::printf("\n--- Generated framework path: error workload ---\n");
    auto error_result = run_dispatch_bench(
        "benchmark_api_framework_error_mix", dispatch, error_workload, error_iterations);
    print_result(error_result);

    return 0;
}
