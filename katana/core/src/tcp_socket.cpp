#include "katana/core/tcp_socket.hpp"

#include <unistd.h>
#include <cerrno>
#include <system_error>

namespace katana {

result<std::span<uint8_t>> tcp_socket::read(std::span<uint8_t> buf) {
    if (fd_ < 0) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    ssize_t n;
    do {
        n = ::read(fd_, buf.data(), buf.size());
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

    ssize_t n;
    do {
        n = ::write(fd_, data.data(), data.size());
    } while (n < 0 && errno == EINTR);

    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    return static_cast<size_t>(n);
}

void tcp_socket::close() noexcept {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

} // namespace katana
