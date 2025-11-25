#pragma once

#if defined(KATANA_USE_IO_URING)
#include "io_uring_reactor.hpp"
namespace katana {
using reactor_impl = io_uring_reactor;
}
#elif defined(KATANA_USE_EPOLL)
#include "epoll_reactor.hpp"
namespace katana {
using reactor_impl = epoll_reactor;
}
#else
#error "No reactor backend selected. Define KATANA_USE_EPOLL or KATANA_USE_IO_URING"
#endif
