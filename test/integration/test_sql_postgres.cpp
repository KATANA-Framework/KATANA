#include "katana/sql/postgres.hpp"

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>

#ifdef KATANA_HAS_LIBPQ

namespace {

const char* postgres_dsn() {
    return std::getenv("KATANA_TEST_POSTGRES_DSN");
}

} // namespace

TEST(PostgresExecutorIntegration, ConnectExecQueryAndRollback) {
    const char* dsn = postgres_dsn();
    if (dsn == nullptr || *dsn == '\0') {
        std::cout << "[sql] KATANA_TEST_POSTGRES_DSN is not set; skipping integration body\n";
        return;
    }

    katana::sql::postgres_executor executor({.connection_string = dsn});
    ASSERT_TRUE(executor.connect());

    ASSERT_TRUE(executor.exec("create_temp_users",
                              "CREATE TEMP TABLE IF NOT EXISTS katana_stage4_users ("
                              "id BIGINT PRIMARY KEY, name TEXT NOT NULL, active BOOLEAN NOT NULL)",
                              {}));
    ASSERT_TRUE(executor.exec("truncate_temp_users", "TRUNCATE katana_stage4_users", {}));
    ASSERT_TRUE(executor.exec("insert_user_1",
                              "INSERT INTO katana_stage4_users (id, name, active) "
                              "VALUES (1, 'Ada', true)",
                              {}));
    ASSERT_TRUE(executor.exec("insert_user_2",
                              "INSERT INTO katana_stage4_users (id, name, active) "
                              "VALUES (2, 'Linus', false)",
                              {}));

    auto rows = executor.query("select_active_users",
                               "SELECT id, name, active "
                               "FROM katana_stage4_users WHERE active = $1 ORDER BY id",
                               {katana::sql::encode_value(true)});
    ASSERT_TRUE(rows);
    ASSERT_EQ(rows->size(), 1u);
    ASSERT_EQ(rows->at(0).at("name"), katana::sql::cell(std::string("Ada")));

    katana::sql::postgres_transaction tx(executor);
    ASSERT_TRUE(tx.begin());
    ASSERT_TRUE(tx.active());
    ASSERT_TRUE(executor.exec("insert_user_3",
                              "INSERT INTO katana_stage4_users (id, name, active) "
                              "VALUES (3, 'Grace', true)",
                              {}));
    ASSERT_TRUE(tx.rollback());
    ASSERT_FALSE(tx.active());

    auto after_rollback = executor.query("count_users",
                                         "SELECT COUNT(*)::bigint AS total "
                                         "FROM katana_stage4_users",
                                         {});
    ASSERT_TRUE(after_rollback);
    ASSERT_EQ(after_rollback->size(), 1u);
    ASSERT_EQ(after_rollback->at(0).at("total"), katana::sql::cell(std::string("2")));
}

#else

TEST(PostgresExecutorIntegration, LibpqUnavailable) {
    EXPECT_TRUE(true);
}

#endif
