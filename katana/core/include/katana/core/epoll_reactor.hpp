#pragma once

#include "reactor.hpp"
#include "metrics.hpp"
#include "mpsc_queue.hpp"

#include <atomic>
#include <unordered_map>
#include <vector>
#include <queue>

namespace katana {

class epoll_reactor : public reactor {
public:
    static constexpr size_t DEFAULT_MAX_PENDING_TASKS = 10000;

    explicit epoll_reactor(int max_events = 128, size_t max_pending_tasks = DEFAULT_MAX_PENDING_TASKS);
    ~epoll_reactor() override;

    epoll_reactor(const epoll_reactor&) = delete;
    epoll_reactor& operator=(const epoll_reactor&) = delete;

    result<void> run() override;
    void stop() override;

    result<void> register_fd(
        int fd,
        event_type events,
        event_callback callback
    ) override;

    result<void> modify_fd(
        int fd,
        event_type events
    ) override;

    result<void> unregister_fd(int fd) override;

    bool schedule(task_fn task) override;

    bool schedule_after(
        std::chrono::milliseconds delay,
        task_fn task
    ) override;

    void set_exception_handler(exception_handler handler) override;

    const reactor_metrics& metrics() const noexcept { return metrics_; }

private:
    struct fd_state {
        event_callback callback;
        event_type events;
    };

    struct timer_entry {
        std::chrono::steady_clock::time_point deadline;
        task_fn task;

        bool operator>(const timer_entry& other) const {
            return deadline > other.deadline;
        }
    };

    result<void> process_events(int timeout_ms);
    void process_tasks();
    void process_timers();
    int calculate_timeout() const;
    void handle_exception(std::string_view location, std::exception_ptr ex, int fd = -1) noexcept;

    int epoll_fd_;
    int max_events_;
    std::atomic<bool> running_;

    std::unordered_map<int, fd_state> fd_states_;
    mpsc_queue<task_fn> pending_tasks_;
    std::priority_queue<timer_entry, std::vector<timer_entry>, std::greater<timer_entry>> timers_;
    mpsc_queue<timer_entry> pending_timers_;

    exception_handler exception_handler_;
    reactor_metrics metrics_;

    // Reusable buffer for epoll_wait to avoid allocations in hot path
    std::vector<epoll_event> events_buffer_;
};

} // namespace katana
