#pragma once

#include "result.hpp"
#include "problem.hpp"
#include "http_headers.hpp"

#include <string>
#include <string_view>
#include <optional>
#include <span>

namespace katana::http {

// Security limits for HTTP parsing
constexpr size_t MAX_HEADER_SIZE = 8192UL;
constexpr size_t MAX_BODY_SIZE = 10UL * 1024UL * 1024UL;
constexpr size_t MAX_URI_LENGTH = 2048UL;
constexpr size_t MAX_HEADER_COUNT = 100;  // Max number of headers
constexpr size_t MAX_BUFFER_SIZE = MAX_HEADER_SIZE + MAX_BODY_SIZE;  // Total buffer limit

enum class method : uint8_t {
    get,
    post,
    put,
    del,
    patch,
    head,
    options,
    unknown
};

struct request {
    method http_method = method::unknown;
    std::string uri;
    std::string version;
    headers_map headers;
    std::string body;

    request() = default;
    request(request&&) noexcept = default;
    request& operator=(request&&) noexcept = default;
    request(const request&) = default;
    request& operator=(const request&) = default;

    [[nodiscard]] std::optional<std::string_view> header(std::string_view name) const {
        return headers.get(name);
    }
};

struct response {
    int32_t status = 200;
    std::string reason;
    headers_map headers;
    std::string body;
    bool chunked = false;

    response() = default;
    response(response&&) noexcept = default;
    response& operator=(response&&) noexcept = default;
    response(const response&) = default;
    response& operator=(const response&) = default;

    void set_header(std::string name, std::string value) {
        headers.set(std::move(name), std::move(value));
    }

    [[nodiscard]] std::string serialize() const;
    [[nodiscard]] std::string serialize_chunked(size_t chunk_size = 4096) const;

    static response ok(std::string body = "", std::string content_type = "text/plain");
    static response json(std::string body);
    static response error(const problem_details& problem);
};

class parser {
public:
    parser() = default;

    enum class state : uint8_t {
        request_line,
        headers,
        body,
        chunk_size,
        chunk_data,
        chunk_trailer,
        complete
    };

    result<state> parse(std::span<const uint8_t> data);

    [[nodiscard]] bool is_complete() const noexcept { return state_ == state::complete; }
    [[nodiscard]] const request& get_request() const noexcept { return request_; }
    request&& take_request() { return std::move(request_); }
    [[nodiscard]] const std::string& buffer() const noexcept { return buffer_; }

private:
    result<void> parse_request_line(std::string_view line);
    result<void> parse_header_line(std::string_view line);
    void compact_buffer();

    state state_ = state::request_line;
    request request_;
    std::string buffer_;
    std::string chunked_body_;
    std::string last_header_name_;
    size_t parse_pos_ = 0;
    size_t content_length_ = 0;
    size_t current_chunk_size_ = 0;
    size_t header_count_ = 0;
    bool is_chunked_ = false;

    static constexpr size_t COMPACT_THRESHOLD = 4096;
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
