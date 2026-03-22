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

[[nodiscard]] inline std::optional<SumRequest> parse_SumRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SumRequest_Values_t> parse_SumRequest_Values_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SumRequest_Item_t> parse_SumRequest_Item_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SumResponse> parse_SumResponse(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SumResponse_Result_t> parse_SumResponse_Result_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SumResponse_Count_t> parse_SumResponse_Count_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsRequest> parse_StatsRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsRequest_Values_t> parse_StatsRequest_Values_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsRequest_Item_t> parse_StatsRequest_Item_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsRequest_Include_median_t> parse_StatsRequest_Include_median_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsResponse> parse_StatsResponse(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsResponse_Min_t> parse_StatsResponse_Min_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsResponse_Max_t> parse_StatsResponse_Max_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsResponse_Mean_t> parse_StatsResponse_Mean_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsResponse_Sum_t> parse_StatsResponse_Sum_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsResponse_Count_t> parse_StatsResponse_Count_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsResponse_Median_t> parse_StatsResponse_Median_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest> parse_RegisterRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest_Username_t> parse_RegisterRequest_Username_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest_Email_t> parse_RegisterRequest_Email_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest_Password_t> parse_RegisterRequest_Password_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest_Age_t> parse_RegisterRequest_Age_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest_Role_t> parse_RegisterRequest_Role_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest_Tags_t> parse_RegisterRequest_Tags_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest_Item_t> parse_RegisterRequest_Item_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserRole> parse_UserRole(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserResponse> parse_UserResponse(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserResponse_Id_t> parse_UserResponse_Id_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserResponse_Username_t> parse_UserResponse_Username_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserResponse_Email_t> parse_UserResponse_Email_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserResponse_Role_t> parse_UserResponse_Role_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserResponse_Created_at_t> parse_UserResponse_Created_at_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemCategory> parse_ItemCategory(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item> parse_Item(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item_Id_t> parse_Item_Id_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item_Name_t> parse_Item_Name_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item_Description_t> parse_Item_Description_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item_Price_t> parse_Item_Price_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item_Stock_t> parse_Item_Stock_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item_Category_t> parse_Item_Category_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item_Tags_t> parse_Item_Tags_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item_Item_t> parse_Item_Item_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item_Metadata_t> parse_Item_Metadata_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemMetadata> parse_ItemMetadata(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemMetadata_Weight_kg_t> parse_ItemMetadata_Weight_kg_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemMetadata_Dimensions_t> parse_ItemMetadata_Dimensions_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemMetadata_Manufacturer_t> parse_ItemMetadata_Manufacturer_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemMetadata_Sku_t> parse_ItemMetadata_Sku_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest> parse_CreateItemRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest_Name_t> parse_CreateItemRequest_Name_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest_Description_t> parse_CreateItemRequest_Description_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest_Price_t> parse_CreateItemRequest_Price_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest_Stock_t> parse_CreateItemRequest_Stock_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest_Category_t> parse_CreateItemRequest_Category_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest_Tags_t> parse_CreateItemRequest_Tags_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest_Item_t> parse_CreateItemRequest_Item_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest_Metadata_t> parse_CreateItemRequest_Metadata_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest> parse_UpdateItemRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest_Name_t> parse_UpdateItemRequest_Name_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest_Description_t> parse_UpdateItemRequest_Description_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest_Price_t> parse_UpdateItemRequest_Price_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest_Stock_t> parse_UpdateItemRequest_Stock_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest_Category_t> parse_UpdateItemRequest_Category_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest_Tags_t> parse_UpdateItemRequest_Tags_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest_Item_t> parse_UpdateItemRequest_Item_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemList> parse_ItemList(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemList_Items_t> parse_ItemList_Items_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemList_Item_t> parse_ItemList_Item_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemList_Total_t> parse_ItemList_Total_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemList_Limit_t> parse_ItemList_Limit_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemList_Offset_t> parse_ItemList_Offset_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoRequest> parse_EchoRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoRequest_Message_t> parse_EchoRequest_Message_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoRequest_Repeat_t> parse_EchoRequest_Repeat_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoRequest_Uppercase_t> parse_EchoRequest_Uppercase_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoResponse> parse_EchoResponse(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoResponse_Message_t> parse_EchoResponse_Message_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoResponse_Length_t> parse_EchoResponse_Length_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema> parse_schema(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_1> parse_schema_1(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_2> parse_schema_2(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_3> parse_schema_3(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_4> parse_schema_4(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_5> parse_schema_5(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<list_items_param_limit> parse_list_items_param_limit(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<list_items_param_offset> parse_list_items_param_offset(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_6> parse_schema_6(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_7> parse_schema_7(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<create_item_param_X_Request_Id> parse_create_item_param_X_Request_Id(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<create_item_param_session> parse_create_item_param_session(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_8> parse_schema_8(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_9> parse_schema_9(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<get_item_param_id> parse_get_item_param_id(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_10> parse_schema_10(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<update_item_param_id> parse_update_item_param_id(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_11> parse_schema_11(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_12> parse_schema_12(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<delete_item_param_id> parse_delete_item_param_id(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_13> parse_schema_13(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_14> parse_schema_14(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<health_check_response> parse_health_check_response(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<health_check_response_Status> parse_health_check_response_Status(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<health_check_response_Uptime_ms> parse_health_check_response_Uptime_ms(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<SumRequest> parse_SumRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SumRequest_Values_t> parse_SumRequest_Values_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SumRequest_Item_t> parse_SumRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SumResponse> parse_SumResponse(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SumResponse_Result_t> parse_SumResponse_Result_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SumResponse_Count_t> parse_SumResponse_Count_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsRequest> parse_StatsRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsRequest_Values_t> parse_StatsRequest_Values_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsRequest_Item_t> parse_StatsRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsRequest_Include_median_t> parse_StatsRequest_Include_median_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsResponse> parse_StatsResponse(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsResponse_Min_t> parse_StatsResponse_Min_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsResponse_Max_t> parse_StatsResponse_Max_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsResponse_Mean_t> parse_StatsResponse_Mean_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsResponse_Sum_t> parse_StatsResponse_Sum_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsResponse_Count_t> parse_StatsResponse_Count_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsResponse_Median_t> parse_StatsResponse_Median_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest> parse_RegisterRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest_Username_t> parse_RegisterRequest_Username_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest_Email_t> parse_RegisterRequest_Email_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest_Password_t> parse_RegisterRequest_Password_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest_Age_t> parse_RegisterRequest_Age_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest_Role_t> parse_RegisterRequest_Role_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest_Tags_t> parse_RegisterRequest_Tags_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest_Item_t> parse_RegisterRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserRole> parse_UserRole(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserResponse> parse_UserResponse(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserResponse_Id_t> parse_UserResponse_Id_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserResponse_Username_t> parse_UserResponse_Username_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserResponse_Email_t> parse_UserResponse_Email_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserResponse_Role_t> parse_UserResponse_Role_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserResponse_Created_at_t> parse_UserResponse_Created_at_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemCategory> parse_ItemCategory(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item> parse_Item(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item_Id_t> parse_Item_Id_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item_Name_t> parse_Item_Name_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item_Description_t> parse_Item_Description_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item_Price_t> parse_Item_Price_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item_Stock_t> parse_Item_Stock_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item_Category_t> parse_Item_Category_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item_Tags_t> parse_Item_Tags_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item_Item_t> parse_Item_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item_Metadata_t> parse_Item_Metadata_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemMetadata> parse_ItemMetadata(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemMetadata_Weight_kg_t> parse_ItemMetadata_Weight_kg_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemMetadata_Dimensions_t> parse_ItemMetadata_Dimensions_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemMetadata_Manufacturer_t> parse_ItemMetadata_Manufacturer_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemMetadata_Sku_t> parse_ItemMetadata_Sku_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest> parse_CreateItemRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest_Name_t> parse_CreateItemRequest_Name_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest_Description_t> parse_CreateItemRequest_Description_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest_Price_t> parse_CreateItemRequest_Price_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest_Stock_t> parse_CreateItemRequest_Stock_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest_Category_t> parse_CreateItemRequest_Category_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest_Tags_t> parse_CreateItemRequest_Tags_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest_Item_t> parse_CreateItemRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest_Metadata_t> parse_CreateItemRequest_Metadata_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest> parse_UpdateItemRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest_Name_t> parse_UpdateItemRequest_Name_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest_Description_t> parse_UpdateItemRequest_Description_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest_Price_t> parse_UpdateItemRequest_Price_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest_Stock_t> parse_UpdateItemRequest_Stock_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest_Category_t> parse_UpdateItemRequest_Category_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest_Tags_t> parse_UpdateItemRequest_Tags_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest_Item_t> parse_UpdateItemRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemList> parse_ItemList(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemList_Items_t> parse_ItemList_Items_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemList_Item_t> parse_ItemList_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemList_Total_t> parse_ItemList_Total_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemList_Limit_t> parse_ItemList_Limit_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemList_Offset_t> parse_ItemList_Offset_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoRequest> parse_EchoRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoRequest_Message_t> parse_EchoRequest_Message_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoRequest_Repeat_t> parse_EchoRequest_Repeat_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoRequest_Uppercase_t> parse_EchoRequest_Uppercase_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoResponse> parse_EchoResponse(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoResponse_Message_t> parse_EchoResponse_Message_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoResponse_Length_t> parse_EchoResponse_Length_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema> parse_schema(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_1> parse_schema_1(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_2> parse_schema_2(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_3> parse_schema_3(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_4> parse_schema_4(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_5> parse_schema_5(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<list_items_param_limit> parse_list_items_param_limit(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<list_items_param_offset> parse_list_items_param_offset(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_6> parse_schema_6(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_7> parse_schema_7(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<create_item_param_X_Request_Id> parse_create_item_param_X_Request_Id(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<create_item_param_session> parse_create_item_param_session(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_8> parse_schema_8(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_9> parse_schema_9(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<get_item_param_id> parse_get_item_param_id(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_10> parse_schema_10(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<update_item_param_id> parse_update_item_param_id(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_11> parse_schema_11(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_12> parse_schema_12(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<delete_item_param_id> parse_delete_item_param_id(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_13> parse_schema_13(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_14> parse_schema_14(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<health_check_response> parse_health_check_response(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<health_check_response_Status> parse_health_check_response_Status(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<health_check_response_Uptime_ms> parse_health_check_response_Uptime_ms(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_SumRequest_into(const SumRequest& obj, std::string& out);
inline void serialize_SumRequest_Values_t_into(const SumRequest_Values_t& obj, std::string& out);
inline void serialize_SumRequest_Item_t_into(const SumRequest_Item_t& obj, std::string& out);
inline void serialize_SumResponse_into(const SumResponse& obj, std::string& out);
inline void serialize_SumResponse_Result_t_into(const SumResponse_Result_t& obj, std::string& out);
inline void serialize_SumResponse_Count_t_into(const SumResponse_Count_t& obj, std::string& out);
inline void serialize_StatsRequest_into(const StatsRequest& obj, std::string& out);
inline void serialize_StatsRequest_Values_t_into(const StatsRequest_Values_t& obj, std::string& out);
inline void serialize_StatsRequest_Item_t_into(const StatsRequest_Item_t& obj, std::string& out);
inline void serialize_StatsRequest_Include_median_t_into(const StatsRequest_Include_median_t& obj, std::string& out);
inline void serialize_StatsResponse_into(const StatsResponse& obj, std::string& out);
inline void serialize_StatsResponse_Min_t_into(const StatsResponse_Min_t& obj, std::string& out);
inline void serialize_StatsResponse_Max_t_into(const StatsResponse_Max_t& obj, std::string& out);
inline void serialize_StatsResponse_Mean_t_into(const StatsResponse_Mean_t& obj, std::string& out);
inline void serialize_StatsResponse_Sum_t_into(const StatsResponse_Sum_t& obj, std::string& out);
inline void serialize_StatsResponse_Count_t_into(const StatsResponse_Count_t& obj, std::string& out);
inline void serialize_StatsResponse_Median_t_into(const StatsResponse_Median_t& obj, std::string& out);
inline void serialize_RegisterRequest_into(const RegisterRequest& obj, std::string& out);
inline void serialize_RegisterRequest_Username_t_into(const RegisterRequest_Username_t& obj, std::string& out);
inline void serialize_RegisterRequest_Email_t_into(const RegisterRequest_Email_t& obj, std::string& out);
inline void serialize_RegisterRequest_Password_t_into(const RegisterRequest_Password_t& obj, std::string& out);
inline void serialize_RegisterRequest_Age_t_into(const RegisterRequest_Age_t& obj, std::string& out);
inline void serialize_RegisterRequest_Role_t_into(const RegisterRequest_Role_t& obj, std::string& out);
inline void serialize_RegisterRequest_Tags_t_into(const RegisterRequest_Tags_t& obj, std::string& out);
inline void serialize_RegisterRequest_Item_t_into(const RegisterRequest_Item_t& obj, std::string& out);
inline void serialize_UserRole_into(const UserRole& obj, std::string& out);
inline void serialize_UserResponse_into(const UserResponse& obj, std::string& out);
inline void serialize_UserResponse_Id_t_into(const UserResponse_Id_t& obj, std::string& out);
inline void serialize_UserResponse_Username_t_into(const UserResponse_Username_t& obj, std::string& out);
inline void serialize_UserResponse_Email_t_into(const UserResponse_Email_t& obj, std::string& out);
inline void serialize_UserResponse_Role_t_into(const UserResponse_Role_t& obj, std::string& out);
inline void serialize_UserResponse_Created_at_t_into(const UserResponse_Created_at_t& obj, std::string& out);
inline void serialize_ItemCategory_into(const ItemCategory& obj, std::string& out);
inline void serialize_Item_into(const Item& obj, std::string& out);
inline void serialize_Item_Id_t_into(const Item_Id_t& obj, std::string& out);
inline void serialize_Item_Name_t_into(const Item_Name_t& obj, std::string& out);
inline void serialize_Item_Description_t_into(const Item_Description_t& obj, std::string& out);
inline void serialize_Item_Price_t_into(const Item_Price_t& obj, std::string& out);
inline void serialize_Item_Stock_t_into(const Item_Stock_t& obj, std::string& out);
inline void serialize_Item_Category_t_into(const Item_Category_t& obj, std::string& out);
inline void serialize_Item_Tags_t_into(const Item_Tags_t& obj, std::string& out);
inline void serialize_Item_Item_t_into(const Item_Item_t& obj, std::string& out);
inline void serialize_Item_Metadata_t_into(const Item_Metadata_t& obj, std::string& out);
inline void serialize_ItemMetadata_into(const ItemMetadata& obj, std::string& out);
inline void serialize_ItemMetadata_Weight_kg_t_into(const ItemMetadata_Weight_kg_t& obj, std::string& out);
inline void serialize_ItemMetadata_Dimensions_t_into(const ItemMetadata_Dimensions_t& obj, std::string& out);
inline void serialize_ItemMetadata_Manufacturer_t_into(const ItemMetadata_Manufacturer_t& obj, std::string& out);
inline void serialize_ItemMetadata_Sku_t_into(const ItemMetadata_Sku_t& obj, std::string& out);
inline void serialize_CreateItemRequest_into(const CreateItemRequest& obj, std::string& out);
inline void serialize_CreateItemRequest_Name_t_into(const CreateItemRequest_Name_t& obj, std::string& out);
inline void serialize_CreateItemRequest_Description_t_into(const CreateItemRequest_Description_t& obj, std::string& out);
inline void serialize_CreateItemRequest_Price_t_into(const CreateItemRequest_Price_t& obj, std::string& out);
inline void serialize_CreateItemRequest_Stock_t_into(const CreateItemRequest_Stock_t& obj, std::string& out);
inline void serialize_CreateItemRequest_Category_t_into(const CreateItemRequest_Category_t& obj, std::string& out);
inline void serialize_CreateItemRequest_Tags_t_into(const CreateItemRequest_Tags_t& obj, std::string& out);
inline void serialize_CreateItemRequest_Item_t_into(const CreateItemRequest_Item_t& obj, std::string& out);
inline void serialize_CreateItemRequest_Metadata_t_into(const CreateItemRequest_Metadata_t& obj, std::string& out);
inline void serialize_UpdateItemRequest_into(const UpdateItemRequest& obj, std::string& out);
inline void serialize_UpdateItemRequest_Name_t_into(const UpdateItemRequest_Name_t& obj, std::string& out);
inline void serialize_UpdateItemRequest_Description_t_into(const UpdateItemRequest_Description_t& obj, std::string& out);
inline void serialize_UpdateItemRequest_Price_t_into(const UpdateItemRequest_Price_t& obj, std::string& out);
inline void serialize_UpdateItemRequest_Stock_t_into(const UpdateItemRequest_Stock_t& obj, std::string& out);
inline void serialize_UpdateItemRequest_Category_t_into(const UpdateItemRequest_Category_t& obj, std::string& out);
inline void serialize_UpdateItemRequest_Tags_t_into(const UpdateItemRequest_Tags_t& obj, std::string& out);
inline void serialize_UpdateItemRequest_Item_t_into(const UpdateItemRequest_Item_t& obj, std::string& out);
inline void serialize_ItemList_into(const ItemList& obj, std::string& out);
inline void serialize_ItemList_Items_t_into(const ItemList_Items_t& obj, std::string& out);
inline void serialize_ItemList_Item_t_into(const ItemList_Item_t& obj, std::string& out);
inline void serialize_ItemList_Total_t_into(const ItemList_Total_t& obj, std::string& out);
inline void serialize_ItemList_Limit_t_into(const ItemList_Limit_t& obj, std::string& out);
inline void serialize_ItemList_Offset_t_into(const ItemList_Offset_t& obj, std::string& out);
inline void serialize_EchoRequest_into(const EchoRequest& obj, std::string& out);
inline void serialize_EchoRequest_Message_t_into(const EchoRequest_Message_t& obj, std::string& out);
inline void serialize_EchoRequest_Repeat_t_into(const EchoRequest_Repeat_t& obj, std::string& out);
inline void serialize_EchoRequest_Uppercase_t_into(const EchoRequest_Uppercase_t& obj, std::string& out);
inline void serialize_EchoResponse_into(const EchoResponse& obj, std::string& out);
inline void serialize_EchoResponse_Message_t_into(const EchoResponse_Message_t& obj, std::string& out);
inline void serialize_EchoResponse_Length_t_into(const EchoResponse_Length_t& obj, std::string& out);
inline void serialize_schema_into(const schema& obj, std::string& out);
inline void serialize_schema_1_into(const schema_1& obj, std::string& out);
inline void serialize_schema_2_into(const schema_2& obj, std::string& out);
inline void serialize_schema_3_into(const schema_3& obj, std::string& out);
inline void serialize_schema_4_into(const schema_4& obj, std::string& out);
inline void serialize_schema_5_into(const schema_5& obj, std::string& out);
inline void serialize_list_items_param_limit_into(const list_items_param_limit& obj, std::string& out);
inline void serialize_list_items_param_offset_into(const list_items_param_offset& obj, std::string& out);
inline void serialize_schema_6_into(const schema_6& obj, std::string& out);
inline void serialize_schema_7_into(const schema_7& obj, std::string& out);
inline void serialize_create_item_param_X_Request_Id_into(const create_item_param_X_Request_Id& obj, std::string& out);
inline void serialize_create_item_param_session_into(const create_item_param_session& obj, std::string& out);
inline void serialize_schema_8_into(const schema_8& obj, std::string& out);
inline void serialize_schema_9_into(const schema_9& obj, std::string& out);
inline void serialize_get_item_param_id_into(const get_item_param_id& obj, std::string& out);
inline void serialize_schema_10_into(const schema_10& obj, std::string& out);
inline void serialize_update_item_param_id_into(const update_item_param_id& obj, std::string& out);
inline void serialize_schema_11_into(const schema_11& obj, std::string& out);
inline void serialize_schema_12_into(const schema_12& obj, std::string& out);
inline void serialize_delete_item_param_id_into(const delete_item_param_id& obj, std::string& out);
inline void serialize_schema_13_into(const schema_13& obj, std::string& out);
inline void serialize_schema_14_into(const schema_14& obj, std::string& out);
inline void serialize_health_check_response_into(const health_check_response& obj, std::string& out);
inline void serialize_health_check_response_Status_into(const health_check_response_Status& obj, std::string& out);
inline void serialize_health_check_response_Uptime_ms_into(const health_check_response_Uptime_ms& obj, std::string& out);

inline std::string serialize_SumRequest(const SumRequest& obj);
inline std::string serialize_SumRequest_Values_t(const SumRequest_Values_t& obj);
inline std::string serialize_SumRequest_Item_t(const SumRequest_Item_t& obj);
inline std::string serialize_SumResponse(const SumResponse& obj);
inline std::string serialize_SumResponse_Result_t(const SumResponse_Result_t& obj);
inline std::string serialize_SumResponse_Count_t(const SumResponse_Count_t& obj);
inline std::string serialize_StatsRequest(const StatsRequest& obj);
inline std::string serialize_StatsRequest_Values_t(const StatsRequest_Values_t& obj);
inline std::string serialize_StatsRequest_Item_t(const StatsRequest_Item_t& obj);
inline std::string serialize_StatsRequest_Include_median_t(const StatsRequest_Include_median_t& obj);
inline std::string serialize_StatsResponse(const StatsResponse& obj);
inline std::string serialize_StatsResponse_Min_t(const StatsResponse_Min_t& obj);
inline std::string serialize_StatsResponse_Max_t(const StatsResponse_Max_t& obj);
inline std::string serialize_StatsResponse_Mean_t(const StatsResponse_Mean_t& obj);
inline std::string serialize_StatsResponse_Sum_t(const StatsResponse_Sum_t& obj);
inline std::string serialize_StatsResponse_Count_t(const StatsResponse_Count_t& obj);
inline std::string serialize_StatsResponse_Median_t(const StatsResponse_Median_t& obj);
inline std::string serialize_RegisterRequest(const RegisterRequest& obj);
inline std::string serialize_RegisterRequest_Username_t(const RegisterRequest_Username_t& obj);
inline std::string serialize_RegisterRequest_Email_t(const RegisterRequest_Email_t& obj);
inline std::string serialize_RegisterRequest_Password_t(const RegisterRequest_Password_t& obj);
inline std::string serialize_RegisterRequest_Age_t(const RegisterRequest_Age_t& obj);
inline std::string serialize_RegisterRequest_Role_t(const RegisterRequest_Role_t& obj);
inline std::string serialize_RegisterRequest_Tags_t(const RegisterRequest_Tags_t& obj);
inline std::string serialize_RegisterRequest_Item_t(const RegisterRequest_Item_t& obj);
inline std::string serialize_UserRole(const UserRole& obj);
inline std::string serialize_UserResponse(const UserResponse& obj);
inline std::string serialize_UserResponse_Id_t(const UserResponse_Id_t& obj);
inline std::string serialize_UserResponse_Username_t(const UserResponse_Username_t& obj);
inline std::string serialize_UserResponse_Email_t(const UserResponse_Email_t& obj);
inline std::string serialize_UserResponse_Role_t(const UserResponse_Role_t& obj);
inline std::string serialize_UserResponse_Created_at_t(const UserResponse_Created_at_t& obj);
inline std::string serialize_ItemCategory(const ItemCategory& obj);
inline std::string serialize_Item(const Item& obj);
inline std::string serialize_Item_Id_t(const Item_Id_t& obj);
inline std::string serialize_Item_Name_t(const Item_Name_t& obj);
inline std::string serialize_Item_Description_t(const Item_Description_t& obj);
inline std::string serialize_Item_Price_t(const Item_Price_t& obj);
inline std::string serialize_Item_Stock_t(const Item_Stock_t& obj);
inline std::string serialize_Item_Category_t(const Item_Category_t& obj);
inline std::string serialize_Item_Tags_t(const Item_Tags_t& obj);
inline std::string serialize_Item_Item_t(const Item_Item_t& obj);
inline std::string serialize_Item_Metadata_t(const Item_Metadata_t& obj);
inline std::string serialize_ItemMetadata(const ItemMetadata& obj);
inline std::string serialize_ItemMetadata_Weight_kg_t(const ItemMetadata_Weight_kg_t& obj);
inline std::string serialize_ItemMetadata_Dimensions_t(const ItemMetadata_Dimensions_t& obj);
inline std::string serialize_ItemMetadata_Manufacturer_t(const ItemMetadata_Manufacturer_t& obj);
inline std::string serialize_ItemMetadata_Sku_t(const ItemMetadata_Sku_t& obj);
inline std::string serialize_CreateItemRequest(const CreateItemRequest& obj);
inline std::string serialize_CreateItemRequest_Name_t(const CreateItemRequest_Name_t& obj);
inline std::string serialize_CreateItemRequest_Description_t(const CreateItemRequest_Description_t& obj);
inline std::string serialize_CreateItemRequest_Price_t(const CreateItemRequest_Price_t& obj);
inline std::string serialize_CreateItemRequest_Stock_t(const CreateItemRequest_Stock_t& obj);
inline std::string serialize_CreateItemRequest_Category_t(const CreateItemRequest_Category_t& obj);
inline std::string serialize_CreateItemRequest_Tags_t(const CreateItemRequest_Tags_t& obj);
inline std::string serialize_CreateItemRequest_Item_t(const CreateItemRequest_Item_t& obj);
inline std::string serialize_CreateItemRequest_Metadata_t(const CreateItemRequest_Metadata_t& obj);
inline std::string serialize_UpdateItemRequest(const UpdateItemRequest& obj);
inline std::string serialize_UpdateItemRequest_Name_t(const UpdateItemRequest_Name_t& obj);
inline std::string serialize_UpdateItemRequest_Description_t(const UpdateItemRequest_Description_t& obj);
inline std::string serialize_UpdateItemRequest_Price_t(const UpdateItemRequest_Price_t& obj);
inline std::string serialize_UpdateItemRequest_Stock_t(const UpdateItemRequest_Stock_t& obj);
inline std::string serialize_UpdateItemRequest_Category_t(const UpdateItemRequest_Category_t& obj);
inline std::string serialize_UpdateItemRequest_Tags_t(const UpdateItemRequest_Tags_t& obj);
inline std::string serialize_UpdateItemRequest_Item_t(const UpdateItemRequest_Item_t& obj);
inline std::string serialize_ItemList(const ItemList& obj);
inline std::string serialize_ItemList_Items_t(const ItemList_Items_t& obj);
inline std::string serialize_ItemList_Item_t(const ItemList_Item_t& obj);
inline std::string serialize_ItemList_Total_t(const ItemList_Total_t& obj);
inline std::string serialize_ItemList_Limit_t(const ItemList_Limit_t& obj);
inline std::string serialize_ItemList_Offset_t(const ItemList_Offset_t& obj);
inline std::string serialize_EchoRequest(const EchoRequest& obj);
inline std::string serialize_EchoRequest_Message_t(const EchoRequest_Message_t& obj);
inline std::string serialize_EchoRequest_Repeat_t(const EchoRequest_Repeat_t& obj);
inline std::string serialize_EchoRequest_Uppercase_t(const EchoRequest_Uppercase_t& obj);
inline std::string serialize_EchoResponse(const EchoResponse& obj);
inline std::string serialize_EchoResponse_Message_t(const EchoResponse_Message_t& obj);
inline std::string serialize_EchoResponse_Length_t(const EchoResponse_Length_t& obj);
inline std::string serialize_schema(const schema& obj);
inline std::string serialize_schema_1(const schema_1& obj);
inline std::string serialize_schema_2(const schema_2& obj);
inline std::string serialize_schema_3(const schema_3& obj);
inline std::string serialize_schema_4(const schema_4& obj);
inline std::string serialize_schema_5(const schema_5& obj);
inline std::string serialize_list_items_param_limit(const list_items_param_limit& obj);
inline std::string serialize_list_items_param_offset(const list_items_param_offset& obj);
inline std::string serialize_schema_6(const schema_6& obj);
inline std::string serialize_schema_7(const schema_7& obj);
inline std::string serialize_create_item_param_X_Request_Id(const create_item_param_X_Request_Id& obj);
inline std::string serialize_create_item_param_session(const create_item_param_session& obj);
inline std::string serialize_schema_8(const schema_8& obj);
inline std::string serialize_schema_9(const schema_9& obj);
inline std::string serialize_get_item_param_id(const get_item_param_id& obj);
inline std::string serialize_schema_10(const schema_10& obj);
inline std::string serialize_update_item_param_id(const update_item_param_id& obj);
inline std::string serialize_schema_11(const schema_11& obj);
inline std::string serialize_schema_12(const schema_12& obj);
inline std::string serialize_delete_item_param_id(const delete_item_param_id& obj);
inline std::string serialize_schema_13(const schema_13& obj);
inline std::string serialize_schema_14(const schema_14& obj);
inline std::string serialize_health_check_response(const health_check_response& obj);
inline std::string serialize_health_check_response_Status(const health_check_response_Status& obj);
inline std::string serialize_health_check_response_Uptime_ms(const health_check_response_Uptime_ms& obj);

[[nodiscard]] inline std::optional<std::vector<SumRequest>> parse_SumRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SumRequest_Values_t>> parse_SumRequest_Values_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SumRequest_Item_t>> parse_SumRequest_Item_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SumResponse>> parse_SumResponse_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SumResponse_Result_t>> parse_SumResponse_Result_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SumResponse_Count_t>> parse_SumResponse_Count_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsRequest>> parse_StatsRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsRequest_Values_t>> parse_StatsRequest_Values_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsRequest_Item_t>> parse_StatsRequest_Item_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsRequest_Include_median_t>> parse_StatsRequest_Include_median_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsResponse>> parse_StatsResponse_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsResponse_Min_t>> parse_StatsResponse_Min_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsResponse_Max_t>> parse_StatsResponse_Max_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsResponse_Mean_t>> parse_StatsResponse_Mean_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsResponse_Sum_t>> parse_StatsResponse_Sum_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsResponse_Count_t>> parse_StatsResponse_Count_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsResponse_Median_t>> parse_StatsResponse_Median_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterRequest>> parse_RegisterRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Username_t>> parse_RegisterRequest_Username_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Email_t>> parse_RegisterRequest_Email_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Password_t>> parse_RegisterRequest_Password_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Age_t>> parse_RegisterRequest_Age_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Role_t>> parse_RegisterRequest_Role_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Tags_t>> parse_RegisterRequest_Tags_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Item_t>> parse_RegisterRequest_Item_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserRole>> parse_UserRole_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserResponse>> parse_UserResponse_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserResponse_Id_t>> parse_UserResponse_Id_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserResponse_Username_t>> parse_UserResponse_Username_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserResponse_Email_t>> parse_UserResponse_Email_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserResponse_Role_t>> parse_UserResponse_Role_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserResponse_Created_at_t>> parse_UserResponse_Created_at_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemCategory>> parse_ItemCategory_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item>> parse_Item_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item_Id_t>> parse_Item_Id_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item_Name_t>> parse_Item_Name_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item_Description_t>> parse_Item_Description_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item_Price_t>> parse_Item_Price_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item_Stock_t>> parse_Item_Stock_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item_Category_t>> parse_Item_Category_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item_Tags_t>> parse_Item_Tags_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item_Item_t>> parse_Item_Item_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item_Metadata_t>> parse_Item_Metadata_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemMetadata>> parse_ItemMetadata_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemMetadata_Weight_kg_t>> parse_ItemMetadata_Weight_kg_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemMetadata_Dimensions_t>> parse_ItemMetadata_Dimensions_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemMetadata_Manufacturer_t>> parse_ItemMetadata_Manufacturer_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemMetadata_Sku_t>> parse_ItemMetadata_Sku_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest>> parse_CreateItemRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Name_t>> parse_CreateItemRequest_Name_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Description_t>> parse_CreateItemRequest_Description_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Price_t>> parse_CreateItemRequest_Price_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Stock_t>> parse_CreateItemRequest_Stock_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Category_t>> parse_CreateItemRequest_Category_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Tags_t>> parse_CreateItemRequest_Tags_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Item_t>> parse_CreateItemRequest_Item_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Metadata_t>> parse_CreateItemRequest_Metadata_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest>> parse_UpdateItemRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Name_t>> parse_UpdateItemRequest_Name_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Description_t>> parse_UpdateItemRequest_Description_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Price_t>> parse_UpdateItemRequest_Price_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Stock_t>> parse_UpdateItemRequest_Stock_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Category_t>> parse_UpdateItemRequest_Category_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Tags_t>> parse_UpdateItemRequest_Tags_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Item_t>> parse_UpdateItemRequest_Item_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemList>> parse_ItemList_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemList_Items_t>> parse_ItemList_Items_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemList_Item_t>> parse_ItemList_Item_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemList_Total_t>> parse_ItemList_Total_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemList_Limit_t>> parse_ItemList_Limit_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemList_Offset_t>> parse_ItemList_Offset_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<EchoRequest>> parse_EchoRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<EchoRequest_Message_t>> parse_EchoRequest_Message_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<EchoRequest_Repeat_t>> parse_EchoRequest_Repeat_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<EchoRequest_Uppercase_t>> parse_EchoRequest_Uppercase_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<EchoResponse>> parse_EchoResponse_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<EchoResponse_Message_t>> parse_EchoResponse_Message_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<EchoResponse_Length_t>> parse_EchoResponse_Length_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_1>> parse_schema_1_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_2>> parse_schema_2_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_3>> parse_schema_3_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_4>> parse_schema_4_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_5>> parse_schema_5_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<list_items_param_limit>> parse_list_items_param_limit_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<list_items_param_offset>> parse_list_items_param_offset_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_6>> parse_schema_6_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_7>> parse_schema_7_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<create_item_param_X_Request_Id>> parse_create_item_param_X_Request_Id_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<create_item_param_session>> parse_create_item_param_session_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_8>> parse_schema_8_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_9>> parse_schema_9_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<get_item_param_id>> parse_get_item_param_id_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_10>> parse_schema_10_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<update_item_param_id>> parse_update_item_param_id_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_11>> parse_schema_11_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_12>> parse_schema_12_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<delete_item_param_id>> parse_delete_item_param_id_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_13>> parse_schema_13_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_14>> parse_schema_14_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<health_check_response>> parse_health_check_response_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<health_check_response_Status>> parse_health_check_response_Status_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<health_check_response_Uptime_ms>> parse_health_check_response_Uptime_ms_array(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<std::vector<SumRequest>> parse_SumRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SumRequest_Values_t>> parse_SumRequest_Values_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SumRequest_Item_t>> parse_SumRequest_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SumResponse>> parse_SumResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SumResponse_Result_t>> parse_SumResponse_Result_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SumResponse_Count_t>> parse_SumResponse_Count_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsRequest>> parse_StatsRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsRequest_Values_t>> parse_StatsRequest_Values_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsRequest_Item_t>> parse_StatsRequest_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsRequest_Include_median_t>> parse_StatsRequest_Include_median_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsResponse>> parse_StatsResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsResponse_Min_t>> parse_StatsResponse_Min_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsResponse_Max_t>> parse_StatsResponse_Max_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsResponse_Mean_t>> parse_StatsResponse_Mean_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsResponse_Sum_t>> parse_StatsResponse_Sum_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsResponse_Count_t>> parse_StatsResponse_Count_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsResponse_Median_t>> parse_StatsResponse_Median_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterRequest>> parse_RegisterRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Username_t>> parse_RegisterRequest_Username_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Email_t>> parse_RegisterRequest_Email_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Password_t>> parse_RegisterRequest_Password_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Age_t>> parse_RegisterRequest_Age_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Role_t>> parse_RegisterRequest_Role_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Tags_t>> parse_RegisterRequest_Tags_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Item_t>> parse_RegisterRequest_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserRole>> parse_UserRole_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserResponse>> parse_UserResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserResponse_Id_t>> parse_UserResponse_Id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserResponse_Username_t>> parse_UserResponse_Username_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserResponse_Email_t>> parse_UserResponse_Email_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserResponse_Role_t>> parse_UserResponse_Role_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserResponse_Created_at_t>> parse_UserResponse_Created_at_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemCategory>> parse_ItemCategory_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item>> parse_Item_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item_Id_t>> parse_Item_Id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item_Name_t>> parse_Item_Name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item_Description_t>> parse_Item_Description_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item_Price_t>> parse_Item_Price_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item_Stock_t>> parse_Item_Stock_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item_Category_t>> parse_Item_Category_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item_Tags_t>> parse_Item_Tags_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item_Item_t>> parse_Item_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item_Metadata_t>> parse_Item_Metadata_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemMetadata>> parse_ItemMetadata_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemMetadata_Weight_kg_t>> parse_ItemMetadata_Weight_kg_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemMetadata_Dimensions_t>> parse_ItemMetadata_Dimensions_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemMetadata_Manufacturer_t>> parse_ItemMetadata_Manufacturer_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemMetadata_Sku_t>> parse_ItemMetadata_Sku_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest>> parse_CreateItemRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Name_t>> parse_CreateItemRequest_Name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Description_t>> parse_CreateItemRequest_Description_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Price_t>> parse_CreateItemRequest_Price_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Stock_t>> parse_CreateItemRequest_Stock_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Category_t>> parse_CreateItemRequest_Category_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Tags_t>> parse_CreateItemRequest_Tags_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Item_t>> parse_CreateItemRequest_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Metadata_t>> parse_CreateItemRequest_Metadata_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest>> parse_UpdateItemRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Name_t>> parse_UpdateItemRequest_Name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Description_t>> parse_UpdateItemRequest_Description_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Price_t>> parse_UpdateItemRequest_Price_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Stock_t>> parse_UpdateItemRequest_Stock_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Category_t>> parse_UpdateItemRequest_Category_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Tags_t>> parse_UpdateItemRequest_Tags_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Item_t>> parse_UpdateItemRequest_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemList>> parse_ItemList_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemList_Items_t>> parse_ItemList_Items_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemList_Item_t>> parse_ItemList_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemList_Total_t>> parse_ItemList_Total_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemList_Limit_t>> parse_ItemList_Limit_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemList_Offset_t>> parse_ItemList_Offset_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<EchoRequest>> parse_EchoRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<EchoRequest_Message_t>> parse_EchoRequest_Message_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<EchoRequest_Repeat_t>> parse_EchoRequest_Repeat_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<EchoRequest_Uppercase_t>> parse_EchoRequest_Uppercase_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<EchoResponse>> parse_EchoResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<EchoResponse_Message_t>> parse_EchoResponse_Message_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<EchoResponse_Length_t>> parse_EchoResponse_Length_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_1>> parse_schema_1_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_2>> parse_schema_2_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_3>> parse_schema_3_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_4>> parse_schema_4_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_5>> parse_schema_5_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<list_items_param_limit>> parse_list_items_param_limit_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<list_items_param_offset>> parse_list_items_param_offset_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_6>> parse_schema_6_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_7>> parse_schema_7_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<create_item_param_X_Request_Id>> parse_create_item_param_X_Request_Id_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<create_item_param_session>> parse_create_item_param_session_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_8>> parse_schema_8_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_9>> parse_schema_9_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<get_item_param_id>> parse_get_item_param_id_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_10>> parse_schema_10_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<update_item_param_id>> parse_update_item_param_id_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_11>> parse_schema_11_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_12>> parse_schema_12_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<delete_item_param_id>> parse_delete_item_param_id_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_13>> parse_schema_13_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_14>> parse_schema_14_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<health_check_response>> parse_health_check_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<health_check_response_Status>> parse_health_check_response_Status_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<health_check_response_Uptime_ms>> parse_health_check_response_Uptime_ms_array(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_SumRequest_array_into(const std::vector<SumRequest>& arr, std::string& out);
inline void serialize_SumRequest_array_into(const arena_vector<SumRequest>& arr, std::string& out);
inline void serialize_SumRequest_Values_t_array_into(const std::vector<SumRequest_Values_t>& arr, std::string& out);
inline void serialize_SumRequest_Values_t_array_into(const arena_vector<SumRequest_Values_t>& arr, std::string& out);
inline void serialize_SumRequest_Item_t_array_into(const std::vector<SumRequest_Item_t>& arr, std::string& out);
inline void serialize_SumRequest_Item_t_array_into(const arena_vector<SumRequest_Item_t>& arr, std::string& out);
inline void serialize_SumResponse_array_into(const std::vector<SumResponse>& arr, std::string& out);
inline void serialize_SumResponse_array_into(const arena_vector<SumResponse>& arr, std::string& out);
inline void serialize_SumResponse_Result_t_array_into(const std::vector<SumResponse_Result_t>& arr, std::string& out);
inline void serialize_SumResponse_Result_t_array_into(const arena_vector<SumResponse_Result_t>& arr, std::string& out);
inline void serialize_SumResponse_Count_t_array_into(const std::vector<SumResponse_Count_t>& arr, std::string& out);
inline void serialize_SumResponse_Count_t_array_into(const arena_vector<SumResponse_Count_t>& arr, std::string& out);
inline void serialize_StatsRequest_array_into(const std::vector<StatsRequest>& arr, std::string& out);
inline void serialize_StatsRequest_array_into(const arena_vector<StatsRequest>& arr, std::string& out);
inline void serialize_StatsRequest_Values_t_array_into(const std::vector<StatsRequest_Values_t>& arr, std::string& out);
inline void serialize_StatsRequest_Values_t_array_into(const arena_vector<StatsRequest_Values_t>& arr, std::string& out);
inline void serialize_StatsRequest_Item_t_array_into(const std::vector<StatsRequest_Item_t>& arr, std::string& out);
inline void serialize_StatsRequest_Item_t_array_into(const arena_vector<StatsRequest_Item_t>& arr, std::string& out);
inline void serialize_StatsRequest_Include_median_t_array_into(const std::vector<StatsRequest_Include_median_t>& arr, std::string& out);
inline void serialize_StatsRequest_Include_median_t_array_into(const arena_vector<StatsRequest_Include_median_t>& arr, std::string& out);
inline void serialize_StatsResponse_array_into(const std::vector<StatsResponse>& arr, std::string& out);
inline void serialize_StatsResponse_array_into(const arena_vector<StatsResponse>& arr, std::string& out);
inline void serialize_StatsResponse_Min_t_array_into(const std::vector<StatsResponse_Min_t>& arr, std::string& out);
inline void serialize_StatsResponse_Min_t_array_into(const arena_vector<StatsResponse_Min_t>& arr, std::string& out);
inline void serialize_StatsResponse_Max_t_array_into(const std::vector<StatsResponse_Max_t>& arr, std::string& out);
inline void serialize_StatsResponse_Max_t_array_into(const arena_vector<StatsResponse_Max_t>& arr, std::string& out);
inline void serialize_StatsResponse_Mean_t_array_into(const std::vector<StatsResponse_Mean_t>& arr, std::string& out);
inline void serialize_StatsResponse_Mean_t_array_into(const arena_vector<StatsResponse_Mean_t>& arr, std::string& out);
inline void serialize_StatsResponse_Sum_t_array_into(const std::vector<StatsResponse_Sum_t>& arr, std::string& out);
inline void serialize_StatsResponse_Sum_t_array_into(const arena_vector<StatsResponse_Sum_t>& arr, std::string& out);
inline void serialize_StatsResponse_Count_t_array_into(const std::vector<StatsResponse_Count_t>& arr, std::string& out);
inline void serialize_StatsResponse_Count_t_array_into(const arena_vector<StatsResponse_Count_t>& arr, std::string& out);
inline void serialize_StatsResponse_Median_t_array_into(const std::vector<StatsResponse_Median_t>& arr, std::string& out);
inline void serialize_StatsResponse_Median_t_array_into(const arena_vector<StatsResponse_Median_t>& arr, std::string& out);
inline void serialize_RegisterRequest_array_into(const std::vector<RegisterRequest>& arr, std::string& out);
inline void serialize_RegisterRequest_array_into(const arena_vector<RegisterRequest>& arr, std::string& out);
inline void serialize_RegisterRequest_Username_t_array_into(const std::vector<RegisterRequest_Username_t>& arr, std::string& out);
inline void serialize_RegisterRequest_Username_t_array_into(const arena_vector<RegisterRequest_Username_t>& arr, std::string& out);
inline void serialize_RegisterRequest_Email_t_array_into(const std::vector<RegisterRequest_Email_t>& arr, std::string& out);
inline void serialize_RegisterRequest_Email_t_array_into(const arena_vector<RegisterRequest_Email_t>& arr, std::string& out);
inline void serialize_RegisterRequest_Password_t_array_into(const std::vector<RegisterRequest_Password_t>& arr, std::string& out);
inline void serialize_RegisterRequest_Password_t_array_into(const arena_vector<RegisterRequest_Password_t>& arr, std::string& out);
inline void serialize_RegisterRequest_Age_t_array_into(const std::vector<RegisterRequest_Age_t>& arr, std::string& out);
inline void serialize_RegisterRequest_Age_t_array_into(const arena_vector<RegisterRequest_Age_t>& arr, std::string& out);
inline void serialize_RegisterRequest_Role_t_array_into(const std::vector<RegisterRequest_Role_t>& arr, std::string& out);
inline void serialize_RegisterRequest_Role_t_array_into(const arena_vector<RegisterRequest_Role_t>& arr, std::string& out);
inline void serialize_RegisterRequest_Tags_t_array_into(const std::vector<RegisterRequest_Tags_t>& arr, std::string& out);
inline void serialize_RegisterRequest_Tags_t_array_into(const arena_vector<RegisterRequest_Tags_t>& arr, std::string& out);
inline void serialize_RegisterRequest_Item_t_array_into(const std::vector<RegisterRequest_Item_t>& arr, std::string& out);
inline void serialize_RegisterRequest_Item_t_array_into(const arena_vector<RegisterRequest_Item_t>& arr, std::string& out);
inline void serialize_UserRole_array_into(const std::vector<UserRole>& arr, std::string& out);
inline void serialize_UserRole_array_into(const arena_vector<UserRole>& arr, std::string& out);
inline void serialize_UserResponse_array_into(const std::vector<UserResponse>& arr, std::string& out);
inline void serialize_UserResponse_array_into(const arena_vector<UserResponse>& arr, std::string& out);
inline void serialize_UserResponse_Id_t_array_into(const std::vector<UserResponse_Id_t>& arr, std::string& out);
inline void serialize_UserResponse_Id_t_array_into(const arena_vector<UserResponse_Id_t>& arr, std::string& out);
inline void serialize_UserResponse_Username_t_array_into(const std::vector<UserResponse_Username_t>& arr, std::string& out);
inline void serialize_UserResponse_Username_t_array_into(const arena_vector<UserResponse_Username_t>& arr, std::string& out);
inline void serialize_UserResponse_Email_t_array_into(const std::vector<UserResponse_Email_t>& arr, std::string& out);
inline void serialize_UserResponse_Email_t_array_into(const arena_vector<UserResponse_Email_t>& arr, std::string& out);
inline void serialize_UserResponse_Role_t_array_into(const std::vector<UserResponse_Role_t>& arr, std::string& out);
inline void serialize_UserResponse_Role_t_array_into(const arena_vector<UserResponse_Role_t>& arr, std::string& out);
inline void serialize_UserResponse_Created_at_t_array_into(const std::vector<UserResponse_Created_at_t>& arr, std::string& out);
inline void serialize_UserResponse_Created_at_t_array_into(const arena_vector<UserResponse_Created_at_t>& arr, std::string& out);
inline void serialize_ItemCategory_array_into(const std::vector<ItemCategory>& arr, std::string& out);
inline void serialize_ItemCategory_array_into(const arena_vector<ItemCategory>& arr, std::string& out);
inline void serialize_Item_array_into(const std::vector<Item>& arr, std::string& out);
inline void serialize_Item_array_into(const arena_vector<Item>& arr, std::string& out);
inline void serialize_Item_Id_t_array_into(const std::vector<Item_Id_t>& arr, std::string& out);
inline void serialize_Item_Id_t_array_into(const arena_vector<Item_Id_t>& arr, std::string& out);
inline void serialize_Item_Name_t_array_into(const std::vector<Item_Name_t>& arr, std::string& out);
inline void serialize_Item_Name_t_array_into(const arena_vector<Item_Name_t>& arr, std::string& out);
inline void serialize_Item_Description_t_array_into(const std::vector<Item_Description_t>& arr, std::string& out);
inline void serialize_Item_Description_t_array_into(const arena_vector<Item_Description_t>& arr, std::string& out);
inline void serialize_Item_Price_t_array_into(const std::vector<Item_Price_t>& arr, std::string& out);
inline void serialize_Item_Price_t_array_into(const arena_vector<Item_Price_t>& arr, std::string& out);
inline void serialize_Item_Stock_t_array_into(const std::vector<Item_Stock_t>& arr, std::string& out);
inline void serialize_Item_Stock_t_array_into(const arena_vector<Item_Stock_t>& arr, std::string& out);
inline void serialize_Item_Category_t_array_into(const std::vector<Item_Category_t>& arr, std::string& out);
inline void serialize_Item_Category_t_array_into(const arena_vector<Item_Category_t>& arr, std::string& out);
inline void serialize_Item_Tags_t_array_into(const std::vector<Item_Tags_t>& arr, std::string& out);
inline void serialize_Item_Tags_t_array_into(const arena_vector<Item_Tags_t>& arr, std::string& out);
inline void serialize_Item_Item_t_array_into(const std::vector<Item_Item_t>& arr, std::string& out);
inline void serialize_Item_Item_t_array_into(const arena_vector<Item_Item_t>& arr, std::string& out);
inline void serialize_Item_Metadata_t_array_into(const std::vector<Item_Metadata_t>& arr, std::string& out);
inline void serialize_Item_Metadata_t_array_into(const arena_vector<Item_Metadata_t>& arr, std::string& out);
inline void serialize_ItemMetadata_array_into(const std::vector<ItemMetadata>& arr, std::string& out);
inline void serialize_ItemMetadata_array_into(const arena_vector<ItemMetadata>& arr, std::string& out);
inline void serialize_ItemMetadata_Weight_kg_t_array_into(const std::vector<ItemMetadata_Weight_kg_t>& arr, std::string& out);
inline void serialize_ItemMetadata_Weight_kg_t_array_into(const arena_vector<ItemMetadata_Weight_kg_t>& arr, std::string& out);
inline void serialize_ItemMetadata_Dimensions_t_array_into(const std::vector<ItemMetadata_Dimensions_t>& arr, std::string& out);
inline void serialize_ItemMetadata_Dimensions_t_array_into(const arena_vector<ItemMetadata_Dimensions_t>& arr, std::string& out);
inline void serialize_ItemMetadata_Manufacturer_t_array_into(const std::vector<ItemMetadata_Manufacturer_t>& arr, std::string& out);
inline void serialize_ItemMetadata_Manufacturer_t_array_into(const arena_vector<ItemMetadata_Manufacturer_t>& arr, std::string& out);
inline void serialize_ItemMetadata_Sku_t_array_into(const std::vector<ItemMetadata_Sku_t>& arr, std::string& out);
inline void serialize_ItemMetadata_Sku_t_array_into(const arena_vector<ItemMetadata_Sku_t>& arr, std::string& out);
inline void serialize_CreateItemRequest_array_into(const std::vector<CreateItemRequest>& arr, std::string& out);
inline void serialize_CreateItemRequest_array_into(const arena_vector<CreateItemRequest>& arr, std::string& out);
inline void serialize_CreateItemRequest_Name_t_array_into(const std::vector<CreateItemRequest_Name_t>& arr, std::string& out);
inline void serialize_CreateItemRequest_Name_t_array_into(const arena_vector<CreateItemRequest_Name_t>& arr, std::string& out);
inline void serialize_CreateItemRequest_Description_t_array_into(const std::vector<CreateItemRequest_Description_t>& arr, std::string& out);
inline void serialize_CreateItemRequest_Description_t_array_into(const arena_vector<CreateItemRequest_Description_t>& arr, std::string& out);
inline void serialize_CreateItemRequest_Price_t_array_into(const std::vector<CreateItemRequest_Price_t>& arr, std::string& out);
inline void serialize_CreateItemRequest_Price_t_array_into(const arena_vector<CreateItemRequest_Price_t>& arr, std::string& out);
inline void serialize_CreateItemRequest_Stock_t_array_into(const std::vector<CreateItemRequest_Stock_t>& arr, std::string& out);
inline void serialize_CreateItemRequest_Stock_t_array_into(const arena_vector<CreateItemRequest_Stock_t>& arr, std::string& out);
inline void serialize_CreateItemRequest_Category_t_array_into(const std::vector<CreateItemRequest_Category_t>& arr, std::string& out);
inline void serialize_CreateItemRequest_Category_t_array_into(const arena_vector<CreateItemRequest_Category_t>& arr, std::string& out);
inline void serialize_CreateItemRequest_Tags_t_array_into(const std::vector<CreateItemRequest_Tags_t>& arr, std::string& out);
inline void serialize_CreateItemRequest_Tags_t_array_into(const arena_vector<CreateItemRequest_Tags_t>& arr, std::string& out);
inline void serialize_CreateItemRequest_Item_t_array_into(const std::vector<CreateItemRequest_Item_t>& arr, std::string& out);
inline void serialize_CreateItemRequest_Item_t_array_into(const arena_vector<CreateItemRequest_Item_t>& arr, std::string& out);
inline void serialize_CreateItemRequest_Metadata_t_array_into(const std::vector<CreateItemRequest_Metadata_t>& arr, std::string& out);
inline void serialize_CreateItemRequest_Metadata_t_array_into(const arena_vector<CreateItemRequest_Metadata_t>& arr, std::string& out);
inline void serialize_UpdateItemRequest_array_into(const std::vector<UpdateItemRequest>& arr, std::string& out);
inline void serialize_UpdateItemRequest_array_into(const arena_vector<UpdateItemRequest>& arr, std::string& out);
inline void serialize_UpdateItemRequest_Name_t_array_into(const std::vector<UpdateItemRequest_Name_t>& arr, std::string& out);
inline void serialize_UpdateItemRequest_Name_t_array_into(const arena_vector<UpdateItemRequest_Name_t>& arr, std::string& out);
inline void serialize_UpdateItemRequest_Description_t_array_into(const std::vector<UpdateItemRequest_Description_t>& arr, std::string& out);
inline void serialize_UpdateItemRequest_Description_t_array_into(const arena_vector<UpdateItemRequest_Description_t>& arr, std::string& out);
inline void serialize_UpdateItemRequest_Price_t_array_into(const std::vector<UpdateItemRequest_Price_t>& arr, std::string& out);
inline void serialize_UpdateItemRequest_Price_t_array_into(const arena_vector<UpdateItemRequest_Price_t>& arr, std::string& out);
inline void serialize_UpdateItemRequest_Stock_t_array_into(const std::vector<UpdateItemRequest_Stock_t>& arr, std::string& out);
inline void serialize_UpdateItemRequest_Stock_t_array_into(const arena_vector<UpdateItemRequest_Stock_t>& arr, std::string& out);
inline void serialize_UpdateItemRequest_Category_t_array_into(const std::vector<UpdateItemRequest_Category_t>& arr, std::string& out);
inline void serialize_UpdateItemRequest_Category_t_array_into(const arena_vector<UpdateItemRequest_Category_t>& arr, std::string& out);
inline void serialize_UpdateItemRequest_Tags_t_array_into(const std::vector<UpdateItemRequest_Tags_t>& arr, std::string& out);
inline void serialize_UpdateItemRequest_Tags_t_array_into(const arena_vector<UpdateItemRequest_Tags_t>& arr, std::string& out);
inline void serialize_UpdateItemRequest_Item_t_array_into(const std::vector<UpdateItemRequest_Item_t>& arr, std::string& out);
inline void serialize_UpdateItemRequest_Item_t_array_into(const arena_vector<UpdateItemRequest_Item_t>& arr, std::string& out);
inline void serialize_ItemList_array_into(const std::vector<ItemList>& arr, std::string& out);
inline void serialize_ItemList_array_into(const arena_vector<ItemList>& arr, std::string& out);
inline void serialize_ItemList_Items_t_array_into(const std::vector<ItemList_Items_t>& arr, std::string& out);
inline void serialize_ItemList_Items_t_array_into(const arena_vector<ItemList_Items_t>& arr, std::string& out);
inline void serialize_ItemList_Item_t_array_into(const std::vector<ItemList_Item_t>& arr, std::string& out);
inline void serialize_ItemList_Item_t_array_into(const arena_vector<ItemList_Item_t>& arr, std::string& out);
inline void serialize_ItemList_Total_t_array_into(const std::vector<ItemList_Total_t>& arr, std::string& out);
inline void serialize_ItemList_Total_t_array_into(const arena_vector<ItemList_Total_t>& arr, std::string& out);
inline void serialize_ItemList_Limit_t_array_into(const std::vector<ItemList_Limit_t>& arr, std::string& out);
inline void serialize_ItemList_Limit_t_array_into(const arena_vector<ItemList_Limit_t>& arr, std::string& out);
inline void serialize_ItemList_Offset_t_array_into(const std::vector<ItemList_Offset_t>& arr, std::string& out);
inline void serialize_ItemList_Offset_t_array_into(const arena_vector<ItemList_Offset_t>& arr, std::string& out);
inline void serialize_EchoRequest_array_into(const std::vector<EchoRequest>& arr, std::string& out);
inline void serialize_EchoRequest_array_into(const arena_vector<EchoRequest>& arr, std::string& out);
inline void serialize_EchoRequest_Message_t_array_into(const std::vector<EchoRequest_Message_t>& arr, std::string& out);
inline void serialize_EchoRequest_Message_t_array_into(const arena_vector<EchoRequest_Message_t>& arr, std::string& out);
inline void serialize_EchoRequest_Repeat_t_array_into(const std::vector<EchoRequest_Repeat_t>& arr, std::string& out);
inline void serialize_EchoRequest_Repeat_t_array_into(const arena_vector<EchoRequest_Repeat_t>& arr, std::string& out);
inline void serialize_EchoRequest_Uppercase_t_array_into(const std::vector<EchoRequest_Uppercase_t>& arr, std::string& out);
inline void serialize_EchoRequest_Uppercase_t_array_into(const arena_vector<EchoRequest_Uppercase_t>& arr, std::string& out);
inline void serialize_EchoResponse_array_into(const std::vector<EchoResponse>& arr, std::string& out);
inline void serialize_EchoResponse_array_into(const arena_vector<EchoResponse>& arr, std::string& out);
inline void serialize_EchoResponse_Message_t_array_into(const std::vector<EchoResponse_Message_t>& arr, std::string& out);
inline void serialize_EchoResponse_Message_t_array_into(const arena_vector<EchoResponse_Message_t>& arr, std::string& out);
inline void serialize_EchoResponse_Length_t_array_into(const std::vector<EchoResponse_Length_t>& arr, std::string& out);
inline void serialize_EchoResponse_Length_t_array_into(const arena_vector<EchoResponse_Length_t>& arr, std::string& out);
inline void serialize_schema_array_into(const std::vector<schema>& arr, std::string& out);
inline void serialize_schema_array_into(const arena_vector<schema>& arr, std::string& out);
inline void serialize_schema_1_array_into(const std::vector<schema_1>& arr, std::string& out);
inline void serialize_schema_1_array_into(const arena_vector<schema_1>& arr, std::string& out);
inline void serialize_schema_2_array_into(const std::vector<schema_2>& arr, std::string& out);
inline void serialize_schema_2_array_into(const arena_vector<schema_2>& arr, std::string& out);
inline void serialize_schema_3_array_into(const std::vector<schema_3>& arr, std::string& out);
inline void serialize_schema_3_array_into(const arena_vector<schema_3>& arr, std::string& out);
inline void serialize_schema_4_array_into(const std::vector<schema_4>& arr, std::string& out);
inline void serialize_schema_4_array_into(const arena_vector<schema_4>& arr, std::string& out);
inline void serialize_schema_5_array_into(const std::vector<schema_5>& arr, std::string& out);
inline void serialize_schema_5_array_into(const arena_vector<schema_5>& arr, std::string& out);
inline void serialize_list_items_param_limit_array_into(const std::vector<list_items_param_limit>& arr, std::string& out);
inline void serialize_list_items_param_limit_array_into(const arena_vector<list_items_param_limit>& arr, std::string& out);
inline void serialize_list_items_param_offset_array_into(const std::vector<list_items_param_offset>& arr, std::string& out);
inline void serialize_list_items_param_offset_array_into(const arena_vector<list_items_param_offset>& arr, std::string& out);
inline void serialize_schema_6_array_into(const std::vector<schema_6>& arr, std::string& out);
inline void serialize_schema_6_array_into(const arena_vector<schema_6>& arr, std::string& out);
inline void serialize_schema_7_array_into(const std::vector<schema_7>& arr, std::string& out);
inline void serialize_schema_7_array_into(const arena_vector<schema_7>& arr, std::string& out);
inline void serialize_create_item_param_X_Request_Id_array_into(const std::vector<create_item_param_X_Request_Id>& arr, std::string& out);
inline void serialize_create_item_param_X_Request_Id_array_into(const arena_vector<create_item_param_X_Request_Id>& arr, std::string& out);
inline void serialize_create_item_param_session_array_into(const std::vector<create_item_param_session>& arr, std::string& out);
inline void serialize_create_item_param_session_array_into(const arena_vector<create_item_param_session>& arr, std::string& out);
inline void serialize_schema_8_array_into(const std::vector<schema_8>& arr, std::string& out);
inline void serialize_schema_8_array_into(const arena_vector<schema_8>& arr, std::string& out);
inline void serialize_schema_9_array_into(const std::vector<schema_9>& arr, std::string& out);
inline void serialize_schema_9_array_into(const arena_vector<schema_9>& arr, std::string& out);
inline void serialize_get_item_param_id_array_into(const std::vector<get_item_param_id>& arr, std::string& out);
inline void serialize_get_item_param_id_array_into(const arena_vector<get_item_param_id>& arr, std::string& out);
inline void serialize_schema_10_array_into(const std::vector<schema_10>& arr, std::string& out);
inline void serialize_schema_10_array_into(const arena_vector<schema_10>& arr, std::string& out);
inline void serialize_update_item_param_id_array_into(const std::vector<update_item_param_id>& arr, std::string& out);
inline void serialize_update_item_param_id_array_into(const arena_vector<update_item_param_id>& arr, std::string& out);
inline void serialize_schema_11_array_into(const std::vector<schema_11>& arr, std::string& out);
inline void serialize_schema_11_array_into(const arena_vector<schema_11>& arr, std::string& out);
inline void serialize_schema_12_array_into(const std::vector<schema_12>& arr, std::string& out);
inline void serialize_schema_12_array_into(const arena_vector<schema_12>& arr, std::string& out);
inline void serialize_delete_item_param_id_array_into(const std::vector<delete_item_param_id>& arr, std::string& out);
inline void serialize_delete_item_param_id_array_into(const arena_vector<delete_item_param_id>& arr, std::string& out);
inline void serialize_schema_13_array_into(const std::vector<schema_13>& arr, std::string& out);
inline void serialize_schema_13_array_into(const arena_vector<schema_13>& arr, std::string& out);
inline void serialize_schema_14_array_into(const std::vector<schema_14>& arr, std::string& out);
inline void serialize_schema_14_array_into(const arena_vector<schema_14>& arr, std::string& out);
inline void serialize_health_check_response_array_into(const std::vector<health_check_response>& arr, std::string& out);
inline void serialize_health_check_response_array_into(const arena_vector<health_check_response>& arr, std::string& out);
inline void serialize_health_check_response_Status_array_into(const std::vector<health_check_response_Status>& arr, std::string& out);
inline void serialize_health_check_response_Status_array_into(const arena_vector<health_check_response_Status>& arr, std::string& out);
inline void serialize_health_check_response_Uptime_ms_array_into(const std::vector<health_check_response_Uptime_ms>& arr, std::string& out);
inline void serialize_health_check_response_Uptime_ms_array_into(const arena_vector<health_check_response_Uptime_ms>& arr, std::string& out);

inline std::string serialize_SumRequest_array(const std::vector<SumRequest>& arr);
inline std::string serialize_SumRequest_array(const arena_vector<SumRequest>& arr);
inline std::string serialize_SumRequest_Values_t_array(const std::vector<SumRequest_Values_t>& arr);
inline std::string serialize_SumRequest_Values_t_array(const arena_vector<SumRequest_Values_t>& arr);
inline std::string serialize_SumRequest_Item_t_array(const std::vector<SumRequest_Item_t>& arr);
inline std::string serialize_SumRequest_Item_t_array(const arena_vector<SumRequest_Item_t>& arr);
inline std::string serialize_SumResponse_array(const std::vector<SumResponse>& arr);
inline std::string serialize_SumResponse_array(const arena_vector<SumResponse>& arr);
inline std::string serialize_SumResponse_Result_t_array(const std::vector<SumResponse_Result_t>& arr);
inline std::string serialize_SumResponse_Result_t_array(const arena_vector<SumResponse_Result_t>& arr);
inline std::string serialize_SumResponse_Count_t_array(const std::vector<SumResponse_Count_t>& arr);
inline std::string serialize_SumResponse_Count_t_array(const arena_vector<SumResponse_Count_t>& arr);
inline std::string serialize_StatsRequest_array(const std::vector<StatsRequest>& arr);
inline std::string serialize_StatsRequest_array(const arena_vector<StatsRequest>& arr);
inline std::string serialize_StatsRequest_Values_t_array(const std::vector<StatsRequest_Values_t>& arr);
inline std::string serialize_StatsRequest_Values_t_array(const arena_vector<StatsRequest_Values_t>& arr);
inline std::string serialize_StatsRequest_Item_t_array(const std::vector<StatsRequest_Item_t>& arr);
inline std::string serialize_StatsRequest_Item_t_array(const arena_vector<StatsRequest_Item_t>& arr);
inline std::string serialize_StatsRequest_Include_median_t_array(const std::vector<StatsRequest_Include_median_t>& arr);
inline std::string serialize_StatsRequest_Include_median_t_array(const arena_vector<StatsRequest_Include_median_t>& arr);
inline std::string serialize_StatsResponse_array(const std::vector<StatsResponse>& arr);
inline std::string serialize_StatsResponse_array(const arena_vector<StatsResponse>& arr);
inline std::string serialize_StatsResponse_Min_t_array(const std::vector<StatsResponse_Min_t>& arr);
inline std::string serialize_StatsResponse_Min_t_array(const arena_vector<StatsResponse_Min_t>& arr);
inline std::string serialize_StatsResponse_Max_t_array(const std::vector<StatsResponse_Max_t>& arr);
inline std::string serialize_StatsResponse_Max_t_array(const arena_vector<StatsResponse_Max_t>& arr);
inline std::string serialize_StatsResponse_Mean_t_array(const std::vector<StatsResponse_Mean_t>& arr);
inline std::string serialize_StatsResponse_Mean_t_array(const arena_vector<StatsResponse_Mean_t>& arr);
inline std::string serialize_StatsResponse_Sum_t_array(const std::vector<StatsResponse_Sum_t>& arr);
inline std::string serialize_StatsResponse_Sum_t_array(const arena_vector<StatsResponse_Sum_t>& arr);
inline std::string serialize_StatsResponse_Count_t_array(const std::vector<StatsResponse_Count_t>& arr);
inline std::string serialize_StatsResponse_Count_t_array(const arena_vector<StatsResponse_Count_t>& arr);
inline std::string serialize_StatsResponse_Median_t_array(const std::vector<StatsResponse_Median_t>& arr);
inline std::string serialize_StatsResponse_Median_t_array(const arena_vector<StatsResponse_Median_t>& arr);
inline std::string serialize_RegisterRequest_array(const std::vector<RegisterRequest>& arr);
inline std::string serialize_RegisterRequest_array(const arena_vector<RegisterRequest>& arr);
inline std::string serialize_RegisterRequest_Username_t_array(const std::vector<RegisterRequest_Username_t>& arr);
inline std::string serialize_RegisterRequest_Username_t_array(const arena_vector<RegisterRequest_Username_t>& arr);
inline std::string serialize_RegisterRequest_Email_t_array(const std::vector<RegisterRequest_Email_t>& arr);
inline std::string serialize_RegisterRequest_Email_t_array(const arena_vector<RegisterRequest_Email_t>& arr);
inline std::string serialize_RegisterRequest_Password_t_array(const std::vector<RegisterRequest_Password_t>& arr);
inline std::string serialize_RegisterRequest_Password_t_array(const arena_vector<RegisterRequest_Password_t>& arr);
inline std::string serialize_RegisterRequest_Age_t_array(const std::vector<RegisterRequest_Age_t>& arr);
inline std::string serialize_RegisterRequest_Age_t_array(const arena_vector<RegisterRequest_Age_t>& arr);
inline std::string serialize_RegisterRequest_Role_t_array(const std::vector<RegisterRequest_Role_t>& arr);
inline std::string serialize_RegisterRequest_Role_t_array(const arena_vector<RegisterRequest_Role_t>& arr);
inline std::string serialize_RegisterRequest_Tags_t_array(const std::vector<RegisterRequest_Tags_t>& arr);
inline std::string serialize_RegisterRequest_Tags_t_array(const arena_vector<RegisterRequest_Tags_t>& arr);
inline std::string serialize_RegisterRequest_Item_t_array(const std::vector<RegisterRequest_Item_t>& arr);
inline std::string serialize_RegisterRequest_Item_t_array(const arena_vector<RegisterRequest_Item_t>& arr);
inline std::string serialize_UserRole_array(const std::vector<UserRole>& arr);
inline std::string serialize_UserRole_array(const arena_vector<UserRole>& arr);
inline std::string serialize_UserResponse_array(const std::vector<UserResponse>& arr);
inline std::string serialize_UserResponse_array(const arena_vector<UserResponse>& arr);
inline std::string serialize_UserResponse_Id_t_array(const std::vector<UserResponse_Id_t>& arr);
inline std::string serialize_UserResponse_Id_t_array(const arena_vector<UserResponse_Id_t>& arr);
inline std::string serialize_UserResponse_Username_t_array(const std::vector<UserResponse_Username_t>& arr);
inline std::string serialize_UserResponse_Username_t_array(const arena_vector<UserResponse_Username_t>& arr);
inline std::string serialize_UserResponse_Email_t_array(const std::vector<UserResponse_Email_t>& arr);
inline std::string serialize_UserResponse_Email_t_array(const arena_vector<UserResponse_Email_t>& arr);
inline std::string serialize_UserResponse_Role_t_array(const std::vector<UserResponse_Role_t>& arr);
inline std::string serialize_UserResponse_Role_t_array(const arena_vector<UserResponse_Role_t>& arr);
inline std::string serialize_UserResponse_Created_at_t_array(const std::vector<UserResponse_Created_at_t>& arr);
inline std::string serialize_UserResponse_Created_at_t_array(const arena_vector<UserResponse_Created_at_t>& arr);
inline std::string serialize_ItemCategory_array(const std::vector<ItemCategory>& arr);
inline std::string serialize_ItemCategory_array(const arena_vector<ItemCategory>& arr);
inline std::string serialize_Item_array(const std::vector<Item>& arr);
inline std::string serialize_Item_array(const arena_vector<Item>& arr);
inline std::string serialize_Item_Id_t_array(const std::vector<Item_Id_t>& arr);
inline std::string serialize_Item_Id_t_array(const arena_vector<Item_Id_t>& arr);
inline std::string serialize_Item_Name_t_array(const std::vector<Item_Name_t>& arr);
inline std::string serialize_Item_Name_t_array(const arena_vector<Item_Name_t>& arr);
inline std::string serialize_Item_Description_t_array(const std::vector<Item_Description_t>& arr);
inline std::string serialize_Item_Description_t_array(const arena_vector<Item_Description_t>& arr);
inline std::string serialize_Item_Price_t_array(const std::vector<Item_Price_t>& arr);
inline std::string serialize_Item_Price_t_array(const arena_vector<Item_Price_t>& arr);
inline std::string serialize_Item_Stock_t_array(const std::vector<Item_Stock_t>& arr);
inline std::string serialize_Item_Stock_t_array(const arena_vector<Item_Stock_t>& arr);
inline std::string serialize_Item_Category_t_array(const std::vector<Item_Category_t>& arr);
inline std::string serialize_Item_Category_t_array(const arena_vector<Item_Category_t>& arr);
inline std::string serialize_Item_Tags_t_array(const std::vector<Item_Tags_t>& arr);
inline std::string serialize_Item_Tags_t_array(const arena_vector<Item_Tags_t>& arr);
inline std::string serialize_Item_Item_t_array(const std::vector<Item_Item_t>& arr);
inline std::string serialize_Item_Item_t_array(const arena_vector<Item_Item_t>& arr);
inline std::string serialize_Item_Metadata_t_array(const std::vector<Item_Metadata_t>& arr);
inline std::string serialize_Item_Metadata_t_array(const arena_vector<Item_Metadata_t>& arr);
inline std::string serialize_ItemMetadata_array(const std::vector<ItemMetadata>& arr);
inline std::string serialize_ItemMetadata_array(const arena_vector<ItemMetadata>& arr);
inline std::string serialize_ItemMetadata_Weight_kg_t_array(const std::vector<ItemMetadata_Weight_kg_t>& arr);
inline std::string serialize_ItemMetadata_Weight_kg_t_array(const arena_vector<ItemMetadata_Weight_kg_t>& arr);
inline std::string serialize_ItemMetadata_Dimensions_t_array(const std::vector<ItemMetadata_Dimensions_t>& arr);
inline std::string serialize_ItemMetadata_Dimensions_t_array(const arena_vector<ItemMetadata_Dimensions_t>& arr);
inline std::string serialize_ItemMetadata_Manufacturer_t_array(const std::vector<ItemMetadata_Manufacturer_t>& arr);
inline std::string serialize_ItemMetadata_Manufacturer_t_array(const arena_vector<ItemMetadata_Manufacturer_t>& arr);
inline std::string serialize_ItemMetadata_Sku_t_array(const std::vector<ItemMetadata_Sku_t>& arr);
inline std::string serialize_ItemMetadata_Sku_t_array(const arena_vector<ItemMetadata_Sku_t>& arr);
inline std::string serialize_CreateItemRequest_array(const std::vector<CreateItemRequest>& arr);
inline std::string serialize_CreateItemRequest_array(const arena_vector<CreateItemRequest>& arr);
inline std::string serialize_CreateItemRequest_Name_t_array(const std::vector<CreateItemRequest_Name_t>& arr);
inline std::string serialize_CreateItemRequest_Name_t_array(const arena_vector<CreateItemRequest_Name_t>& arr);
inline std::string serialize_CreateItemRequest_Description_t_array(const std::vector<CreateItemRequest_Description_t>& arr);
inline std::string serialize_CreateItemRequest_Description_t_array(const arena_vector<CreateItemRequest_Description_t>& arr);
inline std::string serialize_CreateItemRequest_Price_t_array(const std::vector<CreateItemRequest_Price_t>& arr);
inline std::string serialize_CreateItemRequest_Price_t_array(const arena_vector<CreateItemRequest_Price_t>& arr);
inline std::string serialize_CreateItemRequest_Stock_t_array(const std::vector<CreateItemRequest_Stock_t>& arr);
inline std::string serialize_CreateItemRequest_Stock_t_array(const arena_vector<CreateItemRequest_Stock_t>& arr);
inline std::string serialize_CreateItemRequest_Category_t_array(const std::vector<CreateItemRequest_Category_t>& arr);
inline std::string serialize_CreateItemRequest_Category_t_array(const arena_vector<CreateItemRequest_Category_t>& arr);
inline std::string serialize_CreateItemRequest_Tags_t_array(const std::vector<CreateItemRequest_Tags_t>& arr);
inline std::string serialize_CreateItemRequest_Tags_t_array(const arena_vector<CreateItemRequest_Tags_t>& arr);
inline std::string serialize_CreateItemRequest_Item_t_array(const std::vector<CreateItemRequest_Item_t>& arr);
inline std::string serialize_CreateItemRequest_Item_t_array(const arena_vector<CreateItemRequest_Item_t>& arr);
inline std::string serialize_CreateItemRequest_Metadata_t_array(const std::vector<CreateItemRequest_Metadata_t>& arr);
inline std::string serialize_CreateItemRequest_Metadata_t_array(const arena_vector<CreateItemRequest_Metadata_t>& arr);
inline std::string serialize_UpdateItemRequest_array(const std::vector<UpdateItemRequest>& arr);
inline std::string serialize_UpdateItemRequest_array(const arena_vector<UpdateItemRequest>& arr);
inline std::string serialize_UpdateItemRequest_Name_t_array(const std::vector<UpdateItemRequest_Name_t>& arr);
inline std::string serialize_UpdateItemRequest_Name_t_array(const arena_vector<UpdateItemRequest_Name_t>& arr);
inline std::string serialize_UpdateItemRequest_Description_t_array(const std::vector<UpdateItemRequest_Description_t>& arr);
inline std::string serialize_UpdateItemRequest_Description_t_array(const arena_vector<UpdateItemRequest_Description_t>& arr);
inline std::string serialize_UpdateItemRequest_Price_t_array(const std::vector<UpdateItemRequest_Price_t>& arr);
inline std::string serialize_UpdateItemRequest_Price_t_array(const arena_vector<UpdateItemRequest_Price_t>& arr);
inline std::string serialize_UpdateItemRequest_Stock_t_array(const std::vector<UpdateItemRequest_Stock_t>& arr);
inline std::string serialize_UpdateItemRequest_Stock_t_array(const arena_vector<UpdateItemRequest_Stock_t>& arr);
inline std::string serialize_UpdateItemRequest_Category_t_array(const std::vector<UpdateItemRequest_Category_t>& arr);
inline std::string serialize_UpdateItemRequest_Category_t_array(const arena_vector<UpdateItemRequest_Category_t>& arr);
inline std::string serialize_UpdateItemRequest_Tags_t_array(const std::vector<UpdateItemRequest_Tags_t>& arr);
inline std::string serialize_UpdateItemRequest_Tags_t_array(const arena_vector<UpdateItemRequest_Tags_t>& arr);
inline std::string serialize_UpdateItemRequest_Item_t_array(const std::vector<UpdateItemRequest_Item_t>& arr);
inline std::string serialize_UpdateItemRequest_Item_t_array(const arena_vector<UpdateItemRequest_Item_t>& arr);
inline std::string serialize_ItemList_array(const std::vector<ItemList>& arr);
inline std::string serialize_ItemList_array(const arena_vector<ItemList>& arr);
inline std::string serialize_ItemList_Items_t_array(const std::vector<ItemList_Items_t>& arr);
inline std::string serialize_ItemList_Items_t_array(const arena_vector<ItemList_Items_t>& arr);
inline std::string serialize_ItemList_Item_t_array(const std::vector<ItemList_Item_t>& arr);
inline std::string serialize_ItemList_Item_t_array(const arena_vector<ItemList_Item_t>& arr);
inline std::string serialize_ItemList_Total_t_array(const std::vector<ItemList_Total_t>& arr);
inline std::string serialize_ItemList_Total_t_array(const arena_vector<ItemList_Total_t>& arr);
inline std::string serialize_ItemList_Limit_t_array(const std::vector<ItemList_Limit_t>& arr);
inline std::string serialize_ItemList_Limit_t_array(const arena_vector<ItemList_Limit_t>& arr);
inline std::string serialize_ItemList_Offset_t_array(const std::vector<ItemList_Offset_t>& arr);
inline std::string serialize_ItemList_Offset_t_array(const arena_vector<ItemList_Offset_t>& arr);
inline std::string serialize_EchoRequest_array(const std::vector<EchoRequest>& arr);
inline std::string serialize_EchoRequest_array(const arena_vector<EchoRequest>& arr);
inline std::string serialize_EchoRequest_Message_t_array(const std::vector<EchoRequest_Message_t>& arr);
inline std::string serialize_EchoRequest_Message_t_array(const arena_vector<EchoRequest_Message_t>& arr);
inline std::string serialize_EchoRequest_Repeat_t_array(const std::vector<EchoRequest_Repeat_t>& arr);
inline std::string serialize_EchoRequest_Repeat_t_array(const arena_vector<EchoRequest_Repeat_t>& arr);
inline std::string serialize_EchoRequest_Uppercase_t_array(const std::vector<EchoRequest_Uppercase_t>& arr);
inline std::string serialize_EchoRequest_Uppercase_t_array(const arena_vector<EchoRequest_Uppercase_t>& arr);
inline std::string serialize_EchoResponse_array(const std::vector<EchoResponse>& arr);
inline std::string serialize_EchoResponse_array(const arena_vector<EchoResponse>& arr);
inline std::string serialize_EchoResponse_Message_t_array(const std::vector<EchoResponse_Message_t>& arr);
inline std::string serialize_EchoResponse_Message_t_array(const arena_vector<EchoResponse_Message_t>& arr);
inline std::string serialize_EchoResponse_Length_t_array(const std::vector<EchoResponse_Length_t>& arr);
inline std::string serialize_EchoResponse_Length_t_array(const arena_vector<EchoResponse_Length_t>& arr);
inline std::string serialize_schema_array(const std::vector<schema>& arr);
inline std::string serialize_schema_array(const arena_vector<schema>& arr);
inline std::string serialize_schema_1_array(const std::vector<schema_1>& arr);
inline std::string serialize_schema_1_array(const arena_vector<schema_1>& arr);
inline std::string serialize_schema_2_array(const std::vector<schema_2>& arr);
inline std::string serialize_schema_2_array(const arena_vector<schema_2>& arr);
inline std::string serialize_schema_3_array(const std::vector<schema_3>& arr);
inline std::string serialize_schema_3_array(const arena_vector<schema_3>& arr);
inline std::string serialize_schema_4_array(const std::vector<schema_4>& arr);
inline std::string serialize_schema_4_array(const arena_vector<schema_4>& arr);
inline std::string serialize_schema_5_array(const std::vector<schema_5>& arr);
inline std::string serialize_schema_5_array(const arena_vector<schema_5>& arr);
inline std::string serialize_list_items_param_limit_array(const std::vector<list_items_param_limit>& arr);
inline std::string serialize_list_items_param_limit_array(const arena_vector<list_items_param_limit>& arr);
inline std::string serialize_list_items_param_offset_array(const std::vector<list_items_param_offset>& arr);
inline std::string serialize_list_items_param_offset_array(const arena_vector<list_items_param_offset>& arr);
inline std::string serialize_schema_6_array(const std::vector<schema_6>& arr);
inline std::string serialize_schema_6_array(const arena_vector<schema_6>& arr);
inline std::string serialize_schema_7_array(const std::vector<schema_7>& arr);
inline std::string serialize_schema_7_array(const arena_vector<schema_7>& arr);
inline std::string serialize_create_item_param_X_Request_Id_array(const std::vector<create_item_param_X_Request_Id>& arr);
inline std::string serialize_create_item_param_X_Request_Id_array(const arena_vector<create_item_param_X_Request_Id>& arr);
inline std::string serialize_create_item_param_session_array(const std::vector<create_item_param_session>& arr);
inline std::string serialize_create_item_param_session_array(const arena_vector<create_item_param_session>& arr);
inline std::string serialize_schema_8_array(const std::vector<schema_8>& arr);
inline std::string serialize_schema_8_array(const arena_vector<schema_8>& arr);
inline std::string serialize_schema_9_array(const std::vector<schema_9>& arr);
inline std::string serialize_schema_9_array(const arena_vector<schema_9>& arr);
inline std::string serialize_get_item_param_id_array(const std::vector<get_item_param_id>& arr);
inline std::string serialize_get_item_param_id_array(const arena_vector<get_item_param_id>& arr);
inline std::string serialize_schema_10_array(const std::vector<schema_10>& arr);
inline std::string serialize_schema_10_array(const arena_vector<schema_10>& arr);
inline std::string serialize_update_item_param_id_array(const std::vector<update_item_param_id>& arr);
inline std::string serialize_update_item_param_id_array(const arena_vector<update_item_param_id>& arr);
inline std::string serialize_schema_11_array(const std::vector<schema_11>& arr);
inline std::string serialize_schema_11_array(const arena_vector<schema_11>& arr);
inline std::string serialize_schema_12_array(const std::vector<schema_12>& arr);
inline std::string serialize_schema_12_array(const arena_vector<schema_12>& arr);
inline std::string serialize_delete_item_param_id_array(const std::vector<delete_item_param_id>& arr);
inline std::string serialize_delete_item_param_id_array(const arena_vector<delete_item_param_id>& arr);
inline std::string serialize_schema_13_array(const std::vector<schema_13>& arr);
inline std::string serialize_schema_13_array(const arena_vector<schema_13>& arr);
inline std::string serialize_schema_14_array(const std::vector<schema_14>& arr);
inline std::string serialize_schema_14_array(const arena_vector<schema_14>& arr);
inline std::string serialize_health_check_response_array(const std::vector<health_check_response>& arr);
inline std::string serialize_health_check_response_array(const arena_vector<health_check_response>& arr);
inline std::string serialize_health_check_response_Status_array(const std::vector<health_check_response_Status>& arr);
inline std::string serialize_health_check_response_Status_array(const arena_vector<health_check_response_Status>& arr);
inline std::string serialize_health_check_response_Uptime_ms_array(const std::vector<health_check_response_Uptime_ms>& arr);
inline std::string serialize_health_check_response_Uptime_ms_array(const arena_vector<health_check_response_Uptime_ms>& arr);

// ============================================================
// JSON Parse Functions
// ============================================================

[[nodiscard]] inline std::optional<SumRequest> parse_SumRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    SumRequest obj(arena);
    bool has_values = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "values") {
            has_values = true;
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) break;
                    if (auto v = katana::serde::parse_double(cur)) {
                        obj.values.push_back(*v);
                    } else { cur.skip_value(); }
                    cur.try_comma();
                }
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_values) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<SumRequest> parse_SumRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SumRequest(cur, arena);
}

[[nodiscard]] inline std::optional<SumRequest_Values_t> parse_SumRequest_Values_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;
    SumRequest_Values_t result{arena_allocator<SumRequest_Item_t>(arena)};
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto v = katana::serde::parse_double(cur)) {
            result.push_back(*v);
        } else { cur.skip_value(); }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<SumRequest_Values_t> parse_SumRequest_Values_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start()) return std::nullopt;
    SumRequest_Values_t result{arena_allocator<SumRequest_Item_t>(arena)};
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto v = katana::serde::parse_double(cur)) {
            result.push_back(*v);
        } else { cur.skip_value(); }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<SumRequest_Item_t> parse_SumRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur)) return SumRequest_Item_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<SumRequest_Item_t> parse_SumRequest_Item_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SumRequest_Item_t(cur, arena);
}

[[nodiscard]] inline std::optional<SumResponse> parse_SumResponse(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    SumResponse obj(arena);
    bool has_result = false;
    bool has_count = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "result") {
            has_result = true;
            if (auto v = katana::serde::parse_double(cur)) {
                obj.result = *v;
            } else { cur.skip_value(); }
        } else         if (*key == "count") {
            has_count = true;
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.count = *v;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_result) return std::nullopt;
    if (!has_count) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<SumResponse> parse_SumResponse(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SumResponse(cur, arena);
}

[[nodiscard]] inline std::optional<SumResponse_Result_t> parse_SumResponse_Result_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur)) return SumResponse_Result_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<SumResponse_Result_t> parse_SumResponse_Result_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SumResponse_Result_t(cur, arena);
}

[[nodiscard]] inline std::optional<SumResponse_Count_t> parse_SumResponse_Count_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return SumResponse_Count_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<SumResponse_Count_t> parse_SumResponse_Count_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SumResponse_Count_t(cur, arena);
}

[[nodiscard]] inline std::optional<StatsRequest> parse_StatsRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    StatsRequest obj(arena);
    bool has_values = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "values") {
            has_values = true;
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) break;
                    if (auto v = katana::serde::parse_double(cur)) {
                        obj.values.push_back(*v);
                    } else { cur.skip_value(); }
                    cur.try_comma();
                }
            } else { cur.skip_value(); }
        } else         if (*key == "include_median") {
            if (auto v = katana::serde::parse_bool(cur)) {
                obj.include_median = *v;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_values) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<StatsRequest> parse_StatsRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsRequest(cur, arena);
}

[[nodiscard]] inline std::optional<StatsRequest_Values_t> parse_StatsRequest_Values_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;
    StatsRequest_Values_t result{arena_allocator<StatsRequest_Item_t>(arena)};
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto v = katana::serde::parse_double(cur)) {
            result.push_back(*v);
        } else { cur.skip_value(); }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<StatsRequest_Values_t> parse_StatsRequest_Values_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start()) return std::nullopt;
    StatsRequest_Values_t result{arena_allocator<StatsRequest_Item_t>(arena)};
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto v = katana::serde::parse_double(cur)) {
            result.push_back(*v);
        } else { cur.skip_value(); }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<StatsRequest_Item_t> parse_StatsRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur)) return StatsRequest_Item_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<StatsRequest_Item_t> parse_StatsRequest_Item_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsRequest_Item_t(cur, arena);
}

[[nodiscard]] inline std::optional<StatsRequest_Include_median_t> parse_StatsRequest_Include_median_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_bool(cur)) return StatsRequest_Include_median_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<StatsRequest_Include_median_t> parse_StatsRequest_Include_median_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsRequest_Include_median_t(cur, arena);
}

[[nodiscard]] inline std::optional<StatsResponse> parse_StatsResponse(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    StatsResponse obj(arena);

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 3:
            if (*key == "min") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.min = *v;
                } else { cur.skip_value(); }
            } else if (*key == "max") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.max = *v;
                } else { cur.skip_value(); }
            } else if (*key == "sum") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.sum = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 4:
            if (*key == "mean") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.mean = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 5:
            if (*key == "count") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.count = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 6:
            if (*key == "median") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.median = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    return obj;
}

[[nodiscard]] inline std::optional<StatsResponse> parse_StatsResponse(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsResponse(cur, arena);
}

[[nodiscard]] inline std::optional<StatsResponse_Min_t> parse_StatsResponse_Min_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur)) return StatsResponse_Min_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<StatsResponse_Min_t> parse_StatsResponse_Min_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsResponse_Min_t(cur, arena);
}

[[nodiscard]] inline std::optional<StatsResponse_Max_t> parse_StatsResponse_Max_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur)) return StatsResponse_Max_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<StatsResponse_Max_t> parse_StatsResponse_Max_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsResponse_Max_t(cur, arena);
}

[[nodiscard]] inline std::optional<StatsResponse_Mean_t> parse_StatsResponse_Mean_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur)) return StatsResponse_Mean_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<StatsResponse_Mean_t> parse_StatsResponse_Mean_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsResponse_Mean_t(cur, arena);
}

[[nodiscard]] inline std::optional<StatsResponse_Sum_t> parse_StatsResponse_Sum_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur)) return StatsResponse_Sum_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<StatsResponse_Sum_t> parse_StatsResponse_Sum_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsResponse_Sum_t(cur, arena);
}

[[nodiscard]] inline std::optional<StatsResponse_Count_t> parse_StatsResponse_Count_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return StatsResponse_Count_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<StatsResponse_Count_t> parse_StatsResponse_Count_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsResponse_Count_t(cur, arena);
}

[[nodiscard]] inline std::optional<StatsResponse_Median_t> parse_StatsResponse_Median_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur)) return StatsResponse_Median_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<StatsResponse_Median_t> parse_StatsResponse_Median_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsResponse_Median_t(cur, arena);
}

[[nodiscard]] inline std::optional<RegisterRequest> parse_RegisterRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    RegisterRequest obj(arena);
    bool has_username = false;
    bool has_email = false;
    bool has_password = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 3:
            if (*key == "age") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.age = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 4:
            if (*key == "role") {
                if (auto v = cur.string()) {
                    auto enum_val = UserRole_enum_from_string(std::string_view(v->begin(), v->end()));
                    if (enum_val) obj.role = *enum_val;
                } else { cur.skip_value(); }
            } else if (*key == "tags") {
                if (cur.try_array_start()) {
                    obj.tags.emplace(arena);
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end()) break;
                        if (auto v = cur.string()) {
                            (*obj.tags).emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
                        } else { cur.skip_value(); }
                        cur.try_comma();
                    }
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 5:
            if (*key == "email") {
                has_email = true;
                if (auto v = cur.string()) {
                    obj.email = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "username") {
                has_username = true;
                if (auto v = cur.string()) {
                    obj.username = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else if (*key == "password") {
                has_password = true;
                if (auto v = cur.string()) {
                    obj.password = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!has_username) return std::nullopt;
    if (!has_email) return std::nullopt;
    if (!has_password) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<RegisterRequest> parse_RegisterRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterRequest(cur, arena);
}

[[nodiscard]] inline std::optional<RegisterRequest_Username_t> parse_RegisterRequest_Username_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return RegisterRequest_Username_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<RegisterRequest_Username_t> parse_RegisterRequest_Username_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterRequest_Username_t(cur, arena);
}

[[nodiscard]] inline std::optional<RegisterRequest_Email_t> parse_RegisterRequest_Email_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return RegisterRequest_Email_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<RegisterRequest_Email_t> parse_RegisterRequest_Email_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterRequest_Email_t(cur, arena);
}

[[nodiscard]] inline std::optional<RegisterRequest_Password_t> parse_RegisterRequest_Password_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return RegisterRequest_Password_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<RegisterRequest_Password_t> parse_RegisterRequest_Password_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterRequest_Password_t(cur, arena);
}

[[nodiscard]] inline std::optional<RegisterRequest_Age_t> parse_RegisterRequest_Age_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return RegisterRequest_Age_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<RegisterRequest_Age_t> parse_RegisterRequest_Age_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterRequest_Age_t(cur, arena);
}

[[nodiscard]] inline std::optional<RegisterRequest_Role_t> parse_RegisterRequest_Role_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
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
    return RegisterRequest_Role_t{};
}

[[nodiscard]] inline std::optional<RegisterRequest_Role_t> parse_RegisterRequest_Role_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterRequest_Role_t(cur, arena);
}

[[nodiscard]] inline std::optional<RegisterRequest_Tags_t> parse_RegisterRequest_Tags_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;
    RegisterRequest_Tags_t result{arena_allocator<RegisterRequest_Item_t>(arena)};
    size_t reserve_hint = 0;
    for (const char* p = cur.ptr; p < cur.end; ++p) {
        if (*p == ',') ++reserve_hint;
    }
    if (cur.ptr < cur.end && *cur.ptr != ']') ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto v = cur.string()) {
            result.emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
        } else { cur.skip_value(); }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<RegisterRequest_Tags_t> parse_RegisterRequest_Tags_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start()) return std::nullopt;
    RegisterRequest_Tags_t result{arena_allocator<RegisterRequest_Item_t>(arena)};
    size_t reserve_hint = 0;
    for (char ch : json) {
        if (ch == ',') ++reserve_hint;
    }
    if (!json.empty() && json != "[]") ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto v = cur.string()) {
            result.emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
        } else { cur.skip_value(); }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<RegisterRequest_Item_t> parse_RegisterRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return RegisterRequest_Item_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<RegisterRequest_Item_t> parse_RegisterRequest_Item_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterRequest_Item_t(cur, arena);
}

[[nodiscard]] inline std::optional<UserRole> parse_UserRole(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = cur.string()) {
        return UserRole_enum_from_string(std::string_view(v->begin(), v->end()));
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UserRole> parse_UserRole(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserRole(cur, arena);
}

[[nodiscard]] inline std::optional<UserResponse> parse_UserResponse(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    UserResponse obj(arena);
    bool has_id = false;
    bool has_username = false;
    bool has_email = false;
    bool has_role = false;
    bool has_created_at = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 2:
            if (*key == "id") {
                has_id = true;
                if (auto v = cur.string()) {
                    obj.id = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 4:
            if (*key == "role") {
                has_role = true;
                if (auto v = cur.string()) {
                    auto enum_val = UserRole_enum_from_string(std::string_view(v->begin(), v->end()));
                    if (enum_val) obj.role = *enum_val;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 5:
            if (*key == "email") {
                has_email = true;
                if (auto v = cur.string()) {
                    obj.email = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "username") {
                has_username = true;
                if (auto v = cur.string()) {
                    obj.username = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 10:
            if (*key == "created_at") {
                has_created_at = true;
                if (auto v = cur.string()) {
                    obj.created_at = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!has_id) return std::nullopt;
    if (!has_username) return std::nullopt;
    if (!has_email) return std::nullopt;
    if (!has_role) return std::nullopt;
    if (!has_created_at) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<UserResponse> parse_UserResponse(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserResponse(cur, arena);
}

[[nodiscard]] inline std::optional<UserResponse_Id_t> parse_UserResponse_Id_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return UserResponse_Id_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UserResponse_Id_t> parse_UserResponse_Id_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserResponse_Id_t(cur, arena);
}

[[nodiscard]] inline std::optional<UserResponse_Username_t> parse_UserResponse_Username_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return UserResponse_Username_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UserResponse_Username_t> parse_UserResponse_Username_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserResponse_Username_t(cur, arena);
}

[[nodiscard]] inline std::optional<UserResponse_Email_t> parse_UserResponse_Email_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return UserResponse_Email_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UserResponse_Email_t> parse_UserResponse_Email_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserResponse_Email_t(cur, arena);
}

[[nodiscard]] inline std::optional<UserResponse_Role_t> parse_UserResponse_Role_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
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
    return UserResponse_Role_t{};
}

[[nodiscard]] inline std::optional<UserResponse_Role_t> parse_UserResponse_Role_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserResponse_Role_t(cur, arena);
}

[[nodiscard]] inline std::optional<UserResponse_Created_at_t> parse_UserResponse_Created_at_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return UserResponse_Created_at_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UserResponse_Created_at_t> parse_UserResponse_Created_at_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserResponse_Created_at_t(cur, arena);
}

[[nodiscard]] inline std::optional<ItemCategory> parse_ItemCategory(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = cur.string()) {
        return ItemCategory_enum_from_string(std::string_view(v->begin(), v->end()));
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<ItemCategory> parse_ItemCategory(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemCategory(cur, arena);
}

[[nodiscard]] inline std::optional<Item> parse_Item(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    Item obj(arena);
    bool has_id = false;
    bool has_name = false;
    bool has_price = false;
    bool has_category = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 2:
            if (*key == "id") {
                has_id = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.id = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 4:
            if (*key == "name") {
                has_name = true;
                if (auto v = cur.string()) {
                    obj.name = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else if (*key == "tags") {
                if (cur.try_array_start()) {
                    obj.tags.emplace(arena);
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end()) break;
                        if (auto v = cur.string()) {
                            (*obj.tags).emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
                        } else { cur.skip_value(); }
                        cur.try_comma();
                    }
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 5:
            if (*key == "price") {
                has_price = true;
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.price = *v;
                } else { cur.skip_value(); }
            } else if (*key == "stock") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.stock = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "category") {
                has_category = true;
                if (auto v = cur.string()) {
                    auto enum_val = ItemCategory_enum_from_string(std::string_view(v->begin(), v->end()));
                    if (enum_val) obj.category = *enum_val;
                } else { cur.skip_value(); }
            } else if (*key == "metadata") {
                if (auto nested = parse_ItemMetadata(cur, arena)) {
                    obj.metadata = std::move(*nested);
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 11:
            if (*key == "description") {
                if (auto v = cur.string()) {
                    obj.description = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!has_id) return std::nullopt;
    if (!has_name) return std::nullopt;
    if (!has_price) return std::nullopt;
    if (!has_category) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<Item> parse_Item(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item(cur, arena);
}

[[nodiscard]] inline std::optional<Item_Id_t> parse_Item_Id_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return Item_Id_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<Item_Id_t> parse_Item_Id_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_Id_t(cur, arena);
}

[[nodiscard]] inline std::optional<Item_Name_t> parse_Item_Name_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return Item_Name_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<Item_Name_t> parse_Item_Name_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_Name_t(cur, arena);
}

[[nodiscard]] inline std::optional<Item_Description_t> parse_Item_Description_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return Item_Description_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<Item_Description_t> parse_Item_Description_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_Description_t(cur, arena);
}

[[nodiscard]] inline std::optional<Item_Price_t> parse_Item_Price_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur)) return Item_Price_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<Item_Price_t> parse_Item_Price_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_Price_t(cur, arena);
}

[[nodiscard]] inline std::optional<Item_Stock_t> parse_Item_Stock_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return Item_Stock_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<Item_Stock_t> parse_Item_Stock_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_Stock_t(cur, arena);
}

[[nodiscard]] inline std::optional<Item_Category_t> parse_Item_Category_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
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
    return Item_Category_t{};
}

[[nodiscard]] inline std::optional<Item_Category_t> parse_Item_Category_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_Category_t(cur, arena);
}

[[nodiscard]] inline std::optional<Item_Tags_t> parse_Item_Tags_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;
    Item_Tags_t result{arena_allocator<Item_Item_t>(arena)};
    size_t reserve_hint = 0;
    for (const char* p = cur.ptr; p < cur.end; ++p) {
        if (*p == ',') ++reserve_hint;
    }
    if (cur.ptr < cur.end && *cur.ptr != ']') ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto v = cur.string()) {
            result.emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
        } else { cur.skip_value(); }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<Item_Tags_t> parse_Item_Tags_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start()) return std::nullopt;
    Item_Tags_t result{arena_allocator<Item_Item_t>(arena)};
    size_t reserve_hint = 0;
    for (char ch : json) {
        if (ch == ',') ++reserve_hint;
    }
    if (!json.empty() && json != "[]") ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto v = cur.string()) {
            result.emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
        } else { cur.skip_value(); }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<Item_Item_t> parse_Item_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return Item_Item_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<Item_Item_t> parse_Item_Item_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_Item_t(cur, arena);
}

[[nodiscard]] inline std::optional<Item_Metadata_t> parse_Item_Metadata_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
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
    return Item_Metadata_t{};
}

[[nodiscard]] inline std::optional<Item_Metadata_t> parse_Item_Metadata_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_Metadata_t(cur, arena);
}

[[nodiscard]] inline std::optional<ItemMetadata> parse_ItemMetadata(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    ItemMetadata obj(arena);

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 3:
            if (*key == "sku") {
                if (auto v = cur.string()) {
                    obj.sku = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 9:
            if (*key == "weight_kg") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.weight_kg = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 10:
            if (*key == "dimensions") {
                if (auto v = cur.string()) {
                    obj.dimensions = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 12:
            if (*key == "manufacturer") {
                if (auto v = cur.string()) {
                    obj.manufacturer = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    return obj;
}

[[nodiscard]] inline std::optional<ItemMetadata> parse_ItemMetadata(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemMetadata(cur, arena);
}

[[nodiscard]] inline std::optional<ItemMetadata_Weight_kg_t> parse_ItemMetadata_Weight_kg_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur)) return ItemMetadata_Weight_kg_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<ItemMetadata_Weight_kg_t> parse_ItemMetadata_Weight_kg_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemMetadata_Weight_kg_t(cur, arena);
}

[[nodiscard]] inline std::optional<ItemMetadata_Dimensions_t> parse_ItemMetadata_Dimensions_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return ItemMetadata_Dimensions_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<ItemMetadata_Dimensions_t> parse_ItemMetadata_Dimensions_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemMetadata_Dimensions_t(cur, arena);
}

[[nodiscard]] inline std::optional<ItemMetadata_Manufacturer_t> parse_ItemMetadata_Manufacturer_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return ItemMetadata_Manufacturer_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<ItemMetadata_Manufacturer_t> parse_ItemMetadata_Manufacturer_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemMetadata_Manufacturer_t(cur, arena);
}

[[nodiscard]] inline std::optional<ItemMetadata_Sku_t> parse_ItemMetadata_Sku_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return ItemMetadata_Sku_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<ItemMetadata_Sku_t> parse_ItemMetadata_Sku_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemMetadata_Sku_t(cur, arena);
}

[[nodiscard]] inline std::optional<CreateItemRequest> parse_CreateItemRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    CreateItemRequest obj(arena);
    bool has_name = false;
    bool has_price = false;
    bool has_category = false;

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
            } else if (*key == "tags") {
                if (cur.try_array_start()) {
                    obj.tags.emplace(arena);
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end()) break;
                        if (auto v = cur.string()) {
                            (*obj.tags).emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
                        } else { cur.skip_value(); }
                        cur.try_comma();
                    }
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 5:
            if (*key == "price") {
                has_price = true;
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.price = *v;
                } else { cur.skip_value(); }
            } else if (*key == "stock") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.stock = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "category") {
                has_category = true;
                if (auto v = cur.string()) {
                    auto enum_val = ItemCategory_enum_from_string(std::string_view(v->begin(), v->end()));
                    if (enum_val) obj.category = *enum_val;
                } else { cur.skip_value(); }
            } else if (*key == "metadata") {
                if (auto nested = parse_ItemMetadata(cur, arena)) {
                    obj.metadata = std::move(*nested);
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 11:
            if (*key == "description") {
                if (auto v = cur.string()) {
                    obj.description = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!has_name) return std::nullopt;
    if (!has_price) return std::nullopt;
    if (!has_category) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<CreateItemRequest> parse_CreateItemRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateItemRequest(cur, arena);
}

[[nodiscard]] inline std::optional<CreateItemRequest_Name_t> parse_CreateItemRequest_Name_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return CreateItemRequest_Name_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CreateItemRequest_Name_t> parse_CreateItemRequest_Name_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateItemRequest_Name_t(cur, arena);
}

[[nodiscard]] inline std::optional<CreateItemRequest_Description_t> parse_CreateItemRequest_Description_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return CreateItemRequest_Description_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CreateItemRequest_Description_t> parse_CreateItemRequest_Description_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateItemRequest_Description_t(cur, arena);
}

[[nodiscard]] inline std::optional<CreateItemRequest_Price_t> parse_CreateItemRequest_Price_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur)) return CreateItemRequest_Price_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CreateItemRequest_Price_t> parse_CreateItemRequest_Price_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateItemRequest_Price_t(cur, arena);
}

[[nodiscard]] inline std::optional<CreateItemRequest_Stock_t> parse_CreateItemRequest_Stock_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return CreateItemRequest_Stock_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CreateItemRequest_Stock_t> parse_CreateItemRequest_Stock_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateItemRequest_Stock_t(cur, arena);
}

[[nodiscard]] inline std::optional<CreateItemRequest_Category_t> parse_CreateItemRequest_Category_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
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
    return CreateItemRequest_Category_t{};
}

[[nodiscard]] inline std::optional<CreateItemRequest_Category_t> parse_CreateItemRequest_Category_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateItemRequest_Category_t(cur, arena);
}

[[nodiscard]] inline std::optional<CreateItemRequest_Tags_t> parse_CreateItemRequest_Tags_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;
    CreateItemRequest_Tags_t result{arena_allocator<CreateItemRequest_Item_t>(arena)};
    size_t reserve_hint = 0;
    for (const char* p = cur.ptr; p < cur.end; ++p) {
        if (*p == ',') ++reserve_hint;
    }
    if (cur.ptr < cur.end && *cur.ptr != ']') ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto v = cur.string()) {
            result.emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
        } else { cur.skip_value(); }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<CreateItemRequest_Tags_t> parse_CreateItemRequest_Tags_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start()) return std::nullopt;
    CreateItemRequest_Tags_t result{arena_allocator<CreateItemRequest_Item_t>(arena)};
    size_t reserve_hint = 0;
    for (char ch : json) {
        if (ch == ',') ++reserve_hint;
    }
    if (!json.empty() && json != "[]") ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto v = cur.string()) {
            result.emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
        } else { cur.skip_value(); }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<CreateItemRequest_Item_t> parse_CreateItemRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return CreateItemRequest_Item_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CreateItemRequest_Item_t> parse_CreateItemRequest_Item_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateItemRequest_Item_t(cur, arena);
}

