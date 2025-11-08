#include "katana/core/io_uring_reactor.hpp"

#include <poll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <system_error>
#include <algorithm>

namespace katana {

namespace {

constexpr uint32_t to_poll_events(event_type events) noexcept {
    uint32_t result = 0;

    if (has_flag(events, event_type::readable)) {
        result |= POLLIN;
    }
    if (has_flag(events, event_type::writable)) {
        result |= POLLOUT;
    }

    return result;
}

constexpr event_type from_poll_events(uint32_t events) noexcept {
    event_type result = event_type::none;

    if (events & POLLIN) {
        result = result | event_type::readable;
    }
    if (events & POLLOUT) {
        result = result | event_type::writable;
    }
    if (events & POLLERR) {
        result = result | event_type::error;
    }
    if (events & POLLHUP) {
        result = result | event_type::hup;
    }

    return result;
}

} // namespace

io_uring_reactor::io_uring_reactor(size_t ring_size, size_t max_pending_tasks)
    : wakeup_fd_(-1)
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
    io_uring_params params{};
    params.flags = IORING_SETUP_CQSIZE;
    params.cq_entries = static_cast<__u32>(ring_size * 2);

    int ret = io_uring_queue_init_params(static_cast<unsigned int>(ring_size), &ring_, &params);
    if (ret < 0) {
        throw std::system_error(
            -ret,
            std::system_category(),
            "io_uring_queue_init_params failed"
        );
    }

    wakeup_fd_ = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (wakeup_fd_ < 0) {
        io_uring_queue_exit(&ring_);
        throw std::system_error(
            errno,
            std::system_category(),
            "eventfd failed"
        );
    }

    fd_states_.reserve(65536);
}

io_uring_reactor::~io_uring_reactor() {
    if (wakeup_fd_ >= 0) {
        close(wakeup_fd_);
    }
    io_uring_queue_exit(&ring_);
}

result<void> io_uring_reactor::run() {
    if (running_.exchange(true)) {
        return std::unexpected(make_error_code(error_code::reactor_stopped));
    }

    auto wakeup_res = register_fd(
        wakeup_fd_,
        event_type::readable | event_type::edge_triggered,
        [this](event_type) {
            uint64_t val;
            ssize_t ret = read(wakeup_fd_, &val, sizeof(val));
            (void)ret;
        }
    );
    if (!wakeup_res) {
        running_ = false;
        return wakeup_res;
    }

    while (running_.load(std::memory_order_relaxed)) {
        process_wheel_timer();
        process_timers();
        process_tasks();

        if (graceful_shutdown_.load(std::memory_order_relaxed)) {
            auto now = std::chrono::steady_clock::now();
            bool has_active_fds = false;
            for (const auto& state : fd_states_) {
                if (state.callback) {
                    has_active_fds = true;
                    break;
                }
            }
            if (!has_active_fds) {
                running_ = false;
                break;
            }
            if (now >= graceful_shutdown_deadline_) {
                for (size_t fd = 0; fd < fd_states_.size(); ++fd) {
                    if (!fd_states_[fd].callback) continue;
                    try {
                        fd_states_[fd].callback(event_type::error);
                    } catch (...) {
                        handle_exception("forced_shutdown_callback", std::current_exception(), static_cast<int32_t>(fd));
                    }
                    if (fd_states_[fd].callback) {
                        submit_poll_remove(static_cast<int32_t>(fd));
                        close(static_cast<int32_t>(fd));
                        fd_states_[fd] = fd_state{};
                    }
                }
                running_ = false;
                break;
            }
        }

        int timeout_ms = calculate_timeout();
        auto res = process_completions(timeout_ms);
        if (!res) {
            running_ = false;
            return res;
        }
    }

    unregister_fd(wakeup_fd_);
    return {};
}

