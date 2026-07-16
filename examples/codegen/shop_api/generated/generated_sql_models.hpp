#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include <vector>

namespace generated {

struct CategoryStatsRow {
    std::optional<std::string> category;
    std::optional<int64_t> product_count;
    std::optional<double> avg_price;
    std::optional<int64_t> total_stock;
    std::optional<double> min_price;
    std::optional<double> max_price;
};

struct CreateOrderRow {
    std::optional<int64_t> id;
};

struct CustomerRevenueRankedRow {
    std::optional<int64_t> customer_id;
    std::optional<std::string> name;
    std::optional<std::string> country;
    std::optional<double> revenue;
    std::optional<int64_t> order_count;
    std::optional<int64_t> revenue_rank;
};

struct OrderDetailRow {
    std::optional<int64_t> order_id;
    std::optional<std::string> status;
    std::optional<std::string> customer_name;
    std::optional<std::string> country;
    std::optional<int64_t> line_count;
    std::optional<int64_t> total_qty;
    std::optional<double> total_amount;
};

struct TopProductsByCategoryRow {
    std::optional<int64_t> product_id;
    std::optional<std::string> name;
    std::optional<std::string> category;
    std::optional<int64_t> units_sold;
    std::optional<double> revenue;
    std::optional<int64_t> category_rank;
};

} // namespace generated
