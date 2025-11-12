#pragma once

#include "http_field.hpp"
#include "arena.hpp"

#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <optional>
#include <algorithm>
#include <cctype>
#include <cstring>

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#ifndef KATANA_HAS_SSE2
#define KATANA_HAS_SSE2 1
#endif
#if defined(__AVX2__)
#ifndef KATANA_HAS_AVX2
#define KATANA_HAS_AVX2 1
#endif
#endif
#endif

namespace katana::http {

inline bool ci_char_equal(char a, char b) noexcept {
    return std::tolower(static_cast<unsigned char>(a)) ==
           std::tolower(static_cast<unsigned char>(b));
}

#ifdef KATANA_HAS_AVX2
inline bool ci_equal_simd_avx2(std::string_view a, std::string_view b) noexcept {
    if (a.size() != b.size()) {
        return false;
    }

    size_t i = 0;
    constexpr size_t vec_size = 32;

    for (; i + vec_size <= a.size(); i += vec_size) {
        __m256i va = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(a.data() + i));
        __m256i vb = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(b.data() + i));

        __m256i lower_a = _mm256_or_si256(va, _mm256_set1_epi8(0x20));
        __m256i lower_b = _mm256_or_si256(vb, _mm256_set1_epi8(0x20));

        __m256i cmp = _mm256_cmpeq_epi8(lower_a, lower_b);
        if (_mm256_movemask_epi8(cmp) != static_cast<int>(0xFFFFFFFF)) {
            return false;
        }
    }

    for (; i < a.size(); ++i) {
        if (!ci_char_equal(a[i], b[i])) {
            return false;
        }
    }

    return true;
}
#endif

#ifdef KATANA_HAS_SSE2
inline bool ci_equal_simd_sse2(std::string_view a, std::string_view b) noexcept {
    if (a.size() != b.size()) {
        return false;
    }

    size_t i = 0;
    const size_t vec_size = 16;

    for (; i + vec_size <= a.size(); i += vec_size) {
        __m128i va = _mm_loadu_si128(reinterpret_cast<const __m128i*>(a.data() + i));
        __m128i vb = _mm_loadu_si128(reinterpret_cast<const __m128i*>(b.data() + i));

        __m128i lower_a = _mm_or_si128(va, _mm_set1_epi8(0x20));
        __m128i lower_b = _mm_or_si128(vb, _mm_set1_epi8(0x20));

        __m128i cmp = _mm_cmpeq_epi8(lower_a, lower_b);
        if (_mm_movemask_epi8(cmp) != 0xFFFF) {
            return false;
        }
    }

    for (; i < a.size(); ++i) {
        if (!ci_char_equal(a[i], b[i])) {
            return false;
        }
    }

    return true;
}
#endif

inline bool ci_equal(std::string_view a, std::string_view b) noexcept {
#ifdef KATANA_HAS_AVX2
    if (a.size() >= 32) {
        return ci_equal_simd_avx2(a, b);
    }
#endif
#ifdef KATANA_HAS_SSE2
    if (a.size() >= 16) {
        return ci_equal_simd_sse2(a, b);
    }
#endif
    return a.size() == b.size() &&
           std::equal(a.begin(), a.end(), b.begin(), ci_char_equal);
}

