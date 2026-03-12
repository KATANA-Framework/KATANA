// layer: flat
// Auto-generated DTOs (Data Transfer Objects) from OpenAPI specification
//
// This file contains:
//   - Type definitions for request/response bodies
//   - Enum types with string conversion functions
//   - Compile-time metadata for validation constraints
//   - Zero-copy arena allocators for high performance
//
// All types include metadata structs with validation constraints:
//   - Required/optional flags
//   - String length constraints (min_length, max_length)
//   - Numeric constraints (minimum, maximum, exclusive bounds)
//   - Array constraints (min_items, max_items, uniqueness)
//
#pragma once

#include "katana/core/arena.hpp"
using katana::arena_allocator;
using katana::arena_string;
using katana::arena_vector;
using katana::monotonic_arena;

#include <cctype>
#include <optional>
#include <string_view>

#include <variant>

// ============================================================
// Enum Types
// ============================================================

/// Enum with 3 possible values
enum class UserRole_enum { user, moderator, admin };

inline std::string_view to_string(UserRole_enum e) {
    switch (e) {
    case UserRole_enum::user:
        return "user";
    case UserRole_enum::moderator:
        return "moderator";
    case UserRole_enum::admin:
        return "admin";
    }
    return "";
}

inline std::optional<UserRole_enum> UserRole_enum_from_string(std::string_view s) {
    if (s == "user")
        return UserRole_enum::user;
    if (s == "moderator")
        return UserRole_enum::moderator;
    if (s == "admin")
        return UserRole_enum::admin;
    return std::nullopt;
}

/// Enum with 6 possible values
enum class ItemCategory_enum { electronics, books, clothing, food, tools, other };

inline std::string_view to_string(ItemCategory_enum e) {
    switch (e) {
    case ItemCategory_enum::electronics:
        return "electronics";
    case ItemCategory_enum::books:
        return "books";
    case ItemCategory_enum::clothing:
        return "clothing";
    case ItemCategory_enum::food:
        return "food";
    case ItemCategory_enum::tools:
        return "tools";
    case ItemCategory_enum::other:
        return "other";
    }
    return "";
}

inline std::optional<ItemCategory_enum> ItemCategory_enum_from_string(std::string_view s) {
    if (s == "electronics")
        return ItemCategory_enum::electronics;
    if (s == "books")
        return ItemCategory_enum::books;
    if (s == "clothing")
        return ItemCategory_enum::clothing;
    if (s == "food")
        return ItemCategory_enum::food;
    if (s == "tools")
        return ItemCategory_enum::tools;
    if (s == "other")
        return ItemCategory_enum::other;
    return std::nullopt;
}

// ============================================================
// Data Transfer Objects (DTOs)
// ============================================================

/// Data type with 1 fields
struct SumRequest {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool VALUES_REQUIRED = true;
        static constexpr size_t VALUES_MIN_ITEMS = 1;
        static constexpr size_t VALUES_MAX_ITEMS = 10000;
    };

    static_assert(metadata::VALUES_MIN_ITEMS <= metadata::VALUES_MAX_ITEMS,
                  "values: min_items must be <= max_items");

    explicit SumRequest(monotonic_arena* arena = nullptr)
        : arena_(arena), values(arena_allocator<double>(arena)) {}

    monotonic_arena* arena_;
    arena_vector<double> values;
};

using SumRequest_Values_t = arena_vector<double, 8>;

using SumRequest_Item_t = double;

/// Data type with 2 fields
struct SumResponse {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool RESULT_REQUIRED = true;
        static constexpr bool COUNT_REQUIRED = true;
    };

    explicit SumResponse(monotonic_arena* arena = nullptr) : arena_(arena) {}

    monotonic_arena* arena_;
    double result;
    int64_t count;
};

using SumResponse_Result_t = double;

using SumResponse_Count_t = int64_t;

/// Data type with 2 fields
struct StatsRequest {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool VALUES_REQUIRED = true;
        static constexpr size_t VALUES_MIN_ITEMS = 1;
        static constexpr size_t VALUES_MAX_ITEMS = 10000;
        static constexpr bool INCLUDE_MEDIAN_REQUIRED = false;
    };

    static_assert(metadata::VALUES_MIN_ITEMS <= metadata::VALUES_MAX_ITEMS,
                  "values: min_items must be <= max_items");

    explicit StatsRequest(monotonic_arena* arena = nullptr)
        : arena_(arena), values(arena_allocator<double>(arena)) {}

    monotonic_arena* arena_;
    arena_vector<double> values;
    /// Optional field
    std::optional<bool> include_median;
};

using StatsRequest_Values_t = arena_vector<double, 8>;

using StatsRequest_Item_t = double;

using StatsRequest_Include_median_t = bool;