[[nodiscard]] inline std::optional<CreateItemRequest_Metadata_t> parse_CreateItemRequest_Metadata_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
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
    return CreateItemRequest_Metadata_t{};
}

[[nodiscard]] inline std::optional<CreateItemRequest_Metadata_t> parse_CreateItemRequest_Metadata_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateItemRequest_Metadata_t(cur, arena);
}

[[nodiscard]] inline std::optional<UpdateItemRequest> parse_UpdateItemRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    UpdateItemRequest obj(arena);

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 4:
            if (*key == "name") {
                if (auto v = cur.string()) {
                    obj.name = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else if (*key == "tags") {
                if (cur.try_array_start()) {
                    obj.tags.emplace(arena);
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end()) break;
                        if (auto v = cur.string()) {
                            (*obj.tags).emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
                        } else { cur.skip_value(); }
                        cur.try_comma();
                    }
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 5:
            if (*key == "price") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.price = *v;
                } else { cur.skip_value(); }
            } else if (*key == "stock") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.stock = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "category") {
                if (auto v = cur.string()) {
                    auto enum_val = ItemCategory_enum_from_string(std::string_view(v->begin(), v->end()));
                    if (enum_val) obj.category = *enum_val;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 11:
            if (*key == "description") {
                if (auto v = cur.string()) {
                    obj.description = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    return obj;
}

[[nodiscard]] inline std::optional<UpdateItemRequest> parse_UpdateItemRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateItemRequest(cur, arena);
}

[[nodiscard]] inline std::optional<UpdateItemRequest_Name_t> parse_UpdateItemRequest_Name_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return UpdateItemRequest_Name_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UpdateItemRequest_Name_t> parse_UpdateItemRequest_Name_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateItemRequest_Name_t(cur, arena);
}

[[nodiscard]] inline std::optional<UpdateItemRequest_Description_t> parse_UpdateItemRequest_Description_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return UpdateItemRequest_Description_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UpdateItemRequest_Description_t> parse_UpdateItemRequest_Description_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateItemRequest_Description_t(cur, arena);
}

