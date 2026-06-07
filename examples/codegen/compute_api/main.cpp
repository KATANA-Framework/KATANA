// Compute API example: POST /compute/sum -> returns sum of numbers
// Pure CPU path: zero I/O deps, zero-copy JSON → arena_vector<double>.
// Demonstrates katana_gen end-to-end (DTOs, validators, router, streaming JSON).

#include "generated/generated_dtos.hpp"
#include "generated/generated_handlers.hpp"
#include "generated/generated_json.hpp"
#include "generated/generated_router_bindings.hpp"
#include "generated/generated_routes.hpp"
#include "generated/generated_validators.hpp"
#include "katana/core/http_server.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <thread>

using namespace katana;
using namespace katana::http;

struct compute_handler : generated::api_handler {
    result<void> compute_sum(const compute_sum_request& nums, response& out) override {
        double acc = 0.0;
        // Tight loop over arena-backed vector to stress CPU/serialization only.
        for (double v : nums)
            acc += v;
        out.reset();
        out.status = 200;
        out.reason.assign(canonical_reason_phrase(200));
        out.body.clear();
        out.body.reserve(32);
        serialize_compute_sum_response_into(acc, out.body);
        out.set_header(http::field::content_type, "application/json");
        return {};
    }
};

static uint16_t read_port(const char* env_name, uint16_t fallback) {
    if (const char* value = std::getenv(env_name)) {
        int parsed = std::atoi(value);
        if (parsed > 0 && parsed < 65536)
            return static_cast<uint16_t>(parsed);
    }
    return fallback;
}

static uint16_t worker_count() {
    if (const char* value = std::getenv("KATANA_WORKERS")) {
        int parsed = std::atoi(value);
        if (parsed > 0 && parsed < 65536)
            return static_cast<uint16_t>(parsed);
    }
    const uint32_t hw = std::max(1u, std::thread::hardware_concurrency());
    const uint32_t capped = std::min<uint32_t>(hw, 64);
    return static_cast<uint16_t>(capped);
}

int main() {
    compute_handler handler;
    const auto& api_router = generated::make_fast_router(handler);

    const uint16_t port = read_port("PORT", read_port("COMPUTE_PORT", 8080));
    const uint16_t workers = worker_count();

    return http::server(api_router)
        .listen(port)
        .workers(workers)
        .on_start([&]() {
            std::cout << "Compute API running on :" << port << " with " << workers
                      << " worker threads" << std::endl;
        })
        .run();
}
