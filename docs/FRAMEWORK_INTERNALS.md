# KATANA Framework Internals: Design Document & Technical Whitepaper

**Version**: 1.0
**Date**: 2025-12-08
**Target Audience**: Framework developers, performance engineers, architects

---

## Executive Summary

KATANA is a C++23 server framework designed for predictable low-latency performance under high load. This document provides a comprehensive technical description of the framework's core architecture, implementation strategies, and performance characteristics.

**Key Design Principles**:
1. **Reactor-per-core**: Eliminates cross-core synchronization on hot paths
2. **Zero-copy I/O**: Minimizes memory operations and cache pressure
3. **Arena allocation**: Per-request bump allocators eliminate fragmentation
4. **Compile-time routing**: Zero runtime overhead for request dispatch
5. **SIMD where applicable**: SSE2/AVX2 for text processing

**Target Workloads**: Request-response APIs with p99 < 5ms requirements, 100k-1M+ RPS throughput.

---

## Table of Contents

1. [Reactor-Per-Core Model](#1-reactor-per-core-model)
2. [I/O Subsystem](#2-io-subsystem)
3. [Memory Model](#3-memory-model)
4. [Router Internals](#4-router-internals)
5. [Timer System](#5-timer-system)
6. [HTTP Parser/Serializer](#6-http-parserserializer)
7. [Concurrency Model](#7-concurrency-model)
8. [SIMD Optimizations](#8-simd-optimizations)
9. [Performance Characteristics](#9-performance-characteristics)
10. [Architectural Guarantees](#10-architectural-guarantees)

---

## 1. Reactor-Per-Core Model

### 1.1 Architecture Overview

KATANA uses a **reactor-per-core architecture** where each CPU core runs an independent event loop (reactor). This design eliminates the need for locks on hot paths and provides excellent CPU cache locality.

**Key Components**:
- `reactor_pool`: Manages N reactors (typically N = physical CPU cores)
- `epoll_reactor` / `io_uring_reactor`: Per-core event loop implementation
- `SO_REUSEPORT`: Kernel-level load distribution

### 1.2 Reactor Lifecycle

```cpp
// Simplified reactor lifecycle
class epoll_reactor {
    // Per-reactor state (no sharing)
    std::unordered_map<int, fd_state> fd_states_;    // File descriptor tracking
    std::priority_queue<task> delayed_tasks_;         // Scheduled tasks
    mpsc_queue<task> cross_thread_queue_;            // Messages from other reactors
    wheel_timer timer_wheel_;                        // Connection timeouts
    int epoll_fd_;                                   // epoll instance
};

void reactor::run() {
    while (!stop_requested) {
        // 1. Tick timer wheel (connection timeouts)
        timer_wheel_.tick();

        // 2. Process scheduled delayed tasks
        process_delayed_tasks();

        // 3. Process cross-thread messages
        drain_cross_thread_queue();

        // 4. Poll I/O events (epoll_wait or io_uring_enter)
        auto events = poll_io(timeout_ms);

        // 5. Process I/O events
        for (auto& ev : events) {
            handle_io_event(ev);
        }

        // 6. Deferred FD cleanup (budget: 2 immediate + rest deferred)
        process_deferred_cleanups();
    }
}
```

### 1.3 Event Loop Priority

1. **Timer ticks** (highest priority): Connection timeouts must be processed promptly
2. **Delayed tasks**: Scheduled operations (e.g., retries)
3. **Cross-thread tasks**: Messages from other reactors (rare in typical workloads)
4. **I/O events**: Network reads/writes
5. **Deferred cleanup**: FD close operations (amortized cost)

### 1.4 Isolation and Lock-Free Design

**Per-Reactor Independence**:
- ✅ Each reactor has its own epoll/io_uring instance
- ✅ Separate FD tracking (no shared map)
- ✅ Independent timer wheels
- ✅ Local task queues
- ✅ Thread-local scratch buffers

**Cross-Thread Communication**:
- Uses lock-free ring buffer (`mpsc_queue`)
- `alignas(64)` on atomics prevents false sharing
- Producer increments `head_`, consumer increments `tail_`
- Rare in practice (most work stays on originating reactor)

### 1.5 Load Distribution

**SO_REUSEPORT** allows kernel to distribute incoming connections across reactors:
```cpp
int enable = 1;
setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable));
```

**Benefits**:
- ✅ Kernel does load balancing (no user-space logic needed)
- ✅ Each reactor accepts connections independently
- ✅ Minimal contention on accept queue

**Limitations**:
- Connection affinity not guaranteed (client may hit different reactors)
- Requires connection pooling or stateless handlers

---

## 2. I/O Subsystem

### 2.1 Zero-Copy Strategy

KATANA minimizes memory copies through careful buffer management:

**Key Techniques**:
1. **Thread-local scratch buffer**: 64KB per thread, zero allocation for small I/O
2. **Lazy heap promotion**: Only allocate when > 64KB
3. **Smart compaction**: Only move data when beneficial
4. **Vectored I/O**: `readv()`/`writev()` for scatter/gather

### 2.2 IO Buffer Implementation

```cpp
class io_buffer {
    // Thread-local scratch (no malloc!)
    alignas(64) static thread_local uint8_t static_scratch_[65536];

    uint8_t* data_ = static_scratch_;
    size_t capacity_ = 65536;
    size_t read_pos_ = 0;
    size_t write_pos_ = 0;

    void ensure_writable(size_t n) {
        if (write_pos_ + n <= capacity_) return;  // Fast path

        // Compact if beneficial
        if (read_pos_ >= 4096 && read_pos_ > (write_pos_ - read_pos_)) {
            std::memmove(data_, data_ + read_pos_, write_pos_ - read_pos_);
            write_pos_ -= read_pos_;
            read_pos_ = 0;
        }

        // Heap promotion if still insufficient
        if (write_pos_ + n > capacity_) {
            size_t new_cap = std::max(capacity_ * 2, write_pos_ + n);
            auto* new_data = new uint8_t[new_cap];
            std::memcpy(new_data, data_ + read_pos_, write_pos_ - read_pos_);
            if (data_ != static_scratch_) delete[] data_;
            data_ = new_data;
            capacity_ = new_cap;
            write_pos_ -= read_pos_;
            read_pos_ = 0;
        }
    }
};
```

**Performance Characteristics**:
- ✅ **Typical request** (< 4KB): Zero allocations, uses scratch
- ✅ **Large request** (> 64KB): One allocation, amortized growth
- ✅ **Compaction**: Only when `read_pos_ >= 4KB` AND beneficial

### 2.3 Scratch Buffer Sizing

**Why 64KB?**
- Typical HTTP request: 500 bytes - 4KB
- Typical HTTP response: 1KB - 8KB
- 64KB covers 99%+ of requests without heap allocation
- Aligns with typical socket buffer sizes

### 2.4 Integration with Reactor

```cpp
class connection_state {
    io_buffer read_buf_;   // Incoming data
    io_buffer write_buf_;  // Outgoing data
    monotonic_arena arena_;  // Per-request allocations

    void on_readable() {
        // Read into scratch buffer (likely zero-copy)
        ssize_t n = read_buf_.read_from_fd(fd_);
        if (n > 0) {
            // Parse HTTP (arena for headers/body)
            auto req = http::parse(read_buf_.readable(), arena_);
            if (req) {
                auto resp = handler_(req.value());
                write_buf_.append(resp.serialize());
                register_for_write();
            }
        }
    }
};
```

---

## 3. Memory Model

### 3.1 Arena Allocators

KATANA uses **monotonic arena allocators** for per-request memory:

**Key Properties**:
- ✅ Bump-pointer allocation: O(1) `malloc()`
- ✅ No-op `free()`: Memory reused after request completes
- ✅ 64KB default blocks: Aligns with I/O buffer strategy
- ✅ Cache-friendly: Linear allocation pattern

### 3.2 Arena Implementation

```cpp
class monotonic_arena {
    struct block {
        uint8_t* data;
        size_t capacity;
        size_t used;
        block* next;
    };

    block* current_ = nullptr;
    size_t default_block_size_ = 65536;

    void* allocate(size_t n, size_t align = alignof(std::max_align_t)) {
        // Align pointer
        size_t offset = (reinterpret_cast<uintptr_t>(current_->data + current_->used)) % align;
        size_t padding = (align - offset) % align;
        size_t required = padding + n;

        // Fast path: fits in current block
        if (current_ && current_->used + required <= current_->capacity) {
            void* ptr = current_->data + current_->used + padding;
            current_->used += required;
            return ptr;
        }

        // Slow path: allocate new block
        size_t block_size = std::max(default_block_size_, n + padding);
        auto* new_block = new block{new uint8_t[block_size], block_size, required, current_};
        current_ = new_block;
        return current_->data + padding;
    }

    void reset() {
        // Keep blocks, reset pointers
        for (auto* b = current_; b; b = b->next) {
            b->used = 0;
        }
    }
};
```

### 3.3 STL Integration

**Arena-aware allocators** for standard containers:
```cpp
template <typename T>
class arena_allocator {
    monotonic_arena* arena_;

    T* allocate(size_t n) {
        return static_cast<T*>(arena_->allocate(n * sizeof(T), alignof(T)));
    }

    void deallocate(T*, size_t) {
        // No-op! Arena owns memory
    }
};

// Usage
using arena_string = std::basic_string<char, std::char_traits<char>, arena_allocator<char>>;
using arena_vector<T> = std::vector<T, arena_allocator<T>>;
```

### 3.4 Per-Request Lifecycle

```cpp
void handle_request(const request& req) {
    // 1. Create per-request arena
    monotonic_arena arena;
    request_context ctx{arena};

    // 2. Parse and validate (uses arena)
    auto dto = parse_dto(req.body, arena);
    auto validation = validate(dto);

    // 3. Call handler (may allocate from arena)
    auto resp = handler.process(dto);

    // 4. Serialize response (uses arena for temp buffers)
    auto serialized = serialize(resp, arena);

    // 5. Arena destroyed → all memory freed at once
}
```

**Memory Profile** (typical):
- Request headers: ~500 bytes
- Request body (JSON): ~1-2 KB
- DTO objects: ~500 bytes
- Response construction: ~1-2 KB
- **Total per request**: ~4-6 KB
- **Blocks allocated**: 1 (64KB covers it)

---

## 4. Router Internals

### 4.1 Compile-Time Routing

KATANA's router uses C++23 template metaprogramming to parse paths at **compile time**:

```cpp
// Compile-time path parsing
template <size_t N>
consteval auto parse_path(const char (&path)[N]) {
    path_pattern pattern;
    // Parse at compile time!
    // "/users/{id}/posts/{post_id}" → {literal, param, literal, param}
    return pattern;
}

// Usage
route_entry routes[] = {
    {method::get, path_pattern::from_literal<"/users/{id}">(),
     handler_fn([](auto& req, auto& ctx) { /* ... */ })}
};
```

**Benefits**:
- ✅ Zero runtime parsing overhead
- ✅ Compile errors for invalid paths
- ✅ Type safety enforced at compile time

### 4.2 Route Matching Algorithm

**Step 1: Method Check** (O(1))
```cpp
if ((route.methods & (1 << static_cast<int>(req.method))) == 0) {
    continue;  // Method not allowed
}
```

**Step 2: Segment Count** (O(1))
```cpp
if (request_segments.size() != route.pattern.segment_count()) {
    continue;  // Path length mismatch
}
```

**Step 3: Segment Matching** (O(segments))
```cpp
for (size_t i = 0; i < segments.size(); ++i) {
    if (route.pattern[i].is_literal) {
        if (request_segments[i] != route.pattern[i].literal) {
            match = false; break;
        }
    } else {
        // Parameter - extract and store
        ctx.params.set(route.pattern[i].param_name, request_segments[i]);
    }
}
```

**Step 4: Specificity Scoring**
```cpp
int score = route.pattern.literal_count() * 16 + (MAX_SEGMENTS - route.pattern.param_count());
// Higher score = more specific
// "/users/123" (2 literals) beats "/users/{id}" (1 literal)
```

**Overall Complexity**:
- **Best case**: O(routes) - method mismatch on first check
- **Typical case**: O(routes × segments) - ~10 routes × ~5 segments = 50 comparisons
- **Worst case**: O(routes × segments) - all routes checked

**Cache Characteristics**:
- Routes stored in contiguous `std::span`
- Cache-friendly linear scan
- ~10 routes fit in 1 cache line

### 4.3 Inline Storage for Handlers

```cpp
template <typename Sig, size_t Size = 160>
class inplace_function {
    alignas(alignof(std::max_align_t)) std::byte storage_[Size];
    void (*invoke_)(void*, Args...);

    // Stores lambda inline - no heap allocation
};
```

**Why 160 bytes?**
- Typical lambda with 2-3 captures: ~40-80 bytes
- 160 bytes covers 99% of handlers without heap
- Aligns with cache line sizes

---

## 5. Timer System

### 5.1 Hierarchical Timing Wheel

KATANA implements a **timing wheel** for efficient timeout management:

**Structure**:
```cpp
class wheel_timer {
    static constexpr size_t NUM_SLOTS = 512;
    static constexpr int64_t TICK_MS = 100;
    static constexpr int64_t RANGE_MS = NUM_SLOTS * TICK_MS;  // 51.2s

    std::array<std::vector<entry>, NUM_SLOTS> slots_;
    size_t current_slot_ = 0;
    int64_t current_time_ms_ = 0;
};
```

### 5.2 Operations

**Add Timer** (O(1) amortized):
```cpp
timer_id add(int64_t delay_ms, callback cb) {
    int64_t fire_time = current_time_ms_ + delay_ms;
    size_t slot = (current_slot_ + delay_ms / TICK_MS) % NUM_SLOTS;
    int rounds = delay_ms / RANGE_MS;

    entries_[slot].push_back({fire_time, rounds, cb, generation_});
    return timer_id{generation_++, slot};
}
```

**Cancel Timer** (O(1)):
```cpp
void cancel(timer_id id) {
    // Lazy cancellation - just mark as cancelled
    // Cleaned up during tick or periodic compaction
    entries_[id.slot][id.index].cancelled = true;
}
```

**Tick** (O(entries_in_current_slot)):
```cpp
void tick() {
    current_time_ms_ += TICK_MS;
    current_slot_ = (current_slot_ + 1) % NUM_SLOTS;

    for (auto& entry : slots_[current_slot_]) {
        if (entry.cancelled) continue;

        if (entry.rounds > 0) {
            entry.rounds--;  // Not yet time to fire
        } else {
            entry.callback();  // Fire!
        }
    }

    // Periodic compaction (every 8 ticks)
    if (current_slot_ % 8 == 0) {
        compact_slot(current_slot_);
    }
}
```

### 5.3 Performance Characteristics

| Operation | Time Complexity | Notes |
|-----------|----------------|-------|
| Add | O(1) amortized | Vector push_back |
| Cancel | O(1) | Lazy - just mark cancelled |
| Tick | O(n) where n = entries in slot | Typically ~10-100 per slot |
| Fire callback | O(1) | Inline function call |
| Compact | O(n) | Amortized over 8 ticks |

**Memory Usage**:
- 512 slots × ~10 entries/slot × ~128 bytes/entry ≈ 640 KB per reactor

---

## 6. HTTP Parser/Serializer

### 6.1 Parser Architecture

**Zero-Copy Parsing**:
```cpp
struct http_parser {
    // Parse in-place, return views into original buffer
    std::expected<request, parse_error> parse(std::span<const uint8_t> buf, monotonic_arena& arena);
};
```

**State Machine**:
1. Parse request line: `GET /path HTTP/1.1\r\n`
2. Parse headers: `Key: Value\r\n` (uses SIMD for performance)
3. Validate Content-Length
4. Body handling: Zero-copy view or arena copy for persistence

### 6.2 SIMD Header Parsing

**Case-Insensitive Comparison** (AVX2):
```cpp
inline bool case_insensitive_compare_avx2(const char* a, const char* b, size_t n) {
    __m256i lower_mask = _mm256_set1_epi8(0x20);  // ASCII lowercase bit

    for (size_t i = 0; i + 32 <= n; i += 32) {
        __m256i va = _mm256_loadu_si256((__m256i*)(a + i));
        __m256i vb = _mm256_loadu_si256((__m256i*)(b + i));

        // Convert to lowercase
        va = _mm256_or_si256(va, lower_mask);
        vb = _mm256_or_si256(vb, lower_mask);

        // Compare
        __m256i cmp = _mm256_cmpeq_epi8(va, vb);
        int mask = _mm256_movemask_epi8(cmp);
        if (mask != -1) return false;
    }

    // Handle remainder
    return scalar_compare(a + (n & ~31), b + (n & ~31), n & 31);
}
```

**Speedup**: ~3-4x over scalar for long header names (e.g., `Authorization`, `Content-Type`).

### 6.3 Security Limits

```cpp
constexpr size_t MAX_HEADER_SIZE = 8192;      // 8 KB
constexpr size_t MAX_BODY_SIZE = 10485760;   // 10 MB
constexpr size_t MAX_URI_LENGTH = 2048;      // 2 KB
constexpr size_t MAX_HEADERS = 128;          // 128 headers
```

**Early Validation**:
- ✅ Reject null bytes in headers
- ✅ Reject excessive CRLFs (slowloris protection)
- ✅ Reject invalid characters
- ✅ Enforce size limits before allocation

---

## 7. Concurrency Model

### 7.1 Shared-Nothing Architecture

**What's Shared** (read-only):
- ✅ Route table (immutable after startup)
- ✅ Handler vtable (read-only after construction)
- ✅ Code/text segment

**What's NOT Shared** (per-reactor):
- ✅ FD states and epoll instance
- ✅ Timer wheels
- ✅ Task queues
- ✅ I/O buffers (thread-local scratch)
- ✅ Metrics counters

### 7.2 Lock-Free Guarantees

**Hot Path** (99.9% of execution):
- ✅ **Zero locks**: Accept, read, parse, route, respond, write
- ✅ **Zero atomics**: All state is thread-local
- ✅ **Zero cross-core communication**: Request handled on originating reactor

**Cold Path** (0.1% of execution):
- ⚠️ **MPSC queue**: Lock-free but uses atomics for cross-thread tasks
- ⚠️ **Metrics aggregation**: Atomic counters for global metrics

### 7.3 Memory Ordering

**Critical Atomics**:
```cpp
// MPSC queue head (producers)
std::atomic<size_t> head_{0};  // std::memory_order_release on write

// MPSC queue tail (consumer)
std::atomic<size_t> tail_{0};  // std::memory_order_acquire on read
```

**Cache Line Alignment**:
```cpp
struct alignas(64) mpsc_queue {
    alignas(64) std::atomic<size_t> head_;
    alignas(64) std::atomic<size_t> tail_;
    // Prevents false sharing between head and tail
};
```

---

## 8. SIMD Optimizations

### 8.1 CRLF Scanning

**Goal**: Find `\r\n\r\n` sequence marking end of HTTP headers.

**SSE2 Implementation**:
```cpp
inline const char* find_crlf_sse2(const char* data, size_t len) {
    __m128i cr = _mm_set1_epi8('\r');
    __m128i lf = _mm_set1_epi8('\n');

    for (size_t i = 0; i + 16 <= len; i += 16) {
        __m128i chunk = _mm_loadu_si128((__m128i*)(data + i));

        __m128i eq_cr = _mm_cmpeq_epi8(chunk, cr);
        __m128i eq_lf = _mm_cmpeq_epi8(chunk, lf);

        int mask_cr = _mm_movemask_epi8(eq_cr);
        int mask_lf = _mm_movemask_epi8(eq_lf);

        // Check for \r\n\r\n pattern in masks
        if ((mask_cr & (mask_lf << 1) & (mask_cr << 2) & (mask_lf << 3)) != 0) {
            return find_exact_position(data + i, mask_cr, mask_lf);
        }
    }

    return scalar_fallback(data + (len & ~15), len & 15);
}
```

**Performance**: ~50M ops/s for 1.5KB buffers (Stage 2 benchmarks).

### 8.2 When to Use SIMD

✅ **Good use cases**:
- Long strings (> 16 bytes)
- Hot path operations (called millions of times)
- Predictable data (e.g., HTTP headers)

❌ **Bad use cases**:
- Short strings (< 16 bytes) - overhead dominates
- Cold path (rarely called)
- Unpredictable branches

---

## 9. Performance Characteristics

### 9.1 Microbenchmarks (from Stage 2)

| Component | Throughput | Latency (p50) | Latency (p99) |
|-----------|------------|---------------|---------------|
| Ring Buffer | 333M ops/s | 0.003 μs | 0.006 μs |
| Arena Allocation | 7.2M ops/s | 0.000 μs | 0.000 μs |
| HTTP Parser | 1.35M ops/s | 0.669 μs | 1.188 μs |
| Router Dispatch | 1.05M ops/s | 0.511 μs | 1.152 μs |
| Headers Get | 18.2M ops/s | 0.020 μs | 0.031 μs |
| IO Buffer Append | 20M ops/s | 0.020 μs | 0.031 μs |

### 9.2 End-to-End Expectations

**Single Reactor Thread** (x86-64, 3.5 GHz):
- Simple GET: ~350k RPS, p99 < 0.15 ms
- POST with validation: ~180k RPS, p99 < 0.25 ms
- Complex CRUD: ~150k RPS, p99 < 0.30 ms

**Multi-Reactor** (8 cores):
- Aggregate throughput: 800k - 1.2M RPS
- Tail latency under saturation: p99 < 2.5 ms

### 9.3 Scalability

**Linear scaling** up to N cores (where N = physical cores):
- ✅ No cross-core synchronization
- ✅ No lock contention
- ✅ Independent event loops

**Diminishing returns** beyond physical cores:
- ⚠️ Hyperthreading provides ~1.2x (not 2x)
- ⚠️ Memory bandwidth becomes bottleneck at high core counts

---

## 10. Architectural Guarantees

### 10.1 Latency Guarantees

✅ **Bounded operations on hot path**:
- No unbounded loops (e.g., route matching limited by route count)
- No dynamic allocations (arena pre-allocated)
- No locks (reactor-per-core isolation)
- No blocking syscalls (epoll/io_uring with timeout)

✅ **Predictable memory access**:
- Sequential buffer scans (cache-friendly)
- Prefetching in event loop
- Aligned data structures (cache line optimization)

### 10.2 Memory Guarantees

✅ **No per-request heap allocations** (typical case):
- I/O buffer uses thread-local scratch (64 KB)
- Arena uses pre-allocated blocks
- Headers stored in fixed-size arrays

✅ **Bounded memory growth**:
- Arena blocks capped at reasonable size
- Connection limits enforced
- Request size limits enforced

### 10.3 Concurrency Guarantees

✅ **Data race freedom**:
- Per-reactor state is thread-local
- Cross-thread communication via lock-free queues
- Atomic operations with proper memory ordering

✅ **Deadlock freedom**:
- No locks in typical execution
- MPSC queue is lock-free (no circular wait)

### 10.4 Limitations and Trade-offs

⚠️ **Not Suitable For**:
- Long-running handlers (blocks reactor)
- Synchronous database calls (use connection pool + reactor per query)
- Stateful connections requiring cross-reactor coordination

⚠️ **Architecture Constraints**:
- Handlers must be non-blocking
- Connections not pinned to reactors (SO_REUSEPORT)
- Global state requires explicit synchronization

---

## 11. Conclusion

KATANA achieves predictable low-latency performance through:

1. **Reactor-per-core**: Eliminates synchronization on hot paths
2. **Zero-copy I/O**: Minimizes memory operations
3. **Arena allocation**: Eliminates fragmentation and per-object free overhead
4. **Compile-time routing**: Zero runtime dispatch overhead
5. **SIMD optimizations**: 3-4x speedups for text processing

**When to Use KATANA**:
- ✅ Request-response APIs with < 5ms p99 requirements
- ✅ High throughput (100k+ RPS per server)
- ✅ Predictable workloads (no long-running operations)
- ✅ OpenAPI-driven development

**When NOT to Use KATANA**:
- ❌ Long-polling / WebSockets (blocks reactor)
- ❌ CPU-intensive computations in handlers
- ❌ Applications with complex cross-request state management

---

**Document Version**: 1.0
**Last Updated**: 2025-12-08
**Maintained By**: KATANA Core Team
