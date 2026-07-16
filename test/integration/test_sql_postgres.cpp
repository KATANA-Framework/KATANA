#include "katana/sql/postgres.hpp"

#include "katana/core/arena.hpp"
#include "katana/core/handler_context.hpp"
#include "katana/core/http.hpp"
#include "katana/core/reactor.hpp"
#include "katana/core/router.hpp"
#include "katana/sql/gather.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>
#include <memory>
#include <optional>
#include <thread>
#include <tuple>

#ifdef KATANA_HAS_LIBPQ

namespace {

const char* postgres_dsn() {
    return std::getenv("KATANA_TEST_POSTGRES_DSN");
}

// Decode a single-column "v" row into a long (the tests below select scalar ints).
long first_int(const katana::sql::rows& r) {
    if (r.empty()) {
        return -1;
    }
    const auto& c = r.front().at(0);
    return c.has_value() ? std::stol(*c) : -1;
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

// F14: gather() fans three queries out over the reactor's own connection using Postgres pipeline
// mode, and joins them into one typed tuple. Running on a live reactor thread exercises the real
// pipeline state machine (start_reactor_pipeline / poll_pipeline) end-to-end against Postgres.
TEST(PostgresPipelineIntegration, GatherJoinsThreeQueriesOverPipeline) {
    const char* dsn = postgres_dsn();
    if (dsn == nullptr || *dsn == '\0') {
        std::cout << "[sql] KATANA_TEST_POSTGRES_DSN is not set; skipping integration body\n";
        return;
    }

    katana::reactor reactor;
    std::thread reactor_thread([&] { (void)reactor.run(); });

    auto executor = std::make_unique<katana::sql::postgres_executor>(
        katana::sql::postgres_config{.connection_string = dsn});

    using tuple_t = std::tuple<long, std::optional<long>, std::vector<long>>;
    auto promise = std::make_shared<std::promise<katana::result<tuple_t>>>();
    auto future = promise->get_future();

    const bool scheduled = reactor.schedule([&reactor, &executor, promise] {
        // Establish a handler context so the executor takes its per-reactor (pipelined) path.
        katana::monotonic_arena arena;
        katana::http::request request;
        katana::http::request_context ctx{arena};
        ctx.reactor_user = &reactor;
        katana::http::handler_context::scope guard(request, ctx);

        using namespace katana::sql;
        gather(
            *executor,
            [promise](katana::result<tuple_t> joined) { promise->set_value(std::move(joined)); },
            query_step<long>{"pl_sel1", "SELECT 1 AS v", {},
                             [](rows r) { return katana::result<long>(first_int(r)); }},
            query_step<std::optional<long>>{
                "pl_sel2", "SELECT 2 AS v", {},
                [](rows r) { return katana::result<std::optional<long>>(std::optional<long>(first_int(r))); }},
            query_step<std::vector<long>>{
                "pl_sel3", "SELECT $1::int AS v", {encode_value(static_cast<int64_t>(7))},
                [](rows r) { return katana::result<std::vector<long>>(std::vector<long>{first_int(r)}); }});
    });
    ASSERT_TRUE(scheduled);

    ASSERT_EQ(future.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    auto joined = future.get();
    ASSERT_TRUE(joined.has_value());
    EXPECT_EQ(std::get<0>(*joined), 1);
    EXPECT_EQ(std::get<1>(*joined).value_or(-1), 2);
    ASSERT_EQ(std::get<2>(*joined).size(), 1u);
    EXPECT_EQ(std::get<2>(*joined).front(), 7);

    // Tear down the executor while the reactor is still running so its cross-thread cleanup can be
    // scheduled onto the reactor; only then stop the loop.
    executor.reset();
    reactor.stop();
    reactor_thread.join();
}

// A failing statement inside the batch must surface as an error (not a hang or a corrupt result) —
// libpq aborts the rest of the pipeline, and gather short-circuits to the first error.
TEST(PostgresPipelineIntegration, GatherReportsErrorWhenAQueryFails) {
    const char* dsn = postgres_dsn();
    if (dsn == nullptr || *dsn == '\0') {
        std::cout << "[sql] KATANA_TEST_POSTGRES_DSN is not set; skipping integration body\n";
        return;
    }

    katana::reactor reactor;
    std::thread reactor_thread([&] { (void)reactor.run(); });
    auto executor = std::make_unique<katana::sql::postgres_executor>(
        katana::sql::postgres_config{.connection_string = dsn});

    using tuple_t = std::tuple<long, long>;
    auto promise = std::make_shared<std::promise<katana::result<tuple_t>>>();
    auto future = promise->get_future();

    const bool scheduled = reactor.schedule([&reactor, &executor, promise] {
        katana::monotonic_arena arena;
        katana::http::request request;
        katana::http::request_context ctx{arena};
        ctx.reactor_user = &reactor;
        katana::http::handler_context::scope guard(request, ctx);

        using namespace katana::sql;
        gather(
            *executor,
            [promise](katana::result<tuple_t> joined) { promise->set_value(std::move(joined)); },
            query_step<long>{"pl_ok", "SELECT 1 AS v", {},
                             [](rows r) { return katana::result<long>(first_int(r)); }},
            query_step<long>{"pl_bad", "SELECT * FROM a_table_that_does_not_exist", {},
                             [](rows r) { return katana::result<long>(first_int(r)); }});
    });
    ASSERT_TRUE(scheduled);

    ASSERT_EQ(future.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    auto joined = future.get();
    EXPECT_FALSE(joined.has_value());

    executor.reset();
    reactor.stop();
    reactor_thread.join();
}

#else

TEST(PostgresExecutorIntegration, LibpqUnavailable) {
    EXPECT_TRUE(true);
}

#endif
