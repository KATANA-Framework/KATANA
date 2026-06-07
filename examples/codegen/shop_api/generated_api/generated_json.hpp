// layer: flat
// Auto-generated JSON parsers and serializers from OpenAPI specification
//
// This file contains:
//   - parse_<Type>() functions: JSON string → C++ struct
//   - serialize_<Type>() functions: C++ struct → JSON string
//
// Features:
//   - Zero-copy parsing using arena allocators
//   - Streaming JSON generation without intermediate buffers
//   - Type-safe enum conversion
//   - Automatic null handling for optional fields
//
// All parse functions return std::optional<T>:
//   - std::nullopt on parse error (invalid JSON, wrong type, etc.)
//   - Parsed object on success
//
#pragma once

#include "katana/core/arena.hpp"
#include "katana/core/serde.hpp"
#include <optional>
#include <string>
#include <charconv>
#include <vector>

using katana::monotonic_arena;

// ============================================================
// Forward Declarations
// ============================================================

[[nodiscard]] inline std::optional<CustomerRevenue> parse_CustomerRevenue(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenue_Customer_id_t> parse_CustomerRevenue_Customer_id_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenue_Name_t> parse_CustomerRevenue_Name_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenue_Country_t> parse_CustomerRevenue_Country_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenue_Revenue_t> parse_CustomerRevenue_Revenue_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenue_Order_count_t> parse_CustomerRevenue_Order_count_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenue_Revenue_rank_t> parse_CustomerRevenue_Revenue_rank_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenueList> parse_CustomerRevenueList(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenueList_Items_t> parse_CustomerRevenueList_Items_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenueList_Item_t> parse_CustomerRevenueList_Item_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProduct> parse_TopProduct(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProduct_Product_id_t> parse_TopProduct_Product_id_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProduct_Name_t> parse_TopProduct_Name_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProduct_Category_t> parse_TopProduct_Category_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProduct_Units_sold_t> parse_TopProduct_Units_sold_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProduct_Revenue_t> parse_TopProduct_Revenue_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProduct_Category_rank_t> parse_TopProduct_Category_rank_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProductList> parse_TopProductList(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProductList_Items_t> parse_TopProductList_Items_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProductList_Item_t> parse_TopProductList_Item_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStat> parse_CategoryStat(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStat_Category_t> parse_CategoryStat_Category_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStat_Product_count_t> parse_CategoryStat_Product_count_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStat_Avg_price_t> parse_CategoryStat_Avg_price_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStat_Total_stock_t> parse_CategoryStat_Total_stock_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStat_Min_price_t> parse_CategoryStat_Min_price_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStat_Max_price_t> parse_CategoryStat_Max_price_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStatList> parse_CategoryStatList(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStatList_Items_t> parse_CategoryStatList_Items_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStatList_Item_t> parse_CategoryStatList_Item_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<OrderDetail> parse_OrderDetail(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<OrderDetail_Order_id_t> parse_OrderDetail_Order_id_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<OrderDetail_Status_t> parse_OrderDetail_Status_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<OrderDetail_Customer_name_t> parse_OrderDetail_Customer_name_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<OrderDetail_Country_t> parse_OrderDetail_Country_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<OrderDetail_Line_count_t> parse_OrderDetail_Line_count_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<OrderDetail_Total_qty_t> parse_OrderDetail_Total_qty_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<OrderDetail_Total_amount_t> parse_OrderDetail_Total_amount_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateOrderRequest> parse_CreateOrderRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateOrderRequest_Customer_id_t> parse_CreateOrderRequest_Customer_id_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateOrderRequest_Status_t> parse_CreateOrderRequest_Status_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateOrderResponse> parse_CreateOrderResponse(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateOrderResponse_Id_t> parse_CreateOrderResponse_Id_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<customer_revenue_param_country> parse_customer_revenue_param_country(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<customer_revenue_param_limit> parse_customer_revenue_param_limit(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<customer_revenue_param_offset> parse_customer_revenue_param_offset(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema> parse_schema(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<top_products_param_limit> parse_top_products_param_limit(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_1> parse_schema_1(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_2> parse_schema_2(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<order_detail_param_id> parse_order_detail_param_id(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_3> parse_schema_3(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_4> parse_schema_4(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_5> parse_schema_5(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<CustomerRevenue> parse_CustomerRevenue(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenue_Customer_id_t> parse_CustomerRevenue_Customer_id_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenue_Name_t> parse_CustomerRevenue_Name_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenue_Country_t> parse_CustomerRevenue_Country_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenue_Revenue_t> parse_CustomerRevenue_Revenue_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenue_Order_count_t> parse_CustomerRevenue_Order_count_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenue_Revenue_rank_t> parse_CustomerRevenue_Revenue_rank_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenueList> parse_CustomerRevenueList(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenueList_Items_t> parse_CustomerRevenueList_Items_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenueList_Item_t> parse_CustomerRevenueList_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProduct> parse_TopProduct(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProduct_Product_id_t> parse_TopProduct_Product_id_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProduct_Name_t> parse_TopProduct_Name_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProduct_Category_t> parse_TopProduct_Category_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProduct_Units_sold_t> parse_TopProduct_Units_sold_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProduct_Revenue_t> parse_TopProduct_Revenue_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProduct_Category_rank_t> parse_TopProduct_Category_rank_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProductList> parse_TopProductList(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProductList_Items_t> parse_TopProductList_Items_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProductList_Item_t> parse_TopProductList_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStat> parse_CategoryStat(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStat_Category_t> parse_CategoryStat_Category_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStat_Product_count_t> parse_CategoryStat_Product_count_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStat_Avg_price_t> parse_CategoryStat_Avg_price_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStat_Total_stock_t> parse_CategoryStat_Total_stock_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStat_Min_price_t> parse_CategoryStat_Min_price_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStat_Max_price_t> parse_CategoryStat_Max_price_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStatList> parse_CategoryStatList(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStatList_Items_t> parse_CategoryStatList_Items_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStatList_Item_t> parse_CategoryStatList_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<OrderDetail> parse_OrderDetail(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<OrderDetail_Order_id_t> parse_OrderDetail_Order_id_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<OrderDetail_Status_t> parse_OrderDetail_Status_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<OrderDetail_Customer_name_t> parse_OrderDetail_Customer_name_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<OrderDetail_Country_t> parse_OrderDetail_Country_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<OrderDetail_Line_count_t> parse_OrderDetail_Line_count_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<OrderDetail_Total_qty_t> parse_OrderDetail_Total_qty_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<OrderDetail_Total_amount_t> parse_OrderDetail_Total_amount_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateOrderRequest> parse_CreateOrderRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateOrderRequest_Customer_id_t> parse_CreateOrderRequest_Customer_id_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateOrderRequest_Status_t> parse_CreateOrderRequest_Status_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateOrderResponse> parse_CreateOrderResponse(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateOrderResponse_Id_t> parse_CreateOrderResponse_Id_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<customer_revenue_param_country> parse_customer_revenue_param_country(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<customer_revenue_param_limit> parse_customer_revenue_param_limit(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<customer_revenue_param_offset> parse_customer_revenue_param_offset(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema> parse_schema(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<top_products_param_limit> parse_top_products_param_limit(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_1> parse_schema_1(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_2> parse_schema_2(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<order_detail_param_id> parse_order_detail_param_id(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_3> parse_schema_3(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_4> parse_schema_4(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_5> parse_schema_5(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_CustomerRevenue_into(const CustomerRevenue& obj, std::string& out);
inline void serialize_CustomerRevenue_Customer_id_t_into(const CustomerRevenue_Customer_id_t& obj, std::string& out);
inline void serialize_CustomerRevenue_Name_t_into(const CustomerRevenue_Name_t& obj, std::string& out);
inline void serialize_CustomerRevenue_Country_t_into(const CustomerRevenue_Country_t& obj, std::string& out);
inline void serialize_CustomerRevenue_Revenue_t_into(const CustomerRevenue_Revenue_t& obj, std::string& out);
inline void serialize_CustomerRevenue_Order_count_t_into(const CustomerRevenue_Order_count_t& obj, std::string& out);
inline void serialize_CustomerRevenue_Revenue_rank_t_into(const CustomerRevenue_Revenue_rank_t& obj, std::string& out);
inline void serialize_CustomerRevenueList_into(const CustomerRevenueList& obj, std::string& out);
inline void serialize_CustomerRevenueList_Items_t_into(const CustomerRevenueList_Items_t& obj, std::string& out);
inline void serialize_CustomerRevenueList_Item_t_into(const CustomerRevenueList_Item_t& obj, std::string& out);
inline void serialize_TopProduct_into(const TopProduct& obj, std::string& out);
inline void serialize_TopProduct_Product_id_t_into(const TopProduct_Product_id_t& obj, std::string& out);
inline void serialize_TopProduct_Name_t_into(const TopProduct_Name_t& obj, std::string& out);
inline void serialize_TopProduct_Category_t_into(const TopProduct_Category_t& obj, std::string& out);
inline void serialize_TopProduct_Units_sold_t_into(const TopProduct_Units_sold_t& obj, std::string& out);
inline void serialize_TopProduct_Revenue_t_into(const TopProduct_Revenue_t& obj, std::string& out);
inline void serialize_TopProduct_Category_rank_t_into(const TopProduct_Category_rank_t& obj, std::string& out);
inline void serialize_TopProductList_into(const TopProductList& obj, std::string& out);
inline void serialize_TopProductList_Items_t_into(const TopProductList_Items_t& obj, std::string& out);
inline void serialize_TopProductList_Item_t_into(const TopProductList_Item_t& obj, std::string& out);
inline void serialize_CategoryStat_into(const CategoryStat& obj, std::string& out);
inline void serialize_CategoryStat_Category_t_into(const CategoryStat_Category_t& obj, std::string& out);
inline void serialize_CategoryStat_Product_count_t_into(const CategoryStat_Product_count_t& obj, std::string& out);
inline void serialize_CategoryStat_Avg_price_t_into(const CategoryStat_Avg_price_t& obj, std::string& out);
inline void serialize_CategoryStat_Total_stock_t_into(const CategoryStat_Total_stock_t& obj, std::string& out);
inline void serialize_CategoryStat_Min_price_t_into(const CategoryStat_Min_price_t& obj, std::string& out);
inline void serialize_CategoryStat_Max_price_t_into(const CategoryStat_Max_price_t& obj, std::string& out);
inline void serialize_CategoryStatList_into(const CategoryStatList& obj, std::string& out);
inline void serialize_CategoryStatList_Items_t_into(const CategoryStatList_Items_t& obj, std::string& out);
inline void serialize_CategoryStatList_Item_t_into(const CategoryStatList_Item_t& obj, std::string& out);
inline void serialize_OrderDetail_into(const OrderDetail& obj, std::string& out);
inline void serialize_OrderDetail_Order_id_t_into(const OrderDetail_Order_id_t& obj, std::string& out);
inline void serialize_OrderDetail_Status_t_into(const OrderDetail_Status_t& obj, std::string& out);
inline void serialize_OrderDetail_Customer_name_t_into(const OrderDetail_Customer_name_t& obj, std::string& out);
inline void serialize_OrderDetail_Country_t_into(const OrderDetail_Country_t& obj, std::string& out);
inline void serialize_OrderDetail_Line_count_t_into(const OrderDetail_Line_count_t& obj, std::string& out);
inline void serialize_OrderDetail_Total_qty_t_into(const OrderDetail_Total_qty_t& obj, std::string& out);
inline void serialize_OrderDetail_Total_amount_t_into(const OrderDetail_Total_amount_t& obj, std::string& out);
inline void serialize_CreateOrderRequest_into(const CreateOrderRequest& obj, std::string& out);
inline void serialize_CreateOrderRequest_Customer_id_t_into(const CreateOrderRequest_Customer_id_t& obj, std::string& out);
inline void serialize_CreateOrderRequest_Status_t_into(const CreateOrderRequest_Status_t& obj, std::string& out);
inline void serialize_CreateOrderResponse_into(const CreateOrderResponse& obj, std::string& out);
inline void serialize_CreateOrderResponse_Id_t_into(const CreateOrderResponse_Id_t& obj, std::string& out);
inline void serialize_customer_revenue_param_country_into(const customer_revenue_param_country& obj, std::string& out);
inline void serialize_customer_revenue_param_limit_into(const customer_revenue_param_limit& obj, std::string& out);
inline void serialize_customer_revenue_param_offset_into(const customer_revenue_param_offset& obj, std::string& out);
inline void serialize_schema_into(const schema& obj, std::string& out);
inline void serialize_top_products_param_limit_into(const top_products_param_limit& obj, std::string& out);
inline void serialize_schema_1_into(const schema_1& obj, std::string& out);
inline void serialize_schema_2_into(const schema_2& obj, std::string& out);
inline void serialize_order_detail_param_id_into(const order_detail_param_id& obj, std::string& out);
inline void serialize_schema_3_into(const schema_3& obj, std::string& out);
inline void serialize_schema_4_into(const schema_4& obj, std::string& out);
inline void serialize_schema_5_into(const schema_5& obj, std::string& out);

inline std::string serialize_CustomerRevenue(const CustomerRevenue& obj);
inline std::string serialize_CustomerRevenue_Customer_id_t(const CustomerRevenue_Customer_id_t& obj);
inline std::string serialize_CustomerRevenue_Name_t(const CustomerRevenue_Name_t& obj);
inline std::string serialize_CustomerRevenue_Country_t(const CustomerRevenue_Country_t& obj);
inline std::string serialize_CustomerRevenue_Revenue_t(const CustomerRevenue_Revenue_t& obj);
inline std::string serialize_CustomerRevenue_Order_count_t(const CustomerRevenue_Order_count_t& obj);
inline std::string serialize_CustomerRevenue_Revenue_rank_t(const CustomerRevenue_Revenue_rank_t& obj);
inline std::string serialize_CustomerRevenueList(const CustomerRevenueList& obj);
inline std::string serialize_CustomerRevenueList_Items_t(const CustomerRevenueList_Items_t& obj);
inline std::string serialize_CustomerRevenueList_Item_t(const CustomerRevenueList_Item_t& obj);
inline std::string serialize_TopProduct(const TopProduct& obj);
inline std::string serialize_TopProduct_Product_id_t(const TopProduct_Product_id_t& obj);
inline std::string serialize_TopProduct_Name_t(const TopProduct_Name_t& obj);
inline std::string serialize_TopProduct_Category_t(const TopProduct_Category_t& obj);
inline std::string serialize_TopProduct_Units_sold_t(const TopProduct_Units_sold_t& obj);
inline std::string serialize_TopProduct_Revenue_t(const TopProduct_Revenue_t& obj);
inline std::string serialize_TopProduct_Category_rank_t(const TopProduct_Category_rank_t& obj);
inline std::string serialize_TopProductList(const TopProductList& obj);
inline std::string serialize_TopProductList_Items_t(const TopProductList_Items_t& obj);
inline std::string serialize_TopProductList_Item_t(const TopProductList_Item_t& obj);
inline std::string serialize_CategoryStat(const CategoryStat& obj);
inline std::string serialize_CategoryStat_Category_t(const CategoryStat_Category_t& obj);
inline std::string serialize_CategoryStat_Product_count_t(const CategoryStat_Product_count_t& obj);
inline std::string serialize_CategoryStat_Avg_price_t(const CategoryStat_Avg_price_t& obj);
inline std::string serialize_CategoryStat_Total_stock_t(const CategoryStat_Total_stock_t& obj);
inline std::string serialize_CategoryStat_Min_price_t(const CategoryStat_Min_price_t& obj);
inline std::string serialize_CategoryStat_Max_price_t(const CategoryStat_Max_price_t& obj);
inline std::string serialize_CategoryStatList(const CategoryStatList& obj);
inline std::string serialize_CategoryStatList_Items_t(const CategoryStatList_Items_t& obj);
inline std::string serialize_CategoryStatList_Item_t(const CategoryStatList_Item_t& obj);
inline std::string serialize_OrderDetail(const OrderDetail& obj);
inline std::string serialize_OrderDetail_Order_id_t(const OrderDetail_Order_id_t& obj);
inline std::string serialize_OrderDetail_Status_t(const OrderDetail_Status_t& obj);
inline std::string serialize_OrderDetail_Customer_name_t(const OrderDetail_Customer_name_t& obj);
inline std::string serialize_OrderDetail_Country_t(const OrderDetail_Country_t& obj);
inline std::string serialize_OrderDetail_Line_count_t(const OrderDetail_Line_count_t& obj);
inline std::string serialize_OrderDetail_Total_qty_t(const OrderDetail_Total_qty_t& obj);
inline std::string serialize_OrderDetail_Total_amount_t(const OrderDetail_Total_amount_t& obj);
inline std::string serialize_CreateOrderRequest(const CreateOrderRequest& obj);
inline std::string serialize_CreateOrderRequest_Customer_id_t(const CreateOrderRequest_Customer_id_t& obj);
inline std::string serialize_CreateOrderRequest_Status_t(const CreateOrderRequest_Status_t& obj);
inline std::string serialize_CreateOrderResponse(const CreateOrderResponse& obj);
inline std::string serialize_CreateOrderResponse_Id_t(const CreateOrderResponse_Id_t& obj);
inline std::string serialize_customer_revenue_param_country(const customer_revenue_param_country& obj);
inline std::string serialize_customer_revenue_param_limit(const customer_revenue_param_limit& obj);
inline std::string serialize_customer_revenue_param_offset(const customer_revenue_param_offset& obj);
inline std::string serialize_schema(const schema& obj);
inline std::string serialize_top_products_param_limit(const top_products_param_limit& obj);
inline std::string serialize_schema_1(const schema_1& obj);
inline std::string serialize_schema_2(const schema_2& obj);
inline std::string serialize_order_detail_param_id(const order_detail_param_id& obj);
inline std::string serialize_schema_3(const schema_3& obj);
inline std::string serialize_schema_4(const schema_4& obj);
inline std::string serialize_schema_5(const schema_5& obj);

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue>> parse_CustomerRevenue_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Customer_id_t>> parse_CustomerRevenue_Customer_id_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Name_t>> parse_CustomerRevenue_Name_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Country_t>> parse_CustomerRevenue_Country_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Revenue_t>> parse_CustomerRevenue_Revenue_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Order_count_t>> parse_CustomerRevenue_Order_count_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Revenue_rank_t>> parse_CustomerRevenue_Revenue_rank_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenueList>> parse_CustomerRevenueList_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenueList_Items_t>> parse_CustomerRevenueList_Items_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenueList_Item_t>> parse_CustomerRevenueList_Item_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProduct>> parse_TopProduct_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProduct_Product_id_t>> parse_TopProduct_Product_id_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProduct_Name_t>> parse_TopProduct_Name_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProduct_Category_t>> parse_TopProduct_Category_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProduct_Units_sold_t>> parse_TopProduct_Units_sold_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProduct_Revenue_t>> parse_TopProduct_Revenue_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProduct_Category_rank_t>> parse_TopProduct_Category_rank_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProductList>> parse_TopProductList_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProductList_Items_t>> parse_TopProductList_Items_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProductList_Item_t>> parse_TopProductList_Item_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStat>> parse_CategoryStat_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStat_Category_t>> parse_CategoryStat_Category_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStat_Product_count_t>> parse_CategoryStat_Product_count_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStat_Avg_price_t>> parse_CategoryStat_Avg_price_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStat_Total_stock_t>> parse_CategoryStat_Total_stock_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStat_Min_price_t>> parse_CategoryStat_Min_price_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStat_Max_price_t>> parse_CategoryStat_Max_price_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStatList>> parse_CategoryStatList_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStatList_Items_t>> parse_CategoryStatList_Items_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStatList_Item_t>> parse_CategoryStatList_Item_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<OrderDetail>> parse_OrderDetail_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<OrderDetail_Order_id_t>> parse_OrderDetail_Order_id_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<OrderDetail_Status_t>> parse_OrderDetail_Status_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<OrderDetail_Customer_name_t>> parse_OrderDetail_Customer_name_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<OrderDetail_Country_t>> parse_OrderDetail_Country_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<OrderDetail_Line_count_t>> parse_OrderDetail_Line_count_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<OrderDetail_Total_qty_t>> parse_OrderDetail_Total_qty_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<OrderDetail_Total_amount_t>> parse_OrderDetail_Total_amount_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateOrderRequest>> parse_CreateOrderRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateOrderRequest_Customer_id_t>> parse_CreateOrderRequest_Customer_id_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateOrderRequest_Status_t>> parse_CreateOrderRequest_Status_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateOrderResponse>> parse_CreateOrderResponse_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateOrderResponse_Id_t>> parse_CreateOrderResponse_Id_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<customer_revenue_param_country>> parse_customer_revenue_param_country_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<customer_revenue_param_limit>> parse_customer_revenue_param_limit_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<customer_revenue_param_offset>> parse_customer_revenue_param_offset_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<top_products_param_limit>> parse_top_products_param_limit_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_1>> parse_schema_1_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_2>> parse_schema_2_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<order_detail_param_id>> parse_order_detail_param_id_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_3>> parse_schema_3_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_4>> parse_schema_4_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_5>> parse_schema_5_array(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue>> parse_CustomerRevenue_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Customer_id_t>> parse_CustomerRevenue_Customer_id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Name_t>> parse_CustomerRevenue_Name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Country_t>> parse_CustomerRevenue_Country_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Revenue_t>> parse_CustomerRevenue_Revenue_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Order_count_t>> parse_CustomerRevenue_Order_count_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Revenue_rank_t>> parse_CustomerRevenue_Revenue_rank_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenueList>> parse_CustomerRevenueList_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenueList_Items_t>> parse_CustomerRevenueList_Items_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenueList_Item_t>> parse_CustomerRevenueList_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProduct>> parse_TopProduct_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProduct_Product_id_t>> parse_TopProduct_Product_id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProduct_Name_t>> parse_TopProduct_Name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProduct_Category_t>> parse_TopProduct_Category_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProduct_Units_sold_t>> parse_TopProduct_Units_sold_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProduct_Revenue_t>> parse_TopProduct_Revenue_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProduct_Category_rank_t>> parse_TopProduct_Category_rank_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProductList>> parse_TopProductList_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProductList_Items_t>> parse_TopProductList_Items_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProductList_Item_t>> parse_TopProductList_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStat>> parse_CategoryStat_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStat_Category_t>> parse_CategoryStat_Category_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStat_Product_count_t>> parse_CategoryStat_Product_count_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStat_Avg_price_t>> parse_CategoryStat_Avg_price_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStat_Total_stock_t>> parse_CategoryStat_Total_stock_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStat_Min_price_t>> parse_CategoryStat_Min_price_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStat_Max_price_t>> parse_CategoryStat_Max_price_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStatList>> parse_CategoryStatList_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStatList_Items_t>> parse_CategoryStatList_Items_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStatList_Item_t>> parse_CategoryStatList_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<OrderDetail>> parse_OrderDetail_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<OrderDetail_Order_id_t>> parse_OrderDetail_Order_id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<OrderDetail_Status_t>> parse_OrderDetail_Status_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<OrderDetail_Customer_name_t>> parse_OrderDetail_Customer_name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<OrderDetail_Country_t>> parse_OrderDetail_Country_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<OrderDetail_Line_count_t>> parse_OrderDetail_Line_count_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<OrderDetail_Total_qty_t>> parse_OrderDetail_Total_qty_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<OrderDetail_Total_amount_t>> parse_OrderDetail_Total_amount_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateOrderRequest>> parse_CreateOrderRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateOrderRequest_Customer_id_t>> parse_CreateOrderRequest_Customer_id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateOrderRequest_Status_t>> parse_CreateOrderRequest_Status_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateOrderResponse>> parse_CreateOrderResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateOrderResponse_Id_t>> parse_CreateOrderResponse_Id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<customer_revenue_param_country>> parse_customer_revenue_param_country_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<customer_revenue_param_limit>> parse_customer_revenue_param_limit_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<customer_revenue_param_offset>> parse_customer_revenue_param_offset_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<top_products_param_limit>> parse_top_products_param_limit_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_1>> parse_schema_1_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_2>> parse_schema_2_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<order_detail_param_id>> parse_order_detail_param_id_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_3>> parse_schema_3_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_4>> parse_schema_4_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_5>> parse_schema_5_array(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_CustomerRevenue_array_into(const std::vector<CustomerRevenue>& arr, std::string& out);
inline void serialize_CustomerRevenue_array_into(const arena_vector<CustomerRevenue>& arr, std::string& out);
inline void serialize_CustomerRevenue_Customer_id_t_array_into(const std::vector<CustomerRevenue_Customer_id_t>& arr, std::string& out);
inline void serialize_CustomerRevenue_Customer_id_t_array_into(const arena_vector<CustomerRevenue_Customer_id_t>& arr, std::string& out);
inline void serialize_CustomerRevenue_Name_t_array_into(const std::vector<CustomerRevenue_Name_t>& arr, std::string& out);
inline void serialize_CustomerRevenue_Name_t_array_into(const arena_vector<CustomerRevenue_Name_t>& arr, std::string& out);
inline void serialize_CustomerRevenue_Country_t_array_into(const std::vector<CustomerRevenue_Country_t>& arr, std::string& out);
inline void serialize_CustomerRevenue_Country_t_array_into(const arena_vector<CustomerRevenue_Country_t>& arr, std::string& out);
inline void serialize_CustomerRevenue_Revenue_t_array_into(const std::vector<CustomerRevenue_Revenue_t>& arr, std::string& out);
inline void serialize_CustomerRevenue_Revenue_t_array_into(const arena_vector<CustomerRevenue_Revenue_t>& arr, std::string& out);
inline void serialize_CustomerRevenue_Order_count_t_array_into(const std::vector<CustomerRevenue_Order_count_t>& arr, std::string& out);
inline void serialize_CustomerRevenue_Order_count_t_array_into(const arena_vector<CustomerRevenue_Order_count_t>& arr, std::string& out);
inline void serialize_CustomerRevenue_Revenue_rank_t_array_into(const std::vector<CustomerRevenue_Revenue_rank_t>& arr, std::string& out);
inline void serialize_CustomerRevenue_Revenue_rank_t_array_into(const arena_vector<CustomerRevenue_Revenue_rank_t>& arr, std::string& out);
inline void serialize_CustomerRevenueList_array_into(const std::vector<CustomerRevenueList>& arr, std::string& out);
inline void serialize_CustomerRevenueList_array_into(const arena_vector<CustomerRevenueList>& arr, std::string& out);
inline void serialize_CustomerRevenueList_Items_t_array_into(const std::vector<CustomerRevenueList_Items_t>& arr, std::string& out);
inline void serialize_CustomerRevenueList_Items_t_array_into(const arena_vector<CustomerRevenueList_Items_t>& arr, std::string& out);
inline void serialize_CustomerRevenueList_Item_t_array_into(const std::vector<CustomerRevenueList_Item_t>& arr, std::string& out);
inline void serialize_CustomerRevenueList_Item_t_array_into(const arena_vector<CustomerRevenueList_Item_t>& arr, std::string& out);
inline void serialize_TopProduct_array_into(const std::vector<TopProduct>& arr, std::string& out);
inline void serialize_TopProduct_array_into(const arena_vector<TopProduct>& arr, std::string& out);
inline void serialize_TopProduct_Product_id_t_array_into(const std::vector<TopProduct_Product_id_t>& arr, std::string& out);
inline void serialize_TopProduct_Product_id_t_array_into(const arena_vector<TopProduct_Product_id_t>& arr, std::string& out);
inline void serialize_TopProduct_Name_t_array_into(const std::vector<TopProduct_Name_t>& arr, std::string& out);
inline void serialize_TopProduct_Name_t_array_into(const arena_vector<TopProduct_Name_t>& arr, std::string& out);
inline void serialize_TopProduct_Category_t_array_into(const std::vector<TopProduct_Category_t>& arr, std::string& out);
inline void serialize_TopProduct_Category_t_array_into(const arena_vector<TopProduct_Category_t>& arr, std::string& out);
inline void serialize_TopProduct_Units_sold_t_array_into(const std::vector<TopProduct_Units_sold_t>& arr, std::string& out);
inline void serialize_TopProduct_Units_sold_t_array_into(const arena_vector<TopProduct_Units_sold_t>& arr, std::string& out);
inline void serialize_TopProduct_Revenue_t_array_into(const std::vector<TopProduct_Revenue_t>& arr, std::string& out);
inline void serialize_TopProduct_Revenue_t_array_into(const arena_vector<TopProduct_Revenue_t>& arr, std::string& out);
inline void serialize_TopProduct_Category_rank_t_array_into(const std::vector<TopProduct_Category_rank_t>& arr, std::string& out);
inline void serialize_TopProduct_Category_rank_t_array_into(const arena_vector<TopProduct_Category_rank_t>& arr, std::string& out);
inline void serialize_TopProductList_array_into(const std::vector<TopProductList>& arr, std::string& out);
inline void serialize_TopProductList_array_into(const arena_vector<TopProductList>& arr, std::string& out);
inline void serialize_TopProductList_Items_t_array_into(const std::vector<TopProductList_Items_t>& arr, std::string& out);
inline void serialize_TopProductList_Items_t_array_into(const arena_vector<TopProductList_Items_t>& arr, std::string& out);
inline void serialize_TopProductList_Item_t_array_into(const std::vector<TopProductList_Item_t>& arr, std::string& out);
inline void serialize_TopProductList_Item_t_array_into(const arena_vector<TopProductList_Item_t>& arr, std::string& out);
inline void serialize_CategoryStat_array_into(const std::vector<CategoryStat>& arr, std::string& out);
inline void serialize_CategoryStat_array_into(const arena_vector<CategoryStat>& arr, std::string& out);
inline void serialize_CategoryStat_Category_t_array_into(const std::vector<CategoryStat_Category_t>& arr, std::string& out);
inline void serialize_CategoryStat_Category_t_array_into(const arena_vector<CategoryStat_Category_t>& arr, std::string& out);
inline void serialize_CategoryStat_Product_count_t_array_into(const std::vector<CategoryStat_Product_count_t>& arr, std::string& out);
inline void serialize_CategoryStat_Product_count_t_array_into(const arena_vector<CategoryStat_Product_count_t>& arr, std::string& out);
inline void serialize_CategoryStat_Avg_price_t_array_into(const std::vector<CategoryStat_Avg_price_t>& arr, std::string& out);
inline void serialize_CategoryStat_Avg_price_t_array_into(const arena_vector<CategoryStat_Avg_price_t>& arr, std::string& out);
inline void serialize_CategoryStat_Total_stock_t_array_into(const std::vector<CategoryStat_Total_stock_t>& arr, std::string& out);
inline void serialize_CategoryStat_Total_stock_t_array_into(const arena_vector<CategoryStat_Total_stock_t>& arr, std::string& out);
inline void serialize_CategoryStat_Min_price_t_array_into(const std::vector<CategoryStat_Min_price_t>& arr, std::string& out);
inline void serialize_CategoryStat_Min_price_t_array_into(const arena_vector<CategoryStat_Min_price_t>& arr, std::string& out);
inline void serialize_CategoryStat_Max_price_t_array_into(const std::vector<CategoryStat_Max_price_t>& arr, std::string& out);
inline void serialize_CategoryStat_Max_price_t_array_into(const arena_vector<CategoryStat_Max_price_t>& arr, std::string& out);
inline void serialize_CategoryStatList_array_into(const std::vector<CategoryStatList>& arr, std::string& out);
inline void serialize_CategoryStatList_array_into(const arena_vector<CategoryStatList>& arr, std::string& out);
inline void serialize_CategoryStatList_Items_t_array_into(const std::vector<CategoryStatList_Items_t>& arr, std::string& out);
inline void serialize_CategoryStatList_Items_t_array_into(const arena_vector<CategoryStatList_Items_t>& arr, std::string& out);
inline void serialize_CategoryStatList_Item_t_array_into(const std::vector<CategoryStatList_Item_t>& arr, std::string& out);
inline void serialize_CategoryStatList_Item_t_array_into(const arena_vector<CategoryStatList_Item_t>& arr, std::string& out);
inline void serialize_OrderDetail_array_into(const std::vector<OrderDetail>& arr, std::string& out);
inline void serialize_OrderDetail_array_into(const arena_vector<OrderDetail>& arr, std::string& out);
inline void serialize_OrderDetail_Order_id_t_array_into(const std::vector<OrderDetail_Order_id_t>& arr, std::string& out);
inline void serialize_OrderDetail_Order_id_t_array_into(const arena_vector<OrderDetail_Order_id_t>& arr, std::string& out);
inline void serialize_OrderDetail_Status_t_array_into(const std::vector<OrderDetail_Status_t>& arr, std::string& out);
inline void serialize_OrderDetail_Status_t_array_into(const arena_vector<OrderDetail_Status_t>& arr, std::string& out);
inline void serialize_OrderDetail_Customer_name_t_array_into(const std::vector<OrderDetail_Customer_name_t>& arr, std::string& out);
inline void serialize_OrderDetail_Customer_name_t_array_into(const arena_vector<OrderDetail_Customer_name_t>& arr, std::string& out);
inline void serialize_OrderDetail_Country_t_array_into(const std::vector<OrderDetail_Country_t>& arr, std::string& out);
inline void serialize_OrderDetail_Country_t_array_into(const arena_vector<OrderDetail_Country_t>& arr, std::string& out);
inline void serialize_OrderDetail_Line_count_t_array_into(const std::vector<OrderDetail_Line_count_t>& arr, std::string& out);
inline void serialize_OrderDetail_Line_count_t_array_into(const arena_vector<OrderDetail_Line_count_t>& arr, std::string& out);
inline void serialize_OrderDetail_Total_qty_t_array_into(const std::vector<OrderDetail_Total_qty_t>& arr, std::string& out);
inline void serialize_OrderDetail_Total_qty_t_array_into(const arena_vector<OrderDetail_Total_qty_t>& arr, std::string& out);
inline void serialize_OrderDetail_Total_amount_t_array_into(const std::vector<OrderDetail_Total_amount_t>& arr, std::string& out);
inline void serialize_OrderDetail_Total_amount_t_array_into(const arena_vector<OrderDetail_Total_amount_t>& arr, std::string& out);
inline void serialize_CreateOrderRequest_array_into(const std::vector<CreateOrderRequest>& arr, std::string& out);
inline void serialize_CreateOrderRequest_array_into(const arena_vector<CreateOrderRequest>& arr, std::string& out);
inline void serialize_CreateOrderRequest_Customer_id_t_array_into(const std::vector<CreateOrderRequest_Customer_id_t>& arr, std::string& out);
inline void serialize_CreateOrderRequest_Customer_id_t_array_into(const arena_vector<CreateOrderRequest_Customer_id_t>& arr, std::string& out);
inline void serialize_CreateOrderRequest_Status_t_array_into(const std::vector<CreateOrderRequest_Status_t>& arr, std::string& out);
inline void serialize_CreateOrderRequest_Status_t_array_into(const arena_vector<CreateOrderRequest_Status_t>& arr, std::string& out);
inline void serialize_CreateOrderResponse_array_into(const std::vector<CreateOrderResponse>& arr, std::string& out);
inline void serialize_CreateOrderResponse_array_into(const arena_vector<CreateOrderResponse>& arr, std::string& out);
inline void serialize_CreateOrderResponse_Id_t_array_into(const std::vector<CreateOrderResponse_Id_t>& arr, std::string& out);
inline void serialize_CreateOrderResponse_Id_t_array_into(const arena_vector<CreateOrderResponse_Id_t>& arr, std::string& out);
inline void serialize_customer_revenue_param_country_array_into(const std::vector<customer_revenue_param_country>& arr, std::string& out);
inline void serialize_customer_revenue_param_country_array_into(const arena_vector<customer_revenue_param_country>& arr, std::string& out);
inline void serialize_customer_revenue_param_limit_array_into(const std::vector<customer_revenue_param_limit>& arr, std::string& out);
inline void serialize_customer_revenue_param_limit_array_into(const arena_vector<customer_revenue_param_limit>& arr, std::string& out);
inline void serialize_customer_revenue_param_offset_array_into(const std::vector<customer_revenue_param_offset>& arr, std::string& out);
inline void serialize_customer_revenue_param_offset_array_into(const arena_vector<customer_revenue_param_offset>& arr, std::string& out);
inline void serialize_schema_array_into(const std::vector<schema>& arr, std::string& out);
inline void serialize_schema_array_into(const arena_vector<schema>& arr, std::string& out);
inline void serialize_top_products_param_limit_array_into(const std::vector<top_products_param_limit>& arr, std::string& out);
inline void serialize_top_products_param_limit_array_into(const arena_vector<top_products_param_limit>& arr, std::string& out);
inline void serialize_schema_1_array_into(const std::vector<schema_1>& arr, std::string& out);
inline void serialize_schema_1_array_into(const arena_vector<schema_1>& arr, std::string& out);
inline void serialize_schema_2_array_into(const std::vector<schema_2>& arr, std::string& out);
inline void serialize_schema_2_array_into(const arena_vector<schema_2>& arr, std::string& out);
inline void serialize_order_detail_param_id_array_into(const std::vector<order_detail_param_id>& arr, std::string& out);
inline void serialize_order_detail_param_id_array_into(const arena_vector<order_detail_param_id>& arr, std::string& out);
inline void serialize_schema_3_array_into(const std::vector<schema_3>& arr, std::string& out);
inline void serialize_schema_3_array_into(const arena_vector<schema_3>& arr, std::string& out);
inline void serialize_schema_4_array_into(const std::vector<schema_4>& arr, std::string& out);
inline void serialize_schema_4_array_into(const arena_vector<schema_4>& arr, std::string& out);
inline void serialize_schema_5_array_into(const std::vector<schema_5>& arr, std::string& out);
inline void serialize_schema_5_array_into(const arena_vector<schema_5>& arr, std::string& out);

inline std::string serialize_CustomerRevenue_array(const std::vector<CustomerRevenue>& arr);
inline std::string serialize_CustomerRevenue_array(const arena_vector<CustomerRevenue>& arr);
inline std::string serialize_CustomerRevenue_Customer_id_t_array(const std::vector<CustomerRevenue_Customer_id_t>& arr);
inline std::string serialize_CustomerRevenue_Customer_id_t_array(const arena_vector<CustomerRevenue_Customer_id_t>& arr);
inline std::string serialize_CustomerRevenue_Name_t_array(const std::vector<CustomerRevenue_Name_t>& arr);
inline std::string serialize_CustomerRevenue_Name_t_array(const arena_vector<CustomerRevenue_Name_t>& arr);
inline std::string serialize_CustomerRevenue_Country_t_array(const std::vector<CustomerRevenue_Country_t>& arr);
inline std::string serialize_CustomerRevenue_Country_t_array(const arena_vector<CustomerRevenue_Country_t>& arr);
inline std::string serialize_CustomerRevenue_Revenue_t_array(const std::vector<CustomerRevenue_Revenue_t>& arr);
inline std::string serialize_CustomerRevenue_Revenue_t_array(const arena_vector<CustomerRevenue_Revenue_t>& arr);
inline std::string serialize_CustomerRevenue_Order_count_t_array(const std::vector<CustomerRevenue_Order_count_t>& arr);
inline std::string serialize_CustomerRevenue_Order_count_t_array(const arena_vector<CustomerRevenue_Order_count_t>& arr);
inline std::string serialize_CustomerRevenue_Revenue_rank_t_array(const std::vector<CustomerRevenue_Revenue_rank_t>& arr);
inline std::string serialize_CustomerRevenue_Revenue_rank_t_array(const arena_vector<CustomerRevenue_Revenue_rank_t>& arr);
inline std::string serialize_CustomerRevenueList_array(const std::vector<CustomerRevenueList>& arr);
inline std::string serialize_CustomerRevenueList_array(const arena_vector<CustomerRevenueList>& arr);
inline std::string serialize_CustomerRevenueList_Items_t_array(const std::vector<CustomerRevenueList_Items_t>& arr);
inline std::string serialize_CustomerRevenueList_Items_t_array(const arena_vector<CustomerRevenueList_Items_t>& arr);
inline std::string serialize_CustomerRevenueList_Item_t_array(const std::vector<CustomerRevenueList_Item_t>& arr);
inline std::string serialize_CustomerRevenueList_Item_t_array(const arena_vector<CustomerRevenueList_Item_t>& arr);
inline std::string serialize_TopProduct_array(const std::vector<TopProduct>& arr);
inline std::string serialize_TopProduct_array(const arena_vector<TopProduct>& arr);
inline std::string serialize_TopProduct_Product_id_t_array(const std::vector<TopProduct_Product_id_t>& arr);
inline std::string serialize_TopProduct_Product_id_t_array(const arena_vector<TopProduct_Product_id_t>& arr);
inline std::string serialize_TopProduct_Name_t_array(const std::vector<TopProduct_Name_t>& arr);
inline std::string serialize_TopProduct_Name_t_array(const arena_vector<TopProduct_Name_t>& arr);
inline std::string serialize_TopProduct_Category_t_array(const std::vector<TopProduct_Category_t>& arr);
inline std::string serialize_TopProduct_Category_t_array(const arena_vector<TopProduct_Category_t>& arr);
inline std::string serialize_TopProduct_Units_sold_t_array(const std::vector<TopProduct_Units_sold_t>& arr);
inline std::string serialize_TopProduct_Units_sold_t_array(const arena_vector<TopProduct_Units_sold_t>& arr);
inline std::string serialize_TopProduct_Revenue_t_array(const std::vector<TopProduct_Revenue_t>& arr);
inline std::string serialize_TopProduct_Revenue_t_array(const arena_vector<TopProduct_Revenue_t>& arr);
inline std::string serialize_TopProduct_Category_rank_t_array(const std::vector<TopProduct_Category_rank_t>& arr);
inline std::string serialize_TopProduct_Category_rank_t_array(const arena_vector<TopProduct_Category_rank_t>& arr);
inline std::string serialize_TopProductList_array(const std::vector<TopProductList>& arr);
inline std::string serialize_TopProductList_array(const arena_vector<TopProductList>& arr);
inline std::string serialize_TopProductList_Items_t_array(const std::vector<TopProductList_Items_t>& arr);
inline std::string serialize_TopProductList_Items_t_array(const arena_vector<TopProductList_Items_t>& arr);
inline std::string serialize_TopProductList_Item_t_array(const std::vector<TopProductList_Item_t>& arr);
inline std::string serialize_TopProductList_Item_t_array(const arena_vector<TopProductList_Item_t>& arr);
inline std::string serialize_CategoryStat_array(const std::vector<CategoryStat>& arr);
inline std::string serialize_CategoryStat_array(const arena_vector<CategoryStat>& arr);
inline std::string serialize_CategoryStat_Category_t_array(const std::vector<CategoryStat_Category_t>& arr);
inline std::string serialize_CategoryStat_Category_t_array(const arena_vector<CategoryStat_Category_t>& arr);
inline std::string serialize_CategoryStat_Product_count_t_array(const std::vector<CategoryStat_Product_count_t>& arr);
inline std::string serialize_CategoryStat_Product_count_t_array(const arena_vector<CategoryStat_Product_count_t>& arr);
inline std::string serialize_CategoryStat_Avg_price_t_array(const std::vector<CategoryStat_Avg_price_t>& arr);
inline std::string serialize_CategoryStat_Avg_price_t_array(const arena_vector<CategoryStat_Avg_price_t>& arr);
inline std::string serialize_CategoryStat_Total_stock_t_array(const std::vector<CategoryStat_Total_stock_t>& arr);
inline std::string serialize_CategoryStat_Total_stock_t_array(const arena_vector<CategoryStat_Total_stock_t>& arr);
inline std::string serialize_CategoryStat_Min_price_t_array(const std::vector<CategoryStat_Min_price_t>& arr);
inline std::string serialize_CategoryStat_Min_price_t_array(const arena_vector<CategoryStat_Min_price_t>& arr);
inline std::string serialize_CategoryStat_Max_price_t_array(const std::vector<CategoryStat_Max_price_t>& arr);
inline std::string serialize_CategoryStat_Max_price_t_array(const arena_vector<CategoryStat_Max_price_t>& arr);
inline std::string serialize_CategoryStatList_array(const std::vector<CategoryStatList>& arr);
inline std::string serialize_CategoryStatList_array(const arena_vector<CategoryStatList>& arr);
inline std::string serialize_CategoryStatList_Items_t_array(const std::vector<CategoryStatList_Items_t>& arr);
inline std::string serialize_CategoryStatList_Items_t_array(const arena_vector<CategoryStatList_Items_t>& arr);
inline std::string serialize_CategoryStatList_Item_t_array(const std::vector<CategoryStatList_Item_t>& arr);
inline std::string serialize_CategoryStatList_Item_t_array(const arena_vector<CategoryStatList_Item_t>& arr);
inline std::string serialize_OrderDetail_array(const std::vector<OrderDetail>& arr);
inline std::string serialize_OrderDetail_array(const arena_vector<OrderDetail>& arr);
inline std::string serialize_OrderDetail_Order_id_t_array(const std::vector<OrderDetail_Order_id_t>& arr);
inline std::string serialize_OrderDetail_Order_id_t_array(const arena_vector<OrderDetail_Order_id_t>& arr);
inline std::string serialize_OrderDetail_Status_t_array(const std::vector<OrderDetail_Status_t>& arr);
inline std::string serialize_OrderDetail_Status_t_array(const arena_vector<OrderDetail_Status_t>& arr);
inline std::string serialize_OrderDetail_Customer_name_t_array(const std::vector<OrderDetail_Customer_name_t>& arr);
inline std::string serialize_OrderDetail_Customer_name_t_array(const arena_vector<OrderDetail_Customer_name_t>& arr);
inline std::string serialize_OrderDetail_Country_t_array(const std::vector<OrderDetail_Country_t>& arr);
inline std::string serialize_OrderDetail_Country_t_array(const arena_vector<OrderDetail_Country_t>& arr);
inline std::string serialize_OrderDetail_Line_count_t_array(const std::vector<OrderDetail_Line_count_t>& arr);
inline std::string serialize_OrderDetail_Line_count_t_array(const arena_vector<OrderDetail_Line_count_t>& arr);
inline std::string serialize_OrderDetail_Total_qty_t_array(const std::vector<OrderDetail_Total_qty_t>& arr);
inline std::string serialize_OrderDetail_Total_qty_t_array(const arena_vector<OrderDetail_Total_qty_t>& arr);
inline std::string serialize_OrderDetail_Total_amount_t_array(const std::vector<OrderDetail_Total_amount_t>& arr);
inline std::string serialize_OrderDetail_Total_amount_t_array(const arena_vector<OrderDetail_Total_amount_t>& arr);
inline std::string serialize_CreateOrderRequest_array(const std::vector<CreateOrderRequest>& arr);
inline std::string serialize_CreateOrderRequest_array(const arena_vector<CreateOrderRequest>& arr);
inline std::string serialize_CreateOrderRequest_Customer_id_t_array(const std::vector<CreateOrderRequest_Customer_id_t>& arr);
inline std::string serialize_CreateOrderRequest_Customer_id_t_array(const arena_vector<CreateOrderRequest_Customer_id_t>& arr);
inline std::string serialize_CreateOrderRequest_Status_t_array(const std::vector<CreateOrderRequest_Status_t>& arr);
inline std::string serialize_CreateOrderRequest_Status_t_array(const arena_vector<CreateOrderRequest_Status_t>& arr);
inline std::string serialize_CreateOrderResponse_array(const std::vector<CreateOrderResponse>& arr);
inline std::string serialize_CreateOrderResponse_array(const arena_vector<CreateOrderResponse>& arr);
inline std::string serialize_CreateOrderResponse_Id_t_array(const std::vector<CreateOrderResponse_Id_t>& arr);
inline std::string serialize_CreateOrderResponse_Id_t_array(const arena_vector<CreateOrderResponse_Id_t>& arr);
inline std::string serialize_customer_revenue_param_country_array(const std::vector<customer_revenue_param_country>& arr);
inline std::string serialize_customer_revenue_param_country_array(const arena_vector<customer_revenue_param_country>& arr);
inline std::string serialize_customer_revenue_param_limit_array(const std::vector<customer_revenue_param_limit>& arr);
inline std::string serialize_customer_revenue_param_limit_array(const arena_vector<customer_revenue_param_limit>& arr);
inline std::string serialize_customer_revenue_param_offset_array(const std::vector<customer_revenue_param_offset>& arr);
inline std::string serialize_customer_revenue_param_offset_array(const arena_vector<customer_revenue_param_offset>& arr);
inline std::string serialize_schema_array(const std::vector<schema>& arr);
inline std::string serialize_schema_array(const arena_vector<schema>& arr);
inline std::string serialize_top_products_param_limit_array(const std::vector<top_products_param_limit>& arr);
inline std::string serialize_top_products_param_limit_array(const arena_vector<top_products_param_limit>& arr);
inline std::string serialize_schema_1_array(const std::vector<schema_1>& arr);
inline std::string serialize_schema_1_array(const arena_vector<schema_1>& arr);
inline std::string serialize_schema_2_array(const std::vector<schema_2>& arr);
inline std::string serialize_schema_2_array(const arena_vector<schema_2>& arr);
inline std::string serialize_order_detail_param_id_array(const std::vector<order_detail_param_id>& arr);
inline std::string serialize_order_detail_param_id_array(const arena_vector<order_detail_param_id>& arr);
inline std::string serialize_schema_3_array(const std::vector<schema_3>& arr);
inline std::string serialize_schema_3_array(const arena_vector<schema_3>& arr);
inline std::string serialize_schema_4_array(const std::vector<schema_4>& arr);
inline std::string serialize_schema_4_array(const arena_vector<schema_4>& arr);
inline std::string serialize_schema_5_array(const std::vector<schema_5>& arr);
inline std::string serialize_schema_5_array(const arena_vector<schema_5>& arr);

// ============================================================
// JSON Parse Functions
// ============================================================

[[nodiscard]] inline std::optional<CustomerRevenue> parse_CustomerRevenue(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    CustomerRevenue obj(arena);
    bool has_customer_id = false;
    bool has_name = false;
    bool has_country = false;
    bool has_revenue = false;
    bool has_order_count = false;
    bool has_revenue_rank = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 4:
            if (*key == "name") {
                has_name = true;
                if (auto v = cur.string()) {
                    obj.name = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 7:
            if (*key == "country") {
                has_country = true;
                if (auto v = cur.string()) {
                    obj.country = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else if (*key == "revenue") {
                has_revenue = true;
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.revenue = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 11:
            if (*key == "customer_id") {
                has_customer_id = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.customer_id = *v;
                } else { cur.skip_value(); }
            } else if (*key == "order_count") {
                has_order_count = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.order_count = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 12:
            if (*key == "revenue_rank") {
                has_revenue_rank = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.revenue_rank = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!has_customer_id) return std::nullopt;
    if (!has_name) return std::nullopt;
    if (!has_country) return std::nullopt;
    if (!has_revenue) return std::nullopt;
    if (!has_order_count) return std::nullopt;
    if (!has_revenue_rank) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<CustomerRevenue> parse_CustomerRevenue(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenue(cur, arena);
}

[[nodiscard]] inline std::optional<CustomerRevenue_Customer_id_t> parse_CustomerRevenue_Customer_id_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return CustomerRevenue_Customer_id_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CustomerRevenue_Customer_id_t> parse_CustomerRevenue_Customer_id_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenue_Customer_id_t(cur, arena);
}

[[nodiscard]] inline std::optional<CustomerRevenue_Name_t> parse_CustomerRevenue_Name_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return CustomerRevenue_Name_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CustomerRevenue_Name_t> parse_CustomerRevenue_Name_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenue_Name_t(cur, arena);
}

[[nodiscard]] inline std::optional<CustomerRevenue_Country_t> parse_CustomerRevenue_Country_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return CustomerRevenue_Country_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CustomerRevenue_Country_t> parse_CustomerRevenue_Country_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenue_Country_t(cur, arena);
}

[[nodiscard]] inline std::optional<CustomerRevenue_Revenue_t> parse_CustomerRevenue_Revenue_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur)) return CustomerRevenue_Revenue_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CustomerRevenue_Revenue_t> parse_CustomerRevenue_Revenue_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenue_Revenue_t(cur, arena);
}

[[nodiscard]] inline std::optional<CustomerRevenue_Order_count_t> parse_CustomerRevenue_Order_count_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return CustomerRevenue_Order_count_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CustomerRevenue_Order_count_t> parse_CustomerRevenue_Order_count_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenue_Order_count_t(cur, arena);
}

[[nodiscard]] inline std::optional<CustomerRevenue_Revenue_rank_t> parse_CustomerRevenue_Revenue_rank_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return CustomerRevenue_Revenue_rank_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CustomerRevenue_Revenue_rank_t> parse_CustomerRevenue_Revenue_rank_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenue_Revenue_rank_t(cur, arena);
}

[[nodiscard]] inline std::optional<CustomerRevenueList> parse_CustomerRevenueList(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    CustomerRevenueList obj(arena);
    bool has_items = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "items") {
            has_items = true;
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) break;
                    if (auto nested = parse_CustomerRevenue(cur, arena)) { obj.items.push_back(std::move(*nested)); }
                    else { cur.skip_value(); }
                    cur.try_comma();
                }
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_items) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<CustomerRevenueList> parse_CustomerRevenueList(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenueList(cur, arena);
}

[[nodiscard]] inline std::optional<CustomerRevenueList_Items_t> parse_CustomerRevenueList_Items_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;
    CustomerRevenueList_Items_t result{arena_allocator<CustomerRevenue>(arena)};
    size_t reserve_hint = 0;
    for (const char* p = cur.ptr; p < cur.end; ++p) {
        if (*p == ',') ++reserve_hint;
    }
    if (cur.ptr < cur.end && *cur.ptr != ']') ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto parsed = parse_CustomerRevenue(cur, arena)) result.push_back(std::move(*parsed));
        else cur.skip_value();
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<CustomerRevenueList_Items_t> parse_CustomerRevenueList_Items_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start()) return std::nullopt;
    CustomerRevenueList_Items_t result{arena_allocator<CustomerRevenue>(arena)};
    size_t reserve_hint = 0;
    for (char ch : json) {
        if (ch == ',') ++reserve_hint;
    }
    if (!json.empty() && json != "[]") ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto parsed = parse_CustomerRevenue(cur, arena)) result.push_back(std::move(*parsed));
        else cur.skip_value();
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<CustomerRevenueList_Item_t> parse_CustomerRevenueList_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return CustomerRevenueList_Item_t{};
}

[[nodiscard]] inline std::optional<CustomerRevenueList_Item_t> parse_CustomerRevenueList_Item_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenueList_Item_t(cur, arena);
}

[[nodiscard]] inline std::optional<TopProduct> parse_TopProduct(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    TopProduct obj(arena);
    bool has_product_id = false;
    bool has_name = false;
    bool has_category = false;
    bool has_units_sold = false;
    bool has_revenue = false;
    bool has_category_rank = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 4:
            if (*key == "name") {
                has_name = true;
                if (auto v = cur.string()) {
                    obj.name = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 7:
            if (*key == "revenue") {
                has_revenue = true;
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.revenue = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "category") {
                has_category = true;
                if (auto v = cur.string()) {
                    obj.category = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 10:
            if (*key == "product_id") {
                has_product_id = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.product_id = *v;
                } else { cur.skip_value(); }
            } else if (*key == "units_sold") {
                has_units_sold = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.units_sold = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 13:
            if (*key == "category_rank") {
                has_category_rank = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.category_rank = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!has_product_id) return std::nullopt;
    if (!has_name) return std::nullopt;
    if (!has_category) return std::nullopt;
    if (!has_units_sold) return std::nullopt;
    if (!has_revenue) return std::nullopt;
    if (!has_category_rank) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<TopProduct> parse_TopProduct(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProduct(cur, arena);
}

[[nodiscard]] inline std::optional<TopProduct_Product_id_t> parse_TopProduct_Product_id_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return TopProduct_Product_id_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<TopProduct_Product_id_t> parse_TopProduct_Product_id_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProduct_Product_id_t(cur, arena);
}

[[nodiscard]] inline std::optional<TopProduct_Name_t> parse_TopProduct_Name_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return TopProduct_Name_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<TopProduct_Name_t> parse_TopProduct_Name_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProduct_Name_t(cur, arena);
}

[[nodiscard]] inline std::optional<TopProduct_Category_t> parse_TopProduct_Category_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return TopProduct_Category_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<TopProduct_Category_t> parse_TopProduct_Category_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProduct_Category_t(cur, arena);
}

[[nodiscard]] inline std::optional<TopProduct_Units_sold_t> parse_TopProduct_Units_sold_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return TopProduct_Units_sold_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<TopProduct_Units_sold_t> parse_TopProduct_Units_sold_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProduct_Units_sold_t(cur, arena);
}

[[nodiscard]] inline std::optional<TopProduct_Revenue_t> parse_TopProduct_Revenue_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur)) return TopProduct_Revenue_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<TopProduct_Revenue_t> parse_TopProduct_Revenue_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProduct_Revenue_t(cur, arena);
}

[[nodiscard]] inline std::optional<TopProduct_Category_rank_t> parse_TopProduct_Category_rank_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return TopProduct_Category_rank_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<TopProduct_Category_rank_t> parse_TopProduct_Category_rank_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProduct_Category_rank_t(cur, arena);
}

[[nodiscard]] inline std::optional<TopProductList> parse_TopProductList(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    TopProductList obj(arena);
    bool has_items = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "items") {
            has_items = true;
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) break;
                    if (auto nested = parse_TopProduct(cur, arena)) { obj.items.push_back(std::move(*nested)); }
                    else { cur.skip_value(); }
                    cur.try_comma();
                }
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_items) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<TopProductList> parse_TopProductList(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProductList(cur, arena);
}

[[nodiscard]] inline std::optional<TopProductList_Items_t> parse_TopProductList_Items_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;
    TopProductList_Items_t result{arena_allocator<TopProduct>(arena)};
    size_t reserve_hint = 0;
    for (const char* p = cur.ptr; p < cur.end; ++p) {
        if (*p == ',') ++reserve_hint;
    }
    if (cur.ptr < cur.end && *cur.ptr != ']') ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto parsed = parse_TopProduct(cur, arena)) result.push_back(std::move(*parsed));
        else cur.skip_value();
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<TopProductList_Items_t> parse_TopProductList_Items_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start()) return std::nullopt;
    TopProductList_Items_t result{arena_allocator<TopProduct>(arena)};
    size_t reserve_hint = 0;
    for (char ch : json) {
        if (ch == ',') ++reserve_hint;
    }
    if (!json.empty() && json != "[]") ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto parsed = parse_TopProduct(cur, arena)) result.push_back(std::move(*parsed));
        else cur.skip_value();
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<TopProductList_Item_t> parse_TopProductList_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return TopProductList_Item_t{};
}

[[nodiscard]] inline std::optional<TopProductList_Item_t> parse_TopProductList_Item_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProductList_Item_t(cur, arena);
}

[[nodiscard]] inline std::optional<CategoryStat> parse_CategoryStat(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    CategoryStat obj(arena);
    bool has_category = false;
    bool has_product_count = false;
    bool has_avg_price = false;
    bool has_total_stock = false;
    bool has_min_price = false;
    bool has_max_price = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 8:
            if (*key == "category") {
                has_category = true;
                if (auto v = cur.string()) {
                    obj.category = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 9:
            if (*key == "avg_price") {
                has_avg_price = true;
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.avg_price = *v;
                } else { cur.skip_value(); }
            } else if (*key == "min_price") {
                has_min_price = true;
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.min_price = *v;
                } else { cur.skip_value(); }
            } else if (*key == "max_price") {
                has_max_price = true;
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.max_price = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 11:
            if (*key == "total_stock") {
                has_total_stock = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.total_stock = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 13:
            if (*key == "product_count") {
                has_product_count = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.product_count = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!has_category) return std::nullopt;
    if (!has_product_count) return std::nullopt;
    if (!has_avg_price) return std::nullopt;
    if (!has_total_stock) return std::nullopt;
    if (!has_min_price) return std::nullopt;
    if (!has_max_price) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<CategoryStat> parse_CategoryStat(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStat(cur, arena);
}

[[nodiscard]] inline std::optional<CategoryStat_Category_t> parse_CategoryStat_Category_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return CategoryStat_Category_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CategoryStat_Category_t> parse_CategoryStat_Category_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStat_Category_t(cur, arena);
}

[[nodiscard]] inline std::optional<CategoryStat_Product_count_t> parse_CategoryStat_Product_count_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return CategoryStat_Product_count_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CategoryStat_Product_count_t> parse_CategoryStat_Product_count_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStat_Product_count_t(cur, arena);
}

[[nodiscard]] inline std::optional<CategoryStat_Avg_price_t> parse_CategoryStat_Avg_price_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur)) return CategoryStat_Avg_price_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CategoryStat_Avg_price_t> parse_CategoryStat_Avg_price_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStat_Avg_price_t(cur, arena);
}

[[nodiscard]] inline std::optional<CategoryStat_Total_stock_t> parse_CategoryStat_Total_stock_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return CategoryStat_Total_stock_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CategoryStat_Total_stock_t> parse_CategoryStat_Total_stock_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStat_Total_stock_t(cur, arena);
}

[[nodiscard]] inline std::optional<CategoryStat_Min_price_t> parse_CategoryStat_Min_price_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur)) return CategoryStat_Min_price_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CategoryStat_Min_price_t> parse_CategoryStat_Min_price_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStat_Min_price_t(cur, arena);
}

[[nodiscard]] inline std::optional<CategoryStat_Max_price_t> parse_CategoryStat_Max_price_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur)) return CategoryStat_Max_price_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CategoryStat_Max_price_t> parse_CategoryStat_Max_price_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStat_Max_price_t(cur, arena);
}

[[nodiscard]] inline std::optional<CategoryStatList> parse_CategoryStatList(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    CategoryStatList obj(arena);
    bool has_items = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "items") {
            has_items = true;
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) break;
                    if (auto nested = parse_CategoryStat(cur, arena)) { obj.items.push_back(std::move(*nested)); }
                    else { cur.skip_value(); }
                    cur.try_comma();
                }
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_items) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<CategoryStatList> parse_CategoryStatList(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStatList(cur, arena);
}

[[nodiscard]] inline std::optional<CategoryStatList_Items_t> parse_CategoryStatList_Items_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;
    CategoryStatList_Items_t result{arena_allocator<CategoryStat>(arena)};
    size_t reserve_hint = 0;
    for (const char* p = cur.ptr; p < cur.end; ++p) {
        if (*p == ',') ++reserve_hint;
    }
    if (cur.ptr < cur.end && *cur.ptr != ']') ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto parsed = parse_CategoryStat(cur, arena)) result.push_back(std::move(*parsed));
        else cur.skip_value();
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<CategoryStatList_Items_t> parse_CategoryStatList_Items_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start()) return std::nullopt;
    CategoryStatList_Items_t result{arena_allocator<CategoryStat>(arena)};
    size_t reserve_hint = 0;
    for (char ch : json) {
        if (ch == ',') ++reserve_hint;
    }
    if (!json.empty() && json != "[]") ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto parsed = parse_CategoryStat(cur, arena)) result.push_back(std::move(*parsed));
        else cur.skip_value();
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<CategoryStatList_Item_t> parse_CategoryStatList_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return CategoryStatList_Item_t{};
}

[[nodiscard]] inline std::optional<CategoryStatList_Item_t> parse_CategoryStatList_Item_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStatList_Item_t(cur, arena);
}

[[nodiscard]] inline std::optional<OrderDetail> parse_OrderDetail(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    OrderDetail obj(arena);
    bool has_order_id = false;
    bool has_status = false;
    bool has_customer_name = false;
    bool has_country = false;
    bool has_line_count = false;
    bool has_total_qty = false;
    bool has_total_amount = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 6:
            if (*key == "status") {
                has_status = true;
                if (auto v = cur.string()) {
                    obj.status = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 7:
            if (*key == "country") {
                has_country = true;
                if (auto v = cur.string()) {
                    obj.country = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "order_id") {
                has_order_id = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.order_id = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 9:
            if (*key == "total_qty") {
                has_total_qty = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.total_qty = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 10:
            if (*key == "line_count") {
                has_line_count = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.line_count = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 12:
            if (*key == "total_amount") {
                has_total_amount = true;
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.total_amount = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 13:
            if (*key == "customer_name") {
                has_customer_name = true;
                if (auto v = cur.string()) {
                    obj.customer_name = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!has_order_id) return std::nullopt;
    if (!has_status) return std::nullopt;
    if (!has_customer_name) return std::nullopt;
    if (!has_country) return std::nullopt;
    if (!has_line_count) return std::nullopt;
    if (!has_total_qty) return std::nullopt;
    if (!has_total_amount) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<OrderDetail> parse_OrderDetail(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_OrderDetail(cur, arena);
}

[[nodiscard]] inline std::optional<OrderDetail_Order_id_t> parse_OrderDetail_Order_id_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return OrderDetail_Order_id_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<OrderDetail_Order_id_t> parse_OrderDetail_Order_id_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_OrderDetail_Order_id_t(cur, arena);
}

[[nodiscard]] inline std::optional<OrderDetail_Status_t> parse_OrderDetail_Status_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return OrderDetail_Status_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<OrderDetail_Status_t> parse_OrderDetail_Status_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_OrderDetail_Status_t(cur, arena);
}

[[nodiscard]] inline std::optional<OrderDetail_Customer_name_t> parse_OrderDetail_Customer_name_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return OrderDetail_Customer_name_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<OrderDetail_Customer_name_t> parse_OrderDetail_Customer_name_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_OrderDetail_Customer_name_t(cur, arena);
}

[[nodiscard]] inline std::optional<OrderDetail_Country_t> parse_OrderDetail_Country_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return OrderDetail_Country_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<OrderDetail_Country_t> parse_OrderDetail_Country_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_OrderDetail_Country_t(cur, arena);
}

[[nodiscard]] inline std::optional<OrderDetail_Line_count_t> parse_OrderDetail_Line_count_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return OrderDetail_Line_count_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<OrderDetail_Line_count_t> parse_OrderDetail_Line_count_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_OrderDetail_Line_count_t(cur, arena);
}

[[nodiscard]] inline std::optional<OrderDetail_Total_qty_t> parse_OrderDetail_Total_qty_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return OrderDetail_Total_qty_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<OrderDetail_Total_qty_t> parse_OrderDetail_Total_qty_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_OrderDetail_Total_qty_t(cur, arena);
}

[[nodiscard]] inline std::optional<OrderDetail_Total_amount_t> parse_OrderDetail_Total_amount_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur)) return OrderDetail_Total_amount_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<OrderDetail_Total_amount_t> parse_OrderDetail_Total_amount_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_OrderDetail_Total_amount_t(cur, arena);
}

[[nodiscard]] inline std::optional<CreateOrderRequest> parse_CreateOrderRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    CreateOrderRequest obj(arena);
    bool has_customer_id = false;
    bool has_status = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "customer_id") {
            has_customer_id = true;
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.customer_id = *v;
            } else { cur.skip_value(); }
        } else         if (*key == "status") {
            has_status = true;
            if (auto v = cur.string()) {
                obj.status = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_customer_id) return std::nullopt;
    if (!has_status) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<CreateOrderRequest> parse_CreateOrderRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateOrderRequest(cur, arena);
}

[[nodiscard]] inline std::optional<CreateOrderRequest_Customer_id_t> parse_CreateOrderRequest_Customer_id_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return CreateOrderRequest_Customer_id_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CreateOrderRequest_Customer_id_t> parse_CreateOrderRequest_Customer_id_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateOrderRequest_Customer_id_t(cur, arena);
}

[[nodiscard]] inline std::optional<CreateOrderRequest_Status_t> parse_CreateOrderRequest_Status_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return CreateOrderRequest_Status_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CreateOrderRequest_Status_t> parse_CreateOrderRequest_Status_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateOrderRequest_Status_t(cur, arena);
}

[[nodiscard]] inline std::optional<CreateOrderResponse> parse_CreateOrderResponse(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    CreateOrderResponse obj(arena);
    bool has_id = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "id") {
            has_id = true;
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.id = *v;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_id) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<CreateOrderResponse> parse_CreateOrderResponse(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateOrderResponse(cur, arena);
}

[[nodiscard]] inline std::optional<CreateOrderResponse_Id_t> parse_CreateOrderResponse_Id_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return CreateOrderResponse_Id_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CreateOrderResponse_Id_t> parse_CreateOrderResponse_Id_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateOrderResponse_Id_t(cur, arena);
}

