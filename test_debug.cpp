#include "katana/core/http.hpp"
#include <iostream>

int main() {
    using namespace katana::http;
    parser p;
    std::string colon_value = "GET / HTTP/1.1\r\nX-Header: value:with:colons\r\n\r\n";
    auto data = as_bytes(colon_value);
    auto result = p.parse(data);
    
    if (!result.has_value()) {
        std::cout << "Parse error: " << result.error().message() << std::endl;
    } else {
        std::cout << "Parse success, state: " << static_cast<int>(*result) << std::endl;
        auto hdr = p.get_request().header("X-Header");
        if (hdr) {
            std::cout << "Header value: " << *hdr << std::endl;
        } else {
            std::cout << "Header not found" << std::endl;
        }
    }
    
    return 0;
}
