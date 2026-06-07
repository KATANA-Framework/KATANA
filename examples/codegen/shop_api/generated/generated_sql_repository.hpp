#pragma once

#include "generated_sql_models.hpp"
#include "katana/sql/runtime.hpp"

#include <optional>
#include <string_view>
#include <utility>
#include <vector>

namespace katana::sql::generated {

class generated_repository {
public:
    using bulk_insert_order_items_async_handler = katana::inplace_function<void(katana::result<katana::sql::exec_result>), 256>;
    using category_stats_async_handler = katana::inplace_function<void(katana::result<std::vector<CategoryStatsRow>>), 256>;
    using create_order_async_handler = katana::inplace_function<void(katana::result<std::optional<CreateOrderRow>>), 256>;
    using customer_revenue_ranked_async_handler = katana::inplace_function<void(katana::result<std::vector<CustomerRevenueRankedRow>>), 256>;
    using order_detail_async_handler = katana::inplace_function<void(katana::result<std::optional<OrderDetailRow>>), 256>;
    using top_products_by_category_async_handler = katana::inplace_function<void(katana::result<std::vector<TopProductsByCategoryRow>>), 256>;

    explicit generated_repository(katana::sql::executor& executor) noexcept
        : executor_(executor) {}

    katana::result<katana::sql::exec_result> bulk_insert_order_items(const std::vector<int64_t>& p1, const std::vector<int64_t>& p2, const std::vector<int64_t>& p3, const std::vector<double>& p4) const {
        katana::sql::parameters params;
        params.reserve(4);
        params.push_back(katana::sql::encode_value(p1));
        params.push_back(katana::sql::encode_value(p2));
        params.push_back(katana::sql::encode_value(p3));
        params.push_back(katana::sql::encode_value(p4));
        return executor_.exec("bulk_insert_order_items", bulk_insert_order_items_sql, params);
    }

    bool bulk_insert_order_items_async(const std::vector<int64_t>& p1, const std::vector<int64_t>& p2, const std::vector<int64_t>& p3, const std::vector<double>& p4, bulk_insert_order_items_async_handler handler) const {
        if (!handler) {
            return false;
        }
        auto* async_executor = dynamic_cast<katana::sql::async_executor*>(&executor_);
        if (async_executor == nullptr) {
            // No async executor: run synchronously and deliver the result inline
            // so the completion always fires (returning false here would hang a
            // deferred response).
            handler(bulk_insert_order_items(p1, p2, p3, p4));
            return true;
        }
        katana::sql::parameters params;
        params.reserve(4);
        params.push_back(katana::sql::encode_value(p1));
        params.push_back(katana::sql::encode_value(p2));
        params.push_back(katana::sql::encode_value(p3));
        params.push_back(katana::sql::encode_value(p4));
        return async_executor->exec_async("bulk_insert_order_items", bulk_insert_order_items_sql, std::move(params), std::move(handler));
    }

    katana::result<std::vector<CategoryStatsRow>> category_stats() const {
        katana::sql::parameters params;
        params.reserve(0);
        std::vector<CategoryStatsRow> out_rows;
        auto status = executor_.query_each("category_stats", category_stats_sql, params,
            [&](const katana::sql::row_view& row) -> katana::result<void> {
                auto mapped = map_category_stats(row);
                if (!mapped) {
                    return std::unexpected(mapped.error());
                }
                out_rows.push_back(std::move(*mapped));
                return {};
            });
        if (!status) {
            return std::unexpected(status.error());
        }
        return out_rows;
    }

