#include "demo_service.hpp"

#include <gtest/gtest.h>

#include <optional>
#include <string>
#include <string_view>

#ifdef KATANA_HAS_LIBPQ

namespace {

using katana::benchmark_api_demo::create_item_command;
using katana::benchmark_api_demo::item_backend;
using katana::benchmark_api_demo::item_page;
using katana::benchmark_api_demo::item_record;
using katana::benchmark_api_demo::make_handler;
using katana::benchmark_api_demo::update_item_command;
using katana::http::field;
using katana::http::headers_map;
using katana::http::method;
using katana::http::request;
using katana::http::request_context;
using katana::http::response;

struct fake_backend final : item_backend {
    int list_calls = 0;
    int get_calls = 0;
    int create_calls = 0;
    int update_calls = 0;
    int delete_calls = 0;
    std::size_t last_limit = 0;
    std::size_t last_offset = 0;
    std::optional<std::string> last_category;

    katana::result<item_page> list_items(std::size_t limit,
                                         std::size_t offset,
                                         std::optional<std::string_view> category) override {
        ++list_calls;
        last_limit = limit;
        last_offset = offset;
        last_category = category ? std::optional<std::string>(std::string(*category))
                                 : std::optional<std::string>{};

        item_page page;
        page.total = 1;
        page.limit = static_cast<int64_t>(limit);
        page.offset = static_cast<int64_t>(offset);
        page.items.push_back(item_record{
            .id = 42,
            .name = "Bench Item",
            .description = std::string("Generated through backend"),
            .price = 99.0,
            .stock = 7,
            .category = ItemCategory_enum::tools,
        });
        return page;
    }

    katana::result<std::optional<item_record>> get_item(int64_t) override {
        ++get_calls;
        return std::optional<item_record>{};
    }

    katana::result<item_record> create_item(const create_item_command&) override {
        ++create_calls;
        return item_record{};
    }

    katana::result<std::optional<item_record>> update_item(int64_t,
                                                           const update_item_command&) override {
        ++update_calls;
        return std::optional<item_record>{};
    }

    katana::result<bool> delete_item(int64_t) override {
        ++delete_calls;
        return true;
    }
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

} // namespace

TEST(BenchmarkApiDemoUnit, ListItemsUsesSingleBackendCall) {
    fake_backend backend;
    auto handler = make_handler(backend);
    auto router = generated::make_fast_router(*handler);

    request req = make_request(method::get, "/items?limit=11&offset=3&category=tools");
    katana::monotonic_arena arena;
    request_context ctx{arena};
    response out;

    auto dispatched = router.dispatch_to(req, ctx, out);
    ASSERT_TRUE(dispatched);
    EXPECT_EQ(out.status, 200);
    EXPECT_EQ(backend.list_calls, 1);
    EXPECT_EQ(backend.get_calls, 0);
    EXPECT_EQ(backend.create_calls, 0);
    EXPECT_EQ(backend.update_calls, 0);
    EXPECT_EQ(backend.delete_calls, 0);
    EXPECT_EQ(backend.last_limit, 11u);
    EXPECT_EQ(backend.last_offset, 3u);
    ASSERT_TRUE(backend.last_category.has_value());
    EXPECT_EQ(*backend.last_category, "tools");
    EXPECT_NE(out.body.find("\"total\":1"), std::string::npos);
    EXPECT_NE(out.body.find("\"Bench Item\""), std::string::npos);
}

TEST(BenchmarkApiDemoUnit, InvalidCategoryStopsBeforeBackendCall) {
    fake_backend backend;
    auto handler = make_handler(backend);
    auto router = generated::make_fast_router(*handler);

    request req = make_request(method::get, "/items?category=not-a-real-category");
    katana::monotonic_arena arena;
    request_context ctx{arena};
    response out;

    auto dispatched = router.dispatch_to(req, ctx, out);
    ASSERT_TRUE(dispatched);
    EXPECT_EQ(out.status, 400);
    EXPECT_EQ(backend.list_calls, 0);
}

#else

TEST(BenchmarkApiDemoUnit, LibpqUnavailable) {
    EXPECT_TRUE(true);
}

#endif
