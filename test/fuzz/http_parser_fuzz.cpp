#include "katana/core/arena.hpp"
#include "katana/core/http.hpp"
#include <cstddef>
#include <cstdint>
#include <span>

extern "C" int32_t LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size == 0 || size > katana::http::MAX_BUFFER_SIZE) {
        return 0;
    }

    katana::monotonic_arena arena;
    katana::http::parser p(&arena);

    try {
        auto result = p.parse(std::span<const uint8_t>(data, size));

        // Exercise the parser state machine
        if (result && p.is_complete()) {
            const auto& req = p.get_request();

            // Access request fields to ensure no crashes
            [[maybe_unused]] auto method = req.http_method;
            [[maybe_unused]] const auto& uri = req.uri;
            [[maybe_unused]] const auto& body = req.body;

            // Try to get various headers
            [[maybe_unused]] auto host = req.header("Host");
            [[maybe_unused]] auto content_length = req.header("Content-Length");
            [[maybe_unused]] auto transfer_encoding = req.header("Transfer-Encoding");
            [[maybe_unused]] auto connection = req.header("Connection");
        }
    } catch (...) {
        // Fuzzer should not crash, but may throw expected exceptions
    }

    return 0;
}