    bool category_stats_async(category_stats_async_handler handler) const {
        if (!handler) {
            return false;
        }
        auto* async_executor = dynamic_cast<katana::sql::async_executor*>(&executor_);
        if (async_executor == nullptr) {
            // No async executor: run synchronously and deliver the result inline
            // so the completion always fires (returning false here would hang a
            // deferred response).
            handler(category_stats());
            return true;
        }
        katana::sql::parameters params;
        params.reserve(0);
        return async_executor->query_async("category_stats", category_stats_sql, std::move(params),
            [handler = std::move(handler)](katana::result<katana::sql::rows> rows_result) {
                if (!rows_result) {
                    handler(std::unexpected(rows_result.error()));
                    return;
                }
                std::vector<CategoryStatsRow> out_rows;
                out_rows.reserve(rows_result->size());
                for (const auto& row : *rows_result) {
                    auto mapped = map_category_stats(row);
                    if (!mapped) {
                        handler(std::unexpected(mapped.error()));
                        return;
                    }
                    out_rows.push_back(std::move(*mapped));
                }
                handler(std::move(out_rows));
            });
    }

    katana::result<std::optional<CreateOrderRow>> create_order(int64_t p1, std::string_view p2) const {
        katana::sql::parameters params;
        params.reserve(2);
        params.push_back(katana::sql::encode_value(p1));
        params.push_back(katana::sql::encode_value(p2));
        std::optional<CreateOrderRow> out_row;
        std::size_t row_count = 0;
        auto status = executor_.query_each("create_order", create_order_sql, params,
            [&](const katana::sql::row_view& row) -> katana::result<void> {
                ++row_count;
                if (row_count != 1) {
                    return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                }
                auto mapped = map_create_order(row);
                if (!mapped) {
                    return std::unexpected(mapped.error());
                }
                out_row = std::move(*mapped);
                return {};
            });
        if (!status) {
            return std::unexpected(status.error());
        }
        if (row_count == 0) {
            return std::optional<CreateOrderRow>{};
        }
        return out_row;
    }

    bool create_order_async(int64_t p1, std::string_view p2, create_order_async_handler handler) const {
        if (!handler) {
            return false;
        }
        auto* async_executor = dynamic_cast<katana::sql::async_executor*>(&executor_);
        if (async_executor == nullptr) {
            // No async executor: run synchronously and deliver the result inline
            // so the completion always fires (returning false here would hang a
            // deferred response).
            handler(create_order(p1, p2));
            return true;
        }
        katana::sql::parameters params;
        params.reserve(2);
        params.push_back(katana::sql::encode_value(p1));
        params.push_back(katana::sql::encode_value(p2));
        return async_executor->query_async("create_order", create_order_sql, std::move(params),
            [handler = std::move(handler)](katana::result<katana::sql::rows> rows_result) {
                if (!rows_result) {
                    handler(std::unexpected(rows_result.error()));
                    return;
                }
                if (rows_result->size() > 1) {
                    handler(std::unexpected(std::make_error_code(std::errc::invalid_argument)));
                    return;
                }
                if (rows_result->empty()) {
                    handler(std::optional<CreateOrderRow>{});
                    return;
                }
                auto mapped = map_create_order(rows_result->front());
                if (!mapped) {
                    handler(std::unexpected(mapped.error()));
                    return;
                }
                handler(std::optional<CreateOrderRow>(std::move(*mapped)));
            });
    }

    katana::result<std::vector<CustomerRevenueRankedRow>> customer_revenue_ranked(std::string_view p1, int64_t p2, int64_t p3) const {
        katana::sql::parameters params;
        params.reserve(3);
        params.push_back(katana::sql::encode_value(p1));
        params.push_back(katana::sql::encode_value(p2));
        params.push_back(katana::sql::encode_value(p3));
        std::vector<CustomerRevenueRankedRow> out_rows;
        auto status = executor_.query_each("customer_revenue_ranked", customer_revenue_ranked_sql, params,
            [&](const katana::sql::row_view& row) -> katana::result<void> {
                auto mapped = map_customer_revenue_ranked(row);
                if (!mapped) {
                    return std::unexpected(mapped.error());
                }
                out_rows.push_back(std::move(*mapped));
                return {};
            });
        if (!status) {
            return std::unexpected(status.error());
        }
        return out_rows;
    }

