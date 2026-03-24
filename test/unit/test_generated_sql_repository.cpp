#include "generated_sql_repository.hpp"

#include <gtest/gtest.h>

#include <optional>
#include <string>
#include <vector>

namespace {

class FakeExecutor final : public katana::sql::executor {
public:
    katana::result<katana::sql::rows> query(std::string_view statement_name,
                                            std::string_view sql,
                                            const katana::sql::parameters& params) override {
        last_statement_name = std::string(statement_name);
        last_sql = std::string(sql);
        last_params = params;
        return query_rows;
    }

    katana::result<katana::sql::exec_result> exec(std::string_view statement_name,
                                                  std::string_view sql,
                                                  const katana::sql::parameters& params) override {
        last_statement_name = std::string(statement_name);
        last_sql = std::string(sql);
        last_params = params;
        return exec_result_value;
    }

    katana::result<void> query_each(std::string_view statement_name,
                                    std::string_view sql,
                                    const katana::sql::parameters& params,
                                    katana::sql::row_handler handler) override {
        last_statement_name = std::string(statement_name);
        last_sql = std::string(sql);
        last_params = params;

        std::vector<katana::sql::cell_view> scratch;
        for (const auto& input_row : query_rows) {
            scratch.clear();
            scratch.reserve(input_row.size());
            for (std::size_t index = 0; index < input_row.size(); ++index) {
                const auto& cell = input_row.at(index);
                if (cell.has_value()) {
                    scratch.push_back(katana::sql::cell_view::borrowed(*cell));
                } else {
                    scratch.push_back(katana::sql::cell_view::null());
                }
            }
            auto status = handler(katana::sql::row_view(scratch.data(), scratch.size()));
            if (!status) {
                return std::unexpected(status.error());
            }
        }
        return {};
    }

    std::string last_statement_name;
    std::string last_sql;
    katana::sql::parameters last_params;
    katana::sql::rows query_rows;
    katana::sql::exec_result exec_result_value{.affected_rows = 0};
};

TEST(GeneratedSqlRepositoryTest, MapsSingleRowQueries) {
    FakeExecutor executor;
    executor.query_rows = {
        {{"id", std::string("42")}, {"name", std::string("Ada")}, {"active", std::string("true")}}};

    katana::sql::generated::generated_repository repo(executor);
    auto result = repo.get_user(42);

    ASSERT_TRUE(result);
    ASSERT_TRUE(result->has_value());
    EXPECT_EQ(executor.last_statement_name, "get_user");
    ASSERT_EQ(executor.last_params.size(), 1u);
    ASSERT_TRUE(executor.last_params[0].has_value());
    EXPECT_EQ(*executor.last_params[0], "42");
    EXPECT_EQ(result->value().id, std::optional<int64_t>(42));
    EXPECT_EQ(result->value().name, std::optional<std::string>("Ada"));
    EXPECT_EQ(result->value().active, std::optional<bool>(true));
}

TEST(GeneratedSqlRepositoryTest, MapsManyQueries) {
    FakeExecutor executor;
    executor.query_rows = {
        {{"id", std::string("1")}, {"name", std::string("Ada")}},
        {{"id", std::string("2")}, {"name", std::string("Linus")}},
    };

    katana::sql::generated::generated_repository repo(executor);
    auto result = repo.list_users(true);

    ASSERT_TRUE(result);
    ASSERT_EQ(result->size(), 2u);
    EXPECT_EQ(executor.last_statement_name, "list_users");
    ASSERT_EQ(executor.last_params.size(), 1u);
    ASSERT_TRUE(executor.last_params[0].has_value());
    EXPECT_EQ(*executor.last_params[0], "true");
    EXPECT_EQ((*result)[0].id, std::optional<int64_t>(1));
    EXPECT_EQ((*result)[1].name, std::optional<std::string>("Linus"));
}

TEST(GeneratedSqlRepositoryTest, PassesExecQueriesThrough) {
    FakeExecutor executor;
    executor.exec_result_value.affected_rows = 3;

    katana::sql::generated::generated_repository repo(executor);
    auto result = repo.touch_user(77);

    ASSERT_TRUE(result);
    EXPECT_EQ(result->affected_rows, 3u);
    EXPECT_EQ(executor.last_statement_name, "touch_user");
    ASSERT_EQ(executor.last_params.size(), 1u);
    ASSERT_TRUE(executor.last_params[0].has_value());
    EXPECT_EQ(*executor.last_params[0], "77");
}

TEST(GeneratedSqlRepositoryTest, RejectsMultipleRowsForOneQuery) {
    FakeExecutor executor;
    executor.query_rows = {
        {{"id", std::string("1")}, {"name", std::string("Ada")}, {"active", std::string("true")}},
        {{"id", std::string("2")}, {"name", std::string("Linus")}, {"active", std::string("true")}},
    };

    katana::sql::generated::generated_repository repo(executor);
    auto result = repo.get_user(1);

    ASSERT_FALSE(result);
}

TEST(GeneratedSqlRepositoryTest, SupportsUpsertQueriesReturningRows) {
    FakeExecutor executor;
    executor.query_rows = {
        {{"id", std::string("7")}, {"name", std::string("Grace")}, {"active", std::string("true")}}};

    katana::sql::generated::generated_repository repo(executor);
    auto result = repo.upsert_user(7, "Grace", true);

    ASSERT_TRUE(result);
    ASSERT_TRUE(result->has_value());
    EXPECT_EQ(executor.last_statement_name, "upsert_user");
    ASSERT_EQ(executor.last_params.size(), 3u);
    EXPECT_EQ(*executor.last_params[0], "7");
    EXPECT_EQ(*executor.last_params[1], "Grace");
    EXPECT_EQ(*executor.last_params[2], "true");
    EXPECT_EQ(result->value().name, std::optional<std::string>("Grace"));
}

TEST(GeneratedSqlRepositoryTest, SupportsBulkArrayParameters) {
    FakeExecutor executor;
    executor.query_rows = {
        {{"id", std::string("1")}, {"name", std::string("Ada")}, {"active", std::string("true")}},
        {{"id", std::string("2")}, {"name", std::string("Linus")}, {"active", std::string("false")}},
    };

    katana::sql::generated::generated_repository repo(executor);
    auto result = repo.bulk_upsert_users(
        std::vector<int64_t>{1, 2},
        std::vector<std::string>{"Ada", "Linus"},
        std::vector<bool>{true, false});

    ASSERT_TRUE(result);
    ASSERT_EQ(result->size(), 2u);
    EXPECT_EQ(executor.last_statement_name, "bulk_upsert_users");
    ASSERT_EQ(executor.last_params.size(), 3u);
    EXPECT_EQ(*executor.last_params[0], "{1,2}");
    EXPECT_EQ(*executor.last_params[1], "{\"Ada\",\"Linus\"}");
    EXPECT_EQ(*executor.last_params[2], "{true,false}");
    EXPECT_EQ((*result)[0].id, std::optional<int64_t>(1));
    EXPECT_EQ((*result)[1].name, std::optional<std::string>("Linus"));
}

} // namespace
