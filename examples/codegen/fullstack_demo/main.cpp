// Fullstack demo: a Notes API exercising the whole current KATANA stack —
//   OpenAPI codegen (DTOs, JSON, validators, typed router, enums, free-form metadata,
//   date-time), SQL codegen against live PostgreSQL, x-katana cache + rate-limit policies,
//   built-in /healthz + /readyz (readiness pings the DB) and /metrics, and the date helpers.
//
// Build (standalone):
//   c++ -O2 -std=c++23 -DKATANA_USE_EPOLL -DKATANA_HAS_LIBPQ -I katana/core/include \
//       -I examples/codegen/fullstack_demo main.cpp libkatana_core.a -lpq -pthread -o notes
//   PG_DSN=postgresql://katana@127.0.0.1:5433/katana PORT=8090 ./notes

#include "generated/generated_dtos.hpp"
#include "generated/generated_handlers.hpp"
#include "generated/generated_json.hpp"
#include "generated/generated_router_bindings.hpp"
#include "generated/generated_routes.hpp"
#include "generated/generated_sql_repository.hpp"
#include "generated/generated_validators.hpp"

#include "katana/core/config.hpp"
#include "katana/core/contract_policies.hpp"
#include "katana/core/datetime.hpp"
#include "katana/core/http_server.hpp"
#include "katana/sql/postgres.hpp"

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

using namespace katana::http;
using katana::arena_allocator;
using katana::arena_string;
using katana::monotonic_arena;
namespace sqlgen = katana::sql::generated;

namespace {

arena_string<> astr(monotonic_arena& a, std::string_view v) {
    return arena_string<>(v.begin(), v.end(), arena_allocator<char>(&a));
}

// Map the seven note columns (as text from SQL) into a Note DTO.
void fill_note(Note& n, monotonic_arena& a, int64_t id, std::string_view title,
               std::string_view body, std::string_view priority, std::string_view metadata,
               std::optional<std::string_view> due_date, std::string_view created_at) {
    n.id = id;
    n.title = astr(a, title);
    n.body = astr(a, body);
    if (auto p = Priority_enum_from_string(priority)) {
        n.priority = *p;
    }
    if (!metadata.empty()) {
        n.metadata = astr(a, metadata); // free-form: raw JSON passed through verbatim
    }
    if (due_date && !due_date->empty()) {
        n.due_date = astr(a, *due_date);
    }
    n.created_at = astr(a, created_at);
}

template <typename Row> std::string_view col(const Row& opt) {
    return opt ? std::string_view(opt->data(), opt->size()) : std::string_view{};
}

struct notes_handler : generated::api_handler {
    explicit notes_handler(sqlgen::generated_repository& repo) : repo_(repo) {}

    katana::result<void> create_note(const CreateNoteRequest& body, response& out) override {
        // The body is already parsed + validated (required, length, enum) by the time we're here.
        // Demo the date helper: reject a due_date that isn't real RFC 3339.
        std::string_view due(body.due_date.data(), body.due_date.size());
        if (!katana::parse_rfc3339(due)) {
            out.assign_error(katana::problem_details::bad_request("due_date is not valid RFC 3339"));
            return {};
        }
        const std::string priority_text(to_string(body.priority));
        const std::string metadata_json =
            body.metadata ? std::string(body.metadata->data(), body.metadata->size())
                          : std::string("{}");

        auto row = repo_.create_note(std::string_view(body.title.data(), body.title.size()),
                                     std::string_view(body.body.data(), body.body.size()),
                                     priority_text, metadata_json, due);
        if (!row || !*row) {
            out.assign_error(katana::problem_details::internal_server_error("insert failed"));
            return {};
        }

        monotonic_arena a;
        Note n(&a);
        fill_note(n, a, (*row)->id.value_or(0), std::string_view(body.title.data(), body.title.size()),
                  std::string_view(body.body.data(), body.body.size()), priority_text, metadata_json,
                  due, col((*row)->created_at));
        out.assign_json(serialize_Note(n), 201, "Created");
        return {};
    }

    katana::result<void> get_note(int64_t id, response& out) override {
        auto row = repo_.get_note(id);
        if (!row) {
            out.assign_error(katana::problem_details::internal_server_error("query failed"));
            return {};
        }
        if (!*row) {
            out.assign_error(katana::problem_details::not_found("no such note"));
            return {};
        }
        const auto& r = **row;
        monotonic_arena a;
        Note n(&a);
        fill_note(n, a, r.id.value_or(0), col(r.title), col(r.body), col(r.priority), col(r.metadata),
                  r.due_date ? std::optional<std::string_view>(col(r.due_date)) : std::nullopt,
                  col(r.created_at));
        out.assign_json(serialize_Note(n));
        return {};
    }

