#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

// x86_64 SIMD support
#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#ifndef KATANA_HAS_SSE2
#define KATANA_HAS_SSE2
#endif
#ifdef __AVX2__
#ifndef KATANA_HAS_AVX2
#define KATANA_HAS_AVX2
#endif
#endif
#endif

// ARM NEON support (Apple Silicon, ARM64)
#if defined(__aarch64__) || defined(__arm64__) || defined(_M_ARM64)
#include <arm_neon.h>
#ifndef KATANA_HAS_NEON
#define KATANA_HAS_NEON
#endif
#endif

namespace katana::simd {

inline const char* find_crlf_scalar(const char* data, size_t len) noexcept {
    if (len < 2)
        return nullptr;

    for (size_t i = 0; i <= len - 2; ++i) {
        if (data[i] == '\r' && data[i + 1] == '\n') {
            return data + i;
        }
    }
    return nullptr;
}

#ifdef KATANA_HAS_AVX2
inline const char* find_crlf_avx2(const char* data, size_t len) noexcept {
    if (len < 2)
        return nullptr;

    const __m256i cr = _mm256_set1_epi8('\r');
    const __m256i lf = _mm256_set1_epi8('\n');

    size_t i = 0;

    // For large buffers (>4KB), use aggressive prefetching and 2x unrolling
    // to maximize memory throughput and hide latency
    if (len >= 4096) {
        // Prefetch first cache lines
        _mm_prefetch(data, _MM_HINT_T0);
        _mm_prefetch(data + 64, _MM_HINT_T0);
        _mm_prefetch(data + 128, _MM_HINT_T0);

        // Process 64 bytes per iteration (2x32) with prefetch
        for (; i + 65 <= len; i += 64) {
            // Prefetch 256 bytes ahead (4 cache lines) for streaming access
            _mm_prefetch(data + i + 256, _MM_HINT_T0);
            _mm_prefetch(data + i + 320, _MM_HINT_T0);

            // First 32-byte block
            __m256i chunk0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
            __m256i next0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i + 1));
            __m256i cr0 = _mm256_cmpeq_epi8(chunk0, cr);
            __m256i lf0 = _mm256_cmpeq_epi8(next0, lf);
            __m256i match0 = _mm256_and_si256(cr0, lf0);
            int mask0 = _mm256_movemask_epi8(match0);

            if (mask0 != 0) {
                return data + i + static_cast<size_t>(__builtin_ctz(static_cast<unsigned>(mask0)));
            }

            // Second 32-byte block
            __m256i chunk1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i + 32));
            __m256i next1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i + 33));
            __m256i cr1 = _mm256_cmpeq_epi8(chunk1, cr);
            __m256i lf1 = _mm256_cmpeq_epi8(next1, lf);
            __m256i match1 = _mm256_and_si256(cr1, lf1);
            int mask1 = _mm256_movemask_epi8(match1);

            if (mask1 != 0) {
                return data + i + 32 +
                       static_cast<size_t>(__builtin_ctz(static_cast<unsigned>(mask1)));
            }
        }
    }

    // Standard path for smaller buffers or remainder
    for (; i + 33 <= len; i += 32) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        __m256i next_chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i + 1));

        __m256i cr_match = _mm256_cmpeq_epi8(chunk, cr);
        __m256i lf_match = _mm256_cmpeq_epi8(next_chunk, lf);

        __m256i crlf_match = _mm256_and_si256(cr_match, lf_match);
        int mask = _mm256_movemask_epi8(crlf_match);
        const auto mask_bits = static_cast<unsigned int>(mask);

        if (mask_bits != 0U) {
            const auto offset = static_cast<size_t>(__builtin_ctz(mask_bits));
            return data + i + offset;
        }
    }

    return find_crlf_scalar(data + i, len - i);
}
#endif

