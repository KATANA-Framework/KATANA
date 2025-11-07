#pragma once

#include "result.hpp"

#include <cstdint>

namespace katana {

struct limits_config {
    uint64_t max_fds = 65536;
    uint64_t max_body_size = 10 * 1024 * 1024;  // 10MB
    uint64_t max_header_size = 8 * 1024;         // 8KB
};

class system_limits {
public:
    static result<void> set_max_fds(uint64_t limit);
    static result<uint64_t> get_max_fds();

    static result<void> apply(const limits_config& config);
};

} // namespace katana
