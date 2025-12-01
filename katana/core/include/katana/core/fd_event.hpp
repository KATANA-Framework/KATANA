#pragma once

#include "inplace_function.hpp"

#include <cstdint>

namespace katana {

enum class event_type : uint8_t {
    none = 0,
    readable = 1 << 0,
    writable = 1 << 1,
    edge_triggered = 1 << 2,
    error = 1 << 3,
    hup = 1 << 4,
    oneshot = 1 << 5,
    timeout = 1 << 6,
};

constexpr event_type operator|(event_type lhs, event_type rhs) noexcept {
    return static_cast<event_type>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

constexpr event_type operator&(event_type lhs, event_type rhs) noexcept {
    return static_cast<event_type>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

constexpr bool has_flag(event_type value, event_type flag) noexcept {
    return (static_cast<uint8_t>(value) & static_cast<uint8_t>(flag)) != 0;
}

using event_callback = inplace_function<void(event_type events), 96>;

} // namespace katana
