#pragma once

#include "arena.hpp"
#include "http_field.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

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

inline bool ci_equal_short(std::string_view a, std::string_view b) noexcept {
    if (a.size() != b.size()) {
        return false;
    }

    if (a.size() <= 8) {
        uint64_t va = 0;
        uint64_t vb = 0;
        std::memcpy(&va, a.data(), a.size());
        std::memcpy(&vb, b.data(), b.size());
        constexpr uint64_t lower_mask = 0x2020202020202020ULL;
        va |= lower_mask;
        vb |= lower_mask;
        return va == vb;
    }

    if (a.size() <= 15) {
#ifdef KATANA_HAS_SSE2
        __m128i va = _mm_setzero_si128();
        __m128i vb = _mm_setzero_si128();
        std::memcpy(&va, a.data(), a.size());
        std::memcpy(&vb, b.data(), b.size());

        __m128i lower = _mm_set1_epi8(0x20);
        va = _mm_or_si128(va, lower);
        vb = _mm_or_si128(vb, lower);

        __m128i cmp = _mm_cmpeq_epi8(va, vb);
        return _mm_movemask_epi8(cmp) == 0xFFFF;
#else
        return std::equal(a.begin(), a.end(), b.begin(), ci_char_equal);
#endif
    }

    return false;
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
    if (a.size() != b.size()) {
        return false;
    }

    if (a.size() < 16) {
        return ci_equal_short(a, b);
    }
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
    return std::equal(a.begin(), a.end(), b.begin(), ci_char_equal);
}

inline bool ci_equal_fast(std::string_view a, std::string_view b) noexcept {
    return ci_equal(a, b);
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
    using is_transparent = void; // Enable heterogeneous lookup

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
    using is_transparent = void; // Enable heterogeneous lookup

    [[nodiscard]] bool operator()(std::string_view a, std::string_view b) const noexcept {
        return http::ci_equal(a, b);
    }
};

class headers_map {
private:
    struct known_entry {
        const char* value = nullptr;
        size_t length = 0;
    };

    struct unknown_entry {
        const char* name;
        size_t name_length;
        const char* value;
        size_t value_length;
    };

    static constexpr size_t UNKNOWN_HEADERS_INLINE_SIZE = 8;
    static constexpr size_t KNOWN_HEADERS_COUNT = static_cast<size_t>(field::MAX_FIELD_VALUE);

    struct known_headers_storage {
        std::array<known_entry, KNOWN_HEADERS_COUNT> entries{};
    };

public:
    explicit headers_map(monotonic_arena* arena = nullptr) noexcept
        : arena_(arena), fallback_arena_(arena ? nullptr : &owned_arena_),
          known_entries_(std::make_unique<known_headers_storage>()) {}

    headers_map(headers_map&& other) noexcept
        : arena_(other.arena_ == &other.owned_arena_ ? nullptr : other.arena_),
          fallback_arena_(nullptr), owned_arena_(std::move(other.owned_arena_)),
          known_entries_(std::move(other.known_entries_)), known_size_(other.known_size_),
          unknown_entries_(std::move(other.unknown_entries_)),
          known_touched_indices_(other.known_touched_indices_),
          known_touched_flags_(other.known_touched_flags_),
          known_touched_count_(other.known_touched_count_) {
        fallback_arena_ = arena_ ? nullptr : &owned_arena_;
        other.arena_ = nullptr;
        other.fallback_arena_ = &other.owned_arena_;
        other.known_size_ = 0;
        other.known_touched_count_ = 0;
    }

