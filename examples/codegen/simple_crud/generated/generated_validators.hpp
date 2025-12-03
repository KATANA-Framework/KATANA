#pragma once

#include "generated_dtos.hpp"
#include <cmath>
#include <optional>
#include <string>
#include <string_view>

struct Task_validation_error {
    std::string_view field;
    std::string_view message;
};

inline std::optional<Task_validation_error> validate_Task(const Task& obj) {
    if (obj.title.empty()) {
        return Task_validation_error{"title", "required field is missing"};
    }
    if (!obj.title.empty() && obj.title.size() < 1) {
        return Task_validation_error{"title", "string too short (min: 1)"};
    }
    if (obj.title.size() > 200) {
        return Task_validation_error{"title", "string too long (max: 200)"};
    }
    if (obj.description.size() > 1000) {
        return Task_validation_error{"description", "string too long (max: 1000)"};
    }
    return std::nullopt;
}

struct CreateTaskRequest_validation_error {
    std::string_view field;
    std::string_view message;
};

inline std::optional<CreateTaskRequest_validation_error>
validate_CreateTaskRequest(const CreateTaskRequest& obj) {
    if (obj.title.empty()) {
        return CreateTaskRequest_validation_error{"title", "required field is missing"};
    }
    if (!obj.title.empty() && obj.title.size() < 1) {
        return CreateTaskRequest_validation_error{"title", "string too short (min: 1)"};
    }
    if (obj.title.size() > 200) {
        return CreateTaskRequest_validation_error{"title", "string too long (max: 200)"};
    }
    if (obj.description.size() > 1000) {
        return CreateTaskRequest_validation_error{"description", "string too long (max: 1000)"};
    }
    return std::nullopt;
}

struct UpdateTaskRequest_validation_error {
    std::string_view field;
    std::string_view message;
};

inline std::optional<UpdateTaskRequest_validation_error>
validate_UpdateTaskRequest(const UpdateTaskRequest& obj) {
    if (!obj.title.empty() && obj.title.size() < 1) {
        return UpdateTaskRequest_validation_error{"title", "string too short (min: 1)"};
    }
    if (obj.title.size() > 200) {
        return UpdateTaskRequest_validation_error{"title", "string too long (max: 200)"};
    }
    if (obj.description.size() > 1000) {
        return UpdateTaskRequest_validation_error{"description", "string too long (max: 1000)"};
    }
    return std::nullopt;
}
