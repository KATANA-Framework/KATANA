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
struct PetCreateRequest {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool NAME_REQUIRED = true;
        static constexpr size_t NAME_MIN_LENGTH = 3;
        static constexpr bool AGE_REQUIRED = true;
        static constexpr double AGE_MINIMUM = 0;
        static constexpr double AGE_MAXIMUM = 30;
        static constexpr bool OWNEREMAIL_REQUIRED = true;
    };

    static_assert(metadata::AGE_MINIMUM <= metadata::AGE_MAXIMUM, "age: minimum must be <= maximum");

    explicit PetCreateRequest(monotonic_arena* arena = nullptr)
        : arena_(arena),
          name(arena_allocator<char>(arena)),
          ownerEmail(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> name;
    int64_t age;
    arena_string<> ownerEmail;
};

using PetCreateRequest_Name_t = arena_string<>;

using PetCreateRequest_Age_t = int64_t;

using PetCreateRequest_OwnerEmail_t = arena_string<>;

/// Data type with 4 fields
struct PetResponse {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool ID_REQUIRED = true;
        static constexpr bool NAME_REQUIRED = true;
        static constexpr bool AGE_REQUIRED = true;
        static constexpr bool OWNEREMAIL_REQUIRED = true;
    };


    explicit PetResponse(monotonic_arena* arena = nullptr)
        : arena_(arena),
          name(arena_allocator<char>(arena)),
          ownerEmail(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    int64_t id;
    arena_string<> name;
    int64_t age;
    arena_string<> ownerEmail;
};

using PetResponse_Id_t = int64_t;

using PetResponse_Name_t = arena_string<>;

using PetResponse_Age_t = int64_t;

using PetResponse_OwnerEmail_t = arena_string<>;

/// Data type with 3 fields
struct ListPetsResponse {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool TRACE_REQUIRED = true;
        static constexpr bool LIMITAPPLIED_REQUIRED = true;
        static constexpr bool SESSION_REQUIRED = false;
    };


    explicit ListPetsResponse(monotonic_arena* arena = nullptr)
        : arena_(arena),
          trace(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> trace;
    int64_t limitApplied;
    /// Optional field
    std::optional<arena_string<>> session;
};

using ListPetsResponse_Trace_t = arena_string<>;

using ListPetsResponse_LimitApplied_t = int64_t;

using ListPetsResponse_Session_t = arena_string<>;

using listPets_param_limit = int64_t;

using listPets_param_X_Trace = arena_string<>;

using listPets_param_session = arena_string<>;

using schema = std::monostate;

using schema_1 = std::monostate;

using schema_2 = std::monostate;

using schema_3 = std::monostate;

using schema_4 = std::monostate;

using getPet_param_petId = int64_t;

using schema_5 = std::monostate;

using schema_6 = std::monostate;

using deletePet_param_petId = int64_t;

