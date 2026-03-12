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
#include <charconv>
#include <optional>
#include <string>
#include <vector>

using katana::monotonic_arena;

// ============================================================
// Forward Declarations
// ============================================================

[[nodiscard]] inline std::optional<Task> parse_Task(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Id_t> parse_Task_Id_t(std::string_view json,
                                                              monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Title_t> parse_Task_Title_t(std::string_view json,
                                                                    monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Description_t>
parse_Task_Description_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Status_t> parse_Task_Status_t(std::string_view json,
                                                                      monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Priority_t> parse_Task_Priority_t(std::string_view json,
                                                                          monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Tags_t> parse_Task_Tags_t(std::string_view json,
                                                                  monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Item_t> parse_Task_Item_t(std::string_view json,
                                                                  monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Assignee_t> parse_Task_Assignee_t(std::string_view json,
                                                                          monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Due_date_t> parse_Task_Due_date_t(std::string_view json,
                                                                          monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Created_at_t>
parse_Task_Created_at_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Updated_at_t>
parse_Task_Updated_at_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Metadata_t> parse_Task_Metadata_t(std::string_view json,
                                                                          monotonic_arena* arena);
[[nodiscard]] inline std::optional<User> parse_User(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<User_Id_t> parse_User_Id_t(std::string_view json,
                                                              monotonic_arena* arena);
[[nodiscard]] inline std::optional<User_Email_t> parse_User_Email_t(std::string_view json,
                                                                    monotonic_arena* arena);
[[nodiscard]] inline std::optional<User_Name_t> parse_User_Name_t(std::string_view json,
                                                                  monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateTaskRequest>
parse_CreateTaskRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateTaskRequest_Title_t>
parse_CreateTaskRequest_Title_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateTaskRequest_Description_t>
parse_CreateTaskRequest_Description_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateTaskRequest_Priority_t>
parse_CreateTaskRequest_Priority_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateTaskRequest_Tags_t>
parse_CreateTaskRequest_Tags_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateTaskRequest_Item_t>
parse_CreateTaskRequest_Item_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateTaskRequest_Assignee_id_t>
parse_CreateTaskRequest_Assignee_id_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateTaskRequest_Due_date_t>
parse_CreateTaskRequest_Due_date_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest>
parse_UpdateTaskRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest_Title_t>
parse_UpdateTaskRequest_Title_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest_Description_t>
parse_UpdateTaskRequest_Description_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest_Status_t>
parse_UpdateTaskRequest_Status_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest_Priority_t>
parse_UpdateTaskRequest_Priority_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest_Tags_t>
parse_UpdateTaskRequest_Tags_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest_Item_t>
parse_UpdateTaskRequest_Item_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest_Assignee_id_t>
parse_UpdateTaskRequest_Assignee_id_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest_Due_date_t>
parse_UpdateTaskRequest_Due_date_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateRequest>
parse_BatchCreateRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateRequest_Tasks_t>
parse_BatchCreateRequest_Tasks_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateRequest_Item_t>
parse_BatchCreateRequest_Item_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateResponse>
parse_BatchCreateResponse(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateResponse_Created_t>
parse_BatchCreateResponse_Created_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateResponse_Item_t>
parse_BatchCreateResponse_Item_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateResponse_Failed_t>
parse_BatchCreateResponse_Failed_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateResponse_Item_t_1>
parse_BatchCreateResponse_Item_t_1(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateResponse_Index_t>
parse_BatchCreateResponse_Index_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateResponse_Error_t>
parse_BatchCreateResponse_Error_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest> parse_SearchRequest(std::string_view json,
                                                                      monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Title_contains_t>
parse_SearchRequest_Title_contains_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Statuses_t>
parse_SearchRequest_Statuses_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Item_t>
parse_SearchRequest_Item_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Min_priority_t>
parse_SearchRequest_Min_priority_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Max_priority_t>
parse_SearchRequest_Max_priority_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Tags_t>
parse_SearchRequest_Tags_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Item_t_1>
parse_SearchRequest_Item_t_1(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Created_after_t>
parse_SearchRequest_Created_after_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Created_before_t>
parse_SearchRequest_Created_before_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Has_assignee_t>
parse_SearchRequest_Has_assignee_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TaskList> parse_TaskList(std::string_view json,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<TaskList_Tasks_t> parse_TaskList_Tasks_t(std::string_view json,
                                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<TaskList_Item_t> parse_TaskList_Item_t(std::string_view json,
                                                                          monotonic_arena* arena);
[[nodiscard]] inline std::optional<TaskList_Total_t> parse_TaskList_Total_t(std::string_view json,
                                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<TaskList_Has_more_t>
parse_TaskList_Has_more_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<HealthResponse> parse_HealthResponse(std::string_view json,
                                                                        monotonic_arena* arena);
[[nodiscard]] inline std::optional<HealthResponse_Status_t>
parse_HealthResponse_Status_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<HealthResponse_Timestamp_t>
parse_HealthResponse_Timestamp_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<HealthResponse_Uptime_seconds_t>
parse_HealthResponse_Uptime_seconds_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<HealthResponse_Total_requests_t>
parse_HealthResponse_Total_requests_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ProblemDetails> parse_ProblemDetails(std::string_view json,
                                                                        monotonic_arena* arena);
[[nodiscard]] inline std::optional<ProblemDetails_Type_t>
parse_ProblemDetails_Type_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ProblemDetails_Title_t>
parse_ProblemDetails_Title_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ProblemDetails_Status_t>
parse_ProblemDetails_Status_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ProblemDetails_Detail_t>
parse_ProblemDetails_Detail_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ProblemDetails_Instance_t>
parse_ProblemDetails_Instance_t(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<listTasks_param_status>
parse_listTasks_param_status(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<listTasks_param_priority>
parse_listTasks_param_priority(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<listTasks_param_limit>
parse_listTasks_param_limit(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<listTasks_param_offset>
parse_listTasks_param_offset(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema> parse_schema(std::string_view json,
                                                        monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_1> parse_schema_1(std::string_view json,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_2> parse_schema_2(std::string_view json,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_3> parse_schema_3(std::string_view json,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<getTask_param_id> parse_getTask_param_id(std::string_view json,
                                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_4> parse_schema_4(std::string_view json,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_5> parse_schema_5(std::string_view json,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<updateTask_param_id>
parse_updateTask_param_id(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_6> parse_schema_6(std::string_view json,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_7> parse_schema_7(std::string_view json,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_8> parse_schema_8(std::string_view json,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<deleteTask_param_id>
parse_deleteTask_param_id(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_9> parse_schema_9(std::string_view json,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_10> parse_schema_10(std::string_view json,
                                                              monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_11> parse_schema_11(std::string_view json,
                                                              monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_12> parse_schema_12(std::string_view json,
                                                              monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_13> parse_schema_13(std::string_view json,
                                                              monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_14> parse_schema_14(std::string_view json,
                                                              monotonic_arena* arena);

[[nodiscard]] inline std::optional<Task> parse_Task(katana::serde::json_cursor& cur,
                                                    monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Id_t> parse_Task_Id_t(katana::serde::json_cursor& cur,
                                                              monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Title_t> parse_Task_Title_t(katana::serde::json_cursor& cur,
                                                                    monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Description_t>
parse_Task_Description_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Status_t>
parse_Task_Status_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Priority_t>
parse_Task_Priority_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Tags_t> parse_Task_Tags_t(katana::serde::json_cursor& cur,
                                                                  monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Item_t> parse_Task_Item_t(katana::serde::json_cursor& cur,
                                                                  monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Assignee_t>
parse_Task_Assignee_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Due_date_t>
parse_Task_Due_date_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Created_at_t>
parse_Task_Created_at_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Updated_at_t>
parse_Task_Updated_at_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Task_Metadata_t>
parse_Task_Metadata_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<User> parse_User(katana::serde::json_cursor& cur,
                                                    monotonic_arena* arena);
[[nodiscard]] inline std::optional<User_Id_t> parse_User_Id_t(katana::serde::json_cursor& cur,
                                                              monotonic_arena* arena);
[[nodiscard]] inline std::optional<User_Email_t> parse_User_Email_t(katana::serde::json_cursor& cur,
                                                                    monotonic_arena* arena);
[[nodiscard]] inline std::optional<User_Name_t> parse_User_Name_t(katana::serde::json_cursor& cur,
                                                                  monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateTaskRequest>
parse_CreateTaskRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateTaskRequest_Title_t>
parse_CreateTaskRequest_Title_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateTaskRequest_Description_t>
parse_CreateTaskRequest_Description_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateTaskRequest_Priority_t>
parse_CreateTaskRequest_Priority_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateTaskRequest_Tags_t>
parse_CreateTaskRequest_Tags_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateTaskRequest_Item_t>
parse_CreateTaskRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateTaskRequest_Assignee_id_t>
parse_CreateTaskRequest_Assignee_id_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateTaskRequest_Due_date_t>
parse_CreateTaskRequest_Due_date_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest>
parse_UpdateTaskRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest_Title_t>
parse_UpdateTaskRequest_Title_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest_Description_t>
parse_UpdateTaskRequest_Description_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest_Status_t>
parse_UpdateTaskRequest_Status_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest_Priority_t>
parse_UpdateTaskRequest_Priority_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest_Tags_t>
parse_UpdateTaskRequest_Tags_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest_Item_t>
parse_UpdateTaskRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest_Assignee_id_t>
parse_UpdateTaskRequest_Assignee_id_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateTaskRequest_Due_date_t>
parse_UpdateTaskRequest_Due_date_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateRequest>
parse_BatchCreateRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateRequest_Tasks_t>
parse_BatchCreateRequest_Tasks_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateRequest_Item_t>
parse_BatchCreateRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateResponse>
parse_BatchCreateResponse(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateResponse_Created_t>
parse_BatchCreateResponse_Created_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateResponse_Item_t>
parse_BatchCreateResponse_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateResponse_Failed_t>
parse_BatchCreateResponse_Failed_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateResponse_Item_t_1>
parse_BatchCreateResponse_Item_t_1(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateResponse_Index_t>
parse_BatchCreateResponse_Index_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<BatchCreateResponse_Error_t>
parse_BatchCreateResponse_Error_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest>
parse_SearchRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Title_contains_t>
parse_SearchRequest_Title_contains_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Statuses_t>
parse_SearchRequest_Statuses_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Item_t>
parse_SearchRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Min_priority_t>
parse_SearchRequest_Min_priority_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Max_priority_t>
parse_SearchRequest_Max_priority_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Tags_t>
parse_SearchRequest_Tags_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Item_t_1>
parse_SearchRequest_Item_t_1(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Created_after_t>
parse_SearchRequest_Created_after_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Created_before_t>
parse_SearchRequest_Created_before_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SearchRequest_Has_assignee_t>
parse_SearchRequest_Has_assignee_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TaskList> parse_TaskList(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<TaskList_Tasks_t>
parse_TaskList_Tasks_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TaskList_Item_t>
parse_TaskList_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TaskList_Total_t>
parse_TaskList_Total_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TaskList_Has_more_t>
parse_TaskList_Has_more_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<HealthResponse>
parse_HealthResponse(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<HealthResponse_Status_t>
parse_HealthResponse_Status_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<HealthResponse_Timestamp_t>
parse_HealthResponse_Timestamp_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<HealthResponse_Uptime_seconds_t>
parse_HealthResponse_Uptime_seconds_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<HealthResponse_Total_requests_t>
parse_HealthResponse_Total_requests_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ProblemDetails>
parse_ProblemDetails(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ProblemDetails_Type_t>
parse_ProblemDetails_Type_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ProblemDetails_Title_t>
parse_ProblemDetails_Title_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ProblemDetails_Status_t>
parse_ProblemDetails_Status_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ProblemDetails_Detail_t>
parse_ProblemDetails_Detail_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ProblemDetails_Instance_t>
parse_ProblemDetails_Instance_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<listTasks_param_status>
parse_listTasks_param_status(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<listTasks_param_priority>
parse_listTasks_param_priority(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<listTasks_param_limit>
parse_listTasks_param_limit(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<listTasks_param_offset>
parse_listTasks_param_offset(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema> parse_schema(katana::serde::json_cursor& cur,
                                                        monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_1> parse_schema_1(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_2> parse_schema_2(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_3> parse_schema_3(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<getTask_param_id>
parse_getTask_param_id(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_4> parse_schema_4(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_5> parse_schema_5(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<updateTask_param_id>
parse_updateTask_param_id(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_6> parse_schema_6(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_7> parse_schema_7(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_8> parse_schema_8(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<deleteTask_param_id>
parse_deleteTask_param_id(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_9> parse_schema_9(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_10> parse_schema_10(katana::serde::json_cursor& cur,
                                                              monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_11> parse_schema_11(katana::serde::json_cursor& cur,
                                                              monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_12> parse_schema_12(katana::serde::json_cursor& cur,
                                                              monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_13> parse_schema_13(katana::serde::json_cursor& cur,
                                                              monotonic_arena* arena);
[[nodiscard]] inline std::optional<schema_14> parse_schema_14(katana::serde::json_cursor& cur,
                                                              monotonic_arena* arena);

inline void serialize_Task_into(const Task& obj, std::string& out);
inline void serialize_Task_Id_t_into(const Task_Id_t& obj, std::string& out);
inline void serialize_Task_Title_t_into(const Task_Title_t& obj, std::string& out);
inline void serialize_Task_Description_t_into(const Task_Description_t& obj, std::string& out);
inline void serialize_Task_Status_t_into(const Task_Status_t& obj, std::string& out);
inline void serialize_Task_Priority_t_into(const Task_Priority_t& obj, std::string& out);
inline void serialize_Task_Tags_t_into(const Task_Tags_t& obj, std::string& out);
inline void serialize_Task_Item_t_into(const Task_Item_t& obj, std::string& out);
inline void serialize_Task_Assignee_t_into(const Task_Assignee_t& obj, std::string& out);
inline void serialize_Task_Due_date_t_into(const Task_Due_date_t& obj, std::string& out);
inline void serialize_Task_Created_at_t_into(const Task_Created_at_t& obj, std::string& out);
inline void serialize_Task_Updated_at_t_into(const Task_Updated_at_t& obj, std::string& out);
inline void serialize_Task_Metadata_t_into(const Task_Metadata_t& obj, std::string& out);
inline void serialize_User_into(const User& obj, std::string& out);
inline void serialize_User_Id_t_into(const User_Id_t& obj, std::string& out);
inline void serialize_User_Email_t_into(const User_Email_t& obj, std::string& out);
inline void serialize_User_Name_t_into(const User_Name_t& obj, std::string& out);
inline void serialize_CreateTaskRequest_into(const CreateTaskRequest& obj, std::string& out);
inline void serialize_CreateTaskRequest_Title_t_into(const CreateTaskRequest_Title_t& obj,
                                                     std::string& out);
inline void
serialize_CreateTaskRequest_Description_t_into(const CreateTaskRequest_Description_t& obj,
                                               std::string& out);
inline void serialize_CreateTaskRequest_Priority_t_into(const CreateTaskRequest_Priority_t& obj,
                                                        std::string& out);
inline void serialize_CreateTaskRequest_Tags_t_into(const CreateTaskRequest_Tags_t& obj,
                                                    std::string& out);
inline void serialize_CreateTaskRequest_Item_t_into(const CreateTaskRequest_Item_t& obj,
                                                    std::string& out);
inline void
serialize_CreateTaskRequest_Assignee_id_t_into(const CreateTaskRequest_Assignee_id_t& obj,
                                               std::string& out);
inline void serialize_CreateTaskRequest_Due_date_t_into(const CreateTaskRequest_Due_date_t& obj,
                                                        std::string& out);
inline void serialize_UpdateTaskRequest_into(const UpdateTaskRequest& obj, std::string& out);
inline void serialize_UpdateTaskRequest_Title_t_into(const UpdateTaskRequest_Title_t& obj,
                                                     std::string& out);
inline void
serialize_UpdateTaskRequest_Description_t_into(const UpdateTaskRequest_Description_t& obj,
                                               std::string& out);
inline void serialize_UpdateTaskRequest_Status_t_into(const UpdateTaskRequest_Status_t& obj,
                                                      std::string& out);
inline void serialize_UpdateTaskRequest_Priority_t_into(const UpdateTaskRequest_Priority_t& obj,
                                                        std::string& out);
inline void serialize_UpdateTaskRequest_Tags_t_into(const UpdateTaskRequest_Tags_t& obj,
                                                    std::string& out);
inline void serialize_UpdateTaskRequest_Item_t_into(const UpdateTaskRequest_Item_t& obj,
                                                    std::string& out);
inline void
serialize_UpdateTaskRequest_Assignee_id_t_into(const UpdateTaskRequest_Assignee_id_t& obj,
                                               std::string& out);
inline void serialize_UpdateTaskRequest_Due_date_t_into(const UpdateTaskRequest_Due_date_t& obj,
                                                        std::string& out);
inline void serialize_BatchCreateRequest_into(const BatchCreateRequest& obj, std::string& out);
inline void serialize_BatchCreateRequest_Tasks_t_into(const BatchCreateRequest_Tasks_t& obj,
                                                      std::string& out);
inline void serialize_BatchCreateRequest_Item_t_into(const BatchCreateRequest_Item_t& obj,
                                                     std::string& out);
inline void serialize_BatchCreateResponse_into(const BatchCreateResponse& obj, std::string& out);
inline void serialize_BatchCreateResponse_Created_t_into(const BatchCreateResponse_Created_t& obj,
                                                         std::string& out);
inline void serialize_BatchCreateResponse_Item_t_into(const BatchCreateResponse_Item_t& obj,
                                                      std::string& out);
inline void serialize_BatchCreateResponse_Failed_t_into(const BatchCreateResponse_Failed_t& obj,
                                                        std::string& out);
inline void serialize_BatchCreateResponse_Item_t_1_into(const BatchCreateResponse_Item_t_1& obj,
                                                        std::string& out);
inline void serialize_BatchCreateResponse_Index_t_into(const BatchCreateResponse_Index_t& obj,
                                                       std::string& out);
inline void serialize_BatchCreateResponse_Error_t_into(const BatchCreateResponse_Error_t& obj,
                                                       std::string& out);
inline void serialize_SearchRequest_into(const SearchRequest& obj, std::string& out);
inline void serialize_SearchRequest_Title_contains_t_into(const SearchRequest_Title_contains_t& obj,
                                                          std::string& out);
inline void serialize_SearchRequest_Statuses_t_into(const SearchRequest_Statuses_t& obj,
                                                    std::string& out);
inline void serialize_SearchRequest_Item_t_into(const SearchRequest_Item_t& obj, std::string& out);
inline void serialize_SearchRequest_Min_priority_t_into(const SearchRequest_Min_priority_t& obj,
                                                        std::string& out);
inline void serialize_SearchRequest_Max_priority_t_into(const SearchRequest_Max_priority_t& obj,
                                                        std::string& out);
inline void serialize_SearchRequest_Tags_t_into(const SearchRequest_Tags_t& obj, std::string& out);
inline void serialize_SearchRequest_Item_t_1_into(const SearchRequest_Item_t_1& obj,
                                                  std::string& out);
inline void serialize_SearchRequest_Created_after_t_into(const SearchRequest_Created_after_t& obj,
                                                         std::string& out);
inline void serialize_SearchRequest_Created_before_t_into(const SearchRequest_Created_before_t& obj,
                                                          std::string& out);
inline void serialize_SearchRequest_Has_assignee_t_into(const SearchRequest_Has_assignee_t& obj,
                                                        std::string& out);
inline void serialize_TaskList_into(const TaskList& obj, std::string& out);
inline void serialize_TaskList_Tasks_t_into(const TaskList_Tasks_t& obj, std::string& out);
inline void serialize_TaskList_Item_t_into(const TaskList_Item_t& obj, std::string& out);
inline void serialize_TaskList_Total_t_into(const TaskList_Total_t& obj, std::string& out);
inline void serialize_TaskList_Has_more_t_into(const TaskList_Has_more_t& obj, std::string& out);
inline void serialize_HealthResponse_into(const HealthResponse& obj, std::string& out);
inline void serialize_HealthResponse_Status_t_into(const HealthResponse_Status_t& obj,
                                                   std::string& out);
inline void serialize_HealthResponse_Timestamp_t_into(const HealthResponse_Timestamp_t& obj,
                                                      std::string& out);
inline void
serialize_HealthResponse_Uptime_seconds_t_into(const HealthResponse_Uptime_seconds_t& obj,
                                               std::string& out);
inline void
serialize_HealthResponse_Total_requests_t_into(const HealthResponse_Total_requests_t& obj,
                                               std::string& out);
inline void serialize_ProblemDetails_into(const ProblemDetails& obj, std::string& out);
inline void serialize_ProblemDetails_Type_t_into(const ProblemDetails_Type_t& obj,
                                                 std::string& out);
inline void serialize_ProblemDetails_Title_t_into(const ProblemDetails_Title_t& obj,
                                                  std::string& out);
inline void serialize_ProblemDetails_Status_t_into(const ProblemDetails_Status_t& obj,
                                                   std::string& out);
inline void serialize_ProblemDetails_Detail_t_into(const ProblemDetails_Detail_t& obj,
                                                   std::string& out);
inline void serialize_ProblemDetails_Instance_t_into(const ProblemDetails_Instance_t& obj,
                                                     std::string& out);
inline void serialize_listTasks_param_status_into(const listTasks_param_status& obj,
                                                  std::string& out);
inline void serialize_listTasks_param_priority_into(const listTasks_param_priority& obj,
                                                    std::string& out);
inline void serialize_listTasks_param_limit_into(const listTasks_param_limit& obj,
                                                 std::string& out);
inline void serialize_listTasks_param_offset_into(const listTasks_param_offset& obj,
                                                  std::string& out);
inline void serialize_schema_into(const schema& obj, std::string& out);
inline void serialize_schema_1_into(const schema_1& obj, std::string& out);
inline void serialize_schema_2_into(const schema_2& obj, std::string& out);
inline void serialize_schema_3_into(const schema_3& obj, std::string& out);
inline void serialize_getTask_param_id_into(const getTask_param_id& obj, std::string& out);
inline void serialize_schema_4_into(const schema_4& obj, std::string& out);
inline void serialize_schema_5_into(const schema_5& obj, std::string& out);
inline void serialize_updateTask_param_id_into(const updateTask_param_id& obj, std::string& out);
inline void serialize_schema_6_into(const schema_6& obj, std::string& out);
inline void serialize_schema_7_into(const schema_7& obj, std::string& out);
inline void serialize_schema_8_into(const schema_8& obj, std::string& out);
inline void serialize_deleteTask_param_id_into(const deleteTask_param_id& obj, std::string& out);
inline void serialize_schema_9_into(const schema_9& obj, std::string& out);
inline void serialize_schema_10_into(const schema_10& obj, std::string& out);
inline void serialize_schema_11_into(const schema_11& obj, std::string& out);
inline void serialize_schema_12_into(const schema_12& obj, std::string& out);
inline void serialize_schema_13_into(const schema_13& obj, std::string& out);
inline void serialize_schema_14_into(const schema_14& obj, std::string& out);

inline std::string serialize_Task(const Task& obj);
inline std::string serialize_Task_Id_t(const Task_Id_t& obj);
inline std::string serialize_Task_Title_t(const Task_Title_t& obj);
inline std::string serialize_Task_Description_t(const Task_Description_t& obj);
inline std::string serialize_Task_Status_t(const Task_Status_t& obj);
inline std::string serialize_Task_Priority_t(const Task_Priority_t& obj);
inline std::string serialize_Task_Tags_t(const Task_Tags_t& obj);
inline std::string serialize_Task_Item_t(const Task_Item_t& obj);
inline std::string serialize_Task_Assignee_t(const Task_Assignee_t& obj);
inline std::string serialize_Task_Due_date_t(const Task_Due_date_t& obj);
inline std::string serialize_Task_Created_at_t(const Task_Created_at_t& obj);
inline std::string serialize_Task_Updated_at_t(const Task_Updated_at_t& obj);
inline std::string serialize_Task_Metadata_t(const Task_Metadata_t& obj);
inline std::string serialize_User(const User& obj);
inline std::string serialize_User_Id_t(const User_Id_t& obj);
inline std::string serialize_User_Email_t(const User_Email_t& obj);
inline std::string serialize_User_Name_t(const User_Name_t& obj);
inline std::string serialize_CreateTaskRequest(const CreateTaskRequest& obj);
inline std::string serialize_CreateTaskRequest_Title_t(const CreateTaskRequest_Title_t& obj);
inline std::string
serialize_CreateTaskRequest_Description_t(const CreateTaskRequest_Description_t& obj);
inline std::string serialize_CreateTaskRequest_Priority_t(const CreateTaskRequest_Priority_t& obj);
inline std::string serialize_CreateTaskRequest_Tags_t(const CreateTaskRequest_Tags_t& obj);
inline std::string serialize_CreateTaskRequest_Item_t(const CreateTaskRequest_Item_t& obj);
inline std::string
serialize_CreateTaskRequest_Assignee_id_t(const CreateTaskRequest_Assignee_id_t& obj);
inline std::string serialize_CreateTaskRequest_Due_date_t(const CreateTaskRequest_Due_date_t& obj);
inline std::string serialize_UpdateTaskRequest(const UpdateTaskRequest& obj);
inline std::string serialize_UpdateTaskRequest_Title_t(const UpdateTaskRequest_Title_t& obj);
inline std::string
serialize_UpdateTaskRequest_Description_t(const UpdateTaskRequest_Description_t& obj);
inline std::string serialize_UpdateTaskRequest_Status_t(const UpdateTaskRequest_Status_t& obj);
inline std::string serialize_UpdateTaskRequest_Priority_t(const UpdateTaskRequest_Priority_t& obj);
inline std::string serialize_UpdateTaskRequest_Tags_t(const UpdateTaskRequest_Tags_t& obj);
inline std::string serialize_UpdateTaskRequest_Item_t(const UpdateTaskRequest_Item_t& obj);
inline std::string
serialize_UpdateTaskRequest_Assignee_id_t(const UpdateTaskRequest_Assignee_id_t& obj);
inline std::string serialize_UpdateTaskRequest_Due_date_t(const UpdateTaskRequest_Due_date_t& obj);
inline std::string serialize_BatchCreateRequest(const BatchCreateRequest& obj);
inline std::string serialize_BatchCreateRequest_Tasks_t(const BatchCreateRequest_Tasks_t& obj);
inline std::string serialize_BatchCreateRequest_Item_t(const BatchCreateRequest_Item_t& obj);
inline std::string serialize_BatchCreateResponse(const BatchCreateResponse& obj);
inline std::string
serialize_BatchCreateResponse_Created_t(const BatchCreateResponse_Created_t& obj);
inline std::string serialize_BatchCreateResponse_Item_t(const BatchCreateResponse_Item_t& obj);
inline std::string serialize_BatchCreateResponse_Failed_t(const BatchCreateResponse_Failed_t& obj);
inline std::string serialize_BatchCreateResponse_Item_t_1(const BatchCreateResponse_Item_t_1& obj);
inline std::string serialize_BatchCreateResponse_Index_t(const BatchCreateResponse_Index_t& obj);
inline std::string serialize_BatchCreateResponse_Error_t(const BatchCreateResponse_Error_t& obj);
inline std::string serialize_SearchRequest(const SearchRequest& obj);
inline std::string
serialize_SearchRequest_Title_contains_t(const SearchRequest_Title_contains_t& obj);
inline std::string serialize_SearchRequest_Statuses_t(const SearchRequest_Statuses_t& obj);
inline std::string serialize_SearchRequest_Item_t(const SearchRequest_Item_t& obj);
inline std::string serialize_SearchRequest_Min_priority_t(const SearchRequest_Min_priority_t& obj);
inline std::string serialize_SearchRequest_Max_priority_t(const SearchRequest_Max_priority_t& obj);
inline std::string serialize_SearchRequest_Tags_t(const SearchRequest_Tags_t& obj);
inline std::string serialize_SearchRequest_Item_t_1(const SearchRequest_Item_t_1& obj);
inline std::string
serialize_SearchRequest_Created_after_t(const SearchRequest_Created_after_t& obj);
inline std::string
serialize_SearchRequest_Created_before_t(const SearchRequest_Created_before_t& obj);
inline std::string serialize_SearchRequest_Has_assignee_t(const SearchRequest_Has_assignee_t& obj);
inline std::string serialize_TaskList(const TaskList& obj);
inline std::string serialize_TaskList_Tasks_t(const TaskList_Tasks_t& obj);
inline std::string serialize_TaskList_Item_t(const TaskList_Item_t& obj);
inline std::string serialize_TaskList_Total_t(const TaskList_Total_t& obj);
inline std::string serialize_TaskList_Has_more_t(const TaskList_Has_more_t& obj);
inline std::string serialize_HealthResponse(const HealthResponse& obj);
inline std::string serialize_HealthResponse_Status_t(const HealthResponse_Status_t& obj);
inline std::string serialize_HealthResponse_Timestamp_t(const HealthResponse_Timestamp_t& obj);
inline std::string
serialize_HealthResponse_Uptime_seconds_t(const HealthResponse_Uptime_seconds_t& obj);
inline std::string
serialize_HealthResponse_Total_requests_t(const HealthResponse_Total_requests_t& obj);
inline std::string serialize_ProblemDetails(const ProblemDetails& obj);
inline std::string serialize_ProblemDetails_Type_t(const ProblemDetails_Type_t& obj);
inline std::string serialize_ProblemDetails_Title_t(const ProblemDetails_Title_t& obj);
inline std::string serialize_ProblemDetails_Status_t(const ProblemDetails_Status_t& obj);
inline std::string serialize_ProblemDetails_Detail_t(const ProblemDetails_Detail_t& obj);
inline std::string serialize_ProblemDetails_Instance_t(const ProblemDetails_Instance_t& obj);
inline std::string serialize_listTasks_param_status(const listTasks_param_status& obj);
inline std::string serialize_listTasks_param_priority(const listTasks_param_priority& obj);
inline std::string serialize_listTasks_param_limit(const listTasks_param_limit& obj);
inline std::string serialize_listTasks_param_offset(const listTasks_param_offset& obj);
inline std::string serialize_schema(const schema& obj);
inline std::string serialize_schema_1(const schema_1& obj);
inline std::string serialize_schema_2(const schema_2& obj);
inline std::string serialize_schema_3(const schema_3& obj);
inline std::string serialize_getTask_param_id(const getTask_param_id& obj);
inline std::string serialize_schema_4(const schema_4& obj);
inline std::string serialize_schema_5(const schema_5& obj);
inline std::string serialize_updateTask_param_id(const updateTask_param_id& obj);
inline std::string serialize_schema_6(const schema_6& obj);
inline std::string serialize_schema_7(const schema_7& obj);
inline std::string serialize_schema_8(const schema_8& obj);
inline std::string serialize_deleteTask_param_id(const deleteTask_param_id& obj);
inline std::string serialize_schema_9(const schema_9& obj);
inline std::string serialize_schema_10(const schema_10& obj);
inline std::string serialize_schema_11(const schema_11& obj);
inline std::string serialize_schema_12(const schema_12& obj);
inline std::string serialize_schema_13(const schema_13& obj);
inline std::string serialize_schema_14(const schema_14& obj);

[[nodiscard]] inline std::optional<std::vector<Task>> parse_Task_array(std::string_view json,
                                                                       monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Id_t>>
parse_Task_Id_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Title_t>>
parse_Task_Title_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Description_t>>
parse_Task_Description_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Status_t>>
parse_Task_Status_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Priority_t>>
parse_Task_Priority_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Tags_t>>
parse_Task_Tags_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Item_t>>
parse_Task_Item_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Assignee_t>>
parse_Task_Assignee_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Due_date_t>>
parse_Task_Due_date_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Created_at_t>>
parse_Task_Created_at_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Updated_at_t>>
parse_Task_Updated_at_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Metadata_t>>
parse_Task_Metadata_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<User>> parse_User_array(std::string_view json,
                                                                       monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<User_Id_t>>
parse_User_Id_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<User_Email_t>>
parse_User_Email_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<User_Name_t>>
parse_User_Name_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest>>
parse_CreateTaskRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Title_t>>
parse_CreateTaskRequest_Title_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Description_t>>
parse_CreateTaskRequest_Description_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Priority_t>>
parse_CreateTaskRequest_Priority_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Tags_t>>
parse_CreateTaskRequest_Tags_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Item_t>>
parse_CreateTaskRequest_Item_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Assignee_id_t>>
parse_CreateTaskRequest_Assignee_id_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Due_date_t>>
parse_CreateTaskRequest_Due_date_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest>>
parse_UpdateTaskRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Title_t>>
parse_UpdateTaskRequest_Title_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Description_t>>
parse_UpdateTaskRequest_Description_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Status_t>>
parse_UpdateTaskRequest_Status_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Priority_t>>
parse_UpdateTaskRequest_Priority_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Tags_t>>
parse_UpdateTaskRequest_Tags_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Item_t>>
parse_UpdateTaskRequest_Item_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Assignee_id_t>>
parse_UpdateTaskRequest_Assignee_id_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Due_date_t>>
parse_UpdateTaskRequest_Due_date_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateRequest>>
parse_BatchCreateRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateRequest_Tasks_t>>
parse_BatchCreateRequest_Tasks_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateRequest_Item_t>>
parse_BatchCreateRequest_Item_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse>>
parse_BatchCreateResponse_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Created_t>>
parse_BatchCreateResponse_Created_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Item_t>>
parse_BatchCreateResponse_Item_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Failed_t>>
parse_BatchCreateResponse_Failed_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Item_t_1>>
parse_BatchCreateResponse_Item_t_1_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Index_t>>
parse_BatchCreateResponse_Index_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Error_t>>
parse_BatchCreateResponse_Error_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest>>
parse_SearchRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Title_contains_t>>
parse_SearchRequest_Title_contains_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Statuses_t>>
parse_SearchRequest_Statuses_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Item_t>>
parse_SearchRequest_Item_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Min_priority_t>>
parse_SearchRequest_Min_priority_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Max_priority_t>>
parse_SearchRequest_Max_priority_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Tags_t>>
parse_SearchRequest_Tags_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Item_t_1>>
parse_SearchRequest_Item_t_1_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Created_after_t>>
parse_SearchRequest_Created_after_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Created_before_t>>
parse_SearchRequest_Created_before_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Has_assignee_t>>
parse_SearchRequest_Has_assignee_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TaskList>>
parse_TaskList_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TaskList_Tasks_t>>
parse_TaskList_Tasks_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TaskList_Item_t>>
parse_TaskList_Item_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TaskList_Total_t>>
parse_TaskList_Total_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TaskList_Has_more_t>>
parse_TaskList_Has_more_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<HealthResponse>>
parse_HealthResponse_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<HealthResponse_Status_t>>
parse_HealthResponse_Status_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<HealthResponse_Timestamp_t>>
parse_HealthResponse_Timestamp_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<HealthResponse_Uptime_seconds_t>>
parse_HealthResponse_Uptime_seconds_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<HealthResponse_Total_requests_t>>
parse_HealthResponse_Total_requests_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ProblemDetails>>
parse_ProblemDetails_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Type_t>>
parse_ProblemDetails_Type_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Title_t>>
parse_ProblemDetails_Title_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Status_t>>
parse_ProblemDetails_Status_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Detail_t>>
parse_ProblemDetails_Detail_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Instance_t>>
parse_ProblemDetails_Instance_t_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<listTasks_param_status>>
parse_listTasks_param_status_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<listTasks_param_priority>>
parse_listTasks_param_priority_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<listTasks_param_limit>>
parse_listTasks_param_limit_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<listTasks_param_offset>>
parse_listTasks_param_offset_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(std::string_view json,
                                                                           monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_1>>
parse_schema_1_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_2>>
parse_schema_2_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_3>>
parse_schema_3_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<getTask_param_id>>
parse_getTask_param_id_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_4>>
parse_schema_4_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_5>>
parse_schema_5_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<updateTask_param_id>>
parse_updateTask_param_id_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_6>>
parse_schema_6_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_7>>
parse_schema_7_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_8>>
parse_schema_8_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<deleteTask_param_id>>
parse_deleteTask_param_id_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_9>>
parse_schema_9_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_10>>
parse_schema_10_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_11>>
parse_schema_11_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_12>>
parse_schema_12_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_13>>
parse_schema_13_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_14>>
parse_schema_14_array(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<std::vector<Task>>
parse_Task_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Id_t>>
parse_Task_Id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Title_t>>
parse_Task_Title_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Description_t>>
parse_Task_Description_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Status_t>>
parse_Task_Status_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Priority_t>>
parse_Task_Priority_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Tags_t>>
parse_Task_Tags_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Item_t>>
parse_Task_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Assignee_t>>
parse_Task_Assignee_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Due_date_t>>
parse_Task_Due_date_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Created_at_t>>
parse_Task_Created_at_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Updated_at_t>>
parse_Task_Updated_at_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Task_Metadata_t>>
parse_Task_Metadata_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<User>>
parse_User_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<User_Id_t>>
parse_User_Id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<User_Email_t>>
parse_User_Email_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<User_Name_t>>
parse_User_Name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest>>
parse_CreateTaskRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Title_t>>
parse_CreateTaskRequest_Title_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Description_t>>
parse_CreateTaskRequest_Description_t_array(katana::serde::json_cursor& cur,
                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Priority_t>>
parse_CreateTaskRequest_Priority_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Tags_t>>
parse_CreateTaskRequest_Tags_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Item_t>>
parse_CreateTaskRequest_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Assignee_id_t>>
parse_CreateTaskRequest_Assignee_id_t_array(katana::serde::json_cursor& cur,
                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Due_date_t>>
parse_CreateTaskRequest_Due_date_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest>>
parse_UpdateTaskRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Title_t>>
parse_UpdateTaskRequest_Title_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Description_t>>
parse_UpdateTaskRequest_Description_t_array(katana::serde::json_cursor& cur,
                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Status_t>>
parse_UpdateTaskRequest_Status_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Priority_t>>
parse_UpdateTaskRequest_Priority_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Tags_t>>
parse_UpdateTaskRequest_Tags_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Item_t>>
parse_UpdateTaskRequest_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Assignee_id_t>>
parse_UpdateTaskRequest_Assignee_id_t_array(katana::serde::json_cursor& cur,
                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Due_date_t>>
parse_UpdateTaskRequest_Due_date_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateRequest>>
parse_BatchCreateRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateRequest_Tasks_t>>
parse_BatchCreateRequest_Tasks_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateRequest_Item_t>>
parse_BatchCreateRequest_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse>>
parse_BatchCreateResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Created_t>>
parse_BatchCreateResponse_Created_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Item_t>>
parse_BatchCreateResponse_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Failed_t>>
parse_BatchCreateResponse_Failed_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Item_t_1>>
parse_BatchCreateResponse_Item_t_1_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Index_t>>
parse_BatchCreateResponse_Index_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Error_t>>
parse_BatchCreateResponse_Error_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest>>
parse_SearchRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Title_contains_t>>
parse_SearchRequest_Title_contains_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Statuses_t>>
parse_SearchRequest_Statuses_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Item_t>>
parse_SearchRequest_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Min_priority_t>>
parse_SearchRequest_Min_priority_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Max_priority_t>>
parse_SearchRequest_Max_priority_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Tags_t>>
parse_SearchRequest_Tags_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Item_t_1>>
parse_SearchRequest_Item_t_1_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Created_after_t>>
parse_SearchRequest_Created_after_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Created_before_t>>
parse_SearchRequest_Created_before_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SearchRequest_Has_assignee_t>>
parse_SearchRequest_Has_assignee_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TaskList>>
parse_TaskList_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TaskList_Tasks_t>>
parse_TaskList_Tasks_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TaskList_Item_t>>
parse_TaskList_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TaskList_Total_t>>
parse_TaskList_Total_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TaskList_Has_more_t>>
parse_TaskList_Has_more_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<HealthResponse>>
parse_HealthResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<HealthResponse_Status_t>>
parse_HealthResponse_Status_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<HealthResponse_Timestamp_t>>
parse_HealthResponse_Timestamp_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<HealthResponse_Uptime_seconds_t>>
parse_HealthResponse_Uptime_seconds_t_array(katana::serde::json_cursor& cur,
                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<HealthResponse_Total_requests_t>>
parse_HealthResponse_Total_requests_t_array(katana::serde::json_cursor& cur,
                                            monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ProblemDetails>>
parse_ProblemDetails_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Type_t>>
parse_ProblemDetails_Type_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Title_t>>
parse_ProblemDetails_Title_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Status_t>>
parse_ProblemDetails_Status_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Detail_t>>
parse_ProblemDetails_Detail_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Instance_t>>
parse_ProblemDetails_Instance_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<listTasks_param_status>>
parse_listTasks_param_status_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<listTasks_param_priority>>
parse_listTasks_param_priority_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<listTasks_param_limit>>
parse_listTasks_param_limit_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<listTasks_param_offset>>
parse_listTasks_param_offset_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema>>
parse_schema_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_1>>
parse_schema_1_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_2>>
parse_schema_2_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_3>>
parse_schema_3_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<getTask_param_id>>
parse_getTask_param_id_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_4>>
parse_schema_4_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_5>>
parse_schema_5_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<updateTask_param_id>>
parse_updateTask_param_id_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_6>>
parse_schema_6_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_7>>
parse_schema_7_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_8>>
parse_schema_8_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<deleteTask_param_id>>
parse_deleteTask_param_id_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_9>>
parse_schema_9_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_10>>
parse_schema_10_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_11>>
parse_schema_11_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_12>>
parse_schema_12_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_13>>
parse_schema_13_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<schema_14>>
parse_schema_14_array(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_Task_array_into(const std::vector<Task>& arr, std::string& out);
inline void serialize_Task_array_into(const arena_vector<Task>& arr, std::string& out);
inline void serialize_Task_Id_t_array_into(const std::vector<Task_Id_t>& arr, std::string& out);
inline void serialize_Task_Id_t_array_into(const arena_vector<Task_Id_t>& arr, std::string& out);
inline void serialize_Task_Title_t_array_into(const std::vector<Task_Title_t>& arr,
                                              std::string& out);
inline void serialize_Task_Title_t_array_into(const arena_vector<Task_Title_t>& arr,
                                              std::string& out);
inline void serialize_Task_Description_t_array_into(const std::vector<Task_Description_t>& arr,
                                                    std::string& out);
inline void serialize_Task_Description_t_array_into(const arena_vector<Task_Description_t>& arr,
                                                    std::string& out);
inline void serialize_Task_Status_t_array_into(const std::vector<Task_Status_t>& arr,
                                               std::string& out);
inline void serialize_Task_Status_t_array_into(const arena_vector<Task_Status_t>& arr,
                                               std::string& out);
inline void serialize_Task_Priority_t_array_into(const std::vector<Task_Priority_t>& arr,
                                                 std::string& out);
inline void serialize_Task_Priority_t_array_into(const arena_vector<Task_Priority_t>& arr,
                                                 std::string& out);
inline void serialize_Task_Tags_t_array_into(const std::vector<Task_Tags_t>& arr, std::string& out);
inline void serialize_Task_Tags_t_array_into(const arena_vector<Task_Tags_t>& arr,
                                             std::string& out);
inline void serialize_Task_Item_t_array_into(const std::vector<Task_Item_t>& arr, std::string& out);
inline void serialize_Task_Item_t_array_into(const arena_vector<Task_Item_t>& arr,
                                             std::string& out);
inline void serialize_Task_Assignee_t_array_into(const std::vector<Task_Assignee_t>& arr,
                                                 std::string& out);
inline void serialize_Task_Assignee_t_array_into(const arena_vector<Task_Assignee_t>& arr,
                                                 std::string& out);
inline void serialize_Task_Due_date_t_array_into(const std::vector<Task_Due_date_t>& arr,
                                                 std::string& out);
inline void serialize_Task_Due_date_t_array_into(const arena_vector<Task_Due_date_t>& arr,
                                                 std::string& out);
inline void serialize_Task_Created_at_t_array_into(const std::vector<Task_Created_at_t>& arr,
                                                   std::string& out);
inline void serialize_Task_Created_at_t_array_into(const arena_vector<Task_Created_at_t>& arr,
                                                   std::string& out);
inline void serialize_Task_Updated_at_t_array_into(const std::vector<Task_Updated_at_t>& arr,
                                                   std::string& out);
inline void serialize_Task_Updated_at_t_array_into(const arena_vector<Task_Updated_at_t>& arr,
                                                   std::string& out);
inline void serialize_Task_Metadata_t_array_into(const std::vector<Task_Metadata_t>& arr,
                                                 std::string& out);
inline void serialize_Task_Metadata_t_array_into(const arena_vector<Task_Metadata_t>& arr,
                                                 std::string& out);
inline void serialize_User_array_into(const std::vector<User>& arr, std::string& out);
inline void serialize_User_array_into(const arena_vector<User>& arr, std::string& out);
inline void serialize_User_Id_t_array_into(const std::vector<User_Id_t>& arr, std::string& out);
inline void serialize_User_Id_t_array_into(const arena_vector<User_Id_t>& arr, std::string& out);
inline void serialize_User_Email_t_array_into(const std::vector<User_Email_t>& arr,
                                              std::string& out);
inline void serialize_User_Email_t_array_into(const arena_vector<User_Email_t>& arr,
                                              std::string& out);
inline void serialize_User_Name_t_array_into(const std::vector<User_Name_t>& arr, std::string& out);
inline void serialize_User_Name_t_array_into(const arena_vector<User_Name_t>& arr,
                                             std::string& out);
inline void serialize_CreateTaskRequest_array_into(const std::vector<CreateTaskRequest>& arr,
                                                   std::string& out);
inline void serialize_CreateTaskRequest_array_into(const arena_vector<CreateTaskRequest>& arr,
                                                   std::string& out);
inline void
serialize_CreateTaskRequest_Title_t_array_into(const std::vector<CreateTaskRequest_Title_t>& arr,
                                               std::string& out);
inline void
serialize_CreateTaskRequest_Title_t_array_into(const arena_vector<CreateTaskRequest_Title_t>& arr,
                                               std::string& out);
inline void serialize_CreateTaskRequest_Description_t_array_into(
    const std::vector<CreateTaskRequest_Description_t>& arr, std::string& out);
inline void serialize_CreateTaskRequest_Description_t_array_into(
    const arena_vector<CreateTaskRequest_Description_t>& arr, std::string& out);
inline void serialize_CreateTaskRequest_Priority_t_array_into(
    const std::vector<CreateTaskRequest_Priority_t>& arr, std::string& out);
inline void serialize_CreateTaskRequest_Priority_t_array_into(
    const arena_vector<CreateTaskRequest_Priority_t>& arr, std::string& out);
inline void
serialize_CreateTaskRequest_Tags_t_array_into(const std::vector<CreateTaskRequest_Tags_t>& arr,
                                              std::string& out);
inline void
serialize_CreateTaskRequest_Tags_t_array_into(const arena_vector<CreateTaskRequest_Tags_t>& arr,
                                              std::string& out);
inline void
serialize_CreateTaskRequest_Item_t_array_into(const std::vector<CreateTaskRequest_Item_t>& arr,
                                              std::string& out);
inline void
serialize_CreateTaskRequest_Item_t_array_into(const arena_vector<CreateTaskRequest_Item_t>& arr,
                                              std::string& out);
inline void serialize_CreateTaskRequest_Assignee_id_t_array_into(
    const std::vector<CreateTaskRequest_Assignee_id_t>& arr, std::string& out);
inline void serialize_CreateTaskRequest_Assignee_id_t_array_into(
    const arena_vector<CreateTaskRequest_Assignee_id_t>& arr, std::string& out);
inline void serialize_CreateTaskRequest_Due_date_t_array_into(
    const std::vector<CreateTaskRequest_Due_date_t>& arr, std::string& out);
inline void serialize_CreateTaskRequest_Due_date_t_array_into(
    const arena_vector<CreateTaskRequest_Due_date_t>& arr, std::string& out);
inline void serialize_UpdateTaskRequest_array_into(const std::vector<UpdateTaskRequest>& arr,
                                                   std::string& out);
inline void serialize_UpdateTaskRequest_array_into(const arena_vector<UpdateTaskRequest>& arr,
                                                   std::string& out);
inline void
serialize_UpdateTaskRequest_Title_t_array_into(const std::vector<UpdateTaskRequest_Title_t>& arr,
                                               std::string& out);
inline void
serialize_UpdateTaskRequest_Title_t_array_into(const arena_vector<UpdateTaskRequest_Title_t>& arr,
                                               std::string& out);
inline void serialize_UpdateTaskRequest_Description_t_array_into(
    const std::vector<UpdateTaskRequest_Description_t>& arr, std::string& out);
inline void serialize_UpdateTaskRequest_Description_t_array_into(
    const arena_vector<UpdateTaskRequest_Description_t>& arr, std::string& out);
inline void
serialize_UpdateTaskRequest_Status_t_array_into(const std::vector<UpdateTaskRequest_Status_t>& arr,
                                                std::string& out);
inline void
serialize_UpdateTaskRequest_Status_t_array_into(const arena_vector<UpdateTaskRequest_Status_t>& arr,
                                                std::string& out);
inline void serialize_UpdateTaskRequest_Priority_t_array_into(
    const std::vector<UpdateTaskRequest_Priority_t>& arr, std::string& out);
inline void serialize_UpdateTaskRequest_Priority_t_array_into(
    const arena_vector<UpdateTaskRequest_Priority_t>& arr, std::string& out);
inline void
serialize_UpdateTaskRequest_Tags_t_array_into(const std::vector<UpdateTaskRequest_Tags_t>& arr,
                                              std::string& out);
inline void
serialize_UpdateTaskRequest_Tags_t_array_into(const arena_vector<UpdateTaskRequest_Tags_t>& arr,
                                              std::string& out);
inline void
serialize_UpdateTaskRequest_Item_t_array_into(const std::vector<UpdateTaskRequest_Item_t>& arr,
                                              std::string& out);
inline void
serialize_UpdateTaskRequest_Item_t_array_into(const arena_vector<UpdateTaskRequest_Item_t>& arr,
                                              std::string& out);
inline void serialize_UpdateTaskRequest_Assignee_id_t_array_into(
    const std::vector<UpdateTaskRequest_Assignee_id_t>& arr, std::string& out);
inline void serialize_UpdateTaskRequest_Assignee_id_t_array_into(
    const arena_vector<UpdateTaskRequest_Assignee_id_t>& arr, std::string& out);
inline void serialize_UpdateTaskRequest_Due_date_t_array_into(
    const std::vector<UpdateTaskRequest_Due_date_t>& arr, std::string& out);
inline void serialize_UpdateTaskRequest_Due_date_t_array_into(
    const arena_vector<UpdateTaskRequest_Due_date_t>& arr, std::string& out);
inline void serialize_BatchCreateRequest_array_into(const std::vector<BatchCreateRequest>& arr,
                                                    std::string& out);
inline void serialize_BatchCreateRequest_array_into(const arena_vector<BatchCreateRequest>& arr,
                                                    std::string& out);
inline void
serialize_BatchCreateRequest_Tasks_t_array_into(const std::vector<BatchCreateRequest_Tasks_t>& arr,
                                                std::string& out);
inline void
serialize_BatchCreateRequest_Tasks_t_array_into(const arena_vector<BatchCreateRequest_Tasks_t>& arr,
                                                std::string& out);
inline void
serialize_BatchCreateRequest_Item_t_array_into(const std::vector<BatchCreateRequest_Item_t>& arr,
                                               std::string& out);
inline void
serialize_BatchCreateRequest_Item_t_array_into(const arena_vector<BatchCreateRequest_Item_t>& arr,
                                               std::string& out);
inline void serialize_BatchCreateResponse_array_into(const std::vector<BatchCreateResponse>& arr,
                                                     std::string& out);
inline void serialize_BatchCreateResponse_array_into(const arena_vector<BatchCreateResponse>& arr,
                                                     std::string& out);
inline void serialize_BatchCreateResponse_Created_t_array_into(
    const std::vector<BatchCreateResponse_Created_t>& arr, std::string& out);
inline void serialize_BatchCreateResponse_Created_t_array_into(
    const arena_vector<BatchCreateResponse_Created_t>& arr, std::string& out);
inline void
serialize_BatchCreateResponse_Item_t_array_into(const std::vector<BatchCreateResponse_Item_t>& arr,
                                                std::string& out);
inline void
serialize_BatchCreateResponse_Item_t_array_into(const arena_vector<BatchCreateResponse_Item_t>& arr,
                                                std::string& out);
inline void serialize_BatchCreateResponse_Failed_t_array_into(
    const std::vector<BatchCreateResponse_Failed_t>& arr, std::string& out);
inline void serialize_BatchCreateResponse_Failed_t_array_into(
    const arena_vector<BatchCreateResponse_Failed_t>& arr, std::string& out);
inline void serialize_BatchCreateResponse_Item_t_1_array_into(
    const std::vector<BatchCreateResponse_Item_t_1>& arr, std::string& out);
inline void serialize_BatchCreateResponse_Item_t_1_array_into(
    const arena_vector<BatchCreateResponse_Item_t_1>& arr, std::string& out);
inline void serialize_BatchCreateResponse_Index_t_array_into(
    const std::vector<BatchCreateResponse_Index_t>& arr, std::string& out);
inline void serialize_BatchCreateResponse_Index_t_array_into(
    const arena_vector<BatchCreateResponse_Index_t>& arr, std::string& out);
inline void serialize_BatchCreateResponse_Error_t_array_into(
    const std::vector<BatchCreateResponse_Error_t>& arr, std::string& out);
inline void serialize_BatchCreateResponse_Error_t_array_into(
    const arena_vector<BatchCreateResponse_Error_t>& arr, std::string& out);
inline void serialize_SearchRequest_array_into(const std::vector<SearchRequest>& arr,
                                               std::string& out);
inline void serialize_SearchRequest_array_into(const arena_vector<SearchRequest>& arr,
                                               std::string& out);
inline void serialize_SearchRequest_Title_contains_t_array_into(
    const std::vector<SearchRequest_Title_contains_t>& arr, std::string& out);
inline void serialize_SearchRequest_Title_contains_t_array_into(
    const arena_vector<SearchRequest_Title_contains_t>& arr, std::string& out);
inline void
serialize_SearchRequest_Statuses_t_array_into(const std::vector<SearchRequest_Statuses_t>& arr,
                                              std::string& out);
inline void
serialize_SearchRequest_Statuses_t_array_into(const arena_vector<SearchRequest_Statuses_t>& arr,
                                              std::string& out);
inline void serialize_SearchRequest_Item_t_array_into(const std::vector<SearchRequest_Item_t>& arr,
                                                      std::string& out);
inline void serialize_SearchRequest_Item_t_array_into(const arena_vector<SearchRequest_Item_t>& arr,
                                                      std::string& out);
inline void serialize_SearchRequest_Min_priority_t_array_into(
    const std::vector<SearchRequest_Min_priority_t>& arr, std::string& out);
inline void serialize_SearchRequest_Min_priority_t_array_into(
    const arena_vector<SearchRequest_Min_priority_t>& arr, std::string& out);
inline void serialize_SearchRequest_Max_priority_t_array_into(
    const std::vector<SearchRequest_Max_priority_t>& arr, std::string& out);
inline void serialize_SearchRequest_Max_priority_t_array_into(
    const arena_vector<SearchRequest_Max_priority_t>& arr, std::string& out);
inline void serialize_SearchRequest_Tags_t_array_into(const std::vector<SearchRequest_Tags_t>& arr,
                                                      std::string& out);
inline void serialize_SearchRequest_Tags_t_array_into(const arena_vector<SearchRequest_Tags_t>& arr,
                                                      std::string& out);
inline void
serialize_SearchRequest_Item_t_1_array_into(const std::vector<SearchRequest_Item_t_1>& arr,
                                            std::string& out);
inline void
serialize_SearchRequest_Item_t_1_array_into(const arena_vector<SearchRequest_Item_t_1>& arr,
                                            std::string& out);
inline void serialize_SearchRequest_Created_after_t_array_into(
    const std::vector<SearchRequest_Created_after_t>& arr, std::string& out);
inline void serialize_SearchRequest_Created_after_t_array_into(
    const arena_vector<SearchRequest_Created_after_t>& arr, std::string& out);
inline void serialize_SearchRequest_Created_before_t_array_into(
    const std::vector<SearchRequest_Created_before_t>& arr, std::string& out);
inline void serialize_SearchRequest_Created_before_t_array_into(
    const arena_vector<SearchRequest_Created_before_t>& arr, std::string& out);
inline void serialize_SearchRequest_Has_assignee_t_array_into(
    const std::vector<SearchRequest_Has_assignee_t>& arr, std::string& out);
inline void serialize_SearchRequest_Has_assignee_t_array_into(
    const arena_vector<SearchRequest_Has_assignee_t>& arr, std::string& out);
inline void serialize_TaskList_array_into(const std::vector<TaskList>& arr, std::string& out);
inline void serialize_TaskList_array_into(const arena_vector<TaskList>& arr, std::string& out);
inline void serialize_TaskList_Tasks_t_array_into(const std::vector<TaskList_Tasks_t>& arr,
                                                  std::string& out);
inline void serialize_TaskList_Tasks_t_array_into(const arena_vector<TaskList_Tasks_t>& arr,
                                                  std::string& out);
inline void serialize_TaskList_Item_t_array_into(const std::vector<TaskList_Item_t>& arr,
                                                 std::string& out);
inline void serialize_TaskList_Item_t_array_into(const arena_vector<TaskList_Item_t>& arr,
                                                 std::string& out);
inline void serialize_TaskList_Total_t_array_into(const std::vector<TaskList_Total_t>& arr,
                                                  std::string& out);
inline void serialize_TaskList_Total_t_array_into(const arena_vector<TaskList_Total_t>& arr,
                                                  std::string& out);
inline void serialize_TaskList_Has_more_t_array_into(const std::vector<TaskList_Has_more_t>& arr,
                                                     std::string& out);
inline void serialize_TaskList_Has_more_t_array_into(const arena_vector<TaskList_Has_more_t>& arr,
                                                     std::string& out);
inline void serialize_HealthResponse_array_into(const std::vector<HealthResponse>& arr,
                                                std::string& out);
inline void serialize_HealthResponse_array_into(const arena_vector<HealthResponse>& arr,
                                                std::string& out);
inline void
serialize_HealthResponse_Status_t_array_into(const std::vector<HealthResponse_Status_t>& arr,
                                             std::string& out);
inline void
serialize_HealthResponse_Status_t_array_into(const arena_vector<HealthResponse_Status_t>& arr,
                                             std::string& out);
inline void
serialize_HealthResponse_Timestamp_t_array_into(const std::vector<HealthResponse_Timestamp_t>& arr,
                                                std::string& out);
inline void
serialize_HealthResponse_Timestamp_t_array_into(const arena_vector<HealthResponse_Timestamp_t>& arr,
                                                std::string& out);
inline void serialize_HealthResponse_Uptime_seconds_t_array_into(
    const std::vector<HealthResponse_Uptime_seconds_t>& arr, std::string& out);
inline void serialize_HealthResponse_Uptime_seconds_t_array_into(
    const arena_vector<HealthResponse_Uptime_seconds_t>& arr, std::string& out);
inline void serialize_HealthResponse_Total_requests_t_array_into(
    const std::vector<HealthResponse_Total_requests_t>& arr, std::string& out);
inline void serialize_HealthResponse_Total_requests_t_array_into(
    const arena_vector<HealthResponse_Total_requests_t>& arr, std::string& out);
inline void serialize_ProblemDetails_array_into(const std::vector<ProblemDetails>& arr,
                                                std::string& out);
inline void serialize_ProblemDetails_array_into(const arena_vector<ProblemDetails>& arr,
                                                std::string& out);
inline void
serialize_ProblemDetails_Type_t_array_into(const std::vector<ProblemDetails_Type_t>& arr,
                                           std::string& out);
inline void
serialize_ProblemDetails_Type_t_array_into(const arena_vector<ProblemDetails_Type_t>& arr,
                                           std::string& out);
inline void
serialize_ProblemDetails_Title_t_array_into(const std::vector<ProblemDetails_Title_t>& arr,
                                            std::string& out);
inline void
serialize_ProblemDetails_Title_t_array_into(const arena_vector<ProblemDetails_Title_t>& arr,
                                            std::string& out);
inline void
serialize_ProblemDetails_Status_t_array_into(const std::vector<ProblemDetails_Status_t>& arr,
                                             std::string& out);
inline void
serialize_ProblemDetails_Status_t_array_into(const arena_vector<ProblemDetails_Status_t>& arr,
                                             std::string& out);
inline void
serialize_ProblemDetails_Detail_t_array_into(const std::vector<ProblemDetails_Detail_t>& arr,
                                             std::string& out);
inline void
serialize_ProblemDetails_Detail_t_array_into(const arena_vector<ProblemDetails_Detail_t>& arr,
                                             std::string& out);
inline void
serialize_ProblemDetails_Instance_t_array_into(const std::vector<ProblemDetails_Instance_t>& arr,
                                               std::string& out);
inline void
serialize_ProblemDetails_Instance_t_array_into(const arena_vector<ProblemDetails_Instance_t>& arr,
                                               std::string& out);
inline void
serialize_listTasks_param_status_array_into(const std::vector<listTasks_param_status>& arr,
                                            std::string& out);
inline void
serialize_listTasks_param_status_array_into(const arena_vector<listTasks_param_status>& arr,
                                            std::string& out);
inline void
serialize_listTasks_param_priority_array_into(const std::vector<listTasks_param_priority>& arr,
                                              std::string& out);
inline void
serialize_listTasks_param_priority_array_into(const arena_vector<listTasks_param_priority>& arr,
                                              std::string& out);
inline void
serialize_listTasks_param_limit_array_into(const std::vector<listTasks_param_limit>& arr,
                                           std::string& out);
inline void
serialize_listTasks_param_limit_array_into(const arena_vector<listTasks_param_limit>& arr,
                                           std::string& out);
inline void
serialize_listTasks_param_offset_array_into(const std::vector<listTasks_param_offset>& arr,
                                            std::string& out);
inline void
serialize_listTasks_param_offset_array_into(const arena_vector<listTasks_param_offset>& arr,
                                            std::string& out);
inline void serialize_schema_array_into(const std::vector<schema>& arr, std::string& out);
inline void serialize_schema_array_into(const arena_vector<schema>& arr, std::string& out);
inline void serialize_schema_1_array_into(const std::vector<schema_1>& arr, std::string& out);
inline void serialize_schema_1_array_into(const arena_vector<schema_1>& arr, std::string& out);
inline void serialize_schema_2_array_into(const std::vector<schema_2>& arr, std::string& out);
inline void serialize_schema_2_array_into(const arena_vector<schema_2>& arr, std::string& out);
inline void serialize_schema_3_array_into(const std::vector<schema_3>& arr, std::string& out);
inline void serialize_schema_3_array_into(const arena_vector<schema_3>& arr, std::string& out);
inline void serialize_getTask_param_id_array_into(const std::vector<getTask_param_id>& arr,
                                                  std::string& out);
inline void serialize_getTask_param_id_array_into(const arena_vector<getTask_param_id>& arr,
                                                  std::string& out);
inline void serialize_schema_4_array_into(const std::vector<schema_4>& arr, std::string& out);
inline void serialize_schema_4_array_into(const arena_vector<schema_4>& arr, std::string& out);
inline void serialize_schema_5_array_into(const std::vector<schema_5>& arr, std::string& out);
inline void serialize_schema_5_array_into(const arena_vector<schema_5>& arr, std::string& out);
inline void serialize_updateTask_param_id_array_into(const std::vector<updateTask_param_id>& arr,
                                                     std::string& out);
inline void serialize_updateTask_param_id_array_into(const arena_vector<updateTask_param_id>& arr,
                                                     std::string& out);
inline void serialize_schema_6_array_into(const std::vector<schema_6>& arr, std::string& out);
inline void serialize_schema_6_array_into(const arena_vector<schema_6>& arr, std::string& out);
inline void serialize_schema_7_array_into(const std::vector<schema_7>& arr, std::string& out);
inline void serialize_schema_7_array_into(const arena_vector<schema_7>& arr, std::string& out);
inline void serialize_schema_8_array_into(const std::vector<schema_8>& arr, std::string& out);
inline void serialize_schema_8_array_into(const arena_vector<schema_8>& arr, std::string& out);
inline void serialize_deleteTask_param_id_array_into(const std::vector<deleteTask_param_id>& arr,
                                                     std::string& out);
inline void serialize_deleteTask_param_id_array_into(const arena_vector<deleteTask_param_id>& arr,
                                                     std::string& out);
inline void serialize_schema_9_array_into(const std::vector<schema_9>& arr, std::string& out);
inline void serialize_schema_9_array_into(const arena_vector<schema_9>& arr, std::string& out);
inline void serialize_schema_10_array_into(const std::vector<schema_10>& arr, std::string& out);
inline void serialize_schema_10_array_into(const arena_vector<schema_10>& arr, std::string& out);
inline void serialize_schema_11_array_into(const std::vector<schema_11>& arr, std::string& out);
inline void serialize_schema_11_array_into(const arena_vector<schema_11>& arr, std::string& out);
inline void serialize_schema_12_array_into(const std::vector<schema_12>& arr, std::string& out);
inline void serialize_schema_12_array_into(const arena_vector<schema_12>& arr, std::string& out);
inline void serialize_schema_13_array_into(const std::vector<schema_13>& arr, std::string& out);
inline void serialize_schema_13_array_into(const arena_vector<schema_13>& arr, std::string& out);
inline void serialize_schema_14_array_into(const std::vector<schema_14>& arr, std::string& out);
inline void serialize_schema_14_array_into(const arena_vector<schema_14>& arr, std::string& out);

inline std::string serialize_Task_array(const std::vector<Task>& arr);
inline std::string serialize_Task_array(const arena_vector<Task>& arr);
inline std::string serialize_Task_Id_t_array(const std::vector<Task_Id_t>& arr);
inline std::string serialize_Task_Id_t_array(const arena_vector<Task_Id_t>& arr);
inline std::string serialize_Task_Title_t_array(const std::vector<Task_Title_t>& arr);
inline std::string serialize_Task_Title_t_array(const arena_vector<Task_Title_t>& arr);
inline std::string serialize_Task_Description_t_array(const std::vector<Task_Description_t>& arr);
inline std::string serialize_Task_Description_t_array(const arena_vector<Task_Description_t>& arr);
inline std::string serialize_Task_Status_t_array(const std::vector<Task_Status_t>& arr);
inline std::string serialize_Task_Status_t_array(const arena_vector<Task_Status_t>& arr);
inline std::string serialize_Task_Priority_t_array(const std::vector<Task_Priority_t>& arr);
inline std::string serialize_Task_Priority_t_array(const arena_vector<Task_Priority_t>& arr);
inline std::string serialize_Task_Tags_t_array(const std::vector<Task_Tags_t>& arr);
inline std::string serialize_Task_Tags_t_array(const arena_vector<Task_Tags_t>& arr);
inline std::string serialize_Task_Item_t_array(const std::vector<Task_Item_t>& arr);
inline std::string serialize_Task_Item_t_array(const arena_vector<Task_Item_t>& arr);
inline std::string serialize_Task_Assignee_t_array(const std::vector<Task_Assignee_t>& arr);
inline std::string serialize_Task_Assignee_t_array(const arena_vector<Task_Assignee_t>& arr);
inline std::string serialize_Task_Due_date_t_array(const std::vector<Task_Due_date_t>& arr);
inline std::string serialize_Task_Due_date_t_array(const arena_vector<Task_Due_date_t>& arr);
inline std::string serialize_Task_Created_at_t_array(const std::vector<Task_Created_at_t>& arr);
inline std::string serialize_Task_Created_at_t_array(const arena_vector<Task_Created_at_t>& arr);
inline std::string serialize_Task_Updated_at_t_array(const std::vector<Task_Updated_at_t>& arr);
inline std::string serialize_Task_Updated_at_t_array(const arena_vector<Task_Updated_at_t>& arr);
inline std::string serialize_Task_Metadata_t_array(const std::vector<Task_Metadata_t>& arr);
inline std::string serialize_Task_Metadata_t_array(const arena_vector<Task_Metadata_t>& arr);
inline std::string serialize_User_array(const std::vector<User>& arr);
inline std::string serialize_User_array(const arena_vector<User>& arr);
inline std::string serialize_User_Id_t_array(const std::vector<User_Id_t>& arr);
inline std::string serialize_User_Id_t_array(const arena_vector<User_Id_t>& arr);
inline std::string serialize_User_Email_t_array(const std::vector<User_Email_t>& arr);
inline std::string serialize_User_Email_t_array(const arena_vector<User_Email_t>& arr);
inline std::string serialize_User_Name_t_array(const std::vector<User_Name_t>& arr);
inline std::string serialize_User_Name_t_array(const arena_vector<User_Name_t>& arr);
inline std::string serialize_CreateTaskRequest_array(const std::vector<CreateTaskRequest>& arr);
inline std::string serialize_CreateTaskRequest_array(const arena_vector<CreateTaskRequest>& arr);
inline std::string
serialize_CreateTaskRequest_Title_t_array(const std::vector<CreateTaskRequest_Title_t>& arr);
inline std::string
serialize_CreateTaskRequest_Title_t_array(const arena_vector<CreateTaskRequest_Title_t>& arr);
inline std::string serialize_CreateTaskRequest_Description_t_array(
    const std::vector<CreateTaskRequest_Description_t>& arr);
inline std::string serialize_CreateTaskRequest_Description_t_array(
    const arena_vector<CreateTaskRequest_Description_t>& arr);
inline std::string
serialize_CreateTaskRequest_Priority_t_array(const std::vector<CreateTaskRequest_Priority_t>& arr);
inline std::string
serialize_CreateTaskRequest_Priority_t_array(const arena_vector<CreateTaskRequest_Priority_t>& arr);
inline std::string
serialize_CreateTaskRequest_Tags_t_array(const std::vector<CreateTaskRequest_Tags_t>& arr);
inline std::string
serialize_CreateTaskRequest_Tags_t_array(const arena_vector<CreateTaskRequest_Tags_t>& arr);
inline std::string
serialize_CreateTaskRequest_Item_t_array(const std::vector<CreateTaskRequest_Item_t>& arr);
inline std::string
serialize_CreateTaskRequest_Item_t_array(const arena_vector<CreateTaskRequest_Item_t>& arr);
inline std::string serialize_CreateTaskRequest_Assignee_id_t_array(
    const std::vector<CreateTaskRequest_Assignee_id_t>& arr);
inline std::string serialize_CreateTaskRequest_Assignee_id_t_array(
    const arena_vector<CreateTaskRequest_Assignee_id_t>& arr);
inline std::string
serialize_CreateTaskRequest_Due_date_t_array(const std::vector<CreateTaskRequest_Due_date_t>& arr);
inline std::string
serialize_CreateTaskRequest_Due_date_t_array(const arena_vector<CreateTaskRequest_Due_date_t>& arr);
inline std::string serialize_UpdateTaskRequest_array(const std::vector<UpdateTaskRequest>& arr);
inline std::string serialize_UpdateTaskRequest_array(const arena_vector<UpdateTaskRequest>& arr);
inline std::string
serialize_UpdateTaskRequest_Title_t_array(const std::vector<UpdateTaskRequest_Title_t>& arr);
inline std::string
serialize_UpdateTaskRequest_Title_t_array(const arena_vector<UpdateTaskRequest_Title_t>& arr);
inline std::string serialize_UpdateTaskRequest_Description_t_array(
    const std::vector<UpdateTaskRequest_Description_t>& arr);
inline std::string serialize_UpdateTaskRequest_Description_t_array(
    const arena_vector<UpdateTaskRequest_Description_t>& arr);
inline std::string
serialize_UpdateTaskRequest_Status_t_array(const std::vector<UpdateTaskRequest_Status_t>& arr);
inline std::string
serialize_UpdateTaskRequest_Status_t_array(const arena_vector<UpdateTaskRequest_Status_t>& arr);
inline std::string
serialize_UpdateTaskRequest_Priority_t_array(const std::vector<UpdateTaskRequest_Priority_t>& arr);
inline std::string
serialize_UpdateTaskRequest_Priority_t_array(const arena_vector<UpdateTaskRequest_Priority_t>& arr);
inline std::string
serialize_UpdateTaskRequest_Tags_t_array(const std::vector<UpdateTaskRequest_Tags_t>& arr);
inline std::string
serialize_UpdateTaskRequest_Tags_t_array(const arena_vector<UpdateTaskRequest_Tags_t>& arr);
inline std::string
serialize_UpdateTaskRequest_Item_t_array(const std::vector<UpdateTaskRequest_Item_t>& arr);
inline std::string
serialize_UpdateTaskRequest_Item_t_array(const arena_vector<UpdateTaskRequest_Item_t>& arr);
inline std::string serialize_UpdateTaskRequest_Assignee_id_t_array(
    const std::vector<UpdateTaskRequest_Assignee_id_t>& arr);
inline std::string serialize_UpdateTaskRequest_Assignee_id_t_array(
    const arena_vector<UpdateTaskRequest_Assignee_id_t>& arr);
inline std::string
serialize_UpdateTaskRequest_Due_date_t_array(const std::vector<UpdateTaskRequest_Due_date_t>& arr);
inline std::string
serialize_UpdateTaskRequest_Due_date_t_array(const arena_vector<UpdateTaskRequest_Due_date_t>& arr);
inline std::string serialize_BatchCreateRequest_array(const std::vector<BatchCreateRequest>& arr);
inline std::string serialize_BatchCreateRequest_array(const arena_vector<BatchCreateRequest>& arr);
inline std::string
serialize_BatchCreateRequest_Tasks_t_array(const std::vector<BatchCreateRequest_Tasks_t>& arr);
inline std::string
serialize_BatchCreateRequest_Tasks_t_array(const arena_vector<BatchCreateRequest_Tasks_t>& arr);
inline std::string
serialize_BatchCreateRequest_Item_t_array(const std::vector<BatchCreateRequest_Item_t>& arr);
inline std::string
serialize_BatchCreateRequest_Item_t_array(const arena_vector<BatchCreateRequest_Item_t>& arr);
inline std::string serialize_BatchCreateResponse_array(const std::vector<BatchCreateResponse>& arr);
inline std::string
serialize_BatchCreateResponse_array(const arena_vector<BatchCreateResponse>& arr);
inline std::string serialize_BatchCreateResponse_Created_t_array(
    const std::vector<BatchCreateResponse_Created_t>& arr);
inline std::string serialize_BatchCreateResponse_Created_t_array(
    const arena_vector<BatchCreateResponse_Created_t>& arr);
inline std::string
serialize_BatchCreateResponse_Item_t_array(const std::vector<BatchCreateResponse_Item_t>& arr);
inline std::string
serialize_BatchCreateResponse_Item_t_array(const arena_vector<BatchCreateResponse_Item_t>& arr);
inline std::string
serialize_BatchCreateResponse_Failed_t_array(const std::vector<BatchCreateResponse_Failed_t>& arr);
inline std::string
serialize_BatchCreateResponse_Failed_t_array(const arena_vector<BatchCreateResponse_Failed_t>& arr);
inline std::string
serialize_BatchCreateResponse_Item_t_1_array(const std::vector<BatchCreateResponse_Item_t_1>& arr);
inline std::string
serialize_BatchCreateResponse_Item_t_1_array(const arena_vector<BatchCreateResponse_Item_t_1>& arr);
inline std::string
serialize_BatchCreateResponse_Index_t_array(const std::vector<BatchCreateResponse_Index_t>& arr);
inline std::string
serialize_BatchCreateResponse_Index_t_array(const arena_vector<BatchCreateResponse_Index_t>& arr);
inline std::string
serialize_BatchCreateResponse_Error_t_array(const std::vector<BatchCreateResponse_Error_t>& arr);
inline std::string
serialize_BatchCreateResponse_Error_t_array(const arena_vector<BatchCreateResponse_Error_t>& arr);
inline std::string serialize_SearchRequest_array(const std::vector<SearchRequest>& arr);
inline std::string serialize_SearchRequest_array(const arena_vector<SearchRequest>& arr);
inline std::string serialize_SearchRequest_Title_contains_t_array(
    const std::vector<SearchRequest_Title_contains_t>& arr);
inline std::string serialize_SearchRequest_Title_contains_t_array(
    const arena_vector<SearchRequest_Title_contains_t>& arr);
inline std::string
serialize_SearchRequest_Statuses_t_array(const std::vector<SearchRequest_Statuses_t>& arr);
inline std::string
serialize_SearchRequest_Statuses_t_array(const arena_vector<SearchRequest_Statuses_t>& arr);
inline std::string
serialize_SearchRequest_Item_t_array(const std::vector<SearchRequest_Item_t>& arr);
inline std::string
serialize_SearchRequest_Item_t_array(const arena_vector<SearchRequest_Item_t>& arr);
inline std::string
serialize_SearchRequest_Min_priority_t_array(const std::vector<SearchRequest_Min_priority_t>& arr);
inline std::string
serialize_SearchRequest_Min_priority_t_array(const arena_vector<SearchRequest_Min_priority_t>& arr);
inline std::string
serialize_SearchRequest_Max_priority_t_array(const std::vector<SearchRequest_Max_priority_t>& arr);
inline std::string
serialize_SearchRequest_Max_priority_t_array(const arena_vector<SearchRequest_Max_priority_t>& arr);
inline std::string
serialize_SearchRequest_Tags_t_array(const std::vector<SearchRequest_Tags_t>& arr);
inline std::string
serialize_SearchRequest_Tags_t_array(const arena_vector<SearchRequest_Tags_t>& arr);
inline std::string
serialize_SearchRequest_Item_t_1_array(const std::vector<SearchRequest_Item_t_1>& arr);
inline std::string
serialize_SearchRequest_Item_t_1_array(const arena_vector<SearchRequest_Item_t_1>& arr);
inline std::string serialize_SearchRequest_Created_after_t_array(
    const std::vector<SearchRequest_Created_after_t>& arr);
inline std::string serialize_SearchRequest_Created_after_t_array(
    const arena_vector<SearchRequest_Created_after_t>& arr);
inline std::string serialize_SearchRequest_Created_before_t_array(
    const std::vector<SearchRequest_Created_before_t>& arr);
inline std::string serialize_SearchRequest_Created_before_t_array(
    const arena_vector<SearchRequest_Created_before_t>& arr);
inline std::string
serialize_SearchRequest_Has_assignee_t_array(const std::vector<SearchRequest_Has_assignee_t>& arr);
inline std::string
serialize_SearchRequest_Has_assignee_t_array(const arena_vector<SearchRequest_Has_assignee_t>& arr);
inline std::string serialize_TaskList_array(const std::vector<TaskList>& arr);
inline std::string serialize_TaskList_array(const arena_vector<TaskList>& arr);
inline std::string serialize_TaskList_Tasks_t_array(const std::vector<TaskList_Tasks_t>& arr);
inline std::string serialize_TaskList_Tasks_t_array(const arena_vector<TaskList_Tasks_t>& arr);
inline std::string serialize_TaskList_Item_t_array(const std::vector<TaskList_Item_t>& arr);
inline std::string serialize_TaskList_Item_t_array(const arena_vector<TaskList_Item_t>& arr);
inline std::string serialize_TaskList_Total_t_array(const std::vector<TaskList_Total_t>& arr);
inline std::string serialize_TaskList_Total_t_array(const arena_vector<TaskList_Total_t>& arr);
inline std::string serialize_TaskList_Has_more_t_array(const std::vector<TaskList_Has_more_t>& arr);
inline std::string
serialize_TaskList_Has_more_t_array(const arena_vector<TaskList_Has_more_t>& arr);
inline std::string serialize_HealthResponse_array(const std::vector<HealthResponse>& arr);
inline std::string serialize_HealthResponse_array(const arena_vector<HealthResponse>& arr);
inline std::string
serialize_HealthResponse_Status_t_array(const std::vector<HealthResponse_Status_t>& arr);
inline std::string
serialize_HealthResponse_Status_t_array(const arena_vector<HealthResponse_Status_t>& arr);
inline std::string
serialize_HealthResponse_Timestamp_t_array(const std::vector<HealthResponse_Timestamp_t>& arr);
inline std::string
serialize_HealthResponse_Timestamp_t_array(const arena_vector<HealthResponse_Timestamp_t>& arr);
inline std::string serialize_HealthResponse_Uptime_seconds_t_array(
    const std::vector<HealthResponse_Uptime_seconds_t>& arr);
inline std::string serialize_HealthResponse_Uptime_seconds_t_array(
    const arena_vector<HealthResponse_Uptime_seconds_t>& arr);
inline std::string serialize_HealthResponse_Total_requests_t_array(
    const std::vector<HealthResponse_Total_requests_t>& arr);
inline std::string serialize_HealthResponse_Total_requests_t_array(
    const arena_vector<HealthResponse_Total_requests_t>& arr);
inline std::string serialize_ProblemDetails_array(const std::vector<ProblemDetails>& arr);
inline std::string serialize_ProblemDetails_array(const arena_vector<ProblemDetails>& arr);
inline std::string
serialize_ProblemDetails_Type_t_array(const std::vector<ProblemDetails_Type_t>& arr);
inline std::string
serialize_ProblemDetails_Type_t_array(const arena_vector<ProblemDetails_Type_t>& arr);
inline std::string
serialize_ProblemDetails_Title_t_array(const std::vector<ProblemDetails_Title_t>& arr);
inline std::string
serialize_ProblemDetails_Title_t_array(const arena_vector<ProblemDetails_Title_t>& arr);
inline std::string
serialize_ProblemDetails_Status_t_array(const std::vector<ProblemDetails_Status_t>& arr);
inline std::string
serialize_ProblemDetails_Status_t_array(const arena_vector<ProblemDetails_Status_t>& arr);
inline std::string
serialize_ProblemDetails_Detail_t_array(const std::vector<ProblemDetails_Detail_t>& arr);
inline std::string
serialize_ProblemDetails_Detail_t_array(const arena_vector<ProblemDetails_Detail_t>& arr);
inline std::string
serialize_ProblemDetails_Instance_t_array(const std::vector<ProblemDetails_Instance_t>& arr);
inline std::string
serialize_ProblemDetails_Instance_t_array(const arena_vector<ProblemDetails_Instance_t>& arr);
inline std::string
serialize_listTasks_param_status_array(const std::vector<listTasks_param_status>& arr);
inline std::string
serialize_listTasks_param_status_array(const arena_vector<listTasks_param_status>& arr);
inline std::string
serialize_listTasks_param_priority_array(const std::vector<listTasks_param_priority>& arr);
inline std::string
serialize_listTasks_param_priority_array(const arena_vector<listTasks_param_priority>& arr);
inline std::string
serialize_listTasks_param_limit_array(const std::vector<listTasks_param_limit>& arr);
inline std::string
serialize_listTasks_param_limit_array(const arena_vector<listTasks_param_limit>& arr);
inline std::string
serialize_listTasks_param_offset_array(const std::vector<listTasks_param_offset>& arr);
inline std::string
serialize_listTasks_param_offset_array(const arena_vector<listTasks_param_offset>& arr);
inline std::string serialize_schema_array(const std::vector<schema>& arr);
inline std::string serialize_schema_array(const arena_vector<schema>& arr);
inline std::string serialize_schema_1_array(const std::vector<schema_1>& arr);
inline std::string serialize_schema_1_array(const arena_vector<schema_1>& arr);
inline std::string serialize_schema_2_array(const std::vector<schema_2>& arr);
inline std::string serialize_schema_2_array(const arena_vector<schema_2>& arr);
inline std::string serialize_schema_3_array(const std::vector<schema_3>& arr);
inline std::string serialize_schema_3_array(const arena_vector<schema_3>& arr);
inline std::string serialize_getTask_param_id_array(const std::vector<getTask_param_id>& arr);
inline std::string serialize_getTask_param_id_array(const arena_vector<getTask_param_id>& arr);
inline std::string serialize_schema_4_array(const std::vector<schema_4>& arr);
inline std::string serialize_schema_4_array(const arena_vector<schema_4>& arr);
inline std::string serialize_schema_5_array(const std::vector<schema_5>& arr);
inline std::string serialize_schema_5_array(const arena_vector<schema_5>& arr);
inline std::string serialize_updateTask_param_id_array(const std::vector<updateTask_param_id>& arr);
inline std::string
serialize_updateTask_param_id_array(const arena_vector<updateTask_param_id>& arr);
inline std::string serialize_schema_6_array(const std::vector<schema_6>& arr);
inline std::string serialize_schema_6_array(const arena_vector<schema_6>& arr);
inline std::string serialize_schema_7_array(const std::vector<schema_7>& arr);
inline std::string serialize_schema_7_array(const arena_vector<schema_7>& arr);
inline std::string serialize_schema_8_array(const std::vector<schema_8>& arr);
inline std::string serialize_schema_8_array(const arena_vector<schema_8>& arr);
inline std::string serialize_deleteTask_param_id_array(const std::vector<deleteTask_param_id>& arr);
inline std::string
serialize_deleteTask_param_id_array(const arena_vector<deleteTask_param_id>& arr);
inline std::string serialize_schema_9_array(const std::vector<schema_9>& arr);
inline std::string serialize_schema_9_array(const arena_vector<schema_9>& arr);
inline std::string serialize_schema_10_array(const std::vector<schema_10>& arr);
inline std::string serialize_schema_10_array(const arena_vector<schema_10>& arr);
inline std::string serialize_schema_11_array(const std::vector<schema_11>& arr);
inline std::string serialize_schema_11_array(const arena_vector<schema_11>& arr);
inline std::string serialize_schema_12_array(const std::vector<schema_12>& arr);
inline std::string serialize_schema_12_array(const arena_vector<schema_12>& arr);
inline std::string serialize_schema_13_array(const std::vector<schema_13>& arr);
inline std::string serialize_schema_13_array(const arena_vector<schema_13>& arr);
inline std::string serialize_schema_14_array(const std::vector<schema_14>& arr);
inline std::string serialize_schema_14_array(const arena_vector<schema_14>& arr);

// ============================================================
// JSON Parse Functions
// ============================================================

[[nodiscard]] inline std::optional<Task> parse_Task(katana::serde::json_cursor& cur,
                                                    monotonic_arena* arena) {
    if (!cur.try_object_start())
        return std::nullopt;

    Task obj(arena);
    bool has_id = false;
    bool has_title = false;
    bool has_status = false;
    bool has_priority = false;
    bool has_created_at = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        switch (key->size()) {
        case 2:
            if (*key == "id") {
                has_id = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.id = *v;
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 4:
            if (*key == "tags") {
                if (cur.try_array_start()) {
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end())
                            break;
                        if (auto v = cur.string()) {
                            obj.tags.emplace_back(
                                v->begin(), v->end(), arena_allocator<char>(arena));
                        } else {
                            cur.skip_value();
                        }
                        cur.try_comma();
                    }
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 5:
            if (*key == "title") {
                has_title = true;
                if (auto v = cur.string()) {
                    obj.title = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 6:
            if (*key == "status") {
                has_status = true;
                if (auto v = cur.string()) {
                    auto enum_val =
                        Task_Status_t_enum_from_string(std::string_view(v->begin(), v->end()));
                    if (enum_val)
                        obj.status = *enum_val;
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 8:
            if (*key == "priority") {
                has_priority = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.priority = *v;
                } else {
                    cur.skip_value();
                }
            } else if (*key == "assignee") {
                if (auto nested = parse_User(cur, arena)) {
                    obj.assignee = std::move(*nested);
                } else {
                    cur.skip_value();
                }
            } else if (*key == "due_date") {
                if (auto v = cur.string()) {
                    obj.due_date =
                        arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else if (*key == "metadata") {
                if (auto nested = parse_Task_Metadata_t(cur, arena)) {
                    obj.metadata = std::move(*nested);
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 10:
            if (*key == "created_at") {
                has_created_at = true;
                if (auto v = cur.string()) {
                    obj.created_at =
                        arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else if (*key == "updated_at") {
                if (auto v = cur.string()) {
                    obj.updated_at =
                        arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 11:
            if (*key == "description") {
                if (auto v = cur.string()) {
                    obj.description =
                        arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!has_id)
        return std::nullopt;
    if (!has_title)
        return std::nullopt;
    if (!has_status)
        return std::nullopt;
    if (!has_priority)
        return std::nullopt;
    if (!has_created_at)
        return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<Task> parse_Task(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task(cur, arena);
}

[[nodiscard]] inline std::optional<Task_Id_t> parse_Task_Id_t(katana::serde::json_cursor& cur,
                                                              monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return Task_Id_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<Task_Id_t> parse_Task_Id_t(std::string_view json,
                                                              monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Id_t(cur, arena);
}

[[nodiscard]] inline std::optional<Task_Title_t> parse_Task_Title_t(katana::serde::json_cursor& cur,
                                                                    monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return Task_Title_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<Task_Title_t> parse_Task_Title_t(std::string_view json,
                                                                    monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Title_t(cur, arena);
}

[[nodiscard]] inline std::optional<Task_Description_t>
parse_Task_Description_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return Task_Description_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<Task_Description_t>
parse_Task_Description_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Description_t(cur, arena);
}

[[nodiscard]] inline std::optional<Task_Status_t>
parse_Task_Status_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = cur.string()) {
        return Task_Status_t_enum_from_string(std::string_view(v->begin(), v->end()));
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<Task_Status_t> parse_Task_Status_t(std::string_view json,
                                                                      monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Status_t(cur, arena);
}

[[nodiscard]] inline std::optional<Task_Priority_t>
parse_Task_Priority_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return Task_Priority_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<Task_Priority_t> parse_Task_Priority_t(std::string_view json,
                                                                          monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Priority_t(cur, arena);
}

[[nodiscard]] inline std::optional<Task_Tags_t> parse_Task_Tags_t(katana::serde::json_cursor& cur,
                                                                  monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;
    Task_Tags_t result{arena_allocator<Task_Item_t>(arena)};
    size_t reserve_hint = 0;
    for (const char* p = cur.ptr; p < cur.end; ++p) {
        if (*p == ',')
            ++reserve_hint;
    }
    if (cur.ptr < cur.end && *cur.ptr != ']')
        ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto v = cur.string()) {
            result.emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<Task_Tags_t> parse_Task_Tags_t(std::string_view json,
                                                                  monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;
    Task_Tags_t result{arena_allocator<Task_Item_t>(arena)};
    size_t reserve_hint = 0;
    for (char ch : json) {
        if (ch == ',')
            ++reserve_hint;
    }
    if (!json.empty() && json != "[]")
        ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto v = cur.string()) {
            result.emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<Task_Item_t> parse_Task_Item_t(katana::serde::json_cursor& cur,
                                                                  monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return Task_Item_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<Task_Item_t> parse_Task_Item_t(std::string_view json,
                                                                  monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Item_t(cur, arena);
}

[[nodiscard]] inline std::optional<Task_Assignee_t>
parse_Task_Assignee_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return Task_Assignee_t{};
}

[[nodiscard]] inline std::optional<Task_Assignee_t> parse_Task_Assignee_t(std::string_view json,
                                                                          monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Assignee_t(cur, arena);
}

[[nodiscard]] inline std::optional<Task_Due_date_t>
parse_Task_Due_date_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return Task_Due_date_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<Task_Due_date_t> parse_Task_Due_date_t(std::string_view json,
                                                                          monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Due_date_t(cur, arena);
}

[[nodiscard]] inline std::optional<Task_Created_at_t>
parse_Task_Created_at_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return Task_Created_at_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<Task_Created_at_t>
parse_Task_Created_at_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Created_at_t(cur, arena);
}

[[nodiscard]] inline std::optional<Task_Updated_at_t>
parse_Task_Updated_at_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return Task_Updated_at_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<Task_Updated_at_t>
parse_Task_Updated_at_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Updated_at_t(cur, arena);
}

[[nodiscard]] inline std::optional<Task_Metadata_t>
parse_Task_Metadata_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return Task_Metadata_t{};
}

[[nodiscard]] inline std::optional<Task_Metadata_t> parse_Task_Metadata_t(std::string_view json,
                                                                          monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Metadata_t(cur, arena);
}

[[nodiscard]] inline std::optional<User> parse_User(katana::serde::json_cursor& cur,
                                                    monotonic_arena* arena) {
    if (!cur.try_object_start())
        return std::nullopt;

    User obj(arena);
    bool has_id = false;
    bool has_email = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "id") {
            has_id = true;
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.id = *v;
            } else {
                cur.skip_value();
            }
        } else if (*key == "email") {
            has_email = true;
            if (auto v = cur.string()) {
                obj.email = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else if (*key == "name") {
            if (auto v = cur.string()) {
                obj.name = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_id)
        return std::nullopt;
    if (!has_email)
        return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<User> parse_User(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_User(cur, arena);
}

[[nodiscard]] inline std::optional<User_Id_t> parse_User_Id_t(katana::serde::json_cursor& cur,
                                                              monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return User_Id_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<User_Id_t> parse_User_Id_t(std::string_view json,
                                                              monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_User_Id_t(cur, arena);
}

[[nodiscard]] inline std::optional<User_Email_t> parse_User_Email_t(katana::serde::json_cursor& cur,
                                                                    monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return User_Email_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<User_Email_t> parse_User_Email_t(std::string_view json,
                                                                    monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_User_Email_t(cur, arena);
}

[[nodiscard]] inline std::optional<User_Name_t> parse_User_Name_t(katana::serde::json_cursor& cur,
                                                                  monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return User_Name_t{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<User_Name_t> parse_User_Name_t(std::string_view json,
                                                                  monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_User_Name_t(cur, arena);
}

[[nodiscard]] inline std::optional<CreateTaskRequest>
parse_CreateTaskRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start())
        return std::nullopt;

    CreateTaskRequest obj(arena);
    bool has_title = false;
    bool has_priority = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        switch (key->size()) {
        case 4:
            if (*key == "tags") {
                if (cur.try_array_start()) {
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end())
                            break;
                        if (auto v = cur.string()) {
                            obj.tags.emplace_back(
                                v->begin(), v->end(), arena_allocator<char>(arena));
                        } else {
                            cur.skip_value();
                        }
                        cur.try_comma();
                    }
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 5:
            if (*key == "title") {
                has_title = true;
                if (auto v = cur.string()) {
                    obj.title = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 8:
            if (*key == "priority") {
                has_priority = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.priority = *v;
                } else {
                    cur.skip_value();
                }
            } else if (*key == "due_date") {
                if (auto v = cur.string()) {
                    obj.due_date =
                        arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 11:
            if (*key == "description") {
                if (auto v = cur.string()) {
                    obj.description =
                        arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else if (*key == "assignee_id") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.assignee_id = *v;
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!has_title)
        return std::nullopt;
    if (!has_priority)
        return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<CreateTaskRequest>
parse_CreateTaskRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateTaskRequest(cur, arena);
}

[[nodiscard]] inline std::optional<CreateTaskRequest_Title_t>
parse_CreateTaskRequest_Title_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return CreateTaskRequest_Title_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CreateTaskRequest_Title_t>
parse_CreateTaskRequest_Title_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateTaskRequest_Title_t(cur, arena);
}

[[nodiscard]] inline std::optional<CreateTaskRequest_Description_t>
parse_CreateTaskRequest_Description_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return CreateTaskRequest_Description_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CreateTaskRequest_Description_t>
parse_CreateTaskRequest_Description_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateTaskRequest_Description_t(cur, arena);
}

[[nodiscard]] inline std::optional<CreateTaskRequest_Priority_t>
parse_CreateTaskRequest_Priority_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return CreateTaskRequest_Priority_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CreateTaskRequest_Priority_t>
parse_CreateTaskRequest_Priority_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateTaskRequest_Priority_t(cur, arena);
}

[[nodiscard]] inline std::optional<CreateTaskRequest_Tags_t>
parse_CreateTaskRequest_Tags_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;
    CreateTaskRequest_Tags_t result{arena_allocator<CreateTaskRequest_Item_t>(arena)};
    size_t reserve_hint = 0;
    for (const char* p = cur.ptr; p < cur.end; ++p) {
        if (*p == ',')
            ++reserve_hint;
    }
    if (cur.ptr < cur.end && *cur.ptr != ']')
        ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto v = cur.string()) {
            result.emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<CreateTaskRequest_Tags_t>
parse_CreateTaskRequest_Tags_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;
    CreateTaskRequest_Tags_t result{arena_allocator<CreateTaskRequest_Item_t>(arena)};
    size_t reserve_hint = 0;
    for (char ch : json) {
        if (ch == ',')
            ++reserve_hint;
    }
    if (!json.empty() && json != "[]")
        ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto v = cur.string()) {
            result.emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<CreateTaskRequest_Item_t>
parse_CreateTaskRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return CreateTaskRequest_Item_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CreateTaskRequest_Item_t>
parse_CreateTaskRequest_Item_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateTaskRequest_Item_t(cur, arena);
}

[[nodiscard]] inline std::optional<CreateTaskRequest_Assignee_id_t>
parse_CreateTaskRequest_Assignee_id_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return CreateTaskRequest_Assignee_id_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CreateTaskRequest_Assignee_id_t>
parse_CreateTaskRequest_Assignee_id_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateTaskRequest_Assignee_id_t(cur, arena);
}

[[nodiscard]] inline std::optional<CreateTaskRequest_Due_date_t>
parse_CreateTaskRequest_Due_date_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return CreateTaskRequest_Due_date_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<CreateTaskRequest_Due_date_t>
parse_CreateTaskRequest_Due_date_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateTaskRequest_Due_date_t(cur, arena);
}

[[nodiscard]] inline std::optional<UpdateTaskRequest>
parse_UpdateTaskRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start())
        return std::nullopt;

    UpdateTaskRequest obj(arena);

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        switch (key->size()) {
        case 4:
            if (*key == "tags") {
                if (cur.try_array_start()) {
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end())
                            break;
                        if (auto v = cur.string()) {
                            obj.tags.emplace_back(
                                v->begin(), v->end(), arena_allocator<char>(arena));
                        } else {
                            cur.skip_value();
                        }
                        cur.try_comma();
                    }
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 5:
            if (*key == "title") {
                if (auto v = cur.string()) {
                    obj.title = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 6:
            if (*key == "status") {
                if (auto v = cur.string()) {
                    auto enum_val = UpdateTaskRequest_Status_t_enum_from_string(
                        std::string_view(v->begin(), v->end()));
                    if (enum_val)
                        obj.status = *enum_val;
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 8:
            if (*key == "priority") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.priority = *v;
                } else {
                    cur.skip_value();
                }
            } else if (*key == "due_date") {
                if (auto v = cur.string()) {
                    obj.due_date =
                        arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 11:
            if (*key == "description") {
                if (auto v = cur.string()) {
                    obj.description =
                        arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else if (*key == "assignee_id") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.assignee_id = *v;
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    return obj;
}

[[nodiscard]] inline std::optional<UpdateTaskRequest>
parse_UpdateTaskRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateTaskRequest(cur, arena);
}

[[nodiscard]] inline std::optional<UpdateTaskRequest_Title_t>
parse_UpdateTaskRequest_Title_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return UpdateTaskRequest_Title_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UpdateTaskRequest_Title_t>
parse_UpdateTaskRequest_Title_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateTaskRequest_Title_t(cur, arena);
}

[[nodiscard]] inline std::optional<UpdateTaskRequest_Description_t>
parse_UpdateTaskRequest_Description_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return UpdateTaskRequest_Description_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UpdateTaskRequest_Description_t>
parse_UpdateTaskRequest_Description_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateTaskRequest_Description_t(cur, arena);
}

[[nodiscard]] inline std::optional<UpdateTaskRequest_Status_t>
parse_UpdateTaskRequest_Status_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = cur.string()) {
        return UpdateTaskRequest_Status_t_enum_from_string(std::string_view(v->begin(), v->end()));
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UpdateTaskRequest_Status_t>
parse_UpdateTaskRequest_Status_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateTaskRequest_Status_t(cur, arena);
}

[[nodiscard]] inline std::optional<UpdateTaskRequest_Priority_t>
parse_UpdateTaskRequest_Priority_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return UpdateTaskRequest_Priority_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UpdateTaskRequest_Priority_t>
parse_UpdateTaskRequest_Priority_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateTaskRequest_Priority_t(cur, arena);
}

[[nodiscard]] inline std::optional<UpdateTaskRequest_Tags_t>
parse_UpdateTaskRequest_Tags_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;
    UpdateTaskRequest_Tags_t result{arena_allocator<UpdateTaskRequest_Item_t>(arena)};
    size_t reserve_hint = 0;
    for (const char* p = cur.ptr; p < cur.end; ++p) {
        if (*p == ',')
            ++reserve_hint;
    }
    if (cur.ptr < cur.end && *cur.ptr != ']')
        ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto v = cur.string()) {
            result.emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<UpdateTaskRequest_Tags_t>
parse_UpdateTaskRequest_Tags_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;
    UpdateTaskRequest_Tags_t result{arena_allocator<UpdateTaskRequest_Item_t>(arena)};
    size_t reserve_hint = 0;
    for (char ch : json) {
        if (ch == ',')
            ++reserve_hint;
    }
    if (!json.empty() && json != "[]")
        ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto v = cur.string()) {
            result.emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<UpdateTaskRequest_Item_t>
parse_UpdateTaskRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return UpdateTaskRequest_Item_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UpdateTaskRequest_Item_t>
parse_UpdateTaskRequest_Item_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateTaskRequest_Item_t(cur, arena);
}

[[nodiscard]] inline std::optional<UpdateTaskRequest_Assignee_id_t>
parse_UpdateTaskRequest_Assignee_id_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return UpdateTaskRequest_Assignee_id_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UpdateTaskRequest_Assignee_id_t>
parse_UpdateTaskRequest_Assignee_id_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateTaskRequest_Assignee_id_t(cur, arena);
}

[[nodiscard]] inline std::optional<UpdateTaskRequest_Due_date_t>
parse_UpdateTaskRequest_Due_date_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return UpdateTaskRequest_Due_date_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UpdateTaskRequest_Due_date_t>
parse_UpdateTaskRequest_Due_date_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateTaskRequest_Due_date_t(cur, arena);
}

[[nodiscard]] inline std::optional<BatchCreateRequest>
parse_BatchCreateRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start())
        return std::nullopt;

    BatchCreateRequest obj(arena);
    bool has_tasks = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "tasks") {
            has_tasks = true;
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end())
                        break;
                    if (auto nested = parse_CreateTaskRequest(cur, arena)) {
                        obj.tasks.push_back(std::move(*nested));
                    } else {
                        cur.skip_value();
                    }
                    cur.try_comma();
                }
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_tasks)
        return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<BatchCreateRequest>
parse_BatchCreateRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_BatchCreateRequest(cur, arena);
}

[[nodiscard]] inline std::optional<BatchCreateRequest_Tasks_t>
parse_BatchCreateRequest_Tasks_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;
    BatchCreateRequest_Tasks_t result{arena_allocator<CreateTaskRequest>(arena)};
    size_t reserve_hint = 0;
    for (const char* p = cur.ptr; p < cur.end; ++p) {
        if (*p == ',')
            ++reserve_hint;
    }
    if (cur.ptr < cur.end && *cur.ptr != ']')
        ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto parsed = parse_CreateTaskRequest(cur, arena))
            result.push_back(std::move(*parsed));
        else
            cur.skip_value();
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<BatchCreateRequest_Tasks_t>
parse_BatchCreateRequest_Tasks_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;
    BatchCreateRequest_Tasks_t result{arena_allocator<CreateTaskRequest>(arena)};
    size_t reserve_hint = 0;
    for (char ch : json) {
        if (ch == ',')
            ++reserve_hint;
    }
    if (!json.empty() && json != "[]")
        ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto parsed = parse_CreateTaskRequest(cur, arena))
            result.push_back(std::move(*parsed));
        else
            cur.skip_value();
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<BatchCreateRequest_Item_t>
parse_BatchCreateRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return BatchCreateRequest_Item_t{};
}

[[nodiscard]] inline std::optional<BatchCreateRequest_Item_t>
parse_BatchCreateRequest_Item_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_BatchCreateRequest_Item_t(cur, arena);
}

[[nodiscard]] inline std::optional<BatchCreateResponse>
parse_BatchCreateResponse(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start())
        return std::nullopt;

    BatchCreateResponse obj(arena);
    bool has_created = false;
    bool has_failed = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "created") {
            has_created = true;
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end())
                        break;
                    if (auto nested = parse_Task(cur, arena)) {
                        obj.created.push_back(std::move(*nested));
                    } else {
                        cur.skip_value();
                    }
                    cur.try_comma();
                }
            } else {
                cur.skip_value();
            }
        } else if (*key == "failed") {
            has_failed = true;
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end())
                        break;
                    if (auto nested = parse_BatchCreateResponse_Item_t_1(cur, arena)) {
                        obj.failed.push_back(std::move(*nested));
                    } else {
                        cur.skip_value();
                    }
                    cur.try_comma();
                }
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_created)
        return std::nullopt;
    if (!has_failed)
        return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<BatchCreateResponse>
parse_BatchCreateResponse(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_BatchCreateResponse(cur, arena);
}

[[nodiscard]] inline std::optional<BatchCreateResponse_Created_t>
parse_BatchCreateResponse_Created_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;
    BatchCreateResponse_Created_t result{arena_allocator<Task>(arena)};
    size_t reserve_hint = 0;
    for (const char* p = cur.ptr; p < cur.end; ++p) {
        if (*p == ',')
            ++reserve_hint;
    }
    if (cur.ptr < cur.end && *cur.ptr != ']')
        ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto parsed = parse_Task(cur, arena))
            result.push_back(std::move(*parsed));
        else
            cur.skip_value();
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<BatchCreateResponse_Created_t>
parse_BatchCreateResponse_Created_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;
    BatchCreateResponse_Created_t result{arena_allocator<Task>(arena)};
    size_t reserve_hint = 0;
    for (char ch : json) {
        if (ch == ',')
            ++reserve_hint;
    }
    if (!json.empty() && json != "[]")
        ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto parsed = parse_Task(cur, arena))
            result.push_back(std::move(*parsed));
        else
            cur.skip_value();
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<BatchCreateResponse_Item_t>
parse_BatchCreateResponse_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return BatchCreateResponse_Item_t{};
}

[[nodiscard]] inline std::optional<BatchCreateResponse_Item_t>
parse_BatchCreateResponse_Item_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_BatchCreateResponse_Item_t(cur, arena);
}

[[nodiscard]] inline std::optional<BatchCreateResponse_Failed_t>
parse_BatchCreateResponse_Failed_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;
    BatchCreateResponse_Failed_t result{arena_allocator<BatchCreateResponse_Item_t_1>(arena)};
    size_t reserve_hint = 0;
    for (const char* p = cur.ptr; p < cur.end; ++p) {
        if (*p == ',')
            ++reserve_hint;
    }
    if (cur.ptr < cur.end && *cur.ptr != ']')
        ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto parsed = parse_BatchCreateResponse_Item_t_1(cur, arena))
            result.push_back(std::move(*parsed));
        else
            cur.skip_value();
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<BatchCreateResponse_Failed_t>
parse_BatchCreateResponse_Failed_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;
    BatchCreateResponse_Failed_t result{arena_allocator<BatchCreateResponse_Item_t_1>(arena)};
    size_t reserve_hint = 0;
    for (char ch : json) {
        if (ch == ',')
            ++reserve_hint;
    }
    if (!json.empty() && json != "[]")
        ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto parsed = parse_BatchCreateResponse_Item_t_1(cur, arena))
            result.push_back(std::move(*parsed));
        else
            cur.skip_value();
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<BatchCreateResponse_Item_t_1>
parse_BatchCreateResponse_Item_t_1(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start())
        return std::nullopt;

    BatchCreateResponse_Item_t_1 obj(arena);
    bool has_index = false;
    bool has_error = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "index") {
            has_index = true;
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.index = *v;
            } else {
                cur.skip_value();
            }
        } else if (*key == "error") {
            has_error = true;
            if (auto v = cur.string()) {
                obj.error = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_index)
        return std::nullopt;
    if (!has_error)
        return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<BatchCreateResponse_Item_t_1>
parse_BatchCreateResponse_Item_t_1(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_BatchCreateResponse_Item_t_1(cur, arena);
}

[[nodiscard]] inline std::optional<BatchCreateResponse_Index_t>
parse_BatchCreateResponse_Index_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return BatchCreateResponse_Index_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<BatchCreateResponse_Index_t>
parse_BatchCreateResponse_Index_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_BatchCreateResponse_Index_t(cur, arena);
}

[[nodiscard]] inline std::optional<BatchCreateResponse_Error_t>
parse_BatchCreateResponse_Error_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return BatchCreateResponse_Error_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<BatchCreateResponse_Error_t>
parse_BatchCreateResponse_Error_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_BatchCreateResponse_Error_t(cur, arena);
}

[[nodiscard]] inline std::optional<SearchRequest>
parse_SearchRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start())
        return std::nullopt;

    SearchRequest obj(arena);

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        switch (key->size()) {
        case 4:
            if (*key == "tags") {
                if (cur.try_array_start()) {
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end())
                            break;
                        if (auto v = cur.string()) {
                            obj.tags.emplace_back(
                                v->begin(), v->end(), arena_allocator<char>(arena));
                        } else {
                            cur.skip_value();
                        }
                        cur.try_comma();
                    }
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 8:
            if (*key == "statuses") {
                if (cur.try_array_start()) {
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end())
                            break;
                        if (auto v = cur.string()) {
                            auto enum_val = SearchRequest_Item_t_enum_from_string(
                                std::string_view(v->begin(), v->end()));
                            if (enum_val)
                                obj.statuses.push_back(*enum_val);
                        } else {
                            cur.skip_value();
                        }
                        cur.try_comma();
                    }
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 12:
            if (*key == "min_priority") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.min_priority = *v;
                } else {
                    cur.skip_value();
                }
            } else if (*key == "max_priority") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.max_priority = *v;
                } else {
                    cur.skip_value();
                }
            } else if (*key == "has_assignee") {
                if (auto v = katana::serde::parse_bool(cur)) {
                    obj.has_assignee = *v;
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 13:
            if (*key == "created_after") {
                if (auto v = cur.string()) {
                    obj.created_after =
                        arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 14:
            if (*key == "title_contains") {
                if (auto v = cur.string()) {
                    obj.title_contains =
                        arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else if (*key == "created_before") {
                if (auto v = cur.string()) {
                    obj.created_before =
                        arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    return obj;
}

[[nodiscard]] inline std::optional<SearchRequest> parse_SearchRequest(std::string_view json,
                                                                      monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest(cur, arena);
}

[[nodiscard]] inline std::optional<SearchRequest_Title_contains_t>
parse_SearchRequest_Title_contains_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return SearchRequest_Title_contains_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<SearchRequest_Title_contains_t>
parse_SearchRequest_Title_contains_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_Title_contains_t(cur, arena);
}

[[nodiscard]] inline std::optional<SearchRequest_Statuses_t>
parse_SearchRequest_Statuses_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;
    SearchRequest_Statuses_t result{arena_allocator<SearchRequest_Item_t>(arena)};
    size_t reserve_hint = 0;
    for (const char* p = cur.ptr; p < cur.end; ++p) {
        if (*p == ',')
            ++reserve_hint;
    }
    if (cur.ptr < cur.end && *cur.ptr != ']')
        ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto v = cur.string()) {
            auto enum_val =
                SearchRequest_Item_t_enum_from_string(std::string_view(v->begin(), v->end()));
            if (enum_val)
                result.push_back(*enum_val);
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<SearchRequest_Statuses_t>
parse_SearchRequest_Statuses_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;
    SearchRequest_Statuses_t result{arena_allocator<SearchRequest_Item_t>(arena)};
    size_t reserve_hint = 0;
    for (char ch : json) {
        if (ch == ',')
            ++reserve_hint;
    }
    if (!json.empty() && json != "[]")
        ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto v = cur.string()) {
            auto enum_val =
                SearchRequest_Item_t_enum_from_string(std::string_view(v->begin(), v->end()));
            if (enum_val)
                result.push_back(*enum_val);
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<SearchRequest_Item_t>
parse_SearchRequest_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = cur.string()) {
        return SearchRequest_Item_t_enum_from_string(std::string_view(v->begin(), v->end()));
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<SearchRequest_Item_t>
parse_SearchRequest_Item_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_Item_t(cur, arena);
}

[[nodiscard]] inline std::optional<SearchRequest_Min_priority_t>
parse_SearchRequest_Min_priority_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return SearchRequest_Min_priority_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<SearchRequest_Min_priority_t>
parse_SearchRequest_Min_priority_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_Min_priority_t(cur, arena);
}

[[nodiscard]] inline std::optional<SearchRequest_Max_priority_t>
parse_SearchRequest_Max_priority_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return SearchRequest_Max_priority_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<SearchRequest_Max_priority_t>
parse_SearchRequest_Max_priority_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_Max_priority_t(cur, arena);
}

[[nodiscard]] inline std::optional<SearchRequest_Tags_t>
parse_SearchRequest_Tags_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;
    SearchRequest_Tags_t result{arena_allocator<SearchRequest_Item_t_1>(arena)};
    size_t reserve_hint = 0;
    for (const char* p = cur.ptr; p < cur.end; ++p) {
        if (*p == ',')
            ++reserve_hint;
    }
    if (cur.ptr < cur.end && *cur.ptr != ']')
        ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto v = cur.string()) {
            result.emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<SearchRequest_Tags_t>
parse_SearchRequest_Tags_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;
    SearchRequest_Tags_t result{arena_allocator<SearchRequest_Item_t_1>(arena)};
    size_t reserve_hint = 0;
    for (char ch : json) {
        if (ch == ',')
            ++reserve_hint;
    }
    if (!json.empty() && json != "[]")
        ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto v = cur.string()) {
            result.emplace_back(v->begin(), v->end(), arena_allocator<char>(arena));
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<SearchRequest_Item_t_1>
parse_SearchRequest_Item_t_1(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return SearchRequest_Item_t_1{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<SearchRequest_Item_t_1>
parse_SearchRequest_Item_t_1(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_Item_t_1(cur, arena);
}

[[nodiscard]] inline std::optional<SearchRequest_Created_after_t>
parse_SearchRequest_Created_after_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return SearchRequest_Created_after_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<SearchRequest_Created_after_t>
parse_SearchRequest_Created_after_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_Created_after_t(cur, arena);
}

[[nodiscard]] inline std::optional<SearchRequest_Created_before_t>
parse_SearchRequest_Created_before_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return SearchRequest_Created_before_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<SearchRequest_Created_before_t>
parse_SearchRequest_Created_before_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_Created_before_t(cur, arena);
}

[[nodiscard]] inline std::optional<SearchRequest_Has_assignee_t>
parse_SearchRequest_Has_assignee_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_bool(cur))
        return SearchRequest_Has_assignee_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<SearchRequest_Has_assignee_t>
parse_SearchRequest_Has_assignee_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_Has_assignee_t(cur, arena);
}

[[nodiscard]] inline std::optional<TaskList> parse_TaskList(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena) {
    if (!cur.try_object_start())
        return std::nullopt;

    TaskList obj(arena);
    bool has_tasks = false;
    bool has_total = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "tasks") {
            has_tasks = true;
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end())
                        break;
                    if (auto nested = parse_Task(cur, arena)) {
                        obj.tasks.push_back(std::move(*nested));
                    } else {
                        cur.skip_value();
                    }
                    cur.try_comma();
                }
            } else {
                cur.skip_value();
            }
        } else if (*key == "total") {
            has_total = true;
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.total = *v;
            } else {
                cur.skip_value();
            }
        } else if (*key == "has_more") {
            if (auto v = katana::serde::parse_bool(cur)) {
                obj.has_more = *v;
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_tasks)
        return std::nullopt;
    if (!has_total)
        return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<TaskList> parse_TaskList(std::string_view json,
                                                            monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TaskList(cur, arena);
}

[[nodiscard]] inline std::optional<TaskList_Tasks_t>
parse_TaskList_Tasks_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;
    TaskList_Tasks_t result{arena_allocator<Task>(arena)};
    size_t reserve_hint = 0;
    for (const char* p = cur.ptr; p < cur.end; ++p) {
        if (*p == ',')
            ++reserve_hint;
    }
    if (cur.ptr < cur.end && *cur.ptr != ']')
        ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto parsed = parse_Task(cur, arena))
            result.push_back(std::move(*parsed));
        else
            cur.skip_value();
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<TaskList_Tasks_t>
parse_TaskList_Tasks_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;
    TaskList_Tasks_t result{arena_allocator<Task>(arena)};
    size_t reserve_hint = 0;
    for (char ch : json) {
        if (ch == ',')
            ++reserve_hint;
    }
    if (!json.empty() && json != "[]")
        ++reserve_hint;
    result.reserve(reserve_hint);
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;
        if (auto parsed = parse_Task(cur, arena))
            result.push_back(std::move(*parsed));
        else
            cur.skip_value();
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<TaskList_Item_t>
parse_TaskList_Item_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return TaskList_Item_t{};
}

[[nodiscard]] inline std::optional<TaskList_Item_t> parse_TaskList_Item_t(std::string_view json,
                                                                          monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TaskList_Item_t(cur, arena);
}

[[nodiscard]] inline std::optional<TaskList_Total_t>
parse_TaskList_Total_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return TaskList_Total_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<TaskList_Total_t>
parse_TaskList_Total_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TaskList_Total_t(cur, arena);
}

[[nodiscard]] inline std::optional<TaskList_Has_more_t>
parse_TaskList_Has_more_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_bool(cur))
        return TaskList_Has_more_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<TaskList_Has_more_t>
parse_TaskList_Has_more_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TaskList_Has_more_t(cur, arena);
}

[[nodiscard]] inline std::optional<HealthResponse>
parse_HealthResponse(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start())
        return std::nullopt;

    HealthResponse obj(arena);
    bool has_status = false;
    bool has_timestamp = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        switch (key->size()) {
        case 6:
            if (*key == "status") {
                has_status = true;
                if (auto v = cur.string()) {
                    auto enum_val = HealthResponse_Status_t_enum_from_string(
                        std::string_view(v->begin(), v->end()));
                    if (enum_val)
                        obj.status = *enum_val;
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 9:
            if (*key == "timestamp") {
                has_timestamp = true;
                if (auto v = cur.string()) {
                    obj.timestamp =
                        arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 14:
            if (*key == "uptime_seconds") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.uptime_seconds = *v;
                } else {
                    cur.skip_value();
                }
            } else if (*key == "total_requests") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.total_requests = *v;
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!has_status)
        return std::nullopt;
    if (!has_timestamp)
        return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<HealthResponse> parse_HealthResponse(std::string_view json,
                                                                        monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_HealthResponse(cur, arena);
}

[[nodiscard]] inline std::optional<HealthResponse_Status_t>
parse_HealthResponse_Status_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = cur.string()) {
        return HealthResponse_Status_t_enum_from_string(std::string_view(v->begin(), v->end()));
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<HealthResponse_Status_t>
parse_HealthResponse_Status_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_HealthResponse_Status_t(cur, arena);
}

[[nodiscard]] inline std::optional<HealthResponse_Timestamp_t>
parse_HealthResponse_Timestamp_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return HealthResponse_Timestamp_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<HealthResponse_Timestamp_t>
parse_HealthResponse_Timestamp_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_HealthResponse_Timestamp_t(cur, arena);
}

[[nodiscard]] inline std::optional<HealthResponse_Uptime_seconds_t>
parse_HealthResponse_Uptime_seconds_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return HealthResponse_Uptime_seconds_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<HealthResponse_Uptime_seconds_t>
parse_HealthResponse_Uptime_seconds_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_HealthResponse_Uptime_seconds_t(cur, arena);
}

[[nodiscard]] inline std::optional<HealthResponse_Total_requests_t>
parse_HealthResponse_Total_requests_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return HealthResponse_Total_requests_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<HealthResponse_Total_requests_t>
parse_HealthResponse_Total_requests_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_HealthResponse_Total_requests_t(cur, arena);
}

[[nodiscard]] inline std::optional<ProblemDetails>
parse_ProblemDetails(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start())
        return std::nullopt;

    ProblemDetails obj(arena);
    bool has_type = false;
    bool has_title = false;
    bool has_status = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        switch (key->size()) {
        case 4:
            if (*key == "type") {
                has_type = true;
                if (auto v = cur.string()) {
                    obj.type = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 5:
            if (*key == "title") {
                has_title = true;
                if (auto v = cur.string()) {
                    obj.title = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 6:
            if (*key == "status") {
                has_status = true;
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.status = *v;
                } else {
                    cur.skip_value();
                }
            } else if (*key == "detail") {
                if (auto v = cur.string()) {
                    obj.detail = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        case 8:
            if (*key == "instance") {
                if (auto v = cur.string()) {
                    obj.instance =
                        arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
                } else {
                    cur.skip_value();
                }
            } else {
                cur.skip_value();
            }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!has_type)
        return std::nullopt;
    if (!has_title)
        return std::nullopt;
    if (!has_status)
        return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<ProblemDetails> parse_ProblemDetails(std::string_view json,
                                                                        monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ProblemDetails(cur, arena);
}

[[nodiscard]] inline std::optional<ProblemDetails_Type_t>
parse_ProblemDetails_Type_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return ProblemDetails_Type_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<ProblemDetails_Type_t>
parse_ProblemDetails_Type_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ProblemDetails_Type_t(cur, arena);
}

[[nodiscard]] inline std::optional<ProblemDetails_Title_t>
parse_ProblemDetails_Title_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return ProblemDetails_Title_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<ProblemDetails_Title_t>
parse_ProblemDetails_Title_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ProblemDetails_Title_t(cur, arena);
}

[[nodiscard]] inline std::optional<ProblemDetails_Status_t>
parse_ProblemDetails_Status_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return ProblemDetails_Status_t{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<ProblemDetails_Status_t>
parse_ProblemDetails_Status_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ProblemDetails_Status_t(cur, arena);
}

[[nodiscard]] inline std::optional<ProblemDetails_Detail_t>
parse_ProblemDetails_Detail_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return ProblemDetails_Detail_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<ProblemDetails_Detail_t>
parse_ProblemDetails_Detail_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ProblemDetails_Detail_t(cur, arena);
}

[[nodiscard]] inline std::optional<ProblemDetails_Instance_t>
parse_ProblemDetails_Instance_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (auto v = cur.string()) {
        return ProblemDetails_Instance_t{
            arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<ProblemDetails_Instance_t>
parse_ProblemDetails_Instance_t(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ProblemDetails_Instance_t(cur, arena);
}

[[nodiscard]] inline std::optional<listTasks_param_status>
parse_listTasks_param_status(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = cur.string()) {
        return listTasks_param_status_enum_from_string(std::string_view(v->begin(), v->end()));
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<listTasks_param_status>
parse_listTasks_param_status(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_listTasks_param_status(cur, arena);
}

[[nodiscard]] inline std::optional<listTasks_param_priority>
parse_listTasks_param_priority(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return listTasks_param_priority{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<listTasks_param_priority>
parse_listTasks_param_priority(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_listTasks_param_priority(cur, arena);
}

[[nodiscard]] inline std::optional<listTasks_param_limit>
parse_listTasks_param_limit(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return listTasks_param_limit{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<listTasks_param_limit>
parse_listTasks_param_limit(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_listTasks_param_limit(cur, arena);
}

[[nodiscard]] inline std::optional<listTasks_param_offset>
parse_listTasks_param_offset(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return listTasks_param_offset{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<listTasks_param_offset>
parse_listTasks_param_offset(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_listTasks_param_offset(cur, arena);
}

[[nodiscard]] inline std::optional<schema> parse_schema(katana::serde::json_cursor& cur,
                                                        monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema{};
}

[[nodiscard]] inline std::optional<schema> parse_schema(std::string_view json,
                                                        monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema(cur, arena);
}

[[nodiscard]] inline std::optional<schema_1> parse_schema_1(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_1{};
}

[[nodiscard]] inline std::optional<schema_1> parse_schema_1(std::string_view json,
                                                            monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_1(cur, arena);
}

[[nodiscard]] inline std::optional<schema_2> parse_schema_2(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_2{};
}

[[nodiscard]] inline std::optional<schema_2> parse_schema_2(std::string_view json,
                                                            monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_2(cur, arena);
}

[[nodiscard]] inline std::optional<schema_3> parse_schema_3(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_3{};
}

[[nodiscard]] inline std::optional<schema_3> parse_schema_3(std::string_view json,
                                                            monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_3(cur, arena);
}

[[nodiscard]] inline std::optional<getTask_param_id>
parse_getTask_param_id(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return getTask_param_id{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<getTask_param_id>
parse_getTask_param_id(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_getTask_param_id(cur, arena);
}

[[nodiscard]] inline std::optional<schema_4> parse_schema_4(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_4{};
}

[[nodiscard]] inline std::optional<schema_4> parse_schema_4(std::string_view json,
                                                            monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_4(cur, arena);
}

[[nodiscard]] inline std::optional<schema_5> parse_schema_5(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_5{};
}

[[nodiscard]] inline std::optional<schema_5> parse_schema_5(std::string_view json,
                                                            monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_5(cur, arena);
}

[[nodiscard]] inline std::optional<updateTask_param_id>
parse_updateTask_param_id(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return updateTask_param_id{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<updateTask_param_id>
parse_updateTask_param_id(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_updateTask_param_id(cur, arena);
}

[[nodiscard]] inline std::optional<schema_6> parse_schema_6(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_6{};
}

[[nodiscard]] inline std::optional<schema_6> parse_schema_6(std::string_view json,
                                                            monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_6(cur, arena);
}

[[nodiscard]] inline std::optional<schema_7> parse_schema_7(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_7{};
}

[[nodiscard]] inline std::optional<schema_7> parse_schema_7(std::string_view json,
                                                            monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_7(cur, arena);
}

[[nodiscard]] inline std::optional<schema_8> parse_schema_8(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_8{};
}

[[nodiscard]] inline std::optional<schema_8> parse_schema_8(std::string_view json,
                                                            monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_8(cur, arena);
}

[[nodiscard]] inline std::optional<deleteTask_param_id>
parse_deleteTask_param_id(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = katana::serde::parse_int64(cur))
        return deleteTask_param_id{*v};
    return std::nullopt;
}

[[nodiscard]] inline std::optional<deleteTask_param_id>
parse_deleteTask_param_id(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_deleteTask_param_id(cur, arena);
}

[[nodiscard]] inline std::optional<schema_9> parse_schema_9(katana::serde::json_cursor& cur,
                                                            monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_9{};
}

[[nodiscard]] inline std::optional<schema_9> parse_schema_9(std::string_view json,
                                                            monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_9(cur, arena);
}

[[nodiscard]] inline std::optional<schema_10> parse_schema_10(katana::serde::json_cursor& cur,
                                                              monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_10{};
}

[[nodiscard]] inline std::optional<schema_10> parse_schema_10(std::string_view json,
                                                              monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_10(cur, arena);
}

[[nodiscard]] inline std::optional<schema_11> parse_schema_11(katana::serde::json_cursor& cur,
                                                              monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_11{};
}

[[nodiscard]] inline std::optional<schema_11> parse_schema_11(std::string_view json,
                                                              monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_11(cur, arena);
}

[[nodiscard]] inline std::optional<schema_12> parse_schema_12(katana::serde::json_cursor& cur,
                                                              monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_12{};
}

[[nodiscard]] inline std::optional<schema_12> parse_schema_12(std::string_view json,
                                                              monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_12(cur, arena);
}

[[nodiscard]] inline std::optional<schema_13> parse_schema_13(katana::serde::json_cursor& cur,
                                                              monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_13{};
}

[[nodiscard]] inline std::optional<schema_13> parse_schema_13(std::string_view json,
                                                              monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_13(cur, arena);
}

[[nodiscard]] inline std::optional<schema_14> parse_schema_14(katana::serde::json_cursor& cur,
                                                              monotonic_arena* arena) {
    (void)arena;
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return schema_14{};
}

[[nodiscard]] inline std::optional<schema_14> parse_schema_14(std::string_view json,
                                                              monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_14(cur, arena);
}

// ============================================================
// JSON Serialize Functions
// ============================================================

inline void serialize_Task_into(const Task& obj, std::string& json) {
    json.push_back('{');
    json.append("\"id\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.id);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"title\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.title, json);
    json.push_back('"');
    json.append(",\"description\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.description, json);
    json.push_back('"');
    json.append(",\"status\":");
    json.push_back('"');
    json.append(to_string(obj.status));
    json.push_back('"');
    json.append(",\"priority\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.priority);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"tags\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.tags.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        json.push_back('"');
        katana::serde::escape_json_string_into(obj.tags[i], json);
        json.push_back('"');
    }
    json.push_back(']');
    json.append(",\"assignee\":");
    serialize_User_into(obj.assignee, json);
    json.append(",\"due_date\":");
    if (obj.due_date) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.due_date, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    json.append(",\"created_at\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.created_at, json);
    json.push_back('"');
    json.append(",\"updated_at\":");
    if (obj.updated_at) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.updated_at, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    json.append(",\"metadata\":");
    serialize_Task_Metadata_t_into(obj.metadata, json);
    json.push_back('}');
}

inline std::string serialize_Task(const Task& obj) {
    std::string json;
    size_t reserve_estimate = 678;
    reserve_estimate += obj.title.size();
    reserve_estimate += obj.description.size();
    reserve_estimate += obj.tags.size() * 32;
    reserve_estimate += 109;
    if (obj.due_date)
        reserve_estimate += obj.due_date->size();
    reserve_estimate += obj.created_at.size();
    if (obj.updated_at)
        reserve_estimate += obj.updated_at->size();
    reserve_estimate += 2;
    json.reserve(reserve_estimate);
    serialize_Task_into(obj, json);
    return json;
}

inline void serialize_Task_Id_t_into(const Task_Id_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_Task_Id_t(const Task_Id_t& obj) {
    std::string json;
    serialize_Task_Id_t_into(obj, json);
    return json;
}

inline void serialize_Task_Title_t_into(const Task_Title_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_Task_Title_t(const Task_Title_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_Task_Title_t_into(obj, json);
    return json;
}

inline void serialize_Task_Description_t_into(const Task_Description_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_Task_Description_t(const Task_Description_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_Task_Description_t_into(obj, json);
    return json;
}

inline void serialize_Task_Status_t_into(const Task_Status_t& obj, std::string& json) {
    auto str = to_string(obj);
    json.push_back('"');
    json.append(str);
    json.push_back('"');
}

inline std::string serialize_Task_Status_t(const Task_Status_t& obj) {
    std::string json;
    auto str = to_string(obj);
    json.reserve(str.size() + 2);
    serialize_Task_Status_t_into(obj, json);
    return json;
}

inline void serialize_Task_Priority_t_into(const Task_Priority_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_Task_Priority_t(const Task_Priority_t& obj) {
    std::string json;
    serialize_Task_Priority_t_into(obj, json);
    return json;
}

inline void serialize_Task_Tags_t_into(const Task_Tags_t& obj, std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Tags_t(const Task_Tags_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_Task_Tags_t_into(obj, json);
    return json;
}

inline void serialize_Task_Item_t_into(const Task_Item_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_Task_Item_t(const Task_Item_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_Task_Item_t_into(obj, json);
    return json;
}

inline void serialize_Task_Assignee_t_into(const Task_Assignee_t& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_Task_Assignee_t(const Task_Assignee_t& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_Task_Due_date_t_into(const Task_Due_date_t& obj, std::string& json) {
    if (!obj) {
        json.append("null");
        return;
    }
    json.push_back('"');
    katana::serde::escape_json_string_into(*obj, json);
    json.push_back('"');
}

inline std::string serialize_Task_Due_date_t(const Task_Due_date_t& obj) {
    if (!obj)
        return std::string("null");
    std::string json;
    json.reserve(obj->size() + 16);
    serialize_Task_Due_date_t_into(obj, json);
    return json;
}

inline void serialize_Task_Created_at_t_into(const Task_Created_at_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_Task_Created_at_t(const Task_Created_at_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_Task_Created_at_t_into(obj, json);
    return json;
}

inline void serialize_Task_Updated_at_t_into(const Task_Updated_at_t& obj, std::string& json) {
    if (!obj) {
        json.append("null");
        return;
    }
    json.push_back('"');
    katana::serde::escape_json_string_into(*obj, json);
    json.push_back('"');
}

inline std::string serialize_Task_Updated_at_t(const Task_Updated_at_t& obj) {
    if (!obj)
        return std::string("null");
    std::string json;
    json.reserve(obj->size() + 16);
    serialize_Task_Updated_at_t_into(obj, json);
    return json;
}

inline void serialize_Task_Metadata_t_into(const Task_Metadata_t& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_Task_Metadata_t(const Task_Metadata_t& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_User_into(const User& obj, std::string& json) {
    json.push_back('{');
    json.append("\"id\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.id);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"email\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.email, json);
    json.push_back('"');
    json.append(",\"name\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.name, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_User(const User& obj) {
    std::string json;
    size_t reserve_estimate = 109;
    reserve_estimate += obj.email.size();
    reserve_estimate += obj.name.size();
    json.reserve(reserve_estimate);
    serialize_User_into(obj, json);
    return json;
}

inline void serialize_User_Id_t_into(const User_Id_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_User_Id_t(const User_Id_t& obj) {
    std::string json;
    serialize_User_Id_t_into(obj, json);
    return json;
}

inline void serialize_User_Email_t_into(const User_Email_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_User_Email_t(const User_Email_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_User_Email_t_into(obj, json);
    return json;
}

inline void serialize_User_Name_t_into(const User_Name_t& obj, std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_User_Name_t(const User_Name_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_User_Name_t_into(obj, json);
    return json;
}

inline void serialize_CreateTaskRequest_into(const CreateTaskRequest& obj, std::string& json) {
    json.push_back('{');
    json.append("\"title\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.title, json);
    json.push_back('"');
    json.append(",\"description\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.description, json);
    json.push_back('"');
    json.append(",\"priority\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.priority);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"tags\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.tags.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        json.push_back('"');
        katana::serde::escape_json_string_into(obj.tags[i], json);
        json.push_back('"');
    }
    json.push_back(']');
    json.append(",\"assignee_id\":");
    {
        if (!obj.assignee_id) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.assignee_id);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    json.append(",\"due_date\":");
    if (obj.due_date) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.due_date, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    json.push_back('}');
}

inline std::string serialize_CreateTaskRequest(const CreateTaskRequest& obj) {
    std::string json;
    size_t reserve_estimate = 273;
    reserve_estimate += obj.title.size();
    reserve_estimate += obj.description.size();
    reserve_estimate += obj.tags.size() * 32;
    if (obj.due_date)
        reserve_estimate += obj.due_date->size();
    json.reserve(reserve_estimate);
    serialize_CreateTaskRequest_into(obj, json);
    return json;
}

inline void serialize_CreateTaskRequest_Title_t_into(const CreateTaskRequest_Title_t& obj,
                                                     std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_CreateTaskRequest_Title_t(const CreateTaskRequest_Title_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_CreateTaskRequest_Title_t_into(obj, json);
    return json;
}

inline void
serialize_CreateTaskRequest_Description_t_into(const CreateTaskRequest_Description_t& obj,
                                               std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string
serialize_CreateTaskRequest_Description_t(const CreateTaskRequest_Description_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_CreateTaskRequest_Description_t_into(obj, json);
    return json;
}

inline void serialize_CreateTaskRequest_Priority_t_into(const CreateTaskRequest_Priority_t& obj,
                                                        std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_CreateTaskRequest_Priority_t(const CreateTaskRequest_Priority_t& obj) {
    std::string json;
    serialize_CreateTaskRequest_Priority_t_into(obj, json);
    return json;
}

inline void serialize_CreateTaskRequest_Tags_t_into(const CreateTaskRequest_Tags_t& obj,
                                                    std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateTaskRequest_Tags_t(const CreateTaskRequest_Tags_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_CreateTaskRequest_Tags_t_into(obj, json);
    return json;
}

inline void serialize_CreateTaskRequest_Item_t_into(const CreateTaskRequest_Item_t& obj,
                                                    std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_CreateTaskRequest_Item_t(const CreateTaskRequest_Item_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_CreateTaskRequest_Item_t_into(obj, json);
    return json;
}

inline void
serialize_CreateTaskRequest_Assignee_id_t_into(const CreateTaskRequest_Assignee_id_t& obj,
                                               std::string& json) {
    if (!obj) {
        json.append("null");
        return;
    }
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string
serialize_CreateTaskRequest_Assignee_id_t(const CreateTaskRequest_Assignee_id_t& obj) {
    if (!obj)
        return std::string("null");
    std::string json;
    serialize_CreateTaskRequest_Assignee_id_t_into(obj, json);
    return json;
}

inline void serialize_CreateTaskRequest_Due_date_t_into(const CreateTaskRequest_Due_date_t& obj,
                                                        std::string& json) {
    if (!obj) {
        json.append("null");
        return;
    }
    json.push_back('"');
    katana::serde::escape_json_string_into(*obj, json);
    json.push_back('"');
}

inline std::string serialize_CreateTaskRequest_Due_date_t(const CreateTaskRequest_Due_date_t& obj) {
    if (!obj)
        return std::string("null");
    std::string json;
    json.reserve(obj->size() + 16);
    serialize_CreateTaskRequest_Due_date_t_into(obj, json);
    return json;
}

inline void serialize_UpdateTaskRequest_into(const UpdateTaskRequest& obj, std::string& json) {
    json.push_back('{');
    json.append("\"title\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.title, json);
    json.push_back('"');
    json.append(",\"description\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.description, json);
    json.push_back('"');
    json.append(",\"status\":");
    json.push_back('"');
    json.append(to_string(obj.status));
    json.push_back('"');
    json.append(",\"priority\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.priority);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"tags\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.tags.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        json.push_back('"');
        katana::serde::escape_json_string_into(obj.tags[i], json);
        json.push_back('"');
    }
    json.push_back(']');
    json.append(",\"assignee_id\":");
    {
        if (!obj.assignee_id) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.assignee_id);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    json.append(",\"due_date\":");
    if (obj.due_date) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.due_date, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    json.push_back('}');
}

inline std::string serialize_UpdateTaskRequest(const UpdateTaskRequest& obj) {
    std::string json;
    size_t reserve_estimate = 315;
    reserve_estimate += obj.title.size();
    reserve_estimate += obj.description.size();
    reserve_estimate += obj.tags.size() * 32;
    if (obj.due_date)
        reserve_estimate += obj.due_date->size();
    json.reserve(reserve_estimate);
    serialize_UpdateTaskRequest_into(obj, json);
    return json;
}

inline void serialize_UpdateTaskRequest_Title_t_into(const UpdateTaskRequest_Title_t& obj,
                                                     std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_UpdateTaskRequest_Title_t(const UpdateTaskRequest_Title_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_UpdateTaskRequest_Title_t_into(obj, json);
    return json;
}

inline void
serialize_UpdateTaskRequest_Description_t_into(const UpdateTaskRequest_Description_t& obj,
                                               std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string
serialize_UpdateTaskRequest_Description_t(const UpdateTaskRequest_Description_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_UpdateTaskRequest_Description_t_into(obj, json);
    return json;
}

inline void serialize_UpdateTaskRequest_Status_t_into(const UpdateTaskRequest_Status_t& obj,
                                                      std::string& json) {
    auto str = to_string(obj);
    json.push_back('"');
    json.append(str);
    json.push_back('"');
}

inline std::string serialize_UpdateTaskRequest_Status_t(const UpdateTaskRequest_Status_t& obj) {
    std::string json;
    auto str = to_string(obj);
    json.reserve(str.size() + 2);
    serialize_UpdateTaskRequest_Status_t_into(obj, json);
    return json;
}

inline void serialize_UpdateTaskRequest_Priority_t_into(const UpdateTaskRequest_Priority_t& obj,
                                                        std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_UpdateTaskRequest_Priority_t(const UpdateTaskRequest_Priority_t& obj) {
    std::string json;
    serialize_UpdateTaskRequest_Priority_t_into(obj, json);
    return json;
}

inline void serialize_UpdateTaskRequest_Tags_t_into(const UpdateTaskRequest_Tags_t& obj,
                                                    std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateTaskRequest_Tags_t(const UpdateTaskRequest_Tags_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_UpdateTaskRequest_Tags_t_into(obj, json);
    return json;
}

inline void serialize_UpdateTaskRequest_Item_t_into(const UpdateTaskRequest_Item_t& obj,
                                                    std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_UpdateTaskRequest_Item_t(const UpdateTaskRequest_Item_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_UpdateTaskRequest_Item_t_into(obj, json);
    return json;
}

inline void
serialize_UpdateTaskRequest_Assignee_id_t_into(const UpdateTaskRequest_Assignee_id_t& obj,
                                               std::string& json) {
    if (!obj) {
        json.append("null");
        return;
    }
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string
serialize_UpdateTaskRequest_Assignee_id_t(const UpdateTaskRequest_Assignee_id_t& obj) {
    if (!obj)
        return std::string("null");
    std::string json;
    serialize_UpdateTaskRequest_Assignee_id_t_into(obj, json);
    return json;
}

inline void serialize_UpdateTaskRequest_Due_date_t_into(const UpdateTaskRequest_Due_date_t& obj,
                                                        std::string& json) {
    if (!obj) {
        json.append("null");
        return;
    }
    json.push_back('"');
    katana::serde::escape_json_string_into(*obj, json);
    json.push_back('"');
}

inline std::string serialize_UpdateTaskRequest_Due_date_t(const UpdateTaskRequest_Due_date_t& obj) {
    if (!obj)
        return std::string("null");
    std::string json;
    json.reserve(obj->size() + 16);
    serialize_UpdateTaskRequest_Due_date_t_into(obj, json);
    return json;
}

inline void serialize_BatchCreateRequest_into(const BatchCreateRequest& obj, std::string& json) {
    json.push_back('{');
    json.append("\"tasks\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.tasks.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_into(obj.tasks[i], json);
    }
    json.push_back(']');
    json.push_back('}');
}

inline std::string serialize_BatchCreateRequest(const BatchCreateRequest& obj) {
    std::string json;
    size_t reserve_estimate = 75;
    reserve_estimate += obj.tasks.size() * 273;
    json.reserve(reserve_estimate);
    serialize_BatchCreateRequest_into(obj, json);
    return json;
}

inline void serialize_BatchCreateRequest_Tasks_t_into(const BatchCreateRequest_Tasks_t& obj,
                                                      std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_BatchCreateRequest_Tasks_t(const BatchCreateRequest_Tasks_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_BatchCreateRequest_Tasks_t_into(obj, json);
    return json;
}

inline void serialize_BatchCreateRequest_Item_t_into(const BatchCreateRequest_Item_t& obj,
                                                     std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_BatchCreateRequest_Item_t(const BatchCreateRequest_Item_t& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_BatchCreateResponse_into(const BatchCreateResponse& obj, std::string& json) {
    json.push_back('{');
    json.append("\"created\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.created.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_into(obj.created[i], json);
    }
    json.push_back(']');
    json.append(",\"failed\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.failed.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateResponse_Item_t_1_into(obj.failed[i], json);
    }
    json.push_back(']');
    json.push_back('}');
}

inline std::string serialize_BatchCreateResponse(const BatchCreateResponse& obj) {
    std::string json;
    size_t reserve_estimate = 151;
    reserve_estimate += obj.created.size() * 678;
    reserve_estimate += obj.failed.size() * 72;
    json.reserve(reserve_estimate);
    serialize_BatchCreateResponse_into(obj, json);
    return json;
}

inline void serialize_BatchCreateResponse_Created_t_into(const BatchCreateResponse_Created_t& obj,
                                                         std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_BatchCreateResponse_Created_t(const BatchCreateResponse_Created_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_BatchCreateResponse_Created_t_into(obj, json);
    return json;
}

inline void serialize_BatchCreateResponse_Item_t_into(const BatchCreateResponse_Item_t& obj,
                                                      std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_BatchCreateResponse_Item_t(const BatchCreateResponse_Item_t& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_BatchCreateResponse_Failed_t_into(const BatchCreateResponse_Failed_t& obj,
                                                        std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateResponse_Item_t_1_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_BatchCreateResponse_Failed_t(const BatchCreateResponse_Failed_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_BatchCreateResponse_Failed_t_into(obj, json);
    return json;
}

inline void serialize_BatchCreateResponse_Item_t_1_into(const BatchCreateResponse_Item_t_1& obj,
                                                        std::string& json) {
    json.push_back('{');
    json.append("\"index\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.index);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"error\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.error, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_BatchCreateResponse_Item_t_1(const BatchCreateResponse_Item_t_1& obj) {
    std::string json;
    size_t reserve_estimate = 72;
    reserve_estimate += obj.error.size();
    json.reserve(reserve_estimate);
    serialize_BatchCreateResponse_Item_t_1_into(obj, json);
    return json;
}

inline void serialize_BatchCreateResponse_Index_t_into(const BatchCreateResponse_Index_t& obj,
                                                       std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_BatchCreateResponse_Index_t(const BatchCreateResponse_Index_t& obj) {
    std::string json;
    serialize_BatchCreateResponse_Index_t_into(obj, json);
    return json;
}

inline void serialize_BatchCreateResponse_Error_t_into(const BatchCreateResponse_Error_t& obj,
                                                       std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_BatchCreateResponse_Error_t(const BatchCreateResponse_Error_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_BatchCreateResponse_Error_t_into(obj, json);
    return json;
}

inline void serialize_SearchRequest_into(const SearchRequest& obj, std::string& json) {
    json.push_back('{');
    json.append("\"title_contains\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.title_contains, json);
    json.push_back('"');
    json.append(",\"statuses\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.statuses.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        json.push_back('"');
        json.append(to_string(obj.statuses[i]));
        json.push_back('"');
    }
    json.push_back(']');
    json.append(",\"min_priority\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.min_priority);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"max_priority\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.max_priority);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"tags\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.tags.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        json.push_back('"');
        katana::serde::escape_json_string_into(obj.tags[i], json);
        json.push_back('"');
    }
    json.push_back(']');
    json.append(",\"created_after\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.created_after, json);
    json.push_back('"');
    json.append(",\"created_before\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.created_before, json);
    json.push_back('"');
    json.append(",\"has_assignee\":");
    json.append(obj.has_assignee ? "true" : "false");
    json.push_back('}');
}

inline std::string serialize_SearchRequest(const SearchRequest& obj) {
    std::string json;
    size_t reserve_estimate = 392;
    reserve_estimate += obj.title_contains.size();
    reserve_estimate += obj.statuses.size() * 32;
    reserve_estimate += obj.tags.size() * 32;
    reserve_estimate += obj.created_after.size();
    reserve_estimate += obj.created_before.size();
    json.reserve(reserve_estimate);
    serialize_SearchRequest_into(obj, json);
    return json;
}

inline void serialize_SearchRequest_Title_contains_t_into(const SearchRequest_Title_contains_t& obj,
                                                          std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string
serialize_SearchRequest_Title_contains_t(const SearchRequest_Title_contains_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_SearchRequest_Title_contains_t_into(obj, json);
    return json;
}

inline void serialize_SearchRequest_Statuses_t_into(const SearchRequest_Statuses_t& obj,
                                                    std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SearchRequest_Statuses_t(const SearchRequest_Statuses_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_SearchRequest_Statuses_t_into(obj, json);
    return json;
}

inline void serialize_SearchRequest_Item_t_into(const SearchRequest_Item_t& obj,
                                                std::string& json) {
    auto str = to_string(obj);
    json.push_back('"');
    json.append(str);
    json.push_back('"');
}

inline std::string serialize_SearchRequest_Item_t(const SearchRequest_Item_t& obj) {
    std::string json;
    auto str = to_string(obj);
    json.reserve(str.size() + 2);
    serialize_SearchRequest_Item_t_into(obj, json);
    return json;
}

inline void serialize_SearchRequest_Min_priority_t_into(const SearchRequest_Min_priority_t& obj,
                                                        std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_SearchRequest_Min_priority_t(const SearchRequest_Min_priority_t& obj) {
    std::string json;
    serialize_SearchRequest_Min_priority_t_into(obj, json);
    return json;
}

inline void serialize_SearchRequest_Max_priority_t_into(const SearchRequest_Max_priority_t& obj,
                                                        std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_SearchRequest_Max_priority_t(const SearchRequest_Max_priority_t& obj) {
    std::string json;
    serialize_SearchRequest_Max_priority_t_into(obj, json);
    return json;
}

inline void serialize_SearchRequest_Tags_t_into(const SearchRequest_Tags_t& obj,
                                                std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Item_t_1_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SearchRequest_Tags_t(const SearchRequest_Tags_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_SearchRequest_Tags_t_into(obj, json);
    return json;
}

inline void serialize_SearchRequest_Item_t_1_into(const SearchRequest_Item_t_1& obj,
                                                  std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_SearchRequest_Item_t_1(const SearchRequest_Item_t_1& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_SearchRequest_Item_t_1_into(obj, json);
    return json;
}

inline void serialize_SearchRequest_Created_after_t_into(const SearchRequest_Created_after_t& obj,
                                                         std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string
serialize_SearchRequest_Created_after_t(const SearchRequest_Created_after_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_SearchRequest_Created_after_t_into(obj, json);
    return json;
}

inline void serialize_SearchRequest_Created_before_t_into(const SearchRequest_Created_before_t& obj,
                                                          std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string
serialize_SearchRequest_Created_before_t(const SearchRequest_Created_before_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_SearchRequest_Created_before_t_into(obj, json);
    return json;
}

inline void serialize_SearchRequest_Has_assignee_t_into(const SearchRequest_Has_assignee_t& obj,
                                                        std::string& json) {
    json.append(obj ? "true" : "false");
}

inline std::string serialize_SearchRequest_Has_assignee_t(const SearchRequest_Has_assignee_t& obj) {
    return obj ? "true" : "false";
}

inline void serialize_TaskList_into(const TaskList& obj, std::string& json) {
    json.push_back('{');
    json.append("\"tasks\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.tasks.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_into(obj.tasks[i], json);
    }
    json.push_back(']');
    json.append(",\"total\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.total);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"has_more\":");
    json.append(obj.has_more ? "true" : "false");
    json.push_back('}');
}

inline std::string serialize_TaskList(const TaskList& obj) {
    std::string json;
    size_t reserve_estimate = 121;
    reserve_estimate += obj.tasks.size() * 678;
    json.reserve(reserve_estimate);
    serialize_TaskList_into(obj, json);
    return json;
}

inline void serialize_TaskList_Tasks_t_into(const TaskList_Tasks_t& obj, std::string& json) {
    const auto& arr = obj;
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TaskList_Tasks_t(const TaskList_Tasks_t& obj) {
    std::string json;
    json.reserve(obj.size() * 16 + 2);
    serialize_TaskList_Tasks_t_into(obj, json);
    return json;
}

inline void serialize_TaskList_Item_t_into(const TaskList_Item_t& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_TaskList_Item_t(const TaskList_Item_t& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_TaskList_Total_t_into(const TaskList_Total_t& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_TaskList_Total_t(const TaskList_Total_t& obj) {
    std::string json;
    serialize_TaskList_Total_t_into(obj, json);
    return json;
}

inline void serialize_TaskList_Has_more_t_into(const TaskList_Has_more_t& obj, std::string& json) {
    json.append(obj ? "true" : "false");
}

inline std::string serialize_TaskList_Has_more_t(const TaskList_Has_more_t& obj) {
    return obj ? "true" : "false";
}

inline void serialize_HealthResponse_into(const HealthResponse& obj, std::string& json) {
    json.push_back('{');
    json.append("\"status\":");
    json.push_back('"');
    json.append(to_string(obj.status));
    json.push_back('"');
    json.append(",\"timestamp\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.timestamp, json);
    json.push_back('"');
    json.append(",\"uptime_seconds\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.uptime_seconds);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"total_requests\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.total_requests);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.push_back('}');
}

inline std::string serialize_HealthResponse(const HealthResponse& obj) {
    std::string json;
    size_t reserve_estimate = 165;
    reserve_estimate += obj.timestamp.size();
    json.reserve(reserve_estimate);
    serialize_HealthResponse_into(obj, json);
    return json;
}

inline void serialize_HealthResponse_Status_t_into(const HealthResponse_Status_t& obj,
                                                   std::string& json) {
    auto str = to_string(obj);
    json.push_back('"');
    json.append(str);
    json.push_back('"');
}

inline std::string serialize_HealthResponse_Status_t(const HealthResponse_Status_t& obj) {
    std::string json;
    auto str = to_string(obj);
    json.reserve(str.size() + 2);
    serialize_HealthResponse_Status_t_into(obj, json);
    return json;
}

inline void serialize_HealthResponse_Timestamp_t_into(const HealthResponse_Timestamp_t& obj,
                                                      std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_HealthResponse_Timestamp_t(const HealthResponse_Timestamp_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_HealthResponse_Timestamp_t_into(obj, json);
    return json;
}

inline void
serialize_HealthResponse_Uptime_seconds_t_into(const HealthResponse_Uptime_seconds_t& obj,
                                               std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string
serialize_HealthResponse_Uptime_seconds_t(const HealthResponse_Uptime_seconds_t& obj) {
    std::string json;
    serialize_HealthResponse_Uptime_seconds_t_into(obj, json);
    return json;
}

inline void
serialize_HealthResponse_Total_requests_t_into(const HealthResponse_Total_requests_t& obj,
                                               std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string
serialize_HealthResponse_Total_requests_t(const HealthResponse_Total_requests_t& obj) {
    std::string json;
    serialize_HealthResponse_Total_requests_t_into(obj, json);
    return json;
}

inline void serialize_ProblemDetails_into(const ProblemDetails& obj, std::string& json) {
    json.push_back('{');
    json.append("\"type\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.type, json);
    json.push_back('"');
    json.append(",\"title\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.title, json);
    json.push_back('"');
    json.append(",\"status\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.status);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.append(",\"detail\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.detail, json);
    json.push_back('"');
    json.append(",\"instance\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.instance, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_ProblemDetails(const ProblemDetails& obj) {
    std::string json;
    size_t reserve_estimate = 199;
    reserve_estimate += obj.type.size();
    reserve_estimate += obj.title.size();
    reserve_estimate += obj.detail.size();
    reserve_estimate += obj.instance.size();
    json.reserve(reserve_estimate);
    serialize_ProblemDetails_into(obj, json);
    return json;
}

inline void serialize_ProblemDetails_Type_t_into(const ProblemDetails_Type_t& obj,
                                                 std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_ProblemDetails_Type_t(const ProblemDetails_Type_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_ProblemDetails_Type_t_into(obj, json);
    return json;
}

inline void serialize_ProblemDetails_Title_t_into(const ProblemDetails_Title_t& obj,
                                                  std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_ProblemDetails_Title_t(const ProblemDetails_Title_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_ProblemDetails_Title_t_into(obj, json);
    return json;
}

inline void serialize_ProblemDetails_Status_t_into(const ProblemDetails_Status_t& obj,
                                                   std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_ProblemDetails_Status_t(const ProblemDetails_Status_t& obj) {
    std::string json;
    serialize_ProblemDetails_Status_t_into(obj, json);
    return json;
}

inline void serialize_ProblemDetails_Detail_t_into(const ProblemDetails_Detail_t& obj,
                                                   std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_ProblemDetails_Detail_t(const ProblemDetails_Detail_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_ProblemDetails_Detail_t_into(obj, json);
    return json;
}

inline void serialize_ProblemDetails_Instance_t_into(const ProblemDetails_Instance_t& obj,
                                                     std::string& json) {
    json.push_back('"');
    katana::serde::escape_json_string_into(obj, json);
    json.push_back('"');
}

inline std::string serialize_ProblemDetails_Instance_t(const ProblemDetails_Instance_t& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    serialize_ProblemDetails_Instance_t_into(obj, json);
    return json;
}

inline void serialize_listTasks_param_status_into(const listTasks_param_status& obj,
                                                  std::string& json) {
    auto str = to_string(obj);
    json.push_back('"');
    json.append(str);
    json.push_back('"');
}

inline std::string serialize_listTasks_param_status(const listTasks_param_status& obj) {
    std::string json;
    auto str = to_string(obj);
    json.reserve(str.size() + 2);
    serialize_listTasks_param_status_into(obj, json);
    return json;
}

inline void serialize_listTasks_param_priority_into(const listTasks_param_priority& obj,
                                                    std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_listTasks_param_priority(const listTasks_param_priority& obj) {
    std::string json;
    serialize_listTasks_param_priority_into(obj, json);
    return json;
}

inline void serialize_listTasks_param_limit_into(const listTasks_param_limit& obj,
                                                 std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_listTasks_param_limit(const listTasks_param_limit& obj) {
    std::string json;
    serialize_listTasks_param_limit_into(obj, json);
    return json;
}

inline void serialize_listTasks_param_offset_into(const listTasks_param_offset& obj,
                                                  std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_listTasks_param_offset(const listTasks_param_offset& obj) {
    std::string json;
    serialize_listTasks_param_offset_into(obj, json);
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

inline void serialize_getTask_param_id_into(const getTask_param_id& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_getTask_param_id(const getTask_param_id& obj) {
    std::string json;
    serialize_getTask_param_id_into(obj, json);
    return json;
}

inline void serialize_schema_4_into(const schema_4& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_4(const schema_4& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_5_into(const schema_5& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_5(const schema_5& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_updateTask_param_id_into(const updateTask_param_id& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_updateTask_param_id(const updateTask_param_id& obj) {
    std::string json;
    serialize_updateTask_param_id_into(obj, json);
    return json;
}

inline void serialize_schema_6_into(const schema_6& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_6(const schema_6& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_7_into(const schema_7& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_7(const schema_7& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_8_into(const schema_8& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_8(const schema_8& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_deleteTask_param_id_into(const deleteTask_param_id& obj, std::string& json) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    json.append(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_deleteTask_param_id(const deleteTask_param_id& obj) {
    std::string json;
    serialize_deleteTask_param_id_into(obj, json);
    return json;
}

inline void serialize_schema_9_into(const schema_9& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_9(const schema_9& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_10_into(const schema_10& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_10(const schema_10& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_11_into(const schema_11& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_11(const schema_11& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_12_into(const schema_12& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_12(const schema_12& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_13_into(const schema_13& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_13(const schema_13& obj) {
    (void)obj;
    return std::string("{}");
}

inline void serialize_schema_14_into(const schema_14& obj, std::string& json) {
    (void)obj;
    json.append("{}");
}

inline std::string serialize_schema_14(const schema_14& obj) {
    (void)obj;
    return std::string("{}");
}

// ============================================================
// Array Parse Functions
// ============================================================

[[nodiscard]] inline std::optional<std::vector<Task>>
parse_Task_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<Task> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_Task(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Task>> parse_Task_array(std::string_view json,
                                                                       monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Task_Id_t>>
parse_Task_Id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<Task_Id_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_Task_Id_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Task_Id_t>>
parse_Task_Id_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Id_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Task_Title_t>>
parse_Task_Title_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<Task_Title_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_Task_Title_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Task_Title_t>>
parse_Task_Title_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Title_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Task_Description_t>>
parse_Task_Description_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<Task_Description_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_Task_Description_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Task_Description_t>>
parse_Task_Description_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Description_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Task_Status_t>>
parse_Task_Status_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<Task_Status_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_Task_Status_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Task_Status_t>>
parse_Task_Status_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Status_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Task_Priority_t>>
parse_Task_Priority_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<Task_Priority_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_Task_Priority_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Task_Priority_t>>
parse_Task_Priority_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Priority_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Task_Tags_t>>
parse_Task_Tags_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<Task_Tags_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_Task_Tags_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Task_Tags_t>>
parse_Task_Tags_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Tags_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Task_Item_t>>
parse_Task_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<Task_Item_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_Task_Item_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Task_Item_t>>
parse_Task_Item_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Item_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Task_Assignee_t>>
parse_Task_Assignee_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<Task_Assignee_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_Task_Assignee_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Task_Assignee_t>>
parse_Task_Assignee_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Assignee_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Task_Due_date_t>>
parse_Task_Due_date_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<Task_Due_date_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_Task_Due_date_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Task_Due_date_t>>
parse_Task_Due_date_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Due_date_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Task_Created_at_t>>
parse_Task_Created_at_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<Task_Created_at_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_Task_Created_at_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Task_Created_at_t>>
parse_Task_Created_at_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Created_at_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Task_Updated_at_t>>
parse_Task_Updated_at_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<Task_Updated_at_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_Task_Updated_at_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Task_Updated_at_t>>
parse_Task_Updated_at_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Updated_at_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<Task_Metadata_t>>
parse_Task_Metadata_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<Task_Metadata_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_Task_Metadata_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Task_Metadata_t>>
parse_Task_Metadata_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Task_Metadata_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<User>>
parse_User_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<User> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_User(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<User>> parse_User_array(std::string_view json,
                                                                       monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_User_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<User_Id_t>>
parse_User_Id_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<User_Id_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_User_Id_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<User_Id_t>>
parse_User_Id_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_User_Id_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<User_Email_t>>
parse_User_Email_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<User_Email_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_User_Email_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<User_Email_t>>
parse_User_Email_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_User_Email_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<User_Name_t>>
parse_User_Name_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<User_Name_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_User_Name_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<User_Name_t>>
parse_User_Name_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_User_Name_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest>>
parse_CreateTaskRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<CreateTaskRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_CreateTaskRequest(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest>>
parse_CreateTaskRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateTaskRequest_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Title_t>>
parse_CreateTaskRequest_Title_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<CreateTaskRequest_Title_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_CreateTaskRequest_Title_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Title_t>>
parse_CreateTaskRequest_Title_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateTaskRequest_Title_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Description_t>>
parse_CreateTaskRequest_Description_t_array(katana::serde::json_cursor& cur,
                                            monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<CreateTaskRequest_Description_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_CreateTaskRequest_Description_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Description_t>>
parse_CreateTaskRequest_Description_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateTaskRequest_Description_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Priority_t>>
parse_CreateTaskRequest_Priority_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<CreateTaskRequest_Priority_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_CreateTaskRequest_Priority_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Priority_t>>
parse_CreateTaskRequest_Priority_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateTaskRequest_Priority_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Tags_t>>
parse_CreateTaskRequest_Tags_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<CreateTaskRequest_Tags_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_CreateTaskRequest_Tags_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Tags_t>>
parse_CreateTaskRequest_Tags_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateTaskRequest_Tags_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Item_t>>
parse_CreateTaskRequest_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<CreateTaskRequest_Item_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_CreateTaskRequest_Item_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Item_t>>
parse_CreateTaskRequest_Item_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateTaskRequest_Item_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Assignee_id_t>>
parse_CreateTaskRequest_Assignee_id_t_array(katana::serde::json_cursor& cur,
                                            monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<CreateTaskRequest_Assignee_id_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_CreateTaskRequest_Assignee_id_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Assignee_id_t>>
parse_CreateTaskRequest_Assignee_id_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateTaskRequest_Assignee_id_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Due_date_t>>
parse_CreateTaskRequest_Due_date_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<CreateTaskRequest_Due_date_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_CreateTaskRequest_Due_date_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateTaskRequest_Due_date_t>>
parse_CreateTaskRequest_Due_date_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateTaskRequest_Due_date_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest>>
parse_UpdateTaskRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<UpdateTaskRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_UpdateTaskRequest(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest>>
parse_UpdateTaskRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateTaskRequest_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Title_t>>
parse_UpdateTaskRequest_Title_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<UpdateTaskRequest_Title_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_UpdateTaskRequest_Title_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Title_t>>
parse_UpdateTaskRequest_Title_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateTaskRequest_Title_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Description_t>>
parse_UpdateTaskRequest_Description_t_array(katana::serde::json_cursor& cur,
                                            monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<UpdateTaskRequest_Description_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_UpdateTaskRequest_Description_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Description_t>>
parse_UpdateTaskRequest_Description_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateTaskRequest_Description_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Status_t>>
parse_UpdateTaskRequest_Status_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<UpdateTaskRequest_Status_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_UpdateTaskRequest_Status_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Status_t>>
parse_UpdateTaskRequest_Status_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateTaskRequest_Status_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Priority_t>>
parse_UpdateTaskRequest_Priority_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<UpdateTaskRequest_Priority_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_UpdateTaskRequest_Priority_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Priority_t>>
parse_UpdateTaskRequest_Priority_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateTaskRequest_Priority_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Tags_t>>
parse_UpdateTaskRequest_Tags_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<UpdateTaskRequest_Tags_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_UpdateTaskRequest_Tags_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Tags_t>>
parse_UpdateTaskRequest_Tags_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateTaskRequest_Tags_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Item_t>>
parse_UpdateTaskRequest_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<UpdateTaskRequest_Item_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_UpdateTaskRequest_Item_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Item_t>>
parse_UpdateTaskRequest_Item_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateTaskRequest_Item_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Assignee_id_t>>
parse_UpdateTaskRequest_Assignee_id_t_array(katana::serde::json_cursor& cur,
                                            monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<UpdateTaskRequest_Assignee_id_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_UpdateTaskRequest_Assignee_id_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Assignee_id_t>>
parse_UpdateTaskRequest_Assignee_id_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateTaskRequest_Assignee_id_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Due_date_t>>
parse_UpdateTaskRequest_Due_date_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<UpdateTaskRequest_Due_date_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_UpdateTaskRequest_Due_date_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UpdateTaskRequest_Due_date_t>>
parse_UpdateTaskRequest_Due_date_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateTaskRequest_Due_date_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateRequest>>
parse_BatchCreateRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<BatchCreateRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_BatchCreateRequest(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateRequest>>
parse_BatchCreateRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_BatchCreateRequest_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateRequest_Tasks_t>>
parse_BatchCreateRequest_Tasks_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<BatchCreateRequest_Tasks_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_BatchCreateRequest_Tasks_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateRequest_Tasks_t>>
parse_BatchCreateRequest_Tasks_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_BatchCreateRequest_Tasks_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateRequest_Item_t>>
parse_BatchCreateRequest_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<BatchCreateRequest_Item_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_BatchCreateRequest_Item_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateRequest_Item_t>>
parse_BatchCreateRequest_Item_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_BatchCreateRequest_Item_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse>>
parse_BatchCreateResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<BatchCreateResponse> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_BatchCreateResponse(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse>>
parse_BatchCreateResponse_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_BatchCreateResponse_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Created_t>>
parse_BatchCreateResponse_Created_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<BatchCreateResponse_Created_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_BatchCreateResponse_Created_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Created_t>>
parse_BatchCreateResponse_Created_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_BatchCreateResponse_Created_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Item_t>>
parse_BatchCreateResponse_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<BatchCreateResponse_Item_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_BatchCreateResponse_Item_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Item_t>>
parse_BatchCreateResponse_Item_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_BatchCreateResponse_Item_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Failed_t>>
parse_BatchCreateResponse_Failed_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<BatchCreateResponse_Failed_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_BatchCreateResponse_Failed_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Failed_t>>
parse_BatchCreateResponse_Failed_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_BatchCreateResponse_Failed_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Item_t_1>>
parse_BatchCreateResponse_Item_t_1_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<BatchCreateResponse_Item_t_1> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_BatchCreateResponse_Item_t_1(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Item_t_1>>
parse_BatchCreateResponse_Item_t_1_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_BatchCreateResponse_Item_t_1_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Index_t>>
parse_BatchCreateResponse_Index_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<BatchCreateResponse_Index_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_BatchCreateResponse_Index_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Index_t>>
parse_BatchCreateResponse_Index_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_BatchCreateResponse_Index_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Error_t>>
parse_BatchCreateResponse_Error_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<BatchCreateResponse_Error_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_BatchCreateResponse_Error_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<BatchCreateResponse_Error_t>>
parse_BatchCreateResponse_Error_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_BatchCreateResponse_Error_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest>>
parse_SearchRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<SearchRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_SearchRequest(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest>>
parse_SearchRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Title_contains_t>>
parse_SearchRequest_Title_contains_t_array(katana::serde::json_cursor& cur,
                                           monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<SearchRequest_Title_contains_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_SearchRequest_Title_contains_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Title_contains_t>>
parse_SearchRequest_Title_contains_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_Title_contains_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Statuses_t>>
parse_SearchRequest_Statuses_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<SearchRequest_Statuses_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_SearchRequest_Statuses_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Statuses_t>>
parse_SearchRequest_Statuses_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_Statuses_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Item_t>>
parse_SearchRequest_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<SearchRequest_Item_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_SearchRequest_Item_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Item_t>>
parse_SearchRequest_Item_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_Item_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Min_priority_t>>
parse_SearchRequest_Min_priority_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<SearchRequest_Min_priority_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_SearchRequest_Min_priority_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Min_priority_t>>
parse_SearchRequest_Min_priority_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_Min_priority_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Max_priority_t>>
parse_SearchRequest_Max_priority_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<SearchRequest_Max_priority_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_SearchRequest_Max_priority_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Max_priority_t>>
parse_SearchRequest_Max_priority_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_Max_priority_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Tags_t>>
parse_SearchRequest_Tags_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<SearchRequest_Tags_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_SearchRequest_Tags_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Tags_t>>
parse_SearchRequest_Tags_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_Tags_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Item_t_1>>
parse_SearchRequest_Item_t_1_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<SearchRequest_Item_t_1> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_SearchRequest_Item_t_1(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Item_t_1>>
parse_SearchRequest_Item_t_1_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_Item_t_1_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Created_after_t>>
parse_SearchRequest_Created_after_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<SearchRequest_Created_after_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_SearchRequest_Created_after_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Created_after_t>>
parse_SearchRequest_Created_after_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_Created_after_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Created_before_t>>
parse_SearchRequest_Created_before_t_array(katana::serde::json_cursor& cur,
                                           monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<SearchRequest_Created_before_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_SearchRequest_Created_before_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Created_before_t>>
parse_SearchRequest_Created_before_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_Created_before_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Has_assignee_t>>
parse_SearchRequest_Has_assignee_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<SearchRequest_Has_assignee_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_SearchRequest_Has_assignee_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SearchRequest_Has_assignee_t>>
parse_SearchRequest_Has_assignee_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SearchRequest_Has_assignee_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<TaskList>>
parse_TaskList_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<TaskList> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_TaskList(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<TaskList>>
parse_TaskList_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TaskList_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<TaskList_Tasks_t>>
parse_TaskList_Tasks_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<TaskList_Tasks_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_TaskList_Tasks_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<TaskList_Tasks_t>>
parse_TaskList_Tasks_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TaskList_Tasks_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<TaskList_Item_t>>
parse_TaskList_Item_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<TaskList_Item_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_TaskList_Item_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<TaskList_Item_t>>
parse_TaskList_Item_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TaskList_Item_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<TaskList_Total_t>>
parse_TaskList_Total_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<TaskList_Total_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_TaskList_Total_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<TaskList_Total_t>>
parse_TaskList_Total_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TaskList_Total_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<TaskList_Has_more_t>>
parse_TaskList_Has_more_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<TaskList_Has_more_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_TaskList_Has_more_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<TaskList_Has_more_t>>
parse_TaskList_Has_more_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TaskList_Has_more_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<HealthResponse>>
parse_HealthResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<HealthResponse> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_HealthResponse(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<HealthResponse>>
parse_HealthResponse_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_HealthResponse_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<HealthResponse_Status_t>>
parse_HealthResponse_Status_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<HealthResponse_Status_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_HealthResponse_Status_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<HealthResponse_Status_t>>
parse_HealthResponse_Status_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_HealthResponse_Status_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<HealthResponse_Timestamp_t>>
parse_HealthResponse_Timestamp_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<HealthResponse_Timestamp_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_HealthResponse_Timestamp_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<HealthResponse_Timestamp_t>>
parse_HealthResponse_Timestamp_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_HealthResponse_Timestamp_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<HealthResponse_Uptime_seconds_t>>
parse_HealthResponse_Uptime_seconds_t_array(katana::serde::json_cursor& cur,
                                            monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<HealthResponse_Uptime_seconds_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_HealthResponse_Uptime_seconds_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<HealthResponse_Uptime_seconds_t>>
parse_HealthResponse_Uptime_seconds_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_HealthResponse_Uptime_seconds_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<HealthResponse_Total_requests_t>>
parse_HealthResponse_Total_requests_t_array(katana::serde::json_cursor& cur,
                                            monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<HealthResponse_Total_requests_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_HealthResponse_Total_requests_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<HealthResponse_Total_requests_t>>
parse_HealthResponse_Total_requests_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_HealthResponse_Total_requests_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ProblemDetails>>
parse_ProblemDetails_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<ProblemDetails> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_ProblemDetails(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ProblemDetails>>
parse_ProblemDetails_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ProblemDetails_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Type_t>>
parse_ProblemDetails_Type_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<ProblemDetails_Type_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_ProblemDetails_Type_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Type_t>>
parse_ProblemDetails_Type_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ProblemDetails_Type_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Title_t>>
parse_ProblemDetails_Title_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<ProblemDetails_Title_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_ProblemDetails_Title_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Title_t>>
parse_ProblemDetails_Title_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ProblemDetails_Title_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Status_t>>
parse_ProblemDetails_Status_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<ProblemDetails_Status_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_ProblemDetails_Status_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Status_t>>
parse_ProblemDetails_Status_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ProblemDetails_Status_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Detail_t>>
parse_ProblemDetails_Detail_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<ProblemDetails_Detail_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_ProblemDetails_Detail_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Detail_t>>
parse_ProblemDetails_Detail_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ProblemDetails_Detail_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Instance_t>>
parse_ProblemDetails_Instance_t_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<ProblemDetails_Instance_t> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_ProblemDetails_Instance_t(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ProblemDetails_Instance_t>>
parse_ProblemDetails_Instance_t_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ProblemDetails_Instance_t_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<listTasks_param_status>>
parse_listTasks_param_status_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<listTasks_param_status> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_listTasks_param_status(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<listTasks_param_status>>
parse_listTasks_param_status_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_listTasks_param_status_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<listTasks_param_priority>>
parse_listTasks_param_priority_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<listTasks_param_priority> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_listTasks_param_priority(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<listTasks_param_priority>>
parse_listTasks_param_priority_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_listTasks_param_priority_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<listTasks_param_limit>>
parse_listTasks_param_limit_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<listTasks_param_limit> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_listTasks_param_limit(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<listTasks_param_limit>>
parse_listTasks_param_limit_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_listTasks_param_limit_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<listTasks_param_offset>>
parse_listTasks_param_offset_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<listTasks_param_offset> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_listTasks_param_offset(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<listTasks_param_offset>>
parse_listTasks_param_offset_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_listTasks_param_offset_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema>>
parse_schema_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_schema(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema>> parse_schema_array(std::string_view json,
                                                                           monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_1>>
parse_schema_1_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_1> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_schema_1(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_1>>
parse_schema_1_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_1_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_2>>
parse_schema_2_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_2> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_schema_2(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_2>>
parse_schema_2_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_2_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_3>>
parse_schema_3_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_3> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_schema_3(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_3>>
parse_schema_3_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_3_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<getTask_param_id>>
parse_getTask_param_id_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<getTask_param_id> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_getTask_param_id(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<getTask_param_id>>
parse_getTask_param_id_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_getTask_param_id_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_4>>
parse_schema_4_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_4> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_schema_4(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_4>>
parse_schema_4_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_4_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_5>>
parse_schema_5_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_5> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_schema_5(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_5>>
parse_schema_5_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_5_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<updateTask_param_id>>
parse_updateTask_param_id_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<updateTask_param_id> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_updateTask_param_id(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<updateTask_param_id>>
parse_updateTask_param_id_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_updateTask_param_id_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_6>>
parse_schema_6_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_6> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_schema_6(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_6>>
parse_schema_6_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_6_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_7>>
parse_schema_7_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_7> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_schema_7(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_7>>
parse_schema_7_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_7_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_8>>
parse_schema_8_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_8> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_schema_8(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_8>>
parse_schema_8_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_8_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<deleteTask_param_id>>
parse_deleteTask_param_id_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<deleteTask_param_id> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_deleteTask_param_id(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<deleteTask_param_id>>
parse_deleteTask_param_id_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_deleteTask_param_id_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_9>>
parse_schema_9_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_9> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_schema_9(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_9>>
parse_schema_9_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_9_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_10>>
parse_schema_10_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_10> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_schema_10(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_10>>
parse_schema_10_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_10_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_11>>
parse_schema_11_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_11> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_schema_11(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_11>>
parse_schema_11_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_11_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_12>>
parse_schema_12_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_12> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_schema_12(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_12>>
parse_schema_12_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_12_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_13>>
parse_schema_13_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_13> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_schema_13(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_13>>
parse_schema_13_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_13_array(cur, arena);
}

[[nodiscard]] inline std::optional<std::vector<schema_14>>
parse_schema_14_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_14> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current cursor position
        auto obj = parse_schema_14(cur, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<schema_14>>
parse_schema_14_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_schema_14_array(cur, arena);
}

// ============================================================
// Array Serialize Functions
// ============================================================

inline void serialize_Task_array_into(const std::vector<Task>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_array(const std::vector<Task>& arr) {
    std::string json;
    json.reserve(arr.size() * 678 + 2);
    serialize_Task_array_into(arr, json);
    return json;
}

inline void serialize_Task_array_into(const arena_vector<Task>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_array(const arena_vector<Task>& arr) {
    std::string json;
    json.reserve(arr.size() * 678 + 2);
    serialize_Task_array_into(arr, json);
    return json;
}

inline void serialize_Task_Id_t_array_into(const std::vector<Task_Id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Id_t_array(const std::vector<Task_Id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_Task_Id_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Id_t_array_into(const arena_vector<Task_Id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Id_t_array(const arena_vector<Task_Id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_Task_Id_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Title_t_array_into(const std::vector<Task_Title_t>& arr,
                                              std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Title_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Title_t_array(const std::vector<Task_Title_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Task_Title_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Title_t_array_into(const arena_vector<Task_Title_t>& arr,
                                              std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Title_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Title_t_array(const arena_vector<Task_Title_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Task_Title_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Description_t_array_into(const std::vector<Task_Description_t>& arr,
                                                    std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Description_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Description_t_array(const std::vector<Task_Description_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Task_Description_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Description_t_array_into(const arena_vector<Task_Description_t>& arr,
                                                    std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Description_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Description_t_array(const arena_vector<Task_Description_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Task_Description_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Status_t_array_into(const std::vector<Task_Status_t>& arr,
                                               std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Status_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Status_t_array(const std::vector<Task_Status_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Task_Status_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Status_t_array_into(const arena_vector<Task_Status_t>& arr,
                                               std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Status_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Status_t_array(const arena_vector<Task_Status_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Task_Status_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Priority_t_array_into(const std::vector<Task_Priority_t>& arr,
                                                 std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Priority_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Priority_t_array(const std::vector<Task_Priority_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_Task_Priority_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Priority_t_array_into(const arena_vector<Task_Priority_t>& arr,
                                                 std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Priority_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Priority_t_array(const arena_vector<Task_Priority_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_Task_Priority_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Tags_t_array_into(const std::vector<Task_Tags_t>& arr,
                                             std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Tags_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Tags_t_array(const std::vector<Task_Tags_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 130 + 2);
    serialize_Task_Tags_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Tags_t_array_into(const arena_vector<Task_Tags_t>& arr,
                                             std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Tags_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Tags_t_array(const arena_vector<Task_Tags_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 130 + 2);
    serialize_Task_Tags_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Item_t_array_into(const std::vector<Task_Item_t>& arr,
                                             std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Item_t_array(const std::vector<Task_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Task_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Item_t_array_into(const arena_vector<Task_Item_t>& arr,
                                             std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Item_t_array(const arena_vector<Task_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Task_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Assignee_t_array_into(const std::vector<Task_Assignee_t>& arr,
                                                 std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Assignee_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Assignee_t_array(const std::vector<Task_Assignee_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_Task_Assignee_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Assignee_t_array_into(const arena_vector<Task_Assignee_t>& arr,
                                                 std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Assignee_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Assignee_t_array(const arena_vector<Task_Assignee_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_Task_Assignee_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Due_date_t_array_into(const std::vector<Task_Due_date_t>& arr,
                                                 std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Due_date_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Due_date_t_array(const std::vector<Task_Due_date_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Task_Due_date_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Due_date_t_array_into(const arena_vector<Task_Due_date_t>& arr,
                                                 std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Due_date_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Due_date_t_array(const arena_vector<Task_Due_date_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Task_Due_date_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Created_at_t_array_into(const std::vector<Task_Created_at_t>& arr,
                                                   std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Created_at_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Created_at_t_array(const std::vector<Task_Created_at_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Task_Created_at_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Created_at_t_array_into(const arena_vector<Task_Created_at_t>& arr,
                                                   std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Created_at_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Created_at_t_array(const arena_vector<Task_Created_at_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Task_Created_at_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Updated_at_t_array_into(const std::vector<Task_Updated_at_t>& arr,
                                                   std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Updated_at_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Updated_at_t_array(const std::vector<Task_Updated_at_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Task_Updated_at_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Updated_at_t_array_into(const arena_vector<Task_Updated_at_t>& arr,
                                                   std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Updated_at_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Updated_at_t_array(const arena_vector<Task_Updated_at_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_Task_Updated_at_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Metadata_t_array_into(const std::vector<Task_Metadata_t>& arr,
                                                 std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Metadata_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Metadata_t_array(const std::vector<Task_Metadata_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_Task_Metadata_t_array_into(arr, json);
    return json;
}

inline void serialize_Task_Metadata_t_array_into(const arena_vector<Task_Metadata_t>& arr,
                                                 std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_Task_Metadata_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Task_Metadata_t_array(const arena_vector<Task_Metadata_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_Task_Metadata_t_array_into(arr, json);
    return json;
}

inline void serialize_User_array_into(const std::vector<User>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_User_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_User_array(const std::vector<User>& arr) {
    std::string json;
    json.reserve(arr.size() * 109 + 2);
    serialize_User_array_into(arr, json);
    return json;
}

inline void serialize_User_array_into(const arena_vector<User>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_User_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_User_array(const arena_vector<User>& arr) {
    std::string json;
    json.reserve(arr.size() * 109 + 2);
    serialize_User_array_into(arr, json);
    return json;
}

inline void serialize_User_Id_t_array_into(const std::vector<User_Id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_User_Id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_User_Id_t_array(const std::vector<User_Id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_User_Id_t_array_into(arr, json);
    return json;
}

inline void serialize_User_Id_t_array_into(const arena_vector<User_Id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_User_Id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_User_Id_t_array(const arena_vector<User_Id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_User_Id_t_array_into(arr, json);
    return json;
}

inline void serialize_User_Email_t_array_into(const std::vector<User_Email_t>& arr,
                                              std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_User_Email_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_User_Email_t_array(const std::vector<User_Email_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_User_Email_t_array_into(arr, json);
    return json;
}

inline void serialize_User_Email_t_array_into(const arena_vector<User_Email_t>& arr,
                                              std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_User_Email_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_User_Email_t_array(const arena_vector<User_Email_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_User_Email_t_array_into(arr, json);
    return json;
}

inline void serialize_User_Name_t_array_into(const std::vector<User_Name_t>& arr,
                                             std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_User_Name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_User_Name_t_array(const std::vector<User_Name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_User_Name_t_array_into(arr, json);
    return json;
}

inline void serialize_User_Name_t_array_into(const arena_vector<User_Name_t>& arr,
                                             std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_User_Name_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_User_Name_t_array(const arena_vector<User_Name_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_User_Name_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateTaskRequest_array_into(const std::vector<CreateTaskRequest>& arr,
                                                   std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateTaskRequest_array(const std::vector<CreateTaskRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 273 + 2);
    serialize_CreateTaskRequest_array_into(arr, json);
    return json;
}

inline void serialize_CreateTaskRequest_array_into(const arena_vector<CreateTaskRequest>& arr,
                                                   std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateTaskRequest_array(const arena_vector<CreateTaskRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 273 + 2);
    serialize_CreateTaskRequest_array_into(arr, json);
    return json;
}

inline void
serialize_CreateTaskRequest_Title_t_array_into(const std::vector<CreateTaskRequest_Title_t>& arr,
                                               std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_Title_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_CreateTaskRequest_Title_t_array(const std::vector<CreateTaskRequest_Title_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CreateTaskRequest_Title_t_array_into(arr, json);
    return json;
}

inline void
serialize_CreateTaskRequest_Title_t_array_into(const arena_vector<CreateTaskRequest_Title_t>& arr,
                                               std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_Title_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_CreateTaskRequest_Title_t_array(const arena_vector<CreateTaskRequest_Title_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CreateTaskRequest_Title_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateTaskRequest_Description_t_array_into(
    const std::vector<CreateTaskRequest_Description_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_Description_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateTaskRequest_Description_t_array(
    const std::vector<CreateTaskRequest_Description_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CreateTaskRequest_Description_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateTaskRequest_Description_t_array_into(
    const arena_vector<CreateTaskRequest_Description_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_Description_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateTaskRequest_Description_t_array(
    const arena_vector<CreateTaskRequest_Description_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CreateTaskRequest_Description_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateTaskRequest_Priority_t_array_into(
    const std::vector<CreateTaskRequest_Priority_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_Priority_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_CreateTaskRequest_Priority_t_array(const std::vector<CreateTaskRequest_Priority_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CreateTaskRequest_Priority_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateTaskRequest_Priority_t_array_into(
    const arena_vector<CreateTaskRequest_Priority_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_Priority_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateTaskRequest_Priority_t_array(
    const arena_vector<CreateTaskRequest_Priority_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CreateTaskRequest_Priority_t_array_into(arr, json);
    return json;
}

inline void
serialize_CreateTaskRequest_Tags_t_array_into(const std::vector<CreateTaskRequest_Tags_t>& arr,
                                              std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_Tags_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_CreateTaskRequest_Tags_t_array(const std::vector<CreateTaskRequest_Tags_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 130 + 2);
    serialize_CreateTaskRequest_Tags_t_array_into(arr, json);
    return json;
}

inline void
serialize_CreateTaskRequest_Tags_t_array_into(const arena_vector<CreateTaskRequest_Tags_t>& arr,
                                              std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_Tags_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_CreateTaskRequest_Tags_t_array(const arena_vector<CreateTaskRequest_Tags_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 130 + 2);
    serialize_CreateTaskRequest_Tags_t_array_into(arr, json);
    return json;
}

inline void
serialize_CreateTaskRequest_Item_t_array_into(const std::vector<CreateTaskRequest_Item_t>& arr,
                                              std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_CreateTaskRequest_Item_t_array(const std::vector<CreateTaskRequest_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CreateTaskRequest_Item_t_array_into(arr, json);
    return json;
}

inline void
serialize_CreateTaskRequest_Item_t_array_into(const arena_vector<CreateTaskRequest_Item_t>& arr,
                                              std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_CreateTaskRequest_Item_t_array(const arena_vector<CreateTaskRequest_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CreateTaskRequest_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateTaskRequest_Assignee_id_t_array_into(
    const std::vector<CreateTaskRequest_Assignee_id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_Assignee_id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateTaskRequest_Assignee_id_t_array(
    const std::vector<CreateTaskRequest_Assignee_id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CreateTaskRequest_Assignee_id_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateTaskRequest_Assignee_id_t_array_into(
    const arena_vector<CreateTaskRequest_Assignee_id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_Assignee_id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateTaskRequest_Assignee_id_t_array(
    const arena_vector<CreateTaskRequest_Assignee_id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_CreateTaskRequest_Assignee_id_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateTaskRequest_Due_date_t_array_into(
    const std::vector<CreateTaskRequest_Due_date_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_Due_date_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_CreateTaskRequest_Due_date_t_array(const std::vector<CreateTaskRequest_Due_date_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CreateTaskRequest_Due_date_t_array_into(arr, json);
    return json;
}

inline void serialize_CreateTaskRequest_Due_date_t_array_into(
    const arena_vector<CreateTaskRequest_Due_date_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_CreateTaskRequest_Due_date_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateTaskRequest_Due_date_t_array(
    const arena_vector<CreateTaskRequest_Due_date_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_CreateTaskRequest_Due_date_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateTaskRequest_array_into(const std::vector<UpdateTaskRequest>& arr,
                                                   std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateTaskRequest_array(const std::vector<UpdateTaskRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 315 + 2);
    serialize_UpdateTaskRequest_array_into(arr, json);
    return json;
}

inline void serialize_UpdateTaskRequest_array_into(const arena_vector<UpdateTaskRequest>& arr,
                                                   std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateTaskRequest_array(const arena_vector<UpdateTaskRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 315 + 2);
    serialize_UpdateTaskRequest_array_into(arr, json);
    return json;
}

inline void
serialize_UpdateTaskRequest_Title_t_array_into(const std::vector<UpdateTaskRequest_Title_t>& arr,
                                               std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_Title_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_UpdateTaskRequest_Title_t_array(const std::vector<UpdateTaskRequest_Title_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UpdateTaskRequest_Title_t_array_into(arr, json);
    return json;
}

inline void
serialize_UpdateTaskRequest_Title_t_array_into(const arena_vector<UpdateTaskRequest_Title_t>& arr,
                                               std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_Title_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_UpdateTaskRequest_Title_t_array(const arena_vector<UpdateTaskRequest_Title_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UpdateTaskRequest_Title_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateTaskRequest_Description_t_array_into(
    const std::vector<UpdateTaskRequest_Description_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_Description_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateTaskRequest_Description_t_array(
    const std::vector<UpdateTaskRequest_Description_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UpdateTaskRequest_Description_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateTaskRequest_Description_t_array_into(
    const arena_vector<UpdateTaskRequest_Description_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_Description_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateTaskRequest_Description_t_array(
    const arena_vector<UpdateTaskRequest_Description_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UpdateTaskRequest_Description_t_array_into(arr, json);
    return json;
}

inline void
serialize_UpdateTaskRequest_Status_t_array_into(const std::vector<UpdateTaskRequest_Status_t>& arr,
                                                std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_Status_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_UpdateTaskRequest_Status_t_array(const std::vector<UpdateTaskRequest_Status_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UpdateTaskRequest_Status_t_array_into(arr, json);
    return json;
}

inline void
serialize_UpdateTaskRequest_Status_t_array_into(const arena_vector<UpdateTaskRequest_Status_t>& arr,
                                                std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_Status_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_UpdateTaskRequest_Status_t_array(const arena_vector<UpdateTaskRequest_Status_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UpdateTaskRequest_Status_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateTaskRequest_Priority_t_array_into(
    const std::vector<UpdateTaskRequest_Priority_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_Priority_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_UpdateTaskRequest_Priority_t_array(const std::vector<UpdateTaskRequest_Priority_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_UpdateTaskRequest_Priority_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateTaskRequest_Priority_t_array_into(
    const arena_vector<UpdateTaskRequest_Priority_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_Priority_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateTaskRequest_Priority_t_array(
    const arena_vector<UpdateTaskRequest_Priority_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_UpdateTaskRequest_Priority_t_array_into(arr, json);
    return json;
}

inline void
serialize_UpdateTaskRequest_Tags_t_array_into(const std::vector<UpdateTaskRequest_Tags_t>& arr,
                                              std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_Tags_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_UpdateTaskRequest_Tags_t_array(const std::vector<UpdateTaskRequest_Tags_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 130 + 2);
    serialize_UpdateTaskRequest_Tags_t_array_into(arr, json);
    return json;
}

inline void
serialize_UpdateTaskRequest_Tags_t_array_into(const arena_vector<UpdateTaskRequest_Tags_t>& arr,
                                              std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_Tags_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_UpdateTaskRequest_Tags_t_array(const arena_vector<UpdateTaskRequest_Tags_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 130 + 2);
    serialize_UpdateTaskRequest_Tags_t_array_into(arr, json);
    return json;
}

inline void
serialize_UpdateTaskRequest_Item_t_array_into(const std::vector<UpdateTaskRequest_Item_t>& arr,
                                              std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_UpdateTaskRequest_Item_t_array(const std::vector<UpdateTaskRequest_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UpdateTaskRequest_Item_t_array_into(arr, json);
    return json;
}

inline void
serialize_UpdateTaskRequest_Item_t_array_into(const arena_vector<UpdateTaskRequest_Item_t>& arr,
                                              std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_UpdateTaskRequest_Item_t_array(const arena_vector<UpdateTaskRequest_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UpdateTaskRequest_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateTaskRequest_Assignee_id_t_array_into(
    const std::vector<UpdateTaskRequest_Assignee_id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_Assignee_id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateTaskRequest_Assignee_id_t_array(
    const std::vector<UpdateTaskRequest_Assignee_id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_UpdateTaskRequest_Assignee_id_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateTaskRequest_Assignee_id_t_array_into(
    const arena_vector<UpdateTaskRequest_Assignee_id_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_Assignee_id_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateTaskRequest_Assignee_id_t_array(
    const arena_vector<UpdateTaskRequest_Assignee_id_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_UpdateTaskRequest_Assignee_id_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateTaskRequest_Due_date_t_array_into(
    const std::vector<UpdateTaskRequest_Due_date_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_Due_date_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_UpdateTaskRequest_Due_date_t_array(const std::vector<UpdateTaskRequest_Due_date_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UpdateTaskRequest_Due_date_t_array_into(arr, json);
    return json;
}

inline void serialize_UpdateTaskRequest_Due_date_t_array_into(
    const arena_vector<UpdateTaskRequest_Due_date_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_UpdateTaskRequest_Due_date_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateTaskRequest_Due_date_t_array(
    const arena_vector<UpdateTaskRequest_Due_date_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UpdateTaskRequest_Due_date_t_array_into(arr, json);
    return json;
}

inline void serialize_BatchCreateRequest_array_into(const std::vector<BatchCreateRequest>& arr,
                                                    std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_BatchCreateRequest_array(const std::vector<BatchCreateRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 75 + 2);
    serialize_BatchCreateRequest_array_into(arr, json);
    return json;
}

inline void serialize_BatchCreateRequest_array_into(const arena_vector<BatchCreateRequest>& arr,
                                                    std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_BatchCreateRequest_array(const arena_vector<BatchCreateRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 75 + 2);
    serialize_BatchCreateRequest_array_into(arr, json);
    return json;
}

inline void
serialize_BatchCreateRequest_Tasks_t_array_into(const std::vector<BatchCreateRequest_Tasks_t>& arr,
                                                std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateRequest_Tasks_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_BatchCreateRequest_Tasks_t_array(const std::vector<BatchCreateRequest_Tasks_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 1094 + 2);
    serialize_BatchCreateRequest_Tasks_t_array_into(arr, json);
    return json;
}

inline void
serialize_BatchCreateRequest_Tasks_t_array_into(const arena_vector<BatchCreateRequest_Tasks_t>& arr,
                                                std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateRequest_Tasks_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_BatchCreateRequest_Tasks_t_array(const arena_vector<BatchCreateRequest_Tasks_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 1094 + 2);
    serialize_BatchCreateRequest_Tasks_t_array_into(arr, json);
    return json;
}

inline void
serialize_BatchCreateRequest_Item_t_array_into(const std::vector<BatchCreateRequest_Item_t>& arr,
                                               std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_BatchCreateRequest_Item_t_array(const std::vector<BatchCreateRequest_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_BatchCreateRequest_Item_t_array_into(arr, json);
    return json;
}

inline void
serialize_BatchCreateRequest_Item_t_array_into(const arena_vector<BatchCreateRequest_Item_t>& arr,
                                               std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_BatchCreateRequest_Item_t_array(const arena_vector<BatchCreateRequest_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_BatchCreateRequest_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_BatchCreateResponse_array_into(const std::vector<BatchCreateResponse>& arr,
                                                     std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_BatchCreateResponse_array(const std::vector<BatchCreateResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 151 + 2);
    serialize_BatchCreateResponse_array_into(arr, json);
    return json;
}

inline void serialize_BatchCreateResponse_array_into(const arena_vector<BatchCreateResponse>& arr,
                                                     std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_BatchCreateResponse_array(const arena_vector<BatchCreateResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 151 + 2);
    serialize_BatchCreateResponse_array_into(arr, json);
    return json;
}

inline void serialize_BatchCreateResponse_Created_t_array_into(
    const std::vector<BatchCreateResponse_Created_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateResponse_Created_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_BatchCreateResponse_Created_t_array(
    const std::vector<BatchCreateResponse_Created_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2714 + 2);
    serialize_BatchCreateResponse_Created_t_array_into(arr, json);
    return json;
}

inline void serialize_BatchCreateResponse_Created_t_array_into(
    const arena_vector<BatchCreateResponse_Created_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateResponse_Created_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_BatchCreateResponse_Created_t_array(
    const arena_vector<BatchCreateResponse_Created_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2714 + 2);
    serialize_BatchCreateResponse_Created_t_array_into(arr, json);
    return json;
}

inline void
serialize_BatchCreateResponse_Item_t_array_into(const std::vector<BatchCreateResponse_Item_t>& arr,
                                                std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateResponse_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_BatchCreateResponse_Item_t_array(const std::vector<BatchCreateResponse_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_BatchCreateResponse_Item_t_array_into(arr, json);
    return json;
}

inline void
serialize_BatchCreateResponse_Item_t_array_into(const arena_vector<BatchCreateResponse_Item_t>& arr,
                                                std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateResponse_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_BatchCreateResponse_Item_t_array(const arena_vector<BatchCreateResponse_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_BatchCreateResponse_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_BatchCreateResponse_Failed_t_array_into(
    const std::vector<BatchCreateResponse_Failed_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateResponse_Failed_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_BatchCreateResponse_Failed_t_array(const std::vector<BatchCreateResponse_Failed_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 290 + 2);
    serialize_BatchCreateResponse_Failed_t_array_into(arr, json);
    return json;
}

inline void serialize_BatchCreateResponse_Failed_t_array_into(
    const arena_vector<BatchCreateResponse_Failed_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateResponse_Failed_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_BatchCreateResponse_Failed_t_array(
    const arena_vector<BatchCreateResponse_Failed_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 290 + 2);
    serialize_BatchCreateResponse_Failed_t_array_into(arr, json);
    return json;
}

inline void serialize_BatchCreateResponse_Item_t_1_array_into(
    const std::vector<BatchCreateResponse_Item_t_1>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateResponse_Item_t_1_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_BatchCreateResponse_Item_t_1_array(const std::vector<BatchCreateResponse_Item_t_1>& arr) {
    std::string json;
    json.reserve(arr.size() * 72 + 2);
    serialize_BatchCreateResponse_Item_t_1_array_into(arr, json);
    return json;
}

inline void serialize_BatchCreateResponse_Item_t_1_array_into(
    const arena_vector<BatchCreateResponse_Item_t_1>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateResponse_Item_t_1_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_BatchCreateResponse_Item_t_1_array(
    const arena_vector<BatchCreateResponse_Item_t_1>& arr) {
    std::string json;
    json.reserve(arr.size() * 72 + 2);
    serialize_BatchCreateResponse_Item_t_1_array_into(arr, json);
    return json;
}

inline void serialize_BatchCreateResponse_Index_t_array_into(
    const std::vector<BatchCreateResponse_Index_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateResponse_Index_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_BatchCreateResponse_Index_t_array(const std::vector<BatchCreateResponse_Index_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_BatchCreateResponse_Index_t_array_into(arr, json);
    return json;
}

inline void serialize_BatchCreateResponse_Index_t_array_into(
    const arena_vector<BatchCreateResponse_Index_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateResponse_Index_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_BatchCreateResponse_Index_t_array(const arena_vector<BatchCreateResponse_Index_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_BatchCreateResponse_Index_t_array_into(arr, json);
    return json;
}

inline void serialize_BatchCreateResponse_Error_t_array_into(
    const std::vector<BatchCreateResponse_Error_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateResponse_Error_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_BatchCreateResponse_Error_t_array(const std::vector<BatchCreateResponse_Error_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_BatchCreateResponse_Error_t_array_into(arr, json);
    return json;
}

inline void serialize_BatchCreateResponse_Error_t_array_into(
    const arena_vector<BatchCreateResponse_Error_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_BatchCreateResponse_Error_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_BatchCreateResponse_Error_t_array(const arena_vector<BatchCreateResponse_Error_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_BatchCreateResponse_Error_t_array_into(arr, json);
    return json;
}

inline void serialize_SearchRequest_array_into(const std::vector<SearchRequest>& arr,
                                               std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SearchRequest_array(const std::vector<SearchRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 392 + 2);
    serialize_SearchRequest_array_into(arr, json);
    return json;
}

inline void serialize_SearchRequest_array_into(const arena_vector<SearchRequest>& arr,
                                               std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SearchRequest_array(const arena_vector<SearchRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 392 + 2);
    serialize_SearchRequest_array_into(arr, json);
    return json;
}

inline void serialize_SearchRequest_Title_contains_t_array_into(
    const std::vector<SearchRequest_Title_contains_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Title_contains_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SearchRequest_Title_contains_t_array(
    const std::vector<SearchRequest_Title_contains_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_SearchRequest_Title_contains_t_array_into(arr, json);
    return json;
}

inline void serialize_SearchRequest_Title_contains_t_array_into(
    const arena_vector<SearchRequest_Title_contains_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Title_contains_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SearchRequest_Title_contains_t_array(
    const arena_vector<SearchRequest_Title_contains_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_SearchRequest_Title_contains_t_array_into(arr, json);
    return json;
}

inline void
serialize_SearchRequest_Statuses_t_array_into(const std::vector<SearchRequest_Statuses_t>& arr,
                                              std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Statuses_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_SearchRequest_Statuses_t_array(const std::vector<SearchRequest_Statuses_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 130 + 2);
    serialize_SearchRequest_Statuses_t_array_into(arr, json);
    return json;
}

inline void
serialize_SearchRequest_Statuses_t_array_into(const arena_vector<SearchRequest_Statuses_t>& arr,
                                              std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Statuses_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_SearchRequest_Statuses_t_array(const arena_vector<SearchRequest_Statuses_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 130 + 2);
    serialize_SearchRequest_Statuses_t_array_into(arr, json);
    return json;
}

inline void serialize_SearchRequest_Item_t_array_into(const std::vector<SearchRequest_Item_t>& arr,
                                                      std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_SearchRequest_Item_t_array(const std::vector<SearchRequest_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_SearchRequest_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_SearchRequest_Item_t_array_into(const arena_vector<SearchRequest_Item_t>& arr,
                                                      std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_SearchRequest_Item_t_array(const arena_vector<SearchRequest_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_SearchRequest_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_SearchRequest_Min_priority_t_array_into(
    const std::vector<SearchRequest_Min_priority_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Min_priority_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_SearchRequest_Min_priority_t_array(const std::vector<SearchRequest_Min_priority_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_SearchRequest_Min_priority_t_array_into(arr, json);
    return json;
}

inline void serialize_SearchRequest_Min_priority_t_array_into(
    const arena_vector<SearchRequest_Min_priority_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Min_priority_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SearchRequest_Min_priority_t_array(
    const arena_vector<SearchRequest_Min_priority_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_SearchRequest_Min_priority_t_array_into(arr, json);
    return json;
}

inline void serialize_SearchRequest_Max_priority_t_array_into(
    const std::vector<SearchRequest_Max_priority_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Max_priority_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_SearchRequest_Max_priority_t_array(const std::vector<SearchRequest_Max_priority_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_SearchRequest_Max_priority_t_array_into(arr, json);
    return json;
}

inline void serialize_SearchRequest_Max_priority_t_array_into(
    const arena_vector<SearchRequest_Max_priority_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Max_priority_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SearchRequest_Max_priority_t_array(
    const arena_vector<SearchRequest_Max_priority_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_SearchRequest_Max_priority_t_array_into(arr, json);
    return json;
}

inline void serialize_SearchRequest_Tags_t_array_into(const std::vector<SearchRequest_Tags_t>& arr,
                                                      std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Tags_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_SearchRequest_Tags_t_array(const std::vector<SearchRequest_Tags_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 130 + 2);
    serialize_SearchRequest_Tags_t_array_into(arr, json);
    return json;
}

inline void serialize_SearchRequest_Tags_t_array_into(const arena_vector<SearchRequest_Tags_t>& arr,
                                                      std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Tags_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_SearchRequest_Tags_t_array(const arena_vector<SearchRequest_Tags_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 130 + 2);
    serialize_SearchRequest_Tags_t_array_into(arr, json);
    return json;
}

inline void
serialize_SearchRequest_Item_t_1_array_into(const std::vector<SearchRequest_Item_t_1>& arr,
                                            std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Item_t_1_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_SearchRequest_Item_t_1_array(const std::vector<SearchRequest_Item_t_1>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_SearchRequest_Item_t_1_array_into(arr, json);
    return json;
}

inline void
serialize_SearchRequest_Item_t_1_array_into(const arena_vector<SearchRequest_Item_t_1>& arr,
                                            std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Item_t_1_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_SearchRequest_Item_t_1_array(const arena_vector<SearchRequest_Item_t_1>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_SearchRequest_Item_t_1_array_into(arr, json);
    return json;
}

inline void serialize_SearchRequest_Created_after_t_array_into(
    const std::vector<SearchRequest_Created_after_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Created_after_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SearchRequest_Created_after_t_array(
    const std::vector<SearchRequest_Created_after_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_SearchRequest_Created_after_t_array_into(arr, json);
    return json;
}

inline void serialize_SearchRequest_Created_after_t_array_into(
    const arena_vector<SearchRequest_Created_after_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Created_after_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SearchRequest_Created_after_t_array(
    const arena_vector<SearchRequest_Created_after_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_SearchRequest_Created_after_t_array_into(arr, json);
    return json;
}

inline void serialize_SearchRequest_Created_before_t_array_into(
    const std::vector<SearchRequest_Created_before_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Created_before_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SearchRequest_Created_before_t_array(
    const std::vector<SearchRequest_Created_before_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_SearchRequest_Created_before_t_array_into(arr, json);
    return json;
}

inline void serialize_SearchRequest_Created_before_t_array_into(
    const arena_vector<SearchRequest_Created_before_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Created_before_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SearchRequest_Created_before_t_array(
    const arena_vector<SearchRequest_Created_before_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_SearchRequest_Created_before_t_array_into(arr, json);
    return json;
}

inline void serialize_SearchRequest_Has_assignee_t_array_into(
    const std::vector<SearchRequest_Has_assignee_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Has_assignee_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_SearchRequest_Has_assignee_t_array(const std::vector<SearchRequest_Has_assignee_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 5 + 2);
    serialize_SearchRequest_Has_assignee_t_array_into(arr, json);
    return json;
}

inline void serialize_SearchRequest_Has_assignee_t_array_into(
    const arena_vector<SearchRequest_Has_assignee_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_SearchRequest_Has_assignee_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SearchRequest_Has_assignee_t_array(
    const arena_vector<SearchRequest_Has_assignee_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 5 + 2);
    serialize_SearchRequest_Has_assignee_t_array_into(arr, json);
    return json;
}

inline void serialize_TaskList_array_into(const std::vector<TaskList>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_TaskList_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TaskList_array(const std::vector<TaskList>& arr) {
    std::string json;
    json.reserve(arr.size() * 121 + 2);
    serialize_TaskList_array_into(arr, json);
    return json;
}

inline void serialize_TaskList_array_into(const arena_vector<TaskList>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_TaskList_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TaskList_array(const arena_vector<TaskList>& arr) {
    std::string json;
    json.reserve(arr.size() * 121 + 2);
    serialize_TaskList_array_into(arr, json);
    return json;
}

inline void serialize_TaskList_Tasks_t_array_into(const std::vector<TaskList_Tasks_t>& arr,
                                                  std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_TaskList_Tasks_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TaskList_Tasks_t_array(const std::vector<TaskList_Tasks_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2714 + 2);
    serialize_TaskList_Tasks_t_array_into(arr, json);
    return json;
}

inline void serialize_TaskList_Tasks_t_array_into(const arena_vector<TaskList_Tasks_t>& arr,
                                                  std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_TaskList_Tasks_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TaskList_Tasks_t_array(const arena_vector<TaskList_Tasks_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2714 + 2);
    serialize_TaskList_Tasks_t_array_into(arr, json);
    return json;
}

inline void serialize_TaskList_Item_t_array_into(const std::vector<TaskList_Item_t>& arr,
                                                 std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_TaskList_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TaskList_Item_t_array(const std::vector<TaskList_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_TaskList_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_TaskList_Item_t_array_into(const arena_vector<TaskList_Item_t>& arr,
                                                 std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_TaskList_Item_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TaskList_Item_t_array(const arena_vector<TaskList_Item_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_TaskList_Item_t_array_into(arr, json);
    return json;
}

inline void serialize_TaskList_Total_t_array_into(const std::vector<TaskList_Total_t>& arr,
                                                  std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_TaskList_Total_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TaskList_Total_t_array(const std::vector<TaskList_Total_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_TaskList_Total_t_array_into(arr, json);
    return json;
}

inline void serialize_TaskList_Total_t_array_into(const arena_vector<TaskList_Total_t>& arr,
                                                  std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_TaskList_Total_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TaskList_Total_t_array(const arena_vector<TaskList_Total_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_TaskList_Total_t_array_into(arr, json);
    return json;
}

inline void serialize_TaskList_Has_more_t_array_into(const std::vector<TaskList_Has_more_t>& arr,
                                                     std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_TaskList_Has_more_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_TaskList_Has_more_t_array(const std::vector<TaskList_Has_more_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 5 + 2);
    serialize_TaskList_Has_more_t_array_into(arr, json);
    return json;
}

inline void serialize_TaskList_Has_more_t_array_into(const arena_vector<TaskList_Has_more_t>& arr,
                                                     std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_TaskList_Has_more_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_TaskList_Has_more_t_array(const arena_vector<TaskList_Has_more_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 5 + 2);
    serialize_TaskList_Has_more_t_array_into(arr, json);
    return json;
}

inline void serialize_HealthResponse_array_into(const std::vector<HealthResponse>& arr,
                                                std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_HealthResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_HealthResponse_array(const std::vector<HealthResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 165 + 2);
    serialize_HealthResponse_array_into(arr, json);
    return json;
}

inline void serialize_HealthResponse_array_into(const arena_vector<HealthResponse>& arr,
                                                std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_HealthResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_HealthResponse_array(const arena_vector<HealthResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 165 + 2);
    serialize_HealthResponse_array_into(arr, json);
    return json;
}

inline void
serialize_HealthResponse_Status_t_array_into(const std::vector<HealthResponse_Status_t>& arr,
                                             std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_HealthResponse_Status_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_HealthResponse_Status_t_array(const std::vector<HealthResponse_Status_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_HealthResponse_Status_t_array_into(arr, json);
    return json;
}

inline void
serialize_HealthResponse_Status_t_array_into(const arena_vector<HealthResponse_Status_t>& arr,
                                             std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_HealthResponse_Status_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_HealthResponse_Status_t_array(const arena_vector<HealthResponse_Status_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_HealthResponse_Status_t_array_into(arr, json);
    return json;
}

inline void
serialize_HealthResponse_Timestamp_t_array_into(const std::vector<HealthResponse_Timestamp_t>& arr,
                                                std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_HealthResponse_Timestamp_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_HealthResponse_Timestamp_t_array(const std::vector<HealthResponse_Timestamp_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_HealthResponse_Timestamp_t_array_into(arr, json);
    return json;
}

inline void
serialize_HealthResponse_Timestamp_t_array_into(const arena_vector<HealthResponse_Timestamp_t>& arr,
                                                std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_HealthResponse_Timestamp_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_HealthResponse_Timestamp_t_array(const arena_vector<HealthResponse_Timestamp_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_HealthResponse_Timestamp_t_array_into(arr, json);
    return json;
}

inline void serialize_HealthResponse_Uptime_seconds_t_array_into(
    const std::vector<HealthResponse_Uptime_seconds_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_HealthResponse_Uptime_seconds_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_HealthResponse_Uptime_seconds_t_array(
    const std::vector<HealthResponse_Uptime_seconds_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_HealthResponse_Uptime_seconds_t_array_into(arr, json);
    return json;
}

inline void serialize_HealthResponse_Uptime_seconds_t_array_into(
    const arena_vector<HealthResponse_Uptime_seconds_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_HealthResponse_Uptime_seconds_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_HealthResponse_Uptime_seconds_t_array(
    const arena_vector<HealthResponse_Uptime_seconds_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_HealthResponse_Uptime_seconds_t_array_into(arr, json);
    return json;
}

inline void serialize_HealthResponse_Total_requests_t_array_into(
    const std::vector<HealthResponse_Total_requests_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_HealthResponse_Total_requests_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_HealthResponse_Total_requests_t_array(
    const std::vector<HealthResponse_Total_requests_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_HealthResponse_Total_requests_t_array_into(arr, json);
    return json;
}

inline void serialize_HealthResponse_Total_requests_t_array_into(
    const arena_vector<HealthResponse_Total_requests_t>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_HealthResponse_Total_requests_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_HealthResponse_Total_requests_t_array(
    const arena_vector<HealthResponse_Total_requests_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_HealthResponse_Total_requests_t_array_into(arr, json);
    return json;
}

inline void serialize_ProblemDetails_array_into(const std::vector<ProblemDetails>& arr,
                                                std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_ProblemDetails_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ProblemDetails_array(const std::vector<ProblemDetails>& arr) {
    std::string json;
    json.reserve(arr.size() * 199 + 2);
    serialize_ProblemDetails_array_into(arr, json);
    return json;
}

inline void serialize_ProblemDetails_array_into(const arena_vector<ProblemDetails>& arr,
                                                std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_ProblemDetails_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ProblemDetails_array(const arena_vector<ProblemDetails>& arr) {
    std::string json;
    json.reserve(arr.size() * 199 + 2);
    serialize_ProblemDetails_array_into(arr, json);
    return json;
}

inline void
serialize_ProblemDetails_Type_t_array_into(const std::vector<ProblemDetails_Type_t>& arr,
                                           std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_ProblemDetails_Type_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_ProblemDetails_Type_t_array(const std::vector<ProblemDetails_Type_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ProblemDetails_Type_t_array_into(arr, json);
    return json;
}

inline void
serialize_ProblemDetails_Type_t_array_into(const arena_vector<ProblemDetails_Type_t>& arr,
                                           std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_ProblemDetails_Type_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_ProblemDetails_Type_t_array(const arena_vector<ProblemDetails_Type_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ProblemDetails_Type_t_array_into(arr, json);
    return json;
}

inline void
serialize_ProblemDetails_Title_t_array_into(const std::vector<ProblemDetails_Title_t>& arr,
                                            std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_ProblemDetails_Title_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_ProblemDetails_Title_t_array(const std::vector<ProblemDetails_Title_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ProblemDetails_Title_t_array_into(arr, json);
    return json;
}

inline void
serialize_ProblemDetails_Title_t_array_into(const arena_vector<ProblemDetails_Title_t>& arr,
                                            std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_ProblemDetails_Title_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_ProblemDetails_Title_t_array(const arena_vector<ProblemDetails_Title_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ProblemDetails_Title_t_array_into(arr, json);
    return json;
}

inline void
serialize_ProblemDetails_Status_t_array_into(const std::vector<ProblemDetails_Status_t>& arr,
                                             std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_ProblemDetails_Status_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_ProblemDetails_Status_t_array(const std::vector<ProblemDetails_Status_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_ProblemDetails_Status_t_array_into(arr, json);
    return json;
}

inline void
serialize_ProblemDetails_Status_t_array_into(const arena_vector<ProblemDetails_Status_t>& arr,
                                             std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_ProblemDetails_Status_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_ProblemDetails_Status_t_array(const arena_vector<ProblemDetails_Status_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_ProblemDetails_Status_t_array_into(arr, json);
    return json;
}

inline void
serialize_ProblemDetails_Detail_t_array_into(const std::vector<ProblemDetails_Detail_t>& arr,
                                             std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_ProblemDetails_Detail_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_ProblemDetails_Detail_t_array(const std::vector<ProblemDetails_Detail_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ProblemDetails_Detail_t_array_into(arr, json);
    return json;
}

inline void
serialize_ProblemDetails_Detail_t_array_into(const arena_vector<ProblemDetails_Detail_t>& arr,
                                             std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_ProblemDetails_Detail_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_ProblemDetails_Detail_t_array(const arena_vector<ProblemDetails_Detail_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ProblemDetails_Detail_t_array_into(arr, json);
    return json;
}

inline void
serialize_ProblemDetails_Instance_t_array_into(const std::vector<ProblemDetails_Instance_t>& arr,
                                               std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_ProblemDetails_Instance_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_ProblemDetails_Instance_t_array(const std::vector<ProblemDetails_Instance_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ProblemDetails_Instance_t_array_into(arr, json);
    return json;
}

inline void
serialize_ProblemDetails_Instance_t_array_into(const arena_vector<ProblemDetails_Instance_t>& arr,
                                               std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_ProblemDetails_Instance_t_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_ProblemDetails_Instance_t_array(const arena_vector<ProblemDetails_Instance_t>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ProblemDetails_Instance_t_array_into(arr, json);
    return json;
}

inline void
serialize_listTasks_param_status_array_into(const std::vector<listTasks_param_status>& arr,
                                            std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_listTasks_param_status_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_listTasks_param_status_array(const std::vector<listTasks_param_status>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_listTasks_param_status_array_into(arr, json);
    return json;
}

inline void
serialize_listTasks_param_status_array_into(const arena_vector<listTasks_param_status>& arr,
                                            std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_listTasks_param_status_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_listTasks_param_status_array(const arena_vector<listTasks_param_status>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_listTasks_param_status_array_into(arr, json);
    return json;
}

inline void
serialize_listTasks_param_priority_array_into(const std::vector<listTasks_param_priority>& arr,
                                              std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_listTasks_param_priority_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_listTasks_param_priority_array(const std::vector<listTasks_param_priority>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_listTasks_param_priority_array_into(arr, json);
    return json;
}

inline void
serialize_listTasks_param_priority_array_into(const arena_vector<listTasks_param_priority>& arr,
                                              std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_listTasks_param_priority_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_listTasks_param_priority_array(const arena_vector<listTasks_param_priority>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_listTasks_param_priority_array_into(arr, json);
    return json;
}

inline void
serialize_listTasks_param_limit_array_into(const std::vector<listTasks_param_limit>& arr,
                                           std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_listTasks_param_limit_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_listTasks_param_limit_array(const std::vector<listTasks_param_limit>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_listTasks_param_limit_array_into(arr, json);
    return json;
}

inline void
serialize_listTasks_param_limit_array_into(const arena_vector<listTasks_param_limit>& arr,
                                           std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_listTasks_param_limit_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_listTasks_param_limit_array(const arena_vector<listTasks_param_limit>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_listTasks_param_limit_array_into(arr, json);
    return json;
}

inline void
serialize_listTasks_param_offset_array_into(const std::vector<listTasks_param_offset>& arr,
                                            std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_listTasks_param_offset_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_listTasks_param_offset_array(const std::vector<listTasks_param_offset>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_listTasks_param_offset_array_into(arr, json);
    return json;
}

inline void
serialize_listTasks_param_offset_array_into(const arena_vector<listTasks_param_offset>& arr,
                                            std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_listTasks_param_offset_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_listTasks_param_offset_array(const arena_vector<listTasks_param_offset>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_listTasks_param_offset_array_into(arr, json);
    return json;
}

inline void serialize_schema_array_into(const std::vector<schema>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
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
        if (i > 0)
            json.push_back(',');
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
        if (i > 0)
            json.push_back(',');
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
        if (i > 0)
            json.push_back(',');
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
        if (i > 0)
            json.push_back(',');
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
        if (i > 0)
            json.push_back(',');
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
        if (i > 0)
            json.push_back(',');
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
        if (i > 0)
            json.push_back(',');
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

inline void serialize_getTask_param_id_array_into(const std::vector<getTask_param_id>& arr,
                                                  std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_getTask_param_id_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_getTask_param_id_array(const std::vector<getTask_param_id>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_getTask_param_id_array_into(arr, json);
    return json;
}

inline void serialize_getTask_param_id_array_into(const arena_vector<getTask_param_id>& arr,
                                                  std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_getTask_param_id_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_getTask_param_id_array(const arena_vector<getTask_param_id>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_getTask_param_id_array_into(arr, json);
    return json;
}

inline void serialize_schema_4_array_into(const std::vector<schema_4>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
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
        if (i > 0)
            json.push_back(',');
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

inline void serialize_schema_5_array_into(const std::vector<schema_5>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
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
        if (i > 0)
            json.push_back(',');
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

inline void serialize_updateTask_param_id_array_into(const std::vector<updateTask_param_id>& arr,
                                                     std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_updateTask_param_id_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_updateTask_param_id_array(const std::vector<updateTask_param_id>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_updateTask_param_id_array_into(arr, json);
    return json;
}

inline void serialize_updateTask_param_id_array_into(const arena_vector<updateTask_param_id>& arr,
                                                     std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_updateTask_param_id_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_updateTask_param_id_array(const arena_vector<updateTask_param_id>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_updateTask_param_id_array_into(arr, json);
    return json;
}

inline void serialize_schema_6_array_into(const std::vector<schema_6>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
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
        if (i > 0)
            json.push_back(',');
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

inline void serialize_schema_7_array_into(const std::vector<schema_7>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_7_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_7_array(const std::vector<schema_7>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_7_array_into(arr, json);
    return json;
}

inline void serialize_schema_7_array_into(const arena_vector<schema_7>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_7_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_7_array(const arena_vector<schema_7>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_7_array_into(arr, json);
    return json;
}

inline void serialize_schema_8_array_into(const std::vector<schema_8>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_8_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_8_array(const std::vector<schema_8>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_8_array_into(arr, json);
    return json;
}

inline void serialize_schema_8_array_into(const arena_vector<schema_8>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_8_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_8_array(const arena_vector<schema_8>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_8_array_into(arr, json);
    return json;
}

inline void serialize_deleteTask_param_id_array_into(const std::vector<deleteTask_param_id>& arr,
                                                     std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_deleteTask_param_id_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_deleteTask_param_id_array(const std::vector<deleteTask_param_id>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_deleteTask_param_id_array_into(arr, json);
    return json;
}

inline void serialize_deleteTask_param_id_array_into(const arena_vector<deleteTask_param_id>& arr,
                                                     std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_deleteTask_param_id_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string
serialize_deleteTask_param_id_array(const arena_vector<deleteTask_param_id>& arr) {
    std::string json;
    json.reserve(arr.size() * 20 + 2);
    serialize_deleteTask_param_id_array_into(arr, json);
    return json;
}

inline void serialize_schema_9_array_into(const std::vector<schema_9>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_9_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_9_array(const std::vector<schema_9>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_9_array_into(arr, json);
    return json;
}

inline void serialize_schema_9_array_into(const arena_vector<schema_9>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_9_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_9_array(const arena_vector<schema_9>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_9_array_into(arr, json);
    return json;
}

inline void serialize_schema_10_array_into(const std::vector<schema_10>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_10_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_10_array(const std::vector<schema_10>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_10_array_into(arr, json);
    return json;
}

inline void serialize_schema_10_array_into(const arena_vector<schema_10>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_10_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_10_array(const arena_vector<schema_10>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_10_array_into(arr, json);
    return json;
}

inline void serialize_schema_11_array_into(const std::vector<schema_11>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_11_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_11_array(const std::vector<schema_11>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_11_array_into(arr, json);
    return json;
}

inline void serialize_schema_11_array_into(const arena_vector<schema_11>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_11_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_11_array(const arena_vector<schema_11>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_11_array_into(arr, json);
    return json;
}

inline void serialize_schema_12_array_into(const std::vector<schema_12>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_12_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_12_array(const std::vector<schema_12>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_12_array_into(arr, json);
    return json;
}

inline void serialize_schema_12_array_into(const arena_vector<schema_12>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_12_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_12_array(const arena_vector<schema_12>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_12_array_into(arr, json);
    return json;
}

inline void serialize_schema_13_array_into(const std::vector<schema_13>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_13_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_13_array(const std::vector<schema_13>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_13_array_into(arr, json);
    return json;
}

inline void serialize_schema_13_array_into(const arena_vector<schema_13>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_13_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_13_array(const arena_vector<schema_13>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_13_array_into(arr, json);
    return json;
}

inline void serialize_schema_14_array_into(const std::vector<schema_14>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_14_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_14_array(const std::vector<schema_14>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_14_array_into(arr, json);
    return json;
}

inline void serialize_schema_14_array_into(const arena_vector<schema_14>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0)
            json.push_back(',');
        serialize_schema_14_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_schema_14_array(const arena_vector<schema_14>& arr) {
    std::string json;
    json.reserve(arr.size() * 2 + 2);
    serialize_schema_14_array_into(arr, json);
    return json;
}
