// benchmark/codegen_quality_benchmark.cpp
#include <chrono>
#include <cstdio>
#include <string>
#include <string_view>

#include "bench_utils.hpp"
#include "katana/core/arena.hpp"
#include "katana/core/serde.hpp"

using bench_util::clobber_memory;
using bench_util::do_not_optimize;

// This benchmark measures codegen quality independently of the full API stack.
// It isolates: JSON parse, serialize, validation, and key dispatch.

namespace {

struct bench_result {
    const char* name;
    double ops_per_sec;
    double ns_per_op;
};

template <typename Fn> bench_result run_bench(const char* name, int iterations, Fn&& fn) {
    // Warmup
    for (int i = 0; i < iterations / 10; ++i)
        fn();

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i)
        fn();
    auto end = std::chrono::high_resolution_clock::now();

    double ns = std::chrono::duration<double, std::nano>(end - start).count();
    double ns_per = ns / iterations;
    double ops = iterations / (ns / 1e9);
    return {name, ops, ns_per};
}

void print_result(const bench_result& r) {
    if (r.ns_per_op < 1000.0) {
        std::printf("  %-45s %8.1f ns    %12.0f ops/sec\n", r.name, r.ns_per_op, r.ops_per_sec);
    } else {
        std::printf(
            "  %-45s %8.2f us    %12.0f ops/sec\n", r.name, r.ns_per_op / 1000.0, r.ops_per_sec);
    }
}

} // namespace

