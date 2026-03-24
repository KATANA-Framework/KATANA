#include "demo_service.hpp"

#include <gtest/gtest.h>

#include <charconv>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

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

    const response deleted = dispatch_request(
        router, make_request(method::del, "/items/" + std::to_string(*created_id)));
    ASSERT_EQ(deleted.status, 204);

    const response missing = dispatch_request(
        router, make_request(method::get, "/items/" + std::to_string(*created_id)));
    ASSERT_EQ(missing.status, 404);
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

#else

TEST(BenchmarkApiSqlDemoIntegration, LibpqUnavailable) {
    EXPECT_TRUE(true);
}

#endif
