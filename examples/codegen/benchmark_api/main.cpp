// KATANA Benchmark API — unified service for benchmarking all codegen paths.
//
// Exercises: multiple endpoints, JSON parse/serialize, format validators,
// enum types, path/query/header/cookie params, arena DTOs, nested objects.
// Run with: ./benchmark_api   (default port 9090)
//           PORT=8080 ./benchmark_api

#include "generated/generated_dtos.hpp"
#include "generated/generated_handlers.hpp"
#include "generated/generated_json.hpp"
#include "generated/generated_router_bindings.hpp"
#include "generated/generated_routes.hpp"
#include "generated/generated_validators.hpp"
#include "katana/core/http_server.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <numeric>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace katana;
using namespace katana::http;

// ============================================================
// In-memory item store (thread-safe)
// ============================================================

class item_store {
public:
    struct stored_item {
        int64_t id;
        std::string name;
        std::string description;
        double price;
        int64_t stock;
        ItemCategory_enum category;
        std::vector<std::string> tags;
    };

    int64_t create(const CreateItemRequest& req) {
        std::lock_guard lock(mu_);
        stored_item item;
        item.id = next_id_++;
        item.name.assign(req.name.data(), req.name.size());
        if (!req.description.empty())
            item.description.assign(req.description.data(), req.description.size());
        item.price = req.price;
        item.stock = req.stock;
        item.category = req.category;
        if (!req.tags.empty()) {
            for (const auto& t : req.tags)
                item.tags.emplace_back(t.data(), t.size());
        }
        int64_t id = item.id;
        items_[id] = std::move(item);
        return id;
    }

    std::optional<stored_item> get(int64_t id) const {
        std::lock_guard lock(mu_);
        auto it = items_.find(id);
        if (it == items_.end())
            return std::nullopt;
        return it->second;
    }

    std::vector<stored_item> list(int limit, int offset) const {
        std::lock_guard lock(mu_);
        std::vector<stored_item> result;
        int skip = 0;
        for (const auto& [_, item] : items_) {
            if (skip++ < offset)
                continue;
            if (static_cast<int>(result.size()) >= limit)
                break;
            result.push_back(item);
        }
        return result;
    }

    int64_t count() const {
        std::lock_guard lock(mu_);
        return static_cast<int64_t>(items_.size());
    }

    bool update(int64_t id, const UpdateItemRequest& req) {
        std::lock_guard lock(mu_);
        auto it = items_.find(id);
        if (it == items_.end())
            return false;
        auto& item = it->second;
        if (!req.name.empty())
            item.name.assign(req.name.data(), req.name.size());
        if (!req.description.empty())
            item.description.assign(req.description.data(), req.description.size());
        if (req.price != 0.0)
            item.price = req.price;
        if (req.stock != 0)
            item.stock = req.stock;
        if (!req.tags.empty()) {
            item.tags.clear();
            for (const auto& t : req.tags)
                item.tags.emplace_back(t.data(), t.size());
        }
        return true;
    }

    bool remove(int64_t id) {
        std::lock_guard lock(mu_);
        return items_.erase(id) > 0;
    }

private:
    mutable std::mutex mu_;
    std::unordered_map<int64_t, stored_item> items_;
    int64_t next_id_ = 1;
};

// ============================================================
// Helper: convert stored_item -> Item DTO (arena-allocated)
// ============================================================

static Item to_dto(const item_store::stored_item& src, monotonic_arena& arena) {
    Item item(&arena);
    item.id = src.id;
    item.name = arena_string<>(src.name, arena_allocator<char>(&arena));
    if (!src.description.empty())
        item.description = arena_string<>(src.description, arena_allocator<char>(&arena));
    item.price = src.price;
    item.stock = src.stock;
    item.category = src.category;
    if (!src.tags.empty()) {
        for (const auto& t : src.tags)
            item.tags.emplace_back(t, arena_allocator<char>(&arena));
    }
    return item;
}

// ============================================================
// Handler implementation
// ============================================================

class benchmark_handler : public generated::api_handler {
public:
    explicit benchmark_handler(item_store& store)
        : store_(store), start_time_(std::chrono::steady_clock::now()) {}

    // --- Compute ---

    response compute_sum(const SumRequest& req) override {
        double sum = 0.0;
        for (double v : req.values)
            sum += v;

        auto& arena = handler_context::arena();
        SumResponse resp(&arena);
        resp.result = sum;
        resp.count = static_cast<int64_t>(req.values.size());
        return response::json(serialize_SumResponse(resp));
    }