[[nodiscard]] inline std::optional<UpdateItemRequest_Price_t> parse_UpdateItemRequest_Price_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_double(cur)) return UpdateItemRequest_Price_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UpdateItemRequest_Price_t> parse_UpdateItemRequest_Price_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateItemRequest_Price_t(cur, arena);
}

[[nodiscard]] inline std::optional<UpdateItemRequest_Stock_t> parse_UpdateItemRequest_Stock_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return UpdateItemRequest_Stock_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UpdateItemRequest_Stock_t> parse_UpdateItemRequest_Stock_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateItemRequest_Stock_t(cur, arena);
}

[[nodiscard]] inline std::optional<UpdateItemRequest_Category_t> parse_UpdateItemRequest_Category_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
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
    return UpdateItemRequest_Category_t{};
}

[[nodiscard]] inline std::optional<UpdateItemRequest_Category_t> parse_UpdateItemRequest_Category_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateItemRequest_Category_t(cur, arena);
}

[[nodiscard]] inline std::optional<UpdateItemRequest_Tags_t> parse_UpdateItemRequest_Tags_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;
    UpdateItemRequest_Tags_t result{arena_allocator<UpdateItemRequest_Item_t>(arena)};
    size_t reserve_hint = 0;
    for (const char* p = cur.ptr; p < cur.end; ++p) {
        if (*p == ',') ++reserve_hint;
    }
    if (cur.ptr < cur.end && *cur.ptr != ']') ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto v = cur.string()) {
            result.emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
        } else { cur.skip_value(); }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<UpdateItemRequest_Tags_t> parse_UpdateItemRequest_Tags_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start()) return std::nullopt;
    UpdateItemRequest_Tags_t result{arena_allocator<UpdateItemRequest_Item_t>(arena)};
    size_t reserve_hint = 0;
    for (char ch : json) {
        if (ch == ',') ++reserve_hint;
    }
    if (!json.empty() && json != "[]") ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto v = cur.string()) {
            result.emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
        } else { cur.skip_value(); }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<UpdateItemRequest_Item_t> parse_UpdateItemRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return UpdateItemRequest_Item_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UpdateItemRequest_Item_t> parse_UpdateItemRequest_Item_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateItemRequest_Item_t(cur, arena);
}

