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

/// Enum with 3 possible values
enum class Priority_enum {
    low,
    medium,
    high
};

inline std::string_view to_string(Priority_enum e) {
    switch (e) {
    case Priority_enum::low: return "low";
    case Priority_enum::medium: return "medium";
    case Priority_enum::high: return "high";
    }
    return "";
}

inline std::optional<Priority_enum> Priority_enum_from_string(std::string_view s) {
    if (s == "low") return Priority_enum::low;
    if (s == "medium") return Priority_enum::medium;
    if (s == "high") return Priority_enum::high;
    return std::nullopt;
}

// ============================================================
// Data Transfer Objects (DTOs)
// ============================================================

// Priority — enum  ← api.yaml:43
using Priority = Priority_enum;

// CreateNoteRequest — object, 5 field(s)  ← api.yaml:46
/// Data type with 5 fields
struct CreateNoteRequest {
    // Compile-time metadata for validation
    struct field_constraints {
        static constexpr bool TITLE_REQUIRED = true;
        static constexpr size_t TITLE_MIN_LENGTH = 1;
        static constexpr size_t TITLE_MAX_LENGTH = 200;
        static constexpr bool BODY_REQUIRED = true;
        static constexpr size_t BODY_MIN_LENGTH = 1;
        static constexpr bool PRIORITY_REQUIRED = true;
        static constexpr bool METADATA_REQUIRED = false;
        static constexpr bool DUE_DATE_REQUIRED = true;
    };

    static_assert(field_constraints::TITLE_MIN_LENGTH <= field_constraints::TITLE_MAX_LENGTH, "title: min_length must be <= max_length");

    explicit CreateNoteRequest(monotonic_arena* arena = nullptr)
        : arena_(arena),
          title(arena_allocator<char>(arena)),
          body(arena_allocator<char>(arena)),
          due_date(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> title;
    arena_string<> body;
    Priority_enum priority;
    /// Optional field
    std::optional<arena_string<>> metadata;
    arena_string<> due_date;
};

// CreateNoteRequest_Title_t — string, field CreateNoteRequest.title  ← api.yaml:50
using CreateNoteRequest_Title_t = arena_string<>;

// CreateNoteRequest_Body_t — string, field CreateNoteRequest.body  ← api.yaml:51
using CreateNoteRequest_Body_t = arena_string<>;

// CreateNoteRequest_Metadata_t — object, field CreateNoteRequest.metadata  ← api.yaml:53
using CreateNoteRequest_Metadata_t = arena_string<>;

// CreateNoteRequest_Due_date_t — string, field CreateNoteRequest.due_date  ← api.yaml:54
using CreateNoteRequest_Due_date_t = arena_string<>;

// Note — object, 7 field(s)  ← api.yaml:55
/// Data type with 7 fields
struct Note {
    // Compile-time metadata for validation
    struct field_constraints {
        static constexpr bool ID_REQUIRED = true;
        static constexpr bool TITLE_REQUIRED = true;
        static constexpr bool BODY_REQUIRED = true;
        static constexpr bool PRIORITY_REQUIRED = true;
        static constexpr bool METADATA_REQUIRED = false;
        static constexpr bool DUE_DATE_REQUIRED = false;
        static constexpr bool CREATED_AT_REQUIRED = true;
    };


    explicit Note(monotonic_arena* arena = nullptr)
        : arena_(arena),
          title(arena_allocator<char>(arena)),
          body(arena_allocator<char>(arena)),
          created_at(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    int64_t id;
    arena_string<> title;
    arena_string<> body;
    Priority_enum priority;
    /// Optional field
    std::optional<arena_string<>> metadata;
    /// Optional field
    std::optional<arena_string<>> due_date;
    arena_string<> created_at;
};

// Note_Id_t — integer, field Note.id  ← api.yaml:59
using Note_Id_t = int64_t;

// Note_Title_t — string, field Note.title  ← api.yaml:60
using Note_Title_t = arena_string<>;

// Note_Body_t — string, field Note.body  ← api.yaml:61
using Note_Body_t = arena_string<>;

// Note_Metadata_t — object, field Note.metadata  ← api.yaml:63
using Note_Metadata_t = arena_string<>;

// Note_Due_date_t — string, field Note.due_date  ← api.yaml:64
using Note_Due_date_t = arena_string<>;

// Note_Created_at_t — string, field Note.created_at  ← api.yaml:65
using Note_Created_at_t = arena_string<>;

// NoteList_Count_t — integer, field NoteList.count  ← api.yaml:71
using NoteList_Count_t = int64_t;

// list_notes_param_limit — integer  ← api.yaml:22
using list_notes_param_limit = int64_t;

// list_notes_param_offset — integer  ← api.yaml:22
using list_notes_param_offset = int64_t;

// get_note_param_id — integer  ← api.yaml:34
using get_note_param_id = int64_t;

// NoteList — object, 2 field(s)  ← api.yaml:66
/// Data type with 2 fields
struct NoteList {
    // Compile-time metadata for validation
    struct field_constraints {
        static constexpr bool NOTES_REQUIRED = true;
        static constexpr bool COUNT_REQUIRED = true;
    };


    explicit NoteList(monotonic_arena* arena = nullptr)
        : arena_(arena),
          notes(arena_allocator<Note>(arena)) {}

    monotonic_arena* arena_;
    arena_vector<Note> notes;
    int64_t count;
};

// NoteList_Notes_t — array, field NoteList.notes  ← api.yaml:70
using NoteList_Notes_t = arena_vector<Note>;

