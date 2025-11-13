#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <cstdint>

namespace katana {

// High-performance TCP socket options
struct socket_opts {
    // Enable TCP_NODELAY (disable Nagle's algorithm)
    static bool set_nodelay(int32_t fd) noexcept {
        int flag = 1;
        return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag)) == 0;
    }

    // Enable TCP_QUICKACK (send ACKs immediately)
    // Reduces latency for request-response patterns
    static bool set_quickack(int32_t fd) noexcept {
#ifdef TCP_QUICKACK
        int flag = 1;
        return setsockopt(fd, IPPROTO_TCP, TCP_QUICKACK, &flag, sizeof(flag)) == 0;
#else
        (void)fd;
        return true; // Not available, ignore
#endif
    }

    // Set SO_INCOMING_CPU for better CPU affinity
    // Routes packets to specific CPU core
    static bool set_incoming_cpu(int32_t fd, int32_t cpu) noexcept {
#ifdef SO_INCOMING_CPU
        return setsockopt(fd, SOL_SOCKET, SO_INCOMING_CPU, &cpu, sizeof(cpu)) == 0;
#else
        (void)fd;
        (void)cpu;
        return true; // Not available, ignore
#endif
    }

    // Enable SO_REUSEPORT for load balancing
    static bool set_reuseport(int32_t fd) noexcept {
#ifdef SO_REUSEPORT
        int flag = 1;
        return setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag)) == 0;
#else
        (void)fd;
        return true;
#endif
    }

    // Set TCP receive buffer size
    static bool set_rcvbuf(int32_t fd, int32_t size) noexcept {
        return setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) == 0;
    }

    // Set TCP send buffer size
    static bool set_sndbuf(int32_t fd, int32_t size) noexcept {
        return setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)) == 0;
    }

    // Apply optimal settings for low-latency server
    static bool apply_low_latency(int32_t fd, int32_t cpu_hint = -1) noexcept {
        bool ok = true;
        ok &= set_nodelay(fd);
        ok &= set_quickack(fd);
        if (cpu_hint >= 0) {
            ok &= set_incoming_cpu(fd, cpu_hint);
        }
        return ok;
    }

    // Apply optimal settings for high-throughput server
    static bool apply_high_throughput(int32_t fd, int32_t rcvbuf = 256*1024, int32_t sndbuf = 256*1024) noexcept {
        bool ok = true;
        ok &= set_nodelay(fd);
        ok &= set_rcvbuf(fd, rcvbuf);
        ok &= set_sndbuf(fd, sndbuf);
        return ok;
    }
};

} // namespace katana