    response compute_stats(const StatsRequest& req) override {
        const auto& vals = req.values;
        double sum = 0.0, mn = vals[0], mx = vals[0];
        for (double v : vals) {
            sum += v;
            if (v < mn)
                mn = v;
            if (v > mx)
                mx = v;
        }

        auto& arena = handler_context::arena();
        StatsResponse resp(&arena);
        resp.min = mn;
        resp.max = mx;
        resp.mean = sum / static_cast<double>(vals.size());
        resp.sum = sum;
        resp.count = static_cast<int64_t>(vals.size());

        if (req.include_median) {
            std::vector<double> sorted(vals.begin(), vals.end());
            std::sort(sorted.begin(), sorted.end());
            size_t n = sorted.size();
            resp.median = (n % 2 == 0) ? (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0 : sorted[n / 2];
        }

        return response::json(serialize_StatsResponse(resp));
    }

    // --- User registration (validation path) ---

    response register_user(const RegisterRequest& req) override {
        // Validation already performed by generated code.
        auto& arena = handler_context::arena();
        UserResponse resp(&arena);
        resp.id =
            arena_string<>("550e8400-e29b-41d4-a716-446655440000", arena_allocator<char>(&arena));
        resp.username =
            arena_string<>(req.username.data(), req.username.size(), arena_allocator<char>(&arena));
        resp.email =
            arena_string<>(req.email.data(), req.email.size(), arena_allocator<char>(&arena));
        resp.role = req.role;
        resp.created_at = arena_string<>("2025-01-01T00:00:00Z", arena_allocator<char>(&arena));
        return response::json(serialize_UserResponse(resp));
    }

    // --- CRUD ---

    response list_items([[maybe_unused]] std::optional<int64_t> limit,
                        [[maybe_unused]] std::optional<int64_t> offset,
                        [[maybe_unused]] std::optional<std::string_view> category) override {
        int lim = static_cast<int>(limit.value_or(20));
        int off = static_cast<int>(offset.value_or(0));

        auto items = store_.list(lim, off);
        int64_t total = store_.count();

        auto& arena = handler_context::arena();
        ItemList list(&arena);
        list.total = total;
        list.limit = lim;
        list.offset = off;
        for (const auto& si : items)
            list.items.push_back(to_dto(si, arena));

        return response::json(serialize_ItemList(list));
    }

    response create_item([[maybe_unused]] std::string_view X_Request_Id,
                         [[maybe_unused]] std::optional<std::string_view> session,
                         const CreateItemRequest& body) override {
        int64_t id = store_.create(body);
        auto item_opt = store_.get(id);
        if (!item_opt)
            return response::error(problem_details::internal_server_error());

        auto& arena = handler_context::arena();
        Item dto = to_dto(*item_opt, arena);
        return response::json(serialize_Item(dto));
    }

    response get_item(int64_t id) override {
        auto item_opt = store_.get(id);
        if (!item_opt)
            return response::error(problem_details::not_found("Item not found"));

        auto& arena = handler_context::arena();
        return response::json(serialize_Item(to_dto(*item_opt, arena)));
    }

    response update_item(int64_t id, const UpdateItemRequest& body) override {
        if (!store_.update(id, body))
            return response::error(problem_details::not_found("Item not found"));

        auto item_opt = store_.get(id);
        auto& arena = handler_context::arena();
        return response::json(serialize_Item(to_dto(*item_opt, arena)));
    }

    response delete_item(int64_t id) override {
        if (!store_.remove(id))
            return response::error(problem_details::not_found("Item not found"));
        return response::ok();
    }

    // --- Echo (minimal overhead path) ---

    response echo(const EchoRequest& req) override {
        std::string msg(req.message.data(), req.message.size());

        int repeat_count = static_cast<int>(req.repeat);
        if (repeat_count > 1) {
            std::string base = msg;
            msg.reserve(base.size() * static_cast<size_t>(repeat_count));
            for (int i = 1; i < repeat_count; ++i)
                msg += base;
        }

        if (req.uppercase) {
            for (char& c : msg)
                c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }

        auto& arena = handler_context::arena();
        EchoResponse resp(&arena);
        resp.message = arena_string<>(msg, arena_allocator<char>(&arena));
        resp.length = static_cast<int64_t>(msg.size());
        return response::json(serialize_EchoResponse(resp));
    }

    // --- Health ---

    response health_check() override {
        auto now = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time_).count();
        std::string json = R"({"status":"ok","uptime_ms":)" + std::to_string(ms) + "}";
        return response::json(json);
    }

private:
    item_store& store_;
    std::chrono::steady_clock::time_point start_time_;
};

// ============================================================
// Main
// ============================================================

static uint16_t read_port() {
    if (const char* v = std::getenv("PORT")) {
        int p = std::atoi(v);
        if (p > 0 && p < 65536)
            return static_cast<uint16_t>(p);
    }
    return 9090;
}

int main() {
    item_store store;
    benchmark_handler handler(store);
    const auto& router = generated::make_router(handler);

    const uint16_t port = read_port();
    const uint16_t workers = static_cast<uint16_t>(
        std::min<uint32_t>(std::max(1u, std::thread::hardware_concurrency()), 64));

    std::cout << "KATANA Benchmark API on :" << port << " (" << workers << " workers)\n"
              << "Endpoints:\n"
              << "  POST /compute/sum        - sum array of doubles\n"
              << "  POST /compute/stats      - min/max/mean/median\n"
              << "  POST /users/register     - validation-heavy path\n"
              << "  GET  /items              - list with pagination\n"
              << "  POST /items              - create (header: X-Request-Id)\n"
              << "  GET  /items/{id}         - get by id\n"
              << "  PUT  /items/{id}         - update\n"
              << "  DEL  /items/{id}         - delete\n"
              << "  POST /echo               - echo with optional transform\n"
              << "  GET  /health             - health check\n";

    return http::server(router)
        .listen(port)
        .workers(workers)
        .on_start([&]() { std::cout << "Server ready." << std::endl; })
        .run();
}
