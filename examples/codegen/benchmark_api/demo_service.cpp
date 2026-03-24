#include "demo_service.hpp"

#ifdef KATANA_HAS_LIBPQ

#include "katana/core/handler_context.hpp"
#include "katana/core/problem.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <system_error>
#include <utility>

namespace katana::benchmark_api_demo {

namespace {

using sql_repo = katana::sql::generated::generated_repository;

void debug_backend_error(std::string_view operation, const std::error_code& error) {
    if (const char* debug = std::getenv("KATANA_BENCHMARK_API_DEBUG_ERRORS")) {
        if (*debug != '\0' && *debug != '0') {
            std::cerr << "[benchmark_api_demo] " << operation << " failed: " << error.message()
                      << " (" << error.value() << ")\n";
        }
    }
}

template <typename Row> katana::result<item_record> row_to_item(const Row& row) {
    if (!row.id || !row.name || !row.price || !row.category_name) {
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    }

    const auto category = ItemCategory_enum_from_string(*row.category_name);
    if (!category) {
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    }

    item_record out;
    out.id = *row.id;
    out.name = *row.name;
    out.description = row.description;
    out.price = *row.price;
    out.stock = row.stock;
    out.category = *category;
    return out;
}

void fill_item_dto(Item& out, const item_record& item, monotonic_arena& arena) {
    out.id = item.id;
    out.name = arena_string<>(item.name, arena_allocator<char>(&arena));
    if (item.description) {
        out.description = arena_string<>(*item.description, arena_allocator<char>(&arena));
    }
    out.price = item.price;
    if (item.stock) {
        out.stock = *item.stock;
    }
    out.category = item.category;
}

class sql_item_backend final : public item_backend {
public:
    explicit sql_item_backend(katana::sql::executor& executor) : repo_(executor) {}

    katana::result<item_page> list_items(std::size_t limit,
                                         std::size_t offset,
                                         std::optional<std::string_view> category) override {
        item_page page;
        page.limit = static_cast<int64_t>(limit);
        page.offset = static_cast<int64_t>(offset);

        const auto map_rows = [&](const auto& rows) -> katana::result<void> {
            page.items.reserve(rows.size());
            for (const auto& row : rows) {
                if (row.total_count) {
                    page.total = *row.total_count;
                }
                if (!row.id) {
                    continue;
                }
                auto mapped = row_to_item(row);
                if (!mapped) {
                    debug_backend_error("list_items.map", mapped.error());
                    return std::unexpected(mapped.error());
                }
                page.items.push_back(std::move(*mapped));
            }
            return {};
        };

        if (category) {
            auto rows = repo_.list_items_page_by_category(static_cast<int64_t>(limit),
                                                          static_cast<int64_t>(offset),
                                                          *category);
            if (!rows) {
                debug_backend_error("list_items.sql.filtered", rows.error());
                return std::unexpected(rows.error());
            }
            auto mapped = map_rows(*rows);
            if (!mapped) {
                return std::unexpected(mapped.error());
            }
        } else {
            auto rows =
                repo_.list_items_page_all(static_cast<int64_t>(limit), static_cast<int64_t>(offset));
            if (!rows) {
                debug_backend_error("list_items.sql.all", rows.error());
                return std::unexpected(rows.error());
            }
            auto mapped = map_rows(*rows);
            if (!mapped) {
                return std::unexpected(mapped.error());
            }
        }

        return page;
    }

    katana::result<std::optional<item_record>> get_item(int64_t id) override {
        auto row = repo_.get_item(id);
        if (!row) {
            debug_backend_error("get_item.sql", row.error());
            return std::unexpected(row.error());
        }
        if (!row->has_value()) {
            return std::optional<item_record>{};
        }
        auto mapped = row_to_item(row->value());
        if (!mapped) {
            debug_backend_error("get_item.map", mapped.error());
            return std::unexpected(mapped.error());
        }
        return std::optional<item_record>(std::move(*mapped));
    }

    katana::result<item_record> create_item(const create_item_command& command) override {
        const auto category = to_string(command.category);
        auto row = repo_.create_item(command.name,
                                     command.description.has_value(),
                                     command.description.value_or(std::string_view{}),
                                     command.price,
                                     command.stock.has_value(),
                                     command.stock.value_or(0),
                                     category);
        if (!row) {
            debug_backend_error("create_item.sql", row.error());
            return std::unexpected(row.error());
        }
        if (!row->has_value()) {
            return std::unexpected(std::make_error_code(std::errc::io_error));
        }
        return row_to_item(row->value());
    }

