// Validation API example: POST /user/register -> validates and responds "ok".
// Stresses compile-time validators (required, format: email, minLength, ranges, nullable optional).

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

struct validation_handler : generated::api_handler {
    response register_user(const RegisterUserRequest& body) override {
        (void)body; // Validation already performed; no business logic here.
        return response::json("ok");
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
    const uint32_t hw = std::max(1u, std::thread::hardware_concurrency());
    const uint32_t capped = std::min<uint32_t>(hw, 64);
    return static_cast<uint16_t>(capped);
}

int main() {
    validation_handler handler;
    const auto& api_router = generated::make_router(handler);

    const uint16_t port = read_port("PORT", read_port("VALIDATION_PORT", 8081));
    const uint16_t workers = worker_count();

    return http::server(api_router)
        .listen(port)
        .workers(workers)
        .on_start([&]() {
            std::cout << "Validation API running on :" << port << " with " << workers
                      << " worker threads" << std::endl;
        })
        .run();
}
