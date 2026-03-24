#include "demo_service.hpp"

#include <gtest/gtest.h>

#include <charconv>
#include <cstdlib>
#include <future>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#ifdef KATANA_HAS_LIBPQ

namespace {

using katana::benchmark_api_demo::service;
using katana::benchmark_api_demo::service_config;
using katana::http::field;
using katana::http::headers_map;
using katana::http::method;
using katana::http::request;
using katana::http::request_context;
using katana::http::response;

const char* postgres_dsn() {
    return std::getenv("KATANA_TEST_POSTGRES_DSN");
}

class counting_executor final : public katana::sql::executor {
public:
    explicit counting_executor(katana::sql::executor& inner) noexcept : inner_(inner) {}

    katana::result<katana::sql::rows> query(std::string_view statement_name,
                                            std::string_view sql,
                                            const katana::sql::parameters& params) override {
        ++query_calls;
        statement_names.emplace_back(statement_name);
        return inner_.query(statement_name, sql, params);
    }

    katana::result<katana::sql::exec_result> exec(std::string_view statement_name,
                                                  std::string_view sql,
                                                  const katana::sql::parameters& params) override {
        ++exec_calls;
        statement_names.emplace_back(statement_name);
        return inner_.exec(statement_name, sql, params);
    }

    katana::result<void> query_each(std::string_view statement_name,
                                    std::string_view sql,
                                    const katana::sql::parameters& params,
                                    katana::sql::row_handler handler) override {
        ++query_each_calls;
        statement_names.emplace_back(statement_name);
        return inner_.query_each(statement_name, sql, params, std::move(handler));
    }

    [[nodiscard]] std::size_t total_calls() const noexcept {
        return query_calls + exec_calls + query_each_calls;
    }

    katana::sql::executor& inner_;
    std::size_t query_calls = 0;
    std::size_t exec_calls = 0;
    std::size_t query_each_calls = 0;
    std::vector<std::string> statement_names;
};

request make_request(method http_method, std::string_view uri, std::string_view body = {}) {
    request req;
    req.http_method = http_method;
    req.uri = uri;
    req.headers = headers_map(nullptr);
    req.headers.set(field::accept, "application/json");
    if (!body.empty()) {
        req.headers.set(field::content_type, "application/json");
    }
    req.body = body;
    return req;
}

response dispatch_request(auto& router, const request& req) {
    katana::monotonic_arena arena;
    request_context ctx{arena};
    response out;
    auto dispatched = router.dispatch_to(req, ctx, out);
    EXPECT_TRUE(dispatched);
    return out;
}

std::optional<int64_t> extract_id(std::string_view json) {
    const auto key_pos = json.find("\"id\":");
    if (key_pos == std::string_view::npos) {
        return std::nullopt;
    }

    std::size_t pos = key_pos + 5;
    while (pos < json.size() && std::isspace(static_cast<unsigned char>(json[pos]))) {
        ++pos;
    }

    int64_t value = 0;
    const auto* begin = json.data() + static_cast<std::ptrdiff_t>(pos);
    const auto* end = json.data() + static_cast<std::ptrdiff_t>(json.size());
    const auto [ptr, ec] = std::from_chars(begin, end, value);
    if (ec != std::errc{}) {
        return std::nullopt;
    }
    return value;
}

} // namespace

TEST(BenchmarkApiSqlDemoIntegration, CrudLifecycleOverGeneratedRouter) {
    const char* dsn = postgres_dsn();
    if (dsn == nullptr || *dsn == '\0') {
        std::cout << "[sql-demo] KATANA_TEST_POSTGRES_DSN is not set; skipping CRUD lifecycle "
                     "integration body\n";
        return;
    }

    service demo({
        .connection_string = dsn,
        .executor_count = 1,
        .eager_connect = true,
        .bootstrap_schema = true,
        .reset_data_on_start = true,
        .seed_item_count = 4,
    });
    ASSERT_TRUE(demo.start());

    auto router = generated::make_fast_router(demo.handler());

    request create = make_request(
        method::post,
        "/items",
        R"({"name":"Drill","description":"Cordless drill","price":119.99,"stock":25,"category":"tools"})");
    create.headers.set_view("X-Request-Id", "550e8400-e29b-41d4-a716-446655440001");
    create.headers.set(field::cookie, "session=integration");

    const response created = dispatch_request(router, create);
    ASSERT_EQ(created.status, 201);
    EXPECT_NE(created.body.find("\"Drill\""), std::string::npos);

    const auto created_id = extract_id(created.body);
    ASSERT_TRUE(created_id.has_value());

    const response fetched = dispatch_request(
        router, make_request(method::get, "/items/" + std::to_string(*created_id)));
    ASSERT_EQ(fetched.status, 200);
    EXPECT_NE(fetched.body.find("\"Cordless drill\""), std::string::npos);

    const response listed = dispatch_request(
        router, make_request(method::get, "/items?limit=10&offset=0&category=tools"));
    ASSERT_EQ(listed.status, 200);
    EXPECT_NE(listed.body.find("\"total\":"), std::string::npos);
    EXPECT_NE(listed.body.find("\"Drill\""), std::string::npos);

    const response updated =
        dispatch_request(router,
                         make_request(method::put,
                                      "/items/" + std::to_string(*created_id),
                                      R"({"name":"Drill X","price":109.99,"stock":20})"));
    ASSERT_EQ(updated.status, 200);
    EXPECT_NE(updated.body.find("\"Drill X\""), std::string::npos);

    const response fetched_updated = dispatch_request(
        router, make_request(method::get, "/items/" + std::to_string(*created_id)));
    ASSERT_EQ(fetched_updated.status, 200);
    EXPECT_NE(fetched_updated.body.find("\"Drill X\""), std::string::npos);

    const response deleted = dispatch_request(
        router, make_request(method::del, "/items/" + std::to_string(*created_id)));
    ASSERT_EQ(deleted.status, 204);

    const response missing = dispatch_request(
        router, make_request(method::get, "/items/" + std::to_string(*created_id)));
    ASSERT_EQ(missing.status, 404);

    const response delete_missing = dispatch_request(
        router, make_request(method::del, "/items/" + std::to_string(*created_id)));
    ASSERT_EQ(delete_missing.status, 404);
}