[[nodiscard]] inline std::optional<ItemList> parse_ItemList(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    ItemList obj(arena);
    bool has_items = false;
    bool has_total = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 5:
            if (*key == "items") {
                has_items = true;
                if (cur.try_array_start()) {
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end()) break;
                        if (auto nested = parse_Item(cur, arena)) { obj.items.push_back(std::move(*nested)); }
                        else { cur.skip_value(); }
                        cur.try_comma();
                    }
                } else { cur.skip_value(); }
            } else if (*key == "total") {
                has_total = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.total = *v;
                } else { cur.skip_value(); }
            } else if (*key == "limit") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.limit = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 6:
            if (*key == "offset") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.offset = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!has_items) return std::nullopt;
    if (!has_total) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<ItemList> parse_ItemList(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemList(cur, arena);
}

[[nodiscard]] inline std::optional<ItemList_Items_t> parse_ItemList_Items_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;
    ItemList_Items_t result{arena_allocator<Item>(arena)};
    size_t reserve_hint = 0;
    for (const char* p = cur.ptr; p < cur.end; ++p) {
        if (*p == ',') ++reserve_hint;
    }
    if (cur.ptr < cur.end && *cur.ptr != ']') ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto parsed = parse_Item(cur, arena)) result.push_back(std::move(*parsed));
        else cur.skip_value();
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<ItemList_Items_t> parse_ItemList_Items_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start()) return std::nullopt;
    ItemList_Items_t result{arena_allocator<Item>(arena)};
    size_t reserve_hint = 0;
    for (char ch : json) {
        if (ch == ',') ++reserve_hint;
    }
    if (!json.empty() && json != "[]") ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        if (auto parsed = parse_Item(cur, arena)) result.push_back(std::move(*parsed));
        else cur.skip_value();
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<ItemList_Item_t> parse_ItemList_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
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
    return ItemList_Item_t{};
}

