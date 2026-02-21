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

#include <optional>
#include <string_view>
#include <cctype>

#include <variant>

// ============================================================
// Enum Types
// ============================================================

/// Enum with 4 possible values
enum class text_transform_operation_enum {
    upper,
    lower,
    reverse,
    title
};

inline std::string_view to_string(text_transform_operation_enum e) {
    switch (e) {
    case text_transform_operation_enum::upper: return "upper";
    case text_transform_operation_enum::lower: return "lower";
    case text_transform_operation_enum::reverse: return "reverse";
    case text_transform_operation_enum::title: return "title";
    }
    return "";
}

inline std::optional<text_transform_operation_enum> text_transform_operation_enum_from_string(std::string_view s) {
    if (s == "upper") return text_transform_operation_enum::upper;
    if (s == "lower") return text_transform_operation_enum::lower;
    if (s == "reverse") return text_transform_operation_enum::reverse;
    if (s == "title") return text_transform_operation_enum::title;
    return std::nullopt;
}

// ============================================================
// Data Transfer Objects (DTOs)
// ============================================================

/// Request body type with 1 fields
struct text_uppercase_request {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool TEXT_REQUIRED = true;
        static constexpr size_t TEXT_MIN_LENGTH = 1;
        static constexpr size_t TEXT_MAX_LENGTH = 10000;
    };

    static_assert(metadata::TEXT_MIN_LENGTH <= metadata::TEXT_MAX_LENGTH, "text: min_length must be <= max_length");

    explicit text_uppercase_request(monotonic_arena* arena = nullptr)
        : arena_(arena),
          text(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> text;
};

using text_uppercase_request_Text = arena_string<>;

/// Response body type with 1 fields
struct text_uppercase_response {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool RESULT_REQUIRED = false;
    };


    explicit text_uppercase_response(monotonic_arena* arena = nullptr)
        : arena_(arena),
          result(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    /// Optional field
    arena_string<> result;
};

using text_uppercase_response_Result = arena_string<>;

/// Request body type with 1 fields
struct text_lowercase_request {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool TEXT_REQUIRED = true;
        static constexpr size_t TEXT_MIN_LENGTH = 1;
        static constexpr size_t TEXT_MAX_LENGTH = 10000;
    };

    static_assert(metadata::TEXT_MIN_LENGTH <= metadata::TEXT_MAX_LENGTH, "text: min_length must be <= max_length");

    explicit text_lowercase_request(monotonic_arena* arena = nullptr)
        : arena_(arena),
          text(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> text;
};

using text_lowercase_request_Text = arena_string<>;

/// Response body type with 1 fields
struct text_lowercase_response {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool RESULT_REQUIRED = false;
    };


    explicit text_lowercase_response(monotonic_arena* arena = nullptr)
        : arena_(arena),
          result(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    /// Optional field
    arena_string<> result;
};

using text_lowercase_response_Result = arena_string<>;

/// Request body type with 1 fields
struct text_reverse_request {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool TEXT_REQUIRED = true;
        static constexpr size_t TEXT_MIN_LENGTH = 1;
        static constexpr size_t TEXT_MAX_LENGTH = 10000;
    };

    static_assert(metadata::TEXT_MIN_LENGTH <= metadata::TEXT_MAX_LENGTH, "text: min_length must be <= max_length");

    explicit text_reverse_request(monotonic_arena* arena = nullptr)
        : arena_(arena),
          text(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> text;
};

using text_reverse_request_Text = arena_string<>;

/// Response body type with 1 fields
struct text_reverse_response {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool RESULT_REQUIRED = false;
    };


    explicit text_reverse_response(monotonic_arena* arena = nullptr)
        : arena_(arena),
          result(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    /// Optional field
    arena_string<> result;
};

using text_reverse_response_Result = arena_string<>;

/// Request body type with 1 fields
struct text_stats_request {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool TEXT_REQUIRED = true;
        static constexpr size_t TEXT_MAX_LENGTH = 100000;
    };


    explicit text_stats_request(monotonic_arena* arena = nullptr)
        : arena_(arena),
          text(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> text;
};

using text_stats_request_Text = arena_string<>;

/// Response body type with 3 fields
struct text_stats_response {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool CHARS_REQUIRED = false;
        static constexpr bool WORDS_REQUIRED = false;
        static constexpr bool LINES_REQUIRED = false;
    };


    explicit text_stats_response(monotonic_arena* arena = nullptr)
        : arena_(arena) {}

    monotonic_arena* arena_;
    /// Optional field
    int64_t chars = {};
    /// Optional field
    int64_t words = {};
    /// Optional field
    int64_t lines = {};
};

using text_stats_response_Chars = int64_t;

using text_stats_response_Words = int64_t;

using text_stats_response_Lines = int64_t;

/// Request body type with 3 fields
struct text_transform_request {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool TEXT_REQUIRED = true;
        static constexpr size_t TEXT_MIN_LENGTH = 1;
        static constexpr size_t TEXT_MAX_LENGTH = 10000;
        static constexpr bool OPERATION_REQUIRED = true;
        static constexpr bool TRIM_REQUIRED = false;
    };

    static_assert(metadata::TEXT_MIN_LENGTH <= metadata::TEXT_MAX_LENGTH, "text: min_length must be <= max_length");

    explicit text_transform_request(monotonic_arena* arena = nullptr)
        : arena_(arena),
          text(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> text;
    text_transform_operation_enum operation;
    /// Optional field
    bool trim = {};
};

using text_transform_request_Text = arena_string<>;

using text_transform_operation = text_transform_operation_enum;

using text_transform_request_Trim = bool;

/// Response body type with 3 fields
struct text_transform_response {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool ORIGINAL_LENGTH_REQUIRED = false;
        static constexpr bool RESULT_REQUIRED = false;
        static constexpr bool OPERATION_APPLIED_REQUIRED = false;
    };


    explicit text_transform_response(monotonic_arena* arena = nullptr)
        : arena_(arena),
          result(arena_allocator<char>(arena)),
          operation_applied(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    /// Optional field
    int64_t original_length = {};
    /// Optional field
    arena_string<> result;
    /// Optional field
    arena_string<> operation_applied;
};

using text_transform_response_Original_length = int64_t;

using text_transform_response_Result = arena_string<>;

using text_transform_response_Operation_applied = arena_string<>;

