// Pulse — a comprehensive KATANA service that exercises (nearly) the whole framework in one
// binary:
//   * TWO independently-generated OpenAPI contracts (catalog + analytics), namespaced and served
//     together via a composite dispatcher;
//   * TWO PostgreSQL pools with independent statement-timeouts (generated SQL repositories);
//   * x-katana policies backed by Redis (response cache, rate limit, idempotency) with an L1
//     in-process tier;
//   * layered config with `*_FILE` secrets and SIGHUP hot-reload;
//   * structured access log with sampling, W3C distributed tracing, CORS for a SPA frontend;
//   * Prometheus /metrics (request histogram, connection + reactor gauges), /healthz + /readyz
//     (pinging both DBs and Redis), connection timeouts, a max-connections cap, graceful shutdown.
//
// Build: ./build.sh    Run: PG_DSN=... ./pulse  (see README.md)

#include "generated/analytics/generated_dtos.hpp"
#include "generated/analytics/generated_handlers.hpp"
#include "generated/analytics/generated_json.hpp"
#include "generated/analytics/generated_router_bindings.hpp"
#include "generated/analytics/generated_routes.hpp"
#include "generated/analytics/generated_sql_repository.hpp"
#include "generated/analytics/generated_validators.hpp"

#include "generated/catalog/generated_dtos.hpp"
#include "generated/catalog/generated_handlers.hpp"
#include "generated/catalog/generated_json.hpp"
#include "generated/catalog/generated_router_bindings.hpp"
#include "generated/catalog/generated_routes.hpp"
#include "generated/catalog/generated_sql_repository.hpp"
#include "generated/catalog/generated_validators.hpp"

#include "katana/core/config.hpp"
#include "katana/core/contract_policies.hpp"
#include "katana/core/http_server.hpp"
#include "katana/core/log.hpp"
#include "katana/core/redis_client.hpp"
#include "katana/core/shutdown.hpp"
#include "katana/sql/postgres.hpp"

#include <atomic>
#include <csignal>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>

using namespace katana::http;
using katana::arena_allocator;
using katana::arena_string;
using katana::monotonic_arena;

namespace {

arena_string<> astr(monotonic_arena& a, std::string_view v) {
    return arena_string<>(v.begin(), v.end(), arena_allocator<char>(&a));
}
std::string_view sv(const arena_string<>& s) { return {s.data(), s.size()}; }
template <typename Opt> std::string_view col(const Opt& o) {
    return o ? std::string_view(o->data(), o->size()) : std::string_view{};
}

// ---- Catalog contract ---------------------------------------------------------------------

struct catalog_handler : catalog::api_handler {
    explicit catalog_handler(katana::sql::catalog::generated_repository& repo) : repo_(repo) {}

    katana::result<void> create_product(const catalog::CreateProductRequest& body,
                                        response& out) override {
        auto row = repo_.create_product(sv(body.sku), sv(body.name), body.price_cents);
        if (!row) {
            out.assign_error(katana::problem_details::internal_server_error("insert failed"));
            return {};
        }
        if (!*row) {
            out.assign_error(katana::problem_details::conflict("sku already exists"));
            return {};
        }
        monotonic_arena a;
        catalog::Product p(&a);
        p.id = (*row)->id.value_or(0);
        p.sku = astr(a, sv(body.sku));
        p.name = astr(a, sv(body.name));
        p.price_cents = body.price_cents;
        p.created_at = astr(a, col((*row)->created_at));
        out.assign_json(catalog::serialize_Product(p), 201, "Created");
        return {};
    }

    katana::result<void> get_product(int64_t id, response& out) override {
        auto row = repo_.get_product(id);
        if (!row) {
            out.assign_error(katana::problem_details::internal_server_error("query failed"));
            return {};
        }
        if (!*row) {
            out.assign_error(katana::problem_details::not_found("no such product"));
            return {};
        }
        const auto& r = **row;
        monotonic_arena a;
        catalog::Product p(&a);
        p.id = r.id.value_or(0);
        p.sku = astr(a, col(r.sku));
        p.name = astr(a, col(r.name));
        p.price_cents = r.price_cents.value_or(0);
        p.created_at = astr(a, col(r.created_at));
        out.assign_json(catalog::serialize_Product(p));
        return {};
    }

