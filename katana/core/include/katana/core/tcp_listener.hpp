#pragma once

#include "tcp_socket.hpp"
#include "result.hpp"

#include <cstdint>
#include <system_error>

namespace katana {

class tcp_listener {
public:
    tcp_listener() = default;

    static result<tcp_listener> create(uint16_t port, bool ipv6 = false);

    tcp_listener(tcp_listener&& other) noexcept
        : socket_(std::move(other.socket_)), backlog_(other.backlog_) {}

    tcp_listener& operator=(tcp_listener&& other) noexcept {
        if (this != &other) {
            socket_ = std::move(other.socket_);
            backlog_ = other.backlog_;
        }
        return *this;
    }

    tcp_listener(const tcp_listener&) = delete;
    tcp_listener& operator=(const tcp_listener&) = delete;

    result<tcp_socket> accept();

    tcp_listener& set_reuseaddr(bool enable);
    tcp_listener& set_reuseport(bool enable);
    tcp_listener& set_backlog(int32_t backlog);

    [[nodiscard]] int32_t native_handle() const noexcept {
        return socket_.native_handle();
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return static_cast<bool>(socket_);
    }

private:
    explicit tcp_listener(tcp_socket socket, int32_t backlog)
        : socket_(std::move(socket)), backlog_(backlog) {}

    tcp_socket socket_;
    int32_t backlog_{1024};
};

} // namespace katana
