#include "katana/core/http.hpp"
#include "katana/core/detail/syscall_metrics.hpp"
#include "katana/core/simd_utils.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstring>
#include <new>

namespace katana::http {

namespace {

// HTTP protocol constants
constexpr int HEX_BASE = 16; // Hexadecimal base for chunked encoding

constexpr std::string_view CHUNKED_ENCODING_HEADER = "Transfer-Encoding: chunked\r\n\r\n";
constexpr std::string_view CHUNKED_TERMINATOR = "0\r\n\r\n";
constexpr std::string_view HTTP_VERSION_PREFIX = "HTTP/1.1 ";
constexpr std::string_view HEADER_SEPARATOR = ": ";
constexpr std::string_view CRLF = "\r\n";
constexpr std::string_view CONTENT_TYPE_TEXT = "text/plain";
constexpr std::string_view CONTENT_TYPE_JSON = "application/json";
constexpr std::string_view CONTENT_TYPE_PROBLEM_JSON = "application/problem+json";

alignas(64) static const bool TOKEN_CHARS[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

alignas(64) static const bool INVALID_HEADER_CHARS[256] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

inline bool is_token_char(unsigned char c) noexcept {
    return TOKEN_CHARS[c];
}

constexpr bool is_ctl(unsigned char c) noexcept {
    return c < 0x20 || c == 0x7f;
}

std::string_view trim_ows(std::string_view value) noexcept {
    while (!value.empty() && (value.front() == ' ' || value.front() == '\t')) {
        value.remove_prefix(1);
    }
    while (!value.empty() && (value.back() == ' ' || value.back() == '\t')) {
        value.remove_suffix(1);
    }
    return value;
}

bool contains_invalid_header_value(std::string_view value) noexcept {
    for (char ch : value) {
        if (INVALID_HEADER_CHARS[static_cast<unsigned char>(ch)]) {
            return true;
        }
    }
    return false;
}

bool contains_invalid_uri_char(std::string_view uri) noexcept {
    for (char ch : uri) {
        auto c = static_cast<unsigned char>(ch);
        if (c == ' ' || c == '\r' || c == '\n' || is_ctl(c) || c >= 0x80) {
            return true;
        }
    }
    return false;
}

const char* find_header_terminator(const char* data, size_t size) noexcept {
    if (size < 4) {
        return nullptr;
    }
    for (size_t i = 0; i + 3 < size; ++i) {
        if (data[i] == '\r' && data[i + 1] == '\n' && data[i + 2] == '\r' && data[i + 3] == '\n') {
            return data + i;
        }
    }
    return nullptr;
}

void set_content_type(headers_map& headers, std::string_view content_type) noexcept {
    if (content_type == CONTENT_TYPE_TEXT) {
        headers.set_known_borrowed(http::field::content_type, CONTENT_TYPE_TEXT);
        return;
    }
    if (content_type == CONTENT_TYPE_JSON) {
        headers.set_known_borrowed(http::field::content_type, CONTENT_TYPE_JSON);
        return;
    }
    if (content_type == CONTENT_TYPE_PROBLEM_JSON) {
        headers.set_known_borrowed(http::field::content_type, CONTENT_TYPE_PROBLEM_JSON);
        return;
    }
    headers.set_known(http::field::content_type, content_type);
}

} // namespace

method parse_method(std::string_view str) {
    if (str == "GET")
        return method::get;
    if (str == "POST")
        return method::post;
    if (str == "PUT")
        return method::put;
    if (str == "DELETE")
        return method::del;
    if (str == "PATCH")
        return method::patch;
    if (str == "HEAD")
        return method::head;
    if (str == "OPTIONS")
        return method::options;
    return method::unknown;
}

std::string_view canonical_reason_phrase(int32_t status) noexcept {
    switch (status) {
    case 200:
        return "OK";
    case 201:
        return "Created";
    case 202:
        return "Accepted";
    case 204:
        return "No Content";
    case 304:
        return "Not Modified";
    case 400:
        return "Bad Request";
    case 401:
        return "Unauthorized";
    case 403:
        return "Forbidden";
    case 404:
        return "Not Found";
    case 405:
        return "Method Not Allowed";
    case 406:
        return "Not Acceptable";
    case 409:
        return "Conflict";
    case 415:
        return "Unsupported Media Type";
    case 422:
        return "Unprocessable Entity";
    case 429:
        return "Too Many Requests";
    case 500:
        return "Internal Server Error";
    case 503:
        return "Service Unavailable";
    default:
        return "OK";
    }
}

std::string_view method_to_string(method m) {
    switch (m) {
    case method::get:
        return "GET";
    case method::post:
        return "POST";
    case method::put:
        return "PUT";
    case method::del:
        return "DELETE";
    case method::patch:
        return "PATCH";
    case method::head:
        return "HEAD";
    case method::options:
        return "OPTIONS";
    default:
        return "UNKNOWN";
    }
}

std::string response::serialize() const {
    if (chunked) {
        return serialize_chunked();
    }
    std::string out;
    serialize_into(out);
    return out;
}

void response::serialize_into(std::string& out) const {
    if (chunked) {
        out = serialize_chunked();
        ::katana::detail::syscall_metrics_registry::instance().note_response_serialize(
            out.size(), 0, out.capacity());
        return;
    }

    // Calculate Content-Length if not already set
    char content_length_buf[32];
    std::string_view content_length_value;
    bool has_content_length = headers.get("Content-Length").has_value();

    if (!has_content_length) {
        auto [ptr, ec] = std::to_chars(
            content_length_buf, content_length_buf + sizeof(content_length_buf), body.size());
        if (ec == std::errc()) {
            content_length_value =
                std::string_view(content_length_buf, static_cast<size_t>(ptr - content_length_buf));
        }
    }

    size_t headers_size = 0;
    for (const auto& [name, value] : headers) {
        headers_size += name.size() + HEADER_SEPARATOR.size() + value.size() + CRLF.size();
    }

    // Add space for Content-Length header if we're adding it
    if (!content_length_value.empty()) {
        headers_size += 14 + HEADER_SEPARATOR.size() + content_length_value.size() +
                        CRLF.size(); // "Content-Length"
    }

    const size_t old_capacity = out.capacity();
    out.clear();
    out.reserve(32 + reason.size() + headers_size + body.size());

    char status_buf[16];
    auto [ptr, ec] = std::to_chars(status_buf, status_buf + sizeof(status_buf), status);

    out.append(HTTP_VERSION_PREFIX);
    out.append(status_buf, static_cast<size_t>(ptr - status_buf));
    out.push_back(' ');
    out.append(reason);
    out.append(CRLF);

    for (const auto& [name, value] : headers) {
        out.append(name);
        out.append(HEADER_SEPARATOR);
        out.append(value);
        out.append(CRLF);
    }

    // Add Content-Length header if needed
    if (!content_length_value.empty()) {
        out.append("Content-Length");
        out.append(HEADER_SEPARATOR);
        out.append(content_length_value);
        out.append(CRLF);
    }

    out.append(CRLF);
    out.append(body);
    ::katana::detail::syscall_metrics_registry::instance().note_response_serialize(
        out.size(), old_capacity, out.capacity());
}

void response::serialize_head_into(std::string& out) const {
    if (chunked) {
        out = serialize_chunked();
        return;
    }

    char content_length_buf[32];
    std::string_view content_length_value;
    bool has_content_length = headers.get("Content-Length").has_value();

    if (!has_content_length) {
        auto [ptr, ec] = std::to_chars(
            content_length_buf, content_length_buf + sizeof(content_length_buf), body.size());
        if (ec == std::errc()) {
            content_length_value =
                std::string_view(content_length_buf, static_cast<size_t>(ptr - content_length_buf));
        }
    }

    size_t headers_size = 0;
    for (const auto& [name, value] : headers) {
        headers_size += name.size() + HEADER_SEPARATOR.size() + value.size() + CRLF.size();
    }

    if (!content_length_value.empty()) {
        headers_size += 14 + HEADER_SEPARATOR.size() + content_length_value.size() + CRLF.size();
    }

    out.clear();
    out.reserve(32 + reason.size() + headers_size + CRLF.size());

    char status_buf[16];
    auto [ptr, ec] = std::to_chars(status_buf, status_buf + sizeof(status_buf), status);
    (void)ec;

    out.append(HTTP_VERSION_PREFIX);
    out.append(status_buf, static_cast<size_t>(ptr - status_buf));
    out.push_back(' ');
    out.append(reason);
    out.append(CRLF);

    for (const auto& [name, value] : headers) {
        out.append(name);
        out.append(HEADER_SEPARATOR);
        out.append(value);
        out.append(CRLF);
    }

    if (!content_length_value.empty()) {
        out.append("Content-Length");
        out.append(HEADER_SEPARATOR);
        out.append(content_length_value);
        out.append(CRLF);
    }

    out.append(CRLF);
}

void response::serialize_into(io_buffer& out) const {
    if (chunked) {
        out.append(serialize_chunked());
        return;
    }

    // Calculate Content-Length if not already set
    char content_length_buf[32];
    std::string_view content_length_value;
    bool has_content_length = headers.get("Content-Length").has_value();

    if (!has_content_length) {
        auto [ptr, ec] = std::to_chars(
            content_length_buf, content_length_buf + sizeof(content_length_buf), body.size());
        if (ec == std::errc()) {
            content_length_value =
                std::string_view(content_length_buf, static_cast<size_t>(ptr - content_length_buf));
        }
    }

    // Calculate total size in single pass
    size_t headers_size = 0;
    for (const auto& [name, value] : headers) {
        headers_size += name.size() + HEADER_SEPARATOR.size() + value.size() + CRLF.size();
    }

    if (!content_length_value.empty()) {
        headers_size += 14 + HEADER_SEPARATOR.size() + content_length_value.size() + CRLF.size();
    }

    char status_buf[16];
    auto [ptr, ec] = std::to_chars(status_buf, status_buf + sizeof(status_buf), status);
    size_t status_len = static_cast<size_t>(ptr - status_buf);

    // Calculate total size
    size_t total_size = HTTP_VERSION_PREFIX.size() + status_len + 1 + reason.size() + CRLF.size() +
                        headers_size + CRLF.size() + body.size();

    // Get writable buffer and write directly into it (zero-copy)
    auto buf = out.writable_span(total_size);
    uint8_t* write_ptr = buf.data();

    // Write status line
    std::memcpy(write_ptr, HTTP_VERSION_PREFIX.data(), HTTP_VERSION_PREFIX.size());
    write_ptr += HTTP_VERSION_PREFIX.size();
    std::memcpy(write_ptr, status_buf, status_len);
    write_ptr += status_len;
    *write_ptr++ = ' ';
    std::memcpy(write_ptr, reason.data(), reason.size());
    write_ptr += reason.size();
    std::memcpy(write_ptr, CRLF.data(), CRLF.size());
    write_ptr += CRLF.size();

    // Write headers
    for (const auto& [name, value] : headers) {
        std::memcpy(write_ptr, name.data(), name.size());
        write_ptr += name.size();
        std::memcpy(write_ptr, HEADER_SEPARATOR.data(), HEADER_SEPARATOR.size());
        write_ptr += HEADER_SEPARATOR.size();
        std::memcpy(write_ptr, value.data(), value.size());
        write_ptr += value.size();
        std::memcpy(write_ptr, CRLF.data(), CRLF.size());
        write_ptr += CRLF.size();
    }

    // Write Content-Length if needed
    if (!content_length_value.empty()) {
        std::memcpy(write_ptr, "Content-Length", 14);
        write_ptr += 14;
        std::memcpy(write_ptr, HEADER_SEPARATOR.data(), HEADER_SEPARATOR.size());
        write_ptr += HEADER_SEPARATOR.size();
        std::memcpy(write_ptr, content_length_value.data(), content_length_value.size());
        write_ptr += content_length_value.size();
        std::memcpy(write_ptr, CRLF.data(), CRLF.size());
        write_ptr += CRLF.size();
    }

    // Write final CRLF and body
    std::memcpy(write_ptr, CRLF.data(), CRLF.size());
    write_ptr += CRLF.size();
    std::memcpy(write_ptr, body.data(), body.size());
    write_ptr += body.size();

    out.commit(total_size);
}

std::string response::serialize_chunked(size_t chunk_size) const {
    size_t headers_size = 0;
    for (const auto& [name, value] : headers) {
        if (name != "Content-Length") {
            headers_size += name.size() + HEADER_SEPARATOR.size() + value.size() + CRLF.size();
        }
    }

    std::string result;
    result.reserve(64 + reason.size() + headers_size + body.size() + 32);

    char status_buf[16];
    auto [ptr, ec] = std::to_chars(status_buf, status_buf + sizeof(status_buf), status);

    result.append(HTTP_VERSION_PREFIX);
    result.append(status_buf, static_cast<size_t>(ptr - status_buf));
    result.push_back(' ');
    result.append(reason);
    result.append(CRLF);

    for (const auto& [name, value] : headers) {
        if (name != "Content-Length") {
            result.append(name);
            result.append(HEADER_SEPARATOR);
            result.append(value);
            result.append(CRLF);
        }
    }

    result.append(CHUNKED_ENCODING_HEADER);

    size_t offset = 0;
    char chunk_size_buf[32];
    while (offset < body.size()) {
        size_t current_chunk = std::min(chunk_size, body.size() - offset);
        auto [chunk_ptr, chunk_ec] = std::to_chars(
            chunk_size_buf, chunk_size_buf + sizeof(chunk_size_buf), current_chunk, HEX_BASE);
        result.append(chunk_size_buf, static_cast<size_t>(chunk_ptr - chunk_size_buf));
        result.append(CRLF);
        result.append(body.data() + offset, current_chunk);
        result.append(CRLF);
        offset += current_chunk;
    }

    result.append(CHUNKED_TERMINATOR);

    return result;
}

response response::ok(std::string body, std::string content_type) {
    response res;
    res.assign_text(std::move(body), content_type, 200, "OK");
    return res;
}

response response::json(std::string body) {
    return ok(std::move(body), "application/json");
}

response response::error(const problem_details& problem) {
    response res;
    res.assign_error(problem);
    return res;
}

void response::reset() noexcept {
    status = 200;
    reason.clear();
    body.clear();
    chunked = false;
    headers.clear();
}

void response::assign_text(std::string body_value,
                           std::string_view content_type,
                           int32_t status_code,
                           std::string_view reason_phrase) {
    reset();
    status = status_code;
    if (reason_phrase.empty()) {
        reason.assign(canonical_reason_phrase(status_code));
    } else {
        reason.assign(reason_phrase.data(), reason_phrase.size());
    }
    body = std::move(body_value);
    set_content_type(headers, content_type);
}

void response::assign_json(std::string body_value,
                           int32_t status_code,
                           std::string_view reason_phrase) {
    reset();
    status = status_code;
    if (reason_phrase.empty()) {
        reason.assign(canonical_reason_phrase(status_code));
    } else {
        reason.assign(reason_phrase.data(), reason_phrase.size());
    }
    body = std::move(body_value);
    set_content_type(headers, CONTENT_TYPE_JSON);
}

void response::assign_error(const problem_details& problem) {
    reset();
    status = problem.status;
    reason.assign(problem.title.data(), problem.title.size());
    body = problem.to_json();
    set_content_type(headers, CONTENT_TYPE_PROBLEM_JSON);
}

response_builder& response_builder::text(std::string body, int32_t status_code) {
    out_.assign_text(std::move(body),
                     CONTENT_TYPE_TEXT,
                     status_code,
                     canonical_reason_phrase(status_code));
    return *this;
}

response_builder& response_builder::json(std::string body, int32_t status_code) {
    out_.assign_json(std::move(body), status_code, canonical_reason_phrase(status_code));
    return *this;
}

response_builder& response_builder::problem(const problem_details& problem) {
    out_.assign_error(problem);
    return *this;
}

response_builder& response_builder::created_json(std::string body) {
    return json(std::move(body), 201);
}

response_builder& response_builder::no_content() {
    out_.reset();
    out_.status = 204;
    out_.reason.assign(canonical_reason_phrase(204));
    return *this;
}

result<parser::state> parser::parse(std::span<const uint8_t> data) {
    if (data.empty()) {
        return parse_available();
    }

    auto writable = writable_input_span(data.size());
    if (!writable) [[unlikely]] {
        return std::unexpected(writable.error());
    }

    std::memcpy(writable->data(), data.data(), data.size());
    return commit_input(data.size());
}

result<std::span<uint8_t>> parser::writable_input_span(size_t desired_size) {
    if (!buffer_) [[unlikely]] {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    if (desired_size > MAX_BUFFER_SIZE) [[unlikely]] {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    if (buffer_size_ > buffer_capacity_) [[unlikely]] {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    size_t available = buffer_capacity_ - buffer_size_;
    if (desired_size > available) [[unlikely]] {
        if (parse_pos_ > 0 && state_ != state::complete) {
            compact_buffer();
            if (buffer_size_ > buffer_capacity_) [[unlikely]] {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
            available = buffer_capacity_ - buffer_size_;
        }

        if (desired_size > available) {
            if (desired_size > MAX_BUFFER_SIZE - buffer_size_) [[unlikely]] {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
            if (!ensure_buffer_capacity(buffer_size_ + desired_size)) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
        }
    }

    if (desired_size > buffer_capacity_ - buffer_size_) [[unlikely]] {
        if (!ensure_buffer_capacity(buffer_size_ + desired_size)) {
            return std::unexpected(make_error_code(error_code::invalid_fd));
        }
    }

    return std::span<uint8_t>(reinterpret_cast<uint8_t*>(buffer_ + buffer_size_), desired_size);
}

result<parser::state> parser::commit_input(size_t bytes) {
    if (!buffer_) [[unlikely]] {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }
    if (buffer_size_ > buffer_capacity_) [[unlikely]] {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }
    if (bytes > buffer_capacity_ - buffer_size_) [[unlikely]] {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    buffer_size_ += bytes;
    return parse_available();
}

result<parser::state> parser::parse_available() {
    if (!buffer_) [[unlikely]] {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    if (state_ == state::request_line || state_ == state::headers) [[likely]] {
        size_t validation_start = validated_bytes_;
        if (validation_start > 0) {
            --validation_start;
        }
        for (size_t i = validation_start; i < buffer_size_; ++i) {
            uint8_t byte = static_cast<uint8_t>(buffer_[i]);
            if (byte == 0 || byte >= 0x80) [[unlikely]] {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
            if (byte == '\n' && (i == 0 || buffer_[i - 1] != '\r')) [[unlikely]] {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
        }
        validated_bytes_ = buffer_size_;
    }

    if (state_ != state::body && state_ != state::chunk_data) {
        if (buffer_size_ > MAX_HEADER_SIZE) {
            const char* header_end = find_header_terminator(buffer_, buffer_size_);
            if (!header_end || static_cast<size_t>(header_end - buffer_) + 4 > MAX_HEADER_SIZE) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
        }

    } else if (buffer_size_ > MAX_HEADER_SIZE + MAX_BODY_SIZE) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    while (state_ != state::complete) {
        size_t old_parse_pos = parse_pos_;
        result<state> next_state = [&]() -> result<state> {
            switch (state_) {
            case state::request_line:
                return parse_request_line_state();
            case state::headers:
                return parse_headers_state();
            case state::body:
                return parse_body_state();
            case state::chunk_size:
                return parse_chunk_size_state();
            case state::chunk_data:
                return parse_chunk_data_state();
            case state::chunk_trailer:
                return parse_chunk_trailer_state();
            default:
                return state_;
            }
        }();

        if (!next_state) {
            return std::unexpected(next_state.error());
        }

        state_ = *next_state;

        if (parse_pos_ == old_parse_pos && state_ != state::complete) {
            if (parse_pos_ > COMPACT_THRESHOLD || buffer_size_ > MAX_HEADER_SIZE * 2) {
                compact_buffer();
            }
            return state_;
        }
    }

    return state_;
}

result<parser::state> parser::parse_request_line_state() {
    const char* found = simd::find_crlf(buffer_ + parse_pos_, buffer_size_ - parse_pos_);

    if (found) {
        size_t pos = static_cast<size_t>(found - buffer_);
        for (size_t i = parse_pos_; i <= pos; ++i) {
            unsigned char c = static_cast<unsigned char>(buffer_[i]);
            if (c == '\0' || c >= 0x80) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
            if (c == '\n' && (i == 0 || buffer_[i - 1] != '\r')) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
        }

        std::string_view line(buffer_ + parse_pos_, pos - parse_pos_);
        parse_pos_ = pos + 2;

        auto res = process_request_line(line);
        if (!res) {
            return std::unexpected(res.error());
        }

        return state::headers;
    }

    return state::request_line;
}

result<parser::state> parser::parse_headers_state() {
    const char* found = simd::find_crlf(buffer_ + parse_pos_, buffer_size_ - parse_pos_);

    if (found) {
        size_t pos = static_cast<size_t>(found - buffer_);
        for (size_t i = parse_pos_; i <= pos; ++i) {
            unsigned char c = static_cast<unsigned char>(buffer_[i]);
            if (c == '\0' || c >= 0x80) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
            if (c == '\n' && (i == 0 || buffer_[i - 1] != '\r')) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
        }

        std::string_view line(buffer_ + parse_pos_, pos - parse_pos_);
        parse_pos_ = pos + 2;

        if (line.empty()) {
            auto te = request_.headers.get(field::transfer_encoding);
            if (te && ci_equal(*te, "chunked")) {
                is_chunked_ = true;
                return state::chunk_size;
            }

            auto cl = request_.headers.get(field::content_length);
            if (cl) {
                std::string_view cl_view = *cl;
                while (!cl_view.empty() && (cl_view.back() == ' ' || cl_view.back() == '\t')) {
                    cl_view.remove_suffix(1);
                }

                unsigned long long val = 0;
                auto [ptr, ec] =
                    std::from_chars(cl_view.data(), cl_view.data() + cl_view.size(), val);
                if (ec != std::errc() || ptr != cl_view.data() + cl_view.size() || val > SIZE_MAX ||
                    val > MAX_BODY_SIZE) {
                    return std::unexpected(make_error_code(error_code::invalid_fd));
                }
                content_length_ = static_cast<size_t>(val);
                return state::body;
            }

            return state::complete;
        }

        if (line.front() == ' ' || line.front() == '\t') {
            if (!last_header_name_) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }

            // Get current value using either field enum or name
            std::optional<std::string_view> current_value;
            if (last_header_field_ != field::unknown) {
                current_value = request_.headers.get(last_header_field_);
            } else {
                current_value = request_.headers.get(
                    std::string_view(last_header_name_, last_header_name_len_));
            }

            if (!current_value) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }

            // Header folding - allocate combined value in arena
            auto folded_view = std::string_view(line);
            while (!folded_view.empty() &&
                   (folded_view.front() == ' ' || folded_view.front() == '\t')) {
                folded_view.remove_prefix(1);
            }
            folded_view = trim_ows(folded_view);
            if (contains_invalid_header_value(folded_view)) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }

            size_t total_len = current_value->size() + 1 + folded_view.size();
            char* combined = static_cast<char*>(arena_->allocate(total_len + 1, 1));
            if (combined) {
                std::memcpy(combined, current_value->data(), current_value->size());
                combined[current_value->size()] = ' ';
                std::memcpy(
                    combined + current_value->size() + 1, folded_view.data(), folded_view.size());
                combined[total_len] = '\0';

                // Set using either field enum or name
                if (last_header_field_ != field::unknown) {
                    request_.headers.set_known_borrowed(last_header_field_,
                                                        std::string_view(combined, total_len));
                } else {
                    request_.headers.set_unknown_borrowed(
                        std::string_view(last_header_name_, last_header_name_len_),
                        std::string_view(combined, total_len));
                }
            }
        } else {
            auto res = process_header_line(line);
            if (!res) {
                return std::unexpected(res.error());
            }
        }

        return state::headers;
    }

    return state::headers;
}

result<parser::state> parser::parse_body_state() {
    size_t remaining = buffer_size_ - parse_pos_;
    if (remaining >= content_length_) {
        request_.body = std::string_view(buffer_ + parse_pos_, content_length_);
        parse_pos_ += content_length_;
        return state::complete;
    }
    return state::body;
}

result<parser::state> parser::parse_chunk_size_state() {
    const char* found = simd::find_crlf(buffer_ + parse_pos_, buffer_size_ - parse_pos_);
    if (!found) {
        return state::chunk_size;
    }

    size_t pos = static_cast<size_t>(found - buffer_);
    std::string_view chunk_line(buffer_ + parse_pos_, pos - parse_pos_);
    parse_pos_ = pos + 2;

    auto semicolon = chunk_line.find(';');
    if (semicolon != std::string_view::npos) {
        chunk_line = chunk_line.substr(0, semicolon);
    }

    chunk_line = trim_ows(chunk_line);

    unsigned long long chunk_val = 0;
    auto [ptr, ec] =
        std::from_chars(chunk_line.data(), chunk_line.data() + chunk_line.size(), chunk_val, 16);
    if (ec != std::errc() || ptr != chunk_line.data() + chunk_line.size()) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }
    if (chunk_val > SIZE_MAX || chunk_val > MAX_BODY_SIZE) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }
    current_chunk_size_ = static_cast<size_t>(chunk_val);

    if (current_chunk_size_ == 0) {
        return state::chunk_trailer;
    }

    if (current_chunk_size_ > MAX_BODY_SIZE ||
        chunked_body_size_ > MAX_BODY_SIZE - current_chunk_size_) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    return state::chunk_data;
}

result<parser::state> parser::parse_chunk_data_state() {
    size_t remaining = buffer_size_ - parse_pos_;
    if (remaining >= current_chunk_size_ + 2) {
        const char* chunk_start = buffer_ + parse_pos_;
        if (chunk_start[current_chunk_size_] != '\r' ||
            chunk_start[current_chunk_size_ + 1] != '\n') {
            return std::unexpected(make_error_code(error_code::invalid_fd));
        }

        if (!chunked_body_) {
            chunked_body_ = static_cast<char*>(arena_->allocate(MAX_BODY_SIZE, 1));
            if (!chunked_body_) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
        }

        std::memcpy(chunked_body_ + chunked_body_size_, chunk_start, current_chunk_size_);
        chunked_body_size_ += current_chunk_size_;
        parse_pos_ += current_chunk_size_ + 2;
        return state::chunk_size;
    }
    return state::chunk_data;
}

result<parser::state> parser::parse_chunk_trailer_state() {
    const char* found = simd::find_crlf(buffer_ + parse_pos_, buffer_size_ - parse_pos_);
    if (!found) {
        return state::chunk_trailer;
    }

    size_t pos = static_cast<size_t>(found - buffer_);
    parse_pos_ = pos + 2;
    request_.body = std::string_view(chunked_body_, chunked_body_size_);
    return state::complete;
}

result<void> parser::process_request_line(std::string_view line) {
    if (line.empty() || line.front() == ' ' || line.front() == '\t' || line.back() == ' ' ||
        line.back() == '\t') {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    auto method_end = line.find(' ');
    if (method_end == std::string_view::npos) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    auto method_str = line.substr(0, method_end);
    request_.http_method = parse_method(method_str);
    if (request_.http_method == method::unknown) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    auto uri_start = method_end + 1;
    auto uri_end = line.find(' ', uri_start);
    if (uri_end == std::string_view::npos) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    auto uri = line.substr(uri_start, uri_end - uri_start);
    if (uri.size() > MAX_URI_LENGTH) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    if (contains_invalid_uri_char(uri)) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    request_.uri = uri;

    auto version = line.substr(uri_end + 1);
    if (version != "HTTP/1.1") {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    return {};
}

result<void> parser::process_header_line(std::string_view line) {
    if (header_count_ >= MAX_HEADER_COUNT) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    auto colon = line.find(':');
    if (colon == std::string_view::npos) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    auto name = line.substr(0, colon);
    auto value = line.substr(colon + 1);

    if (name.empty()) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    for (char ch : name) {
        auto c = static_cast<unsigned char>(ch);
        if (!is_token_char(c)) {
            return std::unexpected(make_error_code(error_code::invalid_fd));
        }
    }

    value = trim_ows(value);
    if (contains_invalid_header_value(value)) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    last_header_field_ = field::unknown;
    last_header_name_ = nullptr;
    last_header_name_len_ = 0;

    if (name.size() == 4 && (name[0] == 'H' || name[0] == 'h')) {
        if (ci_equal_fast(name, "Host")) {
            request_.headers.set_known_borrowed(field::host, value);
            last_header_field_ = field::host;
            ++header_count_;
            return {};
        }
    }

    if (name.size() == 14 && (name[0] == 'C' || name[0] == 'c')) {
        if (ci_equal_fast(name, "Content-Length")) {
            request_.headers.set_known_borrowed(field::content_length, value);

            unsigned long long len = 0;
            auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), len);
            if (ec == std::errc()) {
                content_length_ = len;
            }

            last_header_field_ = field::content_length;
            ++header_count_;
            return {};
        }
    }

    last_header_field_ = string_to_field(name);

    if (last_header_field_ != field::unknown) {
        request_.headers.set_known_borrowed(last_header_field_, value);
        ++header_count_;
        return {};
    }

    last_header_name_ = name.data();
    last_header_name_len_ = name.size();
    request_.headers.set_unknown_borrowed(name, value);
    ++header_count_;
    return {};
}

void parser::compact_buffer() {
    if (parse_pos_ >= buffer_size_) {
        ::katana::detail::syscall_metrics_registry::instance().note_parser_compact(0);
        buffer_size_ = 0;
        parse_pos_ = 0;
        validated_bytes_ = 0;
        crlf_scan_pos_ = 0;
        crlf_pairs_ = 0;
    } else if (parse_pos_ > COMPACT_THRESHOLD / 2) {
        const size_t consumed = parse_pos_;
        const size_t moved = buffer_size_ - consumed;
        std::memmove(buffer_, buffer_ + consumed, buffer_size_ - consumed);
        ::katana::detail::syscall_metrics_registry::instance().note_parser_compact(moved);
        buffer_size_ -= consumed;
        parse_pos_ = 0;
        validated_bytes_ = (validated_bytes_ > consumed) ? (validated_bytes_ - consumed) : 0;
        crlf_scan_pos_ = (crlf_scan_pos_ > consumed) ? (crlf_scan_pos_ - consumed) : 0;
        crlf_pairs_ = 0;
        for (size_t i = 0; i + 1 < crlf_scan_pos_; ++i) {
            if (buffer_[i] == '\r' && buffer_[i + 1] == '\n') {
                ++crlf_pairs_;
            }
        }
    }
}

bool parser::reserve_buffer(size_t capacity) noexcept {
    if (capacity > MAX_BUFFER_SIZE) {
        return false;
    }

    if (capacity <= buffer_capacity_ && buffer_) {
        return true;
    }

    const size_t old_capacity = buffer_capacity_;
    const size_t copied_bytes = (buffer_ && buffer_size_ > 0) ? buffer_size_ : 0;
    auto new_buffer = std::unique_ptr<char[]>(new (std::nothrow) char[capacity]);
    if (!new_buffer) {
        return false;
    }

    if (buffer_ && buffer_size_ > 0) {
        std::memcpy(new_buffer.get(), buffer_, buffer_size_);
    }

    buffer_owner_ = std::move(new_buffer);
    buffer_ = buffer_owner_.get();
    buffer_capacity_ = capacity;
    ::katana::detail::syscall_metrics_registry::instance().note_parser_reserve(
        old_capacity, capacity, copied_bytes);
    return true;
}

bool parser::ensure_buffer_capacity(size_t required_capacity) noexcept {
    if (required_capacity > MAX_BUFFER_SIZE) {
        return false;
    }

    if (required_capacity <= buffer_capacity_) {
        return true;
    }

    size_t new_capacity = buffer_capacity_ > 0 ? buffer_capacity_ : INITIAL_BUFFER_CAPACITY;
    while (new_capacity < required_capacity) {
        if (new_capacity >= MAX_BUFFER_SIZE / 2) {
            new_capacity = MAX_BUFFER_SIZE;
            break;
        }
        new_capacity *= 2;
    }

    if (new_capacity < required_capacity) {
        new_capacity = required_capacity;
    }

    return reserve_buffer(new_capacity);
}

void parser::maybe_shrink_buffer() noexcept {
    if (buffer_capacity_ <= RETAIN_BUFFER_CAPACITY) {
        if (!buffer_ && buffer_capacity_ == 0) {
            reserve_buffer(INITIAL_BUFFER_CAPACITY);
        }
        return;
    }

    buffer_owner_.reset();
    buffer_ = nullptr;
    buffer_capacity_ = 0;
    reserve_buffer(INITIAL_BUFFER_CAPACITY);
}

void parser::reset_message_state(monotonic_arena* arena) noexcept {
    arena_ = arena;
    state_ = state::request_line;
    request_.http_method = method::unknown;
    request_.uri = {};
    request_.body = {};
    request_.headers.reset(arena_);
    parse_pos_ = 0;
    content_length_ = 0;
    current_chunk_size_ = 0;
    header_count_ = 0;
    chunked_body_ = nullptr;
    chunked_body_size_ = 0;
    last_header_field_ = field::unknown;
    last_header_name_ = nullptr;
    last_header_name_len_ = 0;
    validated_bytes_ = 0;
    crlf_scan_pos_ = 0;
    crlf_pairs_ = 0;
    is_chunked_ = false;
}

void parser::reset(monotonic_arena* arena) noexcept {
    buffer_size_ = 0;
    maybe_shrink_buffer();
    reset_message_state(arena);
}

void parser::prepare_for_next_request(monotonic_arena* arena) noexcept {
    size_t remaining = buffered_bytes();
    if (remaining > 0 && parse_pos_ > 0) {
        std::memmove(buffer_, buffer_ + parse_pos_, remaining);
    }
    buffer_size_ = remaining;
    reset_message_state(arena);
}

} // namespace katana::http