    katana::result<std::optional<item_record>>
    update_item(int64_t id, const update_item_command& command) override {
        const auto category =
            command.category ? std::optional<std::string_view>(to_string(*command.category))
                             : std::optional<std::string_view>{};
        auto row = repo_.update_item(id,
                                     command.name.has_value(),
                                     command.name.value_or(std::string_view{}),
                                     command.description.has_value(),
                                     command.description.value_or(std::string_view{}),
                                     command.price.has_value(),
                                     command.price.value_or(0.0),
                                     command.stock.has_value(),
                                     command.stock.value_or(0),
                                     category.has_value(),
                                     category.value_or(std::string_view{}));
        if (!row) {
            debug_backend_error("update_item.sql", row.error());
            return std::unexpected(row.error());
        }
        if (!row->has_value()) {
            return std::optional<item_record>{};
        }
        auto mapped = row_to_item(row->value());
        if (!mapped) {
            debug_backend_error("update_item.map", mapped.error());
            return std::unexpected(mapped.error());
        }
        return std::optional<item_record>(std::move(*mapped));
    }

    katana::result<bool> delete_item(int64_t id) override {
        auto result = repo_.delete_item(id);
        if (!result) {
            debug_backend_error("delete_item.sql", result.error());
            return std::unexpected(result.error());
        }
        return result->affected_rows != 0U;
    }

private:
    sql_repo repo_;
};

class benchmark_handler final : public generated::api_handler {
public:
    explicit benchmark_handler(item_backend& items)
        : items_(items), start_time_(std::chrono::steady_clock::now()) {}

    result<void> compute_sum(const SumRequest& req, response& out) override {
        double sum = 0.0;
        for (double value : req.values) {
            sum += value;
        }

        auto& arena = katana::http::handler_context::arena();
        SumResponse resp(&arena);
        resp.result = sum;
        resp.count = static_cast<int64_t>(req.values.size());
        out.assign_json(serialize_SumResponse(resp));
        return {};
    }

    result<void> compute_stats(const StatsRequest& req, response& out) override {
        const auto& values = req.values;
        if (values.empty()) {
            out.assign_error(problem_details::bad_request("values must not be empty"));
            return {};
        }

        double sum = 0.0;
        double min_value = values.front();
        double max_value = values.front();
        for (double value : values) {
            sum += value;
            min_value = std::min(min_value, value);
            max_value = std::max(max_value, value);
        }

        auto& arena = katana::http::handler_context::arena();
        StatsResponse resp(&arena);
        resp.min = min_value;
        resp.max = max_value;
        resp.sum = sum;
        resp.mean = sum / static_cast<double>(values.size());
        resp.count = static_cast<int64_t>(values.size());

        if (req.include_median.value_or(false)) {
            std::vector<double> sorted(values.begin(), values.end());
            std::sort(sorted.begin(), sorted.end());
            const std::size_t size = sorted.size();
            resp.median = (size % 2U == 0U) ? (sorted[size / 2U - 1U] + sorted[size / 2U]) * 0.5
                                            : sorted[size / 2U];
        }

        out.assign_json(serialize_StatsResponse(resp));
        return {};
    }

    result<void> register_user(const RegisterRequest& req, response& out) override {
        auto& arena = katana::http::handler_context::arena();
        UserResponse resp(&arena);
        resp.id =
            arena_string<>("550e8400-e29b-41d4-a716-446655440000", arena_allocator<char>(&arena));
        resp.username =
            arena_string<>(req.username.data(), req.username.size(), arena_allocator<char>(&arena));
        resp.email =
            arena_string<>(req.email.data(), req.email.size(), arena_allocator<char>(&arena));
        resp.role = req.role.value_or(UserRole_enum::user);
        resp.created_at = arena_string<>("2026-01-01T00:00:00Z", arena_allocator<char>(&arena));
        out.assign_json(serialize_UserResponse(resp), 201, "Created");
        return {};
    }

    result<void> list_items(std::optional<int64_t> limit,
                            std::optional<int64_t> offset,
                            std::optional<std::string_view> category,
                            response& out) override {
        if (category && !ItemCategory_enum_from_string(*category)) {
            out.assign_error(problem_details::bad_request("invalid category"));
            return {};
        }

        const std::size_t resolved_limit =
            static_cast<std::size_t>(std::max<int64_t>(1, limit.value_or(20)));
        const std::size_t resolved_offset =
            static_cast<std::size_t>(std::max<int64_t>(0, offset.value_or(0)));

        auto page = items_.list_items(resolved_limit, resolved_offset, category);
        if (!page) {
            debug_backend_error("list_items.handler", page.error());
            out.assign_error(problem_details::internal_server_error("list_items failed"));
            return {};
        }

        auto& arena = katana::http::handler_context::arena();
        ItemList resp(&arena);
        resp.total = page->total;
        resp.limit = page->limit;
        resp.offset = page->offset;
        resp.items.reserve(page->items.size());
        for (const auto& item : page->items) {
            Item dto(&arena);
            fill_item_dto(dto, item, arena);
            resp.items.push_back(std::move(dto));
        }

        out.assign_json(serialize_ItemList(resp));
        return {};
    }

