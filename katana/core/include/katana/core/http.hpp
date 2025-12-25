#pragma once

#include "arena.hpp"
#include "http_headers.hpp"
#include "io_buffer.hpp"
#include "problem.hpp"
#include "result.hpp"

#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace katana::http {

// Security limits for HTTP parsing
constexpr size_t MAX_HEADER_SIZE = 8192UL;
constexpr size_t MAX_BODY_SIZE = 10UL * 1024UL * 1024UL;
constexpr size_t MAX_URI_LENGTH = 2048UL;
constexpr size_t MAX_HEADER_COUNT = 100;
constexpr size_t MAX_BUFFER_SIZE = MAX_HEADER_SIZE + MAX_BODY_SIZE;

enum class method : uint8_t { get, post, put, del, patch, head, options, unknown };

struct request {
    method http_method = method::unknown;
    std::string_view uri;
    headers_map headers;
    std::string_view body;

    request() = default;
    request(request&&) noexcept = default;
    request& operator=(request&&) noexcept = default;

    request(const request&) = delete;
    request& operator=(const request&) = delete;

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

    response() : headers(nullptr) {}
    response(response&&) noexcept = default;
    response& operator=(response&&) noexcept = default;
    response(const response&) = delete;
    response& operator=(const response&) = delete;

    void set_header(std::string_view name, std::string_view value) {
        headers.set_view(name, value);
    }

    // Optimized: avoid string_to_field() lookup
    void set_header(http::field f, std::string_view value) { headers.set_known(f, value); }

    // Fluent interface for building responses (lvalue overloads)
    response& header(std::string_view name, std::string_view value) & {
        set_header(name, value);
        return *this;
    }

    response& with_status(int32_t s) & {
        status = s;
        return *this;
    }

    response& with_body(std::string b) & {
        body = std::move(b);
        return *this;
    }

    response& content_type(std::string_view ct) & { return header("Content-Type", ct); }

    // Fluent interface for building responses (rvalue overloads for chaining)
    response&& header(std::string_view name, std::string_view value) && {
        set_header(name, value);
        return std::move(*this);
    }

    response&& with_status(int32_t s) && {
        status = s;
        return std::move(*this);
    }

    response&& with_body(std::string b) && {
        body = std::move(b);
        return std::move(*this);
    }

    response&& content_type(std::string_view ct) && {
        set_header("Content-Type", ct);
        return std::move(*this);
    }

    void serialize_into(std::string& out) const;
    void serialize_into(io_buffer& out) const;
    [[nodiscard]] std::string serialize() const;
    [[nodiscard]] std::string serialize_chunked(size_t chunk_size = 4096) const;

    static response ok(std::string body = "", std::string content_type = "text/plain");
    static response json(std::string body);
    static response error(const problem_details& problem);
};

class parser {
public:
    explicit parser(monotonic_arena* arena) noexcept : arena_(arena), request_{} {
        request_.headers = headers_map(arena);
        buffer_ = static_cast<char*>(arena_->allocate(MAX_BUFFER_SIZE, 1));
    }

    enum class state : uint8_t {
        request_line,
        headers,
        body,
        chunk_size,
        chunk_data,
        chunk_trailer,
        complete
    };

    [[nodiscard]] result<state> parse(std::span<const uint8_t> data);

    [[nodiscard]] bool is_complete() const noexcept { return state_ == state::complete; }
    [[nodiscard]] const request& get_request() const noexcept { return request_; }
    [[nodiscard]] size_t bytes_parsed() const noexcept { return parse_pos_; }
    request&& take_request() { return std::move(request_); }
    void reset(monotonic_arena* arena) noexcept;

private:
    result<state> parse_request_line_state();
    result<state> parse_headers_state();
    result<state> parse_body_state();
    result<state> parse_chunk_size_state();
    result<state> parse_chunk_data_state();
    result<state> parse_chunk_trailer_state();

    result<void> process_request_line(std::string_view line);
    result<void> process_header_line(std::string_view line);
    void compact_buffer();

    monotonic_arena* arena_;
    state state_ = state::request_line;
    request request_;
    char* buffer_;
    size_t buffer_size_ = 0;
    size_t buffer_capacity_ = MAX_BUFFER_SIZE;
    char* chunked_body_ = nullptr;
    size_t chunked_body_size_ = 0;
    field last_header_field_ = field::unknown;
    const char* last_header_name_ = nullptr;
    size_t last_header_name_len_ = 0;
    size_t parse_pos_ = 0;
    size_t content_length_ = 0;
    size_t current_chunk_size_ = 0;
    size_t header_count_ = 0;
    bool is_chunked_ = false;

    static constexpr size_t COMPACT_THRESHOLD = 2048;
};

method parse_method(std::string_view str);
std::string_view method_to_string(method m);

inline std::span<const uint8_t> as_bytes(std::string_view sv) noexcept {
    return std::span<const uint8_t>(
        static_cast<const uint8_t*>(static_cast<const void*>(sv.data())), sv.size());
}

} // namespace katana::http
