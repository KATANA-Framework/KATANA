#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <memory>
#include <new>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace katana {

class monotonic_arena {
public:
    static constexpr size_t DEFAULT_BLOCK_SIZE = 64UL * 1024UL;
    static constexpr size_t MAX_ALIGNMENT = 64;
    static constexpr size_t MAX_BLOCKS = 32;

    explicit monotonic_arena(size_t block_size = DEFAULT_BLOCK_SIZE) noexcept;
    ~monotonic_arena() noexcept;

    monotonic_arena(const monotonic_arena&) = delete;
    monotonic_arena& operator=(const monotonic_arena&) = delete;
    monotonic_arena(monotonic_arena&&) noexcept;
    monotonic_arena& operator=(monotonic_arena&&) noexcept;

    [[nodiscard]] void* allocate(size_t bytes,
                                 size_t alignment = alignof(std::max_align_t)) noexcept;

    template <typename T> [[nodiscard]] T* allocate_array(size_t count) noexcept {
        return static_cast<T*>(allocate(sizeof(T) * count, alignof(T)));
    }

    [[nodiscard]] char* allocate_string(std::string_view str) noexcept {
        char* ptr = static_cast<char*>(allocate(str.size() + 1, 1));
        if (ptr) {
            std::memcpy(ptr, str.data(), str.size());
            ptr[str.size()] = '\0';
        }
        return ptr;
    }

    void reset() noexcept;

    [[nodiscard]] size_t bytes_allocated() const noexcept { return bytes_allocated_; }
    [[nodiscard]] size_t total_capacity() const noexcept { return total_capacity_; }

private:
    struct block {
        uint8_t* data;
        size_t size;
        size_t used;

        block() noexcept : data(nullptr), size(0), used(0) {}
        block(size_t s) noexcept;
        ~block() noexcept;

        block(const block&) = delete;
        block& operator=(const block&) = delete;
        block(block&& other) noexcept;
        block& operator=(block&& other) noexcept;
    };

    [[nodiscard]] static constexpr size_t align_up(size_t n, size_t alignment) noexcept {
        return (n + alignment - 1) & ~(alignment - 1);
    }

    [[nodiscard]] bool allocate_new_block(size_t min_size) noexcept;

    std::array<block, MAX_BLOCKS> blocks_;
    size_t num_blocks_ = 0;
    size_t current_block_index_ = 0;
    size_t block_size_;
    size_t bytes_allocated_ = 0;
    size_t total_capacity_ = 0;
};

template <typename T> class arena_allocator {
public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    explicit arena_allocator(monotonic_arena* arena) noexcept : arena_(arena) {}

    template <typename U>
    arena_allocator(const arena_allocator<U>& other) noexcept : arena_(other.arena_) {}

    [[nodiscard]] T* allocate(size_t n) {
        if (n == 0) {
            return nullptr;
        }
        if (arena_) {
            void* ptr = arena_->allocate(n * sizeof(T), alignof(T));
            if (!ptr) {
                throw std::bad_alloc();
            }
            return static_cast<T*>(ptr);
        }
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* ptr, size_t) noexcept {
        if (!arena_ && ptr) {
            ::operator delete(ptr);
        }
    }

    template <typename U> bool operator==(const arena_allocator<U>& other) const noexcept {
        return arena_ == other.arena_;
    }

    template <typename U> bool operator!=(const arena_allocator<U>& other) const noexcept {
        return arena_ != other.arena_;
    }

    monotonic_arena* arena_;
};

template <typename T, size_t InlineCapacity = 0> class arena_vector;

template <typename T> class arena_vector<T, 0> : public std::vector<T, arena_allocator<T>> {
public:
    using base = std::vector<T, arena_allocator<T>>;
    using base::base;

    arena_vector() = default;
    explicit arena_vector(monotonic_arena* arena) : base(arena_allocator<T>(arena)) {}
};

