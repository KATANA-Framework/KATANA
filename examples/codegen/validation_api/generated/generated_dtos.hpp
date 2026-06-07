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

// RegisterUserRequest — object, 3 field(s)  ← api.yaml:25
/// Data type with 3 fields
struct RegisterUserRequest {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool EMAIL_REQUIRED = true;
        static constexpr bool PASSWORD_REQUIRED = true;
        static constexpr size_t PASSWORD_MIN_LENGTH = 8;
        static constexpr size_t PASSWORD_MAX_LENGTH = 128;
        static constexpr bool AGE_REQUIRED = false;
        static constexpr double AGE_MINIMUM = 0;
        static constexpr double AGE_MAXIMUM = 120;
    };

    static_assert(metadata::PASSWORD_MIN_LENGTH <= metadata::PASSWORD_MAX_LENGTH, "password: min_length must be <= max_length");
    static_assert(metadata::AGE_MINIMUM <= metadata::AGE_MAXIMUM, "age: minimum must be <= maximum");

    explicit RegisterUserRequest(monotonic_arena* arena = nullptr)
        : arena_(arena),
          email(arena_allocator<char>(arena)),
          password(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> email;
    arena_string<> password;
    /// Optional field
    std::optional<int64_t> age;
};

// RegisterUserRequest_Email_t — string, field RegisterUserRequest.email  ← api.yaml:31
using RegisterUserRequest_Email_t = arena_string<>;

// RegisterUserRequest_Password_t — string, field RegisterUserRequest.password  ← api.yaml:34
using RegisterUserRequest_Password_t = arena_string<>;

// RegisterUserRequest_Age_t — integer, field RegisterUserRequest.age  ← api.yaml:38
using RegisterUserRequest_Age_t = std::optional<int64_t>;

// register_user_response — string  ← api.yaml:21
using register_user_response = arena_string<>;

