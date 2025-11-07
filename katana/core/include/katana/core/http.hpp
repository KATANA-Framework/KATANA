#pragma once

#include "result.hpp"
#include "problem.hpp"

#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>
#include <span>

namespace katana::http {

constexpr size_t MAX_HEADER_SIZE = 8192;
constexpr size_t MAX_BODY_SIZE = 10 * 1024 * 1024;
constexpr size_t MAX_URI_LENGTH = 2048;

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
    bool chunked = false;

    void set_header(std::string name, std::string value);
    std::string serialize() const;
    std::string serialize_chunked(size_t chunk_size = 4096) const;

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
        chunk_size,
        chunk_data,
        chunk_trailer,
        complete
    };

    result<state> parse(std::span<const uint8_t> data);

    bool is_complete() const noexcept { return state_ == state::complete; }
    const request& get_request() const noexcept { return request_; }
    request&& take_request() { return std::move(request_); }

private:
    result<void> parse_request_line(std::string_view line);
    result<void> parse_header_line(std::string_view line);

    state state_ = state::request_line;
    request request_;
    std::string buffer_;
    size_t content_length_ = 0;
    size_t current_chunk_size_ = 0;
    bool is_chunked_ = false;
};

method parse_method(std::string_view str);
std::string_view method_to_string(method m);

inline std::span<const uint8_t> as_bytes(std::string_view sv) noexcept {
    return std::span<const uint8_t>(
        static_cast<const uint8_t*>(static_cast<const void*>(sv.data())),
        sv.size()
    );
}

} // namespace katana::http
