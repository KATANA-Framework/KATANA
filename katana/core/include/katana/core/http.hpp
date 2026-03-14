#pragma once

#include "arena.hpp"
#include "http_headers.hpp"
#include "io_buffer.hpp"
#include "problem.hpp"
#include "result.hpp"

#include <memory>
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

[[nodiscard]] std::string_view canonical_reason_phrase(int32_t status) noexcept;

struct response {
    int32_t status = 200;
    std::string reason;
    headers_map headers;
    std::string body;
    bool chunked = false;

    response() : headers(nullptr) {}
    explicit response(monotonic_arena* arena) : headers(arena) {}
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
        reason.assign(canonical_reason_phrase(s));
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
        reason.assign(canonical_reason_phrase(s));
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
    void serialize_head_into(std::string& out) const;
    [[nodiscard]] std::string serialize() const;
    [[nodiscard]] std::string serialize_chunked(size_t chunk_size = 4096) const;
    void reset() noexcept;
    void assign_text(std::string body,
                     std::string_view content_type = "text/plain",
                     int32_t status_code = 200,
                     std::string_view reason_phrase = "OK");
    void
    assign_json(std::string body, int32_t status_code = 200, std::string_view reason_phrase = "OK");
    void assign_error(const problem_details& problem);

    static response ok(std::string body = "", std::string content_type = "text/plain");
    static response json(std::string body);
    static response error(const problem_details& problem);
};

class response_builder {
public:
    explicit response_builder(response& out) noexcept : out_(out) {}

    response_builder& text(std::string body, int32_t status_code = 200);
    response_builder& json(std::string body, int32_t status_code = 200);
    response_builder& problem(const problem_details& problem);
    response_builder& created_json(std::string body);
    response_builder& no_content();

    response_builder& header(std::string_view name, std::string_view value) {
        out_.set_header(name, value);
        return *this;
    }

    response_builder& header(http::field field_name, std::string_view value) {
        out_.set_header(field_name, value);
        return *this;
    }

    [[nodiscard]] response& done() noexcept { return out_; }

private:
    response& out_;
};

namespace respond {

[[nodiscard]] inline response_builder into(response& out) noexcept {
    return response_builder(out);
}

} // namespace respond

class parser {
public:
    explicit parser(monotonic_arena* arena) noexcept : arena_(arena), request_{} {
        request_.headers = headers_map(arena);
        reserve_buffer(INITIAL_BUFFER_CAPACITY);
    }

    parser(parser&& other) noexcept
        : arena_(other.arena_), state_(other.state_), request_(std::move(other.request_)),
          buffer_owner_(std::move(other.buffer_owner_)), buffer_(buffer_owner_.get()),
          buffer_size_(other.buffer_size_), buffer_capacity_(other.buffer_capacity_),
          chunked_body_(other.chunked_body_), chunked_body_size_(other.chunked_body_size_),
          last_header_field_(other.last_header_field_), last_header_name_(other.last_header_name_),
          last_header_name_len_(other.last_header_name_len_), parse_pos_(other.parse_pos_),
          content_length_(other.content_length_), current_chunk_size_(other.current_chunk_size_),
          header_count_(other.header_count_), validated_bytes_(other.validated_bytes_),
          header_end_pos_(other.header_end_pos_), crlf_scan_pos_(other.crlf_scan_pos_),
          crlf_pairs_(other.crlf_pairs_), is_chunked_(other.is_chunked_) {
        other.buffer_ = nullptr;
        other.buffer_size_ = 0;
        other.buffer_capacity_ = 0;
        other.chunked_body_ = nullptr;
        other.chunked_body_size_ = 0;
        other.last_header_field_ = field::unknown;
        other.last_header_name_ = nullptr;
        other.last_header_name_len_ = 0;
        other.parse_pos_ = 0;
        other.content_length_ = 0;
        other.current_chunk_size_ = 0;
        other.header_count_ = 0;
        other.validated_bytes_ = 0;
        other.header_end_pos_ = 0;
        other.crlf_scan_pos_ = 0;
        other.crlf_pairs_ = 0;
        other.is_chunked_ = false;
    }

