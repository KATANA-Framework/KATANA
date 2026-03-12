#include "katana/core/detail/syscall_metrics.hpp"

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace katana::detail {

namespace {

bool getenv_bool(const char* name, bool fallback) noexcept {
    const char* value = std::getenv(name);
    if (!value || !*value) {
        return fallback;
    }

    return std::strcmp(value, "1") == 0 || std::strcmp(value, "true") == 0 ||
           std::strcmp(value, "TRUE") == 0 || std::strcmp(value, "yes") == 0 ||
           std::strcmp(value, "YES") == 0;
}

uint32_t getenv_u32(const char* name, uint32_t fallback) noexcept {
    const char* value = std::getenv(name);
    if (!value || !*value) {
        return fallback;
    }

    char* end = nullptr;
    unsigned long parsed = std::strtoul(value, &end, 10);
    if (end == value || *end != '\0' || parsed == 0) {
        return fallback;
    }

    constexpr unsigned long kMax = 60'000;
    if (parsed > kMax) {
        parsed = kMax;
    }

    return static_cast<uint32_t>(parsed);
}

class reporter_state {
public:
    void start(syscall_metrics_registry& registry) {
        std::lock_guard lock(mutex_);
        ++ref_count_;
        if (thread_.joinable()) {
            return;
        }
        stop_requested_.store(false, std::memory_order_relaxed);
        thread_ = std::thread([&registry] { registry.reporter_main(); });
    }

    void stop(syscall_metrics_registry& registry) {
        {
            std::lock_guard lock(mutex_);
            if (ref_count_ == 0) {
                return;
            }
            --ref_count_;
            if (ref_count_ != 0) {
                return;
            }
            stop_requested_.store(true, std::memory_order_relaxed);
        }
        if (thread_.joinable()) {
            thread_.join();
        }
        registry.emit_delta_report(registry.snapshot(), "final");
    }

    [[nodiscard]] bool stop_requested() const noexcept {
        return stop_requested_.load(std::memory_order_relaxed);
    }

private:
    mutable std::mutex mutex_;
    std::thread thread_;
    std::atomic<bool> stop_requested_{false};
    uint32_t ref_count_{0};
};

reporter_state& shared_reporter_state() {
    static reporter_state state;
    return state;
}

struct alignas(64) slot_counters {
    std::atomic<uint64_t> recv_calls{0};
    std::atomic<uint64_t> recv_would_block{0};
    std::atomic<uint64_t> send_calls{0};
    std::atomic<uint64_t> send_would_block{0};
    std::atomic<uint64_t> epoll_wait_calls{0};
    std::atomic<uint64_t> epoll_wait_ready_events{0};
    std::atomic<uint64_t> epoll_wait_timeouts{0};
    std::atomic<uint64_t> epoll_ctl_add_calls{0};
    std::atomic<uint64_t> epoll_ctl_mod_calls{0};
    std::atomic<uint64_t> epoll_ctl_del_calls{0};
    std::atomic<uint64_t> completed_requests{0};
    std::atomic<uint64_t> arena_alloc_calls{0};
    std::atomic<uint64_t> arena_alloc_bytes{0};
    std::atomic<uint64_t> arena_new_blocks{0};
    std::atomic<uint64_t> arena_new_block_bytes{0};
    std::atomic<uint64_t> parser_reserve_calls{0};
    std::atomic<uint64_t> parser_grow_calls{0};
    std::atomic<uint64_t> parser_buffer_copy_bytes{0};
    std::atomic<uint64_t> parser_compact_calls{0};
    std::atomic<uint64_t> parser_compact_move_bytes{0};
    std::atomic<uint64_t> response_serialize_calls{0};
    std::atomic<uint64_t> response_serialize_bytes{0};
    std::atomic<uint64_t> response_output_grow_calls{0};
    std::atomic<uint64_t> response_output_grow_bytes{0};
};

std::mutex& slots_mutex() {
    static std::mutex mutex;
    return mutex;
}

std::vector<std::unique_ptr<slot_counters>>& slots() {
    static std::vector<std::unique_ptr<slot_counters>> all_slots;
    return all_slots;
}

thread_local slot_counters* tls_slot = nullptr;
thread_local bool tls_slot_initialized = false;

double per_request(uint64_t value, uint64_t completed_requests) noexcept {
    if (completed_requests == 0) {
        return 0.0;
    }
    return static_cast<double>(value) / static_cast<double>(completed_requests);
}

} // namespace

struct syscall_metrics_registry::thread_slot {};

syscall_metrics_snapshot operator-(const syscall_metrics_snapshot& lhs,
                                   const syscall_metrics_snapshot& rhs) noexcept {
    return syscall_metrics_snapshot{
        lhs.recv_calls - rhs.recv_calls,
        lhs.recv_would_block - rhs.recv_would_block,
        lhs.send_calls - rhs.send_calls,
        lhs.send_would_block - rhs.send_would_block,
        lhs.epoll_wait_calls - rhs.epoll_wait_calls,
        lhs.epoll_wait_ready_events - rhs.epoll_wait_ready_events,
        lhs.epoll_wait_timeouts - rhs.epoll_wait_timeouts,
        lhs.epoll_ctl_add_calls - rhs.epoll_ctl_add_calls,
        lhs.epoll_ctl_mod_calls - rhs.epoll_ctl_mod_calls,
        lhs.epoll_ctl_del_calls - rhs.epoll_ctl_del_calls,
        lhs.completed_requests - rhs.completed_requests,
        lhs.arena_alloc_calls - rhs.arena_alloc_calls,
        lhs.arena_alloc_bytes - rhs.arena_alloc_bytes,
        lhs.arena_new_blocks - rhs.arena_new_blocks,
        lhs.arena_new_block_bytes - rhs.arena_new_block_bytes,
        lhs.parser_reserve_calls - rhs.parser_reserve_calls,
        lhs.parser_grow_calls - rhs.parser_grow_calls,
        lhs.parser_buffer_copy_bytes - rhs.parser_buffer_copy_bytes,
        lhs.parser_compact_calls - rhs.parser_compact_calls,
        lhs.parser_compact_move_bytes - rhs.parser_compact_move_bytes,
        lhs.response_serialize_calls - rhs.response_serialize_calls,
        lhs.response_serialize_bytes - rhs.response_serialize_bytes,
        lhs.response_output_grow_calls - rhs.response_output_grow_calls,
        lhs.response_output_grow_bytes - rhs.response_output_grow_bytes,
    };
}

syscall_metrics_snapshot& operator+=(syscall_metrics_snapshot& lhs,
                                     const syscall_metrics_snapshot& rhs) noexcept {
    lhs.recv_calls += rhs.recv_calls;
    lhs.recv_would_block += rhs.recv_would_block;
    lhs.send_calls += rhs.send_calls;
    lhs.send_would_block += rhs.send_would_block;
    lhs.epoll_wait_calls += rhs.epoll_wait_calls;
    lhs.epoll_wait_ready_events += rhs.epoll_wait_ready_events;
    lhs.epoll_wait_timeouts += rhs.epoll_wait_timeouts;
    lhs.epoll_ctl_add_calls += rhs.epoll_ctl_add_calls;
    lhs.epoll_ctl_mod_calls += rhs.epoll_ctl_mod_calls;
    lhs.epoll_ctl_del_calls += rhs.epoll_ctl_del_calls;
    lhs.completed_requests += rhs.completed_requests;
    lhs.arena_alloc_calls += rhs.arena_alloc_calls;
    lhs.arena_alloc_bytes += rhs.arena_alloc_bytes;
    lhs.arena_new_blocks += rhs.arena_new_blocks;
    lhs.arena_new_block_bytes += rhs.arena_new_block_bytes;
    lhs.parser_reserve_calls += rhs.parser_reserve_calls;
    lhs.parser_grow_calls += rhs.parser_grow_calls;
    lhs.parser_buffer_copy_bytes += rhs.parser_buffer_copy_bytes;
    lhs.parser_compact_calls += rhs.parser_compact_calls;
    lhs.parser_compact_move_bytes += rhs.parser_compact_move_bytes;
    lhs.response_serialize_calls += rhs.response_serialize_calls;
    lhs.response_serialize_bytes += rhs.response_serialize_bytes;
    lhs.response_output_grow_calls += rhs.response_output_grow_calls;
    lhs.response_output_grow_bytes += rhs.response_output_grow_bytes;
    return lhs;
}

syscall_metrics_registry& syscall_metrics_registry::instance() noexcept {
    static syscall_metrics_registry registry;
    return registry;
}

syscall_metrics_registry::syscall_metrics_registry() noexcept
    : enabled_(getenv_bool("KATANA_SYSCALL_METRICS", false)),
      interval_ms_(getenv_u32("KATANA_SYSCALL_METRICS_INTERVAL_MS", 250)) {}

syscall_metrics_registry::~syscall_metrics_registry() {
    if (enabled_) {
        stop_periodic_reporting();
    }
}

bool syscall_metrics_registry::enabled() const noexcept {
    return enabled_;
}

syscall_metrics_registry::thread_slot* syscall_metrics_registry::local_slot() noexcept {
    if (!enabled_) {
        return nullptr;
    }
    if (!tls_slot_initialized) {
        tls_slot = reinterpret_cast<slot_counters*>(register_thread_slot());
        tls_slot_initialized = true;
    }
    return reinterpret_cast<thread_slot*>(tls_slot);
}

syscall_metrics_registry::thread_slot* syscall_metrics_registry::register_thread_slot() noexcept {
    try {
        auto slot = std::make_unique<slot_counters>();
        auto* raw = slot.get();
        std::lock_guard lock(slots_mutex());
        slots().push_back(std::move(slot));
        return reinterpret_cast<thread_slot*>(raw);
    } catch (...) {
        return nullptr;
    }
}

void syscall_metrics_registry::note_recv(bool would_block) noexcept {
    auto* slot = reinterpret_cast<slot_counters*>(local_slot());
    if (!slot) {
        return;
    }
    slot->recv_calls.fetch_add(1, std::memory_order_relaxed);
    if (would_block) {
        slot->recv_would_block.fetch_add(1, std::memory_order_relaxed);
    }
}

void syscall_metrics_registry::note_send(bool would_block) noexcept {
    auto* slot = reinterpret_cast<slot_counters*>(local_slot());
    if (!slot) {
        return;
    }
    slot->send_calls.fetch_add(1, std::memory_order_relaxed);
    if (would_block) {
        slot->send_would_block.fetch_add(1, std::memory_order_relaxed);
    }
}

void syscall_metrics_registry::note_epoll_wait(int32_t ready_events) noexcept {
    auto* slot = reinterpret_cast<slot_counters*>(local_slot());
    if (!slot) {
        return;
    }
    slot->epoll_wait_calls.fetch_add(1, std::memory_order_relaxed);
    if (ready_events > 0) {
        slot->epoll_wait_ready_events.fetch_add(static_cast<uint64_t>(ready_events),
                                                std::memory_order_relaxed);
    } else if (ready_events == 0) {
        slot->epoll_wait_timeouts.fetch_add(1, std::memory_order_relaxed);
    }
}

void syscall_metrics_registry::note_epoll_ctl_add() noexcept {
    auto* slot = reinterpret_cast<slot_counters*>(local_slot());
    if (slot) {
        slot->epoll_ctl_add_calls.fetch_add(1, std::memory_order_relaxed);
    }
}

void syscall_metrics_registry::note_epoll_ctl_mod() noexcept {
    auto* slot = reinterpret_cast<slot_counters*>(local_slot());
    if (slot) {
        slot->epoll_ctl_mod_calls.fetch_add(1, std::memory_order_relaxed);
    }
}

void syscall_metrics_registry::note_epoll_ctl_del() noexcept {
    auto* slot = reinterpret_cast<slot_counters*>(local_slot());
    if (slot) {
        slot->epoll_ctl_del_calls.fetch_add(1, std::memory_order_relaxed);
    }
}

void syscall_metrics_registry::note_completed_request() noexcept {
    auto* slot = reinterpret_cast<slot_counters*>(local_slot());
    if (slot) {
        slot->completed_requests.fetch_add(1, std::memory_order_relaxed);
    }
}

void syscall_metrics_registry::note_arena_allocate(size_t bytes) noexcept {
    auto* slot = reinterpret_cast<slot_counters*>(local_slot());
    if (!slot) {
        return;
    }
    slot->arena_alloc_calls.fetch_add(1, std::memory_order_relaxed);
    slot->arena_alloc_bytes.fetch_add(static_cast<uint64_t>(bytes), std::memory_order_relaxed);
}

void syscall_metrics_registry::note_arena_new_block(size_t bytes) noexcept {
    auto* slot = reinterpret_cast<slot_counters*>(local_slot());
    if (!slot) {
        return;
    }
    slot->arena_new_blocks.fetch_add(1, std::memory_order_relaxed);
    slot->arena_new_block_bytes.fetch_add(static_cast<uint64_t>(bytes), std::memory_order_relaxed);
}

void syscall_metrics_registry::note_parser_reserve(size_t old_capacity,
                                                   size_t new_capacity,
                                                   size_t copied_bytes) noexcept {
    auto* slot = reinterpret_cast<slot_counters*>(local_slot());
    if (!slot) {
        return;
    }
    slot->parser_reserve_calls.fetch_add(1, std::memory_order_relaxed);
    if (new_capacity > old_capacity) {
        slot->parser_grow_calls.fetch_add(1, std::memory_order_relaxed);
    }
    if (copied_bytes != 0) {
        slot->parser_buffer_copy_bytes.fetch_add(static_cast<uint64_t>(copied_bytes),
                                                 std::memory_order_relaxed);
    }
}

void syscall_metrics_registry::note_parser_compact(size_t moved_bytes) noexcept {
    auto* slot = reinterpret_cast<slot_counters*>(local_slot());
    if (!slot) {
        return;
    }
    slot->parser_compact_calls.fetch_add(1, std::memory_order_relaxed);
    slot->parser_compact_move_bytes.fetch_add(static_cast<uint64_t>(moved_bytes),
                                              std::memory_order_relaxed);
}

void syscall_metrics_registry::note_response_serialize(size_t bytes,
                                                       size_t old_capacity,
                                                       size_t new_capacity) noexcept {
    auto* slot = reinterpret_cast<slot_counters*>(local_slot());
    if (!slot) {
        return;
    }
    slot->response_serialize_calls.fetch_add(1, std::memory_order_relaxed);
    slot->response_serialize_bytes.fetch_add(static_cast<uint64_t>(bytes),
                                             std::memory_order_relaxed);
    if (new_capacity > old_capacity) {
        slot->response_output_grow_calls.fetch_add(1, std::memory_order_relaxed);
        slot->response_output_grow_bytes.fetch_add(
            static_cast<uint64_t>(new_capacity - old_capacity), std::memory_order_relaxed);
    }
}

syscall_metrics_snapshot syscall_metrics_registry::snapshot() const noexcept {
    syscall_metrics_snapshot total;

    std::lock_guard lock(slots_mutex());
    for (const auto& slot : slots()) {
        total.recv_calls += slot->recv_calls.load(std::memory_order_relaxed);
        total.recv_would_block += slot->recv_would_block.load(std::memory_order_relaxed);
        total.send_calls += slot->send_calls.load(std::memory_order_relaxed);
        total.send_would_block += slot->send_would_block.load(std::memory_order_relaxed);
        total.epoll_wait_calls += slot->epoll_wait_calls.load(std::memory_order_relaxed);
        total.epoll_wait_ready_events +=
            slot->epoll_wait_ready_events.load(std::memory_order_relaxed);
        total.epoll_wait_timeouts += slot->epoll_wait_timeouts.load(std::memory_order_relaxed);
        total.epoll_ctl_add_calls += slot->epoll_ctl_add_calls.load(std::memory_order_relaxed);
        total.epoll_ctl_mod_calls += slot->epoll_ctl_mod_calls.load(std::memory_order_relaxed);
        total.epoll_ctl_del_calls += slot->epoll_ctl_del_calls.load(std::memory_order_relaxed);
        total.completed_requests += slot->completed_requests.load(std::memory_order_relaxed);
        total.arena_alloc_calls += slot->arena_alloc_calls.load(std::memory_order_relaxed);
        total.arena_alloc_bytes += slot->arena_alloc_bytes.load(std::memory_order_relaxed);
        total.arena_new_blocks += slot->arena_new_blocks.load(std::memory_order_relaxed);
        total.arena_new_block_bytes += slot->arena_new_block_bytes.load(std::memory_order_relaxed);
        total.parser_reserve_calls += slot->parser_reserve_calls.load(std::memory_order_relaxed);
        total.parser_grow_calls += slot->parser_grow_calls.load(std::memory_order_relaxed);
        total.parser_buffer_copy_bytes +=
            slot->parser_buffer_copy_bytes.load(std::memory_order_relaxed);
        total.parser_compact_calls += slot->parser_compact_calls.load(std::memory_order_relaxed);
        total.parser_compact_move_bytes +=
            slot->parser_compact_move_bytes.load(std::memory_order_relaxed);
        total.response_serialize_calls +=
            slot->response_serialize_calls.load(std::memory_order_relaxed);
        total.response_serialize_bytes +=
            slot->response_serialize_bytes.load(std::memory_order_relaxed);
        total.response_output_grow_calls +=
            slot->response_output_grow_calls.load(std::memory_order_relaxed);
        total.response_output_grow_bytes +=
            slot->response_output_grow_bytes.load(std::memory_order_relaxed);
    }

    return total;
}

void syscall_metrics_registry::start_periodic_reporting() {
    if (!enabled_) {
        return;
    }
    shared_reporter_state().start(*this);
}

void syscall_metrics_registry::stop_periodic_reporting() {
    if (!enabled_) {
        return;
    }
    shared_reporter_state().stop(*this);
}

void syscall_metrics_registry::reporter_main() {
    auto previous = snapshot();
    while (!shared_reporter_state().stop_requested()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms_));
        auto current = snapshot();
        auto delta = current - previous;
        previous = current;
        emit_delta_report(delta, "interval");
    }
}

