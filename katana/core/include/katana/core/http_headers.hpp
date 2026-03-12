#pragma once

#include "arena.hpp"
#include "http_field.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <new>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

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
        field key = field::unknown;
        const char* value = nullptr;
        uint16_t length = 0;
    };

    struct unknown_entry {
        const char* name = nullptr;
        uint16_t name_length = 0;
        const char* value = nullptr;
        uint16_t value_length = 0;
    };

    template <typename Entry, size_t Capacity> struct entry_chunk {
        std::array<Entry, Capacity> entries{};
        entry_chunk* next = nullptr;
    };

    static constexpr size_t KNOWN_HEADERS_INLINE_SIZE = 16;
    static constexpr size_t KNOWN_HEADERS_CHUNK_SIZE = 16;
    static constexpr size_t UNKNOWN_HEADERS_INLINE_SIZE = 8;
    static constexpr size_t UNKNOWN_HEADERS_CHUNK_SIZE = 8;

public:
    explicit headers_map(monotonic_arena* arena = nullptr) noexcept
        : arena_(arena), fallback_arena_(arena ? nullptr : &owned_arena_) {}

    headers_map(headers_map&& other) noexcept
        : arena_(other.arena_ == &other.owned_arena_ ? nullptr : other.arena_),
          fallback_arena_(nullptr), owned_arena_(std::move(other.owned_arena_)),
          known_inline_(other.known_inline_), known_chunks_(other.known_chunks_),
          known_size_(other.known_size_), unknown_inline_(other.unknown_inline_),
          unknown_chunks_(other.unknown_chunks_), unknown_size_(other.unknown_size_) {
        fallback_arena_ = arena_ ? nullptr : &owned_arena_;
        other.reset_storage();
    }

    headers_map& operator=(headers_map&& other) noexcept {
        if (this != &other) {
            arena_ = other.arena_ == &other.owned_arena_ ? nullptr : other.arena_;
            owned_arena_ = std::move(other.owned_arena_);
            known_inline_ = other.known_inline_;
            known_chunks_ = other.known_chunks_;
            known_size_ = other.known_size_;
            unknown_inline_ = other.unknown_inline_;
            unknown_chunks_ = other.unknown_chunks_;
            unknown_size_ = other.unknown_size_;
            fallback_arena_ = arena_ ? nullptr : &owned_arena_;
            other.reset_storage();
        }
        return *this;
    }

    headers_map(const headers_map&) = delete;
    headers_map& operator=(const headers_map&) = delete;

    void set(field f, std::string_view value) noexcept { set_known(f, value); }

    void set_known(field f, std::string_view value) noexcept {
        if (f == field::unknown || f >= field::MAX_FIELD_VALUE) {
            return;
        }

        monotonic_arena* alloc = allocator();
        const char* value_ptr = alloc ? alloc->allocate_string(value) : nullptr;
        if (!value_ptr) {
            return;
        }

        known_entry* entry = find_known_entry(f);
        if (!entry) {
            entry = append_known_entry();
            if (!entry) {
                return;
            }
            ++known_size_;
        }

        entry->key = f;
        entry->value = value_ptr;
        entry->length = static_cast<uint16_t>(value.size());
    }

    void set_known_borrowed(field f, std::string_view value) noexcept {
        if (f == field::unknown || f >= field::MAX_FIELD_VALUE) {
            return;
        }

        known_entry* entry = find_known_entry(f);
        if (!entry) {
            entry = append_known_entry();
            if (!entry) {
                return;
            }
            ++known_size_;
        }

        entry->key = f;
        entry->value = value.data();
        entry->length = static_cast<uint16_t>(value.size());
    }

    void set_unknown(std::string_view name, std::string_view value) noexcept {
        monotonic_arena* alloc = allocator();
        if (!alloc) {
            return;
        }

        const char* name_ptr = alloc->allocate_string(name);
        const char* value_ptr = alloc->allocate_string(value);
        if (!name_ptr || !value_ptr) {
            return;
        }

        if (unknown_entry* entry = find_unknown_entry(name)) {
            entry->value = value_ptr;
            entry->value_length = static_cast<uint16_t>(value.size());
            return;
        }

        unknown_entry* entry = append_unknown_entry();
        if (!entry) {
            return;
        }

        entry->name = name_ptr;
        entry->name_length = static_cast<uint16_t>(name.size());
        entry->value = value_ptr;
        entry->value_length = static_cast<uint16_t>(value.size());
        ++unknown_size_;
    }

    void set_unknown_borrowed(std::string_view name, std::string_view value) noexcept {
        if (unknown_entry* entry = find_unknown_entry(name)) {
            entry->name = name.data();
            entry->name_length = static_cast<uint16_t>(name.size());
            entry->value = value.data();
            entry->value_length = static_cast<uint16_t>(value.size());
            return;
        }

        unknown_entry* entry = append_unknown_entry();
        if (!entry) {
            return;
        }

        entry->name = name.data();
        entry->name_length = static_cast<uint16_t>(name.size());
        entry->value = value.data();
        entry->value_length = static_cast<uint16_t>(value.size());
        ++unknown_size_;
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

        const known_entry* entry = find_known_entry(f);
        if (!entry) {
            return std::nullopt;
        }
        return std::string_view(entry->value, entry->length);
    }

    [[nodiscard]] std::optional<std::string_view> get(std::string_view name) const noexcept {
        field f = string_to_field(name);

        if (f == field::unknown) {
            if (const unknown_entry* entry = find_unknown_entry(name)) {
                return std::string_view(entry->value, entry->value_length);
            }
            return std::nullopt;
        }

        return get(f);
    }

    [[nodiscard]] bool contains(field f) const noexcept { return find_known_entry(f) != nullptr; }

    [[nodiscard]] bool contains(std::string_view name) const noexcept {
        field f = string_to_field(name);
        if (f == field::unknown) {
            return find_unknown_entry(name) != nullptr;
        }
        return contains(f);
    }

    void remove(field f) noexcept {
        if (known_entry* entry = find_known_entry(f)) {
            *entry = {};
            if (known_size_ > 0) {
                --known_size_;
            }
        }
    }

    void remove(std::string_view name) noexcept {
        field f = string_to_field(name);
        if (f == field::unknown) {
            if (unknown_entry* entry = find_unknown_entry(name)) {
                *entry = {};
                if (unknown_size_ > 0) {
                    --unknown_size_;
                }
            }
            return;
        }

        remove(f);
    }

    void clear() noexcept {
        known_inline_.fill({});
        for (auto* chunk = known_chunks_; chunk; chunk = chunk->next) {
            chunk->entries.fill({});
        }
        known_size_ = 0;

        unknown_inline_.fill({});
        for (auto* chunk = unknown_chunks_; chunk; chunk = chunk->next) {
            chunk->entries.fill({});
        }
        unknown_size_ = 0;
    }

    struct iterator {
        const headers_map* map = nullptr;
        size_t index = 0;
        uint8_t phase = 0;
        const entry_chunk<known_entry, KNOWN_HEADERS_CHUNK_SIZE>* known_chunk = nullptr;
        const entry_chunk<unknown_entry, UNKNOWN_HEADERS_CHUNK_SIZE>* unknown_chunk = nullptr;

        void advance() noexcept {
            for (;;) {
                if (phase == 0) {
                    while (index < KNOWN_HEADERS_INLINE_SIZE) {
                        if (map->known_inline_[index].key != field::unknown) {
                            return;
                        }
                        ++index;
                    }
                    phase = 1;
                    index = 0;
                    known_chunk = map->known_chunks_;
                    continue;
                }

                if (phase == 1) {
                    while (known_chunk) {
                        while (index < KNOWN_HEADERS_CHUNK_SIZE) {
                            if (known_chunk->entries[index].key != field::unknown) {
                                return;
                            }
                            ++index;
                        }
                        known_chunk = known_chunk->next;
                        index = 0;
                    }
                    phase = 2;
                    index = 0;
                    continue;
                }

                if (phase == 2) {
                    while (index < UNKNOWN_HEADERS_INLINE_SIZE) {
                        if (map->unknown_inline_[index].name) {
                            return;
                        }
                        ++index;
                    }
                    phase = 3;
                    index = 0;
                    unknown_chunk = map->unknown_chunks_;
                    continue;
                }

                while (unknown_chunk) {
                    while (index < UNKNOWN_HEADERS_CHUNK_SIZE) {
                        if (unknown_chunk->entries[index].name) {
                            return;
                        }
                        ++index;
                    }
                    unknown_chunk = unknown_chunk->next;
                    index = 0;
                }

                phase = 4;
                return;
            }
        }

        iterator& operator++() {
            ++index;
            advance();
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return phase != other.phase || index != other.index ||
                   known_chunk != other.known_chunk || unknown_chunk != other.unknown_chunk;
        }

        std::pair<std::string_view, std::string_view> operator*() const {
            if (phase == 0) {
                const auto& entry = map->known_inline_[index];
                return {field_to_string(entry.key), std::string_view(entry.value, entry.length)};
            }
            if (phase == 1) {
                const auto& entry = known_chunk->entries[index];
                return {field_to_string(entry.key), std::string_view(entry.value, entry.length)};
            }

            const auto& entry =
                (phase == 2) ? map->unknown_inline_[index] : unknown_chunk->entries[index];
            return {std::string_view(entry.name, entry.name_length),
                    std::string_view(entry.value, entry.value_length)};
        }
    };

    iterator begin() const noexcept {
        iterator it;
        it.map = this;
        it.advance();
        return it;
    }

    iterator end() const noexcept {
        iterator it;
        it.map = this;
        it.phase = 4;
        return it;
    }

    [[nodiscard]] size_t size() const noexcept { return known_size_ + unknown_size_; }
    [[nodiscard]] bool empty() const noexcept { return known_size_ == 0 && unknown_size_ == 0; }

    void reset(monotonic_arena* arena) noexcept {
        arena_ = arena;
        fallback_arena_ = arena_ ? nullptr : &owned_arena_;
        reset_storage();
    }