    result<void> create_item([[maybe_unused]] std::string_view X_Request_Id,
                             [[maybe_unused]] std::optional<std::string_view> session,
                             const CreateItemRequest& body,
                             response& out) override {
        create_item_command command{
            .name = body.name,
            .description = body.description ? std::optional<std::string_view>(*body.description)
                                            : std::optional<std::string_view>{},
            .price = body.price,
            .stock = body.stock,
            .category = body.category,
        };

        auto created = items_.create_item(command);
        if (!created) {
            out.assign_error(problem_details::internal_server_error("create_item failed"));
            return {};
        }

        auto& arena = katana::http::handler_context::arena();
        Item dto(&arena);
        fill_item_dto(dto, *created, arena);
        katana::http::respond::into(out).created_json(serialize_Item(dto));
        return {};
    }

    result<void> get_item(int64_t id, response& out) override {
        auto item = items_.get_item(id);
        if (!item) {
            out.assign_error(problem_details::internal_server_error("get_item failed"));
            return {};
        }
        if (!*item) {
            out.assign_error(problem_details::not_found("Item not found"));
            return {};
        }

        auto& arena = katana::http::handler_context::arena();
        Item dto(&arena);
        fill_item_dto(dto, **item, arena);
        out.assign_json(serialize_Item(dto));
        return {};
    }

    result<void> update_item(int64_t id, const UpdateItemRequest& body, response& out) override {
        update_item_command command{
            .name = body.name ? std::optional<std::string_view>(*body.name)
                              : std::optional<std::string_view>{},
            .description = body.description
                               ? std::optional<std::string_view>(*body.description)
                               : std::optional<std::string_view>{},
            .price = body.price,
            .stock = body.stock,
            .category = body.category,
        };

        auto item = items_.update_item(id, command);
        if (!item) {
            out.assign_error(problem_details::internal_server_error("update_item failed"));
            return {};
        }
        if (!*item) {
            out.assign_error(problem_details::not_found("Item not found"));
            return {};
        }

        auto& arena = katana::http::handler_context::arena();
        Item dto(&arena);
        fill_item_dto(dto, **item, arena);
        out.assign_json(serialize_Item(dto));
        return {};
    }

    result<void> delete_item(int64_t id, response& out) override {
        auto removed = items_.delete_item(id);
        if (!removed) {
            out.assign_error(problem_details::internal_server_error("delete_item failed"));
            return {};
        }
        if (!*removed) {
            out.assign_error(problem_details::not_found("Item not found"));
            return {};
        }

        katana::http::respond::into(out).no_content();
        return {};
    }

    result<void> echo(const EchoRequest& req, response& out) override {
        std::string payload(req.message.data(), req.message.size());
        const int repeat_count = static_cast<int>(req.repeat.value_or(1));
        if (repeat_count > 1) {
            const std::string base = payload;
            payload.reserve(base.size() * static_cast<std::size_t>(repeat_count));
            for (int i = 1; i < repeat_count; ++i) {
                payload += base;
            }
        }
        if (req.uppercase.value_or(false)) {
            for (char& c : payload) {
                c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
            }
        }

        auto& arena = katana::http::handler_context::arena();
        EchoResponse resp(&arena);
        resp.message = arena_string<>(payload, arena_allocator<char>(&arena));
        resp.length = static_cast<int64_t>(payload.size());
        out.assign_json(serialize_EchoResponse(resp));
        return {};
    }

