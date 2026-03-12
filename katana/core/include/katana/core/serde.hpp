#pragma once

#include <cctype>
#include <charconv>
#include <cstdint>
#include <cstring>
#include <optional>
#include <string>
#include <string_view>

#ifdef __AVX2__
#include <immintrin.h> // AVX2 (includes SSE2)
#elif defined(__SSE2__)
#include <emmintrin.h> // SSE2
#endif

namespace katana::serde {

// Fast JSON whitespace detection (space, tab, newline, carriage return)
// Lookup table is faster than std::isspace (avoids function call and locale checks)
constexpr bool is_json_whitespace_table[256] = {
    false, false, false, false, false, false, false, false, false, true,  true,  false, false,
    true,  false, false, // 0x09=tab, 0x0A=\n, 0x0D=\r
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, true,  false, false, false, false, false, false, false, // 0x20=space
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
};

inline constexpr bool is_json_whitespace(unsigned char c) noexcept {
    return is_json_whitespace_table[c];
}

inline std::string_view trim_view(std::string_view sv) noexcept {
    while (!sv.empty() && is_json_whitespace(static_cast<unsigned char>(sv.front()))) {
        sv.remove_prefix(1);
    }
    while (!sv.empty() && is_json_whitespace(static_cast<unsigned char>(sv.back()))) {
        sv.remove_suffix(1);
    }
    return sv;
}

struct json_cursor {
    const char* ptr;
    const char* end;
    const char* start; // Track start for position calculation

    json_cursor(const char* p, const char* e) : ptr(p), end(e), start(p) {}

    bool eof() const noexcept { return ptr >= end; }

    size_t pos() const noexcept { return static_cast<size_t>(ptr - start); }

    void skip_ws() noexcept {
        // Fast path: if no whitespace at current position, return immediately
        // This is critical for compact JSON where whitespace is minimal
        if (eof() || !is_json_whitespace(static_cast<unsigned char>(*ptr))) {
            return;
        }

#ifdef __SSE2__
        // SIMD path: only use for large amounts of whitespace (8+ chars)
        // This amortizes the SIMD setup overhead
        constexpr size_t simd_threshold = 8;
        constexpr size_t simd_width = 16;

        // Quick scalar skip for small whitespace runs
        const char* scan_ptr = ptr;
        size_t count = 0;
        while (count < simd_threshold && scan_ptr < end &&
               is_json_whitespace(static_cast<unsigned char>(*scan_ptr))) {
            ++scan_ptr;
            ++count;
        }

        // If less than threshold, use scalar result
        if (count < simd_threshold) {
            ptr = scan_ptr;
            return;
        }

        // We have significant whitespace - use SIMD
        ptr = scan_ptr; // Start from where scalar left off

        while (static_cast<size_t>(end - ptr) >= simd_width) {
            __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));

            __m128i space = _mm_set1_epi8(' ');
            __m128i tab = _mm_set1_epi8('\t');
            __m128i lf = _mm_set1_epi8('\n');
            __m128i cr = _mm_set1_epi8('\r');

            __m128i eq_space = _mm_cmpeq_epi8(chunk, space);
            __m128i eq_tab = _mm_cmpeq_epi8(chunk, tab);
            __m128i eq_lf = _mm_cmpeq_epi8(chunk, lf);
            __m128i eq_cr = _mm_cmpeq_epi8(chunk, cr);

            __m128i is_ws =
                _mm_or_si128(_mm_or_si128(eq_space, eq_tab), _mm_or_si128(eq_lf, eq_cr));

            int mask = _mm_movemask_epi8(is_ws);

            if (mask == 0) {
                break;
            }

            if (mask == 0xFFFF) {
                ptr += simd_width;
                continue;
            }

            int first_non_ws = __builtin_ctz(~mask & 0xFFFF);
            ptr += first_non_ws;
            return;
        }
#endif

