#include "katana/core/epoll_reactor.hpp"

#include <cstring>
#include <iostream>
#include <unistd.h>

int main() {
    katana::epoll_reactor reactor;

    std::cout << "Starting reactor example...\n";

    reactor.schedule([]() { std::cout << "Immediate task executed\n"; });

    reactor.schedule_after(std::chrono::milliseconds(500),
                           []() { std::cout << "Delayed task executed after 500ms\n"; });

    int pipefd[2];
    if (pipe(pipefd) == 0) {
        auto res = reactor.register_fd(
            pipefd[0], katana::event_type::readable, [pipefd](katana::event_type events) {
                if (katana::has_flag(events, katana::event_type::readable)) {
                    char buf[64];
                    ssize_t n = read(pipefd[0], buf, sizeof(buf));
                    if (n > 0) {
                        std::cout << "Read from pipe: "
                                  << std::string_view(buf, static_cast<size_t>(n)) << "\n";
                    }
                }
            });

        if (res) {
            reactor.schedule_after(std::chrono::milliseconds(200), [pipefd]() {
                const char* msg = "Hello from reactor!";
                [[maybe_unused]] auto _ = write(pipefd[1], msg, strlen(msg));
            });
        }

        reactor.schedule_after(std::chrono::milliseconds(1000), [&reactor, pipefd]() {
            std::cout << "Stopping reactor...\n";
            close(pipefd[0]);
            close(pipefd[1]);
            reactor.stop();
        });
    }

    auto result = reactor.run();

    if (result) {
        std::cout << "Reactor stopped successfully\n";

        const auto& metrics = reactor.metrics();
        std::cout << "\nReactor metrics:\n";
        std::cout << "  Tasks scheduled: " << metrics.tasks_scheduled.load() << "\n";
        std::cout << "  Tasks executed: " << metrics.tasks_executed.load() << "\n";
        std::cout << "  FD events processed: " << metrics.fd_events_processed.load() << "\n";
        std::cout << "  Timers fired: " << metrics.timers_fired.load() << "\n";
        std::cout << "  Exceptions caught: " << metrics.exceptions_caught.load() << "\n";

        return 0;
    } else {
        std::cerr << "Reactor error: " << result.error().message() << "\n";
        return 1;
    }
}
