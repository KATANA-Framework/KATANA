#include "bench_utils.hpp"
#include "generated_sql_repository.hpp"
#include "katana/sql/postgres.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#ifdef KATANA_HAS_LIBPQ

namespace {

const char* postgres_dsn() {
    return std::getenv("KATANA_TEST_POSTGRES_DSN");
}

int read_positive_int_env(const char* name, int fallback) {
    const char* raw = std::getenv(name);
    if (raw == nullptr || *raw == '\0') {
        return fallback;
    }
    char* end = nullptr;
    const long parsed = std::strtol(raw, &end, 10);
    if (end == raw || *end != '\0' || parsed <= 0 || parsed > static_cast<long>(INT32_MAX)) {
        return fallback;
    }
    return static_cast<int>(parsed);
}

struct throughput_stats {
    double latency_us = 0.0;
    double throughput = 0.0;
};

katana::result<void> seed_fixture(katana::sql::postgres_executor& executor, int user_count) {
    if (auto result = executor.exec(
            "bench_set_client_min_messages", "SET client_min_messages TO warning", {});
        !result) {
        return std::unexpected(result.error());
    }

    if (auto result = executor.exec("bench_create_shared_users",
                                    "CREATE TABLE IF NOT EXISTS katana_stage4_users ("
                                    "id BIGINT PRIMARY KEY, "
                                    "name TEXT NOT NULL, "
                                    "active BOOLEAN NOT NULL, "
                                    "last_seen_at TIMESTAMPTZ)",
                                    {});
        !result) {
        return std::unexpected(result.error());
    }

    if (auto result =
            executor.exec("bench_truncate_shared_users", "TRUNCATE katana_stage4_users", {});
        !result) {
        return std::unexpected(result.error());
    }

    const std::string seed_sql = "INSERT INTO katana_stage4_users (id, name, active) "
                                 "SELECT g, 'user_' || g::text, (g % 2 = 0) "
                                 "FROM generate_series(1, " +
                                 std::to_string(user_count) + ") AS g";
    if (auto result = executor.exec("bench_seed_shared_users", seed_sql, {}); !result) {
        return std::unexpected(result.error());
    }

    return {};
}

template <typename Fn>
throughput_stats run_parallel_bench(katana::sql::postgres_pool& pool,
                                    int thread_count,
                                    std::chrono::seconds duration,
                                    Fn&& fn) {
    std::atomic<bool> start{false};
    std::atomic<bool> stop{false};
    std::atomic<int> ready{0};
    std::atomic<std::uint64_t> total_ops{0};
    std::atomic<std::uint64_t> total_errors{0};
    std::vector<std::thread> workers;
    workers.reserve(static_cast<std::size_t>(thread_count));

    for (int thread_index = 0; thread_index < thread_count; ++thread_index) {
        workers.emplace_back([&, thread_index]() {
            auto& executor = pool.for_reactor(static_cast<std::size_t>(thread_index));
            katana::sql::generated::generated_repository repo(executor);
            ready.fetch_add(1, std::memory_order_release);
            while (!start.load(std::memory_order_acquire)) {
                std::this_thread::yield();
            }

            std::uint64_t local_ops = 0;
            std::uint64_t local_errors = 0;
            std::uint64_t iteration = 0;
            while (!stop.load(std::memory_order_relaxed)) {
                const auto completed_ops = fn(repo, executor, thread_index, iteration, stop);
                if (completed_ops == 0U) {
                    ++local_errors;
                } else {
                    local_ops += completed_ops;
                }
                ++iteration;
            }

            total_ops.fetch_add(local_ops, std::memory_order_relaxed);
            total_errors.fetch_add(local_errors, std::memory_order_relaxed);
        });
    }

    while (ready.load(std::memory_order_acquire) != thread_count) {
        std::this_thread::yield();
    }

    const auto start_time = std::chrono::steady_clock::now();
    start.store(true, std::memory_order_release);
    std::this_thread::sleep_for(duration);
    stop.store(true, std::memory_order_release);

    for (auto& worker : workers) {
        worker.join();
    }
    const auto end_time = std::chrono::steady_clock::now();

    bench_util::do_not_optimize(total_errors.load(std::memory_order_relaxed));
    const auto elapsed_ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    const double elapsed_seconds = static_cast<double>(elapsed_ns) / 1e9;
    const auto ops = total_ops.load(std::memory_order_relaxed);
    if (ops == 0 || elapsed_seconds <= 0.0) {
        return {};
    }
    return throughput_stats{
        .latency_us = (elapsed_seconds * 1e6) / static_cast<double>(ops),
        .throughput = static_cast<double>(ops) / elapsed_seconds,
    };
}

void print_result(std::string_view name, const throughput_stats& stats) {
    std::printf("  %-48.*s %8.2f us    %12.0f ops/sec\n",
                static_cast<int>(name.size()),
                name.data(),
                stats.latency_us,
                stats.throughput);
}

} // namespace

