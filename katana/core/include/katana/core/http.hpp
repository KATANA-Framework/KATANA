#pragma once

#include "result.hpp"
#include "problem.hpp"

#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>
#include <span>

namespace katana::http {

enum class method {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
    HEAD,
    OPTIONS,
    UNKNOWN
};

struct request {
    method http_method = method::UNKNOWN;
    std::string uri;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    std::optional<std::string_view> header(std::string_view name) const;
};

struct response {
    int status = 200;
    std::string reason;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    void set_header(std::string name, std::string value);
    std::string serialize() const;

    static response ok(std::string body = "", std::string content_type = "text/plain");
    static response json(std::string body);
    static response error(const problem_details& problem);
};

class parser {
public:
    parser() = default;

    enum class state {
        request_line,
        headers,
        body,
        complete
    };

    result<state> parse(std::span<const uint8_t> data);

    bool is_complete() const noexcept { return state_ == state::complete; }
    request&& take_request() { return std::move(request_); }

private:
    result<void> parse_request_line(std::string_view line);
    result<void> parse_header_line(std::string_view line);

    state state_ = state::request_line;
    request request_;
    std::string buffer_;
    size_t content_length_ = 0;
};

method parse_method(std::string_view str);
std::string_view method_to_string(method m);

} // namespace katana::http
