#include "katana/core/http.hpp"

#include <sstream>
#include <algorithm>

namespace katana::http {

method parse_method(std::string_view str) {
    if (str == "GET") return method::GET;
    if (str == "POST") return method::POST;
    if (str == "PUT") return method::PUT;
    if (str == "DELETE") return method::DELETE;
    if (str == "PATCH") return method::PATCH;
    if (str == "HEAD") return method::HEAD;
    if (str == "OPTIONS") return method::OPTIONS;
    return method::UNKNOWN;
}

std::string_view method_to_string(method m) {
    switch (m) {
        case method::GET: return "GET";
        case method::POST: return "POST";
        case method::PUT: return "PUT";
        case method::DELETE: return "DELETE";
        case method::PATCH: return "PATCH";
        case method::HEAD: return "HEAD";
        case method::OPTIONS: return "OPTIONS";
        default: return "UNKNOWN";
    }
}

std::optional<std::string_view> request::header(std::string_view name) const {
    auto it = headers.find(std::string(name));
    if (it != headers.end()) {
        return it->second;
    }
    return std::nullopt;
}

void response::set_header(std::string name, std::string value) {
    headers[std::move(name)] = std::move(value);
}

std::string response::serialize() const {
    std::ostringstream oss;

    oss << "HTTP/1.1 " << status << " " << reason << "\r\n";

    for (const auto& [name, value] : headers) {
        oss << name << ": " << value << "\r\n";
    }

    oss << "\r\n";

    if (!body.empty()) {
        oss << body;
    }

    return oss.str();
}

response response::ok(std::string body, std::string content_type) {
    response res;
    res.status = 200;
    res.reason = "OK";
    res.body = std::move(body);
    res.set_header("Content-Length", std::to_string(res.body.size()));
    res.set_header("Content-Type", std::move(content_type));
    return res;
}

response response::json(std::string body) {
    return ok(std::move(body), "application/json");
}

response response::error(const problem_details& problem) {
    response res;
    res.status = problem.status;
    res.reason = problem.title;
    res.body = problem.to_json();
    res.set_header("Content-Length", std::to_string(res.body.size()));
    res.set_header("Content-Type", "application/problem+json");
    return res;
}

result<parser::state> parser::parse(std::span<const uint8_t> data) {
    buffer_.append(reinterpret_cast<const char*>(data.data()), data.size());

    while (state_ != state::complete) {
        if (state_ == state::request_line || state_ == state::headers) {
            auto pos = buffer_.find("\r\n");
            if (pos == std::string::npos) {
                return state_;
            }

            std::string line = buffer_.substr(0, pos);
            buffer_.erase(0, pos + 2);

            if (state_ == state::request_line) {
                auto res = parse_request_line(line);
                if (!res) {
                    return std::unexpected(res.error());
                }
                state_ = state::headers;
            } else {
                if (line.empty()) {
                    auto cl = request_.header("Content-Length");
                    if (cl) {
                        content_length_ = std::stoull(std::string(*cl));
                        state_ = state::body;
                    } else {
                        state_ = state::complete;
                    }
                } else {
                    auto res = parse_header_line(line);
                    if (!res) {
                        return std::unexpected(res.error());
                    }
                }
            }
        } else if (state_ == state::body) {
            if (buffer_.size() >= content_length_) {
                request_.body = buffer_.substr(0, content_length_);
                buffer_.erase(0, content_length_);
                state_ = state::complete;
            } else {
                return state_;
            }
        }
    }

    return state_;
}

result<void> parser::parse_request_line(std::string_view line) {
    auto method_end = line.find(' ');
    if (method_end == std::string_view::npos) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    request_.http_method = parse_method(line.substr(0, method_end));

    auto uri_start = method_end + 1;
    auto uri_end = line.find(' ', uri_start);
    if (uri_end == std::string_view::npos) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    request_.uri = std::string(line.substr(uri_start, uri_end - uri_start));
    request_.version = std::string(line.substr(uri_end + 1));

    return {};
}

result<void> parser::parse_header_line(std::string_view line) {
    auto colon = line.find(':');
    if (colon == std::string_view::npos) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    auto name = line.substr(0, colon);
    auto value = line.substr(colon + 1);

    while (!value.empty() && value.front() == ' ') {
        value.remove_prefix(1);
    }

    request_.headers[std::string(name)] = std::string(value);
    return {};
}

} // namespace katana::http