void io_uring_reactor::stop() {
    running_.store(false, std::memory_order_relaxed);
    uint64_t val = 1;
    ssize_t ret;
    do {
        ret = write(wakeup_fd_, &val, sizeof(val));
    } while (ret < 0 && errno == EINTR);
}

void io_uring_reactor::graceful_stop(std::chrono::milliseconds timeout) {
    graceful_shutdown_.store(true, std::memory_order_relaxed);
    graceful_shutdown_deadline_ = std::chrono::steady_clock::now() + timeout;
    uint64_t val = 1;
    ssize_t ret;
    do {
        ret = write(wakeup_fd_, &val, sizeof(val));
    } while (ret < 0 && errno == EINTR);
}

result<void> io_uring_reactor::register_fd(
    int32_t fd,
    event_type events,
    event_callback callback
) {
    if (fd < 0) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    auto ensure = ensure_fd_capacity(fd);
    if (!ensure) {
        return ensure;
    }

    auto& state = fd_states_[static_cast<size_t>(fd)];
    state.callback = std::move(callback);
    state.events = events;
    state.timeouts = {};
    state.timeout_id = 0;
    state.activity_timer = Timeout{};
    state.has_timeout = false;
    state.registered = true;

    return submit_poll_add(fd, events);
}

result<void> io_uring_reactor::register_fd_with_timeout(
    int32_t fd,
    event_type events,
    event_callback callback,
    const timeout_config& config
) {
    if (fd < 0) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    auto ensure = ensure_fd_capacity(fd);
    if (!ensure) {
        return ensure;
    }

    fd_state state{};
    state.callback = std::move(callback);
    state.events = events;
    state.timeouts = config;
    state.timeout_id = 0;
    state.activity_timer = Timeout{};
    state.has_timeout = true;
    state.registered = true;
    setup_fd_timeout(fd, state);

    auto res = submit_poll_add(fd, events);
    if (!res) {
        cancel_fd_timeout(state);
        return res;
    }

    fd_states_[static_cast<size_t>(fd)] = std::move(state);
    return {};
}

result<void> io_uring_reactor::modify_fd(int32_t fd, event_type events) {
    if (fd < 0 || static_cast<size_t>(fd) >= fd_states_.size() ||
        !fd_states_[static_cast<size_t>(fd)].callback) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    auto& state = fd_states_[static_cast<size_t>(fd)];

    auto res = submit_poll_remove(fd);
    if (!res) {
        return res;
    }

    res = submit_poll_add(fd, events);
    if (!res) {
        return res;
    }

    state.events = events;
    if (state.has_timeout) {
        cancel_fd_timeout(state);
        setup_fd_timeout(fd, state);
    }
    return {};
}

result<void> io_uring_reactor::unregister_fd(int32_t fd) {
    if (fd < 0 || static_cast<size_t>(fd) >= fd_states_.size() ||
        !fd_states_[static_cast<size_t>(fd)].callback) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    cancel_fd_timeout(fd_states_[static_cast<size_t>(fd)]);

    auto res = submit_poll_remove(fd);
    if (!res) {
        return res;
    }

    fd_states_[static_cast<size_t>(fd)] = fd_state{};
    return {};
}

void io_uring_reactor::refresh_fd_timeout(int32_t fd) {
    if (fd >= 0 && static_cast<size_t>(fd) < fd_states_.size() &&
        fd_states_[static_cast<size_t>(fd)].has_timeout) {
        auto& state = fd_states_[static_cast<size_t>(fd)];
        cancel_fd_timeout(state);
        setup_fd_timeout(fd, state);
    }
}

bool io_uring_reactor::schedule(task_fn task) {
    if (!pending_tasks_.try_push(std::move(task))) {
        metrics_.tasks_rejected.fetch_add(1, std::memory_order_relaxed);
        return false;
    }
    metrics_.tasks_scheduled.fetch_add(1, std::memory_order_relaxed);

    uint64_t val = 1;
    ssize_t ret;
    do {
        ret = write(wakeup_fd_, &val, sizeof(val));
    } while (ret < 0 && errno == EINTR);

    if (ret < 0 && errno != EAGAIN) {
        handle_exception(
            "schedule_wakeup",
            std::make_exception_ptr(std::system_error(errno, std::system_category(), "eventfd write failed"))
        );
    }

    return true;
}

