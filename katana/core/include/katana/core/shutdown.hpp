#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <thread>

namespace katana {

class shutdown_manager {
public:
    using shutdown_callback = std::function<void()>;
    using clock = std::chrono::steady_clock;
    using time_point = clock::time_point;
    using duration = std::chrono::milliseconds;

    static shutdown_manager& instance() {
        static shutdown_manager mgr;
        return mgr;
    }

    void request_shutdown() noexcept { shutdown_requested_.store(true, std::memory_order_release); }

    [[nodiscard]] bool is_shutdown_requested() const noexcept {
        return shutdown_requested_.load(std::memory_order_acquire);
    }

    void record_shutdown_time() noexcept;

    [[nodiscard]] bool is_deadline_exceeded(duration deadline = std::chrono::seconds(30)) noexcept;

    [[nodiscard]] time_point shutdown_time() const noexcept;

    void set_shutdown_callback(shutdown_callback cb);

    void trigger_shutdown();

    void setup_signal_handlers();
    ~shutdown_manager();

    shutdown_manager(const shutdown_manager&) = delete;
    shutdown_manager& operator=(const shutdown_manager&) = delete;

private:
    shutdown_manager() = default;
    void signal_dispatch_loop();
    void stop_signal_dispatcher() noexcept;

    std::atomic<bool> shutdown_requested_{false};
    mutable std::mutex mutex_;
    time_point shutdown_time_;
    shutdown_callback callback_;
    std::thread signal_dispatch_thread_;
    int signal_read_fd_ = -1;
    int signal_write_fd_ = -1;
};

} // namespace katana
