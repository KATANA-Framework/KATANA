#include "bench_utils.hpp"
#include "generated_dtos.hpp"
#include "generated_handlers.hpp"
#include "generated_json.hpp"
#include "generated_router_bindings.hpp"
#include "katana/core/handler_context.hpp"
#include "katana/core/http.hpp"
#include "katana/core/problem.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace katana;
using namespace katana::http;
using bench_util::do_not_optimize;

namespace {

struct stored_item {
    int64_t id = 0;
    std::string name;
    std::string description;
    double price = 0.0;
    int64_t stock = 0;
    ItemCategory_enum category = ItemCategory_enum::other;
    std::vector<std::string> tags;
};

class item_store {
public:
    void seed(size_t n) {
        for (size_t i = 0; i < n; ++i) {
            stored_item it;
            it.id = next_id_++;
            it.name = "Seed-" + std::to_string(i + 1);
            it.description = "Seed item";
            it.price = 10.0 + static_cast<double>(i % 100);
            it.stock = 100 + static_cast<int64_t>(i % 50);
            it.category = ItemCategory_enum::tools;
            it.tags = {"seed", "bench"};
            items_[it.id] = std::move(it);
        }
    }

    int64_t create(const CreateItemRequest& body) {
        stored_item it;
        it.id = next_id_++;
        it.name.assign(body.name.data(), body.name.size());
        if (!body.description.empty()) {
            it.description.assign(body.description.data(), body.description.size());
        }
        it.price = body.price;
        it.stock = body.stock;
        it.category = body.category;
        for (const auto& tag : body.tags) {
            it.tags.emplace_back(tag.data(), tag.size());
        }
        int64_t id = it.id;
        items_[id] = std::move(it);
        return id;
    }

    const stored_item* get(int64_t id) const {
        auto it = items_.find(id);
        if (it == items_.end()) {
            return nullptr;
        }
        return &it->second;
    }

    bool update(int64_t id, const UpdateItemRequest& body) {
        auto it = items_.find(id);
        if (it == items_.end()) {
            return false;
        }
        if (!body.name.empty()) {
            it->second.name.assign(body.name.data(), body.name.size());
        }
        if (!body.description.empty()) {
            it->second.description.assign(body.description.data(), body.description.size());
        }
        if (body.price > 0.0) {
            it->second.price = body.price;
        }
        if (body.stock > 0) {
            it->second.stock = body.stock;
        }
        if (!body.tags.empty()) {
            it->second.tags.clear();
            for (const auto& tag : body.tags) {
                it->second.tags.emplace_back(tag.data(), tag.size());
            }
        }
        return true;
    }

    bool remove(int64_t id) { return items_.erase(id) > 0; }

    std::vector<const stored_item*> list(size_t limit,
                                         size_t offset,
                                         std::optional<std::string_view> category) const {
        std::optional<ItemCategory_enum> cat;
        if (category) {
            cat = ItemCategory_enum_from_string(*category);
            if (!cat) {
                return {};
            }
        }

        std::vector<const stored_item*> out;
        out.reserve(limit);

        size_t seen = 0;
        for (const auto& kv : items_) {
            const auto& item = kv.second;
            if (cat && item.category != *cat) {
                continue;
            }
            if (seen++ < offset) {
                continue;
            }
            out.push_back(&item);
            if (out.size() >= limit) {
                break;
            }
        }
        return out;
    }

    int64_t size() const { return static_cast<int64_t>(items_.size()); }

private:
    std::unordered_map<int64_t, stored_item> items_;
    int64_t next_id_ = 1;
};

static Item to_item_dto(const stored_item& src, monotonic_arena& arena) {
    Item item(&arena);
    item.id = src.id;
    item.name = arena_string<>(src.name, arena_allocator<char>(&arena));
    if (!src.description.empty()) {
        item.description = arena_string<>(src.description, arena_allocator<char>(&arena));
    }
    item.price = src.price;
    item.stock = src.stock;
    item.category = src.category;
    for (const auto& tag : src.tags) {
        item.tags.emplace_back(tag, arena_allocator<char>(&arena));
    }
    return item;
}

class benchmark_handler : public generated::api_handler {
public:
    explicit benchmark_handler(item_store& store) : store_(store) {}

