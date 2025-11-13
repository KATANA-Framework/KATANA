# Further Optimization Opportunities

This document outlines additional optimization opportunities to push performance even further beyond the current improvements.

## Current Status (After Runtime Optimizations)

### Achieved
- ✅ Bounded arena pool: 640MB → 6-12MB for 10k connections
- ✅ MSG_MORE/sendmsg() for write batching hints
- ✅ Serialization barrier for safe async I/O
- ✅ io_buffer::release() for idle connection cleanup
- ✅ Arena shrinking at 256KB threshold

### Metrics
- Latency p99: 0.149ms (target: <0.1ms)
- Latency p50: 0.050ms (excellent)
- Throughput: 1.2M req/s @ 128 concurrent (excellent)
- Keep-alive: 28.5k req/s (can improve)

## High-Impact Optimizations (Estimated gains)

### 1. Headers Map Optimization (~10-15% improvement)
**Current:** 32-slot block array (800+ bytes)
**Problem:** Cache unfriendly, too large for typical use case

**Solution:**
```cpp
// Reduce to 16 slots (sufficient for 95% of requests)
static constexpr size_t BLOCK_SIZE = 16;

// Add inline storage for short strings (avoid heap allocations)
struct inline_string {
    union {
        char inline_data[23];  // 23 bytes inline
        struct {
            char* heap_ptr;
            size_t len;
            size_t cap;
        } heap;
    };
    uint8_t flags;  // 1 bit: is_heap
};
```

**Impact:** -100 bytes per request, 10-20% less memory, better cache locality

### 2. Perfect Hash for Standard Headers (~5-10% improvement)
**Current:** Linear search through block array
**Problem:** O(n) lookup for every header access

**Solution:**
```cpp
// Compile-time perfect hash for standard HTTP headers
enum class standard_header : uint8_t {
    content_type = 0,
    content_length = 1,
    connection = 2,
    // ... etc
};

constexpr uint8_t perfect_hash(std::string_view name) {
    // Fast compile-time hash
}

// O(1) lookup for standard headers, fallback to linear for custom
```

**Impact:** 5-10% faster header lookups, critical path optimization

### 3. Parser SIMD Extensions (~15-20% improvement)
**Current:** SIMD only for CRLF search
**Problem:** Byte-by-byte parsing for tokens

**Solution:**
```cpp
// Extend SIMD to:
// - HTTP method detection (GET/POST/PUT/DELETE)
// - Header name validation (alphanumeric + hyphen)
// - Whitespace skipping
// - URI validation

__m256i methods = _mm256_loadu_si256(buffer);
// Parallel comparison for "GET ", "POST", etc.
```

**Impact:** 15-20% faster parsing, especially for large headers

### 4. Branch Prediction Hints (~2-5% improvement)
**Current:** No explicit hints
**Problem:** CPU mis-predicts error paths

**Solution:**
```cpp
#define LIKELY(x)   __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

if (UNLIKELY(parse_error)) {
    return error;
}

if (LIKELY(has_body)) {
    parse_body();
}
```

**Impact:** 2-5% improvement in hot paths

### 5. Arena Prefetching (~3-5% improvement)
**Current:** Sequential allocations
**Problem:** Cache misses on arena access

**Solution:**
```cpp
void* allocate(size_t bytes) {
    __builtin_prefetch(current_block->data + current_block->used + 64);
    // ... allocation logic
}
```

**Impact:** 3-5% fewer cache misses in parser

### 6. Zero-Copy Response (~10-15% improvement)
**Current:** Serialize to heap buffer
**Problem:** Extra copy for static responses

**Solution:**
```cpp
struct response {
    enum class type { static_data, dynamic_data };
    type response_type;

    union {
        struct {
            const char* data;
            size_t len;
        } static_view;
        std::vector<uint8_t> dynamic_buffer;
    };
};

// For "Hello, World!" use static view, no allocation
```

**Impact:** 10-15% faster for small static responses

### 7. Connection Pool (~5-10% improvement)
**Current:** shared_ptr per connection
**Problem:** Atomic refcount overhead, allocations

**Solution:**
```cpp
// Intrusive connection pool
struct connection {
    connection* next_free;  // 8 bytes
    // ... other fields
};

class connection_pool {
    connection* free_list;
    std::vector<std::unique_ptr<connection>> storage;
};
```