TEST(BenchmarkApiSqlDemoIntegration, EmptyPagePreservesTotalCount) {
    const char* dsn = postgres_dsn();
    if (dsn == nullptr || *dsn == '\0') {
        std::cout << "[sql-demo] KATANA_TEST_POSTGRES_DSN is not set; skipping empty-page "
                     "integration body\n";
        return;
    }

    service demo({
        .connection_string = dsn,
        .executor_count = 1,
        .eager_connect = true,
        .bootstrap_schema = true,
        .reset_data_on_start = true,
        .seed_item_count = 3,
    });
    ASSERT_TRUE(demo.start());

    auto router = generated::make_fast_router(demo.handler());
    const response listed = dispatch_request(
        router, make_request(method::get, "/items?limit=5&offset=999&category=tools"));

    ASSERT_EQ(listed.status, 200);
    EXPECT_NE(listed.body.find("\"items\":[]"), std::string::npos);
    EXPECT_NE(listed.body.find("\"total\":"), std::string::npos);
}

TEST(BenchmarkApiSqlDemoIntegration, CanonicalCrudUsesSingleSqlRoundTripPerOperation) {
    const char* dsn = postgres_dsn();
    if (dsn == nullptr || *dsn == '\0') {
        std::cout << "[sql-demo] KATANA_TEST_POSTGRES_DSN is not set; skipping SQL round-trip "
                     "integration body\n";
        return;
    }

    service demo({
        .connection_string = dsn,
        .executor_count = 1,
        .eager_connect = true,
        .bootstrap_schema = true,
        .reset_data_on_start = true,
        .seed_item_count = 12,
    });
    ASSERT_TRUE(demo.start());

    katana::sql::postgres_pool_executor pool_executor(demo.pool());
    counting_executor counting(pool_executor);
    auto backend = katana::benchmark_api_demo::make_sql_item_backend(counting);
    auto handler = katana::benchmark_api_demo::make_handler(*backend);
    auto router = generated::make_fast_router(*handler);

    request create = make_request(
        method::post,
        "/items",
        R"({"name":"Stage4 Drill","description":"Round-trip check","price":149.99,"stock":8,"category":"tools"})");
    create.headers.set_view("X-Request-Id", "550e8400-e29b-41d4-a716-446655440002");
    create.headers.set(field::cookie, "session=integration");

    std::size_t total_before = counting.total_calls();
    const response created = dispatch_request(router, create);
    ASSERT_EQ(created.status, 201);
    ASSERT_EQ(counting.total_calls(), total_before + 1U);
    ASSERT_EQ(counting.query_calls, 0U);
    ASSERT_EQ(counting.exec_calls, 0U);
    ASSERT_EQ(counting.query_each_calls, 1U);
    ASSERT_FALSE(counting.statement_names.empty());
    EXPECT_EQ(counting.statement_names.back(), "create_item");

    const auto created_id = extract_id(created.body);
    ASSERT_TRUE(created_id.has_value());

    total_before = counting.total_calls();
    const response listed = dispatch_request(
        router, make_request(method::get, "/items?limit=20&offset=0&category=tools"));
    ASSERT_EQ(listed.status, 200);
    ASSERT_EQ(counting.total_calls(), total_before + 1U);
    EXPECT_EQ(counting.statement_names.back(), "list_items_page_by_category");
    EXPECT_NE(listed.body.find("\"Stage4 Drill\""), std::string::npos);

    total_before = counting.total_calls();
    const response fetched = dispatch_request(
        router, make_request(method::get, "/items/" + std::to_string(*created_id)));
    ASSERT_EQ(fetched.status, 200);
    ASSERT_EQ(counting.total_calls(), total_before + 1U);
    EXPECT_EQ(counting.statement_names.back(), "get_item");
    EXPECT_NE(fetched.body.find("\"Round-trip check\""), std::string::npos);

    total_before = counting.total_calls();
    const response updated =
        dispatch_request(router,
                         make_request(method::put,
                                      "/items/" + std::to_string(*created_id),
                                      R"({"name":"Stage4 Drill X","price":139.99,"stock":5})"));
    ASSERT_EQ(updated.status, 200);
    ASSERT_EQ(counting.total_calls(), total_before + 1U);
    EXPECT_EQ(counting.statement_names.back(), "update_item");
    EXPECT_NE(updated.body.find("\"Stage4 Drill X\""), std::string::npos);

    total_before = counting.total_calls();
    const response deleted = dispatch_request(
        router, make_request(method::del, "/items/" + std::to_string(*created_id)));
    ASSERT_EQ(deleted.status, 204);
    ASSERT_EQ(counting.total_calls(), total_before + 1U);
    EXPECT_EQ(counting.statement_names.back(), "delete_item");
    EXPECT_EQ(counting.exec_calls, 1U);

    total_before = counting.total_calls();
    const response missing = dispatch_request(
        router, make_request(method::get, "/items/" + std::to_string(*created_id)));
    ASSERT_EQ(missing.status, 404);
    ASSERT_EQ(counting.total_calls(), total_before + 1U);
    EXPECT_EQ(counting.statement_names.back(), "get_item");

    EXPECT_EQ(counting.query_calls, 0U);
    EXPECT_EQ(counting.query_each_calls, 5U);
    EXPECT_EQ(counting.exec_calls, 1U);
}

