#include "katana/core/tcp_socket.hpp"

#include <algorithm>
#include <cerrno>
#include <system_error>
#include <unistd.h>

namespace katana {

namespace {
constexpr size_t MIN_BUFFER_SIZE = 16384;
}

result<std::span<uint8_t>> tcp_socket::read(std::span<uint8_t> buf) {
    if (fd_ < 0) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    const size_t read_size = std::max(buf.size(), MIN_BUFFER_SIZE);
    const size_t actual_size = std::min(read_size, buf.size());

    ssize_t n;
    do {
        n = ::read(fd_, buf.data(), actual_size);
    } while (n < 0 && errno == EINTR);

    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return std::span<uint8_t>{};
        }
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    if (n == 0 && !buf.empty()) {
        return std::unexpected(make_error_code(error_code::ok));
    }

    return buf.subspan(0, static_cast<size_t>(n));
}

result<size_t> tcp_socket::write(std::span<const uint8_t> data) {
    if (fd_ < 0) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    size_t total_written = 0;
    while (total_written < data.size()) {
        ssize_t n;
        do {
            n = ::write(fd_, data.data() + total_written, data.size() - total_written);
        } while (n < 0 && errno == EINTR);

        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return total_written;
            }
            return std::unexpected(std::error_code(errno, std::system_category()));
        }

        total_written += static_cast<size_t>(n);

        if (n == 0 || errno == EAGAIN || errno == EWOULDBLOCK) {
            break;
        }
    }

    return total_written;
}

void tcp_socket::close() noexcept {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

} // namespace katana