        // Scalar fallback for remaining bytes
        while (!eof() && is_json_whitespace(static_cast<unsigned char>(*ptr))) {
            ++ptr;
        }
    }

    bool consume(char c) noexcept {
        skip_ws();
        if (eof() || *ptr != c) {
            return false;
        }
        ++ptr;
        return true;
    }

    std::optional<std::string_view> string() noexcept {
        skip_ws();
        if (eof() || *ptr != '\"') {
            return std::nullopt;
        }
        ++ptr;
        const char* str_start = ptr;
        while (!eof() && *ptr != '\"') {
            if (*ptr == '\\' && (ptr + 1) < end) {
                ptr += 2;
                continue;
            }
            ++ptr;
        }
        if (eof()) {
            return std::nullopt;
        }
        const char* stop = ptr;
        ++ptr; // consume closing quote
        return std::string_view(str_start, static_cast<size_t>(stop - str_start));
    }

    bool try_object_start() noexcept { return consume('{'); }
    bool try_object_end() noexcept { return consume('}'); }
    bool try_array_start() noexcept { return consume('['); }
    bool try_array_end() noexcept { return consume(']'); }
    bool try_comma() noexcept { return consume(','); }

    void skip_value() noexcept {
        skip_ws();
        if (eof())
            return;
        char ch = *ptr;
        if (ch == '{' || ch == '[') {
            char open = ch;
            char close = (ch == '{') ? '}' : ']';
            ++ptr;
            int depth = 1;
            while (!eof() && depth > 0) {
                ch = *ptr;
                if (ch == '\"') {
                    // Skip over string content (handles \" inside strings)
                    ++ptr;
                    while (!eof()) {
                        if (*ptr == '\\' && (ptr + 1) < end) {
                            ptr += 2;
                            continue;
                        }
                        if (*ptr == '\"') {
                            ++ptr;
                            break;
                        }
                        ++ptr;
                    }
                } else if (ch == open) {
                    ++depth;
                    ++ptr;
                } else if (ch == close) {
                    --depth;
                    ++ptr;
                } else {
                    ++ptr;
                }
            }
            return;
        }
        if (ch == '\"') {
            (void)string();
            return;
        }
        while (!eof() && *ptr != ',' && *ptr != '}' && *ptr != ']') {
            ++ptr;
        }
    }
};

inline std::optional<size_t> parse_size(json_cursor& cur) noexcept {
    cur.skip_ws();
    if (cur.eof()) {
        return std::nullopt;
    }
    if (*cur.ptr == '\"') {
        if (auto sv = cur.string()) {
            size_t value = 0;
            auto fc = std::from_chars(sv->data(), sv->data() + sv->size(), value);
            if (fc.ec == std::errc()) {
                return value;
            }
        }
        return std::nullopt;
    }
    // Let from_chars handle sign and digit scanning directly
    size_t value = 0;
    auto [p, ec] = std::from_chars(cur.ptr, cur.end, value);
    if (ec != std::errc() || p == cur.ptr) {
        return std::nullopt;
    }
    cur.ptr = p;
    return value;
}

inline std::optional<int64_t> parse_int64(json_cursor& cur) noexcept {
    cur.skip_ws();
    if (cur.eof()) {
        return std::nullopt;
    }
    if (*cur.ptr == '\"') {
        if (auto sv = cur.string()) {
            int64_t value = 0;
            auto fc = std::from_chars(sv->data(), sv->data() + sv->size(), value);
            if (fc.ec == std::errc()) {
                return value;
            }
        }
        return std::nullopt;
    }
    // Hand-rolled fast path for common case (short integers ≤18 digits)
    const char* p = cur.ptr;
    bool negative = false;
    if (*p == '-') {
        negative = true;
        ++p;
        if (p >= cur.end)
            return std::nullopt;
    } else if (*p == '+') {
        ++p;
        if (p >= cur.end)
            return std::nullopt;
    }
    // Must start with a digit
    if (static_cast<unsigned char>(*p - '0') > 9u)
        return std::nullopt;
    uint64_t val = 0;
    // Unrolled digit accumulation (handles up to 18 digits safely without overflow)
    do {
        unsigned int d = static_cast<unsigned char>(*p - '0');
        if (d > 9u)
            break;
        val = val * 10u + d;
        ++p;
    } while (p < cur.end);
    cur.ptr = p;
    // Convert to signed with overflow protection
    if (negative) {
        // INT64_MIN = -9223372036854775808, max unsigned = 9223372036854775808
        if (val > static_cast<uint64_t>(INT64_MAX) + 1u)
            return std::nullopt;
        if (val == static_cast<uint64_t>(INT64_MAX) + 1u) {
            return INT64_MIN;
        }
        return -static_cast<int64_t>(val);
    }
    if (val > static_cast<uint64_t>(INT64_MAX))
        return std::nullopt;
    return static_cast<int64_t>(val);
}

