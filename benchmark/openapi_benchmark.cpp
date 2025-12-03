#include "katana/core/arena.hpp"
#include "katana/core/openapi_loader.hpp"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <vector>

// Don't use 'using namespace std::chrono' to avoid conflicts with std::atomic_impl::load

struct benchmark_result {
    std::string name;
    double throughput;
    double latency_p50;
    double latency_p99;
    double latency_p999;
    uint64_t operations;
    uint64_t duration_ms;
    uint64_t errors;
};

void print_result(const benchmark_result& result) {
    std::cout << "\n=== " << result.name << " ===\n";
    std::cout << "Operations: " << result.operations << "\n";
    std::cout << "Duration: " << result.duration_ms << " ms\n";
    std::cout << "Throughput: " << std::fixed << std::setprecision(2) << result.throughput
              << " ops/sec\n";
    std::cout << "Errors: " << result.errors << "\n";
    if (result.latency_p50 > 0.0) {
        std::cout << "Latency p50: " << std::fixed << std::setprecision(3) << result.latency_p50
                  << " us\n";
        std::cout << "Latency p99: " << std::fixed << std::setprecision(3) << result.latency_p99
                  << " us\n";
        std::cout << "Latency p999: " << std::fixed << std::setprecision(3) << result.latency_p999
                  << " us\n";
    }
}

constexpr std::string_view SIMPLE_SPEC = R"(
{
  "openapi": "3.0.0",
  "info": {
    "title": "Test API",
    "version": "1.0.0"
  },
  "paths": {
    "/users": {
      "get": {
        "operationId": "listUsers",
        "responses": {
          "200": {
            "description": "OK"
          }
        }
      }
    }
  }
}
)";

constexpr std::string_view COMPLEX_SPEC = R"(
{
  "openapi": "3.0.0",
  "info": {
    "title": "Complex API",
    "version": "2.0.0"
  },
  "paths": {
    "/users": {
      "get": {
        "operationId": "listUsers",
        "parameters": [
          {
            "name": "limit",
            "in": "query",
            "schema": {
              "type": "integer"
            }
          }
        ],
        "responses": {
          "200": {
            "description": "OK",
            "content": {
              "application/json": {
                "schema": {
                  "$ref": "#/components/schemas/UserList"
                }
              }
            }
          }
        }
      },
      "post": {
        "operationId": "createUser",
        "requestBody": {
          "content": {
            "application/json": {
              "schema": {
                "$ref": "#/components/schemas/User"
              }
            }
          }
        },
        "responses": {
          "201": {
            "description": "Created"
          }
        }
      }
    },
    "/users/{id}": {
      "get": {
        "operationId": "getUser",
        "parameters": [
          {
            "name": "id",
            "in": "path",
            "required": true,
            "schema": {
              "type": "integer"
            }
          }
        ],
        "responses": {
          "200": {
            "description": "OK",
            "content": {
              "application/json": {
                "schema": {
                  "$ref": "#/components/schemas/User"
                }
              }
            }
          }
        }
      }
    }
  },
  "components": {
    "schemas": {
      "User": {
        "type": "object",
        "required": ["id", "name"],
        "properties": {
          "id": {
            "type": "integer"
          },
          "name": {
            "type": "string",
            "minLength": 1,
            "maxLength": 100
          },
          "email": {
            "type": "string"
          },
          "age": {
            "type": "integer",
            "minimum": 0,
            "maximum": 150
          }
        }
      },
      "UserList": {
        "type": "object",
        "properties": {
          "users": {
            "type": "array",
            "items": {
              "$ref": "#/components/schemas/User"
            }
          },
          "total": {
            "type": "integer"
          }
        }
      }
    }
  }
}
)";

benchmark_result
run_parse_benchmark(std::string_view spec, const std::string& name, uint64_t iterations) {
    std::vector<double> latencies;
    latencies.reserve(iterations);

    uint64_t errors = 0;
    auto start = std::chrono::steady_clock::now();

    for (uint64_t i = 0; i < iterations; ++i) {
        auto iter_start = std::chrono::steady_clock::now();
        katana::monotonic_arena arena;
        auto result = katana::openapi::load_from_string(spec, arena);
        auto iter_end = std::chrono::steady_clock::now();

        if (!result) {
            ++errors;
        }

        auto latency_us =
            std::chrono::duration_cast<std::chrono::microseconds>(iter_end - iter_start).count();
        latencies.push_back(static_cast<double>(latency_us));
    }

    auto end = std::chrono::steady_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::sort(latencies.begin(), latencies.end());

    benchmark_result result;
    result.name = name;
    result.operations = iterations;
    result.errors = errors;
    result.duration_ms = static_cast<uint64_t>(duration_ms);
    result.throughput =
        static_cast<double>(iterations) / (static_cast<double>(duration_ms) / 1000.0);
    result.latency_p50 = latencies[latencies.size() / 2];
    result.latency_p99 = latencies[latencies.size() * 99 / 100];
    result.latency_p999 = latencies[latencies.size() * 999 / 1000];

    return result;
}

int main() {
    std::cout << "OpenAPI Parser Benchmark\n";
    std::cout << "========================\n";

    const uint64_t iterations = 10000;

    auto simple_result =
        run_parse_benchmark(SIMPLE_SPEC, "Simple Spec (1 path, no refs)", iterations);
    print_result(simple_result);

    auto complex_result = run_parse_benchmark(
        COMPLEX_SPEC, "Complex Spec (3 operations, $refs, schemas)", iterations);
    print_result(complex_result);

    std::cout << "\n";
    return 0;
}
