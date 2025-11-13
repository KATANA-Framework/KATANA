#pragma once

// Branch prediction hints for hot paths
#if defined(__GNUC__) || defined(__clang__)
    #define LIKELY(x)   __builtin_expect(!!(x), 1)
    #define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define LIKELY(x)   (x)
    #define UNLIKELY(x) (x)
#endif

// Force inline for critical functions
#if defined(__GNUC__) || defined(__clang__)
    #define FORCE_INLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
    #define FORCE_INLINE __forceinline
#else
    #define FORCE_INLINE inline
#endif

// Prefetch hints
#if defined(__GNUC__) || defined(__clang__)
    #define PREFETCH_READ(addr)  __builtin_prefetch(addr, 0, 3)
    #define PREFETCH_WRITE(addr) __builtin_prefetch(addr, 1, 3)
#else
    #define PREFETCH_READ(addr)  (void)(addr)
    #define PREFETCH_WRITE(addr) (void)(addr)
#endif

// Assume expression is true (for optimizer)
#if defined(__clang__)
    #define ASSUME(x) __builtin_assume(x)
#elif defined(__GNUC__) && __GNUC__ >= 13
    #define ASSUME(x) __attribute__((assume(x)))
#else
    #define ASSUME(x) do { if (!(x)) __builtin_unreachable(); } while (0)
#endif

// Cold/hot attributes
#if defined(__GNUC__) || defined(__clang__)
    #define COLD_FUNCTION   __attribute__((cold))
    #define HOT_FUNCTION    __attribute__((hot))
#else
    #define COLD_FUNCTION
    #define HOT_FUNCTION
#endif