inline std::optional<double> parse_double(json_cursor& cur) noexcept {
    cur.skip_ws();
    if (cur.eof()) {
        return std::nullopt;
    }
    if (*cur.ptr == '\"') {
        if (auto sv = cur.string()) {
            double val = 0.0;
            auto [p, ec] = std::from_chars(sv->data(), sv->data() + sv->size(), val);
            if (ec == std::errc()) {
                return val;
            }
        }
        return std::nullopt;
    }
    const char* start = cur.ptr;
    auto fallback_from_chars = [&]() -> std::optional<double> {
        double v = 0.0;
        auto [parsed_end, ec] = std::from_chars(start, cur.end, v);
        if (ec != std::errc() || parsed_end == start) {
            return std::nullopt;
        }
        cur.ptr = parsed_end;
        return v;
    };

    const char* p = start;
    bool negative = false;

    if (*p == '-' || *p == '+') {
        negative = (*p == '-');
        ++p;
        if (p >= cur.end) {
            return std::nullopt;
        }
    }

    uint64_t mantissa = 0;
    int digits = 0;
    int frac_digits = 0;
    bool seen_digit = false;
    bool seen_dot = false;

    while (p < cur.end) {
        const unsigned char ch = static_cast<unsigned char>(*p);
        const unsigned digit = static_cast<unsigned>(ch - '0');
        if (digit <= 9u) {
            seen_digit = true;
            // Keep a small exact mantissa fast path; larger/longer numbers fallback.
            if (digits >= 15) {
                return fallback_from_chars();
            }
            mantissa = mantissa * 10u + digit;
            ++digits;
            if (seen_dot) {
                ++frac_digits;
            }
            ++p;
            continue;
        }
        if (!seen_dot && ch == '.') {
            seen_dot = true;
            ++p;
            continue;
        }
        break;
    }

    if (!seen_digit) {
        return std::nullopt;
    }

    if (seen_dot && frac_digits == 0) {
        return fallback_from_chars();
    }

    int exp10 = 0;
    if (p < cur.end && (*p == 'e' || *p == 'E')) {
        const char* exp_ptr = p + 1;
        bool exp_negative = false;
        if (exp_ptr < cur.end && (*exp_ptr == '+' || *exp_ptr == '-')) {
            exp_negative = (*exp_ptr == '-');
            ++exp_ptr;
        }
        if (exp_ptr >= cur.end ||
            static_cast<unsigned>(static_cast<unsigned char>(*exp_ptr) - '0') > 9u) {
            return fallback_from_chars();
        }

        int exp_value = 0;
        while (exp_ptr < cur.end) {
            const unsigned d = static_cast<unsigned>(static_cast<unsigned char>(*exp_ptr) - '0');
            if (d > 9u) {
                break;
            }
            if (exp_value > 100) {
                return fallback_from_chars();
            }
            exp_value = exp_value * 10 + static_cast<int>(d);
            ++exp_ptr;
        }
        exp10 = exp_negative ? -exp_value : exp_value;
        p = exp_ptr;
    }

    static constexpr double pow10_pos[] = {1.0,
                                           1e1,
                                           1e2,
                                           1e3,
                                           1e4,
                                           1e5,
                                           1e6,
                                           1e7,
                                           1e8,
                                           1e9,
                                           1e10,
                                           1e11,
                                           1e12,
                                           1e13,
                                           1e14,
                                           1e15,
                                           1e16,
                                           1e17,
                                           1e18};
    static constexpr double pow10_neg[] = {1.0,
                                           1e-1,
                                           1e-2,
                                           1e-3,
                                           1e-4,
                                           1e-5,
                                           1e-6,
                                           1e-7,
                                           1e-8,
                                           1e-9,
                                           1e-10,
                                           1e-11,
                                           1e-12,
                                           1e-13,
                                           1e-14,
                                           1e-15,
                                           1e-16,
                                           1e-17,
                                           1e-18};

    double value = static_cast<double>(mantissa);
    if (frac_digits > 0) {
        value *= pow10_neg[frac_digits];
    }
    if (exp10 != 0) {
        if (exp10 >= -18 && exp10 <= 18) {
            value *= (exp10 > 0) ? pow10_pos[exp10] : pow10_neg[-exp10];
        } else {
            return fallback_from_chars();
        }
    }

    if (negative) {
        value = -value;
    }
    cur.ptr = p;
    return value;
}

