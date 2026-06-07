// layer: flat
// Auto-generated validators from OpenAPI specification
//
// This file contains:
//   - Validation functions for all request/response types
//   - Format validators (email, UUID, date-time, etc.)
//   - Constraint validators (length, range, pattern, etc.)
//   - Enum value validators
//
// All validators return std::optional<validation_error>:
//   - std::nullopt on success
//   - validation_error with field path and error code on failure
//
// Validation is automatically called by router bindings before handler execution.
// Invalid requests return 400 Bad Request with error details.
//
#pragma once

#include "generated_dtos.hpp"
#include "katana/core/validation.hpp"
#include "katana/core/format_validators.hpp"
#include <algorithm>
#include <optional>
#include <string_view>
#include <string>
#include <cmath>
#include <cctype>
#include <unordered_set>
#include <vector>

using katana::validation_error;
using katana::validation_error_code;

// ============================================================
// Format Validators (from framework)
// ============================================================

using katana::format_validators::is_valid_email;
using katana::format_validators::is_valid_uuid;
using katana::format_validators::is_valid_datetime;

// ============================================================
// Validation Functions
// ============================================================

[[nodiscard]] inline std::optional<validation_error> validate_CreateNoteRequest(const CreateNoteRequest&);
[[nodiscard]] inline std::optional<validation_error> validate_Note(const Note&);
[[nodiscard]] inline std::optional<validation_error> validate_NoteList(const NoteList&);
[[nodiscard]] inline std::optional<validation_error> validate_NoteList_Notes_t(const NoteList_Notes_t&);

// validate CreateNoteRequest — object, 5 field(s)  ← api.yaml:46
[[nodiscard]] inline std::optional<validation_error> validate_CreateNoteRequest(const CreateNoteRequest& obj) {
    if (obj.title.empty()) {
        return validation_error{"title", validation_error_code::required_field_missing};
    }
    if (!obj.title.empty() && katana::utf8_length(obj.title) < CreateNoteRequest::field_constraints::TITLE_MIN_LENGTH) {
        return validation_error{"title", validation_error_code::string_too_short, CreateNoteRequest::field_constraints::TITLE_MIN_LENGTH};
    }
    if (katana::utf8_length(obj.title) > CreateNoteRequest::field_constraints::TITLE_MAX_LENGTH) {
        return validation_error{"title", validation_error_code::string_too_long, CreateNoteRequest::field_constraints::TITLE_MAX_LENGTH};
    }
    if (obj.body.empty()) {
        return validation_error{"body", validation_error_code::required_field_missing};
    }
    if (!obj.body.empty() && katana::utf8_length(obj.body) < CreateNoteRequest::field_constraints::BODY_MIN_LENGTH) {
        return validation_error{"body", validation_error_code::string_too_short, CreateNoteRequest::field_constraints::BODY_MIN_LENGTH};
    }
    if (obj.due_date.empty()) {
        return validation_error{"due_date", validation_error_code::required_field_missing};
    }
    if (!obj.due_date.empty() && !is_valid_datetime(obj.due_date)) {
        return validation_error{"due_date", validation_error_code::invalid_datetime_format};
    }
    return std::nullopt;
}

// validate Note — object, 7 field(s)  ← api.yaml:55
[[nodiscard]] inline std::optional<validation_error> validate_Note(const Note& obj) {
    if (obj.title.empty()) {
        return validation_error{"title", validation_error_code::required_field_missing};
    }
    if (obj.body.empty()) {
        return validation_error{"body", validation_error_code::required_field_missing};
    }
    if (obj.due_date && !obj.due_date->empty() && !is_valid_datetime((*obj.due_date))) {
        return validation_error{"due_date", validation_error_code::invalid_datetime_format};
    }
    if (obj.created_at.empty()) {
        return validation_error{"created_at", validation_error_code::required_field_missing};
    }
    if (!obj.created_at.empty() && !is_valid_datetime(obj.created_at)) {
        return validation_error{"created_at", validation_error_code::invalid_datetime_format};
    }
    return std::nullopt;
}

// validate NoteList — object, 2 field(s)  ← api.yaml:66
[[nodiscard]] inline std::optional<validation_error> validate_NoteList(const NoteList& obj) {
    (void)obj;
    for (const auto& it_ : obj.notes) { if (auto e_ = validate_Note(it_)) return e_; }
    return std::nullopt;
}

// validate NoteList_Notes_t — array, field NoteList.notes  ← api.yaml:70
[[nodiscard]] inline std::optional<validation_error> validate_NoteList_Notes_t(const NoteList_Notes_t& arr) {
    (void)arr;
    return std::nullopt;
}

