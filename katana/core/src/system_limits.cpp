#include "katana/core/system_limits.hpp"

#include <cerrno>

#ifdef __linux__
#include <sys/resource.h>
#endif

namespace katana {

result<void> system_limits::set_max_fds(uint64_t limit) {
#ifdef __linux__
    struct rlimit rl;
    rl.rlim_cur = limit;
    rl.rlim_max = limit;

    if (setrlimit(RLIMIT_NOFILE, &rl) != 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    return {};
#else
    (void)limit;
    return {};
#endif
}

result<uint64_t> system_limits::get_max_fds() {
#ifdef __linux__
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) != 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    return rl.rlim_cur;
#else
    return 1024;
#endif
}

result<void> system_limits::apply(const limits_config& config) {
    auto result = set_max_fds(config.max_fds);
    if (!result) {
        return result;
    }

    return {};
}

} // namespace katana
