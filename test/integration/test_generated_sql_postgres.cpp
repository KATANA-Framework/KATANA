#include "generated_sql_repository.hpp"
#include "katana/sql/postgres.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

#ifdef KATANA_HAS_LIBPQ

namespace {

const char* postgres_dsn() {
    return std::getenv("KATANA_TEST_POSTGRES_DSN");
}

} // namespace

TEST(GeneratedSqlRepositoryIntegration, UsesPostgresPoolExecutor) {
    const char* dsn = postgres_dsn();
    if (dsn == nullptr || *dsn == '\0') {
        std::cout << "[sql] KATANA_TEST_POSTGRES_DSN is not set; skipping generated repository "
                     "integration body\n";
        return;
    }

    katana::sql::postgres_pool pool({
        .postgres = {.connection_string = dsn},
        .executor_count = 1,
        .eager_connect = true,
    });
    ASSERT_TRUE(pool.connect_all());

    auto& executor = pool.current_executor();
    ASSERT_TRUE(executor.exec("repo_create_temp_users",
                              "CREATE TEMP TABLE IF NOT EXISTS katana_stage4_users ("
                              "id BIGINT PRIMARY KEY, "
                              "name TEXT NOT NULL, "
                              "active BOOLEAN NOT NULL, "
                              "last_seen_at TIMESTAMPTZ)",
                              {}));
    ASSERT_TRUE(executor.exec("repo_truncate_temp_users", "TRUNCATE katana_stage4_users", {}));
    ASSERT_TRUE(executor.exec("repo_insert_user_1",
                              "INSERT INTO katana_stage4_users (id, name, active) "
                              "VALUES (1, 'Ada', true)",
                              {}));
    ASSERT_TRUE(executor.exec("repo_insert_user_2",
                              "INSERT INTO katana_stage4_users (id, name, active) "
                              "VALUES (2, 'Linus', false)",
                              {}));

    katana::sql::postgres_pool_executor pool_executor(pool);
    katana::sql::generated::generated_repository repo(pool_executor);

    auto one = repo.get_user(1);
    ASSERT_TRUE(one);
    ASSERT_TRUE(one->has_value());
    EXPECT_EQ(one->value().id, std::optional<int64_t>(1));
    EXPECT_EQ(one->value().name, std::optional<std::string>("Ada"));
    EXPECT_EQ(one->value().active, std::optional<bool>(true));

    auto many = repo.list_users(true);
    ASSERT_TRUE(many);
    ASSERT_EQ(many->size(), 1u);
    EXPECT_EQ(many->front().name, std::optional<std::string>("Ada"));

    auto touched = repo.touch_user(1);
    ASSERT_TRUE(touched);
    EXPECT_EQ(touched->affected_rows, 1u);

    auto upserted = repo.upsert_user(2, "Linus-updated", true);
    ASSERT_TRUE(upserted);
    ASSERT_TRUE(upserted->has_value());
    EXPECT_EQ(upserted->value().id, std::optional<int64_t>(2));
    EXPECT_EQ(upserted->value().name, std::optional<std::string>("Linus-updated"));
    EXPECT_EQ(upserted->value().active, std::optional<bool>(true));

    auto bulk = repo.bulk_upsert_users(std::vector<int64_t>{2, 3, 4},
                                       std::vector<std::string>{
                                           "Linus-bulk",
                                           "Grace",
                                           "Margaret",
                                       },
                                       std::vector<bool>{false, true, true});
    ASSERT_TRUE(bulk);
    ASSERT_EQ(bulk->size(), 3u);
    std::sort(bulk->begin(), bulk->end(), [](const auto& lhs, const auto& rhs) {
        return lhs.id.value_or(0) < rhs.id.value_or(0);
    });

    EXPECT_EQ((*bulk)[0].id, std::optional<int64_t>(2));
    EXPECT_EQ((*bulk)[0].name, std::optional<std::string>("Linus-bulk"));
    EXPECT_EQ((*bulk)[0].active, std::optional<bool>(false));
    EXPECT_EQ((*bulk)[1].id, std::optional<int64_t>(3));
    EXPECT_EQ((*bulk)[1].name, std::optional<std::string>("Grace"));
    EXPECT_EQ((*bulk)[2].id, std::optional<int64_t>(4));

    auto refreshed = repo.list_users(true);
    ASSERT_TRUE(refreshed);
    ASSERT_EQ(refreshed->size(), 3u);
    EXPECT_EQ((*refreshed)[0].id, std::optional<int64_t>(1));
    EXPECT_EQ((*refreshed)[1].id, std::optional<int64_t>(3));
    EXPECT_EQ((*refreshed)[2].id, std::optional<int64_t>(4));
}

#else

TEST(GeneratedSqlRepositoryIntegration, LibpqUnavailable) {
    EXPECT_TRUE(true);
}

#endif
