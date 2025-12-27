// Text Processing API example
// Tests katana_gen: multiple endpoints, different schemas, string operations, validation
// Pure stateless transformations - no storage, no state

#include "generated/generated_dtos.hpp"
#include "generated/generated_handlers.hpp"
#include "generated/generated_json.hpp"
#include "generated/generated_router_bindings.hpp"
#include "generated/generated_routes.hpp"
#include "generated/generated_validators.hpp"
#include "katana/core/http_server.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

using namespace katana;
using namespace katana::http;

struct text_handler : generated::api_handler {
    // Convert text to uppercase
    response text_uppercase(const text_uppercase_request& req) override {
        text_uppercase_response resp;
        resp.result = req.text;
        std::transform(resp.result.begin(),
                       resp.result.end(),
                       resp.result.begin(),
                       [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
        return response::json(serialize_text_uppercase_response(resp));
    }

    // Convert text to lowercase
    response text_lowercase(const text_lowercase_request& req) override {
        text_lowercase_response resp;
        resp.result = req.text;
        std::transform(resp.result.begin(),
                       resp.result.end(),
                       resp.result.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return response::json(serialize_text_lowercase_response(resp));
    }

    // Reverse text
    response text_reverse(const text_reverse_request& req) override {
        text_reverse_response resp;
        resp.result = req.text;
        std::reverse(resp.result.begin(), resp.result.end());
        return response::json(serialize_text_reverse_response(resp));
    }

    // Calculate text statistics
    response text_stats(const text_stats_request& req) override {
        text_stats_response stats;
        stats.chars = static_cast<int64_t>(req.text.length());

        // Count words (split by whitespace)
        int64_t word_count = 0;
        bool in_word = false;
        for (char c : req.text) {
            if (std::isspace(static_cast<unsigned char>(c))) {
                in_word = false;
            } else if (!in_word) {
                in_word = true;
                ++word_count;
            }
        }
        stats.words = word_count;

        // Count lines
        int64_t line_count = 1;
        for (char c : req.text) {
            if (c == '\n')
                ++line_count;
        }
        stats.lines = line_count;

        return response::json(serialize_text_stats_response(stats));
    }

    // Apply transformation based on operation type
    response text_transform(const text_transform_request& req) override {
        text_transform_response resp;
        resp.original_length = static_cast<int64_t>(req.text.length());
        resp.operation_applied = to_string(req.operation);

        std::string text(req.text.begin(), req.text.end());

        // Trim if requested
        if (req.trim) {
            // Trim leading whitespace
            size_t start = text.find_first_not_of(" \t\n\r");
            if (start != std::string::npos) {
                text = text.substr(start);
            } else {
                text.clear();
            }
            // Trim trailing whitespace
            size_t end = text.find_last_not_of(" \t\n\r");
            if (end != std::string::npos) {
                text = text.substr(0, end + 1);
            }
        }

        // Apply operation
        if (req.operation == text_transform_operation::upper) {
            std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
                return static_cast<char>(std::toupper(c));
            });
        } else if (req.operation == text_transform_operation::lower) {
            std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });
        } else if (req.operation == text_transform_operation::reverse) {
            std::reverse(text.begin(), text.end());
        } else if (req.operation == text_transform_operation::title) {
            // Title case: capitalize first letter of each word
            bool capitalize_next = true;
            for (char& c : text) {
                if (std::isspace(static_cast<unsigned char>(c))) {
                    capitalize_next = true;
                } else if (capitalize_next) {
                    c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
                    capitalize_next = false;
                } else {
                    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                }
            }
        }

        resp.result = text;
        return response::json(serialize_text_transform_response(resp));
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
    text_handler handler;
    const auto& api_router = generated::make_router(handler);

    const uint16_t port = read_port("PORT", read_port("TEXT_PORT", 8082));
    const uint16_t workers = worker_count();

    return http::server(api_router)
        .listen(port)
        .workers(workers)
        .on_start([&]() {
            std::cout << "Text Processing API running on :" << port << " with " << workers
                      << " worker threads\n";
            std::cout << "Endpoints:\n"
                      << "  POST /text/uppercase - Convert to uppercase\n"
                      << "  POST /text/lowercase - Convert to lowercase\n"
                      << "  POST /text/reverse - Reverse text\n"
                      << "  POST /text/stats - Get text statistics\n"
                      << "  POST /text/transform - Apply transformation with options\n";
        })
        .run();
}
