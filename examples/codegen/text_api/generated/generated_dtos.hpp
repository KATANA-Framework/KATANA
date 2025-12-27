// layer: flat
#pragma once

#include "katana/core/arena.hpp"
using katana::arena_allocator;
using katana::arena_string;
using katana::arena_vector;
using katana::monotonic_arena;

#include <cctype>
#include <optional>
#include <string_view>

enum class schema_11_enum { upper, lower, reverse, title };

inline std::string_view to_string(schema_11_enum e) {
    switch (e) {
    case schema_11_enum::upper:
        return "upper";
    case schema_11_enum::lower:
        return "lower";
    case schema_11_enum::reverse:
        return "reverse";
    case schema_11_enum::title:
        return "title";
    }
    return "";
}

inline std::optional<schema_11_enum> schema_11_enum_from_string(std::string_view s) {
    if (s == "upper")
        return schema_11_enum::upper;
    if (s == "lower")
        return schema_11_enum::lower;
    if (s == "reverse")
        return schema_11_enum::reverse;
    if (s == "title")
        return schema_11_enum::title;
    return std::nullopt;
}

struct text_uppercase_body_0 {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool TEXT_REQUIRED = true;
        static constexpr size_t TEXT_MIN_LENGTH = 1;
        static constexpr size_t TEXT_MAX_LENGTH = 10000;
    };

    static_assert(metadata::TEXT_MIN_LENGTH <= metadata::TEXT_MAX_LENGTH,
                  "text: min_length must be <= max_length");

    explicit text_uppercase_body_0(monotonic_arena* arena = nullptr)
        : arena_(arena), text(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> text;
};

using schema = arena_string<>;

struct text_uppercase_resp_200_0 {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool RESULT_REQUIRED = false;
    };

    explicit text_uppercase_resp_200_0(monotonic_arena* arena = nullptr)
        : arena_(arena), result(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> result;
};

using schema_1 = arena_string<>;

struct text_lowercase_body_0 {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool TEXT_REQUIRED = true;
        static constexpr size_t TEXT_MIN_LENGTH = 1;
        static constexpr size_t TEXT_MAX_LENGTH = 10000;
    };

    static_assert(metadata::TEXT_MIN_LENGTH <= metadata::TEXT_MAX_LENGTH,
                  "text: min_length must be <= max_length");

    explicit text_lowercase_body_0(monotonic_arena* arena = nullptr)
        : arena_(arena), text(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> text;
};

using schema_2 = arena_string<>;

struct text_lowercase_resp_200_0 {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool RESULT_REQUIRED = false;
    };

    explicit text_lowercase_resp_200_0(monotonic_arena* arena = nullptr)
        : arena_(arena), result(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> result;
};

using schema_3 = arena_string<>;

struct text_reverse_body_0 {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool TEXT_REQUIRED = true;
        static constexpr size_t TEXT_MIN_LENGTH = 1;
        static constexpr size_t TEXT_MAX_LENGTH = 10000;
    };

    static_assert(metadata::TEXT_MIN_LENGTH <= metadata::TEXT_MAX_LENGTH,
                  "text: min_length must be <= max_length");

    explicit text_reverse_body_0(monotonic_arena* arena = nullptr)
        : arena_(arena), text(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> text;
};

using schema_4 = arena_string<>;

struct text_reverse_resp_200_0 {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool RESULT_REQUIRED = false;
    };

    explicit text_reverse_resp_200_0(monotonic_arena* arena = nullptr)
        : arena_(arena), result(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> result;
};

using schema_5 = arena_string<>;

struct text_stats_body_0 {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool TEXT_REQUIRED = true;
        static constexpr size_t TEXT_MAX_LENGTH = 100000;
    };

    explicit text_stats_body_0(monotonic_arena* arena = nullptr)
        : arena_(arena), text(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> text;
};

using schema_6 = arena_string<>;

struct text_stats_resp_200_0 {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool CHARS_REQUIRED = false;
        static constexpr bool WORDS_REQUIRED = false;
        static constexpr bool LINES_REQUIRED = false;
    };

    explicit text_stats_resp_200_0(monotonic_arena* arena = nullptr) : arena_(arena) {}

    monotonic_arena* arena_;
    int64_t chars = {};
    int64_t words = {};
    int64_t lines = {};
};

using schema_7 = int64_t;

using schema_8 = int64_t;

using schema_9 = int64_t;

struct text_transform_body_0 {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool TEXT_REQUIRED = true;
        static constexpr size_t TEXT_MIN_LENGTH = 1;
        static constexpr size_t TEXT_MAX_LENGTH = 10000;
        static constexpr bool OPERATION_REQUIRED = true;
        static constexpr bool TRIM_REQUIRED = false;
    };

    static_assert(metadata::TEXT_MIN_LENGTH <= metadata::TEXT_MAX_LENGTH,
                  "text: min_length must be <= max_length");

    explicit text_transform_body_0(monotonic_arena* arena = nullptr)
        : arena_(arena), text(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> text;
    schema_11_enum operation;
    bool trim = {};
};

using schema_10 = arena_string<>;

using schema_11 = schema_11_enum;

using schema_12 = bool;

struct text_transform_resp_200_0 {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool ORIGINAL_LENGTH_REQUIRED = false;
        static constexpr bool RESULT_REQUIRED = false;
        static constexpr bool OPERATION_APPLIED_REQUIRED = false;
    };

    explicit text_transform_resp_200_0(monotonic_arena* arena = nullptr)
        : arena_(arena), result(arena_allocator<char>(arena)),
          operation_applied(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    int64_t original_length = {};
    arena_string<> result;
    arena_string<> operation_applied;
};

using schema_13 = int64_t;

using schema_14 = arena_string<>;

using schema_15 = arena_string<>;