/// Data type with 6 fields
struct StatsResponse {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool MIN_REQUIRED = false;
        static constexpr bool MAX_REQUIRED = false;
        static constexpr bool MEAN_REQUIRED = false;
        static constexpr bool SUM_REQUIRED = false;
        static constexpr bool COUNT_REQUIRED = false;
        static constexpr bool MEDIAN_REQUIRED = false;
    };

    explicit StatsResponse(monotonic_arena* arena = nullptr) : arena_(arena) {}

    monotonic_arena* arena_;
    /// Optional field
    std::optional<double> min;
    /// Optional field
    std::optional<double> max;
    /// Optional field
    std::optional<double> mean;
    /// Optional field
    std::optional<double> sum;
    /// Optional field
    std::optional<int64_t> count;
    /// Optional field
    std::optional<double> median;
};

using StatsResponse_Min_t = double;

using StatsResponse_Max_t = double;

using StatsResponse_Mean_t = double;

using StatsResponse_Sum_t = double;

using StatsResponse_Count_t = int64_t;

using StatsResponse_Median_t = double;

/// Data type with 6 fields
struct RegisterRequest {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool USERNAME_REQUIRED = true;
        static constexpr size_t USERNAME_MIN_LENGTH = 3;
        static constexpr size_t USERNAME_MAX_LENGTH = 32;
        static constexpr std::string_view USERNAME_PATTERN = "^[a-zA-Z0-9_-]+$";
        static constexpr bool EMAIL_REQUIRED = true;
        static constexpr bool PASSWORD_REQUIRED = true;
        static constexpr size_t PASSWORD_MIN_LENGTH = 8;
        static constexpr size_t PASSWORD_MAX_LENGTH = 128;
        static constexpr bool AGE_REQUIRED = false;
        static constexpr double AGE_MINIMUM = 13;
        static constexpr double AGE_MAXIMUM = 150;
        static constexpr bool ROLE_REQUIRED = false;
        static constexpr bool TAGS_REQUIRED = false;
        static constexpr size_t TAGS_MAX_ITEMS = 10;
        static constexpr bool TAGS_UNIQUE_ITEMS = true;
    };

    static_assert(metadata::USERNAME_MIN_LENGTH <= metadata::USERNAME_MAX_LENGTH,
                  "username: min_length must be <= max_length");
    static_assert(metadata::PASSWORD_MIN_LENGTH <= metadata::PASSWORD_MAX_LENGTH,
                  "password: min_length must be <= max_length");
    static_assert(metadata::AGE_MINIMUM <= metadata::AGE_MAXIMUM,
                  "age: minimum must be <= maximum");

    explicit RegisterRequest(monotonic_arena* arena = nullptr)
        : arena_(arena), username(arena_allocator<char>(arena)),
          email(arena_allocator<char>(arena)), password(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> username;
    arena_string<> email;
    arena_string<> password;
    /// Optional field
    std::optional<int64_t> age;
    /// Optional field
    std::optional<UserRole_enum> role;
    /// Optional field
    std::optional<arena_vector<arena_string<>>> tags;
};

using RegisterRequest_Username_t = arena_string<>;

using RegisterRequest_Email_t = arena_string<>;

using RegisterRequest_Password_t = arena_string<>;

using RegisterRequest_Age_t = int64_t;

using RegisterRequest_Role_t = std::monostate;

using RegisterRequest_Tags_t = arena_vector<arena_string<>>;

using RegisterRequest_Item_t = arena_string<>;

using UserRole = UserRole_enum;

/// Data type with 5 fields
struct UserResponse {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool ID_REQUIRED = true;
        static constexpr bool USERNAME_REQUIRED = true;
        static constexpr bool EMAIL_REQUIRED = true;
        static constexpr bool ROLE_REQUIRED = true;
        static constexpr bool CREATED_AT_REQUIRED = true;
    };

    explicit UserResponse(monotonic_arena* arena = nullptr)
        : arena_(arena), id(arena_allocator<char>(arena)), username(arena_allocator<char>(arena)),
          email(arena_allocator<char>(arena)), created_at(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> id;
    arena_string<> username;
    arena_string<> email;
    UserRole_enum role;
    arena_string<> created_at;
};

using UserResponse_Id_t = arena_string<>;

using UserResponse_Username_t = arena_string<>;

using UserResponse_Email_t = arena_string<>;

using UserResponse_Role_t = std::monostate;

using UserResponse_Created_at_t = arena_string<>;

using ItemCategory = ItemCategory_enum;

using Item_Id_t = int64_t;

using Item_Name_t = arena_string<>;

using Item_Description_t = arena_string<>;

using Item_Price_t = double;

using Item_Stock_t = int64_t;

using Item_Category_t = std::monostate;

using Item_Tags_t = arena_vector<arena_string<>>;

using Item_Item_t = arena_string<>;

using Item_Metadata_t = std::monostate;

