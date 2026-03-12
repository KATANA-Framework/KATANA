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

// ============================================================
// Data Transfer Objects (DTOs)
// ============================================================

/// Data type with 3 fields
struct UserInput {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool NAME_REQUIRED = true;
        static constexpr size_t NAME_MIN_LENGTH = 1;
        static constexpr bool EMAIL_REQUIRED = true;
        static constexpr bool AGE_REQUIRED = false;
        static constexpr double AGE_MINIMUM = 0;
    };


    explicit UserInput(monotonic_arena* arena = nullptr)
        : arena_(arena),
          name(arena_allocator<char>(arena)),
          email(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> name;
    arena_string<> email;
    /// Optional field
    int64_t age = {};
};

using UserInput_Name_t = arena_string<>;

using UserInput_Email_t = arena_string<>;

using UserInput_Age_t = int64_t;

using schema = std::monostate;

using getUser_param_id = int64_t;

using schema_1 = std::monostate;