    bool customer_revenue_ranked_async(std::string_view p1, int64_t p2, int64_t p3, customer_revenue_ranked_async_handler handler) const {
        if (!handler) {
            return false;
        }
        auto* async_executor = dynamic_cast<katana::sql::async_executor*>(&executor_);
        if (async_executor == nullptr) {
            // No async executor: run synchronously and deliver the result inline
            // so the completion always fires (returning false here would hang a
            // deferred response).
            handler(customer_revenue_ranked(p1, p2, p3));
            return true;
        }
        katana::sql::parameters params;
        params.reserve(3);
        params.push_back(katana::sql::encode_value(p1));
        params.push_back(katana::sql::encode_value(p2));
        params.push_back(katana::sql::encode_value(p3));
        return async_executor->query_async("customer_revenue_ranked", customer_revenue_ranked_sql, std::move(params),
            [handler = std::move(handler)](katana::result<katana::sql::rows> rows_result) {
                if (!rows_result) {
                    handler(std::unexpected(rows_result.error()));
                    return;
                }
                std::vector<CustomerRevenueRankedRow> out_rows;
                out_rows.reserve(rows_result->size());
                for (const auto& row : *rows_result) {
                    auto mapped = map_customer_revenue_ranked(row);
                    if (!mapped) {
                        handler(std::unexpected(mapped.error()));
                        return;
                    }
                    out_rows.push_back(std::move(*mapped));
                }
                handler(std::move(out_rows));
            });
    }

    katana::result<std::optional<OrderDetailRow>> order_detail(int64_t p1) const {
        katana::sql::parameters params;
        params.reserve(1);
        params.push_back(katana::sql::encode_value(p1));
        std::optional<OrderDetailRow> out_row;
        std::size_t row_count = 0;
        auto status = executor_.query_each("order_detail", order_detail_sql, params,
            [&](const katana::sql::row_view& row) -> katana::result<void> {
                ++row_count;
                if (row_count != 1) {
                    return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                }
                auto mapped = map_order_detail(row);
                if (!mapped) {
                    return std::unexpected(mapped.error());
                }
                out_row = std::move(*mapped);
                return {};
            });
        if (!status) {
            return std::unexpected(status.error());
        }
        if (row_count == 0) {
            return std::optional<OrderDetailRow>{};
        }
        return out_row;
    }

    bool order_detail_async(int64_t p1, order_detail_async_handler handler) const {
        if (!handler) {
            return false;
        }
        auto* async_executor = dynamic_cast<katana::sql::async_executor*>(&executor_);
        if (async_executor == nullptr) {
            // No async executor: run synchronously and deliver the result inline
            // so the completion always fires (returning false here would hang a
            // deferred response).
            handler(order_detail(p1));
            return true;
        }
        katana::sql::parameters params;
        params.reserve(1);
        params.push_back(katana::sql::encode_value(p1));
        return async_executor->query_async("order_detail", order_detail_sql, std::move(params),
            [handler = std::move(handler)](katana::result<katana::sql::rows> rows_result) {
                if (!rows_result) {
                    handler(std::unexpected(rows_result.error()));
                    return;
                }
                if (rows_result->size() > 1) {
                    handler(std::unexpected(std::make_error_code(std::errc::invalid_argument)));
                    return;
                }
                if (rows_result->empty()) {
                    handler(std::optional<OrderDetailRow>{});
                    return;
                }
                auto mapped = map_order_detail(rows_result->front());
                if (!mapped) {
                    handler(std::unexpected(mapped.error()));
                    return;
                }
                handler(std::optional<OrderDetailRow>(std::move(*mapped)));
            });
    }

