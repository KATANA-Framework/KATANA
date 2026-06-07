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

// text_uppercase_request — object, 1 field(s)  ← api.yaml:18
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

// text_uppercase_request_Text_t — string, field text_uppercase_request.text  ← api.yaml:22
using text_uppercase_request_Text_t = arena_string<>;

// text_uppercase_response — object, 1 field(s)  ← api.yaml:31
/// Response body type with 1 fields
struct text_uppercase_response {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool RESULT_REQUIRED = false;
    };


    explicit text_uppercase_response(monotonic_arena* arena = nullptr)
        : arena_(arena) {}

    monotonic_arena* arena_;
    /// Optional field
    std::optional<arena_string<>> result;
};

// text_uppercase_response_Result_t — string, field text_uppercase_response.result  ← api.yaml:34
using text_uppercase_response_Result_t = arena_string<>;

// text_lowercase_request — object, 1 field(s)  ← api.yaml:45
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

// text_lowercase_request_Text_t — string, field text_lowercase_request.text  ← api.yaml:49
using text_lowercase_request_Text_t = arena_string<>;

// text_lowercase_response — object, 1 field(s)  ← api.yaml:58
/// Response body type with 1 fields
struct text_lowercase_response {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool RESULT_REQUIRED = false;
    };


    explicit text_lowercase_response(monotonic_arena* arena = nullptr)
        : arena_(arena) {}

    monotonic_arena* arena_;
    /// Optional field
    std::optional<arena_string<>> result;
};

// text_lowercase_response_Result_t — string, field text_lowercase_response.result  ← api.yaml:61
using text_lowercase_response_Result_t = arena_string<>;

// text_reverse_request — object, 1 field(s)  ← api.yaml:72
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

// text_reverse_request_Text_t — string, field text_reverse_request.text  ← api.yaml:76
using text_reverse_request_Text_t = arena_string<>;

// text_reverse_response — object, 1 field(s)  ← api.yaml:85
/// Response body type with 1 fields
struct text_reverse_response {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool RESULT_REQUIRED = false;
    };


    explicit text_reverse_response(monotonic_arena* arena = nullptr)
        : arena_(arena) {}

    monotonic_arena* arena_;
    /// Optional field
    std::optional<arena_string<>> result;
};

// text_reverse_response_Result_t — string, field text_reverse_response.result  ← api.yaml:88
using text_reverse_response_Result_t = arena_string<>;

// text_stats_request — object, 1 field(s)  ← api.yaml:100
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

// text_stats_request_Text_t — string, field text_stats_request.text  ← api.yaml:104
using text_stats_request_Text_t = arena_string<>;

// text_stats_response — object, 3 field(s)  ← api.yaml:112
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
    std::optional<int64_t> chars;
    /// Optional field
    std::optional<int64_t> words;
    /// Optional field
    std::optional<int64_t> lines;
};

// text_stats_response_Chars_t — integer, field text_stats_response.chars  ← api.yaml:115
using text_stats_response_Chars_t = int64_t;

// text_stats_response_Words_t — integer, field text_stats_response.words  ← api.yaml:118
using text_stats_response_Words_t = int64_t;

// text_stats_response_Lines_t — integer, field text_stats_response.lines  ← api.yaml:121
using text_stats_response_Lines_t = int64_t;

// text_transform_request — object, 3 field(s)  ← api.yaml:134
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
    std::optional<bool> trim;
};

// text_transform_request_Text_t — string, field text_transform_request.text  ← api.yaml:138
using text_transform_request_Text_t = arena_string<>;

// text_transform_operation — enum, field text_transform_request.operation  ← api.yaml:142
using text_transform_operation = text_transform_operation_enum;

// text_transform_request_Trim_t — boolean, field text_transform_request.trim  ← api.yaml:145
using text_transform_request_Trim_t = bool;

// text_transform_response — object, 3 field(s)  ← api.yaml:153
/// Response body type with 3 fields
struct text_transform_response {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool ORIGINAL_LENGTH_REQUIRED = false;
        static constexpr bool RESULT_REQUIRED = false;
        static constexpr bool OPERATION_APPLIED_REQUIRED = false;
    };


    explicit text_transform_response(monotonic_arena* arena = nullptr)
        : arena_(arena) {}

    monotonic_arena* arena_;
    /// Optional field
    std::optional<int64_t> original_length;
    /// Optional field
    std::optional<arena_string<>> result;
    /// Optional field
    std::optional<arena_string<>> operation_applied;
};

// text_transform_response_Original_length_t — integer, field text_transform_response.original_length  ← api.yaml:156
using text_transform_response_Original_length_t = int64_t;

// text_transform_response_Result_t — string, field text_transform_response.result  ← api.yaml:158
using text_transform_response_Result_t = arena_string<>;

// text_transform_response_Operation_applied_t — string, field text_transform_response.operation_applied  ← api.yaml:160
using text_transform_response_Operation_applied_t = arena_string<>;

