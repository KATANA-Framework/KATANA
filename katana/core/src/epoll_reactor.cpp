#include "katana/core/epoll_reactor.hpp"

#include <sys/epoll.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>

namespace katana {

namespace {

constexpr uint32_t to_epoll_events(event_type events) noexcept {
    uint32_t result = 0;

    if (has_flag(events, event_type::readable)) {
        result |= EPOLLIN;
    }
    if (has_flag(events, event_type::writable)) {
        result |= EPOLLOUT;
    }
    if (has_flag(events, event_type::edge_triggered)) {
        result |= EPOLLET;
    }
    if (has_flag(events, event_type::oneshot)) {
        result |= EPOLLONESHOT;
    }

    return result;
}

constexpr event_type from_epoll_events(uint32_t events) noexcept {
    event_type result = event_type::none;

    if (events & EPOLLIN) {
        result = result | event_type::readable;
    }
    if (events & EPOLLOUT) {
        result = result | event_type::writable;
    }
    if (events & EPOLLERR) {
        result = result | event_type::error;
    }
    if (events & EPOLLHUP) {
        result = result | event_type::hup;
    }

    return result;
}

} // namespace

epoll_reactor::epoll_reactor(int max_events, size_t max_pending_tasks)
    : epoll_fd_(-1)
    , max_events_(max_events)
    , running_(false)
    , graceful_shutdown_(false)
    , pending_tasks_(max_pending_tasks)
    , pending_timers_(max_pending_tasks)
    , exception_handler_([](const exception_context& ctx) {
        std::cerr << "[reactor] Exception in " << ctx.location;
        if (ctx.fd >= 0) {
            std::cerr << " (fd=" << ctx.fd << ")";
        }
        std::cerr << ": ";
        try {
            if (ctx.exception) {
                std::rethrow_exception(ctx.exception);
            }
        } catch (const std::exception& e) {
            std::cerr << e.what();
        } catch (...) {
            std::cerr << "unknown exception";
        }
        std::cerr << "\n";
    })
{
    epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd_ < 0) {
        throw std::system_error(
            errno,
            std::system_category(),
            "epoll_create1 failed"
        );
    }

    last_wheel_tick_ = std::chrono::steady_clock::now();
    events_buffer_.resize(max_events_);
}

epoll_reactor::~epoll_reactor() {
    if (epoll_fd_ >= 0) {
        close(epoll_fd_);
    }
}

result<void> epoll_reactor::run() {
    if (running_.exchange(true)) {
        return std::unexpected(make_error_code(error_code::reactor_stopped));
    }

    while (running_.load(std::memory_order_relaxed)) {
        process_wheel_timer();
        process_timers();
        process_tasks();

        if (graceful_shutdown_.load(std::memory_order_relaxed)) {
            auto now = std::chrono::steady_clock::now();
            if (fd_states_.empty()) {
                running_ = false;
                break;
            }
            if (now >= graceful_shutdown_deadline_) {
                std::vector<int> fds_to_close;
                fds_to_close.reserve(fd_states_.size());
                for (const auto& [fd, _] : fd_states_) {
                    fds_to_close.push_back(fd);
                }
                for (int fd : fds_to_close) {
                    auto it = fd_states_.find(fd);
                    if (it == fd_states_.end()) continue;
                    try {
                        it->second.callback(event_type::error);
                    } catch (...) {
                        handle_exception("forced_shutdown_callback", std::current_exception(), fd);
                    }
                    it = fd_states_.find(fd);
                    if (it != fd_states_.end()) {
                        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);
                        close(fd);
                        fd_states_.erase(it);
                    }
                }
                running_ = false;
                break;
            }
        }

        int timeout_ms = calculate_timeout();
        auto res = process_events(timeout_ms);
        if (!res) {
            running_ = false;
            return res;
        }
    }

    return {};
}

void epoll_reactor::stop() {
    running_.store(false, std::memory_order_relaxed);
}

void epoll_reactor::graceful_stop(std::chrono::milliseconds timeout) {
    graceful_shutdown_.store(true, std::memory_order_relaxed);
    graceful_shutdown_deadline_ = std::chrono::steady_clock::now() + timeout;
}

result<void> epoll_reactor::register_fd(
    int fd,
    event_type events,
    event_callback callback
) {
    if (fd < 0) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    epoll_event ev{};
    ev.events = to_epoll_events(events);
    ev.data.fd = fd;

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    fd_states_[fd] = fd_state{std::move(callback), events, {}, 0,
                              std::chrono::steady_clock::now(), false};
    return {};
}

result<void> epoll_reactor::register_fd_with_timeout(
    int fd,
    event_type events,
    event_callback callback,
    const timeout_config& config
) {
    if (fd < 0) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    fd_state state{std::move(callback), events, config, 0,
                   std::chrono::steady_clock::now(), true};
    setup_fd_timeout(fd, state);

    epoll_event ev{};
    ev.events = to_epoll_events(events);
    ev.data.fd = fd;

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) < 0) {
        cancel_fd_timeout(state);
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    fd_states_[fd] = std::move(state);
    return {};
}

result<void> epoll_reactor::modify_fd(int fd, event_type events) {
    if (fd < 0) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    auto it = fd_states_.find(fd);
    if (it == fd_states_.end()) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    epoll_event ev{};
    ev.events = to_epoll_events(events);
    ev.data.fd = fd;

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev) < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    it->second.events = events;
    return {};
}

