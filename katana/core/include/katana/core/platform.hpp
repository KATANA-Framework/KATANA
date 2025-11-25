#pragma once

#include <cstdint>
#include <cstring>

#ifdef __linux__
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#define KATANA_BSD_LIKE
#elif defined(_WIN32)
#include <winsock2.h>
#define KATANA_WINDOWS
#endif

namespace katana::platform {

#ifdef __linux__
inline int accept_nonblock(int sockfd, struct sockaddr* addr, socklen_t* addrlen) {
    return accept4(sockfd, addr, addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
}
#elif defined(KATANA_BSD_LIKE)
inline int accept_nonblock(int sockfd, struct sockaddr* addr, socklen_t* addrlen) {
    int fd = accept(sockfd, addr, addrlen);
    if (fd < 0)
        return fd;

    int flags = fcntl(fd, F_GETFL, 0);
    if (flags >= 0) {
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }

    flags = fcntl(fd, F_GETFD, 0);
    if (flags >= 0) {
        fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
    }

    return fd;
}
#endif

inline const void*
find_pattern(const void* haystack, size_t hlen, const void* needle, size_t nlen) {
    if (nlen == 0 || hlen < nlen)
        return nullptr;

#ifdef __linux__
    return memmem(haystack, hlen, needle, nlen);
#else
    const char* h = static_cast<const char*>(haystack);
    const char* n = static_cast<const char*>(needle);

    if (nlen == 1) {
        return std::memchr(h, n[0], hlen);
    }

    for (size_t i = 0; i <= hlen - nlen; ++i) {
        if (h[i] == n[0] && std::memcmp(h + i, n, nlen) == 0) {
            return h + i;
        }
    }
    return nullptr;
#endif
}

inline bool set_nonblocking(int fd) {
#if defined(__linux__) || defined(KATANA_BSD_LIKE)
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        return false;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK) >= 0;
#else
    return false;
#endif
}

inline bool set_cloexec(int fd) {
#if defined(__linux__) || defined(KATANA_BSD_LIKE)
    int flags = fcntl(fd, F_GETFD, 0);
    if (flags < 0)
        return false;
    return fcntl(fd, F_SETFD, flags | FD_CLOEXEC) >= 0;
#else
    return false;
#endif
}

} // namespace katana::platform