/// Data type with 4 fields
struct ItemMetadata {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool WEIGHT_KG_REQUIRED = false;
        static constexpr bool DIMENSIONS_REQUIRED = false;
        static constexpr bool MANUFACTURER_REQUIRED = false;
        static constexpr bool SKU_REQUIRED = false;
    };

    explicit ItemMetadata(monotonic_arena* arena = nullptr) : arena_(arena) {}

    monotonic_arena* arena_;
    /// Optional field
    std::optional<double> weight_kg;
    /// Optional field
    std::optional<arena_string<>> dimensions;
    /// Optional field
    std::optional<arena_string<>> manufacturer;
    /// Optional field
    std::optional<arena_string<>> sku;
};

using ItemMetadata_Weight_kg_t = double;

using ItemMetadata_Dimensions_t = arena_string<>;

using ItemMetadata_Manufacturer_t = arena_string<>;

using ItemMetadata_Sku_t = arena_string<>;

using CreateItemRequest_Name_t = arena_string<>;

using CreateItemRequest_Description_t = arena_string<>;

using CreateItemRequest_Price_t = double;

using CreateItemRequest_Stock_t = int64_t;

using CreateItemRequest_Category_t = std::monostate;

using CreateItemRequest_Tags_t = arena_vector<arena_string<>>;

using CreateItemRequest_Item_t = arena_string<>;

using CreateItemRequest_Metadata_t = std::monostate;

/// Data type with 6 fields
struct UpdateItemRequest {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool NAME_REQUIRED = false;
        static constexpr size_t NAME_MIN_LENGTH = 1;
        static constexpr size_t NAME_MAX_LENGTH = 200;
        static constexpr bool DESCRIPTION_REQUIRED = false;
        static constexpr size_t DESCRIPTION_MAX_LENGTH = 2000;
        static constexpr bool PRICE_REQUIRED = false;
        static constexpr double PRICE_MINIMUM = 0;
        static constexpr bool STOCK_REQUIRED = false;
        static constexpr double STOCK_MINIMUM = 0;
        static constexpr bool CATEGORY_REQUIRED = false;
        static constexpr bool TAGS_REQUIRED = false;
        static constexpr size_t TAGS_MAX_ITEMS = 20;
    };

    static_assert(metadata::NAME_MIN_LENGTH <= metadata::NAME_MAX_LENGTH,
                  "name: min_length must be <= max_length");

    explicit UpdateItemRequest(monotonic_arena* arena = nullptr) : arena_(arena) {}

    monotonic_arena* arena_;
    /// Optional field
    std::optional<arena_string<>> name;
    /// Optional field
    std::optional<arena_string<>> description;
    /// Optional field
    std::optional<double> price;
    /// Optional field
    std::optional<int64_t> stock;
    /// Optional field
    std::optional<ItemCategory_enum> category;
    /// Optional field
    std::optional<arena_vector<arena_string<>>> tags;
};

using UpdateItemRequest_Name_t = arena_string<>;

using UpdateItemRequest_Description_t = arena_string<>;

using UpdateItemRequest_Price_t = double;

using UpdateItemRequest_Stock_t = int64_t;

using UpdateItemRequest_Category_t = std::monostate;

using UpdateItemRequest_Tags_t = arena_vector<arena_string<>>;

using UpdateItemRequest_Item_t = arena_string<>;

using ItemList_Item_t = std::monostate;

using ItemList_Total_t = int64_t;

using ItemList_Limit_t = int64_t;

using ItemList_Offset_t = int64_t;

/// Data type with 3 fields
struct EchoRequest {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool MESSAGE_REQUIRED = true;
        static constexpr size_t MESSAGE_MAX_LENGTH = 4096;
        static constexpr bool REPEAT_REQUIRED = false;
        static constexpr double REPEAT_MINIMUM = 1;
        static constexpr double REPEAT_MAXIMUM = 100;
        static constexpr bool UPPERCASE_REQUIRED = false;
    };

    static_assert(metadata::REPEAT_MINIMUM <= metadata::REPEAT_MAXIMUM,
                  "repeat: minimum must be <= maximum");

    explicit EchoRequest(monotonic_arena* arena = nullptr)
        : arena_(arena), message(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> message;
    /// Optional field
    std::optional<int64_t> repeat;
    /// Optional field
    std::optional<bool> uppercase;
};

using EchoRequest_Message_t = arena_string<>;

using EchoRequest_Repeat_t = int64_t;

using EchoRequest_Uppercase_t = bool;

/// Data type with 2 fields
struct EchoResponse {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool MESSAGE_REQUIRED = true;
        static constexpr bool LENGTH_REQUIRED = false;
    };

    explicit EchoResponse(monotonic_arena* arena = nullptr)
        : arena_(arena), message(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> message;
    /// Optional field
    std::optional<int64_t> length;
};

using EchoResponse_Message_t = arena_string<>;

