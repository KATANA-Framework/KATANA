#include "katana/core/problem.hpp"

#include <sstream>

namespace katana {

std::string problem_details::to_json() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"type\":\"" << type << "\"";
    oss << ",\"title\":\"" << title << "\"";
    oss << ",\"status\":" << status;

    if (detail) {
        oss << ",\"detail\":\"" << *detail << "\"";
    }

    if (instance) {
        oss << ",\"instance\":\"" << *instance << "\"";
    }

    for (const auto& [key, value] : extensions) {
        oss << ",\"" << key << "\":\"" << value << "\"";
    }

    oss << "}";
    return oss.str();
}

problem_details problem_details::bad_request(std::string_view detail) {
    problem_details p;
    p.status = 400;
    p.title = "Bad Request";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::unauthorized(std::string_view detail) {
    problem_details p;
    p.status = 401;
    p.title = "Unauthorized";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::forbidden(std::string_view detail) {
    problem_details p;
    p.status = 403;
    p.title = "Forbidden";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::not_found(std::string_view detail) {
    problem_details p;
    p.status = 404;
    p.title = "Not Found";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::method_not_allowed(std::string_view detail) {
    problem_details p;
    p.status = 405;
    p.title = "Method Not Allowed";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::not_acceptable(std::string_view detail) {
    problem_details p;
    p.status = 406;
    p.title = "Not Acceptable";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::unsupported_media_type(std::string_view detail) {
    problem_details p;
    p.status = 415;
    p.title = "Unsupported Media Type";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::conflict(std::string_view detail) {
    problem_details p;
    p.status = 409;
    p.title = "Conflict";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::unprocessable_entity(std::string_view detail) {
    problem_details p;
    p.status = 422;
    p.title = "Unprocessable Entity";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::internal_server_error(std::string_view detail) {
    problem_details p;
    p.status = 500;
    p.title = "Internal Server Error";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::service_unavailable(std::string_view detail) {
    problem_details p;
    p.status = 503;
    p.title = "Service Unavailable";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

} // namespace katana