    parser& operator=(parser&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        arena_ = other.arena_;
        state_ = other.state_;
        request_ = std::move(other.request_);
        buffer_owner_ = std::move(other.buffer_owner_);
        buffer_ = buffer_owner_.get();
        buffer_size_ = other.buffer_size_;
        buffer_capacity_ = other.buffer_capacity_;
        chunked_body_ = other.chunked_body_;
        chunked_body_size_ = other.chunked_body_size_;
        last_header_field_ = other.last_header_field_;
        last_header_name_ = other.last_header_name_;
        last_header_name_len_ = other.last_header_name_len_;
        parse_pos_ = other.parse_pos_;
        content_length_ = other.content_length_;
        current_chunk_size_ = other.current_chunk_size_;
        header_count_ = other.header_count_;
        validated_bytes_ = other.validated_bytes_;
        header_end_pos_ = other.header_end_pos_;
        crlf_scan_pos_ = other.crlf_scan_pos_;
        crlf_pairs_ = other.crlf_pairs_;
        is_chunked_ = other.is_chunked_;

        other.buffer_ = nullptr;
        other.buffer_size_ = 0;
        other.buffer_capacity_ = 0;
        other.chunked_body_ = nullptr;
        other.chunked_body_size_ = 0;
        other.last_header_field_ = field::unknown;
        other.last_header_name_ = nullptr;
        other.last_header_name_len_ = 0;
        other.parse_pos_ = 0;
        other.content_length_ = 0;
        other.current_chunk_size_ = 0;
        other.header_count_ = 0;
        other.validated_bytes_ = 0;
        other.header_end_pos_ = 0;
        other.crlf_scan_pos_ = 0;
        other.crlf_pairs_ = 0;
        other.is_chunked_ = false;

        return *this;
    }
    parser(const parser&) = delete;
    parser& operator=(const parser&) = delete;

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
    [[nodiscard]] result<state> parse_available();
    [[nodiscard]] result<std::span<uint8_t>> writable_input_span(size_t desired_size);
    [[nodiscard]] result<state> commit_input(size_t bytes);

    [[nodiscard]] bool is_complete() const noexcept { return state_ == state::complete; }
    [[nodiscard]] const request& get_request() const noexcept { return request_; }
    [[nodiscard]] size_t bytes_parsed() const noexcept { return parse_pos_; }
    [[nodiscard]] size_t buffered_bytes() const noexcept {
        return buffer_size_ > parse_pos_ ? buffer_size_ - parse_pos_ : 0;
    }
    [[nodiscard]] state current_state() const noexcept { return state_; }
    [[nodiscard]] size_t buffer_size() const noexcept { return buffer_size_; }
    [[nodiscard]] size_t parse_pos() const noexcept { return parse_pos_; }
    [[nodiscard]] std::string_view unparsed_view(size_t max_bytes = 64) const noexcept {
        const size_t remaining = buffered_bytes();
        const size_t length = remaining < max_bytes ? remaining : max_bytes;
        return (buffer_ && length > 0) ? std::string_view(buffer_ + parse_pos_, length)
                                       : std::string_view{};
    }
    request&& take_request() { return std::move(request_); }
    void reset(monotonic_arena* arena) noexcept;
    void prepare_for_next_request(monotonic_arena* arena) noexcept;

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
    bool reserve_buffer(size_t capacity) noexcept;
    bool ensure_buffer_capacity(size_t required_capacity) noexcept;
    void maybe_shrink_buffer() noexcept;
    void reset_message_state(monotonic_arena* arena) noexcept;

    monotonic_arena* arena_;
    state state_ = state::request_line;
    request request_;
    std::unique_ptr<char[]> buffer_owner_;
    char* buffer_ = nullptr;
    size_t buffer_size_ = 0;
    size_t buffer_capacity_ = 0;
    char* chunked_body_ = nullptr;
    size_t chunked_body_size_ = 0;
    field last_header_field_ = field::unknown;
    const char* last_header_name_ = nullptr;
    size_t last_header_name_len_ = 0;
    size_t parse_pos_ = 0;
    size_t content_length_ = 0;
    size_t current_chunk_size_ = 0;
    size_t header_count_ = 0;
    size_t validated_bytes_ = 0;
    size_t header_end_pos_ = 0;
    size_t crlf_scan_pos_ = 0;
    size_t crlf_pairs_ = 0;
    bool is_chunked_ = false;

    static constexpr size_t INITIAL_BUFFER_CAPACITY = 16UL * 1024UL;
    static constexpr size_t RETAIN_BUFFER_CAPACITY = 64UL * 1024UL;
    static constexpr size_t COMPACT_THRESHOLD = 2048;
};

method parse_method(std::string_view str);
std::string_view method_to_string(method m);

inline std::span<const uint8_t> as_bytes(std::string_view sv) noexcept {
    return std::span<const uint8_t>(
        static_cast<const uint8_t*>(static_cast<const void*>(sv.data())), sv.size());
}

} // namespace katana::http
