#pragma once

#include "compiler_hints.hpp"

#include <cstdint>
#include <cstddef>
#include <string_view>

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#define KATANA_HAS_SIMD 1
#endif

namespace katana::simd {

// SIMD-accelerated HTTP method detection
enum class http_method : uint8_t {
    get,
    post,
    put,
    delete_,
    patch,
    head,
    options,
    unknown
};

#ifdef KATANA_HAS_SIMD

// Fast HTTP method detection using SIMD
inline http_method detect_method_simd(const char* data, size_t len) noexcept {
    if (UNLIKELY(len < 3)) return http_method::unknown;

    // Load first 8 bytes
    uint64_t first8;
    if (len >= 8) {
        __asm__ __volatile__("" ::: "memory");
        std::memcpy(&first8, data, 8);
    } else {
        // Partial load with zero padding
        first8 = 0;
        std::memcpy(&first8, data, len);
    }

    // Check for common methods using bit patterns
    // "GET " = 0x20544547
    if ((first8 & 0xFFFFFFFF) == 0x20544547) {
        return http_method::get;
    }

    // "POST" = 0x54534F50
    if ((first8 & 0xFFFFFFFF) == 0x54534F50) {
        return http_method::post;
    }

    // "PUT " = 0x20545550
    if ((first8 & 0xFFFFFFFF) == 0x20545550) {
        return http_method::put;
    }

    // "HEAD" = 0x44414548
    if ((first8 & 0xFFFFFFFF) == 0x44414548) {
        return http_method::head;
    }

    // "DELE" (DELETE prefix)
    if ((first8 & 0xFFFFFFFF) == 0x454C4544) {
        if (len >= 6 && data[4] == 'T' && data[5] == 'E') {
            return http_method::delete_;
        }
    }

    // "PATC" (PATCH prefix)
    if ((first8 & 0xFFFFFFFF) == 0x43544150) {
        if (len >= 5 && data[4] == 'H') {
            return http_method::patch;
        }
    }

    // "OPTI" (OPTIONS prefix)
    if ((first8 & 0xFFFFFFFF) == 0x4954504F) {
        return http_method::options;
    }

    return http_method::unknown;
}

#else

// Fallback for non-SIMD platforms
inline http_method detect_method_simd(const char* data, size_t len) noexcept {
    if (UNLIKELY(len < 3)) return http_method::unknown;

    // Simple byte-by-byte comparison
    if (len >= 3 && data[0] == 'G' && data[1] == 'E' && data[2] == 'T') {
        return http_method::get;
    }
    if (len >= 4 && data[0] == 'P' && data[1] == 'O' && data[2] == 'S' && data[3] == 'T') {
        return http_method::post;
    }
    if (len >= 3 && data[0] == 'P' && data[1] == 'U' && data[2] == 'T') {
        return http_method::put;
    }
    if (len >= 4 && data[0] == 'H' && data[1] == 'E' && data[2] == 'A' && data[3] == 'D') {
        return http_method::head;
    }
    if (len >= 6 && data[0] == 'D' && data[1] == 'E' && data[2] == 'L' &&
        data[3] == 'E' && data[4] == 'T' && data[5] == 'E') {
        return http_method::delete_;
    }
    if (len >= 5 && data[0] == 'P' && data[1] == 'A' && data[2] == 'T' &&
        data[3] == 'C' && data[4] == 'H') {
        return http_method::patch;
    }
    if (len >= 7 && data[0] == 'O' && data[1] == 'P' && data[2] == 'T' &&
        data[3] == 'I' && data[4] == 'O' && data[5] == 'N' && data[6] == 'S') {
        return http_method::options;
    }

    return http_method::unknown;
}

#endif

// SIMD-accelerated whitespace skipping
inline size_t skip_whitespace_simd(const char* data, size_t len) noexcept {
#if defined(__SSE2__) || defined(__AVX2__)
    size_t pos = 0;

    #ifdef __AVX2__
    // AVX2: process 32 bytes at a time
    __m256i spaces = _mm256_set1_epi8(' ');
    __m256i tabs = _mm256_set1_epi8('\t');

    while (pos + 32 <= len) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + pos));
        __m256i is_space = _mm256_cmpeq_epi8(chunk, spaces);
        __m256i is_tab = _mm256_cmpeq_epi8(chunk, tabs);
        __m256i is_ws = _mm256_or_si256(is_space, is_tab);

        uint32_t mask = _mm256_movemask_epi8(is_ws);
        if (mask != 0xFFFFFFFF) {
            // Found non-whitespace
            return pos + __builtin_ctz(~mask);
        }
        pos += 32;
    }
    #endif

    #ifdef __SSE2__
    // SSE2: process 16 bytes at a time
    __m128i spaces = _mm_set1_epi8(' ');
    __m128i tabs = _mm_set1_epi8('\t');

    while (pos + 16 <= len) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + pos));
        __m128i is_space = _mm_cmpeq_epi8(chunk, spaces);
        __m128i is_tab = _mm_cmpeq_epi8(chunk, tabs);
        __m128i is_ws = _mm_or_si128(is_space, is_tab);

        uint16_t mask = _mm_movemask_epi8(is_ws);
        if (mask != 0xFFFF) {
            return pos + __builtin_ctz(~mask);
        }
        pos += 16;
    }
    #endif

    // Fallback for remaining bytes
    while (pos < len && (data[pos] == ' ' || data[pos] == '\t')) {
        ++pos;
    }

    return pos;
#else
    // Non-SIMD fallback
    size_t pos = 0;
    while (pos < len && (data[pos] == ' ' || data[pos] == '\t')) {
        ++pos;
    }
    return pos;
#endif
}

// SIMD-accelerated header name validation (alphanumeric + hyphen)
inline bool validate_header_name_simd(const char* data, size_t len) noexcept {
#ifdef __SSE2__
    if (UNLIKELY(len == 0)) return false;

    size_t pos = 0;
    __m128i lower_bound = _mm_set1_epi8('!');
    __m128i upper_bound = _mm_set1_epi8('~');

    while (pos + 16 <= len) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + pos));
        __m128i ge_lower = _mm_cmpgt_epi8(chunk, lower_bound);
        __m128i le_upper = _mm_cmplt_epi8(chunk, upper_bound);
        __m128i valid = _mm_and_si128(ge_lower, le_upper);

        if (_mm_movemask_epi8(valid) != 0xFFFF) {
            return false; // Invalid character found
        }
        pos += 16;
    }

    // Validate remaining bytes
    for (; pos < len; ++pos) {
        char c = data[pos];
        if (c < '!' || c > '~' || c == ':') {
            return false;
        }
    }

    return true;
#else
    // Non-SIMD fallback
    for (size_t i = 0; i < len; ++i) {
        char c = data[i];
        if (c < '!' || c > '~' || c == ':') {
            return false;
        }
    }
    return true;
#endif
}

} // namespace katana::simd
