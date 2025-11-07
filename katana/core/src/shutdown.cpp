#include "katana/core/shutdown.hpp"

#include <csignal>

namespace katana {

namespace {

void signal_handler(int signal) {
    (void)signal;
    shutdown_manager::instance().request_shutdown();
}

} // namespace

void shutdown_manager::setup_signal_handlers() {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
}

} // namespace katana