    katana::result<std::vector<TopProductsByCategoryRow>> top_products_by_category(int64_t p1) const {
        katana::sql::parameters params;
        params.reserve(1);
        params.push_back(katana::sql::encode_value(p1));
        std::vector<TopProductsByCategoryRow> out_rows;
        auto status = executor_.query_each("top_products_by_category", top_products_by_category_sql, params,
            [&](const katana::sql::row_view& row) -> katana::result<void> {
                auto mapped = map_top_products_by_category(row);
                if (!mapped) {
                    return std::unexpected(mapped.error());
                }
                out_rows.push_back(std::move(*mapped));
                return {};
            });
        if (!status) {
            return std::unexpected(status.error());
        }
        return out_rows;
    }

    bool top_products_by_category_async(int64_t p1, top_products_by_category_async_handler handler) const {
        if (!handler) {
            return false;
        }
        auto* async_executor = dynamic_cast<katana::sql::async_executor*>(&executor_);
        if (async_executor == nullptr) {
            // No async executor: run synchronously and deliver the result inline
            // so the completion always fires (returning false here would hang a
            // deferred response).
            handler(top_products_by_category(p1));
            return true;
        }
        katana::sql::parameters params;
        params.reserve(1);
        params.push_back(katana::sql::encode_value(p1));
        return async_executor->query_async("top_products_by_category", top_products_by_category_sql, std::move(params),
            [handler = std::move(handler)](katana::result<katana::sql::rows> rows_result) {
                if (!rows_result) {
                    handler(std::unexpected(rows_result.error()));
                    return;
                }
                std::vector<TopProductsByCategoryRow> out_rows;
                out_rows.reserve(rows_result->size());
                for (const auto& row : *rows_result) {
                    auto mapped = map_top_products_by_category(row);
                    if (!mapped) {
                        handler(std::unexpected(mapped.error()));
                        return;
                    }
                    out_rows.push_back(std::move(*mapped));
                }
                handler(std::move(out_rows));
            });
    }

private:
    katana::sql::executor& executor_;

    static constexpr std::string_view bulk_insert_order_items_sql = R"__KATANA_SQL__(
INSERT INTO shop_order_items (order_id, product_id, qty, unit_price)
SELECT b.order_id, b.product_id, b.qty, b.unit_price
FROM UNNEST($1::bigint[], $2::bigint[], $3::bigint[], $4::float8[])
  AS b(order_id, product_id, qty, unit_price);
)__KATANA_SQL__";

    static constexpr std::string_view category_stats_sql = R"__KATANA_SQL__(
SELECT
  category::text AS category,
  COUNT(*)::bigint AS product_count,
  AVG(price)::float8 AS avg_price,
  SUM(stock)::bigint AS total_stock,
  MIN(price)::float8 AS min_price,
  MAX(price)::float8 AS max_price
FROM shop_products
GROUP BY category
ORDER BY product_count DESC;
)__KATANA_SQL__";

    template <typename Row>
    static katana::result<CategoryStatsRow> map_category_stats(const Row& row) {
        CategoryStatsRow out;
        {
            if (row.size() <= 0) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(0);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.category = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 1) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(1);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<int64_t>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.product_count = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 2) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(2);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<double>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.avg_price = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 3) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(3);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<int64_t>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.total_stock = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 4) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(4);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<double>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.min_price = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 5) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(5);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<double>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.max_price = std::move(*parsed);
            }
        }
        return out;
    }

    static constexpr std::string_view create_order_sql = R"__KATANA_SQL__(
INSERT INTO shop_orders (customer_id, status)
VALUES ($1::bigint, $2::text)
RETURNING id::bigint AS id;
)__KATANA_SQL__";

    template <typename Row>
    static katana::result<CreateOrderRow> map_create_order(const Row& row) {
        CreateOrderRow out;
        {
            if (row.size() <= 0) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(0);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<int64_t>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.id = std::move(*parsed);
            }
        }
        return out;
    }

    static constexpr std::string_view customer_revenue_ranked_sql = R"__KATANA_SQL__(