    headers_map& operator=(headers_map&& other) noexcept {
        if (this != &other) {
            arena_ = other.arena_ == &other.owned_arena_ ? nullptr : other.arena_;
            owned_arena_ = std::move(other.owned_arena_);
            known_entries_ = std::move(other.known_entries_);
            known_size_ = other.known_size_;
            unknown_entries_ = std::move(other.unknown_entries_);
            known_touched_indices_ = other.known_touched_indices_;
            known_touched_flags_ = other.known_touched_flags_;
            known_touched_count_ = other.known_touched_count_;
            fallback_arena_ = arena_ ? nullptr : &owned_arena_;

            other.arena_ = nullptr;
            other.fallback_arena_ = &other.owned_arena_;
            other.known_size_ = 0;
            other.known_touched_count_ = 0;
        }
        return *this;
    }

    headers_map(const headers_map&) = delete;
    headers_map& operator=(const headers_map&) = delete;

    void set(field f, std::string_view value) noexcept { set_known(f, value); }

    void set_known(field f, std::string_view value) noexcept {
        auto idx = static_cast<size_t>(f);
        if (idx == static_cast<size_t>(field::unknown) || idx >= KNOWN_HEADERS_COUNT) {
            return;
        }

        monotonic_arena* alloc = arena_ ? arena_ : fallback_arena_;
        const char* value_ptr = alloc ? alloc->allocate_string(value) : nullptr;
        if (!value_ptr)
            return;

        auto& entry = known_entries_->entries[idx];
        if (!entry.value) {
            if (!known_touched_flags_[idx]) {
                known_touched_flags_[idx] = 1;
                known_touched_indices_[known_touched_count_++] = static_cast<uint16_t>(idx);
            }
            ++known_size_;
        }
        entry.value = value_ptr;
        entry.length = static_cast<uint16_t>(value.size());
    }

    void set_unknown(std::string_view name, std::string_view value) noexcept {
        monotonic_arena* alloc = arena_ ? arena_ : fallback_arena_;
        if (!alloc)
            return;

        const char* name_ptr = alloc->allocate_string(name);
        const char* value_ptr = alloc->allocate_string(value);
        if (!name_ptr || !value_ptr)
            return;

        for (auto& ue : unknown_entries_) {
            if (ue.name_length == name.size() &&
                ci_equal_fast(std::string_view(ue.name, ue.name_length), name)) {
                ue.value = value_ptr;
                ue.value_length = value.size();
                return;
            }
        }

        unknown_entries_.push_back(unknown_entry{name_ptr, name.size(), value_ptr, value.size()});
    }

    void set_view(std::string_view name, std::string_view value) noexcept {
        field f = string_to_field(name);
        if (f == field::unknown) {
            set_unknown(name, value);
        } else {
            set_known(f, value);
        }
    }

    [[nodiscard]] std::optional<std::string_view> get(field f) const noexcept {
        if (f == field::unknown || f >= field::MAX_FIELD_VALUE) {
            return std::nullopt;
        }

        const auto& entry = known_entries_->entries[static_cast<size_t>(f)];
        if (!entry.value) {
            return std::nullopt;
        }
        return std::string_view(entry.value, entry.length);
    }

    [[nodiscard]] std::optional<std::string_view> get(std::string_view name) const noexcept {
        field f = string_to_field(name);

        if (f == field::unknown) {
            // Search in unknown headers
            for (const auto& ue : unknown_entries_) {
                if (ue.name_length == name.size() &&
                    ci_equal(std::string_view(ue.name, ue.name_length), name)) {
                    return std::string_view(ue.value, ue.value_length);
                }
            }
            return std::nullopt;
        }

        return get(f);
    }

    [[nodiscard]] bool contains(field f) const noexcept {
        auto idx = static_cast<size_t>(f);
        if (idx == static_cast<size_t>(field::unknown) || idx >= KNOWN_HEADERS_COUNT) {
            return false;
        }

        return known_entries_->entries[idx].value != nullptr;
    }

    [[nodiscard]] bool contains(std::string_view name) const noexcept {
        field f = string_to_field(name);

        if (f == field::unknown) {
            // Search in unknown headers
            for (const auto& ue : unknown_entries_) {
                if (ue.name_length == name.size() &&
                    ci_equal(std::string_view(ue.name, ue.name_length), name)) {
                    return true;
                }
            }
            return false;
        }

        return contains(f);
    }