**Impact:** 5-10% less overhead, especially under load

### 8. SO_BUSYPOLL for Ultra-Low Latency (~20-30% p99 improvement)
**Current:** Standard epoll_wait
**Problem:** Context switch latency

**Solution:**
```cpp
// For low-latency mode
setsockopt(fd, SOL_SOCKET, SO_BUSY_POLL, &busy_poll_us, sizeof(busy_poll_us));

// Busy-poll for first N microseconds before sleeping
int epoll_wait_adaptive(int timeout_ms) {
    auto start = now();
    while (duration(now() - start) < BUSY_POLL_THRESHOLD) {
        int n = epoll_wait(epoll_fd, events, max_events, 0);
        if (n > 0) return n;
    }
    return epoll_wait(epoll_fd, events, max_events, timeout_ms);
}
```

**Impact:** 20-30% better p99 latency for low-latency workloads
**Cost:** Higher CPU usage

### 9. io_uring (Full Implementation) (~30-40% improvement)
**Current:** Stub only
**Problem:** syscall overhead still present

**Solution:**
- Implement full io_uring reactor
- SQPOLL mode for zero-syscall I/O
- Registered buffers for zero-copy
- Batch 100+ operations in single syscall

**Impact:** 30-40% throughput improvement, p99 < 0.5ms
**Effort:** ~1-2 weeks

### 10. Jemalloc/Mimalloc Integration (~5-10% improvement)
**Current:** System allocator
**Problem:** Not optimized for this workload

**Solution:**
```cmake
option(KATANA_USE_JEMALLOC "Use jemalloc" ON)
if(KATANA_USE_JEMALLOC)
    find_package(jemalloc REQUIRED)
    target_link_libraries(katana_core PRIVATE jemalloc)
endif()
```

**Impact:** 5-10% better memory performance
**Effort:** 1-2 days

## Medium-Impact Optimizations

### 11. SSE4.2 String Operations
- Use SSE4.2 CRC32 instruction for hashing
- Faster than current FNV hash

### 12. Huge Pages for Arena Blocks
- Use mmap() with MAP_HUGETLB for large allocations
- Reduce TLB misses

### 13. NUMA Awareness
- Pin threads to NUMA nodes
- Allocate arenas from local NUMA node

### 14. Custom String Class
- Replace std::string with fixed-size string
- Avoid SSO boundary allocations

## Low-Impact Optimizations

### 15. Compiler Optimizations
- PGO (Profile-Guided Optimization)
- LTO (Link-Time Optimization) - already enabled
- -march=native - already enabled

### 16. TCP Tuning
- TCP_QUICKACK for lower latency
- SO_INCOMING_CPU for better CPU affinity

### 17. Metrics Optimization
- Use thread-local counters
- Batch metric updates

## Priority Ranking for Maximum Performance

**Phase 1 (Quick Wins - 1-2 days):**
1. Branch prediction hints (LIKELY/UNLIKELY)
2. Headers map size reduction (32→16 slots)
3. Zero-copy static responses

**Phase 2 (Medium Effort - 3-5 days):**
4. Perfect hash for standard headers
5. Parser SIMD extensions
6. Connection pool
7. Jemalloc integration

**Phase 3 (High Effort - 1-2 weeks):**
8. Full io_uring implementation
9. SO_BUSYPOLL adaptive polling
10. Arena prefetching + NUMA awareness

## Expected Final Results

After all optimizations:
- **Throughput:** 2-3M req/s @ 128 concurrent (2-3x improvement)
- **Latency p50:** 0.020-0.030ms (40-50% improvement)
- **Latency p99:** 0.050-0.080ms (50-70% improvement)
- **Memory:** 3-6MB for 10k connections (50% additional reduction)

## Measurement Strategy

For each optimization:
1. Implement in isolated branch
2. Run full benchmark suite
3. Compare against baseline
4. Document results
5. Merge if >3% improvement

## References

- [io_uring documentation](https://kernel.dk/io_uring.pdf)
- [SO_BUSYPOLL](https://lwn.net/Articles/551284/)
- [SIMD text processing](https://branchfree.org/2018/05/22/parsing-gigabytes-of-json-per-second/)
- [Perfect hashing](https://en.wikipedia.org/wiki/Perfect_hash_function)