SELECT
  c.id::bigint AS customer_id,
  c.name::text AS name,
  c.country::text AS country,
  COALESCE(SUM(oi.qty * oi.unit_price), 0)::float8 AS revenue,
  COUNT(DISTINCT o.id)::bigint AS order_count,
  RANK() OVER (ORDER BY COALESCE(SUM(oi.qty * oi.unit_price), 0) DESC)::bigint AS revenue_rank
FROM shop_customers c
LEFT JOIN shop_orders o ON o.customer_id = c.id
LEFT JOIN shop_order_items oi ON oi.order_id = o.id
WHERE c.country = $1::text
GROUP BY c.id, c.name, c.country
ORDER BY revenue DESC
LIMIT $2::bigint OFFSET $3::bigint;
)__KATANA_SQL__";

    template <typename Row>
    static katana::result<CustomerRevenueRankedRow> map_customer_revenue_ranked(const Row& row) {
        CustomerRevenueRankedRow out;
        {
            if (row.size() <= 0) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(0);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<int64_t>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.customer_id = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 1) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(1);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.name = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 2) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(2);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.country = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 3) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(3);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<double>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.revenue = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 4) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(4);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<int64_t>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.order_count = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 5) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(5);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<int64_t>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.revenue_rank = std::move(*parsed);
            }
        }
        return out;
    }

    static constexpr std::string_view order_detail_sql = R"__KATANA_SQL__(
SELECT
  o.id::bigint AS order_id,
  o.status::text AS status,
  c.name::text AS customer_name,
  c.country::text AS country,
  COUNT(oi.id)::bigint AS line_count,
  COALESCE(SUM(oi.qty), 0)::bigint AS total_qty,
  COALESCE(SUM(oi.qty * oi.unit_price), 0)::float8 AS total_amount
FROM shop_orders o
JOIN shop_customers c ON c.id = o.customer_id
LEFT JOIN shop_order_items oi ON oi.order_id = o.id
WHERE o.id = $1::bigint
GROUP BY o.id, o.status, c.name, c.country;
)__KATANA_SQL__";

    template <typename Row>
    static katana::result<OrderDetailRow> map_order_detail(const Row& row) {
        OrderDetailRow out;
        {
            if (row.size() <= 0) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(0);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<int64_t>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.order_id = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 1) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(1);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.status = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 2) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(2);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.customer_name = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 3) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(3);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.country = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 4) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(4);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<int64_t>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.line_count = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 5) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(5);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<int64_t>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.total_qty = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 6) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(6);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<double>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.total_amount = std::move(*parsed);
            }
        }
        return out;
    }

    static constexpr std::string_view top_products_by_category_sql = R"__KATANA_SQL__(
SELECT
  p.id::bigint AS product_id,
  p.name::text AS name,
  p.category::text AS category,
  COALESCE(SUM(oi.qty), 0)::bigint AS units_sold,
  COALESCE(SUM(oi.qty * oi.unit_price), 0)::float8 AS revenue,
  RANK() OVER (PARTITION BY p.category ORDER BY COALESCE(SUM(oi.qty), 0) DESC)::bigint AS category_rank
FROM shop_products p
LEFT JOIN shop_order_items oi ON oi.product_id = p.id
GROUP BY p.id, p.name, p.category
ORDER BY p.category, units_sold DESC
LIMIT $1::bigint;
)__KATANA_SQL__";

    template <typename Row>
    static katana::result<TopProductsByCategoryRow> map_top_products_by_category(const Row& row) {
        TopProductsByCategoryRow out;
        {
            if (row.size() <= 0) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(0);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<int64_t>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.product_id = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 1) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(1);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.name = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 2) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(2);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.category = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 3) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(3);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<int64_t>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.units_sold = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 4) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(4);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<double>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.revenue = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 5) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(5);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<int64_t>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.category_rank = std::move(*parsed);
            }
        }
        return out;
    }

};

} // namespace katana::sql::generated
