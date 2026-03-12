#pragma once

#include <cstddef>
#include <cstdint>

namespace katana::detail {

struct syscall_metrics_snapshot {
    uint64_t recv_calls = 0;
    uint64_t recv_would_block = 0;
    uint64_t send_calls = 0;
    uint64_t send_would_block = 0;
    uint64_t epoll_wait_calls = 0;
    uint64_t epoll_wait_ready_events = 0;
    uint64_t epoll_wait_timeouts = 0;
    uint64_t epoll_ctl_add_calls = 0;
    uint64_t epoll_ctl_mod_calls = 0;
    uint64_t epoll_ctl_del_calls = 0;
    uint64_t completed_requests = 0;
    uint64_t arena_alloc_calls = 0;
    uint64_t arena_alloc_bytes = 0;
    uint64_t arena_new_blocks = 0;
    uint64_t arena_new_block_bytes = 0;
    uint64_t parser_reserve_calls = 0;
    uint64_t parser_grow_calls = 0;
    uint64_t parser_buffer_copy_bytes = 0;
    uint64_t parser_compact_calls = 0;
    uint64_t parser_compact_move_bytes = 0;
    uint64_t response_serialize_calls = 0;
    uint64_t response_serialize_bytes = 0;
    uint64_t response_output_grow_calls = 0;
    uint64_t response_output_grow_bytes = 0;
};

syscall_metrics_snapshot operator-(const syscall_metrics_snapshot& lhs,
                                   const syscall_metrics_snapshot& rhs) noexcept;
syscall_metrics_snapshot& operator+=(syscall_metrics_snapshot& lhs,
                                     const syscall_metrics_snapshot& rhs) noexcept;

class syscall_metrics_registry {
public:
    static syscall_metrics_registry& instance() noexcept;

    [[nodiscard]] bool enabled() const noexcept;

    void note_recv(bool would_block) noexcept;
    void note_send(bool would_block) noexcept;
    void note_epoll_wait(int32_t ready_events) noexcept;
    void note_epoll_ctl_add() noexcept;
    void note_epoll_ctl_mod() noexcept;
    void note_epoll_ctl_del() noexcept;
    void note_completed_request() noexcept;
    void note_arena_allocate(size_t bytes) noexcept;
    void note_arena_new_block(size_t bytes) noexcept;
    void
    note_parser_reserve(size_t old_capacity, size_t new_capacity, size_t copied_bytes) noexcept;
    void note_parser_compact(size_t moved_bytes) noexcept;
    void note_response_serialize(size_t bytes, size_t old_capacity, size_t new_capacity) noexcept;

    [[nodiscard]] syscall_metrics_snapshot snapshot() const noexcept;

    void start_periodic_reporting();
    void stop_periodic_reporting();
    void reporter_main();
    void emit_delta_report(const syscall_metrics_snapshot& delta, const char* label) const noexcept;

private:
    syscall_metrics_registry() noexcept;
    ~syscall_metrics_registry();

    syscall_metrics_registry(const syscall_metrics_registry&) = delete;
    syscall_metrics_registry& operator=(const syscall_metrics_registry&) = delete;

    struct thread_slot;
    thread_slot* local_slot() noexcept;
    thread_slot* register_thread_slot() noexcept;

    bool enabled_{false};
    uint32_t interval_ms_{250};
};

class scoped_syscall_metrics_reporter {
public:
    scoped_syscall_metrics_reporter();
    ~scoped_syscall_metrics_reporter();

    scoped_syscall_metrics_reporter(const scoped_syscall_metrics_reporter&) = delete;
    scoped_syscall_metrics_reporter& operator=(const scoped_syscall_metrics_reporter&) = delete;

private:
    bool active_{false};
};

} // namespace katana::detail