[[nodiscard]] inline std::optional<customer_revenue_param_country> parse_customer_revenue_param_country(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return customer_revenue_param_country{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<customer_revenue_param_country> parse_customer_revenue_param_country(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_customer_revenue_param_country(cur, arena);
}

[[nodiscard]] inline std::optional<customer_revenue_param_limit> parse_customer_revenue_param_limit(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return customer_revenue_param_limit{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<customer_revenue_param_limit> parse_customer_revenue_param_limit(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_customer_revenue_param_limit(cur, arena);
}

[[nodiscard]] inline std::optional<customer_revenue_param_offset> parse_customer_revenue_param_offset(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return customer_revenue_param_offset{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<customer_revenue_param_offset> parse_customer_revenue_param_offset(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_customer_revenue_param_offset(cur, arena);
}

[[nodiscard]] inline std::optional<schema> parse_schema(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema{};
}

[[nodiscard]] inline std::optional<schema> parse_schema(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema(cur, arena);
}

[[nodiscard]] inline std::optional<top_products_param_limit> parse_top_products_param_limit(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return top_products_param_limit{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<top_products_param_limit> parse_top_products_param_limit(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_top_products_param_limit(cur, arena);
}

[[nodiscard]] inline std::optional<schema_1> parse_schema_1(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_1{};
}

[[nodiscard]] inline std::optional<schema_1> parse_schema_1(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_1(cur, arena);
}

[[nodiscard]] inline std::optional<schema_2> parse_schema_2(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_2{};
}

[[nodiscard]] inline std::optional<schema_2> parse_schema_2(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_2(cur, arena);
}

[[nodiscard]] inline std::optional<order_detail_param_id> parse_order_detail_param_id(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return order_detail_param_id{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<order_detail_param_id> parse_order_detail_param_id(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_order_detail_param_id(cur, arena);
}

[[nodiscard]] inline std::optional<schema_3> parse_schema_3(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_3{};
}

[[nodiscard]] inline std::optional<schema_3> parse_schema_3(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_3(cur, arena);
}

[[nodiscard]] inline std::optional<schema_4> parse_schema_4(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_4{};
}

[[nodiscard]] inline std::optional<schema_4> parse_schema_4(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_4(cur, arena);
}

[[nodiscard]] inline std::optional<schema_5> parse_schema_5(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_5{};
}

[[nodiscard]] inline std::optional<schema_5> parse_schema_5(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_5(cur, arena);
}

// ============================================================
// JSON Serialize Functions
// ============================================================

inline void serialize_CustomerRevenue_into(const CustomerRevenue& obj, std::string& json) {
    json.push_back('{');
    json.append("\"customer_id\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.customer_id);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"name\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.name, json);
    json.push_back('"');
    json.append(",\"country\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.country, json);
    json.push_back('"');
    json.append(",\"revenue\":");
    {
        char buf[64];
        auto res = std::to_chars(buf, buf + sizeof(buf), obj.revenue);
        if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
    }
    json.append(",\"order_count\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.order_count);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"revenue_rank\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.revenue_rank);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.push_back('}');
}

inline std::string serialize_CustomerRevenue(const CustomerRevenue& obj) {
    std::string json;
    size_t reserve_estimate = 227;
    reserve_estimate += obj.name.size();
    reserve_estimate += obj.country.size();
    json.reserve(reserve_estimate);
    serialize_CustomerRevenue_into(obj, json);
    return json;
}

inline void serialize_CustomerRevenue_Customer_id_t_into(const CustomerRevenue_Customer_id_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_CustomerRevenue_Customer_id_t(const CustomerRevenue_Customer_id_t& obj) {
    std::string json;
    serialize_CustomerRevenue_Customer_id_t_into(obj, json);
    return json;
}

inline void serialize_CustomerRevenue_Name_t_into(const CustomerRevenue_Name_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_CustomerRevenue_Name_t(const CustomerRevenue_Name_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_CustomerRevenue_Name_t_into(obj, json);
    return json;
}

inline void serialize_CustomerRevenue_Country_t_into(const CustomerRevenue_Country_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_CustomerRevenue_Country_t(const CustomerRevenue_Country_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_CustomerRevenue_Country_t_into(obj, json);
    return json;
}

inline void serialize_CustomerRevenue_Revenue_t_into(const CustomerRevenue_Revenue_t& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_CustomerRevenue_Revenue_t(const CustomerRevenue_Revenue_t& obj) {
    std::string json;
    serialize_CustomerRevenue_Revenue_t_into(obj, json);
    return json;
}

inline void serialize_CustomerRevenue_Order_count_t_into(const CustomerRevenue_Order_count_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_CustomerRevenue_Order_count_t(const CustomerRevenue_Order_count_t& obj) {
    std::string json;
    serialize_CustomerRevenue_Order_count_t_into(obj, json);
    return json;
}

inline void serialize_CustomerRevenue_Revenue_rank_t_into(const CustomerRevenue_Revenue_rank_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_CustomerRevenue_Revenue_rank_t(const CustomerRevenue_Revenue_rank_t& obj) {
    std::string json;
    serialize_CustomerRevenue_Revenue_rank_t_into(obj, json);
    return json;
}

inline void serialize_CustomerRevenueList_into(const CustomerRevenueList& obj, std::string& json) {
    json.push_back('{');
    json.append("\"items\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.items.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_into(obj.items[i], json);
    }
    json.push_back(']');
    json.push_back('}');
}

inline std::string serialize_CustomerRevenueList(const CustomerRevenueList& obj) {
    std::string json;
    size_t reserve_estimate = 75;
    reserve_estimate += obj.items.size() * 227;
    json.reserve(reserve_estimate);
    serialize_CustomerRevenueList_into(obj, json);
    return json;
}

inline void serialize_CustomerRevenueList_Items_t_into(const CustomerRevenueList_Items_t& obj, std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenueList_Items_t(const CustomerRevenueList_Items_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_CustomerRevenueList_Items_t_into(obj, json);
    return json;
}

inline void serialize_CustomerRevenueList_Item_t_into(const CustomerRevenueList_Item_t& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_CustomerRevenueList_Item_t(const CustomerRevenueList_Item_t& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_TopProduct_into(const TopProduct& obj, std::string& json) {
    json.push_back('{');
    json.append("\"product_id\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.product_id);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"name\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.name, json);
    json.push_back('"');
    json.append(",\"category\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.category, json);
    json.push_back('"');
    json.append(",\"units_sold\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.units_sold);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"revenue\":");
    {
        char buf[64];
        auto res = std::to_chars(buf, buf + sizeof(buf), obj.revenue);
        if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
    }
    json.append(",\"category_rank\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.category_rank);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.push_back('}');
}

inline std::string serialize_TopProduct(const TopProduct& obj) {
    std::string json;
    size_t reserve_estimate = 227;
    reserve_estimate += obj.name.size();
    reserve_estimate += obj.category.size();
    json.reserve(reserve_estimate);
    serialize_TopProduct_into(obj, json);
    return json;
}

inline void serialize_TopProduct_Product_id_t_into(const TopProduct_Product_id_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_TopProduct_Product_id_t(const TopProduct_Product_id_t& obj) {
    std::string json;
    serialize_TopProduct_Product_id_t_into(obj, json);
    return json;
}

inline void serialize_TopProduct_Name_t_into(const TopProduct_Name_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_TopProduct_Name_t(const TopProduct_Name_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_TopProduct_Name_t_into(obj, json);
    return json;
}

inline void serialize_TopProduct_Category_t_into(const TopProduct_Category_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_TopProduct_Category_t(const TopProduct_Category_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_TopProduct_Category_t_into(obj, json);
    return json;
}

inline void serialize_TopProduct_Units_sold_t_into(const TopProduct_Units_sold_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_TopProduct_Units_sold_t(const TopProduct_Units_sold_t& obj) {
    std::string json;
    serialize_TopProduct_Units_sold_t_into(obj, json);
    return json;
}

inline void serialize_TopProduct_Revenue_t_into(const TopProduct_Revenue_t& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_TopProduct_Revenue_t(const TopProduct_Revenue_t& obj) {
    std::string json;
    serialize_TopProduct_Revenue_t_into(obj, json);
    return json;
}

inline void serialize_TopProduct_Category_rank_t_into(const TopProduct_Category_rank_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_TopProduct_Category_rank_t(const TopProduct_Category_rank_t& obj) {
    std::string json;
    serialize_TopProduct_Category_rank_t_into(obj, json);
    return json;
}

inline void serialize_TopProductList_into(const TopProductList& obj, std::string& json) {
    json.push_back('{');
    json.append("\"items\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.items.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_into(obj.items[i], json);
    }
    json.push_back(']');
    json.push_back('}');
}

inline std::string serialize_TopProductList(const TopProductList& obj) {
    std::string json;
    size_t reserve_estimate = 75;
    reserve_estimate += obj.items.size() * 227;
    json.reserve(reserve_estimate);
    serialize_TopProductList_into(obj, json);
    return json;
}

inline void serialize_TopProductList_Items_t_into(const TopProductList_Items_t& obj, std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProductList_Items_t(const TopProductList_Items_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_TopProductList_Items_t_into(obj, json);
    return json;
}

inline void serialize_TopProductList_Item_t_into(const TopProductList_Item_t& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_TopProductList_Item_t(const TopProductList_Item_t& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_CategoryStat_into(const CategoryStat& obj, std::string& json) {
    json.push_back('{');
    json.append("\"category\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.category, json);
    json.push_back('"');
    json.append(",\"product_count\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.product_count);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"avg_price\":");
    {
        char buf[64];
        auto res = std::to_chars(buf, buf + sizeof(buf), obj.avg_price);
        if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
    }
    json.append(",\"total_stock\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.total_stock);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"min_price\":");
    {
        char buf[64];
        auto res = std::to_chars(buf, buf + sizeof(buf), obj.min_price);
        if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
    }
    json.append(",\"max_price\":");
    {
        char buf[64];
        auto res = std::to_chars(buf, buf + sizeof(buf), obj.max_price);
        if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
    }
    json.push_back('}');
}

inline std::string serialize_CategoryStat(const CategoryStat& obj) {
    std::string json;
    size_t reserve_estimate = 232;
    reserve_estimate += obj.category.size();
    json.reserve(reserve_estimate);
    serialize_CategoryStat_into(obj, json);
    return json;
}

inline void serialize_CategoryStat_Category_t_into(const CategoryStat_Category_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_CategoryStat_Category_t(const CategoryStat_Category_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_CategoryStat_Category_t_into(obj, json);
    return json;
}

inline void serialize_CategoryStat_Product_count_t_into(const CategoryStat_Product_count_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_CategoryStat_Product_count_t(const CategoryStat_Product_count_t& obj) {
    std::string json;
    serialize_CategoryStat_Product_count_t_into(obj, json);
    return json;
}

inline void serialize_CategoryStat_Avg_price_t_into(const CategoryStat_Avg_price_t& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_CategoryStat_Avg_price_t(const CategoryStat_Avg_price_t& obj) {
    std::string json;
    serialize_CategoryStat_Avg_price_t_into(obj, json);
    return json;
}

inline void serialize_CategoryStat_Total_stock_t_into(const CategoryStat_Total_stock_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_CategoryStat_Total_stock_t(const CategoryStat_Total_stock_t& obj) {
    std::string json;
    serialize_CategoryStat_Total_stock_t_into(obj, json);
    return json;
}

inline void serialize_CategoryStat_Min_price_t_into(const CategoryStat_Min_price_t& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_CategoryStat_Min_price_t(const CategoryStat_Min_price_t& obj) {
    std::string json;
    serialize_CategoryStat_Min_price_t_into(obj, json);
    return json;
}

inline void serialize_CategoryStat_Max_price_t_into(const CategoryStat_Max_price_t& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_CategoryStat_Max_price_t(const CategoryStat_Max_price_t& obj) {
    std::string json;
    serialize_CategoryStat_Max_price_t_into(obj, json);
    return json;
}

inline void serialize_CategoryStatList_into(const CategoryStatList& obj, std::string& json) {
    json.push_back('{');
    json.append("\"items\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.items.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_into(obj.items[i], json);
    }
    json.push_back(']');
    json.push_back('}');
}

inline std::string serialize_CategoryStatList(const CategoryStatList& obj) {
    std::string json;
    size_t reserve_estimate = 75;
    reserve_estimate += obj.items.size() * 232;
    json.reserve(reserve_estimate);
    serialize_CategoryStatList_into(obj, json);
    return json;
}

inline void serialize_CategoryStatList_Items_t_into(const CategoryStatList_Items_t& obj, std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStatList_Items_t(const CategoryStatList_Items_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_CategoryStatList_Items_t_into(obj, json);
    return json;
}

inline void serialize_CategoryStatList_Item_t_into(const CategoryStatList_Item_t& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_CategoryStatList_Item_t(const CategoryStatList_Item_t& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_OrderDetail_into(const OrderDetail& obj, std::string& json) {
    json.push_back('{');
    json.append("\"order_id\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.order_id);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"status\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.status, json);
    json.push_back('"');
    json.append(",\"customer_name\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.customer_name, json);
    json.push_back('"');
    json.append(",\"country\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.country, json);
    json.push_back('"');
    json.append(",\"line_count\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.line_count);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"total_qty\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.total_qty);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"total_amount\":");
    {
        char buf[64];
        auto res = std::to_chars(buf, buf + sizeof(buf), obj.total_amount);
        if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
    }
    json.push_back('}');
}

inline std::string serialize_OrderDetail(const OrderDetail& obj) {
    std::string json;
    size_t reserve_estimate = 276;
    reserve_estimate += obj.status.size();
    reserve_estimate += obj.customer_name.size();
    reserve_estimate += obj.country.size();
    json.reserve(reserve_estimate);
    serialize_OrderDetail_into(obj, json);
    return json;
}

inline void serialize_OrderDetail_Order_id_t_into(const OrderDetail_Order_id_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_OrderDetail_Order_id_t(const OrderDetail_Order_id_t& obj) {
    std::string json;
    serialize_OrderDetail_Order_id_t_into(obj, json);
    return json;
}

inline void serialize_OrderDetail_Status_t_into(const OrderDetail_Status_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_OrderDetail_Status_t(const OrderDetail_Status_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_OrderDetail_Status_t_into(obj, json);
    return json;
}

inline void serialize_OrderDetail_Customer_name_t_into(const OrderDetail_Customer_name_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_OrderDetail_Customer_name_t(const OrderDetail_Customer_name_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_OrderDetail_Customer_name_t_into(obj, json);
    return json;
}

inline void serialize_OrderDetail_Country_t_into(const OrderDetail_Country_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_OrderDetail_Country_t(const OrderDetail_Country_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_OrderDetail_Country_t_into(obj, json);
    return json;
}

inline void serialize_OrderDetail_Line_count_t_into(const OrderDetail_Line_count_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_OrderDetail_Line_count_t(const OrderDetail_Line_count_t& obj) {
    std::string json;
    serialize_OrderDetail_Line_count_t_into(obj, json);
    return json;
}

inline void serialize_OrderDetail_Total_qty_t_into(const OrderDetail_Total_qty_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_OrderDetail_Total_qty_t(const OrderDetail_Total_qty_t& obj) {
    std::string json;
    serialize_OrderDetail_Total_qty_t_into(obj, json);
    return json;
}

inline void serialize_OrderDetail_Total_amount_t_into(const OrderDetail_Total_amount_t& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_OrderDetail_Total_amount_t(const OrderDetail_Total_amount_t& obj) {
    std::string json;
    serialize_OrderDetail_Total_amount_t_into(obj, json);
    return json;
}

inline void serialize_CreateOrderRequest_into(const CreateOrderRequest& obj, std::string& json) {
    json.push_back('{');
    json.append("\"customer_id\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.customer_id);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"status\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.status, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_CreateOrderRequest(const CreateOrderRequest& obj) {
    std::string json;
    size_t reserve_estimate = 79;
    reserve_estimate += obj.status.size();
    json.reserve(reserve_estimate);
    serialize_CreateOrderRequest_into(obj, json);
    return json;
}

inline void serialize_CreateOrderRequest_Customer_id_t_into(const CreateOrderRequest_Customer_id_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_CreateOrderRequest_Customer_id_t(const CreateOrderRequest_Customer_id_t& obj) {
    std::string json;
    serialize_CreateOrderRequest_Customer_id_t_into(obj, json);
    return json;
}

inline void serialize_CreateOrderRequest_Status_t_into(const CreateOrderRequest_Status_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_CreateOrderRequest_Status_t(const CreateOrderRequest_Status_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_CreateOrderRequest_Status_t_into(obj, json);
    return json;
}

inline void serialize_CreateOrderResponse_into(const CreateOrderResponse& obj, std::string& json) {
    json.push_back('{');
    json.append("\"id\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.id);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.push_back('}');
}

inline std::string serialize_CreateOrderResponse(const CreateOrderResponse& obj) {
    std::string json;
    size_t reserve_estimate = 28;
    json.reserve(reserve_estimate);
    serialize_CreateOrderResponse_into(obj, json);
    return json;
}

inline void serialize_CreateOrderResponse_Id_t_into(const CreateOrderResponse_Id_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_CreateOrderResponse_Id_t(const CreateOrderResponse_Id_t& obj) {
    std::string json;
    serialize_CreateOrderResponse_Id_t_into(obj, json);
    return json;
}

inline void serialize_customer_revenue_param_country_into(const customer_revenue_param_country& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_customer_revenue_param_country(const customer_revenue_param_country& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_customer_revenue_param_country_into(obj, json);
    return json;
}

inline void serialize_customer_revenue_param_limit_into(const customer_revenue_param_limit& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_customer_revenue_param_limit(const customer_revenue_param_limit& obj) {
    std::string json;
    serialize_customer_revenue_param_limit_into(obj, json);
    return json;
}

inline void serialize_customer_revenue_param_offset_into(const customer_revenue_param_offset& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_customer_revenue_param_offset(const customer_revenue_param_offset& obj) {
    std::string json;
    serialize_customer_revenue_param_offset_into(obj, json);
    return json;
}

inline void serialize_schema_into(const schema& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema(const schema& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_top_products_param_limit_into(const top_products_param_limit& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_top_products_param_limit(const top_products_param_limit& obj) {
    std::string json;
    serialize_top_products_param_limit_into(obj, json);
    return json;
}

inline void serialize_schema_1_into(const schema_1& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_1(const schema_1& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_2_into(const schema_2& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_2(const schema_2& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_order_detail_param_id_into(const order_detail_param_id& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_order_detail_param_id(const order_detail_param_id& obj) {
    std::string json;
    serialize_order_detail_param_id_into(obj, json);
    return json;
}

inline void serialize_schema_3_into(const schema_3& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_3(const schema_3& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_4_into(const schema_4& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_4(const schema_4& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_5_into(const schema_5& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_5(const schema_5& obj) {
    (void)obj;
    return std::string("{}");
}

// ============================================================
// Array Parse Functions
// ============================================================

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue>> parse_CustomerRevenue_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CustomerRevenue> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CustomerRevenue(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue>> parse_CustomerRevenue_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenue_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Customer_id_t>> parse_CustomerRevenue_Customer_id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CustomerRevenue_Customer_id_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CustomerRevenue_Customer_id_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Customer_id_t>> parse_CustomerRevenue_Customer_id_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenue_Customer_id_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Name_t>> parse_CustomerRevenue_Name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CustomerRevenue_Name_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CustomerRevenue_Name_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Name_t>> parse_CustomerRevenue_Name_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenue_Name_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Country_t>> parse_CustomerRevenue_Country_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CustomerRevenue_Country_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CustomerRevenue_Country_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Country_t>> parse_CustomerRevenue_Country_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenue_Country_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Revenue_t>> parse_CustomerRevenue_Revenue_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CustomerRevenue_Revenue_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CustomerRevenue_Revenue_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Revenue_t>> parse_CustomerRevenue_Revenue_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenue_Revenue_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Order_count_t>> parse_CustomerRevenue_Order_count_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CustomerRevenue_Order_count_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CustomerRevenue_Order_count_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Order_count_t>> parse_CustomerRevenue_Order_count_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenue_Order_count_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Revenue_rank_t>> parse_CustomerRevenue_Revenue_rank_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CustomerRevenue_Revenue_rank_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CustomerRevenue_Revenue_rank_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue_Revenue_rank_t>> parse_CustomerRevenue_Revenue_rank_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenue_Revenue_rank_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenueList>> parse_CustomerRevenueList_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CustomerRevenueList> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CustomerRevenueList(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenueList>> parse_CustomerRevenueList_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenueList_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenueList_Items_t>> parse_CustomerRevenueList_Items_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CustomerRevenueList_Items_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CustomerRevenueList_Items_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenueList_Items_t>> parse_CustomerRevenueList_Items_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenueList_Items_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenueList_Item_t>> parse_CustomerRevenueList_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CustomerRevenueList_Item_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CustomerRevenueList_Item_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenueList_Item_t>> parse_CustomerRevenueList_Item_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenueList_Item_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<TopProduct>> parse_TopProduct_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<TopProduct> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_TopProduct(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<TopProduct>> parse_TopProduct_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProduct_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<TopProduct_Product_id_t>> parse_TopProduct_Product_id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<TopProduct_Product_id_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_TopProduct_Product_id_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<TopProduct_Product_id_t>> parse_TopProduct_Product_id_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProduct_Product_id_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<TopProduct_Name_t>> parse_TopProduct_Name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<TopProduct_Name_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_TopProduct_Name_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<TopProduct_Name_t>> parse_TopProduct_Name_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProduct_Name_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<TopProduct_Category_t>> parse_TopProduct_Category_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<TopProduct_Category_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_TopProduct_Category_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<TopProduct_Category_t>> parse_TopProduct_Category_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProduct_Category_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<TopProduct_Units_sold_t>> parse_TopProduct_Units_sold_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<TopProduct_Units_sold_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_TopProduct_Units_sold_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<TopProduct_Units_sold_t>> parse_TopProduct_Units_sold_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProduct_Units_sold_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<TopProduct_Revenue_t>> parse_TopProduct_Revenue_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<TopProduct_Revenue_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_TopProduct_Revenue_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<TopProduct_Revenue_t>> parse_TopProduct_Revenue_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProduct_Revenue_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<TopProduct_Category_rank_t>> parse_TopProduct_Category_rank_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<TopProduct_Category_rank_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_TopProduct_Category_rank_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<TopProduct_Category_rank_t>> parse_TopProduct_Category_rank_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProduct_Category_rank_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<TopProductList>> parse_TopProductList_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<TopProductList> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_TopProductList(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<TopProductList>> parse_TopProductList_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProductList_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<TopProductList_Items_t>> parse_TopProductList_Items_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<TopProductList_Items_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_TopProductList_Items_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<TopProductList_Items_t>> parse_TopProductList_Items_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProductList_Items_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<TopProductList_Item_t>> parse_TopProductList_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<TopProductList_Item_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_TopProductList_Item_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<TopProductList_Item_t>> parse_TopProductList_Item_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProductList_Item_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CategoryStat>> parse_CategoryStat_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CategoryStat> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CategoryStat(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CategoryStat>> parse_CategoryStat_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStat_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CategoryStat_Category_t>> parse_CategoryStat_Category_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CategoryStat_Category_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CategoryStat_Category_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CategoryStat_Category_t>> parse_CategoryStat_Category_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStat_Category_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CategoryStat_Product_count_t>> parse_CategoryStat_Product_count_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CategoryStat_Product_count_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CategoryStat_Product_count_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CategoryStat_Product_count_t>> parse_CategoryStat_Product_count_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStat_Product_count_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CategoryStat_Avg_price_t>> parse_CategoryStat_Avg_price_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CategoryStat_Avg_price_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CategoryStat_Avg_price_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CategoryStat_Avg_price_t>> parse_CategoryStat_Avg_price_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStat_Avg_price_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CategoryStat_Total_stock_t>> parse_CategoryStat_Total_stock_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CategoryStat_Total_stock_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CategoryStat_Total_stock_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CategoryStat_Total_stock_t>> parse_CategoryStat_Total_stock_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStat_Total_stock_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CategoryStat_Min_price_t>> parse_CategoryStat_Min_price_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CategoryStat_Min_price_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CategoryStat_Min_price_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CategoryStat_Min_price_t>> parse_CategoryStat_Min_price_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStat_Min_price_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CategoryStat_Max_price_t>> parse_CategoryStat_Max_price_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CategoryStat_Max_price_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CategoryStat_Max_price_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CategoryStat_Max_price_t>> parse_CategoryStat_Max_price_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStat_Max_price_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CategoryStatList>> parse_CategoryStatList_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CategoryStatList> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CategoryStatList(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CategoryStatList>> parse_CategoryStatList_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStatList_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CategoryStatList_Items_t>> parse_CategoryStatList_Items_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CategoryStatList_Items_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CategoryStatList_Items_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CategoryStatList_Items_t>> parse_CategoryStatList_Items_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStatList_Items_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CategoryStatList_Item_t>> parse_CategoryStatList_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CategoryStatList_Item_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CategoryStatList_Item_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CategoryStatList_Item_t>> parse_CategoryStatList_Item_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStatList_Item_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<OrderDetail>> parse_OrderDetail_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<OrderDetail> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_OrderDetail(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<OrderDetail>> parse_OrderDetail_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_OrderDetail_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<OrderDetail_Order_id_t>> parse_OrderDetail_Order_id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<OrderDetail_Order_id_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_OrderDetail_Order_id_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<OrderDetail_Order_id_t>> parse_OrderDetail_Order_id_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_OrderDetail_Order_id_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<OrderDetail_Status_t>> parse_OrderDetail_Status_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<OrderDetail_Status_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_OrderDetail_Status_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<OrderDetail_Status_t>> parse_OrderDetail_Status_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_OrderDetail_Status_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<OrderDetail_Customer_name_t>> parse_OrderDetail_Customer_name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<OrderDetail_Customer_name_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_OrderDetail_Customer_name_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<OrderDetail_Customer_name_t>> parse_OrderDetail_Customer_name_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_OrderDetail_Customer_name_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<OrderDetail_Country_t>> parse_OrderDetail_Country_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<OrderDetail_Country_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_OrderDetail_Country_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<OrderDetail_Country_t>> parse_OrderDetail_Country_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_OrderDetail_Country_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<OrderDetail_Line_count_t>> parse_OrderDetail_Line_count_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<OrderDetail_Line_count_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_OrderDetail_Line_count_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<OrderDetail_Line_count_t>> parse_OrderDetail_Line_count_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_OrderDetail_Line_count_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<OrderDetail_Total_qty_t>> parse_OrderDetail_Total_qty_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<OrderDetail_Total_qty_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_OrderDetail_Total_qty_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<OrderDetail_Total_qty_t>> parse_OrderDetail_Total_qty_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_OrderDetail_Total_qty_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<OrderDetail_Total_amount_t>> parse_OrderDetail_Total_amount_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<OrderDetail_Total_amount_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_OrderDetail_Total_amount_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<OrderDetail_Total_amount_t>> parse_OrderDetail_Total_amount_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_OrderDetail_Total_amount_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateOrderRequest>> parse_CreateOrderRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CreateOrderRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CreateOrderRequest(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateOrderRequest>> parse_CreateOrderRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateOrderRequest_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateOrderRequest_Customer_id_t>> parse_CreateOrderRequest_Customer_id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CreateOrderRequest_Customer_id_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CreateOrderRequest_Customer_id_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateOrderRequest_Customer_id_t>> parse_CreateOrderRequest_Customer_id_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateOrderRequest_Customer_id_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateOrderRequest_Status_t>> parse_CreateOrderRequest_Status_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CreateOrderRequest_Status_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CreateOrderRequest_Status_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateOrderRequest_Status_t>> parse_CreateOrderRequest_Status_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateOrderRequest_Status_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateOrderResponse>> parse_CreateOrderResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CreateOrderResponse> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CreateOrderResponse(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateOrderResponse>> parse_CreateOrderResponse_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateOrderResponse_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateOrderResponse_Id_t>> parse_CreateOrderResponse_Id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CreateOrderResponse_Id_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CreateOrderResponse_Id_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateOrderResponse_Id_t>> parse_CreateOrderResponse_Id_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateOrderResponse_Id_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<customer_revenue_param_country>> parse_customer_revenue_param_country_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<customer_revenue_param_country> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_customer_revenue_param_country(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<customer_revenue_param_country>> parse_customer_revenue_param_country_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_customer_revenue_param_country_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<customer_revenue_param_limit>> parse_customer_revenue_param_limit_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<customer_revenue_param_limit> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_customer_revenue_param_limit(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<customer_revenue_param_limit>> parse_customer_revenue_param_limit_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_customer_revenue_param_limit_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<customer_revenue_param_offset>> parse_customer_revenue_param_offset_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<customer_revenue_param_offset> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_customer_revenue_param_offset(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<customer_revenue_param_offset>> parse_customer_revenue_param_offset_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_customer_revenue_param_offset_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<top_products_param_limit>> parse_top_products_param_limit_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<top_products_param_limit> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_top_products_param_limit(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<top_products_param_limit>> parse_top_products_param_limit_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_top_products_param_limit_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_1>> parse_schema_1_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_1> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_1(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_1>> parse_schema_1_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_1_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_2>> parse_schema_2_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_2> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_2(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_2>> parse_schema_2_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_2_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<order_detail_param_id>> parse_order_detail_param_id_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<order_detail_param_id> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_order_detail_param_id(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<order_detail_param_id>> parse_order_detail_param_id_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_order_detail_param_id_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_3>> parse_schema_3_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_3> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_3(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_3>> parse_schema_3_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_3_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_4>> parse_schema_4_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_4> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_4(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_4>> parse_schema_4_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_4_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_5>> parse_schema_5_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_5> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_5(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_5>> parse_schema_5_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_5_array(cur, arena);
}

// ============================================================
// Array Serialize Functions
// ============================================================

inline void serialize_CustomerRevenue_array_into(const std::vector<CustomerRevenue>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenue_array(const std::vector<CustomerRevenue>& arr) {
    std::string json;
    json.reserve(arr.size() * 227 + 2);
    serialize_CustomerRevenue_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenue_array_into(const arena_vector<CustomerRevenue>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenue_array(const arena_vector<CustomerRevenue>& arr) {
    std::string json;
    json.reserve(arr.size() * 227 + 2);
    serialize_CustomerRevenue_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenue_Customer_id_t_array_into(const std::vector<CustomerRevenue_Customer_id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_Customer_id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenue_Customer_id_t_array(const std::vector<CustomerRevenue_Customer_id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CustomerRevenue_Customer_id_t_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenue_Customer_id_t_array_into(const arena_vector<CustomerRevenue_Customer_id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_Customer_id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenue_Customer_id_t_array(const arena_vector<CustomerRevenue_Customer_id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CustomerRevenue_Customer_id_t_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenue_Name_t_array_into(const std::vector<CustomerRevenue_Name_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_Name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenue_Name_t_array(const std::vector<CustomerRevenue_Name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CustomerRevenue_Name_t_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenue_Name_t_array_into(const arena_vector<CustomerRevenue_Name_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_Name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenue_Name_t_array(const arena_vector<CustomerRevenue_Name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CustomerRevenue_Name_t_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenue_Country_t_array_into(const std::vector<CustomerRevenue_Country_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_Country_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenue_Country_t_array(const std::vector<CustomerRevenue_Country_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CustomerRevenue_Country_t_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenue_Country_t_array_into(const arena_vector<CustomerRevenue_Country_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_Country_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenue_Country_t_array(const arena_vector<CustomerRevenue_Country_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CustomerRevenue_Country_t_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenue_Revenue_t_array_into(const std::vector<CustomerRevenue_Revenue_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_Revenue_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenue_Revenue_t_array(const std::vector<CustomerRevenue_Revenue_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_CustomerRevenue_Revenue_t_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenue_Revenue_t_array_into(const arena_vector<CustomerRevenue_Revenue_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_Revenue_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenue_Revenue_t_array(const arena_vector<CustomerRevenue_Revenue_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_CustomerRevenue_Revenue_t_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenue_Order_count_t_array_into(const std::vector<CustomerRevenue_Order_count_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_Order_count_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenue_Order_count_t_array(const std::vector<CustomerRevenue_Order_count_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CustomerRevenue_Order_count_t_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenue_Order_count_t_array_into(const arena_vector<CustomerRevenue_Order_count_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_Order_count_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenue_Order_count_t_array(const arena_vector<CustomerRevenue_Order_count_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CustomerRevenue_Order_count_t_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenue_Revenue_rank_t_array_into(const std::vector<CustomerRevenue_Revenue_rank_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_Revenue_rank_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenue_Revenue_rank_t_array(const std::vector<CustomerRevenue_Revenue_rank_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CustomerRevenue_Revenue_rank_t_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenue_Revenue_rank_t_array_into(const arena_vector<CustomerRevenue_Revenue_rank_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_Revenue_rank_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenue_Revenue_rank_t_array(const arena_vector<CustomerRevenue_Revenue_rank_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CustomerRevenue_Revenue_rank_t_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenueList_array_into(const std::vector<CustomerRevenueList>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenueList_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenueList_array(const std::vector<CustomerRevenueList>& arr) {
    std::string json;
    json.reserve(arr.size() * 75 + 2);
    serialize_CustomerRevenueList_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenueList_array_into(const arena_vector<CustomerRevenueList>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenueList_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenueList_array(const arena_vector<CustomerRevenueList>& arr) {
    std::string json;
    json.reserve(arr.size() * 75 + 2);
    serialize_CustomerRevenueList_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenueList_Items_t_array_into(const std::vector<CustomerRevenueList_Items_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenueList_Items_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenueList_Items_t_array(const std::vector<CustomerRevenueList_Items_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 910 + 2);
    serialize_CustomerRevenueList_Items_t_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenueList_Items_t_array_into(const arena_vector<CustomerRevenueList_Items_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenueList_Items_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenueList_Items_t_array(const arena_vector<CustomerRevenueList_Items_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 910 + 2);
    serialize_CustomerRevenueList_Items_t_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenueList_Item_t_array_into(const std::vector<CustomerRevenueList_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenueList_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenueList_Item_t_array(const std::vector<CustomerRevenueList_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_CustomerRevenueList_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenueList_Item_t_array_into(const arena_vector<CustomerRevenueList_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenueList_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenueList_Item_t_array(const arena_vector<CustomerRevenueList_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_CustomerRevenueList_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_TopProduct_array_into(const std::vector<TopProduct>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProduct_array(const std::vector<TopProduct>& arr) {
    std::string json;
    json.reserve(arr.size() * 227 + 2);
    serialize_TopProduct_array_into(arr, json);
    return json;
}

inline void serialize_TopProduct_array_into(const arena_vector<TopProduct>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProduct_array(const arena_vector<TopProduct>& arr) {
    std::string json;
    json.reserve(arr.size() * 227 + 2);
    serialize_TopProduct_array_into(arr, json);
    return json;
}

inline void serialize_TopProduct_Product_id_t_array_into(const std::vector<TopProduct_Product_id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_Product_id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProduct_Product_id_t_array(const std::vector<TopProduct_Product_id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_TopProduct_Product_id_t_array_into(arr, json);
    return json;
}

inline void serialize_TopProduct_Product_id_t_array_into(const arena_vector<TopProduct_Product_id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_Product_id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProduct_Product_id_t_array(const arena_vector<TopProduct_Product_id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_TopProduct_Product_id_t_array_into(arr, json);
    return json;
}

inline void serialize_TopProduct_Name_t_array_into(const std::vector<TopProduct_Name_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_Name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProduct_Name_t_array(const std::vector<TopProduct_Name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_TopProduct_Name_t_array_into(arr, json);
    return json;
}

inline void serialize_TopProduct_Name_t_array_into(const arena_vector<TopProduct_Name_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_Name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProduct_Name_t_array(const arena_vector<TopProduct_Name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_TopProduct_Name_t_array_into(arr, json);
    return json;
}

inline void serialize_TopProduct_Category_t_array_into(const std::vector<TopProduct_Category_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_Category_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProduct_Category_t_array(const std::vector<TopProduct_Category_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_TopProduct_Category_t_array_into(arr, json);
    return json;
}

inline void serialize_TopProduct_Category_t_array_into(const arena_vector<TopProduct_Category_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_Category_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProduct_Category_t_array(const arena_vector<TopProduct_Category_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_TopProduct_Category_t_array_into(arr, json);
    return json;
}

inline void serialize_TopProduct_Units_sold_t_array_into(const std::vector<TopProduct_Units_sold_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_Units_sold_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProduct_Units_sold_t_array(const std::vector<TopProduct_Units_sold_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_TopProduct_Units_sold_t_array_into(arr, json);
    return json;
}

inline void serialize_TopProduct_Units_sold_t_array_into(const arena_vector<TopProduct_Units_sold_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_Units_sold_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProduct_Units_sold_t_array(const arena_vector<TopProduct_Units_sold_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_TopProduct_Units_sold_t_array_into(arr, json);
    return json;
}

inline void serialize_TopProduct_Revenue_t_array_into(const std::vector<TopProduct_Revenue_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_Revenue_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProduct_Revenue_t_array(const std::vector<TopProduct_Revenue_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_TopProduct_Revenue_t_array_into(arr, json);
    return json;
}

inline void serialize_TopProduct_Revenue_t_array_into(const arena_vector<TopProduct_Revenue_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_Revenue_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProduct_Revenue_t_array(const arena_vector<TopProduct_Revenue_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_TopProduct_Revenue_t_array_into(arr, json);
    return json;
}

inline void serialize_TopProduct_Category_rank_t_array_into(const std::vector<TopProduct_Category_rank_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_Category_rank_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProduct_Category_rank_t_array(const std::vector<TopProduct_Category_rank_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_TopProduct_Category_rank_t_array_into(arr, json);
    return json;
}

inline void serialize_TopProduct_Category_rank_t_array_into(const arena_vector<TopProduct_Category_rank_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_Category_rank_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProduct_Category_rank_t_array(const arena_vector<TopProduct_Category_rank_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_TopProduct_Category_rank_t_array_into(arr, json);
    return json;
}

inline void serialize_TopProductList_array_into(const std::vector<TopProductList>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProductList_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProductList_array(const std::vector<TopProductList>& arr) {
    std::string json;
    json.reserve(arr.size() * 75 + 2);
    serialize_TopProductList_array_into(arr, json);
    return json;
}

inline void serialize_TopProductList_array_into(const arena_vector<TopProductList>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProductList_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProductList_array(const arena_vector<TopProductList>& arr) {
    std::string json;
    json.reserve(arr.size() * 75 + 2);
    serialize_TopProductList_array_into(arr, json);
    return json;
}

inline void serialize_TopProductList_Items_t_array_into(const std::vector<TopProductList_Items_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProductList_Items_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProductList_Items_t_array(const std::vector<TopProductList_Items_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 910 + 2);
    serialize_TopProductList_Items_t_array_into(arr, json);
    return json;
}

inline void serialize_TopProductList_Items_t_array_into(const arena_vector<TopProductList_Items_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProductList_Items_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProductList_Items_t_array(const arena_vector<TopProductList_Items_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 910 + 2);
    serialize_TopProductList_Items_t_array_into(arr, json);
    return json;
}

inline void serialize_TopProductList_Item_t_array_into(const std::vector<TopProductList_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProductList_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProductList_Item_t_array(const std::vector<TopProductList_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_TopProductList_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_TopProductList_Item_t_array_into(const arena_vector<TopProductList_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProductList_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProductList_Item_t_array(const arena_vector<TopProductList_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_TopProductList_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStat_array_into(const std::vector<CategoryStat>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStat_array(const std::vector<CategoryStat>& arr) {
    std::string json;
    json.reserve(arr.size() * 232 + 2);
    serialize_CategoryStat_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStat_array_into(const arena_vector<CategoryStat>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStat_array(const arena_vector<CategoryStat>& arr) {
    std::string json;
    json.reserve(arr.size() * 232 + 2);
    serialize_CategoryStat_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStat_Category_t_array_into(const std::vector<CategoryStat_Category_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_Category_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStat_Category_t_array(const std::vector<CategoryStat_Category_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CategoryStat_Category_t_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStat_Category_t_array_into(const arena_vector<CategoryStat_Category_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_Category_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStat_Category_t_array(const arena_vector<CategoryStat_Category_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CategoryStat_Category_t_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStat_Product_count_t_array_into(const std::vector<CategoryStat_Product_count_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_Product_count_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStat_Product_count_t_array(const std::vector<CategoryStat_Product_count_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CategoryStat_Product_count_t_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStat_Product_count_t_array_into(const arena_vector<CategoryStat_Product_count_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_Product_count_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStat_Product_count_t_array(const arena_vector<CategoryStat_Product_count_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CategoryStat_Product_count_t_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStat_Avg_price_t_array_into(const std::vector<CategoryStat_Avg_price_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_Avg_price_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStat_Avg_price_t_array(const std::vector<CategoryStat_Avg_price_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_CategoryStat_Avg_price_t_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStat_Avg_price_t_array_into(const arena_vector<CategoryStat_Avg_price_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_Avg_price_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStat_Avg_price_t_array(const arena_vector<CategoryStat_Avg_price_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_CategoryStat_Avg_price_t_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStat_Total_stock_t_array_into(const std::vector<CategoryStat_Total_stock_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_Total_stock_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStat_Total_stock_t_array(const std::vector<CategoryStat_Total_stock_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CategoryStat_Total_stock_t_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStat_Total_stock_t_array_into(const arena_vector<CategoryStat_Total_stock_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_Total_stock_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStat_Total_stock_t_array(const arena_vector<CategoryStat_Total_stock_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CategoryStat_Total_stock_t_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStat_Min_price_t_array_into(const std::vector<CategoryStat_Min_price_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_Min_price_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStat_Min_price_t_array(const std::vector<CategoryStat_Min_price_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_CategoryStat_Min_price_t_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStat_Min_price_t_array_into(const arena_vector<CategoryStat_Min_price_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_Min_price_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStat_Min_price_t_array(const arena_vector<CategoryStat_Min_price_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_CategoryStat_Min_price_t_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStat_Max_price_t_array_into(const std::vector<CategoryStat_Max_price_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_Max_price_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStat_Max_price_t_array(const std::vector<CategoryStat_Max_price_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_CategoryStat_Max_price_t_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStat_Max_price_t_array_into(const arena_vector<CategoryStat_Max_price_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_Max_price_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStat_Max_price_t_array(const arena_vector<CategoryStat_Max_price_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_CategoryStat_Max_price_t_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStatList_array_into(const std::vector<CategoryStatList>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStatList_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStatList_array(const std::vector<CategoryStatList>& arr) {
    std::string json;
    json.reserve(arr.size() * 75 + 2);
    serialize_CategoryStatList_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStatList_array_into(const arena_vector<CategoryStatList>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStatList_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStatList_array(const arena_vector<CategoryStatList>& arr) {
    std::string json;
    json.reserve(arr.size() * 75 + 2);
    serialize_CategoryStatList_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStatList_Items_t_array_into(const std::vector<CategoryStatList_Items_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStatList_Items_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStatList_Items_t_array(const std::vector<CategoryStatList_Items_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 930 + 2);
    serialize_CategoryStatList_Items_t_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStatList_Items_t_array_into(const arena_vector<CategoryStatList_Items_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStatList_Items_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStatList_Items_t_array(const arena_vector<CategoryStatList_Items_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 930 + 2);
    serialize_CategoryStatList_Items_t_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStatList_Item_t_array_into(const std::vector<CategoryStatList_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStatList_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStatList_Item_t_array(const std::vector<CategoryStatList_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_CategoryStatList_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStatList_Item_t_array_into(const arena_vector<CategoryStatList_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStatList_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStatList_Item_t_array(const arena_vector<CategoryStatList_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_CategoryStatList_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_OrderDetail_array_into(const std::vector<OrderDetail>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_OrderDetail_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_OrderDetail_array(const std::vector<OrderDetail>& arr) {
    std::string json;
    json.reserve(arr.size() * 276 + 2);
    serialize_OrderDetail_array_into(arr, json);
    return json;
}

inline void serialize_OrderDetail_array_into(const arena_vector<OrderDetail>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_OrderDetail_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_OrderDetail_array(const arena_vector<OrderDetail>& arr) {
    std::string json;
    json.reserve(arr.size() * 276 + 2);
    serialize_OrderDetail_array_into(arr, json);
    return json;
}

inline void serialize_OrderDetail_Order_id_t_array_into(const std::vector<OrderDetail_Order_id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_OrderDetail_Order_id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_OrderDetail_Order_id_t_array(const std::vector<OrderDetail_Order_id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_OrderDetail_Order_id_t_array_into(arr, json);
    return json;
}

inline void serialize_OrderDetail_Order_id_t_array_into(const arena_vector<OrderDetail_Order_id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_OrderDetail_Order_id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_OrderDetail_Order_id_t_array(const arena_vector<OrderDetail_Order_id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_OrderDetail_Order_id_t_array_into(arr, json);
    return json;
}

inline void serialize_OrderDetail_Status_t_array_into(const std::vector<OrderDetail_Status_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_OrderDetail_Status_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_OrderDetail_Status_t_array(const std::vector<OrderDetail_Status_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_OrderDetail_Status_t_array_into(arr, json);
    return json;
}

inline void serialize_OrderDetail_Status_t_array_into(const arena_vector<OrderDetail_Status_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_OrderDetail_Status_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_OrderDetail_Status_t_array(const arena_vector<OrderDetail_Status_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_OrderDetail_Status_t_array_into(arr, json);
    return json;
}

inline void serialize_OrderDetail_Customer_name_t_array_into(const std::vector<OrderDetail_Customer_name_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_OrderDetail_Customer_name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_OrderDetail_Customer_name_t_array(const std::vector<OrderDetail_Customer_name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_OrderDetail_Customer_name_t_array_into(arr, json);
    return json;
}

inline void serialize_OrderDetail_Customer_name_t_array_into(const arena_vector<OrderDetail_Customer_name_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_OrderDetail_Customer_name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_OrderDetail_Customer_name_t_array(const arena_vector<OrderDetail_Customer_name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_OrderDetail_Customer_name_t_array_into(arr, json);
    return json;
}

inline void serialize_OrderDetail_Country_t_array_into(const std::vector<OrderDetail_Country_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_OrderDetail_Country_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_OrderDetail_Country_t_array(const std::vector<OrderDetail_Country_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_OrderDetail_Country_t_array_into(arr, json);
    return json;
}

inline void serialize_OrderDetail_Country_t_array_into(const arena_vector<OrderDetail_Country_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_OrderDetail_Country_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_OrderDetail_Country_t_array(const arena_vector<OrderDetail_Country_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_OrderDetail_Country_t_array_into(arr, json);
    return json;
}

inline void serialize_OrderDetail_Line_count_t_array_into(const std::vector<OrderDetail_Line_count_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_OrderDetail_Line_count_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_OrderDetail_Line_count_t_array(const std::vector<OrderDetail_Line_count_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_OrderDetail_Line_count_t_array_into(arr, json);
    return json;
}

inline void serialize_OrderDetail_Line_count_t_array_into(const arena_vector<OrderDetail_Line_count_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_OrderDetail_Line_count_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_OrderDetail_Line_count_t_array(const arena_vector<OrderDetail_Line_count_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_OrderDetail_Line_count_t_array_into(arr, json);
    return json;
}

inline void serialize_OrderDetail_Total_qty_t_array_into(const std::vector<OrderDetail_Total_qty_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_OrderDetail_Total_qty_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_OrderDetail_Total_qty_t_array(const std::vector<OrderDetail_Total_qty_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_OrderDetail_Total_qty_t_array_into(arr, json);
    return json;
}

inline void serialize_OrderDetail_Total_qty_t_array_into(const arena_vector<OrderDetail_Total_qty_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_OrderDetail_Total_qty_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_OrderDetail_Total_qty_t_array(const arena_vector<OrderDetail_Total_qty_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_OrderDetail_Total_qty_t_array_into(arr, json);
    return json;
}

inline void serialize_OrderDetail_Total_amount_t_array_into(const std::vector<OrderDetail_Total_amount_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_OrderDetail_Total_amount_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_OrderDetail_Total_amount_t_array(const std::vector<OrderDetail_Total_amount_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_OrderDetail_Total_amount_t_array_into(arr, json);
    return json;
}

inline void serialize_OrderDetail_Total_amount_t_array_into(const arena_vector<OrderDetail_Total_amount_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_OrderDetail_Total_amount_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_OrderDetail_Total_amount_t_array(const arena_vector<OrderDetail_Total_amount_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_OrderDetail_Total_amount_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateOrderRequest_array_into(const std::vector<CreateOrderRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateOrderRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateOrderRequest_array(const std::vector<CreateOrderRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 79 + 2);
    serialize_CreateOrderRequest_array_into(arr, json);
    return json;
}

inline void serialize_CreateOrderRequest_array_into(const arena_vector<CreateOrderRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateOrderRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateOrderRequest_array(const arena_vector<CreateOrderRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 79 + 2);
    serialize_CreateOrderRequest_array_into(arr, json);
    return json;
}

inline void serialize_CreateOrderRequest_Customer_id_t_array_into(const std::vector<CreateOrderRequest_Customer_id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateOrderRequest_Customer_id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateOrderRequest_Customer_id_t_array(const std::vector<CreateOrderRequest_Customer_id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CreateOrderRequest_Customer_id_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateOrderRequest_Customer_id_t_array_into(const arena_vector<CreateOrderRequest_Customer_id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateOrderRequest_Customer_id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateOrderRequest_Customer_id_t_array(const arena_vector<CreateOrderRequest_Customer_id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CreateOrderRequest_Customer_id_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateOrderRequest_Status_t_array_into(const std::vector<CreateOrderRequest_Status_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateOrderRequest_Status_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateOrderRequest_Status_t_array(const std::vector<CreateOrderRequest_Status_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CreateOrderRequest_Status_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateOrderRequest_Status_t_array_into(const arena_vector<CreateOrderRequest_Status_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateOrderRequest_Status_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateOrderRequest_Status_t_array(const arena_vector<CreateOrderRequest_Status_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CreateOrderRequest_Status_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateOrderResponse_array_into(const std::vector<CreateOrderResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateOrderResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateOrderResponse_array(const std::vector<CreateOrderResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 28 + 2);
    serialize_CreateOrderResponse_array_into(arr, json);
    return json;
}

inline void serialize_CreateOrderResponse_array_into(const arena_vector<CreateOrderResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateOrderResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateOrderResponse_array(const arena_vector<CreateOrderResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 28 + 2);
    serialize_CreateOrderResponse_array_into(arr, json);
    return json;
}

inline void serialize_CreateOrderResponse_Id_t_array_into(const std::vector<CreateOrderResponse_Id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateOrderResponse_Id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateOrderResponse_Id_t_array(const std::vector<CreateOrderResponse_Id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CreateOrderResponse_Id_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateOrderResponse_Id_t_array_into(const arena_vector<CreateOrderResponse_Id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateOrderResponse_Id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateOrderResponse_Id_t_array(const arena_vector<CreateOrderResponse_Id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CreateOrderResponse_Id_t_array_into(arr, json);
    return json;
}

inline void serialize_customer_revenue_param_country_array_into(const std::vector<customer_revenue_param_country>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_customer_revenue_param_country_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_customer_revenue_param_country_array(const std::vector<customer_revenue_param_country>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_customer_revenue_param_country_array_into(arr, json);
    return json;
}

inline void serialize_customer_revenue_param_country_array_into(const arena_vector<customer_revenue_param_country>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_customer_revenue_param_country_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_customer_revenue_param_country_array(const arena_vector<customer_revenue_param_country>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_customer_revenue_param_country_array_into(arr, json);
    return json;
}

inline void serialize_customer_revenue_param_limit_array_into(const std::vector<customer_revenue_param_limit>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_customer_revenue_param_limit_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_customer_revenue_param_limit_array(const std::vector<customer_revenue_param_limit>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_customer_revenue_param_limit_array_into(arr, json);
    return json;
}

inline void serialize_customer_revenue_param_limit_array_into(const arena_vector<customer_revenue_param_limit>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_customer_revenue_param_limit_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_customer_revenue_param_limit_array(const arena_vector<customer_revenue_param_limit>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_customer_revenue_param_limit_array_into(arr, json);
    return json;
}

inline void serialize_customer_revenue_param_offset_array_into(const std::vector<customer_revenue_param_offset>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_customer_revenue_param_offset_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_customer_revenue_param_offset_array(const std::vector<customer_revenue_param_offset>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_customer_revenue_param_offset_array_into(arr, json);
    return json;
}

inline void serialize_customer_revenue_param_offset_array_into(const arena_vector<customer_revenue_param_offset>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_customer_revenue_param_offset_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_customer_revenue_param_offset_array(const arena_vector<customer_revenue_param_offset>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_customer_revenue_param_offset_array_into(arr, json);
    return json;
}

inline void serialize_schema_array_into(const std::vector<schema>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_array(const std::vector<schema>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_array_into(arr, json);
    return json;
}

inline void serialize_schema_array_into(const arena_vector<schema>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_array(const arena_vector<schema>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_array_into(arr, json);
    return json;
}

inline void serialize_top_products_param_limit_array_into(const std::vector<top_products_param_limit>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_top_products_param_limit_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_top_products_param_limit_array(const std::vector<top_products_param_limit>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_top_products_param_limit_array_into(arr, json);
    return json;
}

inline void serialize_top_products_param_limit_array_into(const arena_vector<top_products_param_limit>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_top_products_param_limit_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_top_products_param_limit_array(const arena_vector<top_products_param_limit>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_top_products_param_limit_array_into(arr, json);
    return json;
}

inline void serialize_schema_1_array_into(const std::vector<schema_1>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_1_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_1_array(const std::vector<schema_1>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_1_array_into(arr, json);
    return json;
}

inline void serialize_schema_1_array_into(const arena_vector<schema_1>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_1_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_1_array(const arena_vector<schema_1>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_1_array_into(arr, json);
    return json;
}

inline void serialize_schema_2_array_into(const std::vector<schema_2>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_2_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_2_array(const std::vector<schema_2>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_2_array_into(arr, json);
    return json;
}

inline void serialize_schema_2_array_into(const arena_vector<schema_2>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_2_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_2_array(const arena_vector<schema_2>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_2_array_into(arr, json);
    return json;
}

inline void serialize_order_detail_param_id_array_into(const std::vector<order_detail_param_id>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_order_detail_param_id_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_order_detail_param_id_array(const std::vector<order_detail_param_id>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_order_detail_param_id_array_into(arr, json);
    return json;
}

inline void serialize_order_detail_param_id_array_into(const arena_vector<order_detail_param_id>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_order_detail_param_id_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_order_detail_param_id_array(const arena_vector<order_detail_param_id>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_order_detail_param_id_array_into(arr, json);
    return json;
}

inline void serialize_schema_3_array_into(const std::vector<schema_3>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_3_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_3_array(const std::vector<schema_3>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_3_array_into(arr, json);
    return json;
}

inline void serialize_schema_3_array_into(const arena_vector<schema_3>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_3_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_3_array(const arena_vector<schema_3>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_3_array_into(arr, json);
    return json;
}

inline void serialize_schema_4_array_into(const std::vector<schema_4>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_4_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_4_array(const std::vector<schema_4>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_4_array_into(arr, json);
    return json;
}

inline void serialize_schema_4_array_into(const arena_vector<schema_4>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_4_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_4_array(const arena_vector<schema_4>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_4_array_into(arr, json);
    return json;
}

inline void serialize_schema_5_array_into(const std::vector<schema_5>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_5_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_5_array(const std::vector<schema_5>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_5_array_into(arr, json);
    return json;
}

inline void serialize_schema_5_array_into(const arena_vector<schema_5>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_5_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_5_array(const arena_vector<schema_5>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_5_array_into(arr, json);
    return json;
}

