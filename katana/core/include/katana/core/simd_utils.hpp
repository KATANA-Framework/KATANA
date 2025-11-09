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
    if (len < 2) return nullptr;

    for (size_t i = 0; i <= len - 2; ++i) {
        if (data[i] == '\r' && data[i + 1] == '\n') {
            return data + i;
        }
    }
    return nullptr;
}

#ifdef KATANA_HAS_AVX2
inline const char* find_crlf_avx2(const char* data, size_t len) noexcept {
    if (len < 2) return nullptr;

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
    if (len < 2) return nullptr;

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

inline const void* find_pattern(const void* haystack, size_t hlen,
                                 const void* needle, size_t nlen) noexcept {
    if (nlen == 0 || hlen < nlen) return nullptr;
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

#ifdef KATANA_HAS_AVX2
inline bool validate_http_chars_avx2(const char* data, size_t len) noexcept {
    const __m256i zero = _mm256_setzero_si256();
    const __m256i threshold = _mm256_set1_epi8(static_cast<char>(0x80));

    size_t i = 0;
    for (; i + 32 <= len; i += 32) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        __m256i is_zero = _mm256_cmpeq_epi8(chunk, zero);
        __m256i is_high = _mm256_cmpgt_epi8(chunk, threshold);
        __m256i invalid = _mm256_or_si256(is_zero, is_high);

        if (_mm256_movemask_epi8(invalid) != 0) {
            return false;
        }
    }

    for (; i < len; ++i) {
        uint8_t byte = static_cast<uint8_t>(data[i]);
        if (byte == 0 || byte >= 0x80) {
            return false;
        }
    }

    return true;
}
#endif

#ifdef KATANA_HAS_SSE2
inline bool validate_http_chars_sse2(const char* data, size_t len) noexcept {
    const __m128i zero = _mm_setzero_si128();
    const __m128i threshold = _mm_set1_epi8(static_cast<char>(0x7F));

    size_t i = 0;
    for (; i + 16 <= len; i += 16) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
        __m128i is_zero = _mm_cmpeq_epi8(chunk, zero);
        __m128i is_high = _mm_cmpgt_epi8(chunk, threshold);
        __m128i invalid = _mm_or_si128(is_zero, is_high);

        if (_mm_movemask_epi8(invalid) != 0) {
            return false;
        }
    }

    for (; i < len; ++i) {
        uint8_t byte = static_cast<uint8_t>(data[i]);
        if (byte == 0 || byte >= 0x80) {
            return false;
        }
    }

    return true;
}
#endif

inline bool validate_http_chars(const char* data, size_t len) noexcept {
#ifdef KATANA_HAS_AVX2
    return validate_http_chars_avx2(data, len);
#elif defined(KATANA_HAS_SSE2)
    return validate_http_chars_sse2(data, len);
#else
    for (size_t i = 0; i < len; ++i) {
        uint8_t byte = static_cast<uint8_t>(data[i]);
        if (byte == 0 || byte >= 0x80) {
            return false;
        }
    }
    return true;
#endif
}

inline const char* find_char(const char* data, size_t len, char target) noexcept {
#ifdef KATANA_HAS_AVX2
    const __m256i target_vec = _mm256_set1_epi8(target);
    size_t i = 0;
    for (; i + 32 <= len; i += 32) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        __m256i cmp = _mm256_cmpeq_epi8(chunk, target_vec);
        int mask = _mm256_movemask_epi8(cmp);
        if (mask != 0) {
            return data + i + __builtin_ctz(static_cast<unsigned int>(mask));
        }
    }
    for (; i < len; ++i) {
        if (data[i] == target) return data + i;
    }
#elif defined(KATANA_HAS_SSE2)
    const __m128i target_vec = _mm_set1_epi8(target);
    size_t i = 0;
    for (; i + 16 <= len; i += 16) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
        __m128i cmp = _mm_cmpeq_epi8(chunk, target_vec);
        int mask = _mm_movemask_epi8(cmp);
        if (mask != 0) {
            return data + i + __builtin_ctz(static_cast<unsigned int>(mask));
        }
    }
    for (; i < len; ++i) {
        if (data[i] == target) return data + i;
    }
#else
    for (size_t i = 0; i < len; ++i) {
        if (data[i] == target) return data + i;
    }
#endif
    return nullptr;
}

inline bool has_bare_lf(const char* data, size_t len, bool prev_ends_with_cr) noexcept {
    if (len == 0) return false;

    if (len > 0 && data[0] == '\n' && !prev_ends_with_cr) return true;

    for (size_t i = 1; i < len; ++i) {
        if (data[i] == '\n' && data[i - 1] != '\r') {
            return true;
        }
    }
    return false;
}

} // namespace katana::simd