inline std::optional<bool> parse_bool(json_cursor& cur) noexcept {
    cur.skip_ws();
    if (cur.eof()) {
        return std::nullopt;
    }
    // Use memcmp for word-level comparison (single 32-bit load on most architectures)
    if (cur.end - cur.ptr >= 4 && std::memcmp(cur.ptr, "true", 4) == 0) {
        cur.ptr += 4;
        return true;
    }
    if (cur.end - cur.ptr >= 5 && std::memcmp(cur.ptr, "false", 5) == 0) {
        cur.ptr += 5;
        return false;
    }
    if (*cur.ptr == '\"') {
        if (auto sv = cur.string()) {
            auto v = trim_view(*sv);
            if (v.size() == 4 && std::memcmp(v.data(), "true", 4) == 0) {
                return true;
            }
            if (v.size() == 5 && std::memcmp(v.data(), "false", 5) == 0) {
                return false;
            }
        }
    }
    return std::nullopt;
}

inline std::string_view parse_unquoted_string(json_cursor& cur) {
    cur.skip_ws();
    const char* start = cur.ptr;
    while (!cur.eof() && *cur.ptr != ',' && *cur.ptr != '}' && *cur.ptr != ']') {
        ++cur.ptr;
    }
    const char* end = cur.ptr;
    auto sv = std::string_view(start, static_cast<size_t>(end - start));
    return trim_view(sv);
}

inline bool is_bool_literal(std::string_view sv) noexcept {
    return sv == "true" || sv == "false";
}
inline bool is_null_literal(std::string_view sv) noexcept {
    return sv == "null";
}

inline bool needs_json_escaping(std::string_view sv) noexcept {
#ifdef __AVX2__
    const char* ptr = sv.data();
    const char* end = ptr + sv.size();
    const __m256i backslash = _mm256_set1_epi8('\\');
    const __m256i quote = _mm256_set1_epi8('\"');
    const __m256i control_max = _mm256_set1_epi8(0x1F);

    // Process 32 bytes at a time with AVX2
    while (end - ptr >= 32) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr));
        __m256i eq_bs = _mm256_cmpeq_epi8(chunk, backslash);
        __m256i eq_qt = _mm256_cmpeq_epi8(chunk, quote);
        __m256i is_ctrl = _mm256_cmpeq_epi8(_mm256_min_epu8(chunk, control_max), chunk);
        __m256i needs = _mm256_or_si256(_mm256_or_si256(eq_bs, eq_qt), is_ctrl);
        if (_mm256_movemask_epi8(needs) != 0) {
            return true;
        }
        ptr += 32;
    }

    // SSE2 path for remaining 16-31 bytes
    const __m128i backslash128 = _mm_set1_epi8('\\');
    const __m128i quote128 = _mm_set1_epi8('\"');
    const __m128i control_max128 = _mm_set1_epi8(0x1F);
    while (end - ptr >= 16) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
        __m128i eq_bs = _mm_cmpeq_epi8(chunk, backslash128);
        __m128i eq_qt = _mm_cmpeq_epi8(chunk, quote128);
        __m128i is_ctrl = _mm_cmpeq_epi8(_mm_min_epu8(chunk, control_max128), chunk);
        __m128i needs = _mm_or_si128(_mm_or_si128(eq_bs, eq_qt), is_ctrl);
        if (_mm_movemask_epi8(needs) != 0) {
            return true;
        }
        ptr += 16;
    }
    while (ptr < end) {
        unsigned char c = static_cast<unsigned char>(*ptr);
        if (c == '\\' || c == '\"' || c <= 0x1F) {
            return true;
        }
        ++ptr;
    }
    return false;
