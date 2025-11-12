#include "katana/core/http.hpp"
#include <iostream>

using namespace katana::http;

int main() {
    // Test response headers
    response res;
    std::cout << "Initial headers size: " << res.headers.size() << std::endl;

    res.set_header("Content-Length", "13");
    std::cout << "After set Content-Length, headers size: " << res.headers.size() << std::endl;

    res.set_header("Content-Type", "text/plain");
    std::cout << "After set Content-Type, headers size: " << res.headers.size() << std::endl;

    std::cout << "\nIterating headers:" << std::endl;
    for (const auto& [name, value] : res.headers) {
        std::cout << "  " << name << ": " << value << std::endl;
    }

    std::cout << "\nSerialized response:" << std::endl;
    std::cout << res.serialize() << std::endl;

    // Test response::ok
    auto resp = response::ok("Hello!", "text/plain");
    std::cout << "\nresponse::ok headers size: " << resp.headers.size() << std::endl;
    std::cout << "Serialized:" << std::endl;
    std::cout << resp.serialize() << std::endl;

    return 0;
}
