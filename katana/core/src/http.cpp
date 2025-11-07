#include "katana/core/http.hpp"

#include <algorithm>
#include <charconv>

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

std::string response::serialize() const {
    if (chunked) {
        return serialize_chunked();
    }

    std::string result;
    result.reserve(256 + body.size());

    char status_buf[16];
    auto [ptr, ec] = std::to_chars(status_buf, status_buf + sizeof(status_buf), status);

    result += "HTTP/1.1 ";
    result.append(status_buf, ptr - status_buf);
    result += " ";
    result += reason;
    result += "\r\n";

    for (const auto& [name, value] : headers) {
        result += name;
        result += ": ";
        result += value;
        result += "\r\n";
    }

    result += "\r\n";
    result += body;

    return result;
}

std::string response::serialize_chunked(size_t chunk_size) const {
    std::string result;
    result.reserve(512 + body.size());

    char status_buf[16];
    auto [ptr, ec] = std::to_chars(status_buf, status_buf + sizeof(status_buf), status);

    result += "HTTP/1.1 ";
    result.append(status_buf, ptr - status_buf);
    result += " ";
    result += reason;
    result += "\r\n";

    for (const auto& [name, value] : headers) {
        if (name != "Content-Length") {
            result += name;
            result += ": ";
            result += value;
            result += "\r\n";
        }
    }

    result += "Transfer-Encoding: chunked\r\n\r\n";

    size_t offset = 0;
    char chunk_size_buf[32];
    while (offset < body.size()) {
        size_t current_chunk = std::min(chunk_size, body.size() - offset);
        auto [chunk_ptr, chunk_ec] = std::to_chars(chunk_size_buf, chunk_size_buf + sizeof(chunk_size_buf),
                                                     current_chunk, 16);
        result.append(chunk_size_buf, chunk_ptr - chunk_size_buf);
        result += "\r\n";
        result.append(body.data() + offset, current_chunk);
        result += "\r\n";
        offset += current_chunk;
    }

    result += "0\r\n\r\n";

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
    if (data.size() > MAX_BUFFER_SIZE || buffer_.size() > MAX_BUFFER_SIZE - data.size()) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    if (state_ != state::body && state_ != state::chunk_data) {
        if (buffer_.size() + data.size() > MAX_HEADER_SIZE) {
            return std::unexpected(make_error_code(error_code::invalid_fd));
        }
        size_t crlf_count = 0;
        for (size_t i = 0; i < buffer_.size(); ++i) {
            if (buffer_[i] == '\n') ++crlf_count;
        }
        for (size_t i = 0; i < data.size(); ++i) {
            if (data[i] == '\n') ++crlf_count;
        }
        if (crlf_count > MAX_HEADER_COUNT + 2) {
            return std::unexpected(make_error_code(error_code::invalid_fd));
        }
    } else {
        if (buffer_.size() + data.size() > MAX_HEADER_SIZE + MAX_BODY_SIZE) {
            return std::unexpected(make_error_code(error_code::invalid_fd));
        }
    }

    buffer_.append(static_cast<const char*>(static_cast<const void*>(data.data())), data.size());

    while (state_ != state::complete) {
        if (state_ == state::request_line || state_ == state::headers) {
            auto pos = buffer_.find("\r\n", parse_pos_);
            if (pos == std::string::npos) {
                return state_;
            }

            std::string_view line(buffer_.data() + parse_pos_, pos - parse_pos_);
            parse_pos_ = pos + 2;

            if (state_ == state::request_line) {
                auto res = parse_request_line(line);
                if (!res) {
                    return std::unexpected(res.error());
                }
                state_ = state::headers;
            } else {
                if (line.empty()) {
                    auto te = request_.header("Transfer-Encoding");
                    if (te && *te == "chunked") {
                        is_chunked_ = true;
                        state_ = state::chunk_size;
                    } else {
                        auto cl = request_.header("Content-Length");
                        if (cl) {
                            try {
                                unsigned long long val = std::stoull(std::string(*cl));
                                if (val > SIZE_MAX || val > MAX_BODY_SIZE) {
                                    return std::unexpected(make_error_code(error_code::invalid_fd));
                                }
                                content_length_ = static_cast<size_t>(val);
                                state_ = state::body;
                            } catch (...) {
                                return std::unexpected(make_error_code(error_code::invalid_fd));
                            }
                        } else {
                            state_ = state::complete;
                        }
                    }
                } else {
                    auto res = parse_header_line(line);
                    if (!res) {
                        return std::unexpected(res.error());
                    }
                }
            }
        } else if (state_ == state::body) {
            size_t remaining = buffer_.size() - parse_pos_;
            if (remaining >= content_length_) {
                request_.body = std::string(buffer_.data() + parse_pos_, content_length_);
                parse_pos_ += content_length_;
                state_ = state::complete;
            } else {
                return state_;
            }
        } else if (state_ == state::chunk_size) {
            auto pos = buffer_.find("\r\n", parse_pos_);
            if (pos == std::string::npos) {
                return state_;
            }

            std::string_view chunk_line(buffer_.data() + parse_pos_, pos - parse_pos_);
            parse_pos_ = pos + 2;

            auto semicolon = chunk_line.find(';');
            if (semicolon != std::string_view::npos) {
                chunk_line = chunk_line.substr(0, semicolon);
            }

            try {
                std::string chunk_str(chunk_line);
                unsigned long long chunk_val = std::stoull(chunk_str, nullptr, 16);
                if (chunk_val > SIZE_MAX || chunk_val > MAX_BODY_SIZE) {
                    return std::unexpected(make_error_code(error_code::invalid_fd));
                }
                current_chunk_size_ = static_cast<size_t>(chunk_val);
                if (current_chunk_size_ == 0) {
                    state_ = state::chunk_trailer;
                } else {
                    if (current_chunk_size_ > MAX_BODY_SIZE ||
                        chunked_body_.size() > MAX_BODY_SIZE - current_chunk_size_) {
                        return std::unexpected(make_error_code(error_code::invalid_fd));
                    }
                    state_ = state::chunk_data;
                }
            } catch (...) {
                return std::unexpected(make_error_code(error_code::invalid_fd));
            }
        } else if (state_ == state::chunk_data) {
            size_t remaining = buffer_.size() - parse_pos_;
            if (remaining >= current_chunk_size_ + 2) {
                chunked_body_.append(buffer_.substr(parse_pos_, current_chunk_size_));
                parse_pos_ += current_chunk_size_ + 2;
                state_ = state::chunk_size;
            } else {
                return state_;
            }
        } else if (state_ == state::chunk_trailer) {
            auto pos = buffer_.find("\r\n", parse_pos_);
            if (pos == std::string::npos) {
                return state_;
            }
            parse_pos_ = pos + 2;
            request_.body = chunked_body_;
            state_ = state::complete;
        }
    }

    // Compact buffer if parse_pos_ exceeds threshold or buffer is too large
    if (parse_pos_ > COMPACT_THRESHOLD || buffer_.size() > MAX_HEADER_SIZE * 2) {
        compact_buffer();
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

    auto uri = line.substr(uri_start, uri_end - uri_start);
    if (uri.size() > MAX_URI_LENGTH) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    request_.uri = std::string(uri);
    request_.version = std::string(line.substr(uri_end + 1));

    return {};
}

result<void> parser::parse_header_line(std::string_view line) {
    if (header_count_ >= MAX_HEADER_COUNT) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    auto colon = line.find(':');
    if (colon == std::string_view::npos) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    auto name = line.substr(0, colon);
    auto value = line.substr(colon + 1);

    while (!value.empty() && value.front() == ' ') {
        value.remove_prefix(1);
    }

    request_.headers.set(std::string(name), std::string(value));
    ++header_count_;
    return {};
}

void parser::compact_buffer() {
    if (parse_pos_ > 0 && parse_pos_ < buffer_.size()) {
        // Move unparsed data to the beginning of the buffer
        buffer_.erase(0, parse_pos_);
        parse_pos_ = 0;
    } else if (parse_pos_ >= buffer_.size()) {
        // All data has been parsed, clear the buffer
        buffer_.clear();
        parse_pos_ = 0;
    }
}

} // namespace katana::http
