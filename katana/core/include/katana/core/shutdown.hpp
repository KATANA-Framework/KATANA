#pragma once

#include <atomic>
#include <chrono>
#include <functional>

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

    void record_shutdown_time() noexcept { shutdown_time_ = clock::now(); }

    [[nodiscard]] bool is_deadline_exceeded(duration deadline = std::chrono::seconds(30)) noexcept {
        if (!is_shutdown_requested()) {
            return false;
        }
        if (shutdown_time_ == time_point{}) {
            shutdown_time_ = clock::now();
        }
        auto elapsed = clock::now() - shutdown_time_;
        return elapsed >= deadline;
    }

    [[nodiscard]] time_point shutdown_time() const noexcept { return shutdown_time_; }

    void set_shutdown_callback(shutdown_callback cb) { callback_ = std::move(cb); }

    void trigger_shutdown() {
        request_shutdown();
        if (callback_) {
            callback_();
        }
    }

    void setup_signal_handlers();

private:
    shutdown_manager() = default;

    std::atomic<bool> shutdown_requested_{false};
    time_point shutdown_time_;
    shutdown_callback callback_;
};

} // namespace katana