    katana::result<void> list_notes(std::optional<std::string_view> priority,
                                    std::optional<int64_t> limit, std::optional<int64_t> offset,
                                    response& out) override {
        const std::string filter(priority.value_or(std::string_view{})); // "" => no filter (SQL NULL-or)
        auto rows = repo_.list_notes(filter, limit.value_or(20), offset.value_or(0));
        if (!rows) {
            out.assign_error(katana::problem_details::internal_server_error("query failed"));
            return {};
        }
        monotonic_arena a;
        NoteList list(&a);
        for (const auto& r : *rows) {
            Note n(&a);
            fill_note(n, a, r.id.value_or(0), col(r.title), col(r.body), col(r.priority),
                      col(r.metadata),
                      r.due_date ? std::optional<std::string_view>(col(r.due_date)) : std::nullopt,
                      col(r.created_at));
            list.notes.push_back(std::move(n));
        }
        list.count = static_cast<int64_t>(list.notes.size());
        out.assign_json(serialize_NoteList(list));
        return {};
    }

private:
    sqlgen::generated_repository& repo_;
};

} // namespace

int main(int argc, char** argv) {
    // Layered config: built-in defaults < optional file (NOTES_CONFIG) < env (KATANA_*) <
    // --flags. So `KATANA_PORT=9000 ./notes --workers 8` or `./notes --pg-dsn ...` all work,
    // with one key space (pg_dsn / port / workers / access_log).
    katana::config::config cfg;
    cfg.defaults({{"pg_dsn", "postgresql://katana@127.0.0.1:5433/katana"},
                  {"port", "8090"},
                  {"workers", "4"},
                  {"access_log", "true"},
                  {"conn_read_timeout_ms", "15000"},
                  {"conn_idle_timeout_ms", "30000"},
                  {"db_statement_timeout_ms", "5000"}});
    if (const char* file = std::getenv("NOTES_CONFIG")) {
        cfg.from_file(file);
    }
    cfg.from_env("KATANA").from_args(argc, argv);

    // Back-compat: the original demo used PG_DSN/PORT/WORKERS without the KATANA_ prefix.
    if (const char* dsn = std::getenv("PG_DSN")) {
        cfg.set("pg_dsn", dsn);
    }
    if (const char* p = std::getenv("PORT")) {
        cfg.set("port", p);
    }
    if (const char* w = std::getenv("WORKERS")) {
        cfg.set("workers", w);
    }

    if (auto problems = cfg.validate({"pg_dsn"}); !problems.empty()) {
        for (const auto& msg : problems) {
            std::cerr << "config: " << msg << "\n";
        }
        return 2;
    }

    const std::string conn(cfg.get_or("pg_dsn", ""));
    const uint16_t port = cfg.get_u16("port", 8090);
    const size_t workers = static_cast<size_t>(cfg.get_int("workers", 4));

    katana::sql::postgres_pool pool(
        {.postgres = {.connection_string = conn,
                      .statement_timeout_ms =
                          static_cast<int>(cfg.get_int("db_statement_timeout_ms", 5000))},
         .executor_count = workers,
         .eager_connect = true});
    if (auto c = pool.connect_all(); !c) {
        std::cerr << "DB connect failed\n";
        return 1;
    }
    katana::sql::postgres_pool_executor pool_executor(pool); // routes to the per-reactor connection
    sqlgen::generated_repository repo(pool_executor);
    notes_handler handler(repo);

    // x-katana policies: response caching (GET /notes) + rate limiting (POST /notes), in-memory.
    in_memory_contract_policy_executor policies;

    std::atomic<bool> db_ready{true};

    const auto& router = generated::make_router(handler);
    const auto read_to = std::chrono::milliseconds(cfg.get_int("conn_read_timeout_ms", 15000));
    const auto idle_to = std::chrono::milliseconds(cfg.get_int("conn_idle_timeout_ms", 30000));

    server(router)
        .policy_executor(policies)
        .access_log(cfg.get_bool("access_log", true),
                    static_cast<uint32_t>(cfg.get_int("access_log_sample", 1))) // 1-in-N sampling
        .connection_timeout(read_to, read_to, idle_to) // slowloris protection
        .tracing(cfg.get_bool("tracing", false))       // W3C trace context + per-request spans
        .reactor_metrics(cfg.get_bool("reactor_metrics", false)) // worker/reactor gauges
        .cors() // permissive CORS for a separate frontend (Next.js) in dev
        .readiness_check([&] {
            // Live readiness: a trivial query through the pool confirms the DB is reachable.
            return db_ready.load() && pool_executor.query("readyz_ping", "SELECT 1", {}).has_value();
        })
        .listen(port)
        .workers(workers)
        .on_start([&] {
            std::cout << "Notes API on :" << port << " (" << workers << " workers) — "
                      << "/notes, /notes/{id}, /healthz, /readyz, /metrics\n";
        })
        .run();
    return 0;
}
