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

[[nodiscard]] inline std::optional<PetCreateRequest> parse_PetCreateRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<PetCreateRequest_Name_t> parse_PetCreateRequest_Name_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<PetCreateRequest_Age_t> parse_PetCreateRequest_Age_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<PetCreateRequest_OwnerEmail_t> parse_PetCreateRequest_OwnerEmail_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<PetResponse> parse_PetResponse(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<PetResponse_Id_t> parse_PetResponse_Id_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<PetResponse_Name_t> parse_PetResponse_Name_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<PetResponse_Age_t> parse_PetResponse_Age_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<PetResponse_OwnerEmail_t> parse_PetResponse_OwnerEmail_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ListPetsResponse> parse_ListPetsResponse(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ListPetsResponse_Trace_t> parse_ListPetsResponse_Trace_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ListPetsResponse_LimitApplied_t> parse_ListPetsResponse_LimitApplied_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ListPetsResponse_Session_t> parse_ListPetsResponse_Session_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<listPets_param_limit> parse_listPets_param_limit(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<listPets_param_X_Trace> parse_listPets_param_X_Trace(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<listPets_param_session> parse_listPets_param_session(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema> parse_schema(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_1> parse_schema_1(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_2> parse_schema_2(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_3> parse_schema_3(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_4> parse_schema_4(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<getPet_param_petId> parse_getPet_param_petId(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_5> parse_schema_5(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_6> parse_schema_6(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<deletePet_param_petId> parse_deletePet_param_petId(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<PetCreateRequest> parse_PetCreateRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<PetCreateRequest_Name_t> parse_PetCreateRequest_Name_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<PetCreateRequest_Age_t> parse_PetCreateRequest_Age_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<PetCreateRequest_OwnerEmail_t> parse_PetCreateRequest_OwnerEmail_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<PetResponse> parse_PetResponse(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<PetResponse_Id_t> parse_PetResponse_Id_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<PetResponse_Name_t> parse_PetResponse_Name_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<PetResponse_Age_t> parse_PetResponse_Age_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<PetResponse_OwnerEmail_t> parse_PetResponse_OwnerEmail_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ListPetsResponse> parse_ListPetsResponse(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ListPetsResponse_Trace_t> parse_ListPetsResponse_Trace_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ListPetsResponse_LimitApplied_t> parse_ListPetsResponse_LimitApplied_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ListPetsResponse_Session_t> parse_ListPetsResponse_Session_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<listPets_param_limit> parse_listPets_param_limit(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<listPets_param_X_Trace> parse_listPets_param_X_Trace(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<listPets_param_session> parse_listPets_param_session(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema> parse_schema(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_1> parse_schema_1(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_2> parse_schema_2(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_3> parse_schema_3(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_4> parse_schema_4(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<getPet_param_petId> parse_getPet_param_petId(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_5> parse_schema_5(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_6> parse_schema_6(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<deletePet_param_petId> parse_deletePet_param_petId(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_PetCreateRequest_into(const PetCreateRequest& obj, std::string& out);
inline void serialize_PetCreateRequest_Name_t_into(const PetCreateRequest_Name_t& obj, std::string& out);
inline void serialize_PetCreateRequest_Age_t_into(const PetCreateRequest_Age_t& obj, std::string& out);
inline void serialize_PetCreateRequest_OwnerEmail_t_into(const PetCreateRequest_OwnerEmail_t& obj, std::string& out);
inline void serialize_PetResponse_into(const PetResponse& obj, std::string& out);
inline void serialize_PetResponse_Id_t_into(const PetResponse_Id_t& obj, std::string& out);
inline void serialize_PetResponse_Name_t_into(const PetResponse_Name_t& obj, std::string& out);
inline void serialize_PetResponse_Age_t_into(const PetResponse_Age_t& obj, std::string& out);
inline void serialize_PetResponse_OwnerEmail_t_into(const PetResponse_OwnerEmail_t& obj, std::string& out);
inline void serialize_ListPetsResponse_into(const ListPetsResponse& obj, std::string& out);
inline void serialize_ListPetsResponse_Trace_t_into(const ListPetsResponse_Trace_t& obj, std::string& out);
inline void serialize_ListPetsResponse_LimitApplied_t_into(const ListPetsResponse_LimitApplied_t& obj, std::string& out);
inline void serialize_ListPetsResponse_Session_t_into(const ListPetsResponse_Session_t& obj, std::string& out);
inline void serialize_listPets_param_limit_into(const listPets_param_limit& obj, std::string& out);
inline void serialize_listPets_param_X_Trace_into(const listPets_param_X_Trace& obj, std::string& out);
inline void serialize_listPets_param_session_into(const listPets_param_session& obj, std::string& out);
inline void serialize_schema_into(const schema& obj, std::string& out);
inline void serialize_schema_1_into(const schema_1& obj, std::string& out);
inline void serialize_schema_2_into(const schema_2& obj, std::string& out);
inline void serialize_schema_3_into(const schema_3& obj, std::string& out);
inline void serialize_schema_4_into(const schema_4& obj, std::string& out);
inline void serialize_getPet_param_petId_into(const getPet_param_petId& obj, std::string& out);
inline void serialize_schema_5_into(const schema_5& obj, std::string& out);
inline void serialize_schema_6_into(const schema_6& obj, std::string& out);
inline void serialize_deletePet_param_petId_into(const deletePet_param_petId& obj, std::string& out);

inline std::string serialize_PetCreateRequest(const PetCreateRequest& obj);
inline std::string serialize_PetCreateRequest_Name_t(const PetCreateRequest_Name_t& obj);
inline std::string serialize_PetCreateRequest_Age_t(const PetCreateRequest_Age_t& obj);
inline std::string serialize_PetCreateRequest_OwnerEmail_t(const PetCreateRequest_OwnerEmail_t& obj);
inline std::string serialize_PetResponse(const PetResponse& obj);
inline std::string serialize_PetResponse_Id_t(const PetResponse_Id_t& obj);
inline std::string serialize_PetResponse_Name_t(const PetResponse_Name_t& obj);
inline std::string serialize_PetResponse_Age_t(const PetResponse_Age_t& obj);
inline std::string serialize_PetResponse_OwnerEmail_t(const PetResponse_OwnerEmail_t& obj);
inline std::string serialize_ListPetsResponse(const ListPetsResponse& obj);
inline std::string serialize_ListPetsResponse_Trace_t(const ListPetsResponse_Trace_t& obj);
inline std::string serialize_ListPetsResponse_LimitApplied_t(const ListPetsResponse_LimitApplied_t& obj);
inline std::string serialize_ListPetsResponse_Session_t(const ListPetsResponse_Session_t& obj);
inline std::string serialize_listPets_param_limit(const listPets_param_limit& obj);
inline std::string serialize_listPets_param_X_Trace(const listPets_param_X_Trace& obj);
inline std::string serialize_listPets_param_session(const listPets_param_session& obj);
inline std::string serialize_schema(const schema& obj);
inline std::string serialize_schema_1(const schema_1& obj);
inline std::string serialize_schema_2(const schema_2& obj);
inline std::string serialize_schema_3(const schema_3& obj);
inline std::string serialize_schema_4(const schema_4& obj);
inline std::string serialize_getPet_param_petId(const getPet_param_petId& obj);
inline std::string serialize_schema_5(const schema_5& obj);
inline std::string serialize_schema_6(const schema_6& obj);
inline std::string serialize_deletePet_param_petId(const deletePet_param_petId& obj);

[[nodiscard]] inline std::optional<std::vector<PetCreateRequest>> parse_PetCreateRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<PetCreateRequest_Name_t>> parse_PetCreateRequest_Name_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<PetCreateRequest_Age_t>> parse_PetCreateRequest_Age_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<PetCreateRequest_OwnerEmail_t>> parse_PetCreateRequest_OwnerEmail_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<PetResponse>> parse_PetResponse_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<PetResponse_Id_t>> parse_PetResponse_Id_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<PetResponse_Name_t>> parse_PetResponse_Name_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<PetResponse_Age_t>> parse_PetResponse_Age_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<PetResponse_OwnerEmail_t>> parse_PetResponse_OwnerEmail_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ListPetsResponse>> parse_ListPetsResponse_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ListPetsResponse_Trace_t>> parse_ListPetsResponse_Trace_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ListPetsResponse_LimitApplied_t>> parse_ListPetsResponse_LimitApplied_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ListPetsResponse_Session_t>> parse_ListPetsResponse_Session_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<listPets_param_limit>> parse_listPets_param_limit_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<listPets_param_X_Trace>> parse_listPets_param_X_Trace_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<listPets_param_session>> parse_listPets_param_session_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_1>> parse_schema_1_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_2>> parse_schema_2_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_3>> parse_schema_3_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_4>> parse_schema_4_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<getPet_param_petId>> parse_getPet_param_petId_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_5>> parse_schema_5_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_6>> parse_schema_6_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<deletePet_param_petId>> parse_deletePet_param_petId_array(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<std::vector<PetCreateRequest>> parse_PetCreateRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<PetCreateRequest_Name_t>> parse_PetCreateRequest_Name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<PetCreateRequest_Age_t>> parse_PetCreateRequest_Age_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<PetCreateRequest_OwnerEmail_t>> parse_PetCreateRequest_OwnerEmail_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<PetResponse>> parse_PetResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<PetResponse_Id_t>> parse_PetResponse_Id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<PetResponse_Name_t>> parse_PetResponse_Name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<PetResponse_Age_t>> parse_PetResponse_Age_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<PetResponse_OwnerEmail_t>> parse_PetResponse_OwnerEmail_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ListPetsResponse>> parse_ListPetsResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ListPetsResponse_Trace_t>> parse_ListPetsResponse_Trace_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ListPetsResponse_LimitApplied_t>> parse_ListPetsResponse_LimitApplied_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ListPetsResponse_Session_t>> parse_ListPetsResponse_Session_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<listPets_param_limit>> parse_listPets_param_limit_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<listPets_param_X_Trace>> parse_listPets_param_X_Trace_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<listPets_param_session>> parse_listPets_param_session_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_1>> parse_schema_1_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_2>> parse_schema_2_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_3>> parse_schema_3_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_4>> parse_schema_4_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<getPet_param_petId>> parse_getPet_param_petId_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_5>> parse_schema_5_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_6>> parse_schema_6_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<deletePet_param_petId>> parse_deletePet_param_petId_array(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_PetCreateRequest_array_into(const std::vector<PetCreateRequest>& arr, std::string& out);
inline void serialize_PetCreateRequest_array_into(const arena_vector<PetCreateRequest>& arr, std::string& out);
inline void serialize_PetCreateRequest_Name_t_array_into(const std::vector<PetCreateRequest_Name_t>& arr, std::string& out);
inline void serialize_PetCreateRequest_Name_t_array_into(const arena_vector<PetCreateRequest_Name_t>& arr, std::string& out);
inline void serialize_PetCreateRequest_Age_t_array_into(const std::vector<PetCreateRequest_Age_t>& arr, std::string& out);
inline void serialize_PetCreateRequest_Age_t_array_into(const arena_vector<PetCreateRequest_Age_t>& arr, std::string& out);
inline void serialize_PetCreateRequest_OwnerEmail_t_array_into(const std::vector<PetCreateRequest_OwnerEmail_t>& arr, std::string& out);
inline void serialize_PetCreateRequest_OwnerEmail_t_array_into(const arena_vector<PetCreateRequest_OwnerEmail_t>& arr, std::string& out);
inline void serialize_PetResponse_array_into(const std::vector<PetResponse>& arr, std::string& out);
inline void serialize_PetResponse_array_into(const arena_vector<PetResponse>& arr, std::string& out);
inline void serialize_PetResponse_Id_t_array_into(const std::vector<PetResponse_Id_t>& arr, std::string& out);
inline void serialize_PetResponse_Id_t_array_into(const arena_vector<PetResponse_Id_t>& arr, std::string& out);
inline void serialize_PetResponse_Name_t_array_into(const std::vector<PetResponse_Name_t>& arr, std::string& out);
inline void serialize_PetResponse_Name_t_array_into(const arena_vector<PetResponse_Name_t>& arr, std::string& out);
inline void serialize_PetResponse_Age_t_array_into(const std::vector<PetResponse_Age_t>& arr, std::string& out);
inline void serialize_PetResponse_Age_t_array_into(const arena_vector<PetResponse_Age_t>& arr, std::string& out);
inline void serialize_PetResponse_OwnerEmail_t_array_into(const std::vector<PetResponse_OwnerEmail_t>& arr, std::string& out);
inline void serialize_PetResponse_OwnerEmail_t_array_into(const arena_vector<PetResponse_OwnerEmail_t>& arr, std::string& out);
inline void serialize_ListPetsResponse_array_into(const std::vector<ListPetsResponse>& arr, std::string& out);
inline void serialize_ListPetsResponse_array_into(const arena_vector<ListPetsResponse>& arr, std::string& out);
inline void serialize_ListPetsResponse_Trace_t_array_into(const std::vector<ListPetsResponse_Trace_t>& arr, std::string& out);
inline void serialize_ListPetsResponse_Trace_t_array_into(const arena_vector<ListPetsResponse_Trace_t>& arr, std::string& out);
inline void serialize_ListPetsResponse_LimitApplied_t_array_into(const std::vector<ListPetsResponse_LimitApplied_t>& arr, std::string& out);
inline void serialize_ListPetsResponse_LimitApplied_t_array_into(const arena_vector<ListPetsResponse_LimitApplied_t>& arr, std::string& out);
inline void serialize_ListPetsResponse_Session_t_array_into(const std::vector<ListPetsResponse_Session_t>& arr, std::string& out);
inline void serialize_ListPetsResponse_Session_t_array_into(const arena_vector<ListPetsResponse_Session_t>& arr, std::string& out);
inline void serialize_listPets_param_limit_array_into(const std::vector<listPets_param_limit>& arr, std::string& out);
inline void serialize_listPets_param_limit_array_into(const arena_vector<listPets_param_limit>& arr, std::string& out);
inline void serialize_listPets_param_X_Trace_array_into(const std::vector<listPets_param_X_Trace>& arr, std::string& out);
inline void serialize_listPets_param_X_Trace_array_into(const arena_vector<listPets_param_X_Trace>& arr, std::string& out);
inline void serialize_listPets_param_session_array_into(const std::vector<listPets_param_session>& arr, std::string& out);
inline void serialize_listPets_param_session_array_into(const arena_vector<listPets_param_session>& arr, std::string& out);
inline void serialize_schema_array_into(const std::vector<schema>& arr, std::string& out);
inline void serialize_schema_array_into(const arena_vector<schema>& arr, std::string& out);
inline void serialize_schema_1_array_into(const std::vector<schema_1>& arr, std::string& out);
inline void serialize_schema_1_array_into(const arena_vector<schema_1>& arr, std::string& out);
inline void serialize_schema_2_array_into(const std::vector<schema_2>& arr, std::string& out);
inline void serialize_schema_2_array_into(const arena_vector<schema_2>& arr, std::string& out);
inline void serialize_schema_3_array_into(const std::vector<schema_3>& arr, std::string& out);
inline void serialize_schema_3_array_into(const arena_vector<schema_3>& arr, std::string& out);
inline void serialize_schema_4_array_into(const std::vector<schema_4>& arr, std::string& out);
inline void serialize_schema_4_array_into(const arena_vector<schema_4>& arr, std::string& out);
inline void serialize_getPet_param_petId_array_into(const std::vector<getPet_param_petId>& arr, std::string& out);
inline void serialize_getPet_param_petId_array_into(const arena_vector<getPet_param_petId>& arr, std::string& out);
inline void serialize_schema_5_array_into(const std::vector<schema_5>& arr, std::string& out);
inline void serialize_schema_5_array_into(const arena_vector<schema_5>& arr, std::string& out);
inline void serialize_schema_6_array_into(const std::vector<schema_6>& arr, std::string& out);
inline void serialize_schema_6_array_into(const arena_vector<schema_6>& arr, std::string& out);
inline void serialize_deletePet_param_petId_array_into(const std::vector<deletePet_param_petId>& arr, std::string& out);
inline void serialize_deletePet_param_petId_array_into(const arena_vector<deletePet_param_petId>& arr, std::string& out);

inline std::string serialize_PetCreateRequest_array(const std::vector<PetCreateRequest>& arr);
inline std::string serialize_PetCreateRequest_array(const arena_vector<PetCreateRequest>& arr);
inline std::string serialize_PetCreateRequest_Name_t_array(const std::vector<PetCreateRequest_Name_t>& arr);
inline std::string serialize_PetCreateRequest_Name_t_array(const arena_vector<PetCreateRequest_Name_t>& arr);
inline std::string serialize_PetCreateRequest_Age_t_array(const std::vector<PetCreateRequest_Age_t>& arr);
inline std::string serialize_PetCreateRequest_Age_t_array(const arena_vector<PetCreateRequest_Age_t>& arr);
inline std::string serialize_PetCreateRequest_OwnerEmail_t_array(const std::vector<PetCreateRequest_OwnerEmail_t>& arr);
inline std::string serialize_PetCreateRequest_OwnerEmail_t_array(const arena_vector<PetCreateRequest_OwnerEmail_t>& arr);
inline std::string serialize_PetResponse_array(const std::vector<PetResponse>& arr);
inline std::string serialize_PetResponse_array(const arena_vector<PetResponse>& arr);
inline std::string serialize_PetResponse_Id_t_array(const std::vector<PetResponse_Id_t>& arr);
inline std::string serialize_PetResponse_Id_t_array(const arena_vector<PetResponse_Id_t>& arr);
inline std::string serialize_PetResponse_Name_t_array(const std::vector<PetResponse_Name_t>& arr);
inline std::string serialize_PetResponse_Name_t_array(const arena_vector<PetResponse_Name_t>& arr);
inline std::string serialize_PetResponse_Age_t_array(const std::vector<PetResponse_Age_t>& arr);
inline std::string serialize_PetResponse_Age_t_array(const arena_vector<PetResponse_Age_t>& arr);
inline std::string serialize_PetResponse_OwnerEmail_t_array(const std::vector<PetResponse_OwnerEmail_t>& arr);
inline std::string serialize_PetResponse_OwnerEmail_t_array(const arena_vector<PetResponse_OwnerEmail_t>& arr);
inline std::string serialize_ListPetsResponse_array(const std::vector<ListPetsResponse>& arr);
inline std::string serialize_ListPetsResponse_array(const arena_vector<ListPetsResponse>& arr);
inline std::string serialize_ListPetsResponse_Trace_t_array(const std::vector<ListPetsResponse_Trace_t>& arr);
inline std::string serialize_ListPetsResponse_Trace_t_array(const arena_vector<ListPetsResponse_Trace_t>& arr);
inline std::string serialize_ListPetsResponse_LimitApplied_t_array(const std::vector<ListPetsResponse_LimitApplied_t>& arr);
inline std::string serialize_ListPetsResponse_LimitApplied_t_array(const arena_vector<ListPetsResponse_LimitApplied_t>& arr);
inline std::string serialize_ListPetsResponse_Session_t_array(const std::vector<ListPetsResponse_Session_t>& arr);
inline std::string serialize_ListPetsResponse_Session_t_array(const arena_vector<ListPetsResponse_Session_t>& arr);
inline std::string serialize_listPets_param_limit_array(const std::vector<listPets_param_limit>& arr);
inline std::string serialize_listPets_param_limit_array(const arena_vector<listPets_param_limit>& arr);
inline std::string serialize_listPets_param_X_Trace_array(const std::vector<listPets_param_X_Trace>& arr);
inline std::string serialize_listPets_param_X_Trace_array(const arena_vector<listPets_param_X_Trace>& arr);
inline std::string serialize_listPets_param_session_array(const std::vector<listPets_param_session>& arr);
inline std::string serialize_listPets_param_session_array(const arena_vector<listPets_param_session>& arr);
inline std::string serialize_schema_array(const std::vector<schema>& arr);
inline std::string serialize_schema_array(const arena_vector<schema>& arr);
inline std::string serialize_schema_1_array(const std::vector<schema_1>& arr);
inline std::string serialize_schema_1_array(const arena_vector<schema_1>& arr);
inline std::string serialize_schema_2_array(const std::vector<schema_2>& arr);
inline std::string serialize_schema_2_array(const arena_vector<schema_2>& arr);
inline std::string serialize_schema_3_array(const std::vector<schema_3>& arr);
inline std::string serialize_schema_3_array(const arena_vector<schema_3>& arr);
inline std::string serialize_schema_4_array(const std::vector<schema_4>& arr);
inline std::string serialize_schema_4_array(const arena_vector<schema_4>& arr);
inline std::string serialize_getPet_param_petId_array(const std::vector<getPet_param_petId>& arr);
inline std::string serialize_getPet_param_petId_array(const arena_vector<getPet_param_petId>& arr);
inline std::string serialize_schema_5_array(const std::vector<schema_5>& arr);
inline std::string serialize_schema_5_array(const arena_vector<schema_5>& arr);
inline std::string serialize_schema_6_array(const std::vector<schema_6>& arr);
inline std::string serialize_schema_6_array(const arena_vector<schema_6>& arr);
inline std::string serialize_deletePet_param_petId_array(const std::vector<deletePet_param_petId>& arr);
inline std::string serialize_deletePet_param_petId_array(const arena_vector<deletePet_param_petId>& arr);

// ============================================================
// JSON Parse Functions
// ============================================================

[[nodiscard]] inline std::optional<PetCreateRequest> parse_PetCreateRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    PetCreateRequest obj(arena);
    bool has_name = false;
    bool has_age = false;
    bool has_ownerEmail = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "name") {
            has_name = true;
            if (auto v = cur.string()) {
                obj.name = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else         if (*key == "age") {
            has_age = true;
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.age = *v;
            } else { cur.skip_value(); }
        } else         if (*key == "ownerEmail") {
            has_ownerEmail = true;
            if (auto v = cur.string()) {
                obj.ownerEmail = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_name) return std::nullopt;
    if (!has_age) return std::nullopt;
    if (!has_ownerEmail) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<PetCreateRequest> parse_PetCreateRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_PetCreateRequest(cur, arena);
}

[[nodiscard]] inline std::optional<PetCreateRequest_Name_t> parse_PetCreateRequest_Name_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return PetCreateRequest_Name_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<PetCreateRequest_Name_t> parse_PetCreateRequest_Name_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_PetCreateRequest_Name_t(cur, arena);
}

[[nodiscard]] inline std::optional<PetCreateRequest_Age_t> parse_PetCreateRequest_Age_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return PetCreateRequest_Age_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<PetCreateRequest_Age_t> parse_PetCreateRequest_Age_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_PetCreateRequest_Age_t(cur, arena);
}

[[nodiscard]] inline std::optional<PetCreateRequest_OwnerEmail_t> parse_PetCreateRequest_OwnerEmail_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return PetCreateRequest_OwnerEmail_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<PetCreateRequest_OwnerEmail_t> parse_PetCreateRequest_OwnerEmail_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_PetCreateRequest_OwnerEmail_t(cur, arena);
}

[[nodiscard]] inline std::optional<PetResponse> parse_PetResponse(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    PetResponse obj(arena);
    bool has_id = false;
    bool has_name = false;
    bool has_age = false;
    bool has_ownerEmail = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 2:
            if (*key == "id") {
                has_id = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.id = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 3:
            if (*key == "age") {
                has_age = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.age = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 4:
            if (*key == "name") {
                has_name = true;
                if (auto v = cur.string()) {
                    obj.name = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 10:
            if (*key == "ownerEmail") {
                has_ownerEmail = true;
                if (auto v = cur.string()) {
                    obj.ownerEmail = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!has_id) return std::nullopt;
    if (!has_name) return std::nullopt;
    if (!has_age) return std::nullopt;
    if (!has_ownerEmail) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<PetResponse> parse_PetResponse(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_PetResponse(cur, arena);
}

[[nodiscard]] inline std::optional<PetResponse_Id_t> parse_PetResponse_Id_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return PetResponse_Id_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<PetResponse_Id_t> parse_PetResponse_Id_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_PetResponse_Id_t(cur, arena);
}

[[nodiscard]] inline std::optional<PetResponse_Name_t> parse_PetResponse_Name_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return PetResponse_Name_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<PetResponse_Name_t> parse_PetResponse_Name_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_PetResponse_Name_t(cur, arena);
}

[[nodiscard]] inline std::optional<PetResponse_Age_t> parse_PetResponse_Age_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return PetResponse_Age_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<PetResponse_Age_t> parse_PetResponse_Age_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_PetResponse_Age_t(cur, arena);
}

[[nodiscard]] inline std::optional<PetResponse_OwnerEmail_t> parse_PetResponse_OwnerEmail_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return PetResponse_OwnerEmail_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<PetResponse_OwnerEmail_t> parse_PetResponse_OwnerEmail_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_PetResponse_OwnerEmail_t(cur, arena);
}

[[nodiscard]] inline std::optional<ListPetsResponse> parse_ListPetsResponse(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    ListPetsResponse obj(arena);
    bool has_trace = false;
    bool has_limitApplied = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "trace") {
            has_trace = true;
            if (auto v = cur.string()) {
                obj.trace = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else         if (*key == "limitApplied") {
            has_limitApplied = true;
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.limitApplied = *v;
            } else { cur.skip_value(); }
        } else         if (*key == "session") {
            if (auto v = cur.string()) {
                obj.session = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_trace) return std::nullopt;
    if (!has_limitApplied) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<ListPetsResponse> parse_ListPetsResponse(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ListPetsResponse(cur, arena);
}

[[nodiscard]] inline std::optional<ListPetsResponse_Trace_t> parse_ListPetsResponse_Trace_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return ListPetsResponse_Trace_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<ListPetsResponse_Trace_t> parse_ListPetsResponse_Trace_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ListPetsResponse_Trace_t(cur, arena);
}

[[nodiscard]] inline std::optional<ListPetsResponse_LimitApplied_t> parse_ListPetsResponse_LimitApplied_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return ListPetsResponse_LimitApplied_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<ListPetsResponse_LimitApplied_t> parse_ListPetsResponse_LimitApplied_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ListPetsResponse_LimitApplied_t(cur, arena);
}

[[nodiscard]] inline std::optional<ListPetsResponse_Session_t> parse_ListPetsResponse_Session_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return ListPetsResponse_Session_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<ListPetsResponse_Session_t> parse_ListPetsResponse_Session_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ListPetsResponse_Session_t(cur, arena);
}

[[nodiscard]] inline std::optional<listPets_param_limit> parse_listPets_param_limit(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return listPets_param_limit{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<listPets_param_limit> parse_listPets_param_limit(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_listPets_param_limit(cur, arena);
}

[[nodiscard]] inline std::optional<listPets_param_X_Trace> parse_listPets_param_X_Trace(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return listPets_param_X_Trace{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<listPets_param_X_Trace> parse_listPets_param_X_Trace(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_listPets_param_X_Trace(cur, arena);
}

[[nodiscard]] inline std::optional<listPets_param_session> parse_listPets_param_session(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return listPets_param_session{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<listPets_param_session> parse_listPets_param_session(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_listPets_param_session(cur, arena);
}

[[nodiscard]] inline std::optional<schema> parse_schema(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema{};
}

[[nodiscard]] inline std::optional<schema> parse_schema(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema(cur, arena);
}

[[nodiscard]] inline std::optional<schema_1> parse_schema_1(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_1{};
}

[[nodiscard]] inline std::optional<schema_1> parse_schema_1(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_1(cur, arena);
}

[[nodiscard]] inline std::optional<schema_2> parse_schema_2(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_2{};
}

[[nodiscard]] inline std::optional<schema_2> parse_schema_2(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_2(cur, arena);
}

[[nodiscard]] inline std::optional<schema_3> parse_schema_3(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_3{};
}

[[nodiscard]] inline std::optional<schema_3> parse_schema_3(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_3(cur, arena);
}

[[nodiscard]] inline std::optional<schema_4> parse_schema_4(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_4{};
}

[[nodiscard]] inline std::optional<schema_4> parse_schema_4(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_4(cur, arena);
}

[[nodiscard]] inline std::optional<getPet_param_petId> parse_getPet_param_petId(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return getPet_param_petId{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<getPet_param_petId> parse_getPet_param_petId(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_getPet_param_petId(cur, arena);
}

[[nodiscard]] inline std::optional<schema_5> parse_schema_5(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_5{};
}

[[nodiscard]] inline std::optional<schema_5> parse_schema_5(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_5(cur, arena);
}

[[nodiscard]] inline std::optional<schema_6> parse_schema_6(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_6{};
}

[[nodiscard]] inline std::optional<schema_6> parse_schema_6(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_6(cur, arena);
}

[[nodiscard]] inline std::optional<deletePet_param_petId> parse_deletePet_param_petId(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur)) return deletePet_param_petId{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<deletePet_param_petId> parse_deletePet_param_petId(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_deletePet_param_petId(cur, arena);
}

// ============================================================
// JSON Serialize Functions
// ============================================================

inline void serialize_PetCreateRequest_into(const PetCreateRequest& obj, std::string& json) {
    json.push_back('{');
    json.append("\"name\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.name, json);
    json.push_back('"');
    json.append(",\"age\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.age);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"ownerEmail\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.ownerEmail, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_PetCreateRequest(const PetCreateRequest& obj) {
    std::string json;
    size_t reserve_estimate = 115;
    reserve_estimate += obj.name.size();
    reserve_estimate += obj.ownerEmail.size();
    json.reserve(reserve_estimate);
    serialize_PetCreateRequest_into(obj, json);
    return json;
}

inline void serialize_PetCreateRequest_Name_t_into(const PetCreateRequest_Name_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_PetCreateRequest_Name_t(const PetCreateRequest_Name_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_PetCreateRequest_Name_t_into(obj, json);
    return json;
}

inline void serialize_PetCreateRequest_Age_t_into(const PetCreateRequest_Age_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_PetCreateRequest_Age_t(const PetCreateRequest_Age_t& obj) {
    std::string json;
    serialize_PetCreateRequest_Age_t_into(obj, json);
    return json;
}

inline void serialize_PetCreateRequest_OwnerEmail_t_into(const PetCreateRequest_OwnerEmail_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_PetCreateRequest_OwnerEmail_t(const PetCreateRequest_OwnerEmail_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_PetCreateRequest_OwnerEmail_t_into(obj, json);
    return json;
}

inline void serialize_PetResponse_into(const PetResponse& obj, std::string& json) {
    json.push_back('{');
    json.append("\"id\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.id);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"name\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.name, json);
    json.push_back('"');
    json.append(",\"age\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.age);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"ownerEmail\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.ownerEmail, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_PetResponse(const PetResponse& obj) {
    std::string json;
    size_t reserve_estimate = 141;
    reserve_estimate += obj.name.size();
    reserve_estimate += obj.ownerEmail.size();
    json.reserve(reserve_estimate);
    serialize_PetResponse_into(obj, json);
    return json;
}

inline void serialize_PetResponse_Id_t_into(const PetResponse_Id_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_PetResponse_Id_t(const PetResponse_Id_t& obj) {
    std::string json;
    serialize_PetResponse_Id_t_into(obj, json);
    return json;
}

inline void serialize_PetResponse_Name_t_into(const PetResponse_Name_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_PetResponse_Name_t(const PetResponse_Name_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_PetResponse_Name_t_into(obj, json);
    return json;
}

inline void serialize_PetResponse_Age_t_into(const PetResponse_Age_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_PetResponse_Age_t(const PetResponse_Age_t& obj) {
    std::string json;
    serialize_PetResponse_Age_t_into(obj, json);
    return json;
}

inline void serialize_PetResponse_OwnerEmail_t_into(const PetResponse_OwnerEmail_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_PetResponse_OwnerEmail_t(const PetResponse_OwnerEmail_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_PetResponse_OwnerEmail_t_into(obj, json);
    return json;
}

inline void serialize_ListPetsResponse_into(const ListPetsResponse& obj, std::string& json) {
    json.push_back('{');
    json.append("\"trace\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.trace, json);
    json.push_back('"');
    json.append(",\"limitApplied\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.limitApplied);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"session\":");
    if (obj.session) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.session, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    json.push_back('}');
}

inline std::string serialize_ListPetsResponse(const ListPetsResponse& obj) {
    std::string json;
    size_t reserve_estimate = 122;
    reserve_estimate += obj.trace.size();
    if (obj.session) reserve_estimate += obj.session->size();
    json.reserve(reserve_estimate);
    serialize_ListPetsResponse_into(obj, json);
    return json;
}

inline void serialize_ListPetsResponse_Trace_t_into(const ListPetsResponse_Trace_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_ListPetsResponse_Trace_t(const ListPetsResponse_Trace_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_ListPetsResponse_Trace_t_into(obj, json);
    return json;
}

inline void serialize_ListPetsResponse_LimitApplied_t_into(const ListPetsResponse_LimitApplied_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_ListPetsResponse_LimitApplied_t(const ListPetsResponse_LimitApplied_t& obj) {
    std::string json;
    serialize_ListPetsResponse_LimitApplied_t_into(obj, json);
    return json;
}

inline void serialize_ListPetsResponse_Session_t_into(const ListPetsResponse_Session_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_ListPetsResponse_Session_t(const ListPetsResponse_Session_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_ListPetsResponse_Session_t_into(obj, json);
    return json;
}

inline void serialize_listPets_param_limit_into(const listPets_param_limit& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_listPets_param_limit(const listPets_param_limit& obj) {
    std::string json;
    serialize_listPets_param_limit_into(obj, json);
    return json;
}

inline void serialize_listPets_param_X_Trace_into(const listPets_param_X_Trace& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_listPets_param_X_Trace(const listPets_param_X_Trace& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_listPets_param_X_Trace_into(obj, json);
    return json;
}

inline void serialize_listPets_param_session_into(const listPets_param_session& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_listPets_param_session(const listPets_param_session& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_listPets_param_session_into(obj, json);
    return json;
}

inline void serialize_schema_into(const schema& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema(const schema& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_1_into(const schema_1& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_1(const schema_1& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_2_into(const schema_2& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_2(const schema_2& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_3_into(const schema_3& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_3(const schema_3& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_4_into(const schema_4& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_4(const schema_4& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_getPet_param_petId_into(const getPet_param_petId& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_getPet_param_petId(const getPet_param_petId& obj) {
    std::string json;
    serialize_getPet_param_petId_into(obj, json);
    return json;
}

inline void serialize_schema_5_into(const schema_5& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_5(const schema_5& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_6_into(const schema_6& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_6(const schema_6& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_deletePet_param_petId_into(const deletePet_param_petId& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_deletePet_param_petId(const deletePet_param_petId& obj) {
    std::string json;
    serialize_deletePet_param_petId_into(obj, json);
    return json;
}

// ============================================================
// Array Parse Functions
// ============================================================

[[nodiscard]] inline std::optional<std::vector<PetCreateRequest>> parse_PetCreateRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<PetCreateRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_PetCreateRequest(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<PetCreateRequest>> parse_PetCreateRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_PetCreateRequest_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<PetCreateRequest_Name_t>> parse_PetCreateRequest_Name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<PetCreateRequest_Name_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_PetCreateRequest_Name_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<PetCreateRequest_Name_t>> parse_PetCreateRequest_Name_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_PetCreateRequest_Name_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<PetCreateRequest_Age_t>> parse_PetCreateRequest_Age_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<PetCreateRequest_Age_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_PetCreateRequest_Age_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<PetCreateRequest_Age_t>> parse_PetCreateRequest_Age_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_PetCreateRequest_Age_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<PetCreateRequest_OwnerEmail_t>> parse_PetCreateRequest_OwnerEmail_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<PetCreateRequest_OwnerEmail_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_PetCreateRequest_OwnerEmail_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<PetCreateRequest_OwnerEmail_t>> parse_PetCreateRequest_OwnerEmail_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_PetCreateRequest_OwnerEmail_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<PetResponse>> parse_PetResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<PetResponse> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_PetResponse(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<PetResponse>> parse_PetResponse_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_PetResponse_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<PetResponse_Id_t>> parse_PetResponse_Id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<PetResponse_Id_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_PetResponse_Id_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<PetResponse_Id_t>> parse_PetResponse_Id_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_PetResponse_Id_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<PetResponse_Name_t>> parse_PetResponse_Name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<PetResponse_Name_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_PetResponse_Name_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<PetResponse_Name_t>> parse_PetResponse_Name_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_PetResponse_Name_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<PetResponse_Age_t>> parse_PetResponse_Age_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<PetResponse_Age_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_PetResponse_Age_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<PetResponse_Age_t>> parse_PetResponse_Age_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_PetResponse_Age_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<PetResponse_OwnerEmail_t>> parse_PetResponse_OwnerEmail_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<PetResponse_OwnerEmail_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_PetResponse_OwnerEmail_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<PetResponse_OwnerEmail_t>> parse_PetResponse_OwnerEmail_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_PetResponse_OwnerEmail_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ListPetsResponse>> parse_ListPetsResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ListPetsResponse> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ListPetsResponse(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ListPetsResponse>> parse_ListPetsResponse_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ListPetsResponse_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ListPetsResponse_Trace_t>> parse_ListPetsResponse_Trace_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ListPetsResponse_Trace_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ListPetsResponse_Trace_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ListPetsResponse_Trace_t>> parse_ListPetsResponse_Trace_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ListPetsResponse_Trace_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ListPetsResponse_LimitApplied_t>> parse_ListPetsResponse_LimitApplied_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ListPetsResponse_LimitApplied_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ListPetsResponse_LimitApplied_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ListPetsResponse_LimitApplied_t>> parse_ListPetsResponse_LimitApplied_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ListPetsResponse_LimitApplied_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ListPetsResponse_Session_t>> parse_ListPetsResponse_Session_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ListPetsResponse_Session_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ListPetsResponse_Session_t(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ListPetsResponse_Session_t>> parse_ListPetsResponse_Session_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ListPetsResponse_Session_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<listPets_param_limit>> parse_listPets_param_limit_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<listPets_param_limit> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_listPets_param_limit(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<listPets_param_limit>> parse_listPets_param_limit_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_listPets_param_limit_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<listPets_param_X_Trace>> parse_listPets_param_X_Trace_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<listPets_param_X_Trace> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_listPets_param_X_Trace(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<listPets_param_X_Trace>> parse_listPets_param_X_Trace_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_listPets_param_X_Trace_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<listPets_param_session>> parse_listPets_param_session_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<listPets_param_session> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_listPets_param_session(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<listPets_param_session>> parse_listPets_param_session_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_listPets_param_session_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_1>> parse_schema_1_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_1> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_1(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_1>> parse_schema_1_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_1_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_2>> parse_schema_2_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_2> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_2(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_2>> parse_schema_2_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_2_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_3>> parse_schema_3_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_3> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_3(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_3>> parse_schema_3_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_3_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_4>> parse_schema_4_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_4> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_4(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_4>> parse_schema_4_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_4_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<getPet_param_petId>> parse_getPet_param_petId_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<getPet_param_petId> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_getPet_param_petId(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<getPet_param_petId>> parse_getPet_param_petId_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_getPet_param_petId_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_5>> parse_schema_5_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_5> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_5(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_5>> parse_schema_5_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_5_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_6>> parse_schema_6_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<schema_6> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_schema_6(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_6>> parse_schema_6_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_6_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<deletePet_param_petId>> parse_deletePet_param_petId_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<deletePet_param_petId> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_deletePet_param_petId(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<deletePet_param_petId>> parse_deletePet_param_petId_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_deletePet_param_petId_array(cur, arena);
}

// ============================================================
// Array Serialize Functions
// ============================================================

inline void serialize_PetCreateRequest_array_into(const std::vector<PetCreateRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_PetCreateRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_PetCreateRequest_array(const std::vector<PetCreateRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 115 + 2);
    serialize_PetCreateRequest_array_into(arr, json);
    return json;
}

inline void serialize_PetCreateRequest_array_into(const arena_vector<PetCreateRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_PetCreateRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_PetCreateRequest_array(const arena_vector<PetCreateRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 115 + 2);
    serialize_PetCreateRequest_array_into(arr, json);
    return json;
}

inline void serialize_PetCreateRequest_Name_t_array_into(const std::vector<PetCreateRequest_Name_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_PetCreateRequest_Name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_PetCreateRequest_Name_t_array(const std::vector<PetCreateRequest_Name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_PetCreateRequest_Name_t_array_into(arr, json);
    return json;
}

inline void serialize_PetCreateRequest_Name_t_array_into(const arena_vector<PetCreateRequest_Name_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_PetCreateRequest_Name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_PetCreateRequest_Name_t_array(const arena_vector<PetCreateRequest_Name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_PetCreateRequest_Name_t_array_into(arr, json);
    return json;
}

inline void serialize_PetCreateRequest_Age_t_array_into(const std::vector<PetCreateRequest_Age_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_PetCreateRequest_Age_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_PetCreateRequest_Age_t_array(const std::vector<PetCreateRequest_Age_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_PetCreateRequest_Age_t_array_into(arr, json);
    return json;
}

inline void serialize_PetCreateRequest_Age_t_array_into(const arena_vector<PetCreateRequest_Age_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_PetCreateRequest_Age_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_PetCreateRequest_Age_t_array(const arena_vector<PetCreateRequest_Age_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_PetCreateRequest_Age_t_array_into(arr, json);
    return json;
}

inline void serialize_PetCreateRequest_OwnerEmail_t_array_into(const std::vector<PetCreateRequest_OwnerEmail_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_PetCreateRequest_OwnerEmail_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_PetCreateRequest_OwnerEmail_t_array(const std::vector<PetCreateRequest_OwnerEmail_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_PetCreateRequest_OwnerEmail_t_array_into(arr, json);
    return json;
}

inline void serialize_PetCreateRequest_OwnerEmail_t_array_into(const arena_vector<PetCreateRequest_OwnerEmail_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_PetCreateRequest_OwnerEmail_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_PetCreateRequest_OwnerEmail_t_array(const arena_vector<PetCreateRequest_OwnerEmail_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_PetCreateRequest_OwnerEmail_t_array_into(arr, json);
    return json;
}

inline void serialize_PetResponse_array_into(const std::vector<PetResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_PetResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_PetResponse_array(const std::vector<PetResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 141 + 2);
    serialize_PetResponse_array_into(arr, json);
    return json;
}

inline void serialize_PetResponse_array_into(const arena_vector<PetResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_PetResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_PetResponse_array(const arena_vector<PetResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 141 + 2);
    serialize_PetResponse_array_into(arr, json);
    return json;
}

inline void serialize_PetResponse_Id_t_array_into(const std::vector<PetResponse_Id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_PetResponse_Id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_PetResponse_Id_t_array(const std::vector<PetResponse_Id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_PetResponse_Id_t_array_into(arr, json);
    return json;
}

inline void serialize_PetResponse_Id_t_array_into(const arena_vector<PetResponse_Id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_PetResponse_Id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_PetResponse_Id_t_array(const arena_vector<PetResponse_Id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_PetResponse_Id_t_array_into(arr, json);
    return json;
}

inline void serialize_PetResponse_Name_t_array_into(const std::vector<PetResponse_Name_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_PetResponse_Name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_PetResponse_Name_t_array(const std::vector<PetResponse_Name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_PetResponse_Name_t_array_into(arr, json);
    return json;
}

inline void serialize_PetResponse_Name_t_array_into(const arena_vector<PetResponse_Name_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_PetResponse_Name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_PetResponse_Name_t_array(const arena_vector<PetResponse_Name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_PetResponse_Name_t_array_into(arr, json);
    return json;
}

inline void serialize_PetResponse_Age_t_array_into(const std::vector<PetResponse_Age_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_PetResponse_Age_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_PetResponse_Age_t_array(const std::vector<PetResponse_Age_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_PetResponse_Age_t_array_into(arr, json);
    return json;
}

inline void serialize_PetResponse_Age_t_array_into(const arena_vector<PetResponse_Age_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_PetResponse_Age_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_PetResponse_Age_t_array(const arena_vector<PetResponse_Age_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_PetResponse_Age_t_array_into(arr, json);
    return json;
}

inline void serialize_PetResponse_OwnerEmail_t_array_into(const std::vector<PetResponse_OwnerEmail_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_PetResponse_OwnerEmail_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_PetResponse_OwnerEmail_t_array(const std::vector<PetResponse_OwnerEmail_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_PetResponse_OwnerEmail_t_array_into(arr, json);
    return json;
}

inline void serialize_PetResponse_OwnerEmail_t_array_into(const arena_vector<PetResponse_OwnerEmail_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_PetResponse_OwnerEmail_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_PetResponse_OwnerEmail_t_array(const arena_vector<PetResponse_OwnerEmail_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_PetResponse_OwnerEmail_t_array_into(arr, json);
    return json;
}

inline void serialize_ListPetsResponse_array_into(const std::vector<ListPetsResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ListPetsResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ListPetsResponse_array(const std::vector<ListPetsResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 122 + 2);
    serialize_ListPetsResponse_array_into(arr, json);
    return json;
}

inline void serialize_ListPetsResponse_array_into(const arena_vector<ListPetsResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ListPetsResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ListPetsResponse_array(const arena_vector<ListPetsResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 122 + 2);
    serialize_ListPetsResponse_array_into(arr, json);
    return json;
}

inline void serialize_ListPetsResponse_Trace_t_array_into(const std::vector<ListPetsResponse_Trace_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ListPetsResponse_Trace_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ListPetsResponse_Trace_t_array(const std::vector<ListPetsResponse_Trace_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ListPetsResponse_Trace_t_array_into(arr, json);
    return json;
}

inline void serialize_ListPetsResponse_Trace_t_array_into(const arena_vector<ListPetsResponse_Trace_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ListPetsResponse_Trace_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ListPetsResponse_Trace_t_array(const arena_vector<ListPetsResponse_Trace_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ListPetsResponse_Trace_t_array_into(arr, json);
    return json;
}

inline void serialize_ListPetsResponse_LimitApplied_t_array_into(const std::vector<ListPetsResponse_LimitApplied_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ListPetsResponse_LimitApplied_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ListPetsResponse_LimitApplied_t_array(const std::vector<ListPetsResponse_LimitApplied_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_ListPetsResponse_LimitApplied_t_array_into(arr, json);
    return json;
}

inline void serialize_ListPetsResponse_LimitApplied_t_array_into(const arena_vector<ListPetsResponse_LimitApplied_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ListPetsResponse_LimitApplied_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ListPetsResponse_LimitApplied_t_array(const arena_vector<ListPetsResponse_LimitApplied_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_ListPetsResponse_LimitApplied_t_array_into(arr, json);
    return json;
}

inline void serialize_ListPetsResponse_Session_t_array_into(const std::vector<ListPetsResponse_Session_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ListPetsResponse_Session_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ListPetsResponse_Session_t_array(const std::vector<ListPetsResponse_Session_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ListPetsResponse_Session_t_array_into(arr, json);
    return json;
}

inline void serialize_ListPetsResponse_Session_t_array_into(const arena_vector<ListPetsResponse_Session_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ListPetsResponse_Session_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ListPetsResponse_Session_t_array(const arena_vector<ListPetsResponse_Session_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ListPetsResponse_Session_t_array_into(arr, json);
    return json;
}

inline void serialize_listPets_param_limit_array_into(const std::vector<listPets_param_limit>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_listPets_param_limit_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_listPets_param_limit_array(const std::vector<listPets_param_limit>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_listPets_param_limit_array_into(arr, json);
    return json;
}

inline void serialize_listPets_param_limit_array_into(const arena_vector<listPets_param_limit>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_listPets_param_limit_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_listPets_param_limit_array(const arena_vector<listPets_param_limit>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_listPets_param_limit_array_into(arr, json);
    return json;
}

inline void serialize_listPets_param_X_Trace_array_into(const std::vector<listPets_param_X_Trace>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_listPets_param_X_Trace_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_listPets_param_X_Trace_array(const std::vector<listPets_param_X_Trace>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_listPets_param_X_Trace_array_into(arr, json);
    return json;
}

inline void serialize_listPets_param_X_Trace_array_into(const arena_vector<listPets_param_X_Trace>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_listPets_param_X_Trace_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_listPets_param_X_Trace_array(const arena_vector<listPets_param_X_Trace>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_listPets_param_X_Trace_array_into(arr, json);
    return json;
}

inline void serialize_listPets_param_session_array_into(const std::vector<listPets_param_session>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_listPets_param_session_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_listPets_param_session_array(const std::vector<listPets_param_session>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_listPets_param_session_array_into(arr, json);
    return json;
}

inline void serialize_listPets_param_session_array_into(const arena_vector<listPets_param_session>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_listPets_param_session_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_listPets_param_session_array(const arena_vector<listPets_param_session>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_listPets_param_session_array_into(arr, json);
    return json;
}

inline void serialize_schema_array_into(const std::vector<schema>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_array(const std::vector<schema>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_array_into(arr, json);
    return json;
}

inline void serialize_schema_array_into(const arena_vector<schema>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_array(const arena_vector<schema>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_array_into(arr, json);
    return json;
}

inline void serialize_schema_1_array_into(const std::vector<schema_1>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_1_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_1_array(const std::vector<schema_1>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_1_array_into(arr, json);
    return json;
}

inline void serialize_schema_1_array_into(const arena_vector<schema_1>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_1_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_1_array(const arena_vector<schema_1>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_1_array_into(arr, json);
    return json;
}

inline void serialize_schema_2_array_into(const std::vector<schema_2>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_2_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_2_array(const std::vector<schema_2>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_2_array_into(arr, json);
    return json;
}

inline void serialize_schema_2_array_into(const arena_vector<schema_2>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_2_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_2_array(const arena_vector<schema_2>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_2_array_into(arr, json);
    return json;
}

inline void serialize_schema_3_array_into(const std::vector<schema_3>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_3_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_3_array(const std::vector<schema_3>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_3_array_into(arr, json);
    return json;
}

inline void serialize_schema_3_array_into(const arena_vector<schema_3>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_3_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_3_array(const arena_vector<schema_3>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_3_array_into(arr, json);
    return json;
}

inline void serialize_schema_4_array_into(const std::vector<schema_4>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_4_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_4_array(const std::vector<schema_4>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_4_array_into(arr, json);
    return json;
}

inline void serialize_schema_4_array_into(const arena_vector<schema_4>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_4_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_4_array(const arena_vector<schema_4>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_4_array_into(arr, json);
    return json;
}

inline void serialize_getPet_param_petId_array_into(const std::vector<getPet_param_petId>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_getPet_param_petId_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_getPet_param_petId_array(const std::vector<getPet_param_petId>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_getPet_param_petId_array_into(arr, json);
    return json;
}

inline void serialize_getPet_param_petId_array_into(const arena_vector<getPet_param_petId>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_getPet_param_petId_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_getPet_param_petId_array(const arena_vector<getPet_param_petId>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_getPet_param_petId_array_into(arr, json);
    return json;
}

inline void serialize_schema_5_array_into(const std::vector<schema_5>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_5_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_5_array(const std::vector<schema_5>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_5_array_into(arr, json);
    return json;
}

inline void serialize_schema_5_array_into(const arena_vector<schema_5>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_5_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_5_array(const arena_vector<schema_5>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_5_array_into(arr, json);
    return json;
}

inline void serialize_schema_6_array_into(const std::vector<schema_6>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_6_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_6_array(const std::vector<schema_6>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_6_array_into(arr, json);
    return json;
}

inline void serialize_schema_6_array_into(const arena_vector<schema_6>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_schema_6_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_6_array(const arena_vector<schema_6>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_6_array_into(arr, json);
    return json;
}

inline void serialize_deletePet_param_petId_array_into(const std::vector<deletePet_param_petId>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_deletePet_param_petId_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_deletePet_param_petId_array(const std::vector<deletePet_param_petId>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_deletePet_param_petId_array_into(arr, json);
    return json;
}

inline void serialize_deletePet_param_petId_array_into(const arena_vector<deletePet_param_petId>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_deletePet_param_petId_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_deletePet_param_petId_array(const arena_vector<deletePet_param_petId>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_deletePet_param_petId_array_into(arr, json);
    return json;
}

