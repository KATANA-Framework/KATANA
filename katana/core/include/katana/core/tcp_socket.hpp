#pragma once

#include "result.hpp"

#include <cstdint>
#include <span>
#include <utility>

namespace katana {

class tcp_socket {
public:
    tcp_socket() = default;

    explicit tcp_socket(int32_t fd) noexcept : fd_(fd) {}

    ~tcp_socket() { close(); }

    tcp_socket(tcp_socket&& other) noexcept : fd_(std::exchange(other.fd_, -1)) {}

    tcp_socket& operator=(tcp_socket&& other) noexcept {
        if (this != &other) {
            close();
            fd_ = std::exchange(other.fd_, -1);
        }
        return *this;
    }

    tcp_socket(const tcp_socket&) = delete;
    tcp_socket& operator=(const tcp_socket&) = delete;

    result<std::span<uint8_t>> read(std::span<uint8_t> buf);
    result<size_t> write(std::span<const uint8_t> data);

    void close() noexcept;

    [[nodiscard]] int32_t native_handle() const noexcept { return fd_; }
    [[nodiscard]] explicit operator bool() const noexcept { return fd_ >= 0; }

    [[nodiscard]] int32_t release() noexcept { return std::exchange(fd_, -1); }

private:
    int32_t fd_{-1};
};

} // namespace katana