[[nodiscard]] inline std::optional<ItemList_Item_t> parse_ItemList_Item_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemList_Item_t(cur, arena);
}

[[nodiscard]] inline std::optional<ItemList_Total_t> parse_ItemList_Total_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return ItemList_Total_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<ItemList_Total_t> parse_ItemList_Total_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemList_Total_t(cur, arena);
}

[[nodiscard]] inline std::optional<ItemList_Limit_t> parse_ItemList_Limit_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return ItemList_Limit_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<ItemList_Limit_t> parse_ItemList_Limit_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemList_Limit_t(cur, arena);
}

[[nodiscard]] inline std::optional<ItemList_Offset_t> parse_ItemList_Offset_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return ItemList_Offset_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<ItemList_Offset_t> parse_ItemList_Offset_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemList_Offset_t(cur, arena);
}

[[nodiscard]] inline std::optional<EchoRequest> parse_EchoRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    EchoRequest obj(arena);
    bool has_message = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "message") {
            has_message = true;
            if (auto v = cur.string()) {
                obj.message = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else         if (*key == "repeat") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.repeat = *v;
            } else { cur.skip_value(); }
        } else         if (*key == "uppercase") {
            if (auto v = katana::serde::parse_bool(cur)) {
                obj.uppercase = *v;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_message) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<EchoRequest> parse_EchoRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_EchoRequest(cur, arena);
}

[[nodiscard]] inline std::optional<EchoRequest_Message_t> parse_EchoRequest_Message_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return EchoRequest_Message_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<EchoRequest_Message_t> parse_EchoRequest_Message_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_EchoRequest_Message_t(cur, arena);
}

[[nodiscard]] inline std::optional<EchoRequest_Repeat_t> parse_EchoRequest_Repeat_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return EchoRequest_Repeat_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<EchoRequest_Repeat_t> parse_EchoRequest_Repeat_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_EchoRequest_Repeat_t(cur, arena);
}

[[nodiscard]] inline std::optional<EchoRequest_Uppercase_t> parse_EchoRequest_Uppercase_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_bool(cur)) return EchoRequest_Uppercase_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<EchoRequest_Uppercase_t> parse_EchoRequest_Uppercase_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_EchoRequest_Uppercase_t(cur, arena);
}

[[nodiscard]] inline std::optional<EchoResponse> parse_EchoResponse(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    EchoResponse obj(arena);
    bool has_message = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "message") {
            has_message = true;
            if (auto v = cur.string()) {
                obj.message = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else         if (*key == "length") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.length = *v;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_message) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<EchoResponse> parse_EchoResponse(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_EchoResponse(cur, arena);
}

[[nodiscard]] inline std::optional<EchoResponse_Message_t> parse_EchoResponse_Message_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return EchoResponse_Message_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<EchoResponse_Message_t> parse_EchoResponse_Message_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_EchoResponse_Message_t(cur, arena);
}

[[nodiscard]] inline std::optional<EchoResponse_Length_t> parse_EchoResponse_Length_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return EchoResponse_Length_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<EchoResponse_Length_t> parse_EchoResponse_Length_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_EchoResponse_Length_t(cur, arena);
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

