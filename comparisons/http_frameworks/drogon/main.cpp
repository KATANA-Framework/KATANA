#include <drogon/drogon.h>

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

constexpr uint16_t kDefaultPort = 8080;
constexpr size_t kDefaultWorkers = 4;
constexpr size_t kMaxItems = 1024;

uint16_t read_port() {
    for (const char* key : {"PORT", "HELLO_PORT", "COMPUTE_PORT"}) {
        if (const char* value = std::getenv(key)) {
            const int parsed = std::atoi(value);
            if (parsed > 0 && parsed < 65536) {
                return static_cast<uint16_t>(parsed);
            }
        }
    }
    return kDefaultPort;
}

size_t read_workers() {
    for (const char* key : {"BENCH_WORKERS", "KATANA_WORKERS", "WORKERS"}) {
        if (const char* value = std::getenv(key)) {
            const int parsed = std::atoi(value);
            if (parsed > 0) {
                return static_cast<size_t>(parsed);
            }
        }
    }
    return kDefaultWorkers;
}

drogon::HttpResponsePtr make_text_response(drogon::HttpStatusCode status, std::string body) {
    auto response = drogon::HttpResponse::newHttpResponse();
    response->setStatusCode(status);
    response->setContentTypeCode(drogon::CT_TEXT_PLAIN);
    response->setBody(std::move(body));
    return response;
}

} // namespace

int main() {
    using namespace drogon;

    const uint16_t port = read_port();
    const size_t workers = read_workers();

    app().setLogLevel(trantor::Logger::kWarn);
    app().setThreadNum(workers);
    app().addListener("0.0.0.0", port);

    app().registerHandler(
        "/",
        [](const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& callback) {
            callback(make_text_response(k200OK, "Hello, World!"));
        },
        {Get});

    app().registerHandler(
        "/compute/sum",
        [](const HttpRequestPtr& request, std::function<void(const HttpResponsePtr&)>&& callback) {
            const auto json = request->getJsonObject();
            if (!json || !json->isArray()) {
                callback(make_text_response(k400BadRequest, "payload must be a JSON array"));
                return;
            }

            const Json::ArrayIndex size = json->size();
            if (size == 0 || size > static_cast<Json::ArrayIndex>(kMaxItems)) {
                callback(
                    make_text_response(k400BadRequest, "payload must contain 1..=1024 numbers"));
                return;
            }

            double total = 0.0;
            for (const auto& item : *json) {
                if (!item.isNumeric()) {
                    callback(make_text_response(k400BadRequest,
                                                "payload must contain only numeric values"));
                    return;
                }
                total += item.asDouble();
            }

            Json::Value output(total);
            callback(HttpResponse::newHttpJsonResponse(output));
        },
        {Post});

    std::cout << "drogon comparison server listening on :" << port << " with " << workers
              << " workers" << std::endl;

    app().run();
    return 0;
}
