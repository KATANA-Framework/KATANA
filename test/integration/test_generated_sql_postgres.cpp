#include "generated_sql_repository.hpp"
#include "katana/core/handler_context.hpp"
#include "katana/core/http.hpp"
#include "katana/core/reactor.hpp"
#include "katana/sql/postgres.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdlib>
#include <future>
#include <iostream>
#include <thread>
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
    generated::generated_repository repo(pool_executor);

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

TEST(GeneratedSqlRepositoryIntegration, SupportsAsyncGeneratedQueries) {
    const char* dsn = postgres_dsn();
    if (dsn == nullptr || *dsn == '\0') {
        std::cout << "[sql] KATANA_TEST_POSTGRES_DSN is not set; skipping generated async "
                     "repository integration body\n";
        return;
    }

    katana::sql::postgres_pool pool({
        .postgres = {.connection_string = dsn},
        .executor_count = 1,
        .eager_connect = true,
    });
    ASSERT_TRUE(pool.connect_all());

    auto& executor = pool.current_executor();
    ASSERT_TRUE(executor.exec("repo_async_create_users",
                              "CREATE TABLE IF NOT EXISTS katana_stage4_users ("
                              "id BIGINT PRIMARY KEY, "
                              "name TEXT NOT NULL, "
                              "active BOOLEAN NOT NULL, "
                              "last_seen_at TIMESTAMPTZ)",
                              {}));
    ASSERT_TRUE(
        executor.exec("repo_async_truncate_temp_users", "TRUNCATE katana_stage4_users", {}));
    ASSERT_TRUE(executor.exec("repo_async_insert_user_1",
                              "INSERT INTO katana_stage4_users (id, name, active) "
                              "VALUES (1, 'Ada', true)",
                              {}));

    katana::sql::postgres_pool_executor pool_executor(pool);
    generated::generated_repository repo(pool_executor);

    std::promise<katana::result<std::optional<generated::GetUserRow>>> get_promise;
    ASSERT_TRUE(repo.get_user_async(
        1, [&get_promise](auto result) { get_promise.set_value(std::move(result)); }));
    auto async_user = get_promise.get_future().get();
    ASSERT_TRUE(async_user);
    ASSERT_TRUE(async_user->has_value());
    EXPECT_EQ(async_user->value().name, std::optional<std::string>("Ada"));

    std::promise<katana::result<katana::sql::exec_result>> exec_promise;
    ASSERT_TRUE(repo.touch_user_async(
        1, [&exec_promise](auto result) { exec_promise.set_value(std::move(result)); }));
    auto async_exec = exec_promise.get_future().get();
    ASSERT_TRUE(async_exec);
    EXPECT_EQ(async_exec->affected_rows, 1u);
}

TEST(GeneratedSqlRepositoryIntegration,
     AsyncGeneratedQueriesUseReactorThreadWhenHandlerContextAvailable) {
    const char* dsn = postgres_dsn();
    if (dsn == nullptr || *dsn == '\0') {
        std::cout << "[sql] KATANA_TEST_POSTGRES_DSN is not set; skipping reactor-thread async "
                     "repository integration body\n";
        return;
    }

    katana::sql::postgres_pool pool({
        .postgres = {.connection_string = dsn},
        .executor_count = 1,
        .eager_connect = true,
    });
    ASSERT_TRUE(pool.connect_all());

    auto& executor = pool.current_executor();
    ASSERT_TRUE(executor.exec("repo_reactor_async_create_users",
                              "CREATE TABLE IF NOT EXISTS katana_stage4_users ("
                              "id BIGINT PRIMARY KEY, "
                              "name TEXT NOT NULL, "
                              "active BOOLEAN NOT NULL, "
                              "last_seen_at TIMESTAMPTZ)",
                              {}));
    ASSERT_TRUE(
        executor.exec("repo_reactor_async_truncate_users", "TRUNCATE katana_stage4_users", {}));
    ASSERT_TRUE(executor.exec("repo_reactor_async_insert_user_1",
                              "INSERT INTO katana_stage4_users (id, name, active) "
                              "VALUES (1, 'Ada', true)",
                              {}));

    katana::sql::postgres_pool_executor pool_executor(pool);
    generated::generated_repository repo(pool_executor);
    katana::reactor r;

    std::promise<katana::result<std::optional<generated::GetUserRow>>> result_promise;
    std::promise<std::thread::id> callback_thread_promise;
    std::thread::id reactor_thread_id{};

    ASSERT_TRUE(r.schedule([&]() {
        reactor_thread_id = std::this_thread::get_id();

        katana::monotonic_arena arena;
        katana::http::request req;
        katana::http::request_context ctx{arena};
        ctx.reactor_user = &r;
        katana::http::handler_context::scope scope(req, ctx);

        const bool started =
            repo.get_user_async(1, [&result_promise, &callback_thread_promise, &r](auto result) {
                callback_thread_promise.set_value(std::this_thread::get_id());
                result_promise.set_value(std::move(result));
                r.stop();
            });

        if (!started) {
            callback_thread_promise.set_value(std::this_thread::get_id());
            result_promise.set_value(
                std::unexpected(std::make_error_code(std::errc::operation_not_supported)));
            r.stop();
        }
    }));

    ASSERT_TRUE(r.run());
    const auto callback_thread_id = callback_thread_promise.get_future().get();
    auto async_user = result_promise.get_future().get();
    ASSERT_TRUE(async_user);
    ASSERT_TRUE(async_user->has_value());
    EXPECT_EQ(async_user->value().name, std::optional<std::string>("Ada"));
    EXPECT_EQ(callback_thread_id, reactor_thread_id);
}

#else

TEST(GeneratedSqlRepositoryIntegration, LibpqUnavailable) {
    EXPECT_TRUE(true);
}

#endif