    response compute_sum(const SumRequest& body) override {
        double sum = 0.0;
        for (double v : body.values) {
            sum += v;
        }
        auto& arena = handler_context::arena();
        SumResponse resp(&arena);
        resp.result = sum;
        resp.count = static_cast<int64_t>(body.values.size());
        return response::json(serialize_SumResponse(resp));
    }

    response compute_stats(const StatsRequest& body) override {
        if (body.values.empty()) {
            return response::error(problem_details::bad_request("empty values"));
        }

        double sum = 0.0;
        double mn = body.values[0];
        double mx = body.values[0];
        for (double v : body.values) {
            sum += v;
            mn = std::min(mn, v);
            mx = std::max(mx, v);
        }

        auto& arena = handler_context::arena();
        StatsResponse resp(&arena);
        resp.min = mn;
        resp.max = mx;
        resp.sum = sum;
        resp.mean = sum / static_cast<double>(body.values.size());
        resp.count = static_cast<int64_t>(body.values.size());
        if (body.include_median) {
            std::vector<double> sorted(body.values.begin(), body.values.end());
            std::sort(sorted.begin(), sorted.end());
            const size_t n = sorted.size();
            resp.median =
                (n % 2 == 0) ? ((sorted[n / 2 - 1] + sorted[n / 2]) * 0.5) : sorted[n / 2];
        }
        return response::json(serialize_StatsResponse(resp));
    }

    response register_user(const RegisterRequest& body) override {
        auto& arena = handler_context::arena();
        UserResponse resp(&arena);
        resp.id = arena_string<>("550e8400-e29b-41d4-a716-446655440000", arena_allocator<char>(&arena));
        resp.username =
            arena_string<>(body.username.data(), body.username.size(), arena_allocator<char>(&arena));
        resp.email = arena_string<>(body.email.data(), body.email.size(), arena_allocator<char>(&arena));
        resp.role = body.role;
        resp.created_at = arena_string<>("2026-01-01T00:00:00Z", arena_allocator<char>(&arena));
        auto out = response::json(serialize_UserResponse(resp));
        out.status = 201;
        out.reason = "Created";
        return out;
    }

    response list_items(std::optional<int64_t> limit,
                        std::optional<int64_t> offset,
                        std::optional<std::string_view> category) override {
        const size_t lim = static_cast<size_t>(limit.value_or(20));
        const size_t off = static_cast<size_t>(offset.value_or(0));
        auto list = store_.list(lim, off, category);

        auto& arena = handler_context::arena();
        ItemList resp(&arena);
        resp.total = store_.size();
        resp.limit = static_cast<int64_t>(lim);
        resp.offset = static_cast<int64_t>(off);
        for (const auto* item : list) {
            resp.items.push_back(to_item_dto(*item, arena));
        }
        return response::json(serialize_ItemList(resp));
    }

    response create_item([[maybe_unused]] std::string_view X_Request_Id,
                         [[maybe_unused]] std::optional<std::string_view> session,
                         const CreateItemRequest& body) override {
        int64_t id = store_.create(body);
        const auto* item = store_.get(id);
        if (!item) {
            return response::error(problem_details::internal_server_error());
        }
        auto& arena = handler_context::arena();
        auto out = response::json(serialize_Item(to_item_dto(*item, arena)));
        out.status = 201;
        out.reason = "Created";
        return out;
    }

    response get_item(int64_t id) override {
        const auto* item = store_.get(id);
        if (!item) {
            return response::error(problem_details::not_found("item not found"));
        }
        auto& arena = handler_context::arena();
        return response::json(serialize_Item(to_item_dto(*item, arena)));
    }

    response update_item(int64_t id, const UpdateItemRequest& body) override {
        if (!store_.update(id, body)) {
            return response::error(problem_details::not_found("item not found"));
        }
        const auto* item = store_.get(id);
        if (!item) {
            return response::error(problem_details::internal_server_error());
        }
        auto& arena = handler_context::arena();
        return response::json(serialize_Item(to_item_dto(*item, arena)));
    }

    response delete_item(int64_t id) override {
        if (!store_.remove(id)) {
            return response::error(problem_details::not_found("item not found"));
        }
        response out;
        out.status = 204;
        out.reason = "No Content";
        return out;
    }