[[nodiscard]] inline std::optional<list_items_param_limit> parse_list_items_param_limit(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return list_items_param_limit{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<list_items_param_limit> parse_list_items_param_limit(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_list_items_param_limit(cur, arena);
}

[[nodiscard]] inline std::optional<list_items_param_offset> parse_list_items_param_offset(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return list_items_param_offset{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<list_items_param_offset> parse_list_items_param_offset(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_list_items_param_offset(cur, arena);
}

[[nodiscard]] inline std::optional<schema_6> parse_schema_6(katana::serde::json_cursor& cur, monotonic_arena* arena) {
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
    return schema_6{};
}

[[nodiscard]] inline std::optional<schema_6> parse_schema_6(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_6(cur, arena);
}

[[nodiscard]] inline std::optional<schema_7> parse_schema_7(katana::serde::json_cursor& cur, monotonic_arena* arena) {
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
    return schema_7{};
}

[[nodiscard]] inline std::optional<schema_7> parse_schema_7(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_7(cur, arena);
}

[[nodiscard]] inline std::optional<create_item_param_X_Request_Id> parse_create_item_param_X_Request_Id(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return create_item_param_X_Request_Id{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<create_item_param_X_Request_Id> parse_create_item_param_X_Request_Id(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_create_item_param_X_Request_Id(cur, arena);
}

[[nodiscard]] inline std::optional<create_item_param_session> parse_create_item_param_session(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return create_item_param_session{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<create_item_param_session> parse_create_item_param_session(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_create_item_param_session(cur, arena);
}

[[nodiscard]] inline std::optional<schema_8> parse_schema_8(katana::serde::json_cursor& cur, monotonic_arena* arena) {
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
    return schema_8{};
}

[[nodiscard]] inline std::optional<schema_8> parse_schema_8(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_8(cur, arena);
}

[[nodiscard]] inline std::optional<schema_9> parse_schema_9(katana::serde::json_cursor& cur, monotonic_arena* arena) {
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
    return schema_9{};
}

[[nodiscard]] inline std::optional<schema_9> parse_schema_9(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_9(cur, arena);
}

[[nodiscard]] inline std::optional<get_item_param_id> parse_get_item_param_id(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return get_item_param_id{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<get_item_param_id> parse_get_item_param_id(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_get_item_param_id(cur, arena);
}

[[nodiscard]] inline std::optional<schema_10> parse_schema_10(katana::serde::json_cursor& cur, monotonic_arena* arena) {
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
    return schema_10{};
}

[[nodiscard]] inline std::optional<schema_10> parse_schema_10(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_10(cur, arena);
}

[[nodiscard]] inline std::optional<update_item_param_id> parse_update_item_param_id(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return update_item_param_id{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<update_item_param_id> parse_update_item_param_id(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_update_item_param_id(cur, arena);
}

[[nodiscard]] inline std::optional<schema_11> parse_schema_11(katana::serde::json_cursor& cur, monotonic_arena* arena) {
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
    return schema_11{};
}

[[nodiscard]] inline std::optional<schema_11> parse_schema_11(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_11(cur, arena);
}

[[nodiscard]] inline std::optional<schema_12> parse_schema_12(katana::serde::json_cursor& cur, monotonic_arena* arena) {
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
    return schema_12{};
}

[[nodiscard]] inline std::optional<schema_12> parse_schema_12(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_12(cur, arena);
}

[[nodiscard]] inline std::optional<delete_item_param_id> parse_delete_item_param_id(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return delete_item_param_id{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<delete_item_param_id> parse_delete_item_param_id(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_delete_item_param_id(cur, arena);
}

[[nodiscard]] inline std::optional<schema_13> parse_schema_13(katana::serde::json_cursor& cur, monotonic_arena* arena) {
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
    return schema_13{};
}

[[nodiscard]] inline std::optional<schema_13> parse_schema_13(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_13(cur, arena);
}

[[nodiscard]] inline std::optional<schema_14> parse_schema_14(katana::serde::json_cursor& cur, monotonic_arena* arena) {
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
    return schema_14{};
}

[[nodiscard]] inline std::optional<schema_14> parse_schema_14(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_14(cur, arena);
}

[[nodiscard]] inline std::optional<health_check_response> parse_health_check_response(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    health_check_response obj(arena);

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "status") {
            if (auto v = cur.string()) {
                obj.status = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else         if (*key == "uptime_ms") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.uptime_ms = *v;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return obj;
}

[[nodiscard]] inline std::optional<health_check_response> parse_health_check_response(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_health_check_response(cur, arena);
}

[[nodiscard]] inline std::optional<health_check_response_Status> parse_health_check_response_Status(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return health_check_response_Status{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<health_check_response_Status> parse_health_check_response_Status(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_health_check_response_Status(cur, arena);
}

[[nodiscard]] inline std::optional<health_check_response_Uptime_ms> parse_health_check_response_Uptime_ms(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return health_check_response_Uptime_ms{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<health_check_response_Uptime_ms> parse_health_check_response_Uptime_ms(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_health_check_response_Uptime_ms(cur, arena);
}

// ============================================================
// JSON Serialize Functions
// ============================================================

inline void serialize_SumRequest_into(const SumRequest& obj, std::string& json) {
    json.push_back('{');
    json.append("\"values\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.values.size(); ++i) {
        if (i > 0) json.push_back(',');
        {
            char buf[64];
            auto res = std::to_chars(buf, buf + sizeof(buf), obj.values[i]);
            if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
        }
    }
    json.push_back(']');
    json.push_back('}');
}

inline std::string serialize_SumRequest(const SumRequest& obj) {
    std::string json;
    size_t reserve_estimate = 76;
    reserve_estimate += obj.values.size() * 25;
    json.reserve(reserve_estimate);
    serialize_SumRequest_into(obj, json);
    return json;
}

inline void serialize_SumRequest_Values_t_into(const SumRequest_Values_t& obj, std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_SumRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SumRequest_Values_t(const SumRequest_Values_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_SumRequest_Values_t_into(obj, json);
    return json;
}

inline void serialize_SumRequest_Item_t_into(const SumRequest_Item_t& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_SumRequest_Item_t(const SumRequest_Item_t& obj) {
    std::string json;
    serialize_SumRequest_Item_t_into(obj, json);
    return json;
}

inline void serialize_SumResponse_into(const SumResponse& obj, std::string& json) {
    json.push_back('{');
    json.append("\"result\":");
    {
        char buf[64];
        auto res = std::to_chars(buf, buf + sizeof(buf), obj.result);
        if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
    }
    json.append(",\"count\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.count);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.push_back('}');
}

inline std::string serialize_SumResponse(const SumResponse& obj) {
    std::string json;
    size_t reserve_estimate = 66;
    json.reserve(reserve_estimate);
    serialize_SumResponse_into(obj, json);
    return json;
}

inline void serialize_SumResponse_Result_t_into(const SumResponse_Result_t& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_SumResponse_Result_t(const SumResponse_Result_t& obj) {
    std::string json;
    serialize_SumResponse_Result_t_into(obj, json);
    return json;
}

inline void serialize_SumResponse_Count_t_into(const SumResponse_Count_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_SumResponse_Count_t(const SumResponse_Count_t& obj) {
    std::string json;
    serialize_SumResponse_Count_t_into(obj, json);
    return json;
}

inline void serialize_StatsRequest_into(const StatsRequest& obj, std::string& json) {
    json.push_back('{');
    json.append("\"values\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.values.size(); ++i) {
        if (i > 0) json.push_back(',');
        {
            char buf[64];
            auto res = std::to_chars(buf, buf + sizeof(buf), obj.values[i]);
            if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
        }
    }
    json.push_back(']');
    json.append(",\"include_median\":");
    if (!obj.include_median) {
        json.append("null");
    } else {
        json.append(*obj.include_median ? "true" : "false");
    }
    json.push_back('}');
}

inline std::string serialize_StatsRequest(const StatsRequest& obj) {
    std::string json;
    size_t reserve_estimate = 99;
    reserve_estimate += obj.values.size() * 25;
    json.reserve(reserve_estimate);
    serialize_StatsRequest_into(obj, json);
    return json;
}

inline void serialize_StatsRequest_Values_t_into(const StatsRequest_Values_t& obj, std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsRequest_Values_t(const StatsRequest_Values_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_StatsRequest_Values_t_into(obj, json);
    return json;
}

inline void serialize_StatsRequest_Item_t_into(const StatsRequest_Item_t& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_StatsRequest_Item_t(const StatsRequest_Item_t& obj) {
    std::string json;
    serialize_StatsRequest_Item_t_into(obj, json);
    return json;
}

inline void serialize_StatsRequest_Include_median_t_into(const StatsRequest_Include_median_t& obj, std::string& json) {
    json.append(obj ? "true" : "false");
}

inline std::string serialize_StatsRequest_Include_median_t(const StatsRequest_Include_median_t& obj) {
    return obj ? "true" : "false";
}

inline void serialize_StatsResponse_into(const StatsResponse& obj, std::string& json) {
    json.push_back('{');
    json.append("\"min\":");
    {
        if (!obj.min) {
            json.append("null");
        } else {
        char buf[64];
        auto res = std::to_chars(buf, buf + sizeof(buf), *obj.min);
        if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
        }
    }
    json.append(",\"max\":");
    {
        if (!obj.max) {
            json.append("null");
        } else {
        char buf[64];
        auto res = std::to_chars(buf, buf + sizeof(buf), *obj.max);
        if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
        }
    }
    json.append(",\"mean\":");
    {
        if (!obj.mean) {
            json.append("null");
        } else {
        char buf[64];
        auto res = std::to_chars(buf, buf + sizeof(buf), *obj.mean);
        if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
        }
    }
    json.append(",\"sum\":");
    {
        if (!obj.sum) {
            json.append("null");
        } else {
        char buf[64];
        auto res = std::to_chars(buf, buf + sizeof(buf), *obj.sum);
        if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
        }
    }
    json.append(",\"count\":");
    {
        if (!obj.count) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.count);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    json.append(",\"median\":");
    {
        if (!obj.median) {
            json.append("null");
        } else {
        char buf[64];
        auto res = std::to_chars(buf, buf + sizeof(buf), *obj.median);
        if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
        }
    }
    json.push_back('}');
}

inline std::string serialize_StatsResponse(const StatsResponse& obj) {
    std::string json;
    size_t reserve_estimate = 195;
    json.reserve(reserve_estimate);
    serialize_StatsResponse_into(obj, json);
    return json;
}

inline void serialize_StatsResponse_Min_t_into(const StatsResponse_Min_t& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_StatsResponse_Min_t(const StatsResponse_Min_t& obj) {
    std::string json;
    serialize_StatsResponse_Min_t_into(obj, json);
    return json;
}

inline void serialize_StatsResponse_Max_t_into(const StatsResponse_Max_t& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_StatsResponse_Max_t(const StatsResponse_Max_t& obj) {
    std::string json;
    serialize_StatsResponse_Max_t_into(obj, json);
    return json;
}

inline void serialize_StatsResponse_Mean_t_into(const StatsResponse_Mean_t& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_StatsResponse_Mean_t(const StatsResponse_Mean_t& obj) {
    std::string json;
    serialize_StatsResponse_Mean_t_into(obj, json);
    return json;
}

inline void serialize_StatsResponse_Sum_t_into(const StatsResponse_Sum_t& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_StatsResponse_Sum_t(const StatsResponse_Sum_t& obj) {
    std::string json;
    serialize_StatsResponse_Sum_t_into(obj, json);
    return json;
}

inline void serialize_StatsResponse_Count_t_into(const StatsResponse_Count_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_StatsResponse_Count_t(const StatsResponse_Count_t& obj) {
    std::string json;
    serialize_StatsResponse_Count_t_into(obj, json);
    return json;
}

inline void serialize_StatsResponse_Median_t_into(const StatsResponse_Median_t& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_StatsResponse_Median_t(const StatsResponse_Median_t& obj) {
    std::string json;
    serialize_StatsResponse_Median_t_into(obj, json);
    return json;
}

inline void serialize_RegisterRequest_into(const RegisterRequest& obj, std::string& json) {
    json.push_back('{');
    json.append("\"username\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.username, json);
    json.push_back('"');
    json.append(",\"email\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.email, json);
    json.push_back('"');
    json.append(",\"password\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.password, json);
    json.push_back('"');
    json.append(",\"age\":");
    {
        if (!obj.age) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.age);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    json.append(",\"role\":");
    if (obj.role) {
        json.push_back('"');
        json.append(to_string(*obj.role));
        json.push_back('"');
    } else {
        json.append("null");
    }
    json.append(",\"tags\":");
    if (!obj.tags) {
        json.append("null");
    } else {
    json.push_back('[');
    for (size_t i = 0; i < obj.tags->size(); ++i) {
        if (i > 0) json.push_back(',');
        json.push_back('"');
        katana::serde::escape_json_string_into((*obj.tags)[i], json);
        json.push_back('"');
    }
    json.push_back(']');
    }
    json.push_back('}');
}

inline std::string serialize_RegisterRequest(const RegisterRequest& obj) {
    std::string json;
    size_t reserve_estimate = 270;
    reserve_estimate += obj.username.size();
    reserve_estimate += obj.email.size();
    reserve_estimate += obj.password.size();
    if (obj.tags) reserve_estimate += obj.tags->size() * 32;
    json.reserve(reserve_estimate);
    serialize_RegisterRequest_into(obj, json);
    return json;
}

inline void serialize_RegisterRequest_Username_t_into(const RegisterRequest_Username_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_RegisterRequest_Username_t(const RegisterRequest_Username_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_RegisterRequest_Username_t_into(obj, json);
    return json;
}

inline void serialize_RegisterRequest_Email_t_into(const RegisterRequest_Email_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_RegisterRequest_Email_t(const RegisterRequest_Email_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_RegisterRequest_Email_t_into(obj, json);
    return json;
}

inline void serialize_RegisterRequest_Password_t_into(const RegisterRequest_Password_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_RegisterRequest_Password_t(const RegisterRequest_Password_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_RegisterRequest_Password_t_into(obj, json);
    return json;
}

inline void serialize_RegisterRequest_Age_t_into(const RegisterRequest_Age_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_RegisterRequest_Age_t(const RegisterRequest_Age_t& obj) {
    std::string json;
    serialize_RegisterRequest_Age_t_into(obj, json);
    return json;
}

inline void serialize_RegisterRequest_Role_t_into(const RegisterRequest_Role_t& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_RegisterRequest_Role_t(const RegisterRequest_Role_t& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_RegisterRequest_Tags_t_into(const RegisterRequest_Tags_t& obj, std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_Tags_t(const RegisterRequest_Tags_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_RegisterRequest_Tags_t_into(obj, json);
    return json;
}

inline void serialize_RegisterRequest_Item_t_into(const RegisterRequest_Item_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_RegisterRequest_Item_t(const RegisterRequest_Item_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_RegisterRequest_Item_t_into(obj, json);
    return json;
}

inline void serialize_UserRole_into(const UserRole& obj, std::string& json) {
    auto str = to_string(obj);
    json.push_back('"');
    json.append(str);
    json.push_back('"');
}

inline std::string serialize_UserRole(const UserRole& obj) {
    std::string json;
    auto str = to_string(obj);
    json.reserve(str.size() + 2);
    serialize_UserRole_into(obj, json);
    return json;
}

inline void serialize_UserResponse_into(const UserResponse& obj, std::string& json) {
    json.push_back('{');
    json.append("\"id\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.id, json);
    json.push_back('"');
    json.append(",\"username\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.username, json);
    json.push_back('"');
    json.append(",\"email\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.email, json);
    json.push_back('"');
    json.append(",\"role\":");
    json.push_back('"');
    json.append(to_string(obj.role));
    json.push_back('"');
    json.append(",\"created_at\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.created_at, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_UserResponse(const UserResponse& obj) {
    std::string json;
    size_t reserve_estimate = 211;
    reserve_estimate += obj.id.size();
    reserve_estimate += obj.username.size();
    reserve_estimate += obj.email.size();
    reserve_estimate += obj.created_at.size();
    json.reserve(reserve_estimate);
    serialize_UserResponse_into(obj, json);
    return json;
}

inline void serialize_UserResponse_Id_t_into(const UserResponse_Id_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_UserResponse_Id_t(const UserResponse_Id_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_UserResponse_Id_t_into(obj, json);
    return json;
}

inline void serialize_UserResponse_Username_t_into(const UserResponse_Username_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_UserResponse_Username_t(const UserResponse_Username_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_UserResponse_Username_t_into(obj, json);
    return json;
}

inline void serialize_UserResponse_Email_t_into(const UserResponse_Email_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_UserResponse_Email_t(const UserResponse_Email_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_UserResponse_Email_t_into(obj, json);
    return json;
}

inline void serialize_UserResponse_Role_t_into(const UserResponse_Role_t& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_UserResponse_Role_t(const UserResponse_Role_t& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_UserResponse_Created_at_t_into(const UserResponse_Created_at_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_UserResponse_Created_at_t(const UserResponse_Created_at_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_UserResponse_Created_at_t_into(obj, json);
    return json;
}

inline void serialize_ItemCategory_into(const ItemCategory& obj, std::string& json) {
    auto str = to_string(obj);
    json.push_back('"');
    json.append(str);
    json.push_back('"');
}

inline std::string serialize_ItemCategory(const ItemCategory& obj) {
    std::string json;
    auto str = to_string(obj);
    json.reserve(str.size() + 2);
    serialize_ItemCategory_into(obj, json);
    return json;
}

inline void serialize_Item_into(const Item& obj, std::string& json) {
    json.push_back('{');
    json.append("\"id\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.id);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"name\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.name, json);
    json.push_back('"');
    json.append(",\"description\":");
    if (obj.description) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.description, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    json.append(",\"price\":");
    {
        char buf[64];
        auto res = std::to_chars(buf, buf + sizeof(buf), obj.price);
        if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
    }
    json.append(",\"stock\":");
    {
        if (!obj.stock) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.stock);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    json.append(",\"category\":");
    json.push_back('"');
    json.append(to_string(obj.category));
    json.push_back('"');
    json.append(",\"tags\":");
    if (!obj.tags) {
        json.append("null");
    } else {
    json.push_back('[');
    for (size_t i = 0; i < obj.tags->size(); ++i) {
        if (i > 0) json.push_back(',');
        json.push_back('"');
        katana::serde::escape_json_string_into((*obj.tags)[i], json);
        json.push_back('"');
    }
    json.push_back(']');
    }
    json.append(",\"metadata\":");
    if (obj.metadata) {
        serialize_ItemMetadata_into(*obj.metadata, json);
    } else {
        json.append("null");
    }
    json.push_back('}');
}

inline std::string serialize_Item(const Item& obj) {
    std::string json;
    size_t reserve_estimate = 434;
    reserve_estimate += obj.name.size();
    if (obj.description) reserve_estimate += obj.description->size();
    if (obj.tags) reserve_estimate += obj.tags->size() * 32;
    json.reserve(reserve_estimate);
    serialize_Item_into(obj, json);
    return json;
}

inline void serialize_Item_Id_t_into(const Item_Id_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_Item_Id_t(const Item_Id_t& obj) {
    std::string json;
    serialize_Item_Id_t_into(obj, json);
    return json;
}

inline void serialize_Item_Name_t_into(const Item_Name_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_Item_Name_t(const Item_Name_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_Item_Name_t_into(obj, json);
    return json;
}

inline void serialize_Item_Description_t_into(const Item_Description_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_Item_Description_t(const Item_Description_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_Item_Description_t_into(obj, json);
    return json;
}

inline void serialize_Item_Price_t_into(const Item_Price_t& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_Item_Price_t(const Item_Price_t& obj) {
    std::string json;
    serialize_Item_Price_t_into(obj, json);
    return json;
}

inline void serialize_Item_Stock_t_into(const Item_Stock_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_Item_Stock_t(const Item_Stock_t& obj) {
    std::string json;
    serialize_Item_Stock_t_into(obj, json);
    return json;
}

inline void serialize_Item_Category_t_into(const Item_Category_t& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_Item_Category_t(const Item_Category_t& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_Item_Tags_t_into(const Item_Tags_t& obj, std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Tags_t(const Item_Tags_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_Item_Tags_t_into(obj, json);
    return json;
}

inline void serialize_Item_Item_t_into(const Item_Item_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_Item_Item_t(const Item_Item_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_Item_Item_t_into(obj, json);
    return json;
}

inline void serialize_Item_Metadata_t_into(const Item_Metadata_t& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_Item_Metadata_t(const Item_Metadata_t& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_ItemMetadata_into(const ItemMetadata& obj, std::string& json) {
    json.push_back('{');
    json.append("\"weight_kg\":");
    {
        if (!obj.weight_kg) {
            json.append("null");
        } else {
        char buf[64];
        auto res = std::to_chars(buf, buf + sizeof(buf), *obj.weight_kg);
        if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
        }
    }
    json.append(",\"dimensions\":");
    if (obj.dimensions) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.dimensions, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    json.append(",\"manufacturer\":");
    if (obj.manufacturer) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.manufacturer, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    json.append(",\"sku\":");
    if (obj.sku) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.sku, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    json.push_back('}');
}

inline std::string serialize_ItemMetadata(const ItemMetadata& obj) {
    std::string json;
    size_t reserve_estimate = 173;
    if (obj.dimensions) reserve_estimate += obj.dimensions->size();
    if (obj.manufacturer) reserve_estimate += obj.manufacturer->size();
    if (obj.sku) reserve_estimate += obj.sku->size();
    json.reserve(reserve_estimate);
    serialize_ItemMetadata_into(obj, json);
    return json;
}

inline void serialize_ItemMetadata_Weight_kg_t_into(const ItemMetadata_Weight_kg_t& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_ItemMetadata_Weight_kg_t(const ItemMetadata_Weight_kg_t& obj) {
    std::string json;
    serialize_ItemMetadata_Weight_kg_t_into(obj, json);
    return json;
}

inline void serialize_ItemMetadata_Dimensions_t_into(const ItemMetadata_Dimensions_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_ItemMetadata_Dimensions_t(const ItemMetadata_Dimensions_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_ItemMetadata_Dimensions_t_into(obj, json);
    return json;
}

inline void serialize_ItemMetadata_Manufacturer_t_into(const ItemMetadata_Manufacturer_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_ItemMetadata_Manufacturer_t(const ItemMetadata_Manufacturer_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_ItemMetadata_Manufacturer_t_into(obj, json);
    return json;
}

inline void serialize_ItemMetadata_Sku_t_into(const ItemMetadata_Sku_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_ItemMetadata_Sku_t(const ItemMetadata_Sku_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_ItemMetadata_Sku_t_into(obj, json);
    return json;
}

inline void serialize_CreateItemRequest_into(const CreateItemRequest& obj, std::string& json) {
    json.push_back('{');
    json.append("\"name\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.name, json);
    json.push_back('"');
    json.append(",\"description\":");
    if (obj.description) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.description, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    json.append(",\"price\":");
    {
        char buf[64];
        auto res = std::to_chars(buf, buf + sizeof(buf), obj.price);
        if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
    }
    json.append(",\"stock\":");
    {
        if (!obj.stock) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.stock);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    json.append(",\"category\":");
    json.push_back('"');
    json.append(to_string(obj.category));
    json.push_back('"');
    json.append(",\"tags\":");
    if (!obj.tags) {
        json.append("null");
    } else {
    json.push_back('[');
    for (size_t i = 0; i < obj.tags->size(); ++i) {
        if (i > 0) json.push_back(',');
        json.push_back('"');
        katana::serde::escape_json_string_into((*obj.tags)[i], json);
        json.push_back('"');
    }
    json.push_back(']');
    }
    json.append(",\"metadata\":");
    if (obj.metadata) {
        serialize_ItemMetadata_into(*obj.metadata, json);
    } else {
        json.append("null");
    }
    json.push_back('}');
}

inline std::string serialize_CreateItemRequest(const CreateItemRequest& obj) {
    std::string json;
    size_t reserve_estimate = 408;
    reserve_estimate += obj.name.size();
    if (obj.description) reserve_estimate += obj.description->size();
    if (obj.tags) reserve_estimate += obj.tags->size() * 32;
    json.reserve(reserve_estimate);
    serialize_CreateItemRequest_into(obj, json);
    return json;
}

inline void serialize_CreateItemRequest_Name_t_into(const CreateItemRequest_Name_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_CreateItemRequest_Name_t(const CreateItemRequest_Name_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_CreateItemRequest_Name_t_into(obj, json);
    return json;
}

inline void serialize_CreateItemRequest_Description_t_into(const CreateItemRequest_Description_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_CreateItemRequest_Description_t(const CreateItemRequest_Description_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_CreateItemRequest_Description_t_into(obj, json);
    return json;
}

inline void serialize_CreateItemRequest_Price_t_into(const CreateItemRequest_Price_t& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_CreateItemRequest_Price_t(const CreateItemRequest_Price_t& obj) {
    std::string json;
    serialize_CreateItemRequest_Price_t_into(obj, json);
    return json;
}

inline void serialize_CreateItemRequest_Stock_t_into(const CreateItemRequest_Stock_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_CreateItemRequest_Stock_t(const CreateItemRequest_Stock_t& obj) {
    std::string json;
    serialize_CreateItemRequest_Stock_t_into(obj, json);
    return json;
}

inline void serialize_CreateItemRequest_Category_t_into(const CreateItemRequest_Category_t& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_CreateItemRequest_Category_t(const CreateItemRequest_Category_t& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_CreateItemRequest_Tags_t_into(const CreateItemRequest_Tags_t& obj, std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_Tags_t(const CreateItemRequest_Tags_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_CreateItemRequest_Tags_t_into(obj, json);
    return json;
}

inline void serialize_CreateItemRequest_Item_t_into(const CreateItemRequest_Item_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_CreateItemRequest_Item_t(const CreateItemRequest_Item_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_CreateItemRequest_Item_t_into(obj, json);
    return json;
}

inline void serialize_CreateItemRequest_Metadata_t_into(const CreateItemRequest_Metadata_t& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_CreateItemRequest_Metadata_t(const CreateItemRequest_Metadata_t& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_UpdateItemRequest_into(const UpdateItemRequest& obj, std::string& json) {
    json.push_back('{');
    json.append("\"name\":");
    if (obj.name) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.name, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    json.append(",\"description\":");
    if (obj.description) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.description, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    json.append(",\"price\":");
    {
        if (!obj.price) {
            json.append("null");
        } else {
        char buf[64];
        auto res = std::to_chars(buf, buf + sizeof(buf), *obj.price);
        if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
        }
    }
    json.append(",\"stock\":");
    {
        if (!obj.stock) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.stock);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    json.append(",\"category\":");
    if (obj.category) {
        json.push_back('"');
        json.append(to_string(*obj.category));
        json.push_back('"');
    } else {
        json.append("null");
    }
    json.append(",\"tags\":");
    if (!obj.tags) {
        json.append("null");
    } else {
    json.push_back('[');
    for (size_t i = 0; i < obj.tags->size(); ++i) {
        if (i > 0) json.push_back(',');
        json.push_back('"');
        katana::serde::escape_json_string_into((*obj.tags)[i], json);
        json.push_back('"');
    }
    json.push_back(']');
    }
    json.push_back('}');
}

inline std::string serialize_UpdateItemRequest(const UpdateItemRequest& obj) {
    std::string json;
    size_t reserve_estimate = 268;
    if (obj.name) reserve_estimate += obj.name->size();
    if (obj.description) reserve_estimate += obj.description->size();
    if (obj.tags) reserve_estimate += obj.tags->size() * 32;
    json.reserve(reserve_estimate);
    serialize_UpdateItemRequest_into(obj, json);
    return json;
}

inline void serialize_UpdateItemRequest_Name_t_into(const UpdateItemRequest_Name_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_UpdateItemRequest_Name_t(const UpdateItemRequest_Name_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_UpdateItemRequest_Name_t_into(obj, json);
    return json;
}

inline void serialize_UpdateItemRequest_Description_t_into(const UpdateItemRequest_Description_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_UpdateItemRequest_Description_t(const UpdateItemRequest_Description_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_UpdateItemRequest_Description_t_into(obj, json);
    return json;
}

inline void serialize_UpdateItemRequest_Price_t_into(const UpdateItemRequest_Price_t& obj, std::string& json) {
    char buf[64];
    auto res = std::to_chars(buf, buf + sizeof(buf), obj);
    if (res.ec == std::errc()) json.append(buf, static_cast<size_t>(res.ptr - buf));
}

inline std::string serialize_UpdateItemRequest_Price_t(const UpdateItemRequest_Price_t& obj) {
    std::string json;
    serialize_UpdateItemRequest_Price_t_into(obj, json);
    return json;
}

inline void serialize_UpdateItemRequest_Stock_t_into(const UpdateItemRequest_Stock_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_UpdateItemRequest_Stock_t(const UpdateItemRequest_Stock_t& obj) {
    std::string json;
    serialize_UpdateItemRequest_Stock_t_into(obj, json);
    return json;
}

inline void serialize_UpdateItemRequest_Category_t_into(const UpdateItemRequest_Category_t& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_UpdateItemRequest_Category_t(const UpdateItemRequest_Category_t& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_UpdateItemRequest_Tags_t_into(const UpdateItemRequest_Tags_t& obj, std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_Tags_t(const UpdateItemRequest_Tags_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_UpdateItemRequest_Tags_t_into(obj, json);
    return json;
}

inline void serialize_UpdateItemRequest_Item_t_into(const UpdateItemRequest_Item_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_UpdateItemRequest_Item_t(const UpdateItemRequest_Item_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_UpdateItemRequest_Item_t_into(obj, json);
    return json;
}

inline void serialize_ItemList_into(const ItemList& obj, std::string& json) {
    json.push_back('{');
    json.append("\"items\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.items.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_into(obj.items[i], json);
    }
    json.push_back(']');
    json.append(",\"total\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.total);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"limit\":");
    {
        if (!obj.limit) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.limit);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    json.append(",\"offset\":");
    {
        if (!obj.offset) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.offset);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    json.push_back('}');
}

inline std::string serialize_ItemList(const ItemList& obj) {
    std::string json;
    size_t reserve_estimate = 163;
    reserve_estimate += obj.items.size() * 434;
    json.reserve(reserve_estimate);
    serialize_ItemList_into(obj, json);
    return json;
}

inline void serialize_ItemList_Items_t_into(const ItemList_Items_t& obj, std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemList_Items_t(const ItemList_Items_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_ItemList_Items_t_into(obj, json);
    return json;
}

inline void serialize_ItemList_Item_t_into(const ItemList_Item_t& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_ItemList_Item_t(const ItemList_Item_t& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_ItemList_Total_t_into(const ItemList_Total_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_ItemList_Total_t(const ItemList_Total_t& obj) {
    std::string json;
    serialize_ItemList_Total_t_into(obj, json);
    return json;
}

inline void serialize_ItemList_Limit_t_into(const ItemList_Limit_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_ItemList_Limit_t(const ItemList_Limit_t& obj) {
    std::string json;
    serialize_ItemList_Limit_t_into(obj, json);
    return json;
}

inline void serialize_ItemList_Offset_t_into(const ItemList_Offset_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_ItemList_Offset_t(const ItemList_Offset_t& obj) {
    std::string json;
    serialize_ItemList_Offset_t_into(obj, json);
    return json;
}

inline void serialize_EchoRequest_into(const EchoRequest& obj, std::string& json) {
    json.push_back('{');
    json.append("\"message\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.message, json);
    json.push_back('"');
    json.append(",\"repeat\":");
    {
        if (!obj.repeat) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.repeat);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    json.append(",\"uppercase\":");
    if (!obj.uppercase) {
        json.append("null");
    } else {
        json.append(*obj.uppercase ? "true" : "false");
    }
    json.push_back('}');
}

inline std::string serialize_EchoRequest(const EchoRequest& obj) {
    std::string json;
    size_t reserve_estimate = 93;
    reserve_estimate += obj.message.size();
    json.reserve(reserve_estimate);
    serialize_EchoRequest_into(obj, json);
    return json;
}

inline void serialize_EchoRequest_Message_t_into(const EchoRequest_Message_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_EchoRequest_Message_t(const EchoRequest_Message_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_EchoRequest_Message_t_into(obj, json);
    return json;
}

inline void serialize_EchoRequest_Repeat_t_into(const EchoRequest_Repeat_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_EchoRequest_Repeat_t(const EchoRequest_Repeat_t& obj) {
    std::string json;
    serialize_EchoRequest_Repeat_t_into(obj, json);
    return json;
}

inline void serialize_EchoRequest_Uppercase_t_into(const EchoRequest_Uppercase_t& obj, std::string& json) {
    json.append(obj ? "true" : "false");
}

inline std::string serialize_EchoRequest_Uppercase_t(const EchoRequest_Uppercase_t& obj) {
    return obj ? "true" : "false";
}

inline void serialize_EchoResponse_into(const EchoResponse& obj, std::string& json) {
    json.push_back('{');
    json.append("\"message\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.message, json);
    json.push_back('"');
    json.append(",\"length\":");
    {
        if (!obj.length) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.length);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    json.push_back('}');
}

inline std::string serialize_EchoResponse(const EchoResponse& obj) {
    std::string json;
    size_t reserve_estimate = 75;
    reserve_estimate += obj.message.size();
    json.reserve(reserve_estimate);
    serialize_EchoResponse_into(obj, json);
    return json;
}

inline void serialize_EchoResponse_Message_t_into(const EchoResponse_Message_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_EchoResponse_Message_t(const EchoResponse_Message_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_EchoResponse_Message_t_into(obj, json);
    return json;
}

inline void serialize_EchoResponse_Length_t_into(const EchoResponse_Length_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_EchoResponse_Length_t(const EchoResponse_Length_t& obj) {
    std::string json;
    serialize_EchoResponse_Length_t_into(obj, json);
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

inline void serialize_list_items_param_limit_into(const list_items_param_limit& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_list_items_param_limit(const list_items_param_limit& obj) {
    std::string json;
    serialize_list_items_param_limit_into(obj, json);
    return json;
}

inline void serialize_list_items_param_offset_into(const list_items_param_offset& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_list_items_param_offset(const list_items_param_offset& obj) {
    std::string json;
    serialize_list_items_param_offset_into(obj, json);
    return json;
}

inline void serialize_schema_6_into(const schema_6& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_6(const schema_6& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_7_into(const schema_7& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_7(const schema_7& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_create_item_param_X_Request_Id_into(const create_item_param_X_Request_Id& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_create_item_param_X_Request_Id(const create_item_param_X_Request_Id& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_create_item_param_X_Request_Id_into(obj, json);
    return json;
}

inline void serialize_create_item_param_session_into(const create_item_param_session& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_create_item_param_session(const create_item_param_session& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_create_item_param_session_into(obj, json);
    return json;
}

inline void serialize_schema_8_into(const schema_8& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_8(const schema_8& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_9_into(const schema_9& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_9(const schema_9& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_get_item_param_id_into(const get_item_param_id& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_get_item_param_id(const get_item_param_id& obj) {
    std::string json;
    serialize_get_item_param_id_into(obj, json);
    return json;
}

inline void serialize_schema_10_into(const schema_10& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_10(const schema_10& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_update_item_param_id_into(const update_item_param_id& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_update_item_param_id(const update_item_param_id& obj) {
    std::string json;
    serialize_update_item_param_id_into(obj, json);
    return json;
}

inline void serialize_schema_11_into(const schema_11& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_11(const schema_11& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_12_into(const schema_12& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_12(const schema_12& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_delete_item_param_id_into(const delete_item_param_id& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_delete_item_param_id(const delete_item_param_id& obj) {
    std::string json;
    serialize_delete_item_param_id_into(obj, json);
    return json;
}

inline void serialize_schema_13_into(const schema_13& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_13(const schema_13& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_14_into(const schema_14& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_14(const schema_14& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_health_check_response_into(const health_check_response& obj, std::string& json) {
    json.push_back('{');
    json.append("\"status\":");
    if (obj.status) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.status, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    json.append(",\"uptime_ms\":");
    {
        if (!obj.uptime_ms) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.uptime_ms);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    json.push_back('}');
}

inline std::string serialize_health_check_response(const health_check_response& obj) {
    std::string json;
    size_t reserve_estimate = 77;
    if (obj.status) reserve_estimate += obj.status->size();
    json.reserve(reserve_estimate);
    serialize_health_check_response_into(obj, json);
    return json;
}

inline void serialize_health_check_response_Status_into(const health_check_response_Status& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_health_check_response_Status(const health_check_response_Status& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_health_check_response_Status_into(obj, json);
    return json;
}

inline void serialize_health_check_response_Uptime_ms_into(const health_check_response_Uptime_ms& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_health_check_response_Uptime_ms(const health_check_response_Uptime_ms& obj) {
    std::string json;
    serialize_health_check_response_Uptime_ms_into(obj, json);
    return json;
}

// ============================================================
// Array Parse Functions
// ============================================================

[[nodiscard]] inline std::optional<std::vector<SumRequest>> parse_SumRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<SumRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_SumRequest(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SumRequest>> parse_SumRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SumRequest_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<SumRequest_Values_t>> parse_SumRequest_Values_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<SumRequest_Values_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_SumRequest_Values_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SumRequest_Values_t>> parse_SumRequest_Values_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SumRequest_Values_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<SumRequest_Item_t>> parse_SumRequest_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<SumRequest_Item_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_SumRequest_Item_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SumRequest_Item_t>> parse_SumRequest_Item_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SumRequest_Item_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<SumResponse>> parse_SumResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<SumResponse> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_SumResponse(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SumResponse>> parse_SumResponse_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SumResponse_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<SumResponse_Result_t>> parse_SumResponse_Result_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<SumResponse_Result_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_SumResponse_Result_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SumResponse_Result_t>> parse_SumResponse_Result_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SumResponse_Result_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<SumResponse_Count_t>> parse_SumResponse_Count_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<SumResponse_Count_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_SumResponse_Count_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SumResponse_Count_t>> parse_SumResponse_Count_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SumResponse_Count_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<StatsRequest>> parse_StatsRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<StatsRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_StatsRequest(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<StatsRequest>> parse_StatsRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsRequest_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<StatsRequest_Values_t>> parse_StatsRequest_Values_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<StatsRequest_Values_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_StatsRequest_Values_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<StatsRequest_Values_t>> parse_StatsRequest_Values_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsRequest_Values_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<StatsRequest_Item_t>> parse_StatsRequest_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<StatsRequest_Item_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_StatsRequest_Item_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<StatsRequest_Item_t>> parse_StatsRequest_Item_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsRequest_Item_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<StatsRequest_Include_median_t>> parse_StatsRequest_Include_median_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<StatsRequest_Include_median_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_StatsRequest_Include_median_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<StatsRequest_Include_median_t>> parse_StatsRequest_Include_median_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsRequest_Include_median_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<StatsResponse>> parse_StatsResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<StatsResponse> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_StatsResponse(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<StatsResponse>> parse_StatsResponse_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsResponse_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<StatsResponse_Min_t>> parse_StatsResponse_Min_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<StatsResponse_Min_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_StatsResponse_Min_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<StatsResponse_Min_t>> parse_StatsResponse_Min_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsResponse_Min_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<StatsResponse_Max_t>> parse_StatsResponse_Max_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<StatsResponse_Max_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_StatsResponse_Max_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<StatsResponse_Max_t>> parse_StatsResponse_Max_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsResponse_Max_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<StatsResponse_Mean_t>> parse_StatsResponse_Mean_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<StatsResponse_Mean_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_StatsResponse_Mean_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<StatsResponse_Mean_t>> parse_StatsResponse_Mean_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsResponse_Mean_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<StatsResponse_Sum_t>> parse_StatsResponse_Sum_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<StatsResponse_Sum_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_StatsResponse_Sum_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<StatsResponse_Sum_t>> parse_StatsResponse_Sum_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsResponse_Sum_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<StatsResponse_Count_t>> parse_StatsResponse_Count_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<StatsResponse_Count_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_StatsResponse_Count_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<StatsResponse_Count_t>> parse_StatsResponse_Count_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsResponse_Count_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<StatsResponse_Median_t>> parse_StatsResponse_Median_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<StatsResponse_Median_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_StatsResponse_Median_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<StatsResponse_Median_t>> parse_StatsResponse_Median_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsResponse_Median_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<RegisterRequest>> parse_RegisterRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<RegisterRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_RegisterRequest(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<RegisterRequest>> parse_RegisterRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterRequest_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Username_t>> parse_RegisterRequest_Username_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<RegisterRequest_Username_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_RegisterRequest_Username_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Username_t>> parse_RegisterRequest_Username_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterRequest_Username_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Email_t>> parse_RegisterRequest_Email_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<RegisterRequest_Email_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_RegisterRequest_Email_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Email_t>> parse_RegisterRequest_Email_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterRequest_Email_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Password_t>> parse_RegisterRequest_Password_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<RegisterRequest_Password_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_RegisterRequest_Password_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Password_t>> parse_RegisterRequest_Password_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterRequest_Password_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Age_t>> parse_RegisterRequest_Age_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<RegisterRequest_Age_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_RegisterRequest_Age_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Age_t>> parse_RegisterRequest_Age_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterRequest_Age_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Role_t>> parse_RegisterRequest_Role_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<RegisterRequest_Role_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_RegisterRequest_Role_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Role_t>> parse_RegisterRequest_Role_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterRequest_Role_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Tags_t>> parse_RegisterRequest_Tags_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<RegisterRequest_Tags_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_RegisterRequest_Tags_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Tags_t>> parse_RegisterRequest_Tags_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterRequest_Tags_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Item_t>> parse_RegisterRequest_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<RegisterRequest_Item_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_RegisterRequest_Item_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<RegisterRequest_Item_t>> parse_RegisterRequest_Item_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterRequest_Item_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UserRole>> parse_UserRole_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UserRole> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UserRole(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UserRole>> parse_UserRole_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserRole_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UserResponse>> parse_UserResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UserResponse> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UserResponse(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UserResponse>> parse_UserResponse_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserResponse_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UserResponse_Id_t>> parse_UserResponse_Id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UserResponse_Id_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UserResponse_Id_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UserResponse_Id_t>> parse_UserResponse_Id_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserResponse_Id_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UserResponse_Username_t>> parse_UserResponse_Username_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UserResponse_Username_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UserResponse_Username_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UserResponse_Username_t>> parse_UserResponse_Username_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserResponse_Username_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UserResponse_Email_t>> parse_UserResponse_Email_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UserResponse_Email_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UserResponse_Email_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UserResponse_Email_t>> parse_UserResponse_Email_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserResponse_Email_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UserResponse_Role_t>> parse_UserResponse_Role_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UserResponse_Role_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UserResponse_Role_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UserResponse_Role_t>> parse_UserResponse_Role_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserResponse_Role_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UserResponse_Created_at_t>> parse_UserResponse_Created_at_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UserResponse_Created_at_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UserResponse_Created_at_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UserResponse_Created_at_t>> parse_UserResponse_Created_at_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserResponse_Created_at_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ItemCategory>> parse_ItemCategory_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ItemCategory> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ItemCategory(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ItemCategory>> parse_ItemCategory_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemCategory_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Item>> parse_Item_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<Item> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_Item(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Item>> parse_Item_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Item_Id_t>> parse_Item_Id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<Item_Id_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_Item_Id_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Item_Id_t>> parse_Item_Id_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_Id_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Item_Name_t>> parse_Item_Name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<Item_Name_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_Item_Name_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Item_Name_t>> parse_Item_Name_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_Name_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Item_Description_t>> parse_Item_Description_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<Item_Description_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_Item_Description_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Item_Description_t>> parse_Item_Description_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_Description_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Item_Price_t>> parse_Item_Price_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<Item_Price_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_Item_Price_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Item_Price_t>> parse_Item_Price_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_Price_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Item_Stock_t>> parse_Item_Stock_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<Item_Stock_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_Item_Stock_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Item_Stock_t>> parse_Item_Stock_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_Stock_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Item_Category_t>> parse_Item_Category_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<Item_Category_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_Item_Category_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Item_Category_t>> parse_Item_Category_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_Category_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Item_Tags_t>> parse_Item_Tags_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<Item_Tags_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_Item_Tags_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Item_Tags_t>> parse_Item_Tags_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_Tags_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Item_Item_t>> parse_Item_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<Item_Item_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_Item_Item_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Item_Item_t>> parse_Item_Item_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_Item_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Item_Metadata_t>> parse_Item_Metadata_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<Item_Metadata_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_Item_Metadata_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Item_Metadata_t>> parse_Item_Metadata_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_Metadata_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ItemMetadata>> parse_ItemMetadata_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ItemMetadata> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ItemMetadata(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ItemMetadata>> parse_ItemMetadata_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemMetadata_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ItemMetadata_Weight_kg_t>> parse_ItemMetadata_Weight_kg_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ItemMetadata_Weight_kg_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ItemMetadata_Weight_kg_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ItemMetadata_Weight_kg_t>> parse_ItemMetadata_Weight_kg_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemMetadata_Weight_kg_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ItemMetadata_Dimensions_t>> parse_ItemMetadata_Dimensions_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ItemMetadata_Dimensions_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ItemMetadata_Dimensions_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ItemMetadata_Dimensions_t>> parse_ItemMetadata_Dimensions_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemMetadata_Dimensions_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ItemMetadata_Manufacturer_t>> parse_ItemMetadata_Manufacturer_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ItemMetadata_Manufacturer_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ItemMetadata_Manufacturer_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ItemMetadata_Manufacturer_t>> parse_ItemMetadata_Manufacturer_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemMetadata_Manufacturer_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ItemMetadata_Sku_t>> parse_ItemMetadata_Sku_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ItemMetadata_Sku_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ItemMetadata_Sku_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ItemMetadata_Sku_t>> parse_ItemMetadata_Sku_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemMetadata_Sku_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest>> parse_CreateItemRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CreateItemRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CreateItemRequest(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest>> parse_CreateItemRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateItemRequest_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Name_t>> parse_CreateItemRequest_Name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CreateItemRequest_Name_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CreateItemRequest_Name_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Name_t>> parse_CreateItemRequest_Name_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateItemRequest_Name_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Description_t>> parse_CreateItemRequest_Description_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CreateItemRequest_Description_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CreateItemRequest_Description_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Description_t>> parse_CreateItemRequest_Description_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateItemRequest_Description_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Price_t>> parse_CreateItemRequest_Price_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CreateItemRequest_Price_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CreateItemRequest_Price_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Price_t>> parse_CreateItemRequest_Price_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateItemRequest_Price_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Stock_t>> parse_CreateItemRequest_Stock_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CreateItemRequest_Stock_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CreateItemRequest_Stock_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Stock_t>> parse_CreateItemRequest_Stock_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateItemRequest_Stock_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Category_t>> parse_CreateItemRequest_Category_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CreateItemRequest_Category_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CreateItemRequest_Category_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Category_t>> parse_CreateItemRequest_Category_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateItemRequest_Category_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Tags_t>> parse_CreateItemRequest_Tags_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CreateItemRequest_Tags_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CreateItemRequest_Tags_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Tags_t>> parse_CreateItemRequest_Tags_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateItemRequest_Tags_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Item_t>> parse_CreateItemRequest_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CreateItemRequest_Item_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CreateItemRequest_Item_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Item_t>> parse_CreateItemRequest_Item_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateItemRequest_Item_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Metadata_t>> parse_CreateItemRequest_Metadata_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CreateItemRequest_Metadata_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CreateItemRequest_Metadata_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest_Metadata_t>> parse_CreateItemRequest_Metadata_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateItemRequest_Metadata_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest>> parse_UpdateItemRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UpdateItemRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UpdateItemRequest(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest>> parse_UpdateItemRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateItemRequest_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Name_t>> parse_UpdateItemRequest_Name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UpdateItemRequest_Name_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UpdateItemRequest_Name_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Name_t>> parse_UpdateItemRequest_Name_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateItemRequest_Name_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Description_t>> parse_UpdateItemRequest_Description_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UpdateItemRequest_Description_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UpdateItemRequest_Description_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Description_t>> parse_UpdateItemRequest_Description_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateItemRequest_Description_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Price_t>> parse_UpdateItemRequest_Price_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UpdateItemRequest_Price_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UpdateItemRequest_Price_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Price_t>> parse_UpdateItemRequest_Price_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateItemRequest_Price_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Stock_t>> parse_UpdateItemRequest_Stock_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UpdateItemRequest_Stock_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UpdateItemRequest_Stock_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Stock_t>> parse_UpdateItemRequest_Stock_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateItemRequest_Stock_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Category_t>> parse_UpdateItemRequest_Category_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UpdateItemRequest_Category_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UpdateItemRequest_Category_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Category_t>> parse_UpdateItemRequest_Category_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateItemRequest_Category_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Tags_t>> parse_UpdateItemRequest_Tags_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UpdateItemRequest_Tags_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UpdateItemRequest_Tags_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Tags_t>> parse_UpdateItemRequest_Tags_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateItemRequest_Tags_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Item_t>> parse_UpdateItemRequest_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UpdateItemRequest_Item_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UpdateItemRequest_Item_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest_Item_t>> parse_UpdateItemRequest_Item_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateItemRequest_Item_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ItemList>> parse_ItemList_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ItemList> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ItemList(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ItemList>> parse_ItemList_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemList_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ItemList_Items_t>> parse_ItemList_Items_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ItemList_Items_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ItemList_Items_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ItemList_Items_t>> parse_ItemList_Items_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemList_Items_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ItemList_Item_t>> parse_ItemList_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ItemList_Item_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ItemList_Item_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ItemList_Item_t>> parse_ItemList_Item_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemList_Item_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ItemList_Total_t>> parse_ItemList_Total_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ItemList_Total_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ItemList_Total_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ItemList_Total_t>> parse_ItemList_Total_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemList_Total_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ItemList_Limit_t>> parse_ItemList_Limit_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ItemList_Limit_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ItemList_Limit_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ItemList_Limit_t>> parse_ItemList_Limit_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemList_Limit_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ItemList_Offset_t>> parse_ItemList_Offset_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ItemList_Offset_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ItemList_Offset_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ItemList_Offset_t>> parse_ItemList_Offset_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemList_Offset_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<EchoRequest>> parse_EchoRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<EchoRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_EchoRequest(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<EchoRequest>> parse_EchoRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_EchoRequest_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<EchoRequest_Message_t>> parse_EchoRequest_Message_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<EchoRequest_Message_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_EchoRequest_Message_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<EchoRequest_Message_t>> parse_EchoRequest_Message_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_EchoRequest_Message_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<EchoRequest_Repeat_t>> parse_EchoRequest_Repeat_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<EchoRequest_Repeat_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_EchoRequest_Repeat_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<EchoRequest_Repeat_t>> parse_EchoRequest_Repeat_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_EchoRequest_Repeat_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<EchoRequest_Uppercase_t>> parse_EchoRequest_Uppercase_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<EchoRequest_Uppercase_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_EchoRequest_Uppercase_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<EchoRequest_Uppercase_t>> parse_EchoRequest_Uppercase_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_EchoRequest_Uppercase_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<EchoResponse>> parse_EchoResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<EchoResponse> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_EchoResponse(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<EchoResponse>> parse_EchoResponse_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_EchoResponse_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<EchoResponse_Message_t>> parse_EchoResponse_Message_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<EchoResponse_Message_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_EchoResponse_Message_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<EchoResponse_Message_t>> parse_EchoResponse_Message_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_EchoResponse_Message_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<EchoResponse_Length_t>> parse_EchoResponse_Length_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<EchoResponse_Length_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_EchoResponse_Length_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<EchoResponse_Length_t>> parse_EchoResponse_Length_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_EchoResponse_Length_t_array(cur, arena);
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

[[nodiscard]] inline std::optional<std::vector<list_items_param_limit>> parse_list_items_param_limit_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<list_items_param_limit> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_list_items_param_limit(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<list_items_param_limit>> parse_list_items_param_limit_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_list_items_param_limit_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<list_items_param_offset>> parse_list_items_param_offset_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<list_items_param_offset> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_list_items_param_offset(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<list_items_param_offset>> parse_list_items_param_offset_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_list_items_param_offset_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_6>> parse_schema_6_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_6> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_6(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_6>> parse_schema_6_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_6_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_7>> parse_schema_7_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_7> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_7(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_7>> parse_schema_7_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_7_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<create_item_param_X_Request_Id>> parse_create_item_param_X_Request_Id_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<create_item_param_X_Request_Id> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_create_item_param_X_Request_Id(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<create_item_param_X_Request_Id>> parse_create_item_param_X_Request_Id_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_create_item_param_X_Request_Id_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<create_item_param_session>> parse_create_item_param_session_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<create_item_param_session> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_create_item_param_session(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<create_item_param_session>> parse_create_item_param_session_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_create_item_param_session_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_8>> parse_schema_8_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_8> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_8(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_8>> parse_schema_8_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_8_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_9>> parse_schema_9_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_9> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_9(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_9>> parse_schema_9_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_9_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<get_item_param_id>> parse_get_item_param_id_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<get_item_param_id> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_get_item_param_id(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<get_item_param_id>> parse_get_item_param_id_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_get_item_param_id_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_10>> parse_schema_10_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_10> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_10(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_10>> parse_schema_10_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_10_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<update_item_param_id>> parse_update_item_param_id_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<update_item_param_id> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_update_item_param_id(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<update_item_param_id>> parse_update_item_param_id_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_update_item_param_id_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_11>> parse_schema_11_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_11> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_11(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_11>> parse_schema_11_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_11_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_12>> parse_schema_12_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_12> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_12(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_12>> parse_schema_12_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_12_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<delete_item_param_id>> parse_delete_item_param_id_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<delete_item_param_id> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_delete_item_param_id(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<delete_item_param_id>> parse_delete_item_param_id_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_delete_item_param_id_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_13>> parse_schema_13_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_13> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_13(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_13>> parse_schema_13_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_13_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_14>> parse_schema_14_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_14> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_14(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_14>> parse_schema_14_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_14_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<health_check_response>> parse_health_check_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<health_check_response> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_health_check_response(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<health_check_response>> parse_health_check_response_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_health_check_response_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<health_check_response_Status>> parse_health_check_response_Status_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<health_check_response_Status> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_health_check_response_Status(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<health_check_response_Status>> parse_health_check_response_Status_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_health_check_response_Status_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<health_check_response_Uptime_ms>> parse_health_check_response_Uptime_ms_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<health_check_response_Uptime_ms> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_health_check_response_Uptime_ms(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<health_check_response_Uptime_ms>> parse_health_check_response_Uptime_ms_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_health_check_response_Uptime_ms_array(cur, arena);
}

// ============================================================
// Array Serialize Functions
// ============================================================

inline void serialize_SumRequest_array_into(const std::vector<SumRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_SumRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SumRequest_array(const std::vector<SumRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 76 + 2);
    serialize_SumRequest_array_into(arr, json);
    return json;
}

inline void serialize_SumRequest_array_into(const arena_vector<SumRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_SumRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SumRequest_array(const arena_vector<SumRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 76 + 2);
    serialize_SumRequest_array_into(arr, json);
    return json;
}

inline void serialize_SumRequest_Values_t_array_into(const std::vector<SumRequest_Values_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_SumRequest_Values_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SumRequest_Values_t_array(const std::vector<SumRequest_Values_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 102 + 2);
    serialize_SumRequest_Values_t_array_into(arr, json);
    return json;
}

inline void serialize_SumRequest_Values_t_array_into(const arena_vector<SumRequest_Values_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_SumRequest_Values_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SumRequest_Values_t_array(const arena_vector<SumRequest_Values_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 102 + 2);
    serialize_SumRequest_Values_t_array_into(arr, json);
    return json;
}

inline void serialize_SumRequest_Item_t_array_into(const std::vector<SumRequest_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_SumRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SumRequest_Item_t_array(const std::vector<SumRequest_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_SumRequest_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_SumRequest_Item_t_array_into(const arena_vector<SumRequest_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_SumRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SumRequest_Item_t_array(const arena_vector<SumRequest_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_SumRequest_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_SumResponse_array_into(const std::vector<SumResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_SumResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SumResponse_array(const std::vector<SumResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 66 + 2);
    serialize_SumResponse_array_into(arr, json);
    return json;
}

inline void serialize_SumResponse_array_into(const arena_vector<SumResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_SumResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SumResponse_array(const arena_vector<SumResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 66 + 2);
    serialize_SumResponse_array_into(arr, json);
    return json;
}

inline void serialize_SumResponse_Result_t_array_into(const std::vector<SumResponse_Result_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_SumResponse_Result_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SumResponse_Result_t_array(const std::vector<SumResponse_Result_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_SumResponse_Result_t_array_into(arr, json);
    return json;
}

inline void serialize_SumResponse_Result_t_array_into(const arena_vector<SumResponse_Result_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_SumResponse_Result_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SumResponse_Result_t_array(const arena_vector<SumResponse_Result_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_SumResponse_Result_t_array_into(arr, json);
    return json;
}

inline void serialize_SumResponse_Count_t_array_into(const std::vector<SumResponse_Count_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_SumResponse_Count_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SumResponse_Count_t_array(const std::vector<SumResponse_Count_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_SumResponse_Count_t_array_into(arr, json);
    return json;
}

inline void serialize_SumResponse_Count_t_array_into(const arena_vector<SumResponse_Count_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_SumResponse_Count_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SumResponse_Count_t_array(const arena_vector<SumResponse_Count_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_SumResponse_Count_t_array_into(arr, json);
    return json;
}

inline void serialize_StatsRequest_array_into(const std::vector<StatsRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsRequest_array(const std::vector<StatsRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 99 + 2);
    serialize_StatsRequest_array_into(arr, json);
    return json;
}

inline void serialize_StatsRequest_array_into(const arena_vector<StatsRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsRequest_array(const arena_vector<StatsRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 99 + 2);
    serialize_StatsRequest_array_into(arr, json);
    return json;
}

inline void serialize_StatsRequest_Values_t_array_into(const std::vector<StatsRequest_Values_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsRequest_Values_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsRequest_Values_t_array(const std::vector<StatsRequest_Values_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 102 + 2);
    serialize_StatsRequest_Values_t_array_into(arr, json);
    return json;
}

inline void serialize_StatsRequest_Values_t_array_into(const arena_vector<StatsRequest_Values_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsRequest_Values_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsRequest_Values_t_array(const arena_vector<StatsRequest_Values_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 102 + 2);
    serialize_StatsRequest_Values_t_array_into(arr, json);
    return json;
}

inline void serialize_StatsRequest_Item_t_array_into(const std::vector<StatsRequest_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsRequest_Item_t_array(const std::vector<StatsRequest_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_StatsRequest_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_StatsRequest_Item_t_array_into(const arena_vector<StatsRequest_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsRequest_Item_t_array(const arena_vector<StatsRequest_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_StatsRequest_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_StatsRequest_Include_median_t_array_into(const std::vector<StatsRequest_Include_median_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsRequest_Include_median_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsRequest_Include_median_t_array(const std::vector<StatsRequest_Include_median_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 5 + 2);
    serialize_StatsRequest_Include_median_t_array_into(arr, json);
    return json;
}

inline void serialize_StatsRequest_Include_median_t_array_into(const arena_vector<StatsRequest_Include_median_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsRequest_Include_median_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsRequest_Include_median_t_array(const arena_vector<StatsRequest_Include_median_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 5 + 2);
    serialize_StatsRequest_Include_median_t_array_into(arr, json);
    return json;
}

inline void serialize_StatsResponse_array_into(const std::vector<StatsResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsResponse_array(const std::vector<StatsResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 195 + 2);
    serialize_StatsResponse_array_into(arr, json);
    return json;
}

inline void serialize_StatsResponse_array_into(const arena_vector<StatsResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsResponse_array(const arena_vector<StatsResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 195 + 2);
    serialize_StatsResponse_array_into(arr, json);
    return json;
}

inline void serialize_StatsResponse_Min_t_array_into(const std::vector<StatsResponse_Min_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsResponse_Min_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsResponse_Min_t_array(const std::vector<StatsResponse_Min_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_StatsResponse_Min_t_array_into(arr, json);
    return json;
}

inline void serialize_StatsResponse_Min_t_array_into(const arena_vector<StatsResponse_Min_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsResponse_Min_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsResponse_Min_t_array(const arena_vector<StatsResponse_Min_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_StatsResponse_Min_t_array_into(arr, json);
    return json;
}

inline void serialize_StatsResponse_Max_t_array_into(const std::vector<StatsResponse_Max_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsResponse_Max_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsResponse_Max_t_array(const std::vector<StatsResponse_Max_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_StatsResponse_Max_t_array_into(arr, json);
    return json;
}

inline void serialize_StatsResponse_Max_t_array_into(const arena_vector<StatsResponse_Max_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsResponse_Max_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsResponse_Max_t_array(const arena_vector<StatsResponse_Max_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_StatsResponse_Max_t_array_into(arr, json);
    return json;
}

inline void serialize_StatsResponse_Mean_t_array_into(const std::vector<StatsResponse_Mean_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsResponse_Mean_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsResponse_Mean_t_array(const std::vector<StatsResponse_Mean_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_StatsResponse_Mean_t_array_into(arr, json);
    return json;
}

inline void serialize_StatsResponse_Mean_t_array_into(const arena_vector<StatsResponse_Mean_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsResponse_Mean_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsResponse_Mean_t_array(const arena_vector<StatsResponse_Mean_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_StatsResponse_Mean_t_array_into(arr, json);
    return json;
}

inline void serialize_StatsResponse_Sum_t_array_into(const std::vector<StatsResponse_Sum_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsResponse_Sum_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsResponse_Sum_t_array(const std::vector<StatsResponse_Sum_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_StatsResponse_Sum_t_array_into(arr, json);
    return json;
}

inline void serialize_StatsResponse_Sum_t_array_into(const arena_vector<StatsResponse_Sum_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsResponse_Sum_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsResponse_Sum_t_array(const arena_vector<StatsResponse_Sum_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_StatsResponse_Sum_t_array_into(arr, json);
    return json;
}

inline void serialize_StatsResponse_Count_t_array_into(const std::vector<StatsResponse_Count_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsResponse_Count_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsResponse_Count_t_array(const std::vector<StatsResponse_Count_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_StatsResponse_Count_t_array_into(arr, json);
    return json;
}

inline void serialize_StatsResponse_Count_t_array_into(const arena_vector<StatsResponse_Count_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsResponse_Count_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsResponse_Count_t_array(const arena_vector<StatsResponse_Count_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_StatsResponse_Count_t_array_into(arr, json);
    return json;
}

inline void serialize_StatsResponse_Median_t_array_into(const std::vector<StatsResponse_Median_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsResponse_Median_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsResponse_Median_t_array(const std::vector<StatsResponse_Median_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_StatsResponse_Median_t_array_into(arr, json);
    return json;
}

inline void serialize_StatsResponse_Median_t_array_into(const arena_vector<StatsResponse_Median_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsResponse_Median_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsResponse_Median_t_array(const arena_vector<StatsResponse_Median_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_StatsResponse_Median_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterRequest_array_into(const std::vector<RegisterRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_array(const std::vector<RegisterRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 270 + 2);
    serialize_RegisterRequest_array_into(arr, json);
    return json;
}

inline void serialize_RegisterRequest_array_into(const arena_vector<RegisterRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_array(const arena_vector<RegisterRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 270 + 2);
    serialize_RegisterRequest_array_into(arr, json);
    return json;
}

inline void serialize_RegisterRequest_Username_t_array_into(const std::vector<RegisterRequest_Username_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_Username_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_Username_t_array(const std::vector<RegisterRequest_Username_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_RegisterRequest_Username_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterRequest_Username_t_array_into(const arena_vector<RegisterRequest_Username_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_Username_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_Username_t_array(const arena_vector<RegisterRequest_Username_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_RegisterRequest_Username_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterRequest_Email_t_array_into(const std::vector<RegisterRequest_Email_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_Email_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_Email_t_array(const std::vector<RegisterRequest_Email_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_RegisterRequest_Email_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterRequest_Email_t_array_into(const arena_vector<RegisterRequest_Email_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_Email_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_Email_t_array(const arena_vector<RegisterRequest_Email_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_RegisterRequest_Email_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterRequest_Password_t_array_into(const std::vector<RegisterRequest_Password_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_Password_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_Password_t_array(const std::vector<RegisterRequest_Password_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_RegisterRequest_Password_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterRequest_Password_t_array_into(const arena_vector<RegisterRequest_Password_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_Password_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_Password_t_array(const arena_vector<RegisterRequest_Password_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_RegisterRequest_Password_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterRequest_Age_t_array_into(const std::vector<RegisterRequest_Age_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_Age_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_Age_t_array(const std::vector<RegisterRequest_Age_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_RegisterRequest_Age_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterRequest_Age_t_array_into(const arena_vector<RegisterRequest_Age_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_Age_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_Age_t_array(const arena_vector<RegisterRequest_Age_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_RegisterRequest_Age_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterRequest_Role_t_array_into(const std::vector<RegisterRequest_Role_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_Role_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_Role_t_array(const std::vector<RegisterRequest_Role_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_RegisterRequest_Role_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterRequest_Role_t_array_into(const arena_vector<RegisterRequest_Role_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_Role_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_Role_t_array(const arena_vector<RegisterRequest_Role_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_RegisterRequest_Role_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterRequest_Tags_t_array_into(const std::vector<RegisterRequest_Tags_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_Tags_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_Tags_t_array(const std::vector<RegisterRequest_Tags_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 130 + 2);
    serialize_RegisterRequest_Tags_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterRequest_Tags_t_array_into(const arena_vector<RegisterRequest_Tags_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_Tags_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_Tags_t_array(const arena_vector<RegisterRequest_Tags_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 130 + 2);
    serialize_RegisterRequest_Tags_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterRequest_Item_t_array_into(const std::vector<RegisterRequest_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_Item_t_array(const std::vector<RegisterRequest_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_RegisterRequest_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_RegisterRequest_Item_t_array_into(const arena_vector<RegisterRequest_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_Item_t_array(const arena_vector<RegisterRequest_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_RegisterRequest_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_UserRole_array_into(const std::vector<UserRole>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserRole_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserRole_array(const std::vector<UserRole>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UserRole_array_into(arr, json);
    return json;
}

inline void serialize_UserRole_array_into(const arena_vector<UserRole>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserRole_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserRole_array(const arena_vector<UserRole>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UserRole_array_into(arr, json);
    return json;
}

inline void serialize_UserResponse_array_into(const std::vector<UserResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserResponse_array(const std::vector<UserResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 211 + 2);
    serialize_UserResponse_array_into(arr, json);
    return json;
}

inline void serialize_UserResponse_array_into(const arena_vector<UserResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserResponse_array(const arena_vector<UserResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 211 + 2);
    serialize_UserResponse_array_into(arr, json);
    return json;
}

inline void serialize_UserResponse_Id_t_array_into(const std::vector<UserResponse_Id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserResponse_Id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserResponse_Id_t_array(const std::vector<UserResponse_Id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UserResponse_Id_t_array_into(arr, json);
    return json;
}

inline void serialize_UserResponse_Id_t_array_into(const arena_vector<UserResponse_Id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserResponse_Id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserResponse_Id_t_array(const arena_vector<UserResponse_Id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UserResponse_Id_t_array_into(arr, json);
    return json;
}

inline void serialize_UserResponse_Username_t_array_into(const std::vector<UserResponse_Username_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserResponse_Username_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserResponse_Username_t_array(const std::vector<UserResponse_Username_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UserResponse_Username_t_array_into(arr, json);
    return json;
}

inline void serialize_UserResponse_Username_t_array_into(const arena_vector<UserResponse_Username_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserResponse_Username_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserResponse_Username_t_array(const arena_vector<UserResponse_Username_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UserResponse_Username_t_array_into(arr, json);
    return json;
}

inline void serialize_UserResponse_Email_t_array_into(const std::vector<UserResponse_Email_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserResponse_Email_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserResponse_Email_t_array(const std::vector<UserResponse_Email_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UserResponse_Email_t_array_into(arr, json);
    return json;
}

inline void serialize_UserResponse_Email_t_array_into(const arena_vector<UserResponse_Email_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserResponse_Email_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserResponse_Email_t_array(const arena_vector<UserResponse_Email_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UserResponse_Email_t_array_into(arr, json);
    return json;
}

inline void serialize_UserResponse_Role_t_array_into(const std::vector<UserResponse_Role_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserResponse_Role_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserResponse_Role_t_array(const std::vector<UserResponse_Role_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_UserResponse_Role_t_array_into(arr, json);
    return json;
}

inline void serialize_UserResponse_Role_t_array_into(const arena_vector<UserResponse_Role_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserResponse_Role_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserResponse_Role_t_array(const arena_vector<UserResponse_Role_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_UserResponse_Role_t_array_into(arr, json);
    return json;
}

inline void serialize_UserResponse_Created_at_t_array_into(const std::vector<UserResponse_Created_at_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserResponse_Created_at_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserResponse_Created_at_t_array(const std::vector<UserResponse_Created_at_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UserResponse_Created_at_t_array_into(arr, json);
    return json;
}

inline void serialize_UserResponse_Created_at_t_array_into(const arena_vector<UserResponse_Created_at_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserResponse_Created_at_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserResponse_Created_at_t_array(const arena_vector<UserResponse_Created_at_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UserResponse_Created_at_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemCategory_array_into(const std::vector<ItemCategory>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemCategory_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemCategory_array(const std::vector<ItemCategory>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ItemCategory_array_into(arr, json);
    return json;
}

inline void serialize_ItemCategory_array_into(const arena_vector<ItemCategory>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemCategory_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemCategory_array(const arena_vector<ItemCategory>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ItemCategory_array_into(arr, json);
    return json;
}

inline void serialize_Item_array_into(const std::vector<Item>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_array(const std::vector<Item>& arr) {
    std::string json;
    json.reserve(arr.size() * 434 + 2);
    serialize_Item_array_into(arr, json);
    return json;
}

inline void serialize_Item_array_into(const arena_vector<Item>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_array(const arena_vector<Item>& arr) {
    std::string json;
    json.reserve(arr.size() * 434 + 2);
    serialize_Item_array_into(arr, json);
    return json;
}

inline void serialize_Item_Id_t_array_into(const std::vector<Item_Id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Id_t_array(const std::vector<Item_Id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_Item_Id_t_array_into(arr, json);
    return json;
}

inline void serialize_Item_Id_t_array_into(const arena_vector<Item_Id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Id_t_array(const arena_vector<Item_Id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_Item_Id_t_array_into(arr, json);
    return json;
}

inline void serialize_Item_Name_t_array_into(const std::vector<Item_Name_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Name_t_array(const std::vector<Item_Name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Item_Name_t_array_into(arr, json);
    return json;
}

inline void serialize_Item_Name_t_array_into(const arena_vector<Item_Name_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Name_t_array(const arena_vector<Item_Name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Item_Name_t_array_into(arr, json);
    return json;
}

inline void serialize_Item_Description_t_array_into(const std::vector<Item_Description_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Description_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Description_t_array(const std::vector<Item_Description_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Item_Description_t_array_into(arr, json);
    return json;
}

inline void serialize_Item_Description_t_array_into(const arena_vector<Item_Description_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Description_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Description_t_array(const arena_vector<Item_Description_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Item_Description_t_array_into(arr, json);
    return json;
}

inline void serialize_Item_Price_t_array_into(const std::vector<Item_Price_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Price_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Price_t_array(const std::vector<Item_Price_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_Item_Price_t_array_into(arr, json);
    return json;
}

inline void serialize_Item_Price_t_array_into(const arena_vector<Item_Price_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Price_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Price_t_array(const arena_vector<Item_Price_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_Item_Price_t_array_into(arr, json);
    return json;
}

inline void serialize_Item_Stock_t_array_into(const std::vector<Item_Stock_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Stock_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Stock_t_array(const std::vector<Item_Stock_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_Item_Stock_t_array_into(arr, json);
    return json;
}

inline void serialize_Item_Stock_t_array_into(const arena_vector<Item_Stock_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Stock_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Stock_t_array(const arena_vector<Item_Stock_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_Item_Stock_t_array_into(arr, json);
    return json;
}

inline void serialize_Item_Category_t_array_into(const std::vector<Item_Category_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Category_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Category_t_array(const std::vector<Item_Category_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_Item_Category_t_array_into(arr, json);
    return json;
}

inline void serialize_Item_Category_t_array_into(const arena_vector<Item_Category_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Category_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Category_t_array(const arena_vector<Item_Category_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_Item_Category_t_array_into(arr, json);
    return json;
}

inline void serialize_Item_Tags_t_array_into(const std::vector<Item_Tags_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Tags_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Tags_t_array(const std::vector<Item_Tags_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 130 + 2);
    serialize_Item_Tags_t_array_into(arr, json);
    return json;
}

inline void serialize_Item_Tags_t_array_into(const arena_vector<Item_Tags_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Tags_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Tags_t_array(const arena_vector<Item_Tags_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 130 + 2);
    serialize_Item_Tags_t_array_into(arr, json);
    return json;
}

inline void serialize_Item_Item_t_array_into(const std::vector<Item_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Item_t_array(const std::vector<Item_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Item_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_Item_Item_t_array_into(const arena_vector<Item_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Item_t_array(const arena_vector<Item_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Item_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_Item_Metadata_t_array_into(const std::vector<Item_Metadata_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Metadata_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Metadata_t_array(const std::vector<Item_Metadata_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_Item_Metadata_t_array_into(arr, json);
    return json;
}

inline void serialize_Item_Metadata_t_array_into(const arena_vector<Item_Metadata_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_Metadata_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_Metadata_t_array(const arena_vector<Item_Metadata_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_Item_Metadata_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemMetadata_array_into(const std::vector<ItemMetadata>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemMetadata_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemMetadata_array(const std::vector<ItemMetadata>& arr) {
    std::string json;
    json.reserve(arr.size() * 173 + 2);
    serialize_ItemMetadata_array_into(arr, json);
    return json;
}

inline void serialize_ItemMetadata_array_into(const arena_vector<ItemMetadata>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemMetadata_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemMetadata_array(const arena_vector<ItemMetadata>& arr) {
    std::string json;
    json.reserve(arr.size() * 173 + 2);
    serialize_ItemMetadata_array_into(arr, json);
    return json;
}

inline void serialize_ItemMetadata_Weight_kg_t_array_into(const std::vector<ItemMetadata_Weight_kg_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemMetadata_Weight_kg_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemMetadata_Weight_kg_t_array(const std::vector<ItemMetadata_Weight_kg_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_ItemMetadata_Weight_kg_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemMetadata_Weight_kg_t_array_into(const arena_vector<ItemMetadata_Weight_kg_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemMetadata_Weight_kg_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemMetadata_Weight_kg_t_array(const arena_vector<ItemMetadata_Weight_kg_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_ItemMetadata_Weight_kg_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemMetadata_Dimensions_t_array_into(const std::vector<ItemMetadata_Dimensions_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemMetadata_Dimensions_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemMetadata_Dimensions_t_array(const std::vector<ItemMetadata_Dimensions_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ItemMetadata_Dimensions_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemMetadata_Dimensions_t_array_into(const arena_vector<ItemMetadata_Dimensions_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemMetadata_Dimensions_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemMetadata_Dimensions_t_array(const arena_vector<ItemMetadata_Dimensions_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ItemMetadata_Dimensions_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemMetadata_Manufacturer_t_array_into(const std::vector<ItemMetadata_Manufacturer_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemMetadata_Manufacturer_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemMetadata_Manufacturer_t_array(const std::vector<ItemMetadata_Manufacturer_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ItemMetadata_Manufacturer_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemMetadata_Manufacturer_t_array_into(const arena_vector<ItemMetadata_Manufacturer_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemMetadata_Manufacturer_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemMetadata_Manufacturer_t_array(const arena_vector<ItemMetadata_Manufacturer_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ItemMetadata_Manufacturer_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemMetadata_Sku_t_array_into(const std::vector<ItemMetadata_Sku_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemMetadata_Sku_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemMetadata_Sku_t_array(const std::vector<ItemMetadata_Sku_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ItemMetadata_Sku_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemMetadata_Sku_t_array_into(const arena_vector<ItemMetadata_Sku_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemMetadata_Sku_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemMetadata_Sku_t_array(const arena_vector<ItemMetadata_Sku_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ItemMetadata_Sku_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_array_into(const std::vector<CreateItemRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_array(const std::vector<CreateItemRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 408 + 2);
    serialize_CreateItemRequest_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_array_into(const arena_vector<CreateItemRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_array(const arena_vector<CreateItemRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 408 + 2);
    serialize_CreateItemRequest_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_Name_t_array_into(const std::vector<CreateItemRequest_Name_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_Name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_Name_t_array(const std::vector<CreateItemRequest_Name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CreateItemRequest_Name_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_Name_t_array_into(const arena_vector<CreateItemRequest_Name_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_Name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_Name_t_array(const arena_vector<CreateItemRequest_Name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CreateItemRequest_Name_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_Description_t_array_into(const std::vector<CreateItemRequest_Description_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_Description_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_Description_t_array(const std::vector<CreateItemRequest_Description_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CreateItemRequest_Description_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_Description_t_array_into(const arena_vector<CreateItemRequest_Description_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_Description_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_Description_t_array(const arena_vector<CreateItemRequest_Description_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CreateItemRequest_Description_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_Price_t_array_into(const std::vector<CreateItemRequest_Price_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_Price_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_Price_t_array(const std::vector<CreateItemRequest_Price_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_CreateItemRequest_Price_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_Price_t_array_into(const arena_vector<CreateItemRequest_Price_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_Price_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_Price_t_array(const arena_vector<CreateItemRequest_Price_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_CreateItemRequest_Price_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_Stock_t_array_into(const std::vector<CreateItemRequest_Stock_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_Stock_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_Stock_t_array(const std::vector<CreateItemRequest_Stock_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CreateItemRequest_Stock_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_Stock_t_array_into(const arena_vector<CreateItemRequest_Stock_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_Stock_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_Stock_t_array(const arena_vector<CreateItemRequest_Stock_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CreateItemRequest_Stock_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_Category_t_array_into(const std::vector<CreateItemRequest_Category_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_Category_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_Category_t_array(const std::vector<CreateItemRequest_Category_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_CreateItemRequest_Category_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_Category_t_array_into(const arena_vector<CreateItemRequest_Category_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_Category_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_Category_t_array(const arena_vector<CreateItemRequest_Category_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_CreateItemRequest_Category_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_Tags_t_array_into(const std::vector<CreateItemRequest_Tags_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_Tags_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_Tags_t_array(const std::vector<CreateItemRequest_Tags_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 130 + 2);
    serialize_CreateItemRequest_Tags_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_Tags_t_array_into(const arena_vector<CreateItemRequest_Tags_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_Tags_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_Tags_t_array(const arena_vector<CreateItemRequest_Tags_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 130 + 2);
    serialize_CreateItemRequest_Tags_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_Item_t_array_into(const std::vector<CreateItemRequest_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_Item_t_array(const std::vector<CreateItemRequest_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CreateItemRequest_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_Item_t_array_into(const arena_vector<CreateItemRequest_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_Item_t_array(const arena_vector<CreateItemRequest_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CreateItemRequest_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_Metadata_t_array_into(const std::vector<CreateItemRequest_Metadata_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_Metadata_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_Metadata_t_array(const std::vector<CreateItemRequest_Metadata_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_CreateItemRequest_Metadata_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_Metadata_t_array_into(const arena_vector<CreateItemRequest_Metadata_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_Metadata_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_Metadata_t_array(const arena_vector<CreateItemRequest_Metadata_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_CreateItemRequest_Metadata_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateItemRequest_array_into(const std::vector<UpdateItemRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_array(const std::vector<UpdateItemRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 268 + 2);
    serialize_UpdateItemRequest_array_into(arr, json);
    return json;
}

inline void serialize_UpdateItemRequest_array_into(const arena_vector<UpdateItemRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_array(const arena_vector<UpdateItemRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 268 + 2);
    serialize_UpdateItemRequest_array_into(arr, json);
    return json;
}

inline void serialize_UpdateItemRequest_Name_t_array_into(const std::vector<UpdateItemRequest_Name_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_Name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_Name_t_array(const std::vector<UpdateItemRequest_Name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UpdateItemRequest_Name_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateItemRequest_Name_t_array_into(const arena_vector<UpdateItemRequest_Name_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_Name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_Name_t_array(const arena_vector<UpdateItemRequest_Name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UpdateItemRequest_Name_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateItemRequest_Description_t_array_into(const std::vector<UpdateItemRequest_Description_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_Description_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_Description_t_array(const std::vector<UpdateItemRequest_Description_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UpdateItemRequest_Description_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateItemRequest_Description_t_array_into(const arena_vector<UpdateItemRequest_Description_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_Description_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_Description_t_array(const arena_vector<UpdateItemRequest_Description_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UpdateItemRequest_Description_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateItemRequest_Price_t_array_into(const std::vector<UpdateItemRequest_Price_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_Price_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_Price_t_array(const std::vector<UpdateItemRequest_Price_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_UpdateItemRequest_Price_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateItemRequest_Price_t_array_into(const arena_vector<UpdateItemRequest_Price_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_Price_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_Price_t_array(const arena_vector<UpdateItemRequest_Price_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 25 + 2);
    serialize_UpdateItemRequest_Price_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateItemRequest_Stock_t_array_into(const std::vector<UpdateItemRequest_Stock_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_Stock_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_Stock_t_array(const std::vector<UpdateItemRequest_Stock_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_UpdateItemRequest_Stock_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateItemRequest_Stock_t_array_into(const arena_vector<UpdateItemRequest_Stock_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_Stock_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_Stock_t_array(const arena_vector<UpdateItemRequest_Stock_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_UpdateItemRequest_Stock_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateItemRequest_Category_t_array_into(const std::vector<UpdateItemRequest_Category_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_Category_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_Category_t_array(const std::vector<UpdateItemRequest_Category_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_UpdateItemRequest_Category_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateItemRequest_Category_t_array_into(const arena_vector<UpdateItemRequest_Category_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_Category_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_Category_t_array(const arena_vector<UpdateItemRequest_Category_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_UpdateItemRequest_Category_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateItemRequest_Tags_t_array_into(const std::vector<UpdateItemRequest_Tags_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_Tags_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_Tags_t_array(const std::vector<UpdateItemRequest_Tags_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 130 + 2);
    serialize_UpdateItemRequest_Tags_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateItemRequest_Tags_t_array_into(const arena_vector<UpdateItemRequest_Tags_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_Tags_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_Tags_t_array(const arena_vector<UpdateItemRequest_Tags_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 130 + 2);
    serialize_UpdateItemRequest_Tags_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateItemRequest_Item_t_array_into(const std::vector<UpdateItemRequest_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_Item_t_array(const std::vector<UpdateItemRequest_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UpdateItemRequest_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateItemRequest_Item_t_array_into(const arena_vector<UpdateItemRequest_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_Item_t_array(const arena_vector<UpdateItemRequest_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UpdateItemRequest_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemList_array_into(const std::vector<ItemList>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemList_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemList_array(const std::vector<ItemList>& arr) {
    std::string json;
    json.reserve(arr.size() * 163 + 2);
    serialize_ItemList_array_into(arr, json);
    return json;
}

inline void serialize_ItemList_array_into(const arena_vector<ItemList>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemList_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemList_array(const arena_vector<ItemList>& arr) {
    std::string json;
    json.reserve(arr.size() * 163 + 2);
    serialize_ItemList_array_into(arr, json);
    return json;
}

inline void serialize_ItemList_Items_t_array_into(const std::vector<ItemList_Items_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemList_Items_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemList_Items_t_array(const std::vector<ItemList_Items_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 1738 + 2);
    serialize_ItemList_Items_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemList_Items_t_array_into(const arena_vector<ItemList_Items_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemList_Items_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemList_Items_t_array(const arena_vector<ItemList_Items_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 1738 + 2);
    serialize_ItemList_Items_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemList_Item_t_array_into(const std::vector<ItemList_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemList_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemList_Item_t_array(const std::vector<ItemList_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_ItemList_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemList_Item_t_array_into(const arena_vector<ItemList_Item_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemList_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemList_Item_t_array(const arena_vector<ItemList_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_ItemList_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemList_Total_t_array_into(const std::vector<ItemList_Total_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemList_Total_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemList_Total_t_array(const std::vector<ItemList_Total_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_ItemList_Total_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemList_Total_t_array_into(const arena_vector<ItemList_Total_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemList_Total_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemList_Total_t_array(const arena_vector<ItemList_Total_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_ItemList_Total_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemList_Limit_t_array_into(const std::vector<ItemList_Limit_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemList_Limit_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemList_Limit_t_array(const std::vector<ItemList_Limit_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_ItemList_Limit_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemList_Limit_t_array_into(const arena_vector<ItemList_Limit_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemList_Limit_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemList_Limit_t_array(const arena_vector<ItemList_Limit_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_ItemList_Limit_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemList_Offset_t_array_into(const std::vector<ItemList_Offset_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemList_Offset_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemList_Offset_t_array(const std::vector<ItemList_Offset_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_ItemList_Offset_t_array_into(arr, json);
    return json;
}

inline void serialize_ItemList_Offset_t_array_into(const arena_vector<ItemList_Offset_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemList_Offset_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemList_Offset_t_array(const arena_vector<ItemList_Offset_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_ItemList_Offset_t_array_into(arr, json);
    return json;
}

inline void serialize_EchoRequest_array_into(const std::vector<EchoRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_EchoRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_EchoRequest_array(const std::vector<EchoRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 93 + 2);
    serialize_EchoRequest_array_into(arr, json);
    return json;
}

inline void serialize_EchoRequest_array_into(const arena_vector<EchoRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_EchoRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_EchoRequest_array(const arena_vector<EchoRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 93 + 2);
    serialize_EchoRequest_array_into(arr, json);
    return json;
}

inline void serialize_EchoRequest_Message_t_array_into(const std::vector<EchoRequest_Message_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_EchoRequest_Message_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_EchoRequest_Message_t_array(const std::vector<EchoRequest_Message_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_EchoRequest_Message_t_array_into(arr, json);
    return json;
}

inline void serialize_EchoRequest_Message_t_array_into(const arena_vector<EchoRequest_Message_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_EchoRequest_Message_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_EchoRequest_Message_t_array(const arena_vector<EchoRequest_Message_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_EchoRequest_Message_t_array_into(arr, json);
    return json;
}

inline void serialize_EchoRequest_Repeat_t_array_into(const std::vector<EchoRequest_Repeat_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_EchoRequest_Repeat_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_EchoRequest_Repeat_t_array(const std::vector<EchoRequest_Repeat_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_EchoRequest_Repeat_t_array_into(arr, json);
    return json;
}

inline void serialize_EchoRequest_Repeat_t_array_into(const arena_vector<EchoRequest_Repeat_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_EchoRequest_Repeat_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_EchoRequest_Repeat_t_array(const arena_vector<EchoRequest_Repeat_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_EchoRequest_Repeat_t_array_into(arr, json);
    return json;
}

inline void serialize_EchoRequest_Uppercase_t_array_into(const std::vector<EchoRequest_Uppercase_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_EchoRequest_Uppercase_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_EchoRequest_Uppercase_t_array(const std::vector<EchoRequest_Uppercase_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 5 + 2);
    serialize_EchoRequest_Uppercase_t_array_into(arr, json);
    return json;
}

inline void serialize_EchoRequest_Uppercase_t_array_into(const arena_vector<EchoRequest_Uppercase_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_EchoRequest_Uppercase_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_EchoRequest_Uppercase_t_array(const arena_vector<EchoRequest_Uppercase_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 5 + 2);
    serialize_EchoRequest_Uppercase_t_array_into(arr, json);
    return json;
}

inline void serialize_EchoResponse_array_into(const std::vector<EchoResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_EchoResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_EchoResponse_array(const std::vector<EchoResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 75 + 2);
    serialize_EchoResponse_array_into(arr, json);
    return json;
}

inline void serialize_EchoResponse_array_into(const arena_vector<EchoResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_EchoResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_EchoResponse_array(const arena_vector<EchoResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 75 + 2);
    serialize_EchoResponse_array_into(arr, json);
    return json;
}

inline void serialize_EchoResponse_Message_t_array_into(const std::vector<EchoResponse_Message_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_EchoResponse_Message_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_EchoResponse_Message_t_array(const std::vector<EchoResponse_Message_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_EchoResponse_Message_t_array_into(arr, json);
    return json;
}

inline void serialize_EchoResponse_Message_t_array_into(const arena_vector<EchoResponse_Message_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_EchoResponse_Message_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_EchoResponse_Message_t_array(const arena_vector<EchoResponse_Message_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_EchoResponse_Message_t_array_into(arr, json);
    return json;
}

inline void serialize_EchoResponse_Length_t_array_into(const std::vector<EchoResponse_Length_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_EchoResponse_Length_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_EchoResponse_Length_t_array(const std::vector<EchoResponse_Length_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_EchoResponse_Length_t_array_into(arr, json);
    return json;
}

inline void serialize_EchoResponse_Length_t_array_into(const arena_vector<EchoResponse_Length_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_EchoResponse_Length_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_EchoResponse_Length_t_array(const arena_vector<EchoResponse_Length_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_EchoResponse_Length_t_array_into(arr, json);
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

inline void serialize_list_items_param_limit_array_into(const std::vector<list_items_param_limit>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_list_items_param_limit_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_list_items_param_limit_array(const std::vector<list_items_param_limit>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_list_items_param_limit_array_into(arr, json);
    return json;
}

inline void serialize_list_items_param_limit_array_into(const arena_vector<list_items_param_limit>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_list_items_param_limit_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_list_items_param_limit_array(const arena_vector<list_items_param_limit>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_list_items_param_limit_array_into(arr, json);
    return json;
}

inline void serialize_list_items_param_offset_array_into(const std::vector<list_items_param_offset>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_list_items_param_offset_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_list_items_param_offset_array(const std::vector<list_items_param_offset>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_list_items_param_offset_array_into(arr, json);
    return json;
}

inline void serialize_list_items_param_offset_array_into(const arena_vector<list_items_param_offset>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_list_items_param_offset_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_list_items_param_offset_array(const arena_vector<list_items_param_offset>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_list_items_param_offset_array_into(arr, json);
    return json;
}

inline void serialize_schema_6_array_into(const std::vector<schema_6>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_6_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_6_array(const std::vector<schema_6>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_6_array_into(arr, json);
    return json;
}

inline void serialize_schema_6_array_into(const arena_vector<schema_6>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_6_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_6_array(const arena_vector<schema_6>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_6_array_into(arr, json);
    return json;
}

inline void serialize_schema_7_array_into(const std::vector<schema_7>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_7_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_7_array(const std::vector<schema_7>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_7_array_into(arr, json);
    return json;
}

inline void serialize_schema_7_array_into(const arena_vector<schema_7>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_7_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_7_array(const arena_vector<schema_7>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_7_array_into(arr, json);
    return json;
}

inline void serialize_create_item_param_X_Request_Id_array_into(const std::vector<create_item_param_X_Request_Id>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_create_item_param_X_Request_Id_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_create_item_param_X_Request_Id_array(const std::vector<create_item_param_X_Request_Id>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_create_item_param_X_Request_Id_array_into(arr, json);
    return json;
}

inline void serialize_create_item_param_X_Request_Id_array_into(const arena_vector<create_item_param_X_Request_Id>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_create_item_param_X_Request_Id_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_create_item_param_X_Request_Id_array(const arena_vector<create_item_param_X_Request_Id>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_create_item_param_X_Request_Id_array_into(arr, json);
    return json;
}

inline void serialize_create_item_param_session_array_into(const std::vector<create_item_param_session>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_create_item_param_session_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_create_item_param_session_array(const std::vector<create_item_param_session>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_create_item_param_session_array_into(arr, json);
    return json;
}

inline void serialize_create_item_param_session_array_into(const arena_vector<create_item_param_session>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_create_item_param_session_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_create_item_param_session_array(const arena_vector<create_item_param_session>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_create_item_param_session_array_into(arr, json);
    return json;
}

inline void serialize_schema_8_array_into(const std::vector<schema_8>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_8_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_8_array(const std::vector<schema_8>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_8_array_into(arr, json);
    return json;
}

inline void serialize_schema_8_array_into(const arena_vector<schema_8>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_8_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_8_array(const arena_vector<schema_8>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_8_array_into(arr, json);
    return json;
}

inline void serialize_schema_9_array_into(const std::vector<schema_9>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_9_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_9_array(const std::vector<schema_9>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_9_array_into(arr, json);
    return json;
}

inline void serialize_schema_9_array_into(const arena_vector<schema_9>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_9_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_9_array(const arena_vector<schema_9>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_9_array_into(arr, json);
    return json;
}

inline void serialize_get_item_param_id_array_into(const std::vector<get_item_param_id>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_get_item_param_id_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_get_item_param_id_array(const std::vector<get_item_param_id>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_get_item_param_id_array_into(arr, json);
    return json;
}

inline void serialize_get_item_param_id_array_into(const arena_vector<get_item_param_id>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_get_item_param_id_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_get_item_param_id_array(const arena_vector<get_item_param_id>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_get_item_param_id_array_into(arr, json);
    return json;
}

inline void serialize_schema_10_array_into(const std::vector<schema_10>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_10_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_10_array(const std::vector<schema_10>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_10_array_into(arr, json);
    return json;
}

inline void serialize_schema_10_array_into(const arena_vector<schema_10>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_10_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_10_array(const arena_vector<schema_10>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_10_array_into(arr, json);
    return json;
}

inline void serialize_update_item_param_id_array_into(const std::vector<update_item_param_id>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_update_item_param_id_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_update_item_param_id_array(const std::vector<update_item_param_id>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_update_item_param_id_array_into(arr, json);
    return json;
}

inline void serialize_update_item_param_id_array_into(const arena_vector<update_item_param_id>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_update_item_param_id_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_update_item_param_id_array(const arena_vector<update_item_param_id>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_update_item_param_id_array_into(arr, json);
    return json;
}

inline void serialize_schema_11_array_into(const std::vector<schema_11>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_11_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_11_array(const std::vector<schema_11>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_11_array_into(arr, json);
    return json;
}

inline void serialize_schema_11_array_into(const arena_vector<schema_11>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_11_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_11_array(const arena_vector<schema_11>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_11_array_into(arr, json);
    return json;
}

inline void serialize_schema_12_array_into(const std::vector<schema_12>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_12_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_12_array(const std::vector<schema_12>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_12_array_into(arr, json);
    return json;
}

inline void serialize_schema_12_array_into(const arena_vector<schema_12>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_12_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_12_array(const arena_vector<schema_12>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_12_array_into(arr, json);
    return json;
}

inline void serialize_delete_item_param_id_array_into(const std::vector<delete_item_param_id>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_delete_item_param_id_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_delete_item_param_id_array(const std::vector<delete_item_param_id>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_delete_item_param_id_array_into(arr, json);
    return json;
}

inline void serialize_delete_item_param_id_array_into(const arena_vector<delete_item_param_id>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_delete_item_param_id_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_delete_item_param_id_array(const arena_vector<delete_item_param_id>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_delete_item_param_id_array_into(arr, json);
    return json;
}

inline void serialize_schema_13_array_into(const std::vector<schema_13>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_13_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_13_array(const std::vector<schema_13>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_13_array_into(arr, json);
    return json;
}

inline void serialize_schema_13_array_into(const arena_vector<schema_13>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_13_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_13_array(const arena_vector<schema_13>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_13_array_into(arr, json);
    return json;
}

inline void serialize_schema_14_array_into(const std::vector<schema_14>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_14_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_14_array(const std::vector<schema_14>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_14_array_into(arr, json);
    return json;
}

inline void serialize_schema_14_array_into(const arena_vector<schema_14>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_14_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_14_array(const arena_vector<schema_14>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_14_array_into(arr, json);
    return json;
}

inline void serialize_health_check_response_array_into(const std::vector<health_check_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_health_check_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_health_check_response_array(const std::vector<health_check_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 77 + 2);
    serialize_health_check_response_array_into(arr, json);
    return json;
}

inline void serialize_health_check_response_array_into(const arena_vector<health_check_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_health_check_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_health_check_response_array(const arena_vector<health_check_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 77 + 2);
    serialize_health_check_response_array_into(arr, json);
    return json;
}

inline void serialize_health_check_response_Status_array_into(const std::vector<health_check_response_Status>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_health_check_response_Status_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_health_check_response_Status_array(const std::vector<health_check_response_Status>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_health_check_response_Status_array_into(arr, json);
    return json;
}

inline void serialize_health_check_response_Status_array_into(const arena_vector<health_check_response_Status>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_health_check_response_Status_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_health_check_response_Status_array(const arena_vector<health_check_response_Status>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_health_check_response_Status_array_into(arr, json);
    return json;
}

inline void serialize_health_check_response_Uptime_ms_array_into(const std::vector<health_check_response_Uptime_ms>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_health_check_response_Uptime_ms_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_health_check_response_Uptime_ms_array(const std::vector<health_check_response_Uptime_ms>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_health_check_response_Uptime_ms_array_into(arr, json);
    return json;
}

inline void serialize_health_check_response_Uptime_ms_array_into(const arena_vector<health_check_response_Uptime_ms>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_health_check_response_Uptime_ms_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_health_check_response_Uptime_ms_array(const arena_vector<health_check_response_Uptime_ms>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_health_check_response_Uptime_ms_array_into(arr, json);
    return json;
}

