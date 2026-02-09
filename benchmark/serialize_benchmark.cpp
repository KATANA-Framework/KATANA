// benchmark/serialize_benchmark.cpp
#include <array>
#include <charconv>
#include <chrono>
#include <cstdio>
#include <string>
#include <vector>

#include "bench_utils.hpp"
#include "katana/core/serde.hpp"

using bench_util::clobber_memory;
using bench_util::do_not_optimize;

namespace {

template <typename Fn> void bench(const char* name, int iterations, Fn&& fn) {
    for (int i = 0; i < iterations / 10; ++i)
        fn();
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i)
        fn();
    auto end = std::chrono::high_resolution_clock::now();
    double ns = std::chrono::duration<double, std::nano>(end - start).count();
    double ns_per = ns / iterations;
    double ops = iterations / (ns / 1e9);
    if (ns_per < 1000.0) {
        std::printf("  %-45s %8.1f ns    %12.0f ops/sec\n", name, ns_per, ops);
    } else {
        std::printf("  %-45s %8.2f us    %12.0f ops/sec\n", name, ns_per / 1000.0, ops);
    }
}

} // namespace

int main() {
    constexpr int N = 1'000'000;

    std::printf("=== Serialize Benchmark ===\n\n");

    // --- escape_json_string ---
    std::printf("--- String Escaping Performance ---\n");
    {
        std::string clean = "simple_value_without_special_chars";
        bench("escape_json_string (clean, return)", N, [&] {
            auto v = katana::serde::escape_json_string(clean);
            do_not_optimize(v);
        });
    }
    {
        std::string clean = "simple_value_without_special_chars";
        std::string buf;
        buf.reserve(64);
        bench("escape_json_string_into (clean, append)", N, [&] {
            buf.clear();
            katana::serde::escape_json_string_into(clean, buf);
            clobber_memory();
        });
    }
    {
        std::string dirty = "has \"quotes\" and \nnewline \t tab";
        bench("escape_json_string (dirty, return)", N, [&] {
            auto v = katana::serde::escape_json_string(dirty);
            do_not_optimize(v);
        });
    }
    {
        std::string dirty = "has \"quotes\" and \nnewline \t tab";
        std::string buf;
        buf.reserve(64);
        bench("escape_json_string_into (dirty, append)", N, [&] {
            buf.clear();
            katana::serde::escape_json_string_into(dirty, buf);
            clobber_memory();
        });
    }

    // --- SIMD scan performance ---
    std::printf("\n--- SIMD Scan Performance ---\n");
    {
        std::string s16(16, 'a');
        bench("needs_json_escaping (16 byte clean)", N, [&] {
            do_not_optimize(katana::serde::needs_json_escaping(s16));
        });
    }
    {
        std::string s64(64, 'a');
        bench("needs_json_escaping (64 byte clean)", N, [&] {
            do_not_optimize(katana::serde::needs_json_escaping(s64));
        });
    }
    {
        std::string s256(256, 'a');
        bench("needs_json_escaping (256 byte clean)", N, [&] {
            do_not_optimize(katana::serde::needs_json_escaping(s256));
        });
    }
    {
        std::string s64(64, 'a');
        s64[63] = '\\'; // escape at end
        bench("needs_json_escaping (64 byte, escape at end)", N, [&] {
            do_not_optimize(katana::serde::needs_json_escaping(s64));
        });
    }
    {
        std::string s64(64, 'a');
        s64[0] = '\\'; // escape at start
        bench("needs_json_escaping (64 byte, escape at start)", N, [&] {
            do_not_optimize(katana::serde::needs_json_escaping(s64));
        });
    }

    // --- Manual JSON construction simulation ---
    std::printf("\n--- JSON Object Construction ---\n");
    {
        // Simulate serialize_into with embedded commas (no first flag)
        // Reuse buffer across iterations to measure serialization, not allocation
        std::string json;
        json.reserve(200);
        bench("serialize 5-field obj (embedded commas)", N, [&] {
            json.clear();
            json.append("{\"name\":");
            json.push_back('"');
            katana::serde::escape_json_string_into("John Doe", json);
            json.push_back('"');
            json.append(",\"email\":");
            json.push_back('"');
            katana::serde::escape_json_string_into("john@example.com", json);
            json.push_back('"');
            json.append(",\"age\":");
            json.append("30");
            json.append(",\"active\":");
            json.append("true");
            json.append(",\"role\":");
            json.push_back('"');
            json.append("admin");
            json.push_back('"');
            json.push_back('}');
            do_not_optimize(json.data());
        });
    }
    {
        // Array of 100 integers — write directly into a flat char buffer
        // to avoid per-element std::string operations
        char flat[512];
        bench("serialize array 100 ints (to_chars)", N / 10, [&] {
            char* p = flat;
            *p++ = '[';
            for (int i = 0; i < 100; ++i) {
                if (i > 0)
                    *p++ = ',';
                auto [end, ec] = std::to_chars(p, flat + sizeof(flat), i);
                p = end;
            }
            *p++ = ']';
            do_not_optimize(flat);
            do_not_optimize(p);
        });
    }
    {
        // Optimized array serialization using lookup tables
        std::array<int, 100> arr;
        for (int i = 0; i < 100; ++i) {
            arr[i] = i;
        }
        std::string buf;
        buf.reserve(512);
        bench("serialize array 100 ints (optimized)", N / 10, [&] {
            buf.clear();
            katana::serde::serialize_int_array_into(arr.data(), arr.size(), buf);
            do_not_optimize(buf.data());
        });
    }
    {
        // Large array serialization test
        std::array<int, 1000> arr;
        for (int i = 0; i < 1000; ++i) {
            arr[i] = i * 7 + 13; // Mix of different digit counts
        }
        std::string buf;
        buf.reserve(8192);
        bench("serialize array 1000 ints (optimized)", N / 100, [&] {
            buf.clear();
            katana::serde::serialize_int_array_into(arr.data(), arr.size(), buf);
            do_not_optimize(buf.data());
        });
    }
    {
        // Very large array (10000 ints)
        std::vector<int> arr(10000);
        for (int i = 0; i < 10000; ++i) {
            arr[i] = i * 13 + 7;
        }
        std::string buf;
        buf.reserve(65536);
        bench("serialize array 10000 ints (optimized)", N / 1000, [&] {
            buf.clear();
            katana::serde::serialize_int_array_into(arr.data(), arr.size(), buf);
            do_not_optimize(buf.data());
        });
    }

    // --- Nested object simulation ---
    std::printf("\n--- Nested Object Performance ---\n");
    {
        // Simulate deeply nested object (5 levels)
        std::string json;
        json.reserve(4096);
        bench("serialize nested obj (5 levels deep)", N / 10, [&] {
            json.clear();
            json.append("{\"level1\":{\"level2\":{\"level3\":{\"level4\":{\"level5\":");
            json.push_back('"');
            katana::serde::escape_json_string_into("deep_value", json);
            json.push_back('"');
            json.append("}}}}}");
            do_not_optimize(json.data());
        });
    }
    {
        // Object with many fields (20 fields)
        std::string json;
        json.reserve(2048);
        bench("serialize 20-field obj", N / 10, [&] {
            json.clear();
            json.push_back('{');
            for (int i = 0; i < 20; ++i) {
                if (i > 0)
                    json.push_back(',');
                json.append("\"field");
                json.append(std::to_string(i));
                json.append("\":\"value");
                json.append(std::to_string(i));
                json.push_back('"');
            }
            json.push_back('}');
            do_not_optimize(json.data());
        });
    }
    {
        // Array of nested objects (100 objects, each with 3 fields)
        std::string json;
        json.reserve(16384);
        bench("serialize 100 nested objects", N / 100, [&] {
            json.clear();
            json.push_back('[');
            for (int i = 0; i < 100; ++i) {
                if (i > 0)
                    json.push_back(',');
                json.append("{\"id\":");
                json.append(std::to_string(i));
                json.append(",\"name\":\"item");
                json.append(std::to_string(i));
                json.append("\",\"active\":true}");
            }
            json.push_back(']');
            do_not_optimize(json.data());
        });
    }

    // --- Worst-case scenarios ---
    std::printf("\n--- Worst-Case Scenarios ---\n");
    {
        // Very long string (10KB clean)
        std::string long_clean(10240, 'a');
        std::string buf;
        buf.reserve(long_clean.size() + 128);
        bench("escape_json_string (10KB clean)", N / 100, [&] {
            buf.clear();
            katana::serde::escape_json_string_into(long_clean, buf);
            clobber_memory();
        });
    }
    {
        // Very long string with heavy escaping (10KB all special chars)
        std::string heavy_escape;
        heavy_escape.reserve(10240);
        for (int i = 0; i < 2560; ++i) {
            heavy_escape.append("\"\\");
            heavy_escape.push_back('\n');
            heavy_escape.push_back('\t');
        }
        std::string buf;
        buf.reserve(heavy_escape.size() * 2 + 128);
        bench("escape_json_string (10KB heavy escaping)", N / 100, [&] {
            buf.clear();
            katana::serde::escape_json_string_into(heavy_escape, buf);
            clobber_memory();
        });
    }
    {
        // String with escape at every 16th byte (SIMD boundary stress)
        std::string simd_stress(1024, 'x');
        for (size_t i = 15; i < simd_stress.size(); i += 16) {
            simd_stress[i] = '"';
        }
        std::string buf;
        buf.reserve(simd_stress.size() * 2);
        bench("escape_json_string (escape at SIMD boundary)", N / 10, [&] {
            buf.clear();
            katana::serde::escape_json_string_into(simd_stress, buf);
            clobber_memory();
        });
    }
    {
        // needs_json_escaping on 1KB clean (hot path)
        std::string s1k(1024, 'a');
        bench("needs_json_escaping (1KB clean)", N, [&] {
            do_not_optimize(katana::serde::needs_json_escaping(s1k));
        });
    }
    {
        // needs_json_escaping on 1KB with escape at very end
        std::string s1k(1024, 'a');
        s1k[1023] = '\\';
        bench("needs_json_escaping (1KB, escape at end)", N, [&] {
            do_not_optimize(katana::serde::needs_json_escaping(s1k));
        });
    }

    // --- Cold cache vs hot cache comparison ---
    std::printf("\n--- Cold vs Hot Cache ---\n");
    {
        // Hot cache: same string repeated
        std::string hot_str = "The quick brown fox jumps over the lazy dog";
        std::string buf;
        buf.reserve(128);
        bench("escape hot (same string repeated)", N, [&] {
            buf.clear();
            katana::serde::escape_json_string_into(hot_str, buf);
            clobber_memory();
        });
    }
    {
        // Cold cache simulation: different strings each time
        std::vector<std::string> cold_strs;
        cold_strs.reserve(10000);
        for (int i = 0; i < 10000; ++i) {
            cold_strs.push_back("string_value_" + std::to_string(i) + "_with_some_extra_data");
        }
        std::string buf;
        buf.reserve(128);
        int idx = 0;
        bench("escape cold (different strings)", N / 10, [&] {
            buf.clear();
            katana::serde::escape_json_string_into(cold_strs[idx % cold_strs.size()], buf);
            ++idx;
            clobber_memory();
        });
    }

    std::printf("\n=== Benchmark Complete ===\n");
    return 0;
}