#elif defined(__SSE2__)
    const char* ptr = sv.data();
    const char* end = ptr + sv.size();
    const __m128i backslash = _mm_set1_epi8('\\');
    const __m128i quote = _mm_set1_epi8('\"');
    const __m128i control_max = _mm_set1_epi8(0x1F);
    while (end - ptr >= 16) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
        __m128i eq_bs = _mm_cmpeq_epi8(chunk, backslash);
        __m128i eq_qt = _mm_cmpeq_epi8(chunk, quote);
        // Check for control characters (bytes <= 0x1F): compare unsigned via max
        __m128i is_ctrl = _mm_cmpeq_epi8(_mm_min_epu8(chunk, control_max), chunk);
        __m128i needs = _mm_or_si128(_mm_or_si128(eq_bs, eq_qt), is_ctrl);
        if (_mm_movemask_epi8(needs) != 0) {
            return true;
        }
        ptr += 16;
    }
    while (ptr < end) {
        unsigned char c = static_cast<unsigned char>(*ptr);
        if (c == '\\' || c == '\"' || c <= 0x1F) {
            return true;
        }
        ++ptr;
    }
    return false;
#elif defined(__ARM_NEON) || defined(__aarch64__)
    const char* ptr = sv.data();
    const char* end = ptr + sv.size();
    const uint8x16_t backslash = vdupq_n_u8('\\');
    const uint8x16_t quote = vdupq_n_u8('\"');
    const uint8x16_t control_max = vdupq_n_u8(0x1F);
    while (end - ptr >= 16) {
        uint8x16_t chunk = vld1q_u8(reinterpret_cast<const uint8_t*>(ptr));
        uint8x16_t eq_bs = vceqq_u8(chunk, backslash);
        uint8x16_t eq_qt = vceqq_u8(chunk, quote);
        uint8x16_t is_ctrl = vcleq_u8(chunk, control_max);
        uint8x16_t needs = vorrq_u8(vorrq_u8(eq_bs, eq_qt), is_ctrl);
        if (vmaxvq_u8(needs) != 0) {
            return true;
        }
        ptr += 16;
    }
    while (ptr < end) {
        unsigned char c = static_cast<unsigned char>(*ptr);
        if (c == '\\' || c == '\"' || c <= 0x1F) {
            return true;
        }
        ++ptr;
    }
    return false;
#else
    for (char c : sv) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (uc == '\\' || uc == '\"' || uc <= 0x1F) {
            return true;
        }
    }
    return false;
#endif
}

// Emit a single escaped character into out
inline void escape_one_char(char c, std::string& out) {
    switch (c) {
    case '\\':
        out.append("\\\\", 2);
        break;
    case '\"':
        out.append("\\\"", 2);
        break;
    case '\n':
        out.append("\\n", 2);
        break;
    case '\r':
        out.append("\\r", 2);
        break;
    case '\t':
        out.append("\\t", 2);
        break;
    default: {
        static constexpr char hex[] = "0123456789abcdef";
        char buf[6] = {'\\',
                       'u',
                       '0',
                       '0',
                       hex[(static_cast<unsigned char>(c) >> 4) & 0xF],
                       hex[static_cast<unsigned char>(c) & 0xF]};
        out.append(buf, 6);
        break;
    }
    }
}

