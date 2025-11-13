#pragma once

#include <vector>
#include <memory>
#include <cstddef>
#include <atomic>

namespace katana {

// Intrusive object pool with no atomic refcount overhead
// T must have a `T* next_free` member for intrusive list
template<typename T>
class intrusive_pool {
public:
    explicit intrusive_pool(size_t initial_capacity = 128) {
        storage_.reserve(initial_capacity);
        grow(initial_capacity);
    }

    ~intrusive_pool() noexcept {
        // All objects must be returned before destruction
        storage_.clear();
    }

    intrusive_pool(const intrusive_pool&) = delete;
    intrusive_pool& operator=(const intrusive_pool&) = delete;
    intrusive_pool(intrusive_pool&&) = delete;
    intrusive_pool& operator=(intrusive_pool&&) = delete;

    // Acquire object from pool (nullptr if need to grow)
    [[nodiscard]] T* acquire() noexcept {
        if (free_list_) {
            T* obj = free_list_;
            free_list_ = obj->next_free;
            obj->next_free = nullptr;
            ++active_count_;
            ++total_acquires_;
            return obj;
        }

        // Need to grow
        if (grow(growth_size_)) {
            return acquire();
        }

        ++oom_count_;
        return nullptr;
    }

    // Return object to pool
    void release(T* obj) noexcept {
        if (!obj) return;

        obj->next_free = free_list_;
        free_list_ = obj;
        --active_count_;
        ++total_releases_;
    }

    // RAII wrapper
    class scoped_object {
    public:
        explicit scoped_object(intrusive_pool* pool, T* obj) noexcept
            : pool_(pool), obj_(obj) {}

        ~scoped_object() noexcept {
            if (pool_ && obj_) {
                pool_->release(obj_);
            }
        }

        scoped_object(const scoped_object&) = delete;
        scoped_object& operator=(const scoped_object&) = delete;

        scoped_object(scoped_object&& other) noexcept
            : pool_(other.pool_), obj_(other.obj_) {
            other.pool_ = nullptr;
            other.obj_ = nullptr;
        }

        scoped_object& operator=(scoped_object&& other) noexcept {
            if (this != &other) {
                if (pool_ && obj_) {
                    pool_->release(obj_);
                }
                pool_ = other.pool_;
                obj_ = other.obj_;
                other.pool_ = nullptr;
                other.obj_ = nullptr;
            }
            return *this;
        }

        [[nodiscard]] T* get() const noexcept { return obj_; }
        [[nodiscard]] T* operator->() const noexcept { return obj_; }
        [[nodiscard]] T& operator*() const noexcept { return *obj_; }
        [[nodiscard]] explicit operator bool() const noexcept { return obj_ != nullptr; }

    private:
        intrusive_pool* pool_;
        T* obj_;
    };

    [[nodiscard]] scoped_object acquire_scoped() noexcept {
        return scoped_object(this, acquire());
    }

    // Metrics
    [[nodiscard]] size_t active_count() const noexcept { return active_count_; }
    [[nodiscard]] size_t total_capacity() const noexcept { return storage_.size(); }
    [[nodiscard]] size_t total_acquires() const noexcept { return total_acquires_; }
    [[nodiscard]] size_t total_releases() const noexcept { return total_releases_; }
    [[nodiscard]] size_t oom_count() const noexcept { return oom_count_; }

private:
    bool grow(size_t count) noexcept {
        size_t old_size = storage_.size();

        try {
            storage_.reserve(old_size + count);
            for (size_t i = 0; i < count; ++i) {
                auto obj = std::make_unique<T>();
                obj->next_free = free_list_;
                free_list_ = obj.get();
                storage_.push_back(std::move(obj));
            }
            return true;
        } catch (...) {
            return false;
        }
    }

    std::vector<std::unique_ptr<T>> storage_;
    T* free_list_ = nullptr;
    size_t active_count_ = 0;
    size_t growth_size_ = 64;
    size_t total_acquires_ = 0;
    size_t total_releases_ = 0;
    size_t oom_count_ = 0;
};

} // namespace katana