void syscall_metrics_registry::emit_delta_report(const syscall_metrics_snapshot& delta,
                                                 const char* label) const noexcept {
    const uint64_t socket_syscalls = delta.recv_calls + delta.send_calls;
    const uint64_t epoll_ctl_calls =
        delta.epoll_ctl_add_calls + delta.epoll_ctl_mod_calls + delta.epoll_ctl_del_calls;

    if (delta.completed_requests == 0 && socket_syscalls == 0 && delta.epoll_wait_calls == 0 &&
        epoll_ctl_calls == 0) {
        return;
    }

    std::cerr
        << std::fixed << std::setprecision(3) << "[syscall_metrics] label=" << label
        << " interval_ms=" << interval_ms_ << " completed_requests=" << delta.completed_requests
        << " recv=" << delta.recv_calls << " recv_would_block=" << delta.recv_would_block
        << " send=" << delta.send_calls << " send_would_block=" << delta.send_would_block
        << " epoll_wait=" << delta.epoll_wait_calls
        << " epoll_wait_ready=" << delta.epoll_wait_ready_events
        << " epoll_wait_timeouts=" << delta.epoll_wait_timeouts
        << " epoll_ctl_add=" << delta.epoll_ctl_add_calls
        << " epoll_ctl_mod=" << delta.epoll_ctl_mod_calls
        << " epoll_ctl_del=" << delta.epoll_ctl_del_calls
        << " arena_alloc_calls=" << delta.arena_alloc_calls
        << " arena_alloc_bytes=" << delta.arena_alloc_bytes
        << " arena_new_blocks=" << delta.arena_new_blocks
        << " arena_new_block_bytes=" << delta.arena_new_block_bytes
        << " parser_reserve_calls=" << delta.parser_reserve_calls
        << " parser_grow_calls=" << delta.parser_grow_calls
        << " parser_buffer_copy_bytes=" << delta.parser_buffer_copy_bytes
        << " parser_compact_calls=" << delta.parser_compact_calls
        << " parser_compact_move_bytes=" << delta.parser_compact_move_bytes
        << " response_serialize_calls=" << delta.response_serialize_calls
        << " response_serialize_bytes=" << delta.response_serialize_bytes
        << " response_output_grow_calls=" << delta.response_output_grow_calls
        << " response_output_grow_bytes=" << delta.response_output_grow_bytes
        << " socket_syscalls_per_req=" << per_request(socket_syscalls, delta.completed_requests)
        << " recv_per_req=" << per_request(delta.recv_calls, delta.completed_requests)
        << " send_per_req=" << per_request(delta.send_calls, delta.completed_requests)
        << " epoll_wait_per_req=" << per_request(delta.epoll_wait_calls, delta.completed_requests)
        << " epoll_ctl_mod_per_req="
        << per_request(delta.epoll_ctl_mod_calls, delta.completed_requests)
        << " epoll_ctl_total_per_req=" << per_request(epoll_ctl_calls, delta.completed_requests)
        << " arena_alloc_calls_per_req="
        << per_request(delta.arena_alloc_calls, delta.completed_requests)
        << " arena_alloc_bytes_per_req="
        << per_request(delta.arena_alloc_bytes, delta.completed_requests)
        << " arena_new_blocks_per_req="
        << per_request(delta.arena_new_blocks, delta.completed_requests)
        << " parser_grow_per_req=" << per_request(delta.parser_grow_calls, delta.completed_requests)
        << " parser_copy_bytes_per_req="
        << per_request(delta.parser_buffer_copy_bytes, delta.completed_requests)
        << " parser_compact_bytes_per_req="
        << per_request(delta.parser_compact_move_bytes, delta.completed_requests)
        << " response_serialize_bytes_per_req="
        << per_request(delta.response_serialize_bytes, delta.completed_requests)
        << " response_output_grow_per_req="
        << per_request(delta.response_output_grow_calls, delta.completed_requests)
        << " ready_events_per_wait="
        << per_request(delta.epoll_wait_ready_events, delta.epoll_wait_calls) << '\n';
}

scoped_syscall_metrics_reporter::scoped_syscall_metrics_reporter() {
    auto& registry = syscall_metrics_registry::instance();
    if (!registry.enabled()) {
        return;
    }
    registry.start_periodic_reporting();
    active_ = true;
}

scoped_syscall_metrics_reporter::~scoped_syscall_metrics_reporter() {
    if (active_) {
        syscall_metrics_registry::instance().stop_periodic_reporting();
    }
}

} // namespace katana::detail
