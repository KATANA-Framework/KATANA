#pragma once

#include "generated_dtos.hpp"
#include "katana/core/http.hpp"
#include "katana/core/router.hpp"
#include <optional>
#include <string_view>

using katana::http::request;
using katana::http::request_context;
using katana::http::response;

namespace generated {

// Base handler interface for all API operations
struct api_handler {
    virtual ~api_handler() = default;

    // GET /tasks
    // Get all tasks
    virtual response list_tasks(const request& req, request_context& ctx) = 0;

    // POST /tasks
    // Create a new task
    virtual response
    create_task(const request& req, request_context& ctx, const CreateTaskRequest& body) = 0;

    // GET /tasks/{id}
    // Get a task by ID
    virtual response get_task(const request& req, request_context& ctx, std::string_view id) = 0;

    // PUT /tasks/{id}
    // Update a task
    virtual response update_task(const request& req,
                                 request_context& ctx,
                                 std::string_view id,
                                 const UpdateTaskRequest& body) = 0;

    // DELETE /tasks/{id}
    // Delete a task
    virtual response delete_task(const request& req, request_context& ctx, std::string_view id) = 0;
};

} // namespace generated