int main() {
    const char* dsn = postgres_dsn();
    if (dsn == nullptr || *dsn == '\0') {
        std::printf("[skip] KATANA_TEST_POSTGRES_DSN is not set\n");
        return 0;
    }

    const int thread_count = read_positive_int_env("KATANA_SQL_BENCH_THREADS", 4);
    const int duration_seconds = read_positive_int_env("KATANA_SQL_BENCH_SECONDS", 2);
    const int shard_size = read_positive_int_env("KATANA_SQL_BENCH_SHARD_SIZE", 64);
    const int user_count = std::max(thread_count, 1) * shard_size;
    const std::size_t active_user_count = static_cast<std::size_t>(user_count / 2);

    katana::sql::postgres_pool pool({
        .postgres = {.connection_string = dsn},
        .executor_count = static_cast<std::size_t>(thread_count),
        .eager_connect = true,
    });
    auto connected = pool.connect_all();
    if (!connected) {
        std::fprintf(stderr, "sql_postgres_throughput_benchmark: failed to connect pool\n");
        return 1;
    }

    auto seeded = seed_fixture(pool.for_reactor(0), user_count);
    if (!seeded) {
        std::fprintf(stderr, "sql_postgres_throughput_benchmark: failed to seed fixture\n");
        return 1;
    }

    const auto duration = std::chrono::seconds(duration_seconds);
    const auto user_id_for = [shard_size](int thread_index, std::uint64_t iteration) -> int64_t {
        const auto shard_base =
            static_cast<std::uint64_t>(thread_index) * static_cast<std::uint64_t>(shard_size);
        return static_cast<int64_t>(1 + shard_base +
                                    (iteration % static_cast<std::uint64_t>(shard_size)));
    };

    const auto get_user_stats = run_parallel_bench(
        pool,
        thread_count,
        duration,
        [&](auto& repo, auto&, int thread_index, std::uint64_t iteration, const auto&) {
            const auto user_id = user_id_for(thread_index, iteration);
            auto result = repo.get_user(user_id);
            return result && result->has_value() ? 1U : 0U;
        });

    const auto list_users_stats = run_parallel_bench(
        pool, thread_count, duration, [&](auto& repo, auto&, int, std::uint64_t, const auto&) {
            auto result = repo.list_users(true);
            return result && result->size() == active_user_count ? 1U : 0U;
        });

    const auto touch_user_stats = run_parallel_bench(
        pool,
        thread_count,
        duration,
        [&](auto& repo, auto&, int thread_index, std::uint64_t iteration, const auto&) {
            const auto user_id = user_id_for(thread_index, iteration);
            auto result = repo.touch_user(user_id);
            return result && result->affected_rows == 1U ? 1U : 0U;
        });

    const auto touch_user_tx_stats = run_parallel_bench(
        pool,
        thread_count,
        duration,
        [&](auto& repo, auto& executor, int thread_index, std::uint64_t iteration, const auto&) {
            constexpr std::uint64_t batch_size = 32;
            auto begun = executor.begin();
            if (!begun) {
                return 0U;
            }

            for (std::uint64_t offset = 0; offset < batch_size; ++offset) {
                const auto user_id = user_id_for(thread_index, iteration * batch_size + offset);
                auto result = repo.touch_user(user_id);
                if (!result || result->affected_rows != 1U) {
                    (void)executor.rollback();
                    return 0U;
                }
            }

            auto committed = executor.commit();
            return committed ? static_cast<unsigned>(batch_size) : 0U;
        });

    const auto mixed_stats = run_parallel_bench(
        pool,
        thread_count,
        duration,
        [&](auto& repo, auto&, int thread_index, std::uint64_t iteration, const auto&) {
            const auto user_id = user_id_for(thread_index, iteration);
            const auto slot = iteration % 10U;
            if (slot < 7U) {
                auto result = repo.get_user(user_id);
                return result && result->has_value() ? 1U : 0U;
            }
            if (slot < 9U) {
                auto result = repo.touch_user(user_id);
                return result && result->affected_rows == 1U ? 1U : 0U;
            }
            auto result = repo.list_users(true);
            return result && result->size() == active_user_count ? 1U : 0U;
        });

    std::printf("SQL PostgreSQL Throughput Benchmarks\n");
    print_result("postgres concurrent get_user", get_user_stats);
    print_result("postgres concurrent list_users", list_users_stats);
    print_result("postgres concurrent touch_user", touch_user_stats);
    print_result("postgres concurrent touch_user tx32", touch_user_tx_stats);
    print_result("postgres concurrent mixed workload", mixed_stats);
    return 0;
}

#else

int main() {
    std::printf("[skip] libpq is unavailable in this build\n");
    return 0;
}

#endif
