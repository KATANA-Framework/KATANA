#include <gtest/gtest.h>

#ifdef KATANA_HAS_LIBPQ

#include "katana/sql/postgres.hpp"

#include <chrono>
#include <future>

namespace {

constexpr std::string_view invalid_async_worker_dsn =
    "host=127.0.0.1 port=1 dbname=katana_invalid connect_timeout=1";

} // namespace

TEST(PostgresPoolTest, CreatesRequestedExecutorCount) {
    katana::sql::postgres_pool pool({
        .postgres = {.connection_string = "dbname=katana_invalid"},
        .executor_count = 4,
    });

    EXPECT_EQ(pool.size(), 4u);
}

TEST(PostgresPoolTest, ZeroCountFallsBackToSingleExecutor) {
    katana::sql::postgres_pool pool({
        .postgres = {.connection_string = "dbname=katana_invalid"},
        .executor_count = 0,
    });

    EXPECT_EQ(pool.size(), 1u);
}

TEST(PostgresPoolTest, ReactorMappingIsStableAndWraps) {
    katana::sql::postgres_pool pool({
        .postgres = {.connection_string = "dbname=katana_invalid"},
        .executor_count = 3,
    });

    auto& e0 = pool.for_reactor(0);
    auto& e1 = pool.for_reactor(1);
    auto& e2 = pool.for_reactor(2);
    auto& e3 = pool.for_reactor(3);

    EXPECT_TRUE(&e0 != &e1);
    EXPECT_TRUE(&e1 != &e2);
    EXPECT_EQ(&e0, &e3);
}

TEST(PostgresPoolTest, CurrentExecutorReturnsPoolMember) {
    katana::sql::postgres_pool pool({
        .postgres = {.connection_string = "dbname=katana_invalid"},
        .executor_count = 2,
    });

    auto& current = pool.current_executor();
    EXPECT_TRUE(&current == &pool.for_reactor(0) || &current == &pool.for_reactor(1));
}

TEST(PostgresExecutorAsyncTest, QueryAsyncPropagatesConnectionFailures) {
    katana::sql::postgres_executor executor({
        .connection_string = std::string(invalid_async_worker_dsn),
    });

    std::promise<katana::result<katana::sql::rows>> result_promise;
    auto result_future = result_promise.get_future();

    ASSERT_TRUE(executor.query_async(
        "async_query_connection_failure", "SELECT 1", {}, [&result_promise](auto result) {
            result_promise.set_value(std::move(result));
        }));

    ASSERT_EQ(result_future.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    EXPECT_FALSE(result_future.get());
}

TEST(PostgresExecutorAsyncTest, ExecAsyncPropagatesConnectionFailures) {
    katana::sql::postgres_executor executor({
        .connection_string = std::string(invalid_async_worker_dsn),
    });

    std::promise<katana::result<katana::sql::exec_result>> result_promise;
    auto result_future = result_promise.get_future();

    ASSERT_TRUE(executor.exec_async(
        "async_exec_connection_failure", "SELECT 1", {}, [&result_promise](auto result) {
            result_promise.set_value(std::move(result));
        }));

    ASSERT_EQ(result_future.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    EXPECT_FALSE(result_future.get());
}

#else

TEST(PostgresPoolTest, LibpqUnavailable) {
    EXPECT_TRUE(true);
}

#endif