result<void> epoll_reactor::unregister_fd(int fd) {
    if (fd < 0) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    auto it = fd_states_.find(fd);
    if (it == fd_states_.end()) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    cancel_fd_timeout(it->second);

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr) < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    fd_states_.erase(it);
    return {};
}

void epoll_reactor::refresh_fd_timeout(int fd) {
    auto it = fd_states_.find(fd);
    if (it != fd_states_.end() && it->second.has_timeout) {
        it->second.last_activity = std::chrono::steady_clock::now();
        cancel_fd_timeout(it->second);
        setup_fd_timeout(fd, it->second);
    }
}

bool epoll_reactor::schedule(task_fn task) {
    if (!pending_tasks_.try_push(std::move(task))) {
        metrics_.tasks_rejected.fetch_add(1, std::memory_order_relaxed);
        return false;
    }
    metrics_.tasks_scheduled.fetch_add(1, std::memory_order_relaxed);
    return true;
}

bool epoll_reactor::schedule_after(
    std::chrono::milliseconds delay,
    task_fn task
) {
    auto deadline = std::chrono::steady_clock::now() + delay;
    if (!pending_timers_.try_push(timer_entry{deadline, std::move(task)})) {
        metrics_.tasks_rejected.fetch_add(1, std::memory_order_relaxed);
        return false;
    }
    metrics_.tasks_scheduled.fetch_add(1, std::memory_order_relaxed);
    return true;
}

result<void> epoll_reactor::process_events(int timeout_ms) {
    events_buffer_.resize(max_events_);

    int nfds = epoll_wait(epoll_fd_, events_buffer_.data(), max_events_, timeout_ms);

    if (nfds < 0) {
        if (errno == EINTR) {
            return {};
        }
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    for (int i = 0; i < nfds; ++i) {
        int fd = events_buffer_[i].data.fd;
        auto it = fd_states_.find(fd);
        if (it != fd_states_.end()) {
            event_type ev = from_epoll_events(events_buffer_[i].events);

            if (it->second.has_timeout) {
                it->second.last_activity = std::chrono::steady_clock::now();
                cancel_fd_timeout(it->second);
                setup_fd_timeout(fd, it->second);
            }

            try {
                it->second.callback(ev);
                metrics_.fd_events_processed.fetch_add(1, std::memory_order_relaxed);
            } catch (...) {
                handle_exception("fd_callback", std::current_exception(), fd);
            }
        }
    }

    return {};
}

void epoll_reactor::process_tasks() {
    while (auto task = pending_tasks_.pop()) {
        try {
            (*task)();
            metrics_.tasks_executed.fetch_add(1, std::memory_order_relaxed);
        } catch (...) {
            handle_exception("scheduled_task", std::current_exception());
        }
    }
}

void epoll_reactor::process_timers() {
    while (auto timer = pending_timers_.pop()) {
        timers_.push(std::move(*timer));
    }

    auto now = std::chrono::steady_clock::now();

    while (!timers_.empty() && timers_.top().deadline <= now) {
        auto task = std::move(timers_.top().task);
        timers_.pop();

        try {
            task();
            metrics_.tasks_executed.fetch_add(1, std::memory_order_relaxed);
            metrics_.timers_fired.fetch_add(1, std::memory_order_relaxed);
        } catch (...) {
            handle_exception("delayed_task", std::current_exception());
        }
    }
}

int epoll_reactor::calculate_timeout() const {
    if (!pending_tasks_.empty()) {
        return 0;
    }

    if (timers_.empty()) {
        return 100;
    }

    auto now = std::chrono::steady_clock::now();
    auto deadline = timers_.top().deadline;

    if (deadline <= now) {
        return 0;
    }

    auto timeout = std::chrono::duration_cast<std::chrono::milliseconds>(
        deadline - now
    );

    return static_cast<int>(std::min<int64_t>(timeout.count(), 100));
}

void epoll_reactor::set_exception_handler(exception_handler handler) {
    exception_handler_ = std::move(handler);
}

void epoll_reactor::process_wheel_timer() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_wheel_tick_);

    if (elapsed.count() >= static_cast<int64_t>(wheel_timer<>::TICK_MS)) {
        wheel_timer_.tick();
        last_wheel_tick_ = now;
    }
}

void epoll_reactor::setup_fd_timeout(int fd, fd_state& state) {
    state.timeout_id = wheel_timer_.add(
        state.timeouts.idle_timeout,
        [this, fd]() {
            auto it = fd_states_.find(fd);
            if (it != fd_states_.end()) {
                try {
                    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);
                    close(fd);
                    fd_states_.erase(it);
                } catch (...) {
                    handle_exception("timeout_handler", std::current_exception(), fd);
                }
            }
        }
    );
}

void epoll_reactor::cancel_fd_timeout(fd_state& state) {
    if (state.timeout_id != 0) {
        wheel_timer_.cancel(state.timeout_id);
        state.timeout_id = 0;
    }
}

void epoll_reactor::handle_exception(
    std::string_view location,
    std::exception_ptr ex,
    int fd
) noexcept {
    metrics_.exceptions_caught.fetch_add(1, std::memory_order_relaxed);

    if (exception_handler_) {
        try {
            exception_handler_(exception_context{location, ex, fd});
        } catch (...) {
            std::cerr << "[reactor] Exception handler threw an exception!\n";
        }
    }
}

} // namespace katana
