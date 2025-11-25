#include "katana/core/cpu_info.hpp"

#include <thread>

#ifdef __linux__
#include <pthread.h>
#include <sched.h>
#endif

namespace katana {

uint32_t cpu_info::core_count() noexcept {
    auto count = std::thread::hardware_concurrency();
    return count > 0 ? count : 1;
}

bool cpu_info::pin_thread_to_core(uint32_t core_id) noexcept {
#ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);

    pthread_t thread = pthread_self();
    return pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset) == 0;
#else
    (void)core_id;
    return true;
#endif
}

} // namespace katana
