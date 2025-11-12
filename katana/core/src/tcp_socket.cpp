#include "katana/core/tcp_socket.hpp"

#include <unistd.h>
#include <cerrno>
#include <system_error>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

namespace katana {

namespace {
    constexpr size_t MIN_BUFFER_SIZE = 65536;
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

result<void> tcp_socket::optimize_for_throughput() {
    return optimize_socket_buffers(fd_);
}

result<void> optimize_socket_buffers(int32_t fd, int32_t sndbuf_size, int32_t rcvbuf_size) {
    if (fd < 0) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    int32_t nodelay = 1;
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay)) < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sndbuf_size, sizeof(sndbuf_size)) < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcvbuf_size, sizeof(rcvbuf_size)) < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    return {};
}

result<void> set_tcp_cork(int32_t fd, bool enable) noexcept {
    if (fd < 0) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    int32_t cork = enable ? 1 : 0;
    if (setsockopt(fd, IPPROTO_TCP, TCP_CORK, &cork, sizeof(cork)) < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    return {};
}

} // namespace katana
