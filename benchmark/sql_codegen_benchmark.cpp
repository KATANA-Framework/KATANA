#include "bench_utils.hpp"
#include "katana_gen/sql_codegen.hpp"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace {

struct benchmark_fixture {
    fs::path dir;
    std::vector<katana_gen::sql_source> sources;

    benchmark_fixture() {
        dir = fs::temp_directory_path() / "katana_sql_codegen_bench";
        fs::create_directories(dir);
        add_source("get_user.sql",
                   R"(-- name: get_user :one
SELECT id::bigint AS id, name::text AS name, active::bool AS active
FROM users
WHERE id = $1::bigint;
)"); 
        add_source("list_users.sql",
                   R"(-- name: list_users :many
SELECT id::bigint AS id, name::text AS name
FROM users
WHERE active = $1::bool
ORDER BY id;
)"); 
        add_source("touch_user.sql",
                   R"(-- name: touch_user :exec
UPDATE users SET last_seen_at = NOW() WHERE id = $1::bigint;
)"); 
    }

    ~benchmark_fixture() {
        std::error_code ec;
        fs::remove_all(dir, ec);
    }

    void add_source(const std::string& name, const std::string& sql) {
        sources.push_back(katana_gen::sql_source{.path = name, .content = sql});
        std::ofstream out(dir / name);
        out << sql;
    }
};

template <typename Fn> double run_bench(int iterations, Fn&& fn) {
    for (int i = 0; i < iterations / 10; ++i) {
        bench_util::do_not_optimize(fn());
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::size_t bytes = 0;
    for (int i = 0; i < iterations; ++i) {
        bytes += fn();
    }
    auto end = std::chrono::high_resolution_clock::now();
    bench_util::do_not_optimize(bytes);

    const auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    return static_cast<double>(elapsed_ns.count()) / static_cast<double>(iterations);
}

} // namespace

int main() {
    benchmark_fixture fixture;
    constexpr int iterations = 5000;

    const double parse_ns = run_bench(iterations, [&]() -> std::size_t {
        auto catalog = katana_gen::load_sql_catalog_from_sources(fixture.sources);
        if (!catalog) {
            return 0;
        }
        return catalog->queries.size();
    });

    auto catalog = katana_gen::load_sql_catalog(fixture.dir);
    if (!catalog) {
        std::fprintf(stderr, "sql_codegen_benchmark: failed to load fixture\n");
        return 1;
    }

    const double generate_ns = run_bench(iterations, [&]() -> std::size_t {
        auto models = katana_gen::generate_sql_models(*catalog);
        auto repo = katana_gen::generate_sql_repository(*catalog);
        return models.size() + repo.size();
    });

    std::printf("SQL Codegen Benchmarks\n");
    std::printf("  %-40s %8.2f us    %12.0f ops/sec\n",
                "sql catalog parse",
                parse_ns / 1000.0,
                1e9 / parse_ns);
    std::printf("  %-40s %8.2f us    %12.0f ops/sec\n",
                "sql model+repository generation",
                generate_ns / 1000.0,
                1e9 / generate_ns);
    return 0;
}