#ifdef KATANA_HAS_SSE2
inline const char* find_crlf_sse2(const char* data, size_t len) noexcept {
    if (len < 2)
        return nullptr;

    const __m128i cr = _mm_set1_epi8('\r');
    const __m128i lf = _mm_set1_epi8('\n');

    size_t i = 0;

    // For large buffers, use prefetching and 4x unrolling
    if (len >= 4096) {
        _mm_prefetch(data, _MM_HINT_T0);
        _mm_prefetch(data + 64, _MM_HINT_T0);

        // Process 64 bytes per iteration (4x16) with prefetch
        for (; i + 65 <= len; i += 64) {
            _mm_prefetch(data + i + 256, _MM_HINT_T0);
            _mm_prefetch(data + i + 320, _MM_HINT_T0);

            // Block 0
            __m128i chunk0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
            __m128i next0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i + 1));
            int mask0 = _mm_movemask_epi8(
                _mm_and_si128(_mm_cmpeq_epi8(chunk0, cr), _mm_cmpeq_epi8(next0, lf)));
            if (mask0 != 0) {
                return data + i + static_cast<size_t>(__builtin_ctz(static_cast<unsigned>(mask0)));
            }

            // Block 1
            __m128i chunk1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i + 16));
            __m128i next1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i + 17));
            int mask1 = _mm_movemask_epi8(
                _mm_and_si128(_mm_cmpeq_epi8(chunk1, cr), _mm_cmpeq_epi8(next1, lf)));
            if (mask1 != 0) {
                return data + i + 16 +
                       static_cast<size_t>(__builtin_ctz(static_cast<unsigned>(mask1)));
            }

            // Block 2
            __m128i chunk2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i + 32));
            __m128i next2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i + 33));
            int mask2 = _mm_movemask_epi8(
                _mm_and_si128(_mm_cmpeq_epi8(chunk2, cr), _mm_cmpeq_epi8(next2, lf)));
            if (mask2 != 0) {
                return data + i + 32 +
                       static_cast<size_t>(__builtin_ctz(static_cast<unsigned>(mask2)));
            }

            // Block 3
            __m128i chunk3 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i + 48));
            __m128i next3 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i + 49));
            int mask3 = _mm_movemask_epi8(
                _mm_and_si128(_mm_cmpeq_epi8(chunk3, cr), _mm_cmpeq_epi8(next3, lf)));
            if (mask3 != 0) {
                return data + i + 48 +
                       static_cast<size_t>(__builtin_ctz(static_cast<unsigned>(mask3)));
            }
        }
    }

    // Standard path for smaller buffers or remainder
    for (; i + 17 <= len; i += 16) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
        __m128i next_chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i + 1));

        __m128i cr_match = _mm_cmpeq_epi8(chunk, cr);
        __m128i lf_match = _mm_cmpeq_epi8(next_chunk, lf);

        __m128i crlf_match = _mm_and_si128(cr_match, lf_match);
        int mask = _mm_movemask_epi8(crlf_match);
        const auto mask_bits = static_cast<unsigned int>(mask);

        if (mask_bits != 0U) {
            const auto offset = static_cast<size_t>(__builtin_ctz(mask_bits));
            return data + i + offset;
        }
    }

    return find_crlf_scalar(data + i, len - i);
}
#endif

#ifdef KATANA_HAS_NEON
inline const char* find_crlf_neon(const char* data, size_t len) noexcept {
    if (len < 2)
        return nullptr;

    const uint8x16_t cr = vdupq_n_u8('\r');
    const uint8x16_t lf = vdupq_n_u8('\n');

    size_t i = 0;
    for (; i + 17 <= len; i += 16) {
        uint8x16_t chunk = vld1q_u8(reinterpret_cast<const uint8_t*>(data + i));
        uint8x16_t next_chunk = vld1q_u8(reinterpret_cast<const uint8_t*>(data + i + 1));

        uint8x16_t cr_match = vceqq_u8(chunk, cr);
        uint8x16_t lf_match = vceqq_u8(next_chunk, lf);

        uint8x16_t crlf_match = vandq_u8(cr_match, lf_match);

        // Convert to scalar mask
        uint64_t mask_low = vgetq_lane_u64(vreinterpretq_u64_u8(crlf_match), 0);
        uint64_t mask_high = vgetq_lane_u64(vreinterpretq_u64_u8(crlf_match), 1);

        if (mask_low != 0) {
            for (size_t j = 0; j < 8; ++j) {
                if ((mask_low >> (j * 8)) & 0xFF) {
                    return data + i + j;
                }
            }
        }
        if (mask_high != 0) {
            for (size_t j = 0; j < 8; ++j) {
                if ((mask_high >> (j * 8)) & 0xFF) {
                    return data + i + 8 + j;
                }
            }
        }
    }

    return find_crlf_scalar(data + i, len - i);
}
#endif

inline const char* find_crlf(const char* data, size_t len) noexcept {
#ifdef KATANA_HAS_AVX2
    return find_crlf_avx2(data, len);
#elif defined(KATANA_HAS_NEON)
    return find_crlf_neon(data, len);
#elif defined(KATANA_HAS_SSE2)
    return find_crlf_sse2(data, len);
#else
    return find_crlf_scalar(data, len);
#endif
}

inline const void*
find_pattern(const void* haystack, size_t hlen, const void* needle, size_t nlen) noexcept {
    if (nlen == 0 || hlen < nlen)
        return nullptr;
    if (nlen == 2) {
        const char* n = static_cast<const char*>(needle);
        if (n[0] == '\r' && n[1] == '\n') {
            return find_crlf(static_cast<const char*>(haystack), hlen);
        }
    }

#ifdef __linux__
    return memmem(haystack, hlen, needle, nlen);
#else
    const char* h = static_cast<const char*>(haystack);
    const char* n = static_cast<const char*>(needle);

    for (size_t i = 0; i <= hlen - nlen; ++i) {
        if (std::memcmp(h + i, n, nlen) == 0) {
            return h + i;
        }
    }
    return nullptr;
#endif
}

} // namespace katana::simd