bool io_uring_reactor::schedule_after(
    std::chrono::milliseconds delay,
    task_fn task
) {
    auto deadline = std::chrono::steady_clock::now() + delay;
    if (!pending_timers_.try_push(timer_entry{deadline, std::move(task)})) {
        metrics_.tasks_rejected.fetch_add(1, std::memory_order_relaxed);
        return false;
    }
    metrics_.tasks_scheduled.fetch_add(1, std::memory_order_relaxed);
    timeout_dirty_.store(true, std::memory_order_relaxed);

    uint64_t val = 1;
    ssize_t ret;
    do {
        ret = write(wakeup_fd_, &val, sizeof(val));
    } while (ret < 0 && errno == EINTR);

    if (ret < 0 && errno != EAGAIN) {
        handle_exception(
            "schedule_timer_wakeup",
            std::make_exception_ptr(std::system_error(errno, std::system_category(), "eventfd write failed"))
        );
    }

    return true;
}

result<void> io_uring_reactor::submit_poll_add(int32_t fd, event_type events) {
    io_uring_sqe* sqe = io_uring_get_sqe(&ring_);
    if (!sqe) {
        return std::unexpected(make_error_code(error_code::reactor_stopped));
    }

    uint32_t poll_mask = to_poll_events(events);
    io_uring_prep_poll_add(sqe, fd, poll_mask);
    io_uring_sqe_set_data(sqe, reinterpret_cast<void*>(static_cast<uintptr_t>(fd)));

    int ret = io_uring_submit(&ring_);
    if (ret < 0) {
        return std::unexpected(std::error_code(-ret, std::system_category()));
    }

    return {};
}

result<void> io_uring_reactor::submit_poll_remove(int32_t fd) {
    io_uring_sqe* sqe = io_uring_get_sqe(&ring_);
    if (!sqe) {
        return std::unexpected(make_error_code(error_code::reactor_stopped));
    }

    io_uring_prep_poll_remove(sqe, static_cast<__u64>(static_cast<uintptr_t>(fd)));

    int ret = io_uring_submit(&ring_);
    if (ret < 0) {
        return std::unexpected(std::error_code(-ret, std::system_category()));
    }

    return {};
}

result<void> io_uring_reactor::process_completions(int32_t timeout_ms) {
    io_uring_cqe* cqe = nullptr;
    int ret;

    if (timeout_ms > 0) {
        __kernel_timespec ts;
        ts.tv_sec = timeout_ms / 1000;
        ts.tv_nsec = (timeout_ms % 1000) * 1000000;
        ret = io_uring_wait_cqe_timeout(&ring_, &cqe, &ts);
    } else if (timeout_ms == 0) {
        ret = io_uring_peek_cqe(&ring_, &cqe);
    } else {
        ret = io_uring_wait_cqe(&ring_, &cqe);
    }

    if (ret < 0 && ret != -ETIME && ret != -EAGAIN) {
        return std::unexpected(std::error_code(-ret, std::system_category()));
    }

    unsigned count = 0;
    io_uring_cqe* current_cqe;
    io_uring_for_each_cqe(&ring_, count, current_cqe) {
        int32_t fd = static_cast<int32_t>(reinterpret_cast<uintptr_t>(io_uring_cqe_get_data(current_cqe)));

        if (fd >= 0 && static_cast<size_t>(fd) < fd_states_.size() &&
            fd_states_[static_cast<size_t>(fd)].callback) {

            int res = current_cqe->res;

            if (res < 0) {
                if (res == -ECANCELED) {
                    continue;
                }
                event_callback callback_copy = fd_states_[static_cast<size_t>(fd)].callback;
                if (callback_copy) {
                    try {
                        callback_copy(event_type::error);
                        metrics_.fd_events_processed.fetch_add(1, std::memory_order_relaxed);
                    } catch (...) {
                        handle_exception("fd_callback_error", std::current_exception(), fd);
                    }
                }
            } else {
                event_type ev = from_poll_events(static_cast<uint32_t>(res));
                event_callback callback_copy = fd_states_[static_cast<size_t>(fd)].callback;

                if (!callback_copy) {
                    continue;
                }

                try {
                    callback_copy(ev);
                    metrics_.fd_events_processed.fetch_add(1, std::memory_order_relaxed);
                } catch (...) {
                    handle_exception("fd_callback", std::current_exception(), fd);
                }

                if (fd_states_[static_cast<size_t>(fd)].registered &&
                    !has_flag(fd_states_[static_cast<size_t>(fd)].events, event_type::oneshot)) {
                    submit_poll_add(fd, fd_states_[static_cast<size_t>(fd)].events);
                }
            }
        }
    }

    if (count > 0) {
        io_uring_cq_advance(&ring_, count);
    }

    return {};
}

