#include "katana/core/arena.hpp"
#include "katana/core/http.hpp"
#include "katana/core/http_field.hpp"
#include <iostream>

using namespace katana;
using namespace katana::http;

int main() {
    // Test array initialization
    const auto& popular = http::detail::get_popular_headers();
    std::cout << "Popular headers size: " << popular.size() << "\n";
    if (popular.size() > 0) {
        std::cout << "First popular header: " << popular[0].name << " = "
                  << static_cast<int>(popular[0].value) << "\n";
        std::cout << "Content-Length should be at index 6: " << popular[6].name << " = "
                  << static_cast<int>(popular[6].value) << "\n";
    }
    std::cout << "\n";

    // Test string_to_field
    auto cl_field = string_to_field("Content-Length");
    std::cout << "string_to_field('Content-Length') = " << static_cast<int>(cl_field) << "\n";
    std::cout << "  field::content_length = " << static_cast<int>(field::content_length) << "\n";
    std::cout << "  field::unknown = " << static_cast<int>(field::unknown) << "\n";
    std::cout << "\n";

    monotonic_arena arena;
    parser p(&arena);

    std::string request = "POST /api/data HTTP/1.1\r\n"
                          "Host: api.example.com\r\n"
                          "Content-Type: application/json\r\n"
                          "Content-Length: 13\r\n"
                          "\r\n"
                          "{\"key\":\"val\"}";

    auto data = as_bytes(request);

    // Test getting Content-Length by field enum directly
    auto cl_by_field = p.get_request().headers.get(field::content_length);
    std::cout << "Content-Length (by field enum) BEFORE parse: "
              << (cl_by_field ? *cl_by_field : "(null)") << "\n";

    auto result = p.parse(data);

    std::cout << "Parse result: " << (result.has_value() ? "success" : "error") << "\n";
    if (result.has_value()) {
        std::cout << "State: " << static_cast<int>(*result) << "\n";
        std::cout << "  (0=request_line, 1=headers, 2=body, 3=chunk_size, 6=complete)\n";
    }

    const auto& req = p.get_request();
    std::cout << "Method: " << static_cast<int>(req.http_method) << "\n";
    std::cout << "URI: " << req.uri << "\n";
    std::cout << "Body: '" << req.body << "'\n";
    std::cout << "Body size: " << req.body.size() << "\n";

    auto ct = req.header("Content-Type");
    std::cout << "Content-Type: " << (ct ? *ct : "(null)") << "\n";

    auto cl = req.header("Content-Length");
    std::cout << "Content-Length (by string): " << (cl ? *cl : "(null)") << "\n";

    auto cl2 = req.headers.get(field::content_length);
    std::cout << "Content-Length (by field enum): " << (cl2 ? *cl2 : "(null)") << "\n";

    std::cout << "Arena bytes allocated: " << arena.bytes_allocated() << "\n";

    return 0;
}