using EchoResponse_Length_t = int64_t;

using schema = std::monostate;

using schema_1 = std::monostate;

using schema_2 = std::monostate;

using schema_3 = std::monostate;

using schema_4 = std::monostate;

using schema_5 = std::monostate;

using list_items_param_limit = int64_t;

using list_items_param_offset = int64_t;

using schema_6 = std::monostate;

using schema_7 = std::monostate;

using create_item_param_X_Request_Id = arena_string<>;

using create_item_param_session = arena_string<>;

using schema_8 = std::monostate;

using schema_9 = std::monostate;

using get_item_param_id = int64_t;

using schema_10 = std::monostate;

using update_item_param_id = int64_t;

using schema_11 = std::monostate;

using schema_12 = std::monostate;

using delete_item_param_id = int64_t;

using schema_13 = std::monostate;

using schema_14 = std::monostate;

/// Response body type with 2 fields
struct health_check_response {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool STATUS_REQUIRED = false;
        static constexpr bool UPTIME_MS_REQUIRED = false;
    };

    explicit health_check_response(monotonic_arena* arena = nullptr) : arena_(arena) {}

    monotonic_arena* arena_;
    /// Optional field
    std::optional<arena_string<>> status;
    /// Optional field
    std::optional<int64_t> uptime_ms;
};

using health_check_response_Status = arena_string<>;

using health_check_response_Uptime_ms = int64_t;

/// Data type with 8 fields
struct Item {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool ID_REQUIRED = true;
        static constexpr bool NAME_REQUIRED = true;
        static constexpr bool DESCRIPTION_REQUIRED = false;
        static constexpr bool PRICE_REQUIRED = true;
        static constexpr bool STOCK_REQUIRED = false;
        static constexpr bool CATEGORY_REQUIRED = true;
        static constexpr bool TAGS_REQUIRED = false;
        static constexpr bool METADATA_REQUIRED = false;
    };

    explicit Item(monotonic_arena* arena = nullptr)
        : arena_(arena), name(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    int64_t id;
    arena_string<> name;
    /// Optional field
    std::optional<arena_string<>> description;
    double price;
    /// Optional field
    std::optional<int64_t> stock;
    ItemCategory_enum category;
    /// Optional field
    std::optional<arena_vector<arena_string<>>> tags;
    /// Optional field
    std::optional<ItemMetadata> metadata;
};

/// Data type with 7 fields
struct CreateItemRequest {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool NAME_REQUIRED = true;
        static constexpr size_t NAME_MIN_LENGTH = 1;
        static constexpr size_t NAME_MAX_LENGTH = 200;
        static constexpr bool DESCRIPTION_REQUIRED = false;
        static constexpr size_t DESCRIPTION_MAX_LENGTH = 2000;
        static constexpr bool PRICE_REQUIRED = true;
        static constexpr double PRICE_MINIMUM = 0;
        static constexpr double PRICE_EXCLUSIVE_MAXIMUM = 1e+06;
        static constexpr bool STOCK_REQUIRED = false;
        static constexpr double STOCK_MINIMUM = 0;
        static constexpr double STOCK_MAXIMUM = 999999;
        static constexpr bool CATEGORY_REQUIRED = true;
        static constexpr bool TAGS_REQUIRED = false;
        static constexpr size_t TAGS_MAX_ITEMS = 20;
        static constexpr bool METADATA_REQUIRED = false;
    };

    static_assert(metadata::NAME_MIN_LENGTH <= metadata::NAME_MAX_LENGTH,
                  "name: min_length must be <= max_length");
    static_assert(metadata::STOCK_MINIMUM <= metadata::STOCK_MAXIMUM,
                  "stock: minimum must be <= maximum");

    explicit CreateItemRequest(monotonic_arena* arena = nullptr)
        : arena_(arena), name(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> name;
    /// Optional field
    std::optional<arena_string<>> description;
    double price;
    /// Optional field
    std::optional<int64_t> stock;
    ItemCategory_enum category;
    /// Optional field
    std::optional<arena_vector<arena_string<>>> tags;
    /// Optional field
    std::optional<ItemMetadata> metadata;
};

/// Data type with 4 fields
struct ItemList {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool ITEMS_REQUIRED = true;
        static constexpr bool TOTAL_REQUIRED = true;
        static constexpr bool LIMIT_REQUIRED = false;
        static constexpr bool OFFSET_REQUIRED = false;
    };

    explicit ItemList(monotonic_arena* arena = nullptr)
        : arena_(arena), items(arena_allocator<Item>(arena)) {}

    monotonic_arena* arena_;
    arena_vector<Item> items;
    int64_t total;
    /// Optional field
    std::optional<int64_t> limit;
    /// Optional field
    std::optional<int64_t> offset;
};

using ItemList_Items_t = arena_vector<Item>;
