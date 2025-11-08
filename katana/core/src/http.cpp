#include "katana/core/http.hpp"
#include "katana/core/simd_utils.hpp"

#include <algorithm>
#include <charconv>
#include <cstring>
#include <cctype>

namespace katana::http {

namespace {

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
        headers_size += name.size() + 2 + value.size() + 2;
    }

    std::string result;
    result.reserve(32 + reason.size() + headers_size + body.size());

    char status_buf[16];
    auto [ptr, ec] = std::to_chars(status_buf, status_buf + sizeof(status_buf), status);

    result.append("HTTP/1.1 ", 9);
    result.append(status_buf, static_cast<size_t>(ptr - status_buf));
    result.push_back(' ');
    result.append(reason);
    result.append("\r\n", 2);

    for (const auto& [name, value] : headers) {
        result.append(name);
        result.append(": ", 2);
        result.append(value);
        result.append("\r\n", 2);
    }

    result.append("\r\n", 2);
    result.append(body);

    return result;
}

std::string response::serialize_chunked(size_t chunk_size) const {
    size_t headers_size = 0;
    for (const auto& [name, value] : headers) {
        if (name != "Content-Length") {
            headers_size += name.size() + 2 + value.size() + 2;
        }
    }

    std::string result;
    result.reserve(64 + reason.size() + headers_size + body.size() + 32);

    char status_buf[16];
    auto [ptr, ec] = std::to_chars(status_buf, status_buf + sizeof(status_buf), status);

    result.append("HTTP/1.1 ", 9);
    result.append(status_buf, static_cast<size_t>(ptr - status_buf));
    result.push_back(' ');
    result.append(reason);
    result.append("\r\n", 2);

    for (const auto& [name, value] : headers) {
        if (name != "Content-Length") {
            result.append(name);
            result.append(": ", 2);
            result.append(value);
            result.append("\r\n", 2);
        }
    }

    result.append("Transfer-Encoding: chunked\r\n\r\n", 30);

    size_t offset = 0;
    char chunk_size_buf[32];
    while (offset < body.size()) {
        size_t current_chunk = std::min(chunk_size, body.size() - offset);
        auto [chunk_ptr, chunk_ec] = std::to_chars(chunk_size_buf, chunk_size_buf + sizeof(chunk_size_buf),
                                                     current_chunk, 16);
        result.append(chunk_size_buf, static_cast<size_t>(chunk_ptr - chunk_size_buf));
        result.append("\r\n", 2);
        result.append(body.data() + offset, current_chunk);
        result.append("\r\n", 2);
        offset += current_chunk;
    }

    result.append("0\r\n\r\n", 5);

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
    size_t max_safe_size = std::min(MAX_BUFFER_SIZE, buffer_.max_size());
    if (data.size() > max_safe_size || buffer_.size() > max_safe_size - data.size()) [[unlikely]] {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    if (state_ == state::request_line || state_ == state::headers) [[likely]] {
        for (size_t i = 0; i < data.size(); ++i) {
            uint8_t byte = data[i];
            if (byte == 0 || byte >= 0x80) [[unlikely]] {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
            if (byte == '\n') [[unlikely]] {
                size_t buf_pos = buffer_.size() + i;
                if (buf_pos == 0 || (buf_pos > 0 &&
                    (buf_pos - 1 < buffer_.size() ? buffer_[buf_pos - 1] : data[i - 1]) != '\r')) {
                    return std::unexpected(make_error_code(error_code::invalid_fd));
                }
            }
        }
    }

    buffer_.append(static_cast<const char*>(static_cast<const void*>(data.data())), data.size());

    if (state_ != state::body && state_ != state::chunk_data) {
        if (buffer_.size() > MAX_HEADER_SIZE) {
            auto header_end = buffer_.find("\r\n\r\n");
            if (header_end == std::string::npos || header_end + 4 > MAX_HEADER_SIZE) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
        }

        size_t crlf_pairs = 0;
        for (size_t i = 0; i + 1 < buffer_.size(); ++i) {
            if (buffer_[i] == '\r' && buffer_[i + 1] == '\n') {
                ++crlf_pairs;
            }
        }
        if (crlf_pairs > MAX_HEADER_COUNT + 2) {
            return std::unexpected(make_error_code(error_code::invalid_fd));
        }
    } else if (buffer_.size() > MAX_HEADER_SIZE + MAX_BODY_SIZE) {
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
            if (parse_pos_ > COMPACT_THRESHOLD || buffer_.size() > MAX_HEADER_SIZE * 2) {
                compact_buffer();
            }
            return state_;
        }
    }

    if (parse_pos_ > COMPACT_THRESHOLD || buffer_.size() > MAX_HEADER_SIZE * 2) {
        compact_buffer();
    }

    return state_;
}

result<parser::state> parser::parse_request_line_state() {
    size_t pos = std::string::npos;
    const char* found = simd::find_crlf(buffer_.data() + parse_pos_, buffer_.size() - parse_pos_);

    if (found) {
        pos = static_cast<size_t>(found - buffer_.data());
        for (size_t i = parse_pos_; i <= pos; ++i) {
            unsigned char c = static_cast<unsigned char>(buffer_[i]);
            if (c == '\0' || c >= 0x80) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
            if (c == '\n' && (i == 0 || buffer_[i-1] != '\r')) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
        }
    }

    if (pos == std::string::npos) {
        return state::request_line;
    }

    std::string_view line(buffer_.data() + parse_pos_, pos - parse_pos_);
    parse_pos_ = pos + 2;

    auto res = process_request_line(line);
    if (!res) {
        return std::unexpected(res.error());
    }

    return state::headers;
}

result<parser::state> parser::parse_headers_state() {
    size_t pos = std::string::npos;
    const char* found = simd::find_crlf(buffer_.data() + parse_pos_, buffer_.size() - parse_pos_);

    if (found) {
        pos = static_cast<size_t>(found - buffer_.data());
        for (size_t i = parse_pos_; i <= pos; ++i) {
            unsigned char c = static_cast<unsigned char>(buffer_[i]);
            if (c == '\0' || c >= 0x80) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
            if (c == '\n' && (i == 0 || buffer_[i-1] != '\r')) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
        }
    }

    if (pos == std::string::npos) {
        return state::headers;
    }

    std::string_view line(buffer_.data() + parse_pos_, pos - parse_pos_);
    parse_pos_ = pos + 2;

    if (line.empty()) {
        auto te = request_.header("Transfer-Encoding");
        if (te && ci_equal(*te, "chunked")) {
            is_chunked_ = true;
            return state::chunk_size;
        }

        auto cl = request_.header("Content-Length");
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
        if (last_header_name_.empty()) {
            return std::unexpected(make_error_code(error_code::invalid_fd));
        }
        auto current_value = request_.header(last_header_name_);
        if (!current_value) {
            return std::unexpected(make_error_code(error_code::invalid_fd));
        }
        auto folded_view = std::string_view(line);
        while (!folded_view.empty() && (folded_view.front() == ' ' || folded_view.front() == '\t')) {
            folded_view.remove_prefix(1);
        }
        folded_view = trim_ows(folded_view);
        if (contains_invalid_header_value(folded_view)) {
            return std::unexpected(make_error_code(error_code::invalid_fd));
        }
        std::string new_value;
        new_value.reserve(current_value->size() + 1 + folded_view.size());
        new_value.append(*current_value);
        new_value.push_back(' ');
        new_value.append(folded_view);
        request_.headers.set(last_header_name_, std::move(new_value));
    } else {
        auto res = process_header_line(line);
        if (!res) {
            return std::unexpected(res.error());
        }
    }

    return state::headers;
}

result<parser::state> parser::parse_body_state() {
    size_t remaining = buffer_.size() - parse_pos_;
    if (remaining >= content_length_) {
        request_.body.assign(buffer_.data() + parse_pos_, content_length_);
        parse_pos_ += content_length_;
        return state::complete;
    }
    return state::body;
}

result<parser::state> parser::parse_chunk_size_state() {
    size_t pos = std::string::npos;
    const char* found = simd::find_crlf(buffer_.data() + parse_pos_, buffer_.size() - parse_pos_);
    if (found) {
        pos = static_cast<size_t>(found - buffer_.data());
    }

    if (pos == std::string::npos) {
        return state::chunk_size;
    }

    std::string_view chunk_line(buffer_.data() + parse_pos_, pos - parse_pos_);
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

    if (current_chunk_size_ > MAX_BODY_SIZE || chunked_body_.size() > MAX_BODY_SIZE - current_chunk_size_) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    return state::chunk_data;
}

result<parser::state> parser::parse_chunk_data_state() {
    size_t remaining = buffer_.size() - parse_pos_;
    if (remaining >= current_chunk_size_ + 2) {
        const char* chunk_start = buffer_.data() + parse_pos_;
        if (chunk_start[current_chunk_size_] != '\r' || chunk_start[current_chunk_size_ + 1] != '\n') {
            return std::unexpected(make_error_code(error_code::invalid_fd));
        }
        chunked_body_.append(chunk_start, current_chunk_size_);
        parse_pos_ += current_chunk_size_ + 2;
        return state::chunk_size;
    }
    return state::chunk_data;
}

result<parser::state> parser::parse_chunk_trailer_state() {
    size_t pos = std::string::npos;
    const char* found = simd::find_crlf(buffer_.data() + parse_pos_, buffer_.size() - parse_pos_);
    if (found) {
        pos = static_cast<size_t>(found - buffer_.data());
    }

    if (pos == std::string::npos) {
        return state::chunk_trailer;
    }

    parse_pos_ = pos + 2;
    request_.body = chunked_body_;
    return state::complete;
}

result<void> parser::process_request_line(std::string_view line) {
    // Reject leading or trailing whitespace
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

    request_.uri = std::string(uri);

    auto version = line.substr(uri_end + 1);
    if (version != "HTTP/1.1") {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }
    request_.version = std::string(version);

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

    last_header_name_ = std::string(name);
    request_.headers.set_view(name, value);
    ++header_count_;
    return {};
}

void parser::compact_buffer() {
    if (parse_pos_ >= buffer_.size()) {
        buffer_.clear();
        parse_pos_ = 0;
    } else if (parse_pos_ > COMPACT_THRESHOLD / 2) {
        buffer_.erase(buffer_.begin(), buffer_.begin() + static_cast<std::string::difference_type>(parse_pos_));
        parse_pos_ = 0;

        if (buffer_.capacity() > buffer_.size() * 2 && buffer_.capacity() > 8192) {
            buffer_.shrink_to_fit();
        }
    }
}

} // namespace katana::http
