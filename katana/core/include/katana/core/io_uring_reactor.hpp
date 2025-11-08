#pragma once

#include "reactor.hpp"
#include "metrics.hpp"
#include "ring_buffer_queue.hpp"
#include "wheel_timer.hpp"
#include "timeout.hpp"

#include <liburing.h>
#include <atomic>
#include <unordered_map>
#include <vector>
#include <queue>

namespace katana {

class io_uring_reactor : public reactor {
public:
    static constexpr size_t DEFAULT_MAX_PENDING_TASKS = 10000;
    static constexpr size_t DEFAULT_RING_SIZE = 4096;

    explicit io_uring_reactor(
        size_t ring_size = DEFAULT_RING_SIZE,
        size_t max_pending_tasks = DEFAULT_MAX_PENDING_TASKS
    );
    ~io_uring_reactor() override;

    io_uring_reactor(const io_uring_reactor&) = delete;
    io_uring_reactor& operator=(const io_uring_reactor&) = delete;

    result<void> run() override;
    void stop() override;
    void graceful_stop(std::chrono::milliseconds timeout) override;

    result<void> register_fd(
        int32_t fd,
        event_type events,
        event_callback callback
    ) override;

    result<void> register_fd_with_timeout(
        int32_t fd,
        event_type events,
        event_callback callback,
        const timeout_config& config
    ) override;

    result<void> modify_fd(
        int32_t fd,
        event_type events
    ) override;

    result<void> unregister_fd(int32_t fd) override;

    void refresh_fd_timeout(int32_t fd) override;

    bool schedule(task_fn task) override;

    bool schedule_after(
        std::chrono::milliseconds delay,
        task_fn task
    ) override;

    void set_exception_handler(exception_handler handler) override;

    const reactor_metrics& metrics() const noexcept { return metrics_; }

    [[nodiscard]] uint64_t get_load_score() const noexcept;

private:
    using fd_wheel_timer = wheel_timer<2048, 8>;

    enum class op_type : uint8_t {
        poll_add,
        poll_remove,
        cancel,
    };

    struct alignas(64) fd_state {
        // Hot data - frequently accessed
        event_callback callback;
        event_type events;
        fd_wheel_timer::timeout_id timeout_id = 0;
        bool has_timeout = false;
        bool registered = false;

        // Cold data - rarely accessed
        char padding1[64 - sizeof(event_callback) - sizeof(event_type) -
                      sizeof(fd_wheel_timer::timeout_id) - sizeof(bool) * 2];

        timeout_config timeouts;
        Timeout activity_timer;
    };

    struct timer_entry {
        std::chrono::steady_clock::time_point deadline;
        task_fn task;

        bool operator>(const timer_entry& other) const {
            return deadline > other.deadline;
        }
    };

    result<void> submit_poll_add(int32_t fd, event_type events);
    result<void> submit_poll_remove(int32_t fd);
    result<void> process_completions(int32_t timeout_ms);
    void process_tasks();
    void process_timers();
    void process_wheel_timer();
    int32_t calculate_timeout() const;
    void handle_exception(std::string_view location, std::exception_ptr ex, int32_t fd = -1) noexcept;
    void setup_fd_timeout(int32_t fd, fd_state& state);
    void cancel_fd_timeout(fd_state& state);
    std::chrono::milliseconds fd_timeout_for(const fd_state& state) const;
    result<void> ensure_fd_capacity(int32_t fd);
    std::chrono::milliseconds time_until_graceful_deadline(std::chrono::steady_clock::time_point now) const;

    io_uring ring_;
    int32_t wakeup_fd_;
    std::atomic<bool> running_;
    std::atomic<bool> graceful_shutdown_;
    std::chrono::steady_clock::time_point graceful_shutdown_deadline_;

    std::vector<fd_state> fd_states_;
    ring_buffer_queue<task_fn> pending_tasks_;
    std::priority_queue<timer_entry, std::vector<timer_entry>, std::greater<timer_entry>> timers_;
    ring_buffer_queue<timer_entry> pending_timers_;

    alignas(64) std::atomic<size_t> active_fds_{0};
    alignas(64) std::atomic<bool> needs_wakeup_{false};
    alignas(64) std::atomic<uint32_t> pending_count_{0};
    exception_handler exception_handler_;
    reactor_metrics metrics_;

    fd_wheel_timer wheel_timer_;

    mutable int32_t cached_timeout_ = -1;
    mutable std::chrono::steady_clock::time_point timeout_cached_at_;
    mutable std::atomic<bool> timeout_dirty_{true};
};

} // namespace katana