TEST(BenchmarkApiSqlDemoIntegration, GeneratedRepositorySupportsAsyncCrudWrites) {
    const char* dsn = postgres_dsn();
    if (dsn == nullptr || *dsn == '\0') {
        std::cout << "[sql-demo] KATANA_TEST_POSTGRES_DSN is not set; skipping generated async "
                     "CRUD write integration body\n";
        return;
    }

    service demo({
        .connection_string = dsn,
        .executor_count = 1,
        .eager_connect = true,
        .bootstrap_schema = true,
        .reset_data_on_start = true,
        .seed_item_count = 0,
    });
    ASSERT_TRUE(demo.start());

    katana::sql::postgres_pool_executor pool_executor(demo.pool());
    katana::sql::generated::generated_repository repo(pool_executor);

    std::promise<katana::result<std::optional<katana::sql::generated::CreateItemRow>>>
        create_promise;
    ASSERT_TRUE(repo.create_item_async("Async Drill",
                                       true,
                                       "Generated async create",
                                       199.50,
                                       true,
                                       11,
                                       "tools",
                                       [&create_promise](auto result) {
                                           create_promise.set_value(std::move(result));
                                       }));
    auto created = create_promise.get_future().get();
    ASSERT_TRUE(created);
    ASSERT_TRUE(created->has_value());
    ASSERT_EQ(created->value().name, std::optional<std::string>("Async Drill"));
    ASSERT_TRUE(created->value().id.has_value());

    const auto created_id = *created->value().id;

    std::promise<katana::result<std::optional<katana::sql::generated::UpdateItemRow>>>
        update_promise;
    ASSERT_TRUE(repo.update_item_async(created_id,
                                       true,
                                       "Async Drill X",
                                       true,
                                       "Generated async update",
                                       true,
                                       149.25,
                                       true,
                                       7,
                                       true,
                                       "electronics",
                                       [&update_promise](auto result) {
                                           update_promise.set_value(std::move(result));
                                       }));
    auto updated = update_promise.get_future().get();
    ASSERT_TRUE(updated);
    ASSERT_TRUE(updated->has_value());
    EXPECT_EQ(updated->value().name, std::optional<std::string>("Async Drill X"));
    EXPECT_EQ(updated->value().category_name, std::optional<std::string>("electronics"));

    std::promise<katana::result<katana::sql::exec_result>> delete_promise;
    ASSERT_TRUE(repo.delete_item_async(created_id, [&delete_promise](auto result) {
        delete_promise.set_value(std::move(result));
    }));
    auto deleted = delete_promise.get_future().get();
    ASSERT_TRUE(deleted);
    EXPECT_EQ(deleted->affected_rows, 1U);
}

#else

TEST(BenchmarkApiSqlDemoIntegration, LibpqUnavailable) {
    EXPECT_TRUE(true);
}

#endif
