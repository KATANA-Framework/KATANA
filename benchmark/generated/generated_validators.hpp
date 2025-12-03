#pragma once

#include "generated_dtos.hpp"
#include <cmath>
#include <optional>
#include <regex>
#include <set>
#include <string>
#include <string_view>

struct UserInput_validation_error {
    std::string_view field;
    std::string_view message;
};

inline std::optional<UserInput_validation_error> validate_UserInput(const UserInput& obj) {
    if (obj.name.empty()) {
        return UserInput_validation_error{"name", "required field is missing"};
    }
    if (obj.name.size() < 1) {
        return UserInput_validation_error{"name", "string too short (min: 1)"};
    }
    if (obj.email.empty()) {
        return UserInput_validation_error{"email", "required field is missing"};
    }
    {
        static const std::regex pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
        if (!std::regex_match(std::string(obj.email.begin(), obj.email.end()), pattern)) {
            return UserInput_validation_error{"email", "invalid format: email"};
        }
    }
    if (obj.age < 0) {
        return UserInput_validation_error{"age", "value too small (min: 0)"};
    }
    return std::nullopt;
}
