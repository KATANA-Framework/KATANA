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

// compute_sum_request — array  ← api.yaml:15
using compute_sum_request = arena_vector<double, 8>;

// compute_sum_request_Item_t — number, field compute_sum_request.item  ← api.yaml:17
using compute_sum_request_Item_t = double;

// compute_sum_response — number  ← api.yaml:27
using compute_sum_response = double;

