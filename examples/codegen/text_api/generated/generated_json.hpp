// layer: flat
// Auto-generated JSON parsers and serializers from OpenAPI specification
//
// This file contains:
//   - parse_<Type>() functions: JSON string → C++ struct
//   - serialize_<Type>() functions: C++ struct → JSON string
//
// Features:
//   - Zero-copy parsing using arena allocators
//   - Streaming JSON generation without intermediate buffers
//   - Type-safe enum conversion
//   - Automatic null handling for optional fields
//
// All parse functions return std::optional<T>:
//   - std::nullopt on parse error (invalid JSON, wrong type, etc.)
//   - Parsed object on success
//
#pragma once

#include "katana/core/arena.hpp"
#include "katana/core/serde.hpp"
#include <optional>
#include <string>
#include <charconv>
#include <vector>

using katana::monotonic_arena;

// ============================================================
// Forward Declarations
// ============================================================

[[nodiscard]] inline std::optional<text_uppercase_request> parse_text_uppercase_request(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_uppercase_request_Text> parse_text_uppercase_request_Text(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_uppercase_response> parse_text_uppercase_response(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_uppercase_response_Result> parse_text_uppercase_response_Result(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_lowercase_request> parse_text_lowercase_request(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_lowercase_request_Text> parse_text_lowercase_request_Text(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_lowercase_response> parse_text_lowercase_response(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_lowercase_response_Result> parse_text_lowercase_response_Result(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_reverse_request> parse_text_reverse_request(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_reverse_request_Text> parse_text_reverse_request_Text(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_reverse_response> parse_text_reverse_response(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_reverse_response_Result> parse_text_reverse_response_Result(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_stats_request> parse_text_stats_request(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_stats_request_Text> parse_text_stats_request_Text(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_stats_response> parse_text_stats_response(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_stats_response_Chars> parse_text_stats_response_Chars(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_stats_response_Words> parse_text_stats_response_Words(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_stats_response_Lines> parse_text_stats_response_Lines(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_request> parse_text_transform_request(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_request_Text> parse_text_transform_request_Text(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_operation> parse_text_transform_operation(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_request_Trim> parse_text_transform_request_Trim(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_response> parse_text_transform_response(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_response_Original_length> parse_text_transform_response_Original_length(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_response_Result> parse_text_transform_response_Result(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_response_Operation_applied> parse_text_transform_response_Operation_applied(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<text_uppercase_request> parse_text_uppercase_request(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_uppercase_request_Text> parse_text_uppercase_request_Text(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_uppercase_response> parse_text_uppercase_response(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_uppercase_response_Result> parse_text_uppercase_response_Result(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_lowercase_request> parse_text_lowercase_request(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_lowercase_request_Text> parse_text_lowercase_request_Text(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_lowercase_response> parse_text_lowercase_response(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_lowercase_response_Result> parse_text_lowercase_response_Result(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_reverse_request> parse_text_reverse_request(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_reverse_request_Text> parse_text_reverse_request_Text(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_reverse_response> parse_text_reverse_response(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_reverse_response_Result> parse_text_reverse_response_Result(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_stats_request> parse_text_stats_request(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_stats_request_Text> parse_text_stats_request_Text(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_stats_response> parse_text_stats_response(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_stats_response_Chars> parse_text_stats_response_Chars(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_stats_response_Words> parse_text_stats_response_Words(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_stats_response_Lines> parse_text_stats_response_Lines(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_request> parse_text_transform_request(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_request_Text> parse_text_transform_request_Text(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_operation> parse_text_transform_operation(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_request_Trim> parse_text_transform_request_Trim(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_response> parse_text_transform_response(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_response_Original_length> parse_text_transform_response_Original_length(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_response_Result> parse_text_transform_response_Result(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<text_transform_response_Operation_applied> parse_text_transform_response_Operation_applied(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_text_uppercase_request_into(const text_uppercase_request& obj, std::string& out);
inline void serialize_text_uppercase_request_Text_into(const text_uppercase_request_Text& obj, std::string& out);
inline void serialize_text_uppercase_response_into(const text_uppercase_response& obj, std::string& out);
inline void serialize_text_uppercase_response_Result_into(const text_uppercase_response_Result& obj, std::string& out);
inline void serialize_text_lowercase_request_into(const text_lowercase_request& obj, std::string& out);
inline void serialize_text_lowercase_request_Text_into(const text_lowercase_request_Text& obj, std::string& out);
inline void serialize_text_lowercase_response_into(const text_lowercase_response& obj, std::string& out);
inline void serialize_text_lowercase_response_Result_into(const text_lowercase_response_Result& obj, std::string& out);
inline void serialize_text_reverse_request_into(const text_reverse_request& obj, std::string& out);
inline void serialize_text_reverse_request_Text_into(const text_reverse_request_Text& obj, std::string& out);
inline void serialize_text_reverse_response_into(const text_reverse_response& obj, std::string& out);
inline void serialize_text_reverse_response_Result_into(const text_reverse_response_Result& obj, std::string& out);
inline void serialize_text_stats_request_into(const text_stats_request& obj, std::string& out);
inline void serialize_text_stats_request_Text_into(const text_stats_request_Text& obj, std::string& out);
inline void serialize_text_stats_response_into(const text_stats_response& obj, std::string& out);
inline void serialize_text_stats_response_Chars_into(const text_stats_response_Chars& obj, std::string& out);
inline void serialize_text_stats_response_Words_into(const text_stats_response_Words& obj, std::string& out);
inline void serialize_text_stats_response_Lines_into(const text_stats_response_Lines& obj, std::string& out);
inline void serialize_text_transform_request_into(const text_transform_request& obj, std::string& out);
inline void serialize_text_transform_request_Text_into(const text_transform_request_Text& obj, std::string& out);
inline void serialize_text_transform_operation_into(const text_transform_operation& obj, std::string& out);
inline void serialize_text_transform_request_Trim_into(const text_transform_request_Trim& obj, std::string& out);
inline void serialize_text_transform_response_into(const text_transform_response& obj, std::string& out);
inline void serialize_text_transform_response_Original_length_into(const text_transform_response_Original_length& obj, std::string& out);
inline void serialize_text_transform_response_Result_into(const text_transform_response_Result& obj, std::string& out);
inline void serialize_text_transform_response_Operation_applied_into(const text_transform_response_Operation_applied& obj, std::string& out);

inline std::string serialize_text_uppercase_request(const text_uppercase_request& obj);
inline std::string serialize_text_uppercase_request_Text(const text_uppercase_request_Text& obj);
inline std::string serialize_text_uppercase_response(const text_uppercase_response& obj);
inline std::string serialize_text_uppercase_response_Result(const text_uppercase_response_Result& obj);
inline std::string serialize_text_lowercase_request(const text_lowercase_request& obj);
inline std::string serialize_text_lowercase_request_Text(const text_lowercase_request_Text& obj);
inline std::string serialize_text_lowercase_response(const text_lowercase_response& obj);
inline std::string serialize_text_lowercase_response_Result(const text_lowercase_response_Result& obj);
inline std::string serialize_text_reverse_request(const text_reverse_request& obj);
inline std::string serialize_text_reverse_request_Text(const text_reverse_request_Text& obj);
inline std::string serialize_text_reverse_response(const text_reverse_response& obj);
inline std::string serialize_text_reverse_response_Result(const text_reverse_response_Result& obj);
inline std::string serialize_text_stats_request(const text_stats_request& obj);
inline std::string serialize_text_stats_request_Text(const text_stats_request_Text& obj);
inline std::string serialize_text_stats_response(const text_stats_response& obj);
inline std::string serialize_text_stats_response_Chars(const text_stats_response_Chars& obj);
inline std::string serialize_text_stats_response_Words(const text_stats_response_Words& obj);
inline std::string serialize_text_stats_response_Lines(const text_stats_response_Lines& obj);
inline std::string serialize_text_transform_request(const text_transform_request& obj);
inline std::string serialize_text_transform_request_Text(const text_transform_request_Text& obj);
inline std::string serialize_text_transform_operation(const text_transform_operation& obj);
inline std::string serialize_text_transform_request_Trim(const text_transform_request_Trim& obj);
inline std::string serialize_text_transform_response(const text_transform_response& obj);
inline std::string serialize_text_transform_response_Original_length(const text_transform_response_Original_length& obj);
inline std::string serialize_text_transform_response_Result(const text_transform_response_Result& obj);
inline std::string serialize_text_transform_response_Operation_applied(const text_transform_response_Operation_applied& obj);

[[nodiscard]] inline std::optional<std::vector<text_uppercase_request>> parse_text_uppercase_request_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_uppercase_request_Text>> parse_text_uppercase_request_Text_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_uppercase_response>> parse_text_uppercase_response_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_uppercase_response_Result>> parse_text_uppercase_response_Result_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_lowercase_request>> parse_text_lowercase_request_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_lowercase_request_Text>> parse_text_lowercase_request_Text_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_lowercase_response>> parse_text_lowercase_response_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_lowercase_response_Result>> parse_text_lowercase_response_Result_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_reverse_request>> parse_text_reverse_request_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_reverse_request_Text>> parse_text_reverse_request_Text_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_reverse_response>> parse_text_reverse_response_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_reverse_response_Result>> parse_text_reverse_response_Result_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_stats_request>> parse_text_stats_request_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_stats_request_Text>> parse_text_stats_request_Text_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_stats_response>> parse_text_stats_response_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_stats_response_Chars>> parse_text_stats_response_Chars_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_stats_response_Words>> parse_text_stats_response_Words_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_stats_response_Lines>> parse_text_stats_response_Lines_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_request>> parse_text_transform_request_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_request_Text>> parse_text_transform_request_Text_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_operation>> parse_text_transform_operation_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_request_Trim>> parse_text_transform_request_Trim_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_response>> parse_text_transform_response_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_response_Original_length>> parse_text_transform_response_Original_length_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_response_Result>> parse_text_transform_response_Result_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_response_Operation_applied>> parse_text_transform_response_Operation_applied_array(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<std::vector<text_uppercase_request>> parse_text_uppercase_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_uppercase_request_Text>> parse_text_uppercase_request_Text_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_uppercase_response>> parse_text_uppercase_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_uppercase_response_Result>> parse_text_uppercase_response_Result_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_lowercase_request>> parse_text_lowercase_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_lowercase_request_Text>> parse_text_lowercase_request_Text_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_lowercase_response>> parse_text_lowercase_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_lowercase_response_Result>> parse_text_lowercase_response_Result_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_reverse_request>> parse_text_reverse_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_reverse_request_Text>> parse_text_reverse_request_Text_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_reverse_response>> parse_text_reverse_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_reverse_response_Result>> parse_text_reverse_response_Result_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_stats_request>> parse_text_stats_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_stats_request_Text>> parse_text_stats_request_Text_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_stats_response>> parse_text_stats_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_stats_response_Chars>> parse_text_stats_response_Chars_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_stats_response_Words>> parse_text_stats_response_Words_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_stats_response_Lines>> parse_text_stats_response_Lines_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_request>> parse_text_transform_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_request_Text>> parse_text_transform_request_Text_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_operation>> parse_text_transform_operation_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_request_Trim>> parse_text_transform_request_Trim_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_response>> parse_text_transform_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_response_Original_length>> parse_text_transform_response_Original_length_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_response_Result>> parse_text_transform_response_Result_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<text_transform_response_Operation_applied>> parse_text_transform_response_Operation_applied_array(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_text_uppercase_request_array_into(const std::vector<text_uppercase_request>& arr, std::string& out);
inline void serialize_text_uppercase_request_array_into(const arena_vector<text_uppercase_request>& arr, std::string& out);
inline void serialize_text_uppercase_request_Text_array_into(const std::vector<text_uppercase_request_Text>& arr, std::string& out);
inline void serialize_text_uppercase_request_Text_array_into(const arena_vector<text_uppercase_request_Text>& arr, std::string& out);
inline void serialize_text_uppercase_response_array_into(const std::vector<text_uppercase_response>& arr, std::string& out);
inline void serialize_text_uppercase_response_array_into(const arena_vector<text_uppercase_response>& arr, std::string& out);
inline void serialize_text_uppercase_response_Result_array_into(const std::vector<text_uppercase_response_Result>& arr, std::string& out);
inline void serialize_text_uppercase_response_Result_array_into(const arena_vector<text_uppercase_response_Result>& arr, std::string& out);
inline void serialize_text_lowercase_request_array_into(const std::vector<text_lowercase_request>& arr, std::string& out);
inline void serialize_text_lowercase_request_array_into(const arena_vector<text_lowercase_request>& arr, std::string& out);
inline void serialize_text_lowercase_request_Text_array_into(const std::vector<text_lowercase_request_Text>& arr, std::string& out);
inline void serialize_text_lowercase_request_Text_array_into(const arena_vector<text_lowercase_request_Text>& arr, std::string& out);
inline void serialize_text_lowercase_response_array_into(const std::vector<text_lowercase_response>& arr, std::string& out);
inline void serialize_text_lowercase_response_array_into(const arena_vector<text_lowercase_response>& arr, std::string& out);
inline void serialize_text_lowercase_response_Result_array_into(const std::vector<text_lowercase_response_Result>& arr, std::string& out);
inline void serialize_text_lowercase_response_Result_array_into(const arena_vector<text_lowercase_response_Result>& arr, std::string& out);
inline void serialize_text_reverse_request_array_into(const std::vector<text_reverse_request>& arr, std::string& out);
inline void serialize_text_reverse_request_array_into(const arena_vector<text_reverse_request>& arr, std::string& out);
inline void serialize_text_reverse_request_Text_array_into(const std::vector<text_reverse_request_Text>& arr, std::string& out);
inline void serialize_text_reverse_request_Text_array_into(const arena_vector<text_reverse_request_Text>& arr, std::string& out);
inline void serialize_text_reverse_response_array_into(const std::vector<text_reverse_response>& arr, std::string& out);
inline void serialize_text_reverse_response_array_into(const arena_vector<text_reverse_response>& arr, std::string& out);
inline void serialize_text_reverse_response_Result_array_into(const std::vector<text_reverse_response_Result>& arr, std::string& out);
inline void serialize_text_reverse_response_Result_array_into(const arena_vector<text_reverse_response_Result>& arr, std::string& out);
inline void serialize_text_stats_request_array_into(const std::vector<text_stats_request>& arr, std::string& out);
inline void serialize_text_stats_request_array_into(const arena_vector<text_stats_request>& arr, std::string& out);
inline void serialize_text_stats_request_Text_array_into(const std::vector<text_stats_request_Text>& arr, std::string& out);
inline void serialize_text_stats_request_Text_array_into(const arena_vector<text_stats_request_Text>& arr, std::string& out);
inline void serialize_text_stats_response_array_into(const std::vector<text_stats_response>& arr, std::string& out);
inline void serialize_text_stats_response_array_into(const arena_vector<text_stats_response>& arr, std::string& out);
inline void serialize_text_stats_response_Chars_array_into(const std::vector<text_stats_response_Chars>& arr, std::string& out);
inline void serialize_text_stats_response_Chars_array_into(const arena_vector<text_stats_response_Chars>& arr, std::string& out);
inline void serialize_text_stats_response_Words_array_into(const std::vector<text_stats_response_Words>& arr, std::string& out);
inline void serialize_text_stats_response_Words_array_into(const arena_vector<text_stats_response_Words>& arr, std::string& out);
inline void serialize_text_stats_response_Lines_array_into(const std::vector<text_stats_response_Lines>& arr, std::string& out);
inline void serialize_text_stats_response_Lines_array_into(const arena_vector<text_stats_response_Lines>& arr, std::string& out);
inline void serialize_text_transform_request_array_into(const std::vector<text_transform_request>& arr, std::string& out);
inline void serialize_text_transform_request_array_into(const arena_vector<text_transform_request>& arr, std::string& out);
inline void serialize_text_transform_request_Text_array_into(const std::vector<text_transform_request_Text>& arr, std::string& out);
inline void serialize_text_transform_request_Text_array_into(const arena_vector<text_transform_request_Text>& arr, std::string& out);
inline void serialize_text_transform_operation_array_into(const std::vector<text_transform_operation>& arr, std::string& out);
inline void serialize_text_transform_operation_array_into(const arena_vector<text_transform_operation>& arr, std::string& out);
inline void serialize_text_transform_request_Trim_array_into(const std::vector<text_transform_request_Trim>& arr, std::string& out);
inline void serialize_text_transform_request_Trim_array_into(const arena_vector<text_transform_request_Trim>& arr, std::string& out);
inline void serialize_text_transform_response_array_into(const std::vector<text_transform_response>& arr, std::string& out);
inline void serialize_text_transform_response_array_into(const arena_vector<text_transform_response>& arr, std::string& out);
inline void serialize_text_transform_response_Original_length_array_into(const std::vector<text_transform_response_Original_length>& arr, std::string& out);
inline void serialize_text_transform_response_Original_length_array_into(const arena_vector<text_transform_response_Original_length>& arr, std::string& out);
inline void serialize_text_transform_response_Result_array_into(const std::vector<text_transform_response_Result>& arr, std::string& out);
inline void serialize_text_transform_response_Result_array_into(const arena_vector<text_transform_response_Result>& arr, std::string& out);
inline void serialize_text_transform_response_Operation_applied_array_into(const std::vector<text_transform_response_Operation_applied>& arr, std::string& out);
inline void serialize_text_transform_response_Operation_applied_array_into(const arena_vector<text_transform_response_Operation_applied>& arr, std::string& out);

inline std::string serialize_text_uppercase_request_array(const std::vector<text_uppercase_request>& arr);
inline std::string serialize_text_uppercase_request_array(const arena_vector<text_uppercase_request>& arr);
inline std::string serialize_text_uppercase_request_Text_array(const std::vector<text_uppercase_request_Text>& arr);
inline std::string serialize_text_uppercase_request_Text_array(const arena_vector<text_uppercase_request_Text>& arr);
inline std::string serialize_text_uppercase_response_array(const std::vector<text_uppercase_response>& arr);
inline std::string serialize_text_uppercase_response_array(const arena_vector<text_uppercase_response>& arr);
inline std::string serialize_text_uppercase_response_Result_array(const std::vector<text_uppercase_response_Result>& arr);
inline std::string serialize_text_uppercase_response_Result_array(const arena_vector<text_uppercase_response_Result>& arr);
inline std::string serialize_text_lowercase_request_array(const std::vector<text_lowercase_request>& arr);
inline std::string serialize_text_lowercase_request_array(const arena_vector<text_lowercase_request>& arr);
inline std::string serialize_text_lowercase_request_Text_array(const std::vector<text_lowercase_request_Text>& arr);
inline std::string serialize_text_lowercase_request_Text_array(const arena_vector<text_lowercase_request_Text>& arr);
inline std::string serialize_text_lowercase_response_array(const std::vector<text_lowercase_response>& arr);
inline std::string serialize_text_lowercase_response_array(const arena_vector<text_lowercase_response>& arr);
inline std::string serialize_text_lowercase_response_Result_array(const std::vector<text_lowercase_response_Result>& arr);
inline std::string serialize_text_lowercase_response_Result_array(const arena_vector<text_lowercase_response_Result>& arr);
inline std::string serialize_text_reverse_request_array(const std::vector<text_reverse_request>& arr);
inline std::string serialize_text_reverse_request_array(const arena_vector<text_reverse_request>& arr);
inline std::string serialize_text_reverse_request_Text_array(const std::vector<text_reverse_request_Text>& arr);
inline std::string serialize_text_reverse_request_Text_array(const arena_vector<text_reverse_request_Text>& arr);
inline std::string serialize_text_reverse_response_array(const std::vector<text_reverse_response>& arr);
inline std::string serialize_text_reverse_response_array(const arena_vector<text_reverse_response>& arr);
inline std::string serialize_text_reverse_response_Result_array(const std::vector<text_reverse_response_Result>& arr);
inline std::string serialize_text_reverse_response_Result_array(const arena_vector<text_reverse_response_Result>& arr);
inline std::string serialize_text_stats_request_array(const std::vector<text_stats_request>& arr);
inline std::string serialize_text_stats_request_array(const arena_vector<text_stats_request>& arr);
inline std::string serialize_text_stats_request_Text_array(const std::vector<text_stats_request_Text>& arr);
inline std::string serialize_text_stats_request_Text_array(const arena_vector<text_stats_request_Text>& arr);
inline std::string serialize_text_stats_response_array(const std::vector<text_stats_response>& arr);
inline std::string serialize_text_stats_response_array(const arena_vector<text_stats_response>& arr);
inline std::string serialize_text_stats_response_Chars_array(const std::vector<text_stats_response_Chars>& arr);
inline std::string serialize_text_stats_response_Chars_array(const arena_vector<text_stats_response_Chars>& arr);
inline std::string serialize_text_stats_response_Words_array(const std::vector<text_stats_response_Words>& arr);
inline std::string serialize_text_stats_response_Words_array(const arena_vector<text_stats_response_Words>& arr);
inline std::string serialize_text_stats_response_Lines_array(const std::vector<text_stats_response_Lines>& arr);
inline std::string serialize_text_stats_response_Lines_array(const arena_vector<text_stats_response_Lines>& arr);
inline std::string serialize_text_transform_request_array(const std::vector<text_transform_request>& arr);
inline std::string serialize_text_transform_request_array(const arena_vector<text_transform_request>& arr);
inline std::string serialize_text_transform_request_Text_array(const std::vector<text_transform_request_Text>& arr);
inline std::string serialize_text_transform_request_Text_array(const arena_vector<text_transform_request_Text>& arr);
inline std::string serialize_text_transform_operation_array(const std::vector<text_transform_operation>& arr);
inline std::string serialize_text_transform_operation_array(const arena_vector<text_transform_operation>& arr);
inline std::string serialize_text_transform_request_Trim_array(const std::vector<text_transform_request_Trim>& arr);
inline std::string serialize_text_transform_request_Trim_array(const arena_vector<text_transform_request_Trim>& arr);
inline std::string serialize_text_transform_response_array(const std::vector<text_transform_response>& arr);
inline std::string serialize_text_transform_response_array(const arena_vector<text_transform_response>& arr);
inline std::string serialize_text_transform_response_Original_length_array(const std::vector<text_transform_response_Original_length>& arr);
inline std::string serialize_text_transform_response_Original_length_array(const arena_vector<text_transform_response_Original_length>& arr);
inline std::string serialize_text_transform_response_Result_array(const std::vector<text_transform_response_Result>& arr);
inline std::string serialize_text_transform_response_Result_array(const arena_vector<text_transform_response_Result>& arr);
inline std::string serialize_text_transform_response_Operation_applied_array(const std::vector<text_transform_response_Operation_applied>& arr);
inline std::string serialize_text_transform_response_Operation_applied_array(const arena_vector<text_transform_response_Operation_applied>& arr);

// ============================================================
// JSON Parse Functions
// ============================================================

[[nodiscard]] inline std::optional<text_uppercase_request> parse_text_uppercase_request(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_uppercase_request obj(arena);
    bool has_text = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "text") {
            has_text = true;
            if (auto v = cur.string()) {
                obj.text = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_text) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<text_uppercase_request> parse_text_uppercase_request(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_uppercase_request(cur, arena);
}

[[nodiscard]] inline std::optional<text_uppercase_request_Text> parse_text_uppercase_request_Text(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return text_uppercase_request_Text{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<text_uppercase_request_Text> parse_text_uppercase_request_Text(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_uppercase_request_Text(cur, arena);
}

[[nodiscard]] inline std::optional<text_uppercase_response> parse_text_uppercase_response(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_uppercase_response obj(arena);

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "result") {
            if (auto v = cur.string()) {
                obj.result = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return obj;
}

[[nodiscard]] inline std::optional<text_uppercase_response> parse_text_uppercase_response(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_uppercase_response(cur, arena);
}

[[nodiscard]] inline std::optional<text_uppercase_response_Result> parse_text_uppercase_response_Result(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return text_uppercase_response_Result{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<text_uppercase_response_Result> parse_text_uppercase_response_Result(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_uppercase_response_Result(cur, arena);
}

[[nodiscard]] inline std::optional<text_lowercase_request> parse_text_lowercase_request(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_lowercase_request obj(arena);
    bool has_text = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "text") {
            has_text = true;
            if (auto v = cur.string()) {
                obj.text = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_text) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<text_lowercase_request> parse_text_lowercase_request(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_lowercase_request(cur, arena);
}

[[nodiscard]] inline std::optional<text_lowercase_request_Text> parse_text_lowercase_request_Text(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return text_lowercase_request_Text{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<text_lowercase_request_Text> parse_text_lowercase_request_Text(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_lowercase_request_Text(cur, arena);
}

[[nodiscard]] inline std::optional<text_lowercase_response> parse_text_lowercase_response(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_lowercase_response obj(arena);

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "result") {
            if (auto v = cur.string()) {
                obj.result = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return obj;
}

[[nodiscard]] inline std::optional<text_lowercase_response> parse_text_lowercase_response(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_lowercase_response(cur, arena);
}

[[nodiscard]] inline std::optional<text_lowercase_response_Result> parse_text_lowercase_response_Result(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return text_lowercase_response_Result{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<text_lowercase_response_Result> parse_text_lowercase_response_Result(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_lowercase_response_Result(cur, arena);
}

[[nodiscard]] inline std::optional<text_reverse_request> parse_text_reverse_request(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_reverse_request obj(arena);
    bool has_text = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "text") {
            has_text = true;
            if (auto v = cur.string()) {
                obj.text = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_text) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<text_reverse_request> parse_text_reverse_request(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_reverse_request(cur, arena);
}

[[nodiscard]] inline std::optional<text_reverse_request_Text> parse_text_reverse_request_Text(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return text_reverse_request_Text{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<text_reverse_request_Text> parse_text_reverse_request_Text(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_reverse_request_Text(cur, arena);
}

[[nodiscard]] inline std::optional<text_reverse_response> parse_text_reverse_response(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_reverse_response obj(arena);

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "result") {
            if (auto v = cur.string()) {
                obj.result = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return obj;
}

[[nodiscard]] inline std::optional<text_reverse_response> parse_text_reverse_response(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_reverse_response(cur, arena);
}

[[nodiscard]] inline std::optional<text_reverse_response_Result> parse_text_reverse_response_Result(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return text_reverse_response_Result{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<text_reverse_response_Result> parse_text_reverse_response_Result(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_reverse_response_Result(cur, arena);
}

[[nodiscard]] inline std::optional<text_stats_request> parse_text_stats_request(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_stats_request obj(arena);
    bool has_text = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "text") {
            has_text = true;
            if (auto v = cur.string()) {
                obj.text = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_text) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<text_stats_request> parse_text_stats_request(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_stats_request(cur, arena);
}

[[nodiscard]] inline std::optional<text_stats_request_Text> parse_text_stats_request_Text(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return text_stats_request_Text{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<text_stats_request_Text> parse_text_stats_request_Text(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_stats_request_Text(cur, arena);
}

[[nodiscard]] inline std::optional<text_stats_response> parse_text_stats_response(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_stats_response obj(arena);

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "chars") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.chars = *v;
            } else { cur.skip_value(); }
        } else         if (*key == "words") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.words = *v;
            } else { cur.skip_value(); }
        } else         if (*key == "lines") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.lines = *v;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return obj;
}

[[nodiscard]] inline std::optional<text_stats_response> parse_text_stats_response(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_stats_response(cur, arena);
}

[[nodiscard]] inline std::optional<text_stats_response_Chars> parse_text_stats_response_Chars(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return text_stats_response_Chars{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<text_stats_response_Chars> parse_text_stats_response_Chars(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_stats_response_Chars(cur, arena);
}

[[nodiscard]] inline std::optional<text_stats_response_Words> parse_text_stats_response_Words(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return text_stats_response_Words{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<text_stats_response_Words> parse_text_stats_response_Words(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_stats_response_Words(cur, arena);
}

[[nodiscard]] inline std::optional<text_stats_response_Lines> parse_text_stats_response_Lines(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return text_stats_response_Lines{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<text_stats_response_Lines> parse_text_stats_response_Lines(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_stats_response_Lines(cur, arena);
}

[[nodiscard]] inline std::optional<text_transform_request> parse_text_transform_request(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_transform_request obj(arena);
    bool has_text = false;
    bool has_operation = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "text") {
            has_text = true;
            if (auto v = cur.string()) {
                obj.text = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else         if (*key == "operation") {
            has_operation = true;
            if (auto v = cur.string()) {
                auto enum_val = text_transform_operation_enum_from_string(std::string_view(v->begin(), v->end()));
                if (enum_val) obj.operation = *enum_val;
            } else { cur.skip_value(); }
        } else         if (*key == "trim") {
            if (auto v = katana::serde::parse_bool(cur)) {
                obj.trim = *v;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_text) return std::nullopt;
    if (!has_operation) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<text_transform_request> parse_text_transform_request(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_transform_request(cur, arena);
}

[[nodiscard]] inline std::optional<text_transform_request_Text> parse_text_transform_request_Text(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return text_transform_request_Text{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<text_transform_request_Text> parse_text_transform_request_Text(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_transform_request_Text(cur, arena);
}

[[nodiscard]] inline std::optional<text_transform_operation> parse_text_transform_operation(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = cur.string()) {
        return text_transform_operation_enum_from_string(std::string_view(v->begin(), v->end()));
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<text_transform_operation> parse_text_transform_operation(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_transform_operation(cur, arena);
}

[[nodiscard]] inline std::optional<text_transform_request_Trim> parse_text_transform_request_Trim(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_bool(cur)) return text_transform_request_Trim{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<text_transform_request_Trim> parse_text_transform_request_Trim(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_transform_request_Trim(cur, arena);
}

[[nodiscard]] inline std::optional<text_transform_response> parse_text_transform_response(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    text_transform_response obj(arena);

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "original_length") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.original_length = *v;
            } else { cur.skip_value(); }
        } else         if (*key == "result") {
            if (auto v = cur.string()) {
                obj.result = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else         if (*key == "operation_applied") {
            if (auto v = cur.string()) {
                obj.operation_applied = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return obj;
}

[[nodiscard]] inline std::optional<text_transform_response> parse_text_transform_response(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_transform_response(cur, arena);
}

[[nodiscard]] inline std::optional<text_transform_response_Original_length> parse_text_transform_response_Original_length(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return text_transform_response_Original_length{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<text_transform_response_Original_length> parse_text_transform_response_Original_length(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_transform_response_Original_length(cur, arena);
}

[[nodiscard]] inline std::optional<text_transform_response_Result> parse_text_transform_response_Result(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return text_transform_response_Result{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<text_transform_response_Result> parse_text_transform_response_Result(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_transform_response_Result(cur, arena);
}

[[nodiscard]] inline std::optional<text_transform_response_Operation_applied> parse_text_transform_response_Operation_applied(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return text_transform_response_Operation_applied{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<text_transform_response_Operation_applied> parse_text_transform_response_Operation_applied(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_transform_response_Operation_applied(cur, arena);
}

// ============================================================
// JSON Serialize Functions
// ============================================================

inline void serialize_text_uppercase_request_into(const text_uppercase_request& obj, std::string& json) {
    json.push_back('{');
    json.append("\"text\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.text, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_text_uppercase_request(const text_uppercase_request& obj) {
    std::string json;
    size_t reserve_estimate = 42;
    reserve_estimate += obj.text.size();
    json.reserve(reserve_estimate);
    serialize_text_uppercase_request_into(obj, json);
    return json;
}

inline void serialize_text_uppercase_request_Text_into(const text_uppercase_request_Text& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_text_uppercase_request_Text(const text_uppercase_request_Text& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_text_uppercase_request_Text_into(obj, json);
    return json;
}

inline void serialize_text_uppercase_response_into(const text_uppercase_response& obj, std::string& json) {
    json.push_back('{');
    json.append("\"result\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.result, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_text_uppercase_response(const text_uppercase_response& obj) {
    std::string json;
    size_t reserve_estimate = 44;
    reserve_estimate += obj.result.size();
    json.reserve(reserve_estimate);
    serialize_text_uppercase_response_into(obj, json);
    return json;
}

inline void serialize_text_uppercase_response_Result_into(const text_uppercase_response_Result& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_text_uppercase_response_Result(const text_uppercase_response_Result& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_text_uppercase_response_Result_into(obj, json);
    return json;
}

inline void serialize_text_lowercase_request_into(const text_lowercase_request& obj, std::string& json) {
    json.push_back('{');
    json.append("\"text\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.text, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_text_lowercase_request(const text_lowercase_request& obj) {
    std::string json;
    size_t reserve_estimate = 42;
    reserve_estimate += obj.text.size();
    json.reserve(reserve_estimate);
    serialize_text_lowercase_request_into(obj, json);
    return json;
}

inline void serialize_text_lowercase_request_Text_into(const text_lowercase_request_Text& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_text_lowercase_request_Text(const text_lowercase_request_Text& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_text_lowercase_request_Text_into(obj, json);
    return json;
}

inline void serialize_text_lowercase_response_into(const text_lowercase_response& obj, std::string& json) {
    json.push_back('{');
    json.append("\"result\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.result, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_text_lowercase_response(const text_lowercase_response& obj) {
    std::string json;
    size_t reserve_estimate = 44;
    reserve_estimate += obj.result.size();
    json.reserve(reserve_estimate);
    serialize_text_lowercase_response_into(obj, json);
    return json;
}

inline void serialize_text_lowercase_response_Result_into(const text_lowercase_response_Result& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_text_lowercase_response_Result(const text_lowercase_response_Result& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_text_lowercase_response_Result_into(obj, json);
    return json;
}

inline void serialize_text_reverse_request_into(const text_reverse_request& obj, std::string& json) {
    json.push_back('{');
    json.append("\"text\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.text, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_text_reverse_request(const text_reverse_request& obj) {
    std::string json;
    size_t reserve_estimate = 42;
    reserve_estimate += obj.text.size();
    json.reserve(reserve_estimate);
    serialize_text_reverse_request_into(obj, json);
    return json;
}

inline void serialize_text_reverse_request_Text_into(const text_reverse_request_Text& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_text_reverse_request_Text(const text_reverse_request_Text& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_text_reverse_request_Text_into(obj, json);
    return json;
}

inline void serialize_text_reverse_response_into(const text_reverse_response& obj, std::string& json) {
    json.push_back('{');
    json.append("\"result\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.result, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_text_reverse_response(const text_reverse_response& obj) {
    std::string json;
    size_t reserve_estimate = 44;
    reserve_estimate += obj.result.size();
    json.reserve(reserve_estimate);
    serialize_text_reverse_response_into(obj, json);
    return json;
}

inline void serialize_text_reverse_response_Result_into(const text_reverse_response_Result& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_text_reverse_response_Result(const text_reverse_response_Result& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_text_reverse_response_Result_into(obj, json);
    return json;
}

inline void serialize_text_stats_request_into(const text_stats_request& obj, std::string& json) {
    json.push_back('{');
    json.append("\"text\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.text, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_text_stats_request(const text_stats_request& obj) {
    std::string json;
    size_t reserve_estimate = 42;
    reserve_estimate += obj.text.size();
    json.reserve(reserve_estimate);
    serialize_text_stats_request_into(obj, json);
    return json;
}

inline void serialize_text_stats_request_Text_into(const text_stats_request_Text& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_text_stats_request_Text(const text_stats_request_Text& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_text_stats_request_Text_into(obj, json);
    return json;
}

inline void serialize_text_stats_response_into(const text_stats_response& obj, std::string& json) {
    json.push_back('{');
    json.append("\"chars\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.chars);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"words\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.words);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"lines\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.lines);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.push_back('}');
}

inline std::string serialize_text_stats_response(const text_stats_response& obj) {
    std::string json;
    size_t reserve_estimate = 89;
    json.reserve(reserve_estimate);
    serialize_text_stats_response_into(obj, json);
    return json;
}

inline void serialize_text_stats_response_Chars_into(const text_stats_response_Chars& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_text_stats_response_Chars(const text_stats_response_Chars& obj) {
    std::string json;
    serialize_text_stats_response_Chars_into(obj, json);
    return json;
}

inline void serialize_text_stats_response_Words_into(const text_stats_response_Words& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_text_stats_response_Words(const text_stats_response_Words& obj) {
    std::string json;
    serialize_text_stats_response_Words_into(obj, json);
    return json;
}

inline void serialize_text_stats_response_Lines_into(const text_stats_response_Lines& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_text_stats_response_Lines(const text_stats_response_Lines& obj) {
    std::string json;
    serialize_text_stats_response_Lines_into(obj, json);
    return json;
}

inline void serialize_text_transform_request_into(const text_transform_request& obj, std::string& json) {
    json.push_back('{');
    json.append("\"text\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.text, json);
    json.push_back('"');
    json.append(",\"operation\":");
    json.push_back('"');
    json.append(to_string(obj.operation));
    json.push_back('"');
    json.append(",\"trim\":");
    json.append(obj.trim ? "true" : "false");
    json.push_back('}');
}

inline std::string serialize_text_transform_request(const text_transform_request& obj) {
    std::string json;
    size_t reserve_estimate = 100;
    reserve_estimate += obj.text.size();
    json.reserve(reserve_estimate);
    serialize_text_transform_request_into(obj, json);
    return json;
}

inline void serialize_text_transform_request_Text_into(const text_transform_request_Text& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_text_transform_request_Text(const text_transform_request_Text& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_text_transform_request_Text_into(obj, json);
    return json;
}

inline void serialize_text_transform_operation_into(const text_transform_operation& obj, std::string& json) {
    auto str = to_string(obj);
    json.push_back('"');
    json.append(str);
    json.push_back('"');
}

inline std::string serialize_text_transform_operation(const text_transform_operation& obj) {
    std::string json;
    auto str = to_string(obj);
    json.reserve(str.size() + 2);
    serialize_text_transform_operation_into(obj, json);
    return json;
}

inline void serialize_text_transform_request_Trim_into(const text_transform_request_Trim& obj, std::string& json) {
    json.append(obj ? "true" : "false");
}

inline std::string serialize_text_transform_request_Trim(const text_transform_request_Trim& obj) {
    return obj ? "true" : "false";
}

inline void serialize_text_transform_response_into(const text_transform_response& obj, std::string& json) {
    json.push_back('{');
    json.append("\"original_length\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.original_length);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"result\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.result, json);
    json.push_back('"');
    json.append(",\"operation_applied\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.operation_applied, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_text_transform_response(const text_transform_response& obj) {
    std::string json;
    size_t reserve_estimate = 136;
    reserve_estimate += obj.result.size();
    reserve_estimate += obj.operation_applied.size();
    json.reserve(reserve_estimate);
    serialize_text_transform_response_into(obj, json);
    return json;
}

inline void serialize_text_transform_response_Original_length_into(const text_transform_response_Original_length& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_text_transform_response_Original_length(const text_transform_response_Original_length& obj) {
    std::string json;
    serialize_text_transform_response_Original_length_into(obj, json);
    return json;
}

inline void serialize_text_transform_response_Result_into(const text_transform_response_Result& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_text_transform_response_Result(const text_transform_response_Result& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_text_transform_response_Result_into(obj, json);
    return json;
}

inline void serialize_text_transform_response_Operation_applied_into(const text_transform_response_Operation_applied& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_text_transform_response_Operation_applied(const text_transform_response_Operation_applied& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_text_transform_response_Operation_applied_into(obj, json);
    return json;
}

// ============================================================
// Array Parse Functions
// ============================================================

[[nodiscard]] inline std::optional<std::vector<text_uppercase_request>> parse_text_uppercase_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_uppercase_request> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_uppercase_request(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_uppercase_request>> parse_text_uppercase_request_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_uppercase_request_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_uppercase_request_Text>> parse_text_uppercase_request_Text_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_uppercase_request_Text> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_uppercase_request_Text(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_uppercase_request_Text>> parse_text_uppercase_request_Text_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_uppercase_request_Text_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_uppercase_response>> parse_text_uppercase_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_uppercase_response> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_uppercase_response(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_uppercase_response>> parse_text_uppercase_response_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_uppercase_response_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_uppercase_response_Result>> parse_text_uppercase_response_Result_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_uppercase_response_Result> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_uppercase_response_Result(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_uppercase_response_Result>> parse_text_uppercase_response_Result_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_uppercase_response_Result_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_lowercase_request>> parse_text_lowercase_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_lowercase_request> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_lowercase_request(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_lowercase_request>> parse_text_lowercase_request_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_lowercase_request_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_lowercase_request_Text>> parse_text_lowercase_request_Text_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_lowercase_request_Text> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_lowercase_request_Text(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_lowercase_request_Text>> parse_text_lowercase_request_Text_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_lowercase_request_Text_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_lowercase_response>> parse_text_lowercase_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_lowercase_response> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_lowercase_response(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_lowercase_response>> parse_text_lowercase_response_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_lowercase_response_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_lowercase_response_Result>> parse_text_lowercase_response_Result_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_lowercase_response_Result> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_lowercase_response_Result(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_lowercase_response_Result>> parse_text_lowercase_response_Result_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_lowercase_response_Result_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_reverse_request>> parse_text_reverse_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_reverse_request> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_reverse_request(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_reverse_request>> parse_text_reverse_request_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_reverse_request_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_reverse_request_Text>> parse_text_reverse_request_Text_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_reverse_request_Text> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_reverse_request_Text(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_reverse_request_Text>> parse_text_reverse_request_Text_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_reverse_request_Text_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_reverse_response>> parse_text_reverse_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_reverse_response> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_reverse_response(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_reverse_response>> parse_text_reverse_response_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_reverse_response_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_reverse_response_Result>> parse_text_reverse_response_Result_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_reverse_response_Result> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_reverse_response_Result(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_reverse_response_Result>> parse_text_reverse_response_Result_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_reverse_response_Result_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_stats_request>> parse_text_stats_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_stats_request> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_stats_request(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_stats_request>> parse_text_stats_request_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_stats_request_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_stats_request_Text>> parse_text_stats_request_Text_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_stats_request_Text> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_stats_request_Text(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_stats_request_Text>> parse_text_stats_request_Text_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_stats_request_Text_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_stats_response>> parse_text_stats_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_stats_response> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_stats_response(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_stats_response>> parse_text_stats_response_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_stats_response_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_stats_response_Chars>> parse_text_stats_response_Chars_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_stats_response_Chars> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_stats_response_Chars(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_stats_response_Chars>> parse_text_stats_response_Chars_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_stats_response_Chars_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_stats_response_Words>> parse_text_stats_response_Words_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_stats_response_Words> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_stats_response_Words(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_stats_response_Words>> parse_text_stats_response_Words_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_stats_response_Words_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_stats_response_Lines>> parse_text_stats_response_Lines_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_stats_response_Lines> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_stats_response_Lines(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_stats_response_Lines>> parse_text_stats_response_Lines_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_stats_response_Lines_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_transform_request>> parse_text_transform_request_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_transform_request> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_transform_request(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_transform_request>> parse_text_transform_request_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_transform_request_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_transform_request_Text>> parse_text_transform_request_Text_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_transform_request_Text> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_transform_request_Text(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_transform_request_Text>> parse_text_transform_request_Text_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_transform_request_Text_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_transform_operation>> parse_text_transform_operation_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_transform_operation> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_transform_operation(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_transform_operation>> parse_text_transform_operation_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_transform_operation_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_transform_request_Trim>> parse_text_transform_request_Trim_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_transform_request_Trim> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_transform_request_Trim(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_transform_request_Trim>> parse_text_transform_request_Trim_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_transform_request_Trim_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_transform_response>> parse_text_transform_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_transform_response> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_transform_response(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_transform_response>> parse_text_transform_response_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_transform_response_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_transform_response_Original_length>> parse_text_transform_response_Original_length_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_transform_response_Original_length> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_transform_response_Original_length(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_transform_response_Original_length>> parse_text_transform_response_Original_length_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_transform_response_Original_length_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_transform_response_Result>> parse_text_transform_response_Result_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_transform_response_Result> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_transform_response_Result(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_transform_response_Result>> parse_text_transform_response_Result_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_transform_response_Result_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<text_transform_response_Operation_applied>> parse_text_transform_response_Operation_applied_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<text_transform_response_Operation_applied> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_text_transform_response_Operation_applied(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<text_transform_response_Operation_applied>> parse_text_transform_response_Operation_applied_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_text_transform_response_Operation_applied_array(cur, arena);
}

// ============================================================
// Array Serialize Functions
// ============================================================

inline void serialize_text_uppercase_request_array_into(const std::vector<text_uppercase_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_uppercase_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_uppercase_request_array(const std::vector<text_uppercase_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 42 + 2);
    serialize_text_uppercase_request_array_into(arr, json);
    return json;
}

inline void serialize_text_uppercase_request_array_into(const arena_vector<text_uppercase_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_uppercase_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_uppercase_request_array(const arena_vector<text_uppercase_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 42 + 2);
    serialize_text_uppercase_request_array_into(arr, json);
    return json;
}

inline void serialize_text_uppercase_request_Text_array_into(const std::vector<text_uppercase_request_Text>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_uppercase_request_Text_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_uppercase_request_Text_array(const std::vector<text_uppercase_request_Text>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_uppercase_request_Text_array_into(arr, json);
    return json;
}

inline void serialize_text_uppercase_request_Text_array_into(const arena_vector<text_uppercase_request_Text>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_uppercase_request_Text_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_uppercase_request_Text_array(const arena_vector<text_uppercase_request_Text>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_uppercase_request_Text_array_into(arr, json);
    return json;
}

inline void serialize_text_uppercase_response_array_into(const std::vector<text_uppercase_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_uppercase_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_uppercase_response_array(const std::vector<text_uppercase_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 44 + 2);
    serialize_text_uppercase_response_array_into(arr, json);
    return json;
}

inline void serialize_text_uppercase_response_array_into(const arena_vector<text_uppercase_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_uppercase_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_uppercase_response_array(const arena_vector<text_uppercase_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 44 + 2);
    serialize_text_uppercase_response_array_into(arr, json);
    return json;
}

inline void serialize_text_uppercase_response_Result_array_into(const std::vector<text_uppercase_response_Result>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_uppercase_response_Result_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_uppercase_response_Result_array(const std::vector<text_uppercase_response_Result>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_uppercase_response_Result_array_into(arr, json);
    return json;
}

inline void serialize_text_uppercase_response_Result_array_into(const arena_vector<text_uppercase_response_Result>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_uppercase_response_Result_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_uppercase_response_Result_array(const arena_vector<text_uppercase_response_Result>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_uppercase_response_Result_array_into(arr, json);
    return json;
}

inline void serialize_text_lowercase_request_array_into(const std::vector<text_lowercase_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_lowercase_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_lowercase_request_array(const std::vector<text_lowercase_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 42 + 2);
    serialize_text_lowercase_request_array_into(arr, json);
    return json;
}

inline void serialize_text_lowercase_request_array_into(const arena_vector<text_lowercase_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_lowercase_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_lowercase_request_array(const arena_vector<text_lowercase_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 42 + 2);
    serialize_text_lowercase_request_array_into(arr, json);
    return json;
}

inline void serialize_text_lowercase_request_Text_array_into(const std::vector<text_lowercase_request_Text>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_lowercase_request_Text_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_lowercase_request_Text_array(const std::vector<text_lowercase_request_Text>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_lowercase_request_Text_array_into(arr, json);
    return json;
}

inline void serialize_text_lowercase_request_Text_array_into(const arena_vector<text_lowercase_request_Text>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_lowercase_request_Text_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_lowercase_request_Text_array(const arena_vector<text_lowercase_request_Text>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_lowercase_request_Text_array_into(arr, json);
    return json;
}

inline void serialize_text_lowercase_response_array_into(const std::vector<text_lowercase_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_lowercase_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_lowercase_response_array(const std::vector<text_lowercase_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 44 + 2);
    serialize_text_lowercase_response_array_into(arr, json);
    return json;
}

inline void serialize_text_lowercase_response_array_into(const arena_vector<text_lowercase_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_lowercase_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_lowercase_response_array(const arena_vector<text_lowercase_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 44 + 2);
    serialize_text_lowercase_response_array_into(arr, json);
    return json;
}

inline void serialize_text_lowercase_response_Result_array_into(const std::vector<text_lowercase_response_Result>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_lowercase_response_Result_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_lowercase_response_Result_array(const std::vector<text_lowercase_response_Result>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_lowercase_response_Result_array_into(arr, json);
    return json;
}

inline void serialize_text_lowercase_response_Result_array_into(const arena_vector<text_lowercase_response_Result>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_lowercase_response_Result_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_lowercase_response_Result_array(const arena_vector<text_lowercase_response_Result>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_lowercase_response_Result_array_into(arr, json);
    return json;
}

inline void serialize_text_reverse_request_array_into(const std::vector<text_reverse_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_reverse_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_reverse_request_array(const std::vector<text_reverse_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 42 + 2);
    serialize_text_reverse_request_array_into(arr, json);
    return json;
}

inline void serialize_text_reverse_request_array_into(const arena_vector<text_reverse_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_reverse_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_reverse_request_array(const arena_vector<text_reverse_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 42 + 2);
    serialize_text_reverse_request_array_into(arr, json);
    return json;
}

inline void serialize_text_reverse_request_Text_array_into(const std::vector<text_reverse_request_Text>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_reverse_request_Text_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_reverse_request_Text_array(const std::vector<text_reverse_request_Text>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_reverse_request_Text_array_into(arr, json);
    return json;
}

inline void serialize_text_reverse_request_Text_array_into(const arena_vector<text_reverse_request_Text>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_reverse_request_Text_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_reverse_request_Text_array(const arena_vector<text_reverse_request_Text>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_reverse_request_Text_array_into(arr, json);
    return json;
}

inline void serialize_text_reverse_response_array_into(const std::vector<text_reverse_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_reverse_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_reverse_response_array(const std::vector<text_reverse_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 44 + 2);
    serialize_text_reverse_response_array_into(arr, json);
    return json;
}

inline void serialize_text_reverse_response_array_into(const arena_vector<text_reverse_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_reverse_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_reverse_response_array(const arena_vector<text_reverse_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 44 + 2);
    serialize_text_reverse_response_array_into(arr, json);
    return json;
}

inline void serialize_text_reverse_response_Result_array_into(const std::vector<text_reverse_response_Result>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_reverse_response_Result_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_reverse_response_Result_array(const std::vector<text_reverse_response_Result>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_reverse_response_Result_array_into(arr, json);
    return json;
}

inline void serialize_text_reverse_response_Result_array_into(const arena_vector<text_reverse_response_Result>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_reverse_response_Result_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_reverse_response_Result_array(const arena_vector<text_reverse_response_Result>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_reverse_response_Result_array_into(arr, json);
    return json;
}

inline void serialize_text_stats_request_array_into(const std::vector<text_stats_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_stats_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_stats_request_array(const std::vector<text_stats_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 42 + 2);
    serialize_text_stats_request_array_into(arr, json);
    return json;
}

inline void serialize_text_stats_request_array_into(const arena_vector<text_stats_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_stats_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_stats_request_array(const arena_vector<text_stats_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 42 + 2);
    serialize_text_stats_request_array_into(arr, json);
    return json;
}

inline void serialize_text_stats_request_Text_array_into(const std::vector<text_stats_request_Text>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_stats_request_Text_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_stats_request_Text_array(const std::vector<text_stats_request_Text>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_stats_request_Text_array_into(arr, json);
    return json;
}

inline void serialize_text_stats_request_Text_array_into(const arena_vector<text_stats_request_Text>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_stats_request_Text_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_stats_request_Text_array(const arena_vector<text_stats_request_Text>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_stats_request_Text_array_into(arr, json);
    return json;
}

inline void serialize_text_stats_response_array_into(const std::vector<text_stats_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_stats_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_stats_response_array(const std::vector<text_stats_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 89 + 2);
    serialize_text_stats_response_array_into(arr, json);
    return json;
}

inline void serialize_text_stats_response_array_into(const arena_vector<text_stats_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_stats_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_stats_response_array(const arena_vector<text_stats_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 89 + 2);
    serialize_text_stats_response_array_into(arr, json);
    return json;
}

inline void serialize_text_stats_response_Chars_array_into(const std::vector<text_stats_response_Chars>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_stats_response_Chars_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_stats_response_Chars_array(const std::vector<text_stats_response_Chars>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_text_stats_response_Chars_array_into(arr, json);
    return json;
}

inline void serialize_text_stats_response_Chars_array_into(const arena_vector<text_stats_response_Chars>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_stats_response_Chars_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_stats_response_Chars_array(const arena_vector<text_stats_response_Chars>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_text_stats_response_Chars_array_into(arr, json);
    return json;
}

inline void serialize_text_stats_response_Words_array_into(const std::vector<text_stats_response_Words>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_stats_response_Words_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_stats_response_Words_array(const std::vector<text_stats_response_Words>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_text_stats_response_Words_array_into(arr, json);
    return json;
}

inline void serialize_text_stats_response_Words_array_into(const arena_vector<text_stats_response_Words>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_stats_response_Words_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_stats_response_Words_array(const arena_vector<text_stats_response_Words>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_text_stats_response_Words_array_into(arr, json);
    return json;
}

inline void serialize_text_stats_response_Lines_array_into(const std::vector<text_stats_response_Lines>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_stats_response_Lines_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_stats_response_Lines_array(const std::vector<text_stats_response_Lines>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_text_stats_response_Lines_array_into(arr, json);
    return json;
}

inline void serialize_text_stats_response_Lines_array_into(const arena_vector<text_stats_response_Lines>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_stats_response_Lines_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_stats_response_Lines_array(const arena_vector<text_stats_response_Lines>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_text_stats_response_Lines_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_request_array_into(const std::vector<text_transform_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_request_array(const std::vector<text_transform_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 100 + 2);
    serialize_text_transform_request_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_request_array_into(const arena_vector<text_transform_request>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_request_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_request_array(const arena_vector<text_transform_request>& arr) {
    std::string json;
    json.reserve(arr.size() * 100 + 2);
    serialize_text_transform_request_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_request_Text_array_into(const std::vector<text_transform_request_Text>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_request_Text_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_request_Text_array(const std::vector<text_transform_request_Text>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_transform_request_Text_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_request_Text_array_into(const arena_vector<text_transform_request_Text>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_request_Text_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_request_Text_array(const arena_vector<text_transform_request_Text>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_transform_request_Text_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_operation_array_into(const std::vector<text_transform_operation>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_operation_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_operation_array(const std::vector<text_transform_operation>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_transform_operation_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_operation_array_into(const arena_vector<text_transform_operation>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_operation_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_operation_array(const arena_vector<text_transform_operation>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_transform_operation_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_request_Trim_array_into(const std::vector<text_transform_request_Trim>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_request_Trim_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_request_Trim_array(const std::vector<text_transform_request_Trim>& arr) {
    std::string json;
    json.reserve(arr.size() * 5 + 2);
    serialize_text_transform_request_Trim_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_request_Trim_array_into(const arena_vector<text_transform_request_Trim>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_request_Trim_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_request_Trim_array(const arena_vector<text_transform_request_Trim>& arr) {
    std::string json;
    json.reserve(arr.size() * 5 + 2);
    serialize_text_transform_request_Trim_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_response_array_into(const std::vector<text_transform_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_response_array(const std::vector<text_transform_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 136 + 2);
    serialize_text_transform_response_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_response_array_into(const arena_vector<text_transform_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_response_array(const arena_vector<text_transform_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 136 + 2);
    serialize_text_transform_response_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_response_Original_length_array_into(const std::vector<text_transform_response_Original_length>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_response_Original_length_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_response_Original_length_array(const std::vector<text_transform_response_Original_length>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_text_transform_response_Original_length_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_response_Original_length_array_into(const arena_vector<text_transform_response_Original_length>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_response_Original_length_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_response_Original_length_array(const arena_vector<text_transform_response_Original_length>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_text_transform_response_Original_length_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_response_Result_array_into(const std::vector<text_transform_response_Result>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_response_Result_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_response_Result_array(const std::vector<text_transform_response_Result>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_transform_response_Result_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_response_Result_array_into(const arena_vector<text_transform_response_Result>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_response_Result_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_response_Result_array(const arena_vector<text_transform_response_Result>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_transform_response_Result_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_response_Operation_applied_array_into(const std::vector<text_transform_response_Operation_applied>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_response_Operation_applied_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_response_Operation_applied_array(const std::vector<text_transform_response_Operation_applied>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_transform_response_Operation_applied_array_into(arr, json);
    return json;
}

inline void serialize_text_transform_response_Operation_applied_array_into(const arena_vector<text_transform_response_Operation_applied>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_text_transform_response_Operation_applied_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_text_transform_response_Operation_applied_array(const arena_vector<text_transform_response_Operation_applied>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_text_transform_response_Operation_applied_array_into(arr, json);
    return json;
}

