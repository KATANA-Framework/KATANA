#pragma once

#include <cstdint>

namespace katana {

struct cpu_info {
    static uint32_t core_count() noexcept;
    static bool pin_thread_to_core(uint32_t core_id) noexcept;
};

} // namespace katana
