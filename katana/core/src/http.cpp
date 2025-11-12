#include "katana/core/http.hpp"
#include "katana/core/simd_utils.hpp"

#include <algorithm>
#include <charconv>
#include <cstring>
#include <cctype>

namespace katana::http {

namespace {

// HTTP protocol constants
constexpr int HEX_BASE = 16;  // Hexadecimal base for chunked encoding

constexpr std::string_view CHUNKED_ENCODING_HEADER = "Transfer-Encoding: chunked\r\n\r\n";
constexpr std::string_view CHUNKED_TERMINATOR = "0\r\n\r\n";
constexpr std::string_view HTTP_VERSION_PREFIX = "HTTP/1.1 ";
constexpr std::string_view HEADER_SEPARATOR = ": ";
constexpr std::string_view CRLF = "\r\n";

alignas(64) static const bool TOKEN_CHARS[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,0,1,1,1,1,1,0,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

alignas(64) static const bool INVALID_HEADER_CHARS[256] = {
    1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

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

} // namespace

method parse_method(std::string_view str) {
    if (str == "GET") return method::get;
    if (str == "POST") return method::post;
    if (str == "PUT") return method::put;
    if (str == "DELETE") return method::del;
    if (str == "PATCH") return method::patch;
    if (str == "HEAD") return method::head;
    if (str == "OPTIONS") return method::options;
    return method::unknown;
}

std::string_view method_to_string(method m) {
    switch (m) {
        case method::get: return "GET";
        case method::post: return "POST";
        case method::put: return "PUT";
        case method::del: return "DELETE";
        case method::patch: return "PATCH";
        case method::head: return "HEAD";
        case method::options: return "OPTIONS";
        default: return "UNKNOWN";
    }
}

std::string response::serialize() const {
    if (chunked) {
        return serialize_chunked();
    }

    size_t headers_size = 0;
    for (const auto& [name, value] : headers) {
        headers_size += name.size() + HEADER_SEPARATOR.size() + value.size() + CRLF.size();
    }

    std::string result;
    result.reserve(32 + reason.size() + headers_size + body.size());

    char status_buf[16];
    auto [ptr, ec] = std::to_chars(status_buf, status_buf + sizeof(status_buf), status);

    result.append(HTTP_VERSION_PREFIX);
    result.append(status_buf, static_cast<size_t>(ptr - status_buf));
    result.push_back(' ');
    result.append(reason);
    result.append(CRLF);

    for (const auto& [name, value] : headers) {
        result.append(name);
        result.append(HEADER_SEPARATOR);
        result.append(value);
        result.append(CRLF);
    }

    result.append(CRLF);
    result.append(body);

    return result;
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
        auto [chunk_ptr, chunk_ec] = std::to_chars(chunk_size_buf, chunk_size_buf + sizeof(chunk_size_buf),
                                                     current_chunk, HEX_BASE);
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
    if (!buffer_ || data.size() > buffer_capacity_ || buffer_size_ > buffer_capacity_ - data.size()) [[unlikely]] {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    if (state_ == state::request_line || state_ == state::headers) [[likely]] {
        for (size_t i = 0; i < data.size(); ++i) {
            uint8_t byte = data[i];
            if (byte == 0 || byte >= 0x80) [[unlikely]] {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
            if (byte == '\n') [[unlikely]] {
                size_t buf_pos = buffer_size_ + i;
                if (buf_pos == 0 || (buf_pos > 0 &&
                    (buf_pos - 1 < buffer_size_ ? buffer_[buf_pos - 1] : data[i - 1]) != '\r')) {
                    return std::unexpected(make_error_code(error_code::invalid_fd));
                }
            }
        }
    }

    std::memcpy(buffer_ + buffer_size_, data.data(), data.size());
    buffer_size_ += data.size();

    if (state_ != state::body && state_ != state::chunk_data) {
        if (buffer_size_ > MAX_HEADER_SIZE) {
            const char* header_end = std::strstr(buffer_, "\r\n\r\n");
            if (!header_end || static_cast<size_t>(header_end - buffer_) + 4 > MAX_HEADER_SIZE) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
        }

        size_t crlf_pairs = 0;
        for (size_t i = 0; i + 1 < buffer_size_; ++i) {
            if (buffer_[i] == '\r' && buffer_[i + 1] == '\n') {
                ++crlf_pairs;
            }
        }
        if (crlf_pairs > MAX_HEADER_COUNT + 2) {
            return std::unexpected(make_error_code(error_code::invalid_fd));
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

    if (parse_pos_ > COMPACT_THRESHOLD || buffer_size_ > MAX_HEADER_SIZE * 2) {
        compact_buffer();
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
            if (c == '\n' && (i == 0 || buffer_[i-1] != '\r')) {
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
            if (c == '\n' && (i == 0 || buffer_[i-1] != '\r')) {
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
                auto [ptr, ec] = std::from_chars(cl_view.data(), cl_view.data() + cl_view.size(), val);
                if (ec != std::errc() || ptr != cl_view.data() + cl_view.size() ||
                    val > SIZE_MAX || val > MAX_BODY_SIZE) {
                    return std::unexpected(make_error_code(error_code::invalid_fd));
                }
                content_length_ = static_cast<size_t>(val);
                return state::body;
            }

            return state::complete;
        }

        if (line.front() == ' ' || line.front() == '\t') {
            if (last_header_field_ == field::unknown) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
            auto current_value = request_.headers.get(last_header_field_);
            if (!current_value) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
            // Header folding - allocate combined value in arena
            auto folded_view = std::string_view(line);
            while (!folded_view.empty() && (folded_view.front() == ' ' || folded_view.front() == '\t')) {
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
                std::memcpy(combined + current_value->size() + 1, folded_view.data(), folded_view.size());
                combined[total_len] = '\0';
                request_.headers.set(last_header_field_, std::string_view(combined, total_len));
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
        char* body_ptr = arena_->allocate_string(std::string_view(buffer_ + parse_pos_, content_length_));
        request_.body = std::string_view(body_ptr, content_length_);
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
    auto [ptr, ec] = std::from_chars(chunk_line.data(), chunk_line.data() + chunk_line.size(), chunk_val, 16);
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

    if (current_chunk_size_ > MAX_BODY_SIZE || chunked_body_size_ > MAX_BODY_SIZE - current_chunk_size_) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    return state::chunk_data;
}

result<parser::state> parser::parse_chunk_data_state() {
    size_t remaining = buffer_size_ - parse_pos_;
    if (remaining >= current_chunk_size_ + 2) {
        const char* chunk_start = buffer_ + parse_pos_;
        if (chunk_start[current_chunk_size_] != '\r' || chunk_start[current_chunk_size_ + 1] != '\n') {
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
    if (line.empty() || line.front() == ' ' || line.front() == '\t' ||
        line.back() == ' ' || line.back() == '\t') {
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

    char* uri_ptr = arena_->allocate_string(uri);
    request_.uri = std::string_view(uri_ptr, uri.size());

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

    last_header_field_ = string_to_field(name);
    request_.headers.set_view(name, value);
    ++header_count_;
    return {};
}

void parser::compact_buffer() {
    if (parse_pos_ >= buffer_size_) {
        buffer_size_ = 0;
        parse_pos_ = 0;
    } else if (parse_pos_ > COMPACT_THRESHOLD / 2) {
        std::memmove(buffer_, buffer_ + parse_pos_, buffer_size_ - parse_pos_);
        buffer_size_ -= parse_pos_;
        parse_pos_ = 0;
    }
}

} // namespace katana::http
