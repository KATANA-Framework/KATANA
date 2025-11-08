#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <optional>
#include <algorithm>
#include <cctype>

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#define KATANA_HAS_SSE2 1
#if defined(__AVX2__)
#define KATANA_HAS_AVX2 1
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

class headers_map {
public:
    headers_map() = default;
    headers_map(headers_map&&) noexcept = default;
    headers_map& operator=(headers_map&&) noexcept = default;
    headers_map(const headers_map&) = default;
    headers_map& operator=(const headers_map&) = default;

    void set(std::string name, std::string value) {
        // Store with lowercase key for O(1) case-insensitive lookup
        std::string lower_key = to_lower(name);
        original_names_[lower_key] = std::move(name);
        headers_[std::move(lower_key)] = std::move(value);
    }

    [[nodiscard]] std::optional<std::string_view> get(std::string_view name) const noexcept {
        std::string lower_key = to_lower(name);
        auto it = headers_.find(lower_key);
        if (it != headers_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    [[nodiscard]] bool contains(std::string_view name) const noexcept {
        std::string lower_key = to_lower(name);
        return headers_.find(lower_key) != headers_.end();
    }

    void remove(std::string_view name) {
        std::string lower_key = to_lower(name);
        headers_.erase(lower_key);
        original_names_.erase(lower_key);
    }

    void clear() noexcept {
        headers_.clear();
        original_names_.clear();
    }

    struct iterator {
        using inner_iterator = std::unordered_map<std::string, std::string>::iterator;
        inner_iterator it;
        std::unordered_map<std::string, std::string>* original_names;

        iterator& operator++() { ++it; return *this; }
        bool operator!=(const iterator& o) const { return it != o.it; }
        std::pair<const std::string&, std::string&> operator*() {
            auto orig_it = original_names->find(it->first);
            return {orig_it->second, it->second};
        }
    };

    struct const_iterator {
        using inner_iterator = std::unordered_map<std::string, std::string>::const_iterator;
        inner_iterator it;
        const std::unordered_map<std::string, std::string>* original_names;

        const_iterator& operator++() { ++it; return *this; }
        bool operator!=(const const_iterator& o) const { return it != o.it; }
        std::pair<const std::string&, const std::string&> operator*() const {
            auto orig_it = original_names->find(it->first);
            return {orig_it->second, it->second};
        }
    };

    iterator begin() noexcept { return {headers_.begin(), &original_names_}; }
    iterator end() noexcept { return {headers_.end(), &original_names_}; }
    const_iterator begin() const noexcept { return {headers_.begin(), &original_names_}; }
    const_iterator end() const noexcept { return {headers_.end(), &original_names_}; }

    [[nodiscard]] size_t size() const noexcept { return headers_.size(); }
    [[nodiscard]] bool empty() const noexcept { return headers_.empty(); }

private:
    std::unordered_map<std::string, std::string> headers_;
    std::unordered_map<std::string, std::string> original_names_;
};

} // namespace katana::http
