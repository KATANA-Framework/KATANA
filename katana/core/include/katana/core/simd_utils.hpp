#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

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

inline const char* find_crlf(const char* data, size_t len) noexcept {
#ifdef KATANA_HAS_AVX2
    return find_crlf_avx2(data, len);
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