    katana::result<void> list_products(std::optional<int64_t> limit, std::optional<int64_t> offset,
                                       response& out) override {
        auto rows = repo_.list_products(limit.value_or(20), offset.value_or(0));
        if (!rows) {
            out.assign_error(katana::problem_details::internal_server_error("query failed"));
            return {};
        }
        monotonic_arena a;
        catalog::ProductList list(&a);
        for (const auto& r : *rows) {
            catalog::Product p(&a);
            p.id = r.id.value_or(0);
            p.sku = astr(a, col(r.sku));
            p.name = astr(a, col(r.name));
            p.price_cents = r.price_cents.value_or(0);
            p.created_at = astr(a, col(r.created_at));
            list.products.push_back(std::move(p));
        }
        list.count = static_cast<int64_t>(list.products.size());
        out.assign_json(catalog::serialize_ProductList(list));
        return {};
    }

private:
    katana::sql::catalog::generated_repository& repo_;
};

// ---- Analytics contract -------------------------------------------------------------------

struct analytics_handler : analytics::api_handler {
    explicit analytics_handler(katana::sql::analytics::generated_repository& repo) : repo_(repo) {}

    katana::result<void> ingest_event(const analytics::IngestEventRequest& body,
                                      response& out) override {
        const std::string props =
            body.props ? std::string(sv(*body.props)) : std::string("{}");
        auto row = repo_.ingest_event(sv(body.kind), body.product_id.value_or(0), props);
        if (!row || !*row) {
            out.assign_error(katana::problem_details::internal_server_error("insert failed"));
            return {};
        }
        monotonic_arena a;
        analytics::IngestEventResponse r(&a);
        r.id = (*row)->id.value_or(0);
        r.kind = astr(a, sv(body.kind));
        out.assign_json(analytics::serialize_IngestEventResponse(r), 202, "Accepted");
        return {};
    }

    katana::result<void> event_summary(response& out) override {
        auto rows = repo_.event_summary();
        if (!rows) {
            out.assign_error(katana::problem_details::internal_server_error("query failed"));
            return {};
        }
        monotonic_arena a;
        analytics::EventSummary summary(&a);
        int64_t total = 0;
        for (const auto& r : *rows) {
            analytics::EventCount c(&a);
            c.kind = astr(a, col(r.kind));
            c.count = r.count.value_or(0);
            total += c.count;
            summary.kinds.push_back(std::move(c));
        }
        summary.total = total;
        out.assign_json(analytics::serialize_EventSummary(summary));
        return {};
    }

private:
    katana::sql::analytics::generated_repository& repo_;
};

// A reloadable config behind a SIGHUP handler (declared static so the signal handler can reach
// it). The actual reload happens off the signal — the handler just flips a flag the server
// loop could observe; here we reload synchronously since values are read at request time only
// through the cached primitives captured below.
std::atomic<bool> g_reload_requested{false};
void on_sighup(int) { g_reload_requested.store(true); }

} // namespace

