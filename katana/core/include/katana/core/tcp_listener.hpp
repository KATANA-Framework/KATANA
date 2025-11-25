#pragma once

#include "result.hpp"
#include "tcp_socket.hpp"

#include <cstdint>
#include <system_error>

namespace katana {

class tcp_listener {
public:
    tcp_listener() = default;

    explicit tcp_listener(uint16_t port, bool ipv6 = false);

    tcp_listener(tcp_listener&& other) noexcept : socket_(std::move(other.socket_)) {}

    tcp_listener& operator=(tcp_listener&& other) noexcept {
        if (this != &other) {
            socket_ = std::move(other.socket_);
        }
        return *this;
    }

    tcp_listener(const tcp_listener&) = delete;
    tcp_listener& operator=(const tcp_listener&) = delete;

    result<tcp_socket> accept();

    tcp_listener& set_reuseaddr(bool enable);
    tcp_listener& set_reuseport(bool enable);
    tcp_listener& set_backlog(int32_t backlog);

    [[nodiscard]] int32_t native_handle() const noexcept { return socket_.native_handle(); }

    [[nodiscard]] explicit operator bool() const noexcept { return static_cast<bool>(socket_); }

private:
    result<void> create_and_bind(uint16_t port, bool ipv6);

    tcp_socket socket_;
    int32_t backlog_{1024};
};

} // namespace katana
