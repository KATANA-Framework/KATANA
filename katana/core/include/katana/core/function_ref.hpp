#pragma once

#include <type_traits>
#include <utility>

namespace katana {

template <typename Sig> class function_ref;

template <typename R, typename... Args> class function_ref<R(Args...)> {
public:
    template <typename F>
    function_ref(F&& f) noexcept
        : callable_(std::addressof(f)), invoker_([](void* callable, Args... args) -> R {
              return (*static_cast<std::remove_reference_t<F>*>(callable))(
                  std::forward<Args>(args)...);
          }) {}

    R operator()(Args... args) const { return invoker_(callable_, std::forward<Args>(args)...); }

    explicit operator bool() const noexcept { return callable_ != nullptr; }

private:
    void* callable_;
    R (*invoker_)(void*, Args...);
};

} // namespace katana
