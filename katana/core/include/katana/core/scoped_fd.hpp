#pragma once

#include <cstdint>
#include <utility>

#ifdef __linux__
#include <unistd.h>
#endif

namespace katana {

/**
 * RAII wrapper for file descriptors.
 * Automatically closes the file descriptor when the object goes out of scope.
 * Move-only type to prevent accidental double-close.
 */
class scoped_fd {
public:
    scoped_fd() noexcept = default;

    explicit scoped_fd(int32_t fd) noexcept : fd_(fd) {}

    ~scoped_fd() noexcept { close_fd(); }

    // Move constructor
    scoped_fd(scoped_fd&& other) noexcept : fd_(other.fd_) { other.fd_ = -1; }

    // Move assignment
    scoped_fd& operator=(scoped_fd&& other) noexcept {
        if (this != &other) {
            close_fd();
            fd_ = other.fd_;
            other.fd_ = -1;
        }
        return *this;
    }

    // Delete copy operations
    scoped_fd(const scoped_fd&) = delete;
    scoped_fd& operator=(const scoped_fd&) = delete;

    /**
     * Release ownership of the file descriptor without closing it.
     * Returns the file descriptor and sets the internal fd to -1.
     */
    [[nodiscard]] int32_t release() noexcept { return std::exchange(fd_, -1); }

    /**
     * Get the file descriptor value without releasing ownership.
     */
    [[nodiscard]] int32_t get() const noexcept { return fd_; }

    /**
     * Check if the file descriptor is valid.
     */
    [[nodiscard]] bool is_valid() const noexcept { return fd_ >= 0; }

    /**
     * Explicit bool conversion for convenience.
     */
    explicit operator bool() const noexcept { return is_valid(); }

    /**
     * Reset the file descriptor, closing the current one if valid.
     */
    void reset(int32_t new_fd = -1) noexcept {
        if (fd_ >= 0 && fd_ != new_fd) {
            close_fd();
        }
        fd_ = new_fd;
    }

private:
    void close_fd() noexcept {
        if (fd_ >= 0) {
#ifdef __linux__
            ::close(fd_);
#endif
            fd_ = -1;
        }
    }

    int32_t fd_{-1};
};

} // namespace katana