void io_uring_reactor::process_tasks() {
    while (auto task = pending_tasks_.pop()) {
        try {
            (*task)();
            metrics_.tasks_executed.fetch_add(1, std::memory_order_relaxed);
        } catch (...) {
            handle_exception("scheduled_task", std::current_exception());
        }
    }
}

void io_uring_reactor::process_timers() {
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

int32_t io_uring_reactor::calculate_timeout() const {
    if (!pending_tasks_.empty()) {
        timeout_dirty_.store(true, std::memory_order_relaxed);
        return 0;
    }

    auto now = std::chrono::steady_clock::now();

    if (!timeout_dirty_.load(std::memory_order_relaxed)) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - timeout_cached_at_);
        if (elapsed.count() < 5 && cached_timeout_ > 0) {
            return std::max(0, cached_timeout_ - static_cast<int32_t>(elapsed.count()));
        }
    }

    auto min_timeout = std::chrono::milliseconds::max();

    if (!timers_.empty()) {
        auto deadline = timers_.top().deadline;
        if (deadline <= now) {
            timeout_dirty_.store(true, std::memory_order_relaxed);
            return 0;
        }
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(deadline - now);
        min_timeout = std::min(min_timeout, delta);
    }

    auto wheel_timeout = wheel_timer_.time_until_next_expiration(now);
    if (wheel_timeout == std::chrono::milliseconds::zero()) {
        timeout_dirty_.store(true, std::memory_order_relaxed);
        return 0;
    }
    if (wheel_timeout != std::chrono::milliseconds::max()) {
        min_timeout = std::min(min_timeout, wheel_timeout);
    }

    if (graceful_shutdown_.load(std::memory_order_relaxed)) {
        auto graceful_timeout = time_until_graceful_deadline(now);
        if (graceful_timeout.count() <= 0) {
            timeout_dirty_.store(true, std::memory_order_relaxed);
            return 0;
        }
        min_timeout = std::min(min_timeout, graceful_timeout);
    }

    int32_t result;
    if (min_timeout == std::chrono::milliseconds::max()) {
        result = -1;
    } else {
        auto clamped = std::min<int64_t>(
            min_timeout.count(),
            static_cast<int64_t>(std::numeric_limits<int32_t>::max())
        );
        result = static_cast<int32_t>(clamped);
    }

    cached_timeout_ = result;
    timeout_cached_at_ = now;
    timeout_dirty_.store(false, std::memory_order_relaxed);

    return result;
}

void io_uring_reactor::set_exception_handler(exception_handler handler) {
    exception_handler_ = std::move(handler);
}

