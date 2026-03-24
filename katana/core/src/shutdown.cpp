#include "katana/core/shutdown.hpp"

#include <csignal>
#include <cerrno>
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>

#include <atomic>
#include <utility>

namespace katana {

namespace {

std::atomic<int> g_shutdown_signal_write_fd{-1};

void signal_handler(int signal) {
    const int fd = g_shutdown_signal_write_fd.load(std::memory_order_relaxed);
    if (fd < 0) {
        return;
    }

    const std::uint8_t value = static_cast<std::uint8_t>(signal);
    ssize_t written = 0;
    do {
        written = write(fd, &value, sizeof(value));
    } while (written < 0 && errno == EINTR);
}

} // namespace

shutdown_manager::~shutdown_manager() {
    stop_signal_dispatcher();
}

void shutdown_manager::record_shutdown_time() noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    shutdown_time_ = clock::now();
}

shutdown_manager::time_point shutdown_manager::shutdown_time() const noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    return shutdown_time_;
}

bool shutdown_manager::is_deadline_exceeded(duration deadline) noexcept {
    if (!is_shutdown_requested()) {
        return false;
    }

    const auto now = clock::now();
    std::lock_guard<std::mutex> lock(mutex_);
    if (shutdown_time_ == time_point{}) {
        shutdown_time_ = now;
    }
    return (now - shutdown_time_) >= deadline;
}

void shutdown_manager::set_shutdown_callback(shutdown_callback cb) {
    std::lock_guard<std::mutex> lock(mutex_);
    callback_ = std::move(cb);
}

void shutdown_manager::trigger_shutdown() {
    request_shutdown();

    shutdown_callback callback;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        callback = callback_;
    }

    if (callback) {
        callback();
    }
}

void shutdown_manager::signal_dispatch_loop() {
    int read_fd = -1;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        read_fd = signal_read_fd_;
    }

    while (read_fd >= 0) {
        std::uint8_t signal_value = 0;
        ssize_t read_result = 0;
        do {
            read_result = read(read_fd, &signal_value, sizeof(signal_value));
        } while (read_result < 0 && errno == EINTR);

        if (read_result == 0) {
            break;
        }
        if (read_result < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            break;
        }

        try {
            trigger_shutdown();
        } catch (...) {
            // Signal-driven shutdown must not terminate the dispatcher thread.
        }
    }
}

void shutdown_manager::stop_signal_dispatcher() noexcept {
    g_shutdown_signal_write_fd.store(-1, std::memory_order_release);

    int read_fd = -1;
    int write_fd = -1;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        read_fd = signal_read_fd_;
        write_fd = signal_write_fd_;
    }

    if (write_fd >= 0) {
        close(write_fd);
    }

    if (signal_dispatch_thread_.joinable()) {
        signal_dispatch_thread_.join();
    }

    if (read_fd >= 0) {
        close(read_fd);
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        signal_read_fd_ = -1;
        signal_write_fd_ = -1;
    }
}

void shutdown_manager::setup_signal_handlers() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (signal_dispatch_thread_.joinable()) {
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);
        return;
    }

    int pipe_fds[2] = {-1, -1};
#if defined(__linux__)
    if (pipe2(pipe_fds, O_CLOEXEC) != 0) {
        return;
    }
#else
    if (pipe(pipe_fds) != 0) {
        return;
    }
    (void)fcntl(pipe_fds[0], F_SETFD, FD_CLOEXEC);
    (void)fcntl(pipe_fds[1], F_SETFD, FD_CLOEXEC);
#endif

    int write_flags = fcntl(pipe_fds[1], F_GETFL, 0);
    if (write_flags >= 0) {
        (void)fcntl(pipe_fds[1], F_SETFL, write_flags | O_NONBLOCK);
    }

    signal_read_fd_ = pipe_fds[0];
    signal_write_fd_ = pipe_fds[1];
    g_shutdown_signal_write_fd.store(signal_write_fd_, std::memory_order_release);
    try {
        signal_dispatch_thread_ = std::thread([this] { signal_dispatch_loop(); });
    } catch (...) {
        g_shutdown_signal_write_fd.store(-1, std::memory_order_release);
        close(signal_write_fd_);
        close(signal_read_fd_);
        signal_write_fd_ = -1;
        signal_read_fd_ = -1;
        return;
    }

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
}

} // namespace katana