    void remove(field f) noexcept {
        auto idx = static_cast<size_t>(f);
        if (idx == static_cast<size_t>(field::unknown) || idx >= KNOWN_HEADERS_COUNT) {
            return;
        }

        auto& entry = known_entries_->entries[idx];
        if (entry.value) {
            entry = {};
            if (known_size_ > 0) {
                --known_size_;
            }
        }
    }

    void remove(std::string_view name) noexcept {
        field f = string_to_field(name);

        if (f == field::unknown) {
            // Remove from unknown headers
            auto it = unknown_entries_.begin();
            while (it != unknown_entries_.end()) {
                if (it->name_length == name.size() &&
                    ci_equal(std::string_view(it->name, it->name_length), name)) {
                    unknown_entries_.erase(it);
                    return;
                }
                ++it;
            }
        } else {
            remove(f);
        }
    }

    void clear() noexcept {
        for (size_t i = 0; i < known_touched_count_; ++i) {
            auto idx = static_cast<size_t>(known_touched_indices_[i]);
            known_entries_->entries[idx] = {};
            known_touched_flags_[idx] = 0;
        }
        known_touched_count_ = 0;
        known_size_ = 0;
        unknown_entries_.clear();
    }

    struct iterator {
        const headers_map* map;
        size_t index;
        bool in_known;

        void advance_known() noexcept {
            while (in_known && index < KNOWN_HEADERS_COUNT &&
                   !map->known_entries_->entries[index].value) {
                ++index;
            }

            if (in_known && index >= KNOWN_HEADERS_COUNT) {
                in_known = false;
                index = 0;
            }
        }

        iterator& operator++() {
            if (in_known) {
                ++index;
                advance_known();
            } else {
                ++index;
            }
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return in_known != other.in_known || index != other.index;
        }

        std::pair<std::string_view, std::string_view> operator*() const {
            if (in_known) {
                const auto& e = map->known_entries_->entries[index];
                return {field_to_string(static_cast<field>(index)),
                        std::string_view(e.value, e.length)};
            }
            const auto& ue = map->unknown_entries_[index];
            return {std::string_view(ue.name, ue.name_length),
                    std::string_view(ue.value, ue.value_length)};
        }
    };

    iterator begin() const noexcept {
        iterator it{this, 0, true};
        it.advance_known();

        if (!it.in_known && unknown_entries_.empty()) {
            return end();
        }
        return it;
    }

    iterator end() const noexcept { return {this, unknown_entries_.size(), false}; }

    [[nodiscard]] size_t size() const noexcept { return known_size_ + unknown_entries_.size(); }
    [[nodiscard]] bool empty() const noexcept {
        return known_size_ == 0 && unknown_entries_.empty();
    }

    void reset(monotonic_arena* arena) noexcept {
        arena_ = arena;
        fallback_arena_ = arena_ ? nullptr : &owned_arena_;
        for (size_t i = 0; i < known_touched_count_; ++i) {
            auto idx = static_cast<size_t>(known_touched_indices_[i]);
            known_entries_->entries[idx] = {};
            known_touched_flags_[idx] = 0;
        }
        known_touched_count_ = 0;
        known_size_ = 0;
        unknown_entries_.clear();
    }

private:
    monotonic_arena* arena_;
    monotonic_arena* fallback_arena_;
    monotonic_arena owned_arena_{4096};
    std::unique_ptr<known_headers_storage> known_entries_;
    size_t known_size_ = 0;
    std::vector<unknown_entry> unknown_entries_;
    std::array<uint16_t, KNOWN_HEADERS_COUNT> known_touched_indices_{};
    std::array<uint8_t, KNOWN_HEADERS_COUNT> known_touched_flags_{};
    size_t known_touched_count_ = 0;
};

} // namespace katana::http
