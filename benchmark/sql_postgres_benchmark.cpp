#include "bench_utils.hpp"
#include "generated_sql_repository.hpp"
#include "katana/sql/postgres.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#ifdef KATANA_HAS_LIBPQ

namespace {

const char* postgres_dsn() {
    return std::getenv("KATANA_TEST_POSTGRES_DSN");
}

struct bench_stats {
    double mean_us = 0.0;
    double p95_us = 0.0;
    double p99_us = 0.0;
};

template <typename Fn> bench_stats run_bench(int iterations, Fn&& fn) {
    std::vector<double> samples_us;
    samples_us.reserve(iterations);

    for (int i = 0; i < std::max(1, iterations / 10); ++i) {
        bench_util::do_not_optimize(fn());
    }

    double total_us = 0.0;
    for (int i = 0; i < iterations; ++i) {
        const auto start = std::chrono::steady_clock::now();
        const auto value = fn();
        const auto end = std::chrono::steady_clock::now();
        bench_util::do_not_optimize(value);

        const double sample_us =
            static_cast<double>(
                std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) /
            1000.0;
        samples_us.push_back(sample_us);
        total_us += sample_us;
    }

    std::sort(samples_us.begin(), samples_us.end());
    const auto percentile = [&](double q) -> double {
        const std::size_t index =
            static_cast<std::size_t>(q * static_cast<double>(samples_us.size() - 1));
        return samples_us[index];
    };

    return bench_stats{
        .mean_us = total_us / static_cast<double>(iterations),
        .p95_us = percentile(0.95),
        .p99_us = percentile(0.99),
    };
}

void print_result(std::string_view name, const bench_stats& stats) {
    std::printf("  %-40.*s %8.2f us    %12.0f ops/sec    p95: %.2f us    p99: %.2f us\n",
                static_cast<int>(name.size()),
                name.data(),
                stats.mean_us,
                1e6 / stats.mean_us,
                stats.p95_us,
                stats.p99_us);
}

katana::result<void> seed_fixture(katana::sql::postgres_executor& executor) {
    if (auto result = executor.exec("bench_create_temp_users",
                                    "CREATE TEMP TABLE IF NOT EXISTS katana_stage4_users ("
                                    "id BIGINT PRIMARY KEY, "
                                    "name TEXT NOT NULL, "
                                    "active BOOLEAN NOT NULL, "
                                    "last_seen_at TIMESTAMPTZ)",
                                    {});
        !result) {
        return std::unexpected(result.error());
    }

    if (auto result =
            executor.exec("bench_truncate_temp_users", "TRUNCATE katana_stage4_users", {});
        !result) {
        return std::unexpected(result.error());
    }

    if (auto result = executor.exec("bench_seed_users",
                                    "INSERT INTO katana_stage4_users (id, name, active) "
                                    "SELECT g, 'user_' || g::text, (g % 2 = 0) "
                                    "FROM generate_series(1, 256) AS g",
                                    {});
        !result) {
        return std::unexpected(result.error());
    }

    return {};
}

} // namespace

int main() {
    const char* dsn = postgres_dsn();
    if (dsn == nullptr || *dsn == '\0') {
        std::printf("[skip] KATANA_TEST_POSTGRES_DSN is not set\n");
        return 0;
    }

    katana::sql::postgres_pool pool({
        .postgres = {.connection_string = dsn},
        .executor_count = 1,
        .eager_connect = true,
    });
    auto connected = pool.connect_all();
    if (!connected) {
        std::fprintf(stderr, "sql_postgres_benchmark: failed to connect pool\n");
        return 1;
    }

    auto& executor = pool.current_executor();
    auto seeded = seed_fixture(executor);
    if (!seeded) {
        std::fprintf(stderr, "sql_postgres_benchmark: failed to seed fixture\n");
        return 1;
    }

    katana::sql::postgres_pool_executor pool_executor(pool);
    katana::sql::generated::generated_repository repo(pool_executor);
    constexpr int iterations = 400;

    const auto get_user_stats = run_bench(iterations, [&]() -> int64_t {
        auto result = repo.get_user(42);
        if (!result || !result->has_value() || !result->value().id.has_value()) {
            return -1;
        }
        return *result->value().id;
    });

    const auto list_users_stats = run_bench(iterations, [&]() -> std::size_t {
        auto result = repo.list_users(true);
        if (!result) {
            return 0;
        }
        return result->size();
    });

    const auto exec_stats = run_bench(iterations, [&]() -> std::size_t {
        auto result = repo.touch_user(42);
        if (!result) {
            return 0;
        }
        return result->affected_rows;
    });

    const auto tx_stats = run_bench(iterations, [&]() -> std::size_t {
        katana::sql::postgres_transaction tx(executor);
        auto begun = tx.begin();
        if (!begun) {
            return 0;
        }
        auto touched = repo.touch_user(7);
        if (!touched) {
            return 0;
        }
        auto rolled_back = tx.rollback();
        if (!rolled_back) {
            return 0;
        }
        return touched->affected_rows;
    });

    std::printf("SQL PostgreSQL Benchmarks\n");
    print_result("postgres repo get_user (1 row)", get_user_stats);
    print_result("postgres repo list_users (128 rows)", list_users_stats);
    print_result("postgres repo touch_user (exec)", exec_stats);
    print_result("postgres tx begin+touch+rollback", tx_stats);
    return 0;
}

#else

int main() {
    std::printf("[skip] libpq is unavailable in this build\n");
    return 0;
}

#endif
