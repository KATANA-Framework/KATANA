#include "katana/core/redis_client.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <cerrno>
#include <chrono>
#include <cstdint>
#include <poll.h>
#include <span>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>

namespace katana::http {

namespace {

std::string encode_command(std::span<const std::string_view> args) {
    std::string encoded;
    encoded.reserve(64);
    encoded.push_back('*');
    encoded.append(std::to_string(args.size()));
    encoded.append("\r\n");
    for (auto arg : args) {
        encoded.push_back('$');
        encoded.append(std::to_string(arg.size()));
        encoded.append("\r\n");
        encoded.append(arg.data(), arg.size());
        encoded.append("\r\n");
    }
    return encoded;
}

} // namespace

redis_sync_client::redis_sync_client() = default;

redis_sync_client::redis_sync_client(options opts) : options_(std::move(opts)) {}

redis_sync_client::~redis_sync_client() { disconnect(); }

redis_sync_client::redis_sync_client(redis_sync_client&& other) noexcept
    : options_(std::move(other.options_)), socket_(std::move(other.socket_)),
      read_buffer_(std::move(other.read_buffer_)), last_error_(other.last_error_) {}

redis_sync_client& redis_sync_client::operator=(redis_sync_client&& other) noexcept {
    if (this != &other) {
        disconnect();
        options_ = std::move(other.options_);
        socket_ = std::move(other.socket_);
        read_buffer_ = std::move(other.read_buffer_);
        last_error_ = other.last_error_;
    }
    return *this;
}

std::optional<std::string> redis_sync_client::get(std::string_view key) {
    const std::array args = {std::string_view{"GET"}, key};
    auto reply = execute(args);
    if (!reply) {
        last_error_ = reply.error();
        return std::nullopt;
    }
    last_error_.clear();
    if (reply->type == resp_reply::kind::bulk_string) {
        return reply->text;
    }
    return std::nullopt;
}

void redis_sync_client::set(std::string_view key,
                            std::string_view value,
                            policy_clock::duration ttl_value) {
    const auto ttl_ms = std::to_string(to_milliseconds(ttl_value).count());
    const std::string_view ttl_ms_view{ttl_ms.data(), ttl_ms.size()};
    const std::array args = {std::string_view{"SET"}, key, value, std::string_view{"PX"},
                             ttl_ms_view};
    auto reply = execute(args);
    if (!reply) {
        last_error_ = reply.error();
        return;
    }
    last_error_.clear();
}

bool redis_sync_client::set_if_absent(std::string_view key,
                                      std::string_view value,
                                      policy_clock::duration ttl_value) {
    const auto ttl_ms = std::to_string(to_milliseconds(ttl_value).count());
    const std::string_view ttl_ms_view{ttl_ms.data(), ttl_ms.size()};
    const std::array args = {std::string_view{"SET"}, key, value, std::string_view{"NX"},
                             std::string_view{"PX"}, ttl_ms_view};
    auto reply = execute(args);
    if (!reply) {
        last_error_ = reply.error();
        return false;
    }
    last_error_.clear();
    return reply->type == resp_reply::kind::simple_string && reply->text == "OK";
}

void redis_sync_client::erase(std::string_view key) {
    const std::array args = {std::string_view{"DEL"}, key};
    auto reply = execute(args);
    if (!reply) {
        last_error_ = reply.error();
        return;
    }
    last_error_.clear();
}

int64_t redis_sync_client::increment(std::string_view key) {
    const std::array args = {std::string_view{"INCR"}, key};
    auto reply = execute(args);
    if (!reply) {
        last_error_ = reply.error();
        return 0;
    }
    last_error_.clear();
    return reply->integer;
}

void redis_sync_client::expire(std::string_view key, policy_clock::duration ttl_value) {
    const auto ttl_ms = std::to_string(to_milliseconds(ttl_value).count());
    const std::string_view ttl_ms_view{ttl_ms.data(), ttl_ms.size()};
    const std::array args = {std::string_view{"PEXPIRE"}, key, ttl_ms_view};
    auto reply = execute(args);
    if (!reply) {
        last_error_ = reply.error();
        return;
    }
    last_error_.clear();
}

std::optional<policy_clock::duration> redis_sync_client::ttl(std::string_view key) {
    const std::array args = {std::string_view{"PTTL"}, key};
    auto reply = execute(args);
    if (!reply) {
        last_error_ = reply.error();
        return std::nullopt;
    }
    last_error_.clear();
    if (reply->type != resp_reply::kind::integer || reply->integer < 0) {
        return std::nullopt;
    }
    return std::chrono::milliseconds(reply->integer);
}

result<void> redis_sync_client::connect() {
    disconnect();

    const int32_t fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (fd < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(options_.port);
    if (::inet_pton(AF_INET, options_.host.c_str(), &addr.sin_addr) != 1) {
        ::close(fd);
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    }

    const int rc = ::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (rc < 0 && errno != EINPROGRESS) {
        const auto ec = std::error_code(errno, std::system_category());
        ::close(fd);
        return std::unexpected(ec);
    }

    socket_ = katana::tcp_socket(fd);
    auto wait = wait_until_ready(POLLOUT, options_.connect_timeout);
    if (!wait) {
        disconnect();
        return wait;
    }

    int socket_error = 0;
    socklen_t socket_error_len = sizeof(socket_error);
    if (::getsockopt(socket_.native_handle(),
                     SOL_SOCKET,
                     SO_ERROR,
                     &socket_error,
                     &socket_error_len) < 0) {
        const auto ec = std::error_code(errno, std::system_category());
        disconnect();
        return std::unexpected(ec);
    }
    if (socket_error != 0) {
        const auto ec = std::error_code(socket_error, std::system_category());
        disconnect();
        return std::unexpected(ec);
    }

    last_error_.clear();
    return {};
}

void redis_sync_client::disconnect() noexcept {
    socket_.close();
    read_buffer_.clear();
}

bool redis_sync_client::connected() const noexcept { return static_cast<bool>(socket_); }

result<redis_sync_client::resp_reply> redis_sync_client::execute(std::span<const std::string_view> args) {
    auto connected_result = ensure_connected();
    if (!connected_result) {
        return std::unexpected(connected_result.error());
    }
    auto send_result = send_command(args);
    if (!send_result) {
        disconnect();
        return std::unexpected(send_result.error());
    }
    auto reply = read_reply();
    if (!reply) {
        disconnect();
        return std::unexpected(reply.error());
    }
    if (reply->type == resp_reply::kind::error) {
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    }
    return reply;
}

result<void> redis_sync_client::ensure_connected() {
    if (connected()) {
        return {};
    }
    return connect();
}

result<void> redis_sync_client::send_command(std::span<const std::string_view> args) {
    std::string encoded = encode_command(args);
    size_t written = 0;
    while (written < encoded.size()) {
        auto result = socket_.write(std::span<const uint8_t>(
            reinterpret_cast<const uint8_t*>(encoded.data() + written), encoded.size() - written));
        if (!result) {
            return std::unexpected(result.error());
        }
        if (*result == 0) {
            auto wait = wait_until_ready(POLLOUT, options_.io_timeout);
            if (!wait) {
                return wait;
            }
            continue;
        }
        written += *result;
    }
    return {};
}

result<redis_sync_client::resp_reply> redis_sync_client::read_reply() {
    size_t cursor = 0;
    for (;;) {
        auto reply = parse_reply(cursor);
        if (reply) {
            consume_buffer(cursor);
            return reply;
        }
        if (reply.error() != std::make_error_code(std::errc::operation_would_block)) {
            return reply;
        }
        auto fill = fill_buffer();
        if (!fill) {
            return std::unexpected(fill.error());
        }
    }
}

result<void> redis_sync_client::wait_until_ready(short events,
                                                 std::chrono::milliseconds timeout) const {
    pollfd pfd{};
    pfd.fd = socket_.native_handle();
    pfd.events = events;

    for (;;) {
        const int rc = ::poll(&pfd, 1, static_cast<int>(timeout.count()));
        if (rc > 0) {
            if ((pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) != 0) {
                return std::unexpected(std::make_error_code(std::errc::connection_reset));
            }
            return {};
        }
        if (rc == 0) {
            return std::unexpected(make_error_code(error_code::timeout));
        }
        if (errno == EINTR) {
            continue;
        }
        return std::unexpected(std::error_code(errno, std::system_category()));
    }
}

result<void> redis_sync_client::fill_buffer() {
    std::array<uint8_t, 4096> chunk{};
    for (;;) {
        auto result = socket_.read(chunk);
        if (!result) {
            if (result.error() == make_error_code(error_code::ok)) {
                return std::unexpected(std::make_error_code(std::errc::connection_reset));
            }
            return std::unexpected(result.error());
        }
        if (result->empty()) {
            auto wait = wait_until_ready(POLLIN, options_.io_timeout);
            if (!wait) {
                return wait;
            }
            continue;
        }
        read_buffer_.append(reinterpret_cast<const char*>(result->data()), result->size());
        return {};
    }
}

result<std::string_view> redis_sync_client::read_line(size_t start) {
    for (;;) {
        const auto eol = read_buffer_.find("\r\n", start);
        if (eol != std::string::npos) {
            return std::string_view(read_buffer_.data() + start, eol - start);
        }
        return std::unexpected(std::make_error_code(std::errc::operation_would_block));
    }
}

result<redis_sync_client::resp_reply> redis_sync_client::parse_reply(size_t& cursor) {
    if (cursor >= read_buffer_.size()) {
        return std::unexpected(std::make_error_code(std::errc::operation_would_block));
    }

    resp_reply reply;
    const char type = read_buffer_[cursor++];
    switch (type) {
    case '+':
    case '-':
    case ':': {
        auto line = read_line(cursor);
        if (!line) {
            --cursor;
            return std::unexpected(line.error());
        }
        cursor += line->size() + 2;
        if (type == '+') {
            reply.type = resp_reply::kind::simple_string;
            reply.text.assign(line->data(), line->size());
            return reply;
        }
        if (type == '-') {
            reply.type = resp_reply::kind::error;
            reply.text.assign(line->data(), line->size());
            return reply;
        }
        reply.type = resp_reply::kind::integer;
        auto [end, ec] =
            std::from_chars(line->data(), line->data() + line->size(), reply.integer);
        if (ec != std::errc() || end != line->data() + line->size()) {
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        return reply;
    }
    case '$': {
        auto line = read_line(cursor);
        if (!line) {
            --cursor;
            return std::unexpected(line.error());
        }
        int64_t length = -1;
        auto [end, ec] =
            std::from_chars(line->data(), line->data() + line->size(), length);
        if (ec != std::errc() || end != line->data() + line->size()) {
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        cursor += line->size() + 2;
        if (length < 0) {
            reply.type = resp_reply::kind::null_bulk;
            return reply;
        }
        const auto remaining = read_buffer_.size() - cursor;
        const auto needed = static_cast<size_t>(length) + 2;
        if (remaining < needed) {
            cursor = 0;
            return std::unexpected(std::make_error_code(std::errc::operation_would_block));
        }
        reply.type = resp_reply::kind::bulk_string;
        reply.text.assign(read_buffer_.data() + cursor, static_cast<size_t>(length));
        cursor += static_cast<size_t>(length);
        if (read_buffer_.compare(cursor, 2, "\r\n") != 0) {
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        cursor += 2;
        return reply;
    }
    default:
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    }
}

void redis_sync_client::consume_buffer(size_t bytes) {
    if (bytes == 0) {
        return;
    }
    read_buffer_.erase(0, bytes);
}

std::chrono::milliseconds
redis_sync_client::to_milliseconds(policy_clock::duration duration) noexcept {
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    if (ms <= std::chrono::milliseconds::zero()) {
        return std::chrono::milliseconds(1);
    }
    return ms;
}

} // namespace katana::http