    response echo(const EchoRequest& body) override {
        std::string payload(body.message.data(), body.message.size());
        const int repeat_count = static_cast<int>(body.repeat);
        if (repeat_count > 1) {
            std::string base = payload;
            payload.reserve(base.size() * static_cast<size_t>(repeat_count));
            for (int i = 1; i < repeat_count; ++i) {
                payload += base;
            }
        }
        if (body.uppercase) {
            for (char& ch : payload) {
                ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
            }
        }

        auto& arena = handler_context::arena();
        EchoResponse resp(&arena);
        resp.message = arena_string<>(payload, arena_allocator<char>(&arena));
        resp.length = static_cast<int64_t>(payload.size());
        return response::json(serialize_EchoResponse(resp));
    }

    response health_check() override { return response::json(R"({"status":"ok","uptime_ms":1234})"); }

private:
    item_store& store_;
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
        auto resp = dispatch(req, ctx);
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

        const auto t0 = std::chrono::steady_clock::now();
        auto resp = dispatch(req, ctx);
        const auto t1 = std::chrono::steady_clock::now();

        if (resp.status >= 400) {
            ++errors;
        }
        latencies_ns.push_back(
            static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count()));
        do_not_optimize(resp.status);
    }
    const auto run_end = std::chrono::steady_clock::now();

    const double total_ns =
        static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(run_end - run_start).count());
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
        std::printf("  %-45s %8.2f us    %12.0f ops/sec\n", r.name, r.ns_per_op / 1000.0, r.ops_per_sec);
    }

    const double error_pct =
        (r.iterations == 0) ? 0.0 : (100.0 * static_cast<double>(r.errors) / static_cast<double>(r.iterations));
    std::printf("      p50: %.3f us | p95: %.3f us | p99: %.3f us | p999: %.3f us | errors: %.3f%% (%llu/%zu)\n",
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
    std::printf("=== Benchmark API Codegen Benchmark ===\n\n");

    item_store store;
    store.seed(256);
    benchmark_handler handler(store);
    const auto& r = generated::make_router(handler);

    auto dispatch = [&](const request& req, request_context& ctx) -> response {
        return dispatch_or_problem(r, req, ctx);
    };

    request req_create =
        make_request(method::post,
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
    valid_workload.push_back(
        make_request(method::post,
                     "/users/register",
                     R"({"username":"bench_user_1","email":"bench1@example.com","password":"StrongPass123","age":30,"role":"moderator","tags":["perf","stage2"]})"));
    valid_workload.push_back(std::move(req_create));
    valid_workload.push_back(make_request(method::get, "/items?limit=20&offset=0&category=tools"));
    valid_workload.push_back(make_request(method::get, "/items/42"));
    valid_workload.push_back(make_request(method::put,
                                          "/items/42",
                                          R"({"name":"Updated Drill","price":109.99,"stock":20,"tags":["updated","bench"]})"));
    valid_workload.push_back(
        make_request(method::post, "/echo", R"({"message":"benchmark","repeat":3,"uppercase":true})"));
    valid_workload.push_back(make_request(method::get, "/health"));

    request req_invalid_missing_header =
        make_request(method::post,
                     "/items",
                     R"({"name":"NoHeader","price":1.0,"category":"tools"})");
    request req_invalid_register =
        make_request(method::post,
                     "/users/register",
                     R"({"username":"x","email":"bad","password":"123"})");
    request req_invalid_echo =
        make_request(method::post,
                     "/echo",
                     R"({"message":"bad","repeat":1000,"uppercase":false})");
    request req_invalid_path = make_request(method::get, "/items/not-an-int");

    std::vector<request> error_workload;
    error_workload.reserve(4);
    error_workload.push_back(std::move(req_invalid_missing_header));
    error_workload.push_back(std::move(req_invalid_register));
    error_workload.push_back(std::move(req_invalid_echo));
    error_workload.push_back(std::move(req_invalid_path));

    constexpr size_t valid_iterations = 220000;
    constexpr size_t error_iterations = 120000;

    std::printf("--- Generated stack: mixed valid workload ---\n");
    auto valid_result =
        run_dispatch_bench("benchmark_api_codegen_valid_mix", dispatch, valid_workload, valid_iterations);
    print_result(valid_result);

    std::printf("\n--- Generated stack: validation/error workload ---\n");
    auto error_result =
        run_dispatch_bench("benchmark_api_codegen_error_mix", dispatch, error_workload, error_iterations);
    print_result(error_result);

    std::printf("\n=== Benchmark Complete ===\n");
    return 0;
}
