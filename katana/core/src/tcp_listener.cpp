#include "katana/core/tcp_listener.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

namespace katana {

result<tcp_listener> tcp_listener::create(uint16_t port, bool ipv6) {
    int32_t fd = ::socket(ipv6 ? AF_INET6 : AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (fd < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    tcp_socket socket(fd);

    int opt = 1;
    if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    if (ipv6) {
        sockaddr_in6 addr{};
        addr.sin6_family = AF_INET6;
        addr.sin6_addr = in6addr_any;
        addr.sin6_port = htons(port);

        if (::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
            return std::unexpected(std::error_code(errno, std::system_category()));
        }
    } else {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        if (::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
            return std::unexpected(std::error_code(errno, std::system_category()));
        }
    }

    constexpr int32_t default_backlog = 1024;
    if (::listen(fd, default_backlog) < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    return tcp_listener(std::move(socket), default_backlog);
}

result<tcp_socket> tcp_listener::accept() {
    int32_t fd;
    do {
        fd = ::accept4(socket_.native_handle(), nullptr, nullptr,
                      SOCK_NONBLOCK | SOCK_CLOEXEC);
    } while (fd < 0 && errno == EINTR);

    if (fd < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    return tcp_socket(fd);
}

tcp_listener& tcp_listener::set_reuseaddr(bool enable) {
    int opt = enable ? 1 : 0;
    ::setsockopt(socket_.native_handle(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    return *this;
}

tcp_listener& tcp_listener::set_reuseport(bool enable) {
    int opt = enable ? 1 : 0;
    ::setsockopt(socket_.native_handle(), SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    return *this;
}

tcp_listener& tcp_listener::set_backlog(int32_t backlog) {
    backlog_ = backlog;
    if (socket_) {
        ::listen(socket_.native_handle(), backlog_);
    }
    return *this;
}

} // namespace katana