inline void escape_json_string_into_escaped(std::string_view sv, std::string& out) {
    out.reserve(out.size() + sv.size() + 8);
    const char* ptr = sv.data();
    const char* end_ptr = ptr + sv.size();

#ifdef __AVX2__
    const __m256i v_backslash = _mm256_set1_epi8('\\');
    const __m256i v_quote = _mm256_set1_epi8('\"');
    const __m256i v_control_max = _mm256_set1_epi8(0x1F);

    while (end_ptr - ptr >= 32) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr));
        __m256i eq_bs = _mm256_cmpeq_epi8(chunk, v_backslash);
        __m256i eq_qt = _mm256_cmpeq_epi8(chunk, v_quote);
        __m256i is_ctrl = _mm256_cmpeq_epi8(_mm256_min_epu8(chunk, v_control_max), chunk);
        __m256i needs = _mm256_or_si256(_mm256_or_si256(eq_bs, eq_qt), is_ctrl);
        const uint32_t mask = static_cast<uint32_t>(_mm256_movemask_epi8(needs));

        if (mask == 0) {
            // All 32 bytes are clean — bulk append
            out.append(ptr, 32);
            ptr += 32;
            continue;
        }
        const char* chunk_ptr = ptr;
        uint32_t bits = mask;
        int last = 0;
        while (bits != 0u) {
            const int idx = __builtin_ctz(bits);
            if (idx > last) {
                out.append(chunk_ptr + last, static_cast<size_t>(idx - last));
            }
            escape_one_char(chunk_ptr[idx], out);
            last = idx + 1;
            bits &= (bits - 1);
        }
        if (last < 32) {
            out.append(chunk_ptr + last, static_cast<size_t>(32 - last));
        }
        ptr += 32;
    }
#elif defined(__SSE2__)
    const __m128i v_backslash = _mm_set1_epi8('\\');
    const __m128i v_quote = _mm_set1_epi8('\"');
    const __m128i v_control_max = _mm_set1_epi8(0x1F);

    while (end_ptr - ptr >= 16) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
        __m128i eq_bs = _mm_cmpeq_epi8(chunk, v_backslash);
        __m128i eq_qt = _mm_cmpeq_epi8(chunk, v_quote);
        __m128i is_ctrl = _mm_cmpeq_epi8(_mm_min_epu8(chunk, v_control_max), chunk);
        __m128i needs = _mm_or_si128(_mm_or_si128(eq_bs, eq_qt), is_ctrl);
        const uint32_t mask = static_cast<uint32_t>(_mm_movemask_epi8(needs));

        if (mask == 0) {
            // All 16 bytes are clean — bulk append
            out.append(ptr, 16);
            ptr += 16;
            continue;
        }
        const char* chunk_ptr = ptr;
        uint32_t bits = mask;
        int last = 0;
        while (bits != 0u) {
            const int idx = __builtin_ctz(bits);
            if (idx > last) {
                out.append(chunk_ptr + last, static_cast<size_t>(idx - last));
            }
            escape_one_char(chunk_ptr[idx], out);
            last = idx + 1;
            bits &= (bits - 1);
        }
        if (last < 16) {
            out.append(chunk_ptr + last, static_cast<size_t>(16 - last));
        }
        ptr += 16;
    }
#endif

    // Scalar tail: scan for clean runs and bulk copy
    while (ptr < end_ptr) {
        const char* scan = ptr;
        while (scan < end_ptr) {
            unsigned char uc = static_cast<unsigned char>(*scan);
            if (uc == '\\' || uc == '\"' || uc <= 0x1F)
                break;
            ++scan;
        }
        if (scan > ptr) {
            out.append(ptr, static_cast<size_t>(scan - ptr));
            ptr = scan;
        }
        if (ptr < end_ptr) {
            escape_one_char(*ptr, out);
            ++ptr;
        }
    }
}

inline void escape_json_string_into(std::string_view sv, std::string& out) {
    // Fast path: if no escaping needed, single bulk append
    if (!needs_json_escaping(sv)) {
        out.append(sv.data(), sv.size());
        return;
    }
    escape_json_string_into_escaped(sv, out);
}

inline std::string escape_json_string(std::string_view sv) {
    if (!needs_json_escaping(sv)) {
        return std::string(sv);
    }
    std::string out;
    out.reserve(sv.size() + 8);
    escape_json_string_into_escaped(sv, out);
    return out;
}

