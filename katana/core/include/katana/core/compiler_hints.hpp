#pragma once

// Cross-platform compiler hints for performance optimization
// These macros provide portable branch prediction hints across different compilers and C++
// standards

namespace katana {

// Branch prediction hints
#if __cplusplus >= 202002L
// C++20: Use standard attributes
#define KATANA_LIKELY [[likely]]
#define KATANA_UNLIKELY [[unlikely]]
#elif defined(__GNUC__) || defined(__clang__)
// GCC/Clang: Use __builtin_expect
#define KATANA_LIKELY
#define KATANA_UNLIKELY
// Note: These are for statement-level hints. For expression-level:
#define KATANA_EXPECT_TRUE(x) __builtin_expect(!!(x), 1)
#define KATANA_EXPECT_FALSE(x) __builtin_expect(!!(x), 0)
#else
// Other compilers: No-op
#define KATANA_LIKELY
#define KATANA_UNLIKELY
#define KATANA_EXPECT_TRUE(x) (x)
#define KATANA_EXPECT_FALSE(x) (x)
#endif

// Prefetch hints (already portable via compiler builtins)
#if defined(__GNUC__) || defined(__clang__)
#define KATANA_PREFETCH_READ(addr) __builtin_prefetch((addr), 0, 1)
#define KATANA_PREFETCH_WRITE(addr) __builtin_prefetch((addr), 1, 1)
#elif defined(_MSC_VER)
#include <xmmintrin.h>
#define KATANA_PREFETCH_READ(addr) _mm_prefetch((const char*)(addr), _MM_HINT_T0)
#define KATANA_PREFETCH_WRITE(addr) _mm_prefetch((const char*)(addr), _MM_HINT_T0)
#else
#define KATANA_PREFETCH_READ(addr) ((void)0)
#define KATANA_PREFETCH_WRITE(addr) ((void)0)
#endif

// Hot/cold function attributes
#if defined(__GNUC__) || defined(__clang__)
#define KATANA_HOT __attribute__((hot))
#define KATANA_COLD __attribute__((cold))
#else
#define KATANA_HOT
#define KATANA_COLD
#endif

// Force inline
#if defined(__GNUC__) || defined(__clang__)
#define KATANA_FORCE_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define KATANA_FORCE_INLINE __forceinline
#else
#define KATANA_FORCE_INLINE inline
#endif

// No inline
#if defined(__GNUC__) || defined(__clang__)
#define KATANA_NO_INLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define KATANA_NO_INLINE __declspec(noinline)
#else
#define KATANA_NO_INLINE
#endif

} // namespace katana
