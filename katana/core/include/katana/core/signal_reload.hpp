#pragma once

// Hot-reload on SIGHUP. Installs a SIGHUP handler that (off the signal, on a background watcher
// thread — file I/O isn't async-signal-safe) re-applies the layered config via config::reload() and
// invokes an optional callback, e.g. to hot-swap the TLS cert with server::reload_tls(). This
// packages the pattern services otherwise hand-roll:
//
//   katana::config::reload_on_sighup(cfg, [&] { srv.reload_tls(); });
//
// The watcher thread runs for the program's lifetime, so `cfg` and anything the callback captures
// must outlive it (typically both live in main). Call once.

#include "config.hpp"

#include <atomic>
#include <chrono>
#include <csignal>
#include <functional>
#include <thread>
#include <utility>

namespace katana::config {

inline std::atomic<bool>& sighup_flag() {
    static std::atomic<bool> flag{false};
    return flag;
}

inline void reload_on_sighup(config& cfg, std::function<void()> after = {}) {
    std::signal(SIGHUP, [](int) { sighup_flag().store(true, std::memory_order_relaxed); });
    std::thread([&cfg, after = std::move(after)]() mutable {
        for (;;) {
            if (sighup_flag().exchange(false, std::memory_order_relaxed)) {
                cfg.reload();
                if (after) {
                    after();
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    }).detach();
}

} // namespace katana::config