int main() {
    constexpr int N = 1'000'000;

    std::printf("=== Codegen Quality Benchmark ===\n\n");

    // --- JSON Parse: serde primitives ---
    std::printf("--- JSON Parse: Primitives ---\n");
    {
        std::string json_int = "42";
        auto r = run_bench("parse_int64 (simple)", N, [&] {
            katana::serde::json_cursor cur{json_int.data(), json_int.data() + json_int.size()};
            auto v = katana::serde::parse_int64(cur);
            do_not_optimize(v);
        });
        print_result(r);
    }
    {
        std::string json_neg = "-12345";
        auto r = run_bench("parse_int64 (negative)", N, [&] {
            katana::serde::json_cursor cur{json_neg.data(), json_neg.data() + json_neg.size()};
            auto v = katana::serde::parse_int64(cur);
            do_not_optimize(v);
        });
        print_result(r);
    }
    {
        std::string json_dbl = "3.14159265358979";
        auto r = run_bench("parse_double", N, [&] {
            katana::serde::json_cursor cur{json_dbl.data(), json_dbl.data() + json_dbl.size()};
            auto v = katana::serde::parse_double(cur);
            do_not_optimize(v);
        });
        print_result(r);
    }
    {
        std::string json_bool = "true";
        auto r = run_bench("parse_bool (strict validation)", N, [&] {
            katana::serde::json_cursor cur{json_bool.data(), json_bool.data() + json_bool.size()};
            auto v = katana::serde::parse_bool(cur);
            do_not_optimize(v);
        });
        print_result(r);
    }

    // --- String escaping ---
    std::printf("\n--- String Escaping ---\n");
    {
        std::string clean = "hello world this is a normal ASCII string without escaping";
        auto r = run_bench("needs_json_escaping (clean, 58 chars)", N, [&] {
            auto v = katana::serde::needs_json_escaping(clean);
            do_not_optimize(v);
        });
        print_result(r);
    }
    {
        std::string dirty = "hello \"world\" with\nnewline";
        auto r = run_bench("needs_json_escaping (dirty, 25 chars)", N, [&] {
            auto v = katana::serde::needs_json_escaping(dirty);
            do_not_optimize(v);
        });
        print_result(r);
    }
    {
        std::string clean = "simple_username_123";
        std::string buf;
        buf.reserve(64);
        auto r = run_bench("escape_json_string (no-alloc path)", N, [&] {
            auto v = katana::serde::escape_json_string(clean);
            do_not_optimize(v);
        });
        print_result(r);
    }
    {
        std::string clean = "simple_username_123";
        std::string buf;
        buf.reserve(64);
        auto r = run_bench("escape_json_string_into (append)", N, [&] {
            buf.clear();
            katana::serde::escape_json_string_into(clean, buf);
            clobber_memory();
        });
        print_result(r);
    }

    // --- skip_value ---
    std::printf("\n--- skip_value ---\n");
    {
        std::string nested =
            R"({"key": "value with \"quotes\"", "nested": {"a": 1, "b": [1,2,3]}})";
        auto r = run_bench("skip_value (nested obj with strings)", N, [&] {
            katana::serde::json_cursor cur{nested.data(), nested.data() + nested.size()};
            cur.skip_value();
            clobber_memory();
        });
        print_result(r);
    }

    // --- Object parse with key dispatch ---
    std::printf("\n--- Key Dispatch ---\n");
    {
        // 3-field object (linear chain)
        std::string json3 = R"({"name":"John","age":30,"active":true})";
        auto r = run_bench("3-field object parse (linear)", N, [&] {
            katana::serde::json_cursor cur{json3.data(), json3.data() + json3.size()};
            if (cur.try_object_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_object_end())
                        break;
                    auto key = cur.string();
                    if (!key || !cur.consume(':'))
                        break;
                    if (*key == "name") {
                        do_not_optimize(cur.string());
                    } else if (*key == "age") {
                        do_not_optimize(katana::serde::parse_int64(cur));
                    } else if (*key == "active") {
                        do_not_optimize(katana::serde::parse_bool(cur));
                    } else {
                        cur.skip_value();
                    }
                    cur.try_comma();
                }
            }
        });
        print_result(r);
    }
    {
        // 8-field object (length-switch)
        std::string json8 =
            R"({"id":1,"name":"John","email":"j@x.com","age":30,"role":"admin","bio":"test","zip":"12345","active":true})";
        auto r = run_bench("8-field object parse (length-switch)", N, [&] {
            katana::serde::json_cursor cur{json8.data(), json8.data() + json8.size()};
            if (cur.try_object_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_object_end())
                        break;
                    auto key = cur.string();
                    if (!key || !cur.consume(':'))
                        break;
                    switch (key->size()) {
                    case 2:
                        if (*key == "id") {
                            do_not_optimize(katana::serde::parse_int64(cur));
                        } else
                            cur.skip_value();
                        break;
                    case 3:
                        if (*key == "age") {
                            do_not_optimize(katana::serde::parse_int64(cur));
                        } else if (*key == "bio") {
                            do_not_optimize(cur.string());
                        } else if (*key == "zip") {
                            do_not_optimize(cur.string());
                        } else
                            cur.skip_value();
                        break;
                    case 4:
                        if (*key == "name") {
                            do_not_optimize(cur.string());
                        } else if (*key == "role") {
                            do_not_optimize(cur.string());
                        } else
                            cur.skip_value();
                        break;
                    case 5:
                        if (*key == "email") {
                            do_not_optimize(cur.string());
                        } else
                            cur.skip_value();
                        break;
                    case 6:
                        if (*key == "active") {
                            do_not_optimize(katana::serde::parse_bool(cur));
                        } else
                            cur.skip_value();
                        break;
                    default:
                        cur.skip_value();
                        break;
                    }
                    cur.try_comma();
                }
            }
        });
        print_result(r);
    }

    // --- Arena allocation ---
    std::printf("\n--- Arena Allocation ---\n");
    {
        auto r = run_bench("arena alloc+reset cycle (4KB)", N, [&] {
            katana::monotonic_arena arena(4096);
            void* p = arena.allocate(256, 8);
            do_not_optimize(p);
        });
        print_result(r);
    }

    std::printf("\n=== Benchmark Complete ===\n");
    return 0;
}