    result<void> health_check(response& out) override {
        const auto now = std::chrono::steady_clock::now();
        const auto uptime_ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time_).count();
        out.assign_json(R"({"status":"ok","uptime_ms":)" + std::to_string(uptime_ms) + "}");
        return {};
    }

private:
    item_backend& items_;
    std::chrono::steady_clock::time_point start_time_;
};

constexpr std::string_view create_table_sql = "CREATE TABLE IF NOT EXISTS katana_stage4_items ("
                                              "id BIGSERIAL PRIMARY KEY, "
                                              "name TEXT NOT NULL, "
                                              "description TEXT, "
                                              "price DOUBLE PRECISION NOT NULL, "
                                              "stock BIGINT, "
                                              "category TEXT NOT NULL, "
                                              "created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(), "
                                              "updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW()"
                                              ")";

constexpr std::string_view category_index_sql =
    "CREATE INDEX IF NOT EXISTS katana_stage4_items_category_id_cover_idx "
    "ON katana_stage4_items (category, id) INCLUDE (name, description, price, stock)";

constexpr std::string_view reset_items_sql = "TRUNCATE katana_stage4_items RESTART IDENTITY";
constexpr std::string_view analyze_items_sql = "ANALYZE katana_stage4_items";

constexpr std::string_view seed_items_sql =
    "INSERT INTO katana_stage4_items (name, description, price, stock, category) "
    "SELECT "
    "'Seed-' || gs::text, "
    "'Seed item', "
    "10.0 + (gs % 100)::double precision, "
    "100 + (gs % 50)::bigint, "
    "CASE (gs % 6) "
    "  WHEN 0 THEN 'electronics' "
    "  WHEN 1 THEN 'books' "
    "  WHEN 2 THEN 'clothing' "
    "  WHEN 3 THEN 'food' "
    "  WHEN 4 THEN 'tools' "
    "  ELSE 'other' "
    "END "
    "FROM generate_series(1, $1::bigint) AS gs";

} // namespace

std::unique_ptr<generated::api_handler> make_handler(item_backend& items) {
    return std::make_unique<benchmark_handler>(items);
}

std::unique_ptr<item_backend> make_sql_item_backend(katana::sql::executor& executor) {
    return std::make_unique<sql_item_backend>(executor);
}

service::service(service_config config)
    : config_(std::move(config)), pool_({
                                      .postgres = {.connection_string = config_.connection_string},
                                      .executor_count = config_.executor_count,
                                      .eager_connect = config_.eager_connect,
                                  }),
      pool_executor_(pool_) {
    items_ = make_sql_item_backend(pool_executor_);
    handler_ = make_handler(*items_);
}

service::~service() {
    stop();
}

katana::result<void> service::start() {
    if (config_.connection_string.empty()) {
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    }

    auto connected = pool_.connect_all();
    if (!connected) {
        return connected;
    }

    if (!config_.bootstrap_schema) {
        return {};
    }

    auto created = ensure_schema();
    if (!created) {
        return created;
    }

    if (config_.reset_data_on_start) {
        auto cleared = reset_items();
        if (!cleared) {
            return cleared;
        }
    }

    if (config_.seed_item_count != 0U) {
        auto seeded = seed_items(config_.seed_item_count);
        if (!seeded) {
            return seeded;
        }
        auto analyzed = analyze_items();
        if (!analyzed) {
            return analyzed;
        }
    }

    return {};
}

void service::stop() noexcept {
    pool_.disconnect_all();
}

generated::api_handler& service::handler() noexcept {
    return *handler_;
}

item_backend& service::items() noexcept {
    return *items_;
}

katana::sql::postgres_pool& service::pool() noexcept {
    return pool_;
}

katana::result<void> service::ensure_schema() {
    auto& executor = pool_.current_executor();
    auto created_table = executor.exec("benchmark_api_create_items_table", create_table_sql, {});
    if (!created_table) {
        return std::unexpected(created_table.error());
    }
    auto created_index =
        executor.exec("benchmark_api_create_items_category_idx", category_index_sql, {});
    if (!created_index) {
        return std::unexpected(created_index.error());
    }
    return {};
}

katana::result<void> service::reset_items() {
    auto& executor = pool_.current_executor();
    auto cleared = executor.exec("benchmark_api_reset_items", reset_items_sql, {});
    if (!cleared) {
        return std::unexpected(cleared.error());
    }
    return {};
}

katana::result<void> service::seed_items(std::size_t count) {
    auto& executor = pool_.current_executor();
    katana::sql::postgres_transaction tx(executor);
    auto begun = tx.begin();
    if (!begun) {
        return begun;
    }

    auto inserted = executor.exec("benchmark_api_seed_items",
                                  seed_items_sql,
                                  {katana::sql::encode_value(static_cast<int64_t>(count))});
    if (!inserted) {
        (void)tx.rollback();
        return std::unexpected(inserted.error());
    }

    return tx.commit();
}

katana::result<void> service::analyze_items() {
    auto& executor = pool_.current_executor();
    auto analyzed = executor.exec("benchmark_api_analyze_items", analyze_items_sql, {});
    if (!analyzed) {
        return std::unexpected(analyzed.error());
    }
    return {};
}

} // namespace katana::benchmark_api_demo

#endif