// Optimized integer to string conversion using lookup tables
// This is ~2-3x faster than std::to_chars for small integers
namespace detail {

// Two-digit lookup table for faster integer formatting
// Each entry contains the ASCII representation of 00-99
alignas(64) constexpr char digits_lut[200] = {
    '0', '0', '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '6', '0', '7', '0', '8', '0',
    '9', '1', '0', '1', '1', '1', '2', '1', '3', '1', '4', '1', '5', '1', '6', '1', '7', '1', '8',
    '1', '9', '2', '0', '2', '1', '2', '2', '2', '3', '2', '4', '2', '5', '2', '6', '2', '7', '2',
    '8', '2', '9', '3', '0', '3', '1', '3', '2', '3', '3', '3', '4', '3', '5', '3', '6', '3', '7',
    '3', '8', '3', '9', '4', '0', '4', '1', '4', '2', '4', '3', '4', '4', '4', '5', '4', '6', '4',
    '7', '4', '8', '4', '9', '5', '0', '5', '1', '5', '2', '5', '3', '5', '4', '5', '5', '5', '6',
    '5', '7', '5', '8', '5', '9', '6', '0', '6', '1', '6', '2', '6', '3', '6', '4', '6', '5', '6',
    '6', '6', '7', '6', '8', '6', '9', '7', '0', '7', '1', '7', '2', '7', '3', '7', '4', '7', '5',
    '7', '6', '7', '7', '7', '8', '7', '9', '8', '0', '8', '1', '8', '2', '8', '3', '8', '4', '8',
    '5', '8', '6', '8', '7', '8', '8', '8', '9', '9', '0', '9', '1', '9', '2', '9', '3', '9', '4',
    '9', '5', '9', '6', '9', '7', '9', '8', '9', '9'};

// Fast integer to string using two-digit lookup
inline char* format_uint_fast(char* buf, uint32_t val) noexcept {
    if (val < 10) {
        *buf++ = static_cast<char>('0' + val);
        return buf;
    }
    if (val < 100) {
        std::memcpy(buf, &digits_lut[val * 2], 2);
        return buf + 2;
    }
    if (val < 1000) {
        *buf++ = static_cast<char>('0' + val / 100);
        std::memcpy(buf, &digits_lut[(val % 100) * 2], 2);
        return buf + 2;
    }
    if (val < 10000) {
        std::memcpy(buf, &digits_lut[(val / 100) * 2], 2);
        std::memcpy(buf + 2, &digits_lut[(val % 100) * 2], 2);
        return buf + 4;
    }
    // Fall back to from_chars for larger values
    auto [ptr, ec] = std::to_chars(buf, buf + 16, val);
    return ptr;
}

} // namespace detail

// Serialize array of integers with optimized formatting
// Pre-allocates buffer and uses lookup table for fast conversion
template <typename IntT>
inline void serialize_int_array_into(const IntT* arr, size_t count, std::string& out) {
    if (count == 0) {
        out.append("[]", 2);
        return;
    }

    // Reserve space: assume avg 4 chars per int + comma + brackets
    out.reserve(out.size() + count * 5 + 2);

    // Use a local buffer for batch formatting
    char local_buf[512];
    char* ptr = local_buf;
    char* const end = local_buf + sizeof(local_buf) - 16; // Leave room for last int

    out.push_back('[');

    for (size_t i = 0; i < count; ++i) {
        if (ptr >= end) {
            // Flush buffer
            out.append(local_buf, static_cast<size_t>(ptr - local_buf));
            ptr = local_buf;
        }

        if (i > 0) {
            *ptr++ = ',';
        }

        // Format integer
        auto val = arr[i];
        using unsigned_t = std::make_unsigned_t<IntT>;
        unsigned_t uval = 0;
        if constexpr (std::is_signed_v<IntT>) {
            if (val < 0) {
                *ptr++ = '-';
                // Convert through unsigned domain to avoid UB on INT_MIN.
                uval = unsigned_t{0} - static_cast<unsigned_t>(val);
            } else {
                uval = static_cast<unsigned_t>(val);
            }
        } else {
            uval = static_cast<unsigned_t>(val);
        }

        if constexpr (sizeof(unsigned_t) <= 4) {
            ptr = detail::format_uint_fast(ptr, static_cast<uint32_t>(uval));
        } else {
            auto [p, ec] = std::to_chars(ptr, ptr + 20, uval);
            ptr = p;
        }
    }

    // Flush remaining
    out.append(local_buf, static_cast<size_t>(ptr - local_buf));
    out.push_back(']');
}

// Convenience wrapper that returns a string
template <typename IntT> inline std::string serialize_int_array(const IntT* arr, size_t count) {
    std::string result;
    serialize_int_array_into(arr, count, result);
    return result;
}

} // namespace katana::serde
