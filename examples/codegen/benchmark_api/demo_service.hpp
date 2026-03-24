#pragma once

#include "benchmark_api_generated_openapi/generated_openapi_package.hpp"
#include "benchmark_api_generated_sql/generated_sql_package.hpp"
#include "katana/sql/postgres.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace katana::benchmark_api_demo {

#ifdef KATANA_HAS_LIBPQ

struct item_record {
    int64_t id = 0;
    std::string name;
    std::optional<std::string> description;
    double price = 0.0;
    std::optional<int64_t> stock;
    ItemCategory_enum category = ItemCategory_enum::other;
};

struct item_page {
    std::vector<item_record> items;
    int64_t total = 0;
    int64_t limit = 0;
    int64_t offset = 0;
};

struct create_item_command {
    std::string name;
    std::optional<std::string> description;
    double price = 0.0;
    std::optional<int64_t> stock;
    ItemCategory_enum category = ItemCategory_enum::other;
};

struct update_item_command {
    std::optional<std::string> name;
    std::optional<std::string> description;
    std::optional<double> price;
    std::optional<int64_t> stock;
    std::optional<ItemCategory_enum> category;
};

class item_backend {
public:
    virtual ~item_backend() = default;

    virtual katana::result<item_page>
    list_items(std::size_t limit, std::size_t offset, std::optional<std::string_view> category) = 0;
    virtual katana::result<std::optional<item_record>> get_item(int64_t id) = 0;
    virtual katana::result<item_record> create_item(const create_item_command& command) = 0;
    virtual katana::result<std::optional<item_record>>
    update_item(int64_t id, const update_item_command& command) = 0;
    virtual katana::result<bool> delete_item(int64_t id) = 0;
};

std::unique_ptr<generated::api_handler> make_handler(item_backend& items);

struct service_config {
    std::string connection_string;
    std::size_t executor_count = 0;
    bool eager_connect = true;
    bool bootstrap_schema = true;
    bool reset_data_on_start = true;
    std::size_t seed_item_count = 256;
};

class service {
public:
    explicit service(service_config config);
    ~service();

    service(const service&) = delete;
    service& operator=(const service&) = delete;

    katana::result<void> start();
    void stop() noexcept;

    [[nodiscard]] generated::api_handler& handler() noexcept;
    [[nodiscard]] item_backend& items() noexcept;
    [[nodiscard]] katana::sql::postgres_pool& pool() noexcept;

private:
    katana::result<void> ensure_schema();
    katana::result<void> reset_items();
    katana::result<void> seed_items(std::size_t count);

    service_config config_;
    katana::sql::postgres_pool pool_;
    katana::sql::postgres_pool_executor pool_executor_;
    std::unique_ptr<item_backend> items_;
    std::unique_ptr<generated::api_handler> handler_;
};

#endif

} // namespace katana::benchmark_api_demo
