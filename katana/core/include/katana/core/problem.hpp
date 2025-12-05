#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace katana {

struct problem_details {
    std::string type = "about:blank";
    std::string title;
    int status = 500;
    std::optional<std::string> detail;
    std::optional<std::string> instance;
    std::unordered_map<std::string, std::string> extensions;

    problem_details() = default;
    problem_details(problem_details&&) noexcept = default;
    problem_details& operator=(problem_details&&) noexcept = default;
    problem_details(const problem_details&) = default;
    problem_details& operator=(const problem_details&) = default;

    std::string to_json() const;

    static problem_details bad_request(std::string_view detail = "");
    static problem_details unauthorized(std::string_view detail = "");
    static problem_details forbidden(std::string_view detail = "");
    static problem_details not_found(std::string_view detail = "");
    static problem_details method_not_allowed(std::string_view detail = "");
    static problem_details not_acceptable(std::string_view detail = "");
    static problem_details unsupported_media_type(std::string_view detail = "");
    static problem_details conflict(std::string_view detail = "");
    static problem_details unprocessable_entity(std::string_view detail = "");
    static problem_details internal_server_error(std::string_view detail = "");
    static problem_details service_unavailable(std::string_view detail = "");
};

} // namespace katana