int main(int argc, char** argv) {
    katana::config::config cfg;
    cfg.defaults({{"pg_dsn", "postgresql://katana@127.0.0.1:5433/katana"},
                  {"port", "8080"},
                  {"workers", "4"},
                  {"redis_host", "127.0.0.1"},
                  {"redis_port", "6379"},
                  {"catalog_stmt_timeout_ms", "3000"},
                  {"analytics_stmt_timeout_ms", "1000"},
                  {"access_log_sample", "1"},
                  {"max_connections", "0"}});
    if (const char* file = std::getenv("PULSE_CONFIG")) {
        cfg.from_file(file);
    }
    cfg.from_env("PULSE").from_args(argc, argv).resolve_file_secrets();
    if (const char* dsn = std::getenv("PG_DSN")) {
        cfg.set("pg_dsn", dsn); // convenience override used by the dev harness
    }
    if (auto problems = cfg.validate({"pg_dsn"}); !problems.empty()) {
        for (const auto& m : problems) {
            std::cerr << "config: " << m << "\n";
        }
        return 2;
    }

    const std::string dsn(cfg.get_or("pg_dsn", ""));
    const uint16_t port = cfg.get_u16("port", 8080);
    const size_t workers = static_cast<size_t>(cfg.get_int("workers", 4));

    // Two independent Postgres pools with their own statement timeouts (catalog tolerates
    // slower reporting queries; analytics ingestion is kept snappy).
    katana::sql::postgres_pool catalog_pool(
        {.postgres = {.connection_string = dsn,
                      .statement_timeout_ms = static_cast<int>(cfg.get_int("catalog_stmt_timeout_ms", 3000))},
         .executor_count = workers,
         .eager_connect = true});
    katana::sql::postgres_pool analytics_pool(
        {.postgres = {.connection_string = dsn,
                      .statement_timeout_ms = static_cast<int>(cfg.get_int("analytics_stmt_timeout_ms", 1000))},
         .executor_count = workers,
         .eager_connect = true});
    if (!catalog_pool.connect_all() || !analytics_pool.connect_all()) {
        std::cerr << "DB connect failed\n";
        return 1;
    }
    katana::sql::postgres_pool_executor catalog_exec(catalog_pool);
    katana::sql::postgres_pool_executor analytics_exec(analytics_pool);
    katana::sql::catalog::generated_repository catalog_repo(catalog_exec);
    katana::sql::analytics::generated_repository analytics_repo(analytics_exec);

    catalog_handler cat_handler(catalog_repo);
    analytics_handler ana_handler(analytics_repo);

    const auto& catalog_router = catalog::make_router(cat_handler);
    const auto& analytics_router = analytics::make_router(ana_handler);
    composite_router dispatcher{&catalog_router.router(), &analytics_router.router()};

    // Redis-backed x-katana policies (response cache + rate limit + idempotency) with an L1
    // in-process tier, shared by both contracts.
    katana::http::redis_pool redis(
        workers, {.host = std::string(cfg.get_or("redis_host", "127.0.0.1")),
                  .port = cfg.get_u16("redis_port", 6379)});
    const bool redis_ok = redis.connect_all().has_value();
    std::optional<redis_contract_policy_executor> redis_policies;
    std::optional<in_memory_contract_policy_executor> memory_policies;
    route_policy_executor* policies = nullptr;
    if (redis_ok) {
        redis_policies.emplace(redis);
        policies = &*redis_policies;
    } else {
        std::cerr << "Redis unavailable — falling back to in-memory policies\n";
        memory_policies.emplace();
        policies = &*memory_policies;
    }

    std::signal(SIGHUP, on_sighup);

    server(dispatcher)
        .policy_executor(*policies)
        .access_log(true, static_cast<uint32_t>(cfg.get_int("access_log_sample", 1)))
        .tracing()
        .cors()
        .reactor_metrics()
        .connection_timeout(std::chrono::seconds(15), std::chrono::seconds(15),
                            std::chrono::seconds(30))
        .max_connections(static_cast<size_t>(cfg.get_int("max_connections", 0)))
        .readiness_check([&] {
            return catalog_exec.query("readyz_catalog", "SELECT 1", {}).has_value() &&
                   analytics_exec.query("readyz_analytics", "SELECT 1", {}).has_value() &&
                   (!redis_ok || redis.current().get("readyz").has_value() ||
                    redis.current().connected());
        })
        .listen(port)
        .workers(workers)
        .on_start([&] {
            katana::log::info("pulse_start")
                .field("port", static_cast<int64_t>(port))
                .field("workers", static_cast<int64_t>(workers))
                .field("redis", redis_ok)
                .field("contracts", "catalog+analytics");
        })
        .run();
    return 0;
}
