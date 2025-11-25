#pragma once

#include "fd_event.hpp"
#include "reactor.hpp"
#include "result.hpp"

#include <cstdint>
#include <utility>

namespace katana {

class fd_watch {
public:
    fd_watch() = default;

    fd_watch(reactor& r, int32_t fd, event_type events, event_callback cb) : reactor_(&r), fd_(fd) {
        auto res = reactor_->register_fd(fd, events, std::move(cb));
        if (!res) {
            reactor_ = nullptr;
            fd_ = -1;
        }
    }

    fd_watch(
        reactor& r, int32_t fd, event_type events, event_callback cb, const timeout_config& config)
        : reactor_(&r), fd_(fd) {
        auto res = reactor_->register_fd_with_timeout(fd, events, std::move(cb), config);
        if (!res) {
            reactor_ = nullptr;
            fd_ = -1;
        }
    }

    ~fd_watch() { unregister(); }

    fd_watch(fd_watch&& other) noexcept
        : reactor_(std::exchange(other.reactor_, nullptr)), fd_(std::exchange(other.fd_, -1)) {}

    fd_watch& operator=(fd_watch&& other) noexcept {
        if (this != &other) {
            unregister();
            reactor_ = std::exchange(other.reactor_, nullptr);
            fd_ = std::exchange(other.fd_, -1);
        }
        return *this;
    }

    fd_watch(const fd_watch&) = delete;
    fd_watch& operator=(const fd_watch&) = delete;

    result<void> modify(event_type events) {
        if (!reactor_ || fd_ < 0) {
            return std::unexpected(make_error_code(error_code::invalid_fd));
        }
        return reactor_->modify_fd(fd_, events);
    }

    void refresh_timeout() {
        if (reactor_ && fd_ >= 0) {
            reactor_->refresh_fd_timeout(fd_);
        }
    }

    void unregister() noexcept {
        if (reactor_ && fd_ >= 0) {
            (void)reactor_->unregister_fd(fd_);
            reactor_ = nullptr;
            fd_ = -1;
        }
    }

    [[nodiscard]] bool is_registered() const noexcept { return reactor_ != nullptr && fd_ >= 0; }

    [[nodiscard]] int32_t fd() const noexcept { return fd_; }

private:
    reactor* reactor_{nullptr};
    int32_t fd_{-1};
};

} // namespace katana
