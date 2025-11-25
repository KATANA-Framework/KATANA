#pragma once

#include <algorithm>
#include <chrono>
#include <thread>

namespace katana {

class chunk_sleep {
public:
    using clock = std::chrono::steady_clock;
    using duration = std::chrono::milliseconds;

    chunk_sleep() = default;

    chunk_sleep(duration /*total*/, duration chunk) { configure(chunk); }

    void configure(duration chunk) {
        if (chunk.count() <= 0) {
            chunk_ = duration{0};
            active_ = false;
            return;
        }
        chunk_ = chunk;
        active_ = true;
    }

    [[nodiscard]] bool is_valid() const noexcept { return active_; }

    void wait(clock::time_point start, duration timeout) const {
        if (!active_) {
            return;
        }

        auto now = clock::now();
        auto elapsed = std::chrono::duration_cast<duration>(now - start);
        while (elapsed < timeout) {
            auto remaining = timeout - elapsed;
            auto slice = std::min(chunk_, remaining);
            std::this_thread::sleep_for(slice);
            now = clock::now();
            elapsed = std::chrono::duration_cast<duration>(now - start);
        }
    }

private:
    duration chunk_{0};
    bool active_{false};
};

class Timeout {
public:
    using Clock = std::chrono::steady_clock;
    using Duration = std::chrono::milliseconds;

    Timeout() = default;

    explicit Timeout(Duration timeoutDuration) : timeout_duration_(timeoutDuration) {
        reset_internal();
    }

    Timeout(Duration timeoutDuration, Duration sleepTime)
        : timeout_duration_(timeoutDuration), sleep_interval_(sleepTime) {
        sleeper_.configure(sleep_interval_);
        reset_internal();
    }

    void reset() { reset_internal(); }

    void enableAutoreset(bool enable) noexcept { enable_autoreset_ = enable; }

    void setTimeout(Duration newTimeout) {
        timeout_duration_ = newTimeout;
        if (sleep_interval_.count() > 0) {
            sleeper_.configure(sleep_interval_);
        }
        reset_internal();
    }

    explicit operator bool() {
        if (!active_) {
            return false;
        }

        if (timeElapsed() >= timeout_duration_) {
            if (enable_autoreset_) {
                reset_internal();
            } else {
                active_ = false;
            }
            return true;
        }
        return false;
    }

    Duration timeElapsed() const {
        if (!active_) {
            return Duration{0};
        }
        return std::chrono::duration_cast<Duration>(Clock::now() - start_time_);
    }

    Duration timeRemaining() const {
        if (!active_) {
            return Duration{0};
        }
        auto elapsed = timeElapsed();
        if (elapsed >= timeout_duration_) {
            return Duration{0};
        }
        return timeout_duration_ - elapsed;
    }

    void wait() const {
        if (!active_) {
            return;
        }
        if (sleeper_.is_valid()) {
            sleeper_.wait(start_time_, timeout_duration_);
        } else {
            auto remaining = timeRemaining();
            if (remaining.count() > 0) {
                std::this_thread::sleep_for(remaining);
            }
        }
    }

    [[nodiscard]] Duration duration() const noexcept { return timeout_duration_; }
    [[nodiscard]] bool active() const noexcept { return active_; }

private:
    void reset_internal() {
        if (timeout_duration_.count() <= 0) {
            active_ = false;
            start_time_ = Clock::now();
            return;
        }
        active_ = true;
        start_time_ = Clock::now();
    }

    Duration timeout_duration_{0};
    Duration sleep_interval_{0};
    Clock::time_point start_time_{Clock::now()};
    chunk_sleep sleeper_{};
    bool enable_autoreset_{false};
    bool active_{false};
};

} // namespace katana
