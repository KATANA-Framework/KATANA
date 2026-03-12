// layer: flat
#pragma once

#include "katana/core/http.hpp"
#include "katana/core/http_utils.hpp"
#include "katana/core/router.hpp"
#include <array>
#include <span>
#include <string_view>

namespace generated {

using katana::http_utils::content_type_info;

struct route_entry {
    std::string_view path;
    katana::http::method method;
    std::string_view operation_id;
    std::span<const content_type_info> consumes;
    std::span<const content_type_info> produces;
};

inline constexpr content_type_info route_0_produces[] = {
    {"application/json"},
};

inline constexpr content_type_info route_1_consumes[] = {
    {"application/json"},
};

inline constexpr content_type_info route_1_produces[] = {
    {"application/json"},
};

inline constexpr content_type_info route_2_produces[] = {
    {"application/json"},
};

inline constexpr content_type_info route_3_consumes[] = {
    {"application/json"},
};

inline constexpr content_type_info route_3_produces[] = {
    {"application/json"},
};

inline constexpr content_type_info route_4_produces[] = {
    {"application/json"},
};

inline constexpr content_type_info route_5_consumes[] = {
    {"application/json"},
};

inline constexpr content_type_info route_5_produces[] = {
    {"application/json"},
};

inline constexpr content_type_info route_6_consumes[] = {
    {"application/json"},
};

inline constexpr content_type_info route_6_produces[] = {
    {"application/json"},
};

inline constexpr content_type_info route_7_produces[] = {
    {"application/json"},
};

inline constexpr route_entry routes[] = {
    {"/tasks", katana::http::method::get, "listTasks", {}, route_0_produces},
    {"/tasks", katana::http::method::post, "createTask", route_1_consumes, route_1_produces},
    {"/tasks/{id}", katana::http::method::get, "getTask", {}, route_2_produces},
    {"/tasks/{id}", katana::http::method::put, "updateTask", route_3_consumes, route_3_produces},
    {"/tasks/{id}", katana::http::method::del, "deleteTask", {}, route_4_produces},
    {"/tasks/batch",
     katana::http::method::post,
     "batchCreateTasks",
     route_5_consumes,
     route_5_produces},
    {"/tasks/search",
     katana::http::method::post,
     "searchTasks",
     route_6_consumes,
     route_6_produces},
    {"/health", katana::http::method::get, "healthCheck", {}, route_7_produces},
};

inline constexpr size_t route_count = sizeof(routes) / sizeof(routes[0]);

// Compile-time route metadata for type safety
namespace route_metadata {
// listTasks: GET /tasks
struct listTasks_metadata {
    static constexpr std::string_view path = "/tasks";
    static constexpr katana::http::method method = katana::http::method::get;
    static constexpr std::string_view operation_id = "listTasks";
    static constexpr size_t path_param_count = 0;
    static constexpr bool has_request_body = false;
};

// createTask: POST /tasks
struct createTask_metadata {
    static constexpr std::string_view path = "/tasks";
    static constexpr katana::http::method method = katana::http::method::post;
    static constexpr std::string_view operation_id = "createTask";
    static constexpr size_t path_param_count = 0;
    static constexpr bool has_request_body = true;
};

// getTask: GET /tasks/{id}
struct getTask_metadata {
    static constexpr std::string_view path = "/tasks/{id}";
    static constexpr katana::http::method method = katana::http::method::get;
    static constexpr std::string_view operation_id = "getTask";
    static constexpr size_t path_param_count = 1;
    static constexpr bool has_request_body = false;
};

// updateTask: PUT /tasks/{id}
struct updateTask_metadata {
    static constexpr std::string_view path = "/tasks/{id}";
    static constexpr katana::http::method method = katana::http::method::put;
    static constexpr std::string_view operation_id = "updateTask";
    static constexpr size_t path_param_count = 1;
    static constexpr bool has_request_body = true;
};

// deleteTask: DELETE /tasks/{id}
struct deleteTask_metadata {
    static constexpr std::string_view path = "/tasks/{id}";
    static constexpr katana::http::method method = katana::http::method::del;
    static constexpr std::string_view operation_id = "deleteTask";
    static constexpr size_t path_param_count = 1;
    static constexpr bool has_request_body = false;
};

// batchCreateTasks: POST /tasks/batch
struct batchCreateTasks_metadata {
    static constexpr std::string_view path = "/tasks/batch";
    static constexpr katana::http::method method = katana::http::method::post;
    static constexpr std::string_view operation_id = "batchCreateTasks";
    static constexpr size_t path_param_count = 0;
    static constexpr bool has_request_body = true;
};

// searchTasks: POST /tasks/search
struct searchTasks_metadata {
    static constexpr std::string_view path = "/tasks/search";
    static constexpr katana::http::method method = katana::http::method::post;
    static constexpr std::string_view operation_id = "searchTasks";
    static constexpr size_t path_param_count = 0;
    static constexpr bool has_request_body = true;
};

// healthCheck: GET /health
struct healthCheck_metadata {
    static constexpr std::string_view path = "/health";
    static constexpr katana::http::method method = katana::http::method::get;
    static constexpr std::string_view operation_id = "healthCheck";
    static constexpr size_t path_param_count = 0;
    static constexpr bool has_request_body = false;
};

} // namespace route_metadata

// Compile-time validations
static_assert(route_count > 0, "At least one route must be defined");
} // namespace generated