inline std::string to_lower(std::string_view s) {
    std::string result;
    result.reserve(s.size());
    for (char c : s) {
        result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return result;
}

// Case-insensitive hash functor for heterogeneous lookup (zero-allocation)
struct ci_hash {
    using is_transparent = void;  // Enable heterogeneous lookup

    [[nodiscard]] size_t operator()(std::string_view sv) const noexcept {
        // FNV-1a hash algorithm with case folding
        size_t hash = 14695981039346656037ULL;
        for (char c : sv) {
            hash ^= static_cast<size_t>(std::tolower(static_cast<unsigned char>(c)));
            hash *= 1099511628211ULL;
        }
        return hash;
    }

    [[nodiscard]] size_t operator()(const std::string& s) const noexcept {
        return (*this)(std::string_view{s});
    }
};

// Case-insensitive equality functor for heterogeneous lookup (zero-allocation)
struct ci_equal_fn {
    using is_transparent = void;  // Enable heterogeneous lookup

    [[nodiscard]] bool operator()(std::string_view a, std::string_view b) const noexcept {
        return http::ci_equal(a, b);
    }
};

class headers_map {
private:
    struct entry {
        field field_id;
        const char* value;
        size_t length;
    };

public:
    explicit headers_map(monotonic_arena* arena = nullptr) noexcept
        : arena_(arena) {
        entries_.reserve(16);
    }

    headers_map(headers_map&&) noexcept = default;
    headers_map& operator=(headers_map&&) noexcept = default;

    headers_map(const headers_map&) = delete;
    headers_map& operator=(const headers_map&) = delete;

    void set(field f, std::string_view value) noexcept {
        if (f == field::unknown || f >= field::MAX_FIELD_VALUE) {
            return;
        }

        auto it = std::lower_bound(entries_.begin(), entries_.end(), f,
            [](const entry& e, field fld) { return e.field_id < fld; });

        if (it != entries_.end() && it->field_id == f) {
            if (arena_) {
                it->value = arena_->allocate_string(value);
                it->length = value.size();
            }
        } else {
            if (arena_) {
                entries_.insert(it, entry{f, arena_->allocate_string(value), value.size()});
            }
        }
    }

    void set_view(std::string_view name, std::string_view value) noexcept {
        field f = string_to_field(name);
        set(f, value);
    }

    [[nodiscard]] std::optional<std::string_view> get(field f) const noexcept {
        if (f == field::unknown || f >= field::MAX_FIELD_VALUE) {
            return std::nullopt;
        }

        auto it = std::lower_bound(entries_.begin(), entries_.end(), f,
            [](const entry& e, field fld) { return e.field_id < fld; });

        if (it != entries_.end() && it->field_id == f) {
            return std::string_view(it->value, it->length);
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<std::string_view> get(std::string_view name) const noexcept {
        return get(string_to_field(name));
    }

    [[nodiscard]] bool contains(field f) const noexcept {
        if (f == field::unknown || f >= field::MAX_FIELD_VALUE) {
            return false;
        }

        auto it = std::lower_bound(entries_.begin(), entries_.end(), f,
            [](const entry& e, field fld) { return e.field_id < fld; });

        return it != entries_.end() && it->field_id == f;
    }

    [[nodiscard]] bool contains(std::string_view name) const noexcept {
        return contains(string_to_field(name));
    }

    void remove(field f) noexcept {
        if (f == field::unknown || f >= field::MAX_FIELD_VALUE) {
            return;
        }

        auto it = std::lower_bound(entries_.begin(), entries_.end(), f,
            [](const entry& e, field fld) { return e.field_id < fld; });

        if (it != entries_.end() && it->field_id == f) {
            entries_.erase(it);
        }
    }

    void remove(std::string_view name) noexcept {
        remove(string_to_field(name));
    }

    void clear() noexcept {
        entries_.clear();
    }

    struct iterator {
        using inner_iterator = std::vector<entry>::const_iterator;
        inner_iterator iter;

        iterator& operator++() { ++iter; return *this; }
        bool operator!=(const iterator& other) const { return iter != other.iter; }

        std::pair<std::string_view, std::string_view> operator*() const {
            return {field_to_string(iter->field_id), std::string_view(iter->value, iter->length)};
        }
    };

    iterator begin() const noexcept { return {entries_.begin()}; }
    iterator end() const noexcept { return {entries_.end()}; }

    [[nodiscard]] size_t size() const noexcept { return entries_.size(); }
    [[nodiscard]] bool empty() const noexcept { return entries_.empty(); }

private:
    monotonic_arena* arena_;
    std::vector<entry> entries_;
};

} // namespace katana::http
