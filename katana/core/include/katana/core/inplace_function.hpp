#pragma once

#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

namespace katana {

template <typename Signature, size_t Capacity = 64> class inplace_function;

template <typename R, typename... Args, size_t Capacity>
class inplace_function<R(Args...), Capacity> {
public:
    inplace_function() noexcept : vtable_(nullptr) {}

    template <typename F,
              typename = std::enable_if_t<!std::is_same_v<std::decay_t<F>, inplace_function>>>
    inplace_function(F&& f) {
        static_assert(sizeof(std::decay_t<F>) <= Capacity,
                      "Callable too large for inplace storage");
        static_assert(alignof(std::decay_t<F>) <= alignof(std::max_align_t),
                      "Callable alignment too strict");
        static_assert(std::is_invocable_r_v<R, F, Args...>, "Callable not compatible");

        vtable_ = &vtable_for<std::decay_t<F>>;
        new (&storage_) std::decay_t<F>(std::forward<F>(f));
    }

    inplace_function(const inplace_function& other) : vtable_(nullptr) {
        if (other.vtable_) {
            other.vtable_->copy(&storage_, &other.storage_);
            vtable_ = other.vtable_;
        }
    }

    inplace_function(inplace_function&& other) noexcept : vtable_(nullptr) {
        if (other.vtable_) {
            other.vtable_->move(&storage_, &other.storage_);
            vtable_ = other.vtable_;
            other.vtable_ = nullptr;
        }
    }

    ~inplace_function() {
        if (vtable_) {
            vtable_->destroy(&storage_);
        }
    }

    inplace_function& operator=(const inplace_function& other) {
        if (this != &other) {
            if (vtable_) {
                vtable_->destroy(&storage_);
                vtable_ = nullptr;
            }
            if (other.vtable_) {
                other.vtable_->copy(&storage_, &other.storage_);
                vtable_ = other.vtable_;
            }
        }
        return *this;
    }

    inplace_function& operator=(inplace_function&& other) noexcept {
        if (this != &other) {
            if (vtable_) {
                vtable_->destroy(&storage_);
                vtable_ = nullptr;
            }
            if (other.vtable_) {
                other.vtable_->move(&storage_, &other.storage_);
                vtable_ = other.vtable_;
                other.vtable_ = nullptr;
            }
        }
        return *this;
    }

    template <typename F> inplace_function& operator=(F&& f) {
        *this = inplace_function(std::forward<F>(f));
        return *this;
    }

    R operator()(Args... args) const {
        return vtable_->invoke(&storage_, std::forward<Args>(args)...);
    }

    explicit operator bool() const noexcept { return vtable_ != nullptr; }

private:
    struct vtable_t {
        void (*destroy)(void* storage);
        void (*copy)(void* dst, const void* src);
        void (*move)(void* dst, void* src) noexcept;
        R (*invoke)(const void* storage, Args... args);
    };

    template <typename F>
    static constexpr vtable_t vtable_for = {
        [](void* storage) { static_cast<F*>(storage)->~F(); },
        [](void* dst, const void* src) { new (dst) F(*static_cast<const F*>(src)); },
        [](void* dst, void* src) noexcept {
            new (dst) F(std::move(*static_cast<F*>(src)));
            static_cast<F*>(src)->~F();
        },
        [](const void* storage, Args... args) -> R {
            return (*static_cast<const F*>(storage))(std::forward<Args>(args)...);
        }};

    alignas(std::max_align_t) std::byte storage_[Capacity];
    const vtable_t* vtable_;
};

} // namespace katana