template <typename T, size_t InlineCapacity> class arena_vector {
public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using allocator_type = arena_allocator<T>;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using iterator = T*;
    using const_iterator = const T*;

    arena_vector() noexcept(noexcept(arena_allocator<T>(nullptr)))
        : allocator_(nullptr), data_(inline_data()), size_(0), capacity_(InlineCapacity) {}

    explicit arena_vector(allocator_type alloc) noexcept
        : allocator_(alloc), data_(inline_data()), size_(0), capacity_(InlineCapacity) {}

    explicit arena_vector(monotonic_arena* arena) noexcept : arena_vector(allocator_type(arena)) {}

    arena_vector(const arena_vector& other)
        : allocator_(other.allocator_), data_(inline_data()), size_(0), capacity_(InlineCapacity) {
        reserve(other.size_);
        for (const auto& value : other) {
            emplace_back(value);
        }
    }

    arena_vector(arena_vector&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
        : allocator_(other.allocator_), data_(inline_data()), size_(0), capacity_(InlineCapacity) {
        move_from(std::move(other));
    }

    arena_vector& operator=(const arena_vector& other) {
        if (this == &other) {
            return *this;
        }

        clear();
        release_heap();
        allocator_ = other.allocator_;
        reserve(other.size_);
        for (const auto& value : other) {
            emplace_back(value);
        }
        return *this;
    }

    arena_vector&
    operator=(arena_vector&& other) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                             std::is_nothrow_move_assignable_v<T>) {
        if (this == &other) {
            return *this;
        }

        clear();
        release_heap();
        allocator_ = other.allocator_;
        move_from(std::move(other));
        return *this;
    }

    ~arena_vector() noexcept {
        clear();
        release_heap();
    }

    [[nodiscard]] iterator begin() noexcept { return data_; }
    [[nodiscard]] const_iterator begin() const noexcept { return data_; }
    [[nodiscard]] const_iterator cbegin() const noexcept { return data_; }

    [[nodiscard]] iterator end() noexcept { return data_ + size_; }
    [[nodiscard]] const_iterator end() const noexcept { return data_ + size_; }
    [[nodiscard]] const_iterator cend() const noexcept { return data_ + size_; }

    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] size_type size() const noexcept { return size_; }
    [[nodiscard]] size_type capacity() const noexcept { return capacity_; }

    [[nodiscard]] pointer data() noexcept { return data_; }
    [[nodiscard]] const_pointer data() const noexcept { return data_; }

    [[nodiscard]] reference operator[](size_type index) noexcept { return data_[index]; }
    [[nodiscard]] const_reference operator[](size_type index) const noexcept {
        return data_[index];
    }

    [[nodiscard]] reference front() noexcept { return data_[0]; }
    [[nodiscard]] const_reference front() const noexcept { return data_[0]; }
    [[nodiscard]] reference back() noexcept { return data_[size_ - 1]; }
    [[nodiscard]] const_reference back() const noexcept { return data_[size_ - 1]; }

    void clear() noexcept {
        for (size_t i = size_; i > 0; --i) {
            data_[i - 1].~T();
        }
        size_ = 0;
    }

    void reserve(size_type new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }
        reallocate(new_capacity);
    }

    template <typename... Args> reference emplace_back(Args&&... args) {
        ensure_capacity_for_one_more();
        new (data_ + size_) T(std::forward<Args>(args)...);
        ++size_;
        return back();
    }

    void push_back(const T& value) { emplace_back(value); }
    void push_back(T&& value) { emplace_back(std::move(value)); }

    void pop_back() noexcept {
        if (size_ == 0) {
            return;
        }
        --size_;
        data_[size_].~T();
    }

    iterator erase(const_iterator pos) { return erase(pos, pos + 1); }

    iterator erase(const_iterator first, const_iterator last) {
        if (first == last) {
            return const_cast<iterator>(first);
        }

        const size_t start = static_cast<size_t>(first - cbegin());
        const size_t count = static_cast<size_t>(last - first);
        const size_t tail = size_ - start - count;

        for (size_t i = 0; i < tail; ++i) {
            data_[start + i] = std::move(data_[start + count + i]);
        }
        for (size_t i = size_; i > size_ - count; --i) {
            data_[i - 1].~T();
        }
        size_ -= count;
        return begin() + static_cast<difference_type>(start);
    }

private:
    using inline_storage_t = std::aligned_storage_t<sizeof(T), alignof(T)>;

    [[nodiscard]] pointer inline_data() noexcept {
        return reinterpret_cast<pointer>(inline_storage_.data());
    }

    [[nodiscard]] const_pointer inline_data() const noexcept {
        return reinterpret_cast<const_pointer>(inline_storage_.data());
    }

    [[nodiscard]] bool using_inline_storage() const noexcept { return data_ == inline_data(); }

    void ensure_capacity_for_one_more() {
        if (size_ < capacity_) {
            return;
        }
        const size_t grown = capacity_ > 0 ? capacity_ * 2 : 1;
        reallocate(grown);
    }

    void reallocate(size_type new_capacity) {
        pointer new_data = allocator_.allocate(new_capacity);
        size_t moved = 0;
        try {
            for (; moved < size_; ++moved) {
                new (new_data + moved) T(std::move_if_noexcept(data_[moved]));
            }
        } catch (...) {
            for (size_t i = moved; i > 0; --i) {
                new_data[i - 1].~T();
            }
            allocator_.deallocate(new_data, new_capacity);
            throw;
        }

        for (size_t i = size_; i > 0; --i) {
            data_[i - 1].~T();
        }

        pointer old_data = data_;
        const size_t old_capacity = capacity_;
        data_ = new_data;
        capacity_ = new_capacity;

        if (old_data != inline_data()) {
            allocator_.deallocate(old_data, old_capacity);
        }
    }

    void release_heap() noexcept {
        if (!using_inline_storage()) {
            allocator_.deallocate(data_, capacity_);
            data_ = inline_data();
            capacity_ = InlineCapacity;
        }
    }

    void move_from(arena_vector&& other) {
        if (other.using_inline_storage()) {
            reserve(other.size_);
            for (auto& value : other) {
                emplace_back(std::move(value));
            }
            other.clear();
            return;
        }

        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = other.inline_data();
        other.size_ = 0;
        other.capacity_ = InlineCapacity;
    }

    allocator_type allocator_;
    pointer data_;
    size_type size_;
    size_type capacity_;
    std::array<inline_storage_t, InlineCapacity> inline_storage_{};
};

template <typename CharT = char>
using arena_string = std::basic_string<CharT, std::char_traits<CharT>, arena_allocator<CharT>>;

using arena_string_view = std::string_view;

} // namespace katana