private:
    [[nodiscard]] monotonic_arena* allocator() noexcept {
        return arena_ ? arena_ : fallback_arena_;
    }

    [[nodiscard]] known_entry* find_known_entry(field f) noexcept {
        for (auto& entry : known_inline_) {
            if (entry.key == f) {
                return &entry;
            }
        }
        for (auto* chunk = known_chunks_; chunk; chunk = chunk->next) {
            for (auto& entry : chunk->entries) {
                if (entry.key == f) {
                    return &entry;
                }
            }
        }
        return nullptr;
    }

    [[nodiscard]] const known_entry* find_known_entry(field f) const noexcept {
        return const_cast<headers_map*>(this)->find_known_entry(f);
    }

    [[nodiscard]] known_entry* append_known_entry() noexcept {
        for (auto& entry : known_inline_) {
            if (entry.key == field::unknown) {
                return &entry;
            }
        }
        for (auto* chunk = known_chunks_; chunk; chunk = chunk->next) {
            for (auto& entry : chunk->entries) {
                if (entry.key == field::unknown) {
                    return &entry;
                }
            }
        }

        using chunk_t = entry_chunk<known_entry, KNOWN_HEADERS_CHUNK_SIZE>;
        auto* alloc = allocator();
        auto* chunk =
            alloc ? static_cast<chunk_t*>(alloc->allocate(sizeof(chunk_t), alignof(chunk_t)))
                  : nullptr;
        if (!chunk) {
            return nullptr;
        }
        new (chunk) chunk_t{};
        chunk->next = known_chunks_;
        known_chunks_ = chunk;
        return &chunk->entries[0];
    }

    [[nodiscard]] unknown_entry* find_unknown_entry(std::string_view name) noexcept {
        for (auto& entry : unknown_inline_) {
            if (entry.name && entry.name_length == name.size() &&
                ci_equal_fast(std::string_view(entry.name, entry.name_length), name)) {
                return &entry;
            }
        }
        for (auto* chunk = unknown_chunks_; chunk; chunk = chunk->next) {
            for (auto& entry : chunk->entries) {
                if (entry.name && entry.name_length == name.size() &&
                    ci_equal_fast(std::string_view(entry.name, entry.name_length), name)) {
                    return &entry;
                }
            }
        }
        return nullptr;
    }

    [[nodiscard]] const unknown_entry* find_unknown_entry(std::string_view name) const noexcept {
        return const_cast<headers_map*>(this)->find_unknown_entry(name);
    }

    [[nodiscard]] unknown_entry* append_unknown_entry() noexcept {
        for (auto& entry : unknown_inline_) {
            if (!entry.name) {
                return &entry;
            }
        }
        for (auto* chunk = unknown_chunks_; chunk; chunk = chunk->next) {
            for (auto& entry : chunk->entries) {
                if (!entry.name) {
                    return &entry;
                }
            }
        }

        using chunk_t = entry_chunk<unknown_entry, UNKNOWN_HEADERS_CHUNK_SIZE>;
        auto* alloc = allocator();
        auto* chunk =
            alloc ? static_cast<chunk_t*>(alloc->allocate(sizeof(chunk_t), alignof(chunk_t)))
                  : nullptr;
        if (!chunk) {
            return nullptr;
        }
        new (chunk) chunk_t{};
        chunk->next = unknown_chunks_;
        unknown_chunks_ = chunk;
        return &chunk->entries[0];
    }

    void reset_storage() noexcept {
        known_inline_.fill({});
        known_chunks_ = nullptr;
        known_size_ = 0;
        unknown_inline_.fill({});
        unknown_chunks_ = nullptr;
        unknown_size_ = 0;
    }

    monotonic_arena* arena_ = nullptr;
    monotonic_arena* fallback_arena_ = nullptr;
    monotonic_arena owned_arena_{4096};
    std::array<known_entry, KNOWN_HEADERS_INLINE_SIZE> known_inline_{};
    entry_chunk<known_entry, KNOWN_HEADERS_CHUNK_SIZE>* known_chunks_ = nullptr;
    size_t known_size_ = 0;
    std::array<unknown_entry, UNKNOWN_HEADERS_INLINE_SIZE> unknown_inline_{};
    entry_chunk<unknown_entry, UNKNOWN_HEADERS_CHUNK_SIZE>* unknown_chunks_ = nullptr;
    size_t unknown_size_ = 0;
};

} // namespace katana::http
