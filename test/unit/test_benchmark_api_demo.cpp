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

struct fake_async_backend final : item_backend {
    bool list_async_called = false;
    bool get_async_called = false;
    bool create_async_called = false;
    bool update_async_called = false;
    bool delete_async_called = false;

    std::size_t last_limit = 0;
    std::size_t last_offset = 0;
    std::optional<std::string> last_category;
    int64_t last_id = 0;
    std::string last_name;
    std::optional<std::string> last_description;
    double last_price = 0.0;
    std::optional<int64_t> last_stock;
    std::optional<ItemCategory_enum> last_item_category;

    [[nodiscard]] bool supports_async_dispatch() const noexcept override { return true; }

    katana::result<item_page>
    list_items(std::size_t, std::size_t, std::optional<std::string_view>) override {
        return item_page{};
    }

    katana::result<std::optional<item_record>> get_item(int64_t) override {
        return std::optional<item_record>{};
    }

    katana::result<item_record> create_item(const create_item_command&) override {
        return item_record{};
    }

    katana::result<std::optional<item_record>> update_item(int64_t,
                                                           const update_item_command&) override {
        return std::optional<item_record>{};
    }

    katana::result<bool> delete_item(int64_t) override { return true; }

    bool list_items_async(std::size_t limit,
                          std::size_t offset,
                          std::optional<std::string_view> category,
                          katana::http::async_response_writer) override {
        list_async_called = true;
        last_limit = limit;
        last_offset = offset;
        last_category = category ? std::optional<std::string>(std::string(*category))
                                 : std::optional<std::string>{};
        return true;
    }

    bool get_item_async(int64_t id, katana::http::async_response_writer) override {
        get_async_called = true;
        last_id = id;
        return true;
    }

    bool create_item_async(const create_item_command& command,
                           katana::http::async_response_writer) override {
        create_async_called = true;
        last_name = std::string(command.name);
        last_description = command.description ? std::optional<std::string>(std::string(*command.description))
                                               : std::optional<std::string>{};
        last_price = command.price;
        last_stock = command.stock;
        last_item_category = command.category;
        return true;
    }

    bool update_item_async(int64_t id,
                           const update_item_command& command,
                           katana::http::async_response_writer) override {
        update_async_called = true;
        last_id = id;
        last_name = command.name ? std::string(*command.name) : std::string{};
        last_description = command.description ? std::optional<std::string>(std::string(*command.description))
                                               : std::optional<std::string>{};
        last_price = command.price.value_or(0.0);
        last_stock = command.stock;
        last_item_category = command.category;
        return true;
    }

    bool delete_item_async(int64_t id, katana::http::async_response_writer) override {
        delete_async_called = true;
        last_id = id;
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

TEST(BenchmarkApiDemoUnit, AsyncMethodsDelegateThroughBackendContract) {
    fake_async_backend backend;
    auto handler = make_handler(backend);
    auto* async_handler = dynamic_cast<generated::async_api_handler*>(handler.get());
    ASSERT_NE(async_handler, nullptr);

    katana::monotonic_arena arena;
    CreateItemRequest create_body(&arena);
    create_body.name = arena_string<>("Async Drill", arena_allocator<char>(&arena));
    create_body.description = arena_string<>("Backend delegated", arena_allocator<char>(&arena));
    create_body.price = 149.5;
    create_body.stock = 6;
    create_body.category = ItemCategory_enum::tools;

    ASSERT_TRUE(async_handler->list_items_async(15, 4, std::string_view("tools"), {}));
    EXPECT_TRUE(backend.list_async_called);
    EXPECT_EQ(backend.last_limit, 15u);
    EXPECT_EQ(backend.last_offset, 4u);
    ASSERT_TRUE(backend.last_category.has_value());
    EXPECT_EQ(*backend.last_category, "tools");

    ASSERT_TRUE(async_handler->get_item_async(42, {}));
    EXPECT_TRUE(backend.get_async_called);
    EXPECT_EQ(backend.last_id, 42);

    ASSERT_TRUE(async_handler->create_item_async("550e8400-e29b-41d4-a716-446655440010",
                                                 std::optional<std::string_view>("session-1"),
                                                 create_body,
                                                 {}));
    EXPECT_TRUE(backend.create_async_called);
    EXPECT_EQ(backend.last_name, "Async Drill");
    ASSERT_TRUE(backend.last_description.has_value());
    EXPECT_EQ(*backend.last_description, "Backend delegated");
    EXPECT_EQ(backend.last_price, 149.5);
    EXPECT_EQ(backend.last_stock, std::optional<int64_t>(6));
    EXPECT_EQ(backend.last_item_category, std::optional<ItemCategory_enum>(ItemCategory_enum::tools));

    UpdateItemRequest update_body(&arena);
    update_body.name = arena_string<>("Async Drill X", arena_allocator<char>(&arena));
    update_body.description = arena_string<>("Updated async", arena_allocator<char>(&arena));
    update_body.price = 129.0;
    update_body.stock = 3;
    update_body.category = ItemCategory_enum::electronics;

    ASSERT_TRUE(async_handler->update_item_async(42, update_body, {}));
    EXPECT_TRUE(backend.update_async_called);
    EXPECT_EQ(backend.last_id, 42);
    EXPECT_EQ(backend.last_name, "Async Drill X");
    ASSERT_TRUE(backend.last_description.has_value());
    EXPECT_EQ(*backend.last_description, "Updated async");
    EXPECT_EQ(backend.last_price, 129.0);
    EXPECT_EQ(backend.last_stock, std::optional<int64_t>(3));
    EXPECT_EQ(backend.last_item_category,
              std::optional<ItemCategory_enum>(ItemCategory_enum::electronics));

    ASSERT_TRUE(async_handler->delete_item_async(42, {}));
    EXPECT_TRUE(backend.delete_async_called);
    EXPECT_EQ(backend.last_id, 42);
}

#else

TEST(BenchmarkApiDemoUnit, LibpqUnavailable) {
    EXPECT_TRUE(true);
}

#endif
