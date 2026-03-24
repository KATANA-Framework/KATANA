#include <gtest/gtest.h>

#ifdef KATANA_HAS_LIBPQ

#include "katana/sql/postgres.hpp"

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

#else

TEST(PostgresPoolTest, LibpqUnavailable) {
    EXPECT_TRUE(true);
}

#endif