uint64_t io_uring_reactor::get_load_score() const noexcept {
    size_t active_fds = 0;
    for (const auto& state : fd_states_) {
        if (state.callback) {
            ++active_fds;
        }
    }

    size_t pending_tasks = pending_tasks_.size();
    size_t pending_timers_count = pending_timers_.size();

    return active_fds * 100 + pending_tasks * 50 + pending_timers_count * 10;
}

void io_uring_reactor::process_wheel_timer() {
    wheel_timer_.tick();
}

void io_uring_reactor::setup_fd_timeout(int32_t fd, fd_state& state) {
    auto timeout = fd_timeout_for(state);

    if (!state.activity_timer.active() || state.activity_timer.duration() != timeout) {
        state.activity_timer = Timeout(timeout);
    } else {
        state.activity_timer.reset();
    }

    state.timeout_id = wheel_timer_.add(
        timeout,
        [this, fd]() {
            if (fd < 0 || static_cast<size_t>(fd) >= fd_states_.size()) {
                return;
            }

            auto index = static_cast<size_t>(fd);
            auto& entry_state = fd_states_[index];
            if (!entry_state.callback) {
                entry_state.timeout_id = 0;
                entry_state.activity_timer = Timeout{};
                return;
            }

            entry_state.timeout_id = 0;
            entry_state.activity_timer = Timeout{};
            metrics_.fd_timeouts.fetch_add(1, std::memory_order_relaxed);

            submit_poll_remove(fd);

            if (close(fd) < 0 && errno != EBADF) {
                handle_exception(
                    "timeout_close",
                    std::make_exception_ptr(std::system_error(errno, std::system_category(), "close failed")),
                    fd
                );
            }

            try {
                entry_state.callback(event_type::timeout);
            } catch (...) {
                handle_exception("timeout_handler", std::current_exception(), fd);
            }

            fd_states_[index] = fd_state{};
        }
    );
}

void io_uring_reactor::cancel_fd_timeout(fd_state& state) {
    if (state.timeout_id != 0) {
        (void)wheel_timer_.cancel(state.timeout_id);
        state.timeout_id = 0;
    }
    state.activity_timer = Timeout{};
}

std::chrono::milliseconds io_uring_reactor::fd_timeout_for(const fd_state& state) const {
    auto timeout = state.timeouts.idle_timeout;

    if (has_flag(state.events, event_type::readable)) {
        timeout = std::min(timeout, state.timeouts.read_timeout);
    }

    if (has_flag(state.events, event_type::writable)) {
        timeout = std::min(timeout, state.timeouts.write_timeout);
    }

    if (timeout.count() <= 0) {
        return std::chrono::milliseconds{1};
    }

    return timeout;
}

result<void> io_uring_reactor::ensure_fd_capacity(int32_t fd) {
    if (fd < 0) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    size_t index = static_cast<size_t>(fd);
    if (index < fd_states_.size()) {
        return {};
    }

    size_t new_size = fd_states_.empty() ? 64 : fd_states_.size();
    while (new_size <= index) {
        if (new_size > fd_states_.max_size() / 2) {
            new_size = index + 1;
            break;
        }
        new_size = std::max(new_size * 2, index + 1);
    }

    try {
        fd_states_.resize(new_size);
    } catch (const std::bad_alloc&) {
        return std::unexpected(std::make_error_code(std::errc::not_enough_memory));
    }

    return {};
}

std::chrono::milliseconds io_uring_reactor::time_until_graceful_deadline(
    std::chrono::steady_clock::time_point now
) const {
    if (!graceful_shutdown_.load(std::memory_order_relaxed)) {
        return std::chrono::milliseconds::max();
    }

    if (now >= graceful_shutdown_deadline_) {
        return std::chrono::milliseconds{0};
    }

    return std::chrono::duration_cast<std::chrono::milliseconds>(
        graceful_shutdown_deadline_ - now
    );
}

void io_uring_reactor::handle_exception(
    std::string_view location,
    std::exception_ptr ex,
    int32_t fd
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
