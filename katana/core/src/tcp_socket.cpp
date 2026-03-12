#include "katana/core/tcp_socket.hpp"
#include "katana/core/detail/syscall_metrics.hpp"

#include <algorithm>
#include <cerrno>
#include <sys/socket.h>
#include <sys/uio.h>
#include <system_error>
#include <unistd.h>

namespace katana {

namespace {
constexpr size_t SMALL_WRITE_FAST_PATH = 16384;

#ifdef MSG_NOSIGNAL
constexpr int WRITE_FLAGS = MSG_DONTWAIT | MSG_NOSIGNAL;
#else
constexpr int WRITE_FLAGS = MSG_DONTWAIT;
#endif
} // namespace

result<std::span<uint8_t>> tcp_socket::read(std::span<uint8_t> buf) {
    if (fd_ < 0) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    auto& metrics = detail::syscall_metrics_registry::instance();
    ssize_t n;
    do {
        n = ::recv(fd_, buf.data(), buf.size(), MSG_DONTWAIT);
        metrics.note_recv(n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK));
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

    auto& metrics = detail::syscall_metrics_registry::instance();
    if (data.size() <= SMALL_WRITE_FAST_PATH) {
        ssize_t n;
        do {
            n = ::send(fd_, data.data(), data.size(), WRITE_FLAGS);
            metrics.note_send(n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK));
        } while (n < 0 && errno == EINTR);

        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return size_t{0};
            }
            return std::unexpected(std::error_code(errno, std::system_category()));
        }

        return static_cast<size_t>(n);
    }

    size_t total_written = 0;
    while (total_written < data.size()) {
        ssize_t n;
        do {
            n = ::send(fd_, data.data() + total_written, data.size() - total_written, WRITE_FLAGS);
            metrics.note_send(n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK));
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

result<size_t> tcp_socket::writev(const iovec* iov, size_t count) {
    if (fd_ < 0) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }
    if (count == 0) {
        return size_t{0};
    }
    if (count == 1) {
        auto* bytes = static_cast<const uint8_t*>(iov[0].iov_base);
        return write(std::span<const uint8_t>(bytes, iov[0].iov_len));
    }

    msghdr msg{};
    msg.msg_iov = const_cast<iovec*>(iov);
    msg.msg_iovlen = count;

    auto& metrics = detail::syscall_metrics_registry::instance();
    ssize_t n;
    do {
        n = ::sendmsg(fd_, &msg, WRITE_FLAGS);
        metrics.note_send(n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK));
    } while (n < 0 && errno == EINTR);

    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return size_t{0};
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
