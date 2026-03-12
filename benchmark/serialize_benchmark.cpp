// benchmark/serialize_benchmark.cpp
#include <array>
#include <charconv>
#include <chrono>
#include <cstdio>
#include <string>
#include <type_traits>
#include <vector>

#include "bench_utils.hpp"
#include "katana/core/serde.hpp"

using bench_util::build_profile_dataset;
using bench_util::clobber_memory;
using bench_util::do_not_optimize;
using bench_util::profile_mix;

namespace {

template <typename Fn> void bench(const char* name, int iterations, Fn&& fn) {
    using invoke_result = std::invoke_result_t<Fn&>;
    constexpr bool returns_bytes = std::is_same_v<invoke_result, size_t>;
    constexpr int max_rounds = 64;
    constexpr int min_rounds = 3;
    constexpr double min_measure_seconds =
        0.20; // Keep ultra-fast tests above timer/frequency noise.

    for (int i = 0; i < iterations / 10; ++i)
        if constexpr (returns_bytes) {
            do_not_optimize(fn());
        } else {
            fn();
        }

    std::vector<double> round_ns_per;
    round_ns_per.reserve(max_rounds);
    std::vector<double> round_bytes_per_op;
    if constexpr (returns_bytes) {
        round_bytes_per_op.reserve(max_rounds);
    }

    double measured_seconds = 0.0;
    for (int round = 0; round < max_rounds; ++round) {
        auto round_start = std::chrono::high_resolution_clock::now();
        size_t round_bytes = 0;
        for (int i = 0; i < iterations; ++i)
            if constexpr (returns_bytes) {
                round_bytes += fn();
            } else {
                fn();
            }

        auto round_end = std::chrono::high_resolution_clock::now();
        const double round_ns =
            std::chrono::duration<double, std::nano>(round_end - round_start).count();
        round_ns_per.push_back(round_ns / static_cast<double>(iterations));
        measured_seconds += std::chrono::duration<double>(round_end - round_start).count();

        if constexpr (returns_bytes) {
            round_bytes_per_op.push_back(static_cast<double>(round_bytes) /
                                         static_cast<double>(iterations));
        }

        if (measured_seconds >= min_measure_seconds && (round + 1) >= min_rounds) {
            break;
        }
    }

    if (round_ns_per.empty()) {
        return;
    }

    std::sort(round_ns_per.begin(), round_ns_per.end());
    const size_t mid = round_ns_per.size() / 2;
    const double ns_per = (round_ns_per.size() % 2 == 0)
                              ? (round_ns_per[mid - 1] + round_ns_per[mid]) * 0.5
                              : round_ns_per[mid];
    const double ops = 1e9 / ns_per;

    double bytes_per_sec = 0.0;
    if constexpr (returns_bytes) {
        std::sort(round_bytes_per_op.begin(), round_bytes_per_op.end());
        const size_t bytes_mid = round_bytes_per_op.size() / 2;
        const double bytes_per_op =
            (round_bytes_per_op.size() % 2 == 0)
                ? (round_bytes_per_op[bytes_mid - 1] + round_bytes_per_op[bytes_mid]) * 0.5
                : round_bytes_per_op[bytes_mid];
        bytes_per_sec = bytes_per_op * ops;
    }
    if (ns_per < 1000.0) {
        if constexpr (returns_bytes) {
            std::printf("  %-45s %8.1f ns    %12.0f ops/sec    %14.0f bytes/sec\n",
                        name,
                        ns_per,
                        ops,
                        bytes_per_sec);
        } else {
            std::printf("  %-45s %8.1f ns    %12.0f ops/sec\n", name, ns_per, ops);
        }
    } else {
        if constexpr (returns_bytes) {
            std::printf("  %-45s %8.2f us    %12.0f ops/sec    %14.0f bytes/sec\n",
                        name,
                        ns_per / 1000.0,
                        ops,
                        bytes_per_sec);
        } else {
            std::printf("  %-45s %8.2f us    %12.0f ops/sec\n", name, ns_per / 1000.0, ops);
        }
    }
}

struct serialization_case {
    std::string name;
    std::string email;
    std::string note;
    int age = 0;
    bool active = true;

    size_t bytes() const noexcept { return name.size() + email.size() + note.size() + sizeof(age); }
};

void serialize_profiled_object_into(const serialization_case& c, std::string& out) {
    out.clear();
    out.append("{\"name\":\"");
    katana::serde::escape_json_string_into(c.name, out);
    out.append("\",\"email\":\"");
    katana::serde::escape_json_string_into(c.email, out);
    out.append("\",\"note\":\"");
    katana::serde::escape_json_string_into(c.note, out);
    out.append("\",\"age\":");
    out.append(std::to_string(c.age));
    out.append(",\"active\":");
    out.append(c.active ? "true" : "false");
    out.push_back('}');
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

    std::printf("\n--- Profiled Serialization Workloads (seeded datasets) ---\n");
    constexpr size_t profile_dataset_size = 4096;
    constexpr uint32_t profile_seed = 0x515EED42;

    const std::vector<std::string> escape_best_pool = {
        "clean_ascii_token",
        "simple_username_123",
        "hello_world",
    };
    const std::vector<std::string> escape_typical_pool = {
        "Name \"Alice\"",
        "Line1\\nLine2",
        "Tab\\tSeparated",
        "Path C:\\\\tmp\\\\file.txt",
    };
    const std::vector<std::string> escape_hard_pool = {
        "\"\\n\\t\"\\n\\t\"\\n\\t",
        std::string(2048, '"'),
        std::string(2048, '\\'),
        std::string(1024, '\n'),
    };

    const auto escape_best_dataset = build_profile_dataset(profile_dataset_size,
                                                           profile_seed + 1,
                                                           escape_best_pool,
                                                           escape_typical_pool,
                                                           escape_hard_pool,
                                                           profile_mix{1.0, 0.0, 0.0});
    const auto escape_mixed_dataset = build_profile_dataset(profile_dataset_size,
                                                            profile_seed + 2,
                                                            escape_best_pool,
                                                            escape_typical_pool,
                                                            escape_hard_pool,
                                                            profile_mix{0.75, 0.20, 0.05});
    const auto escape_hard_dataset = build_profile_dataset(profile_dataset_size,
                                                           profile_seed + 3,
                                                           escape_best_pool,
                                                           escape_typical_pool,
                                                           escape_hard_pool,
                                                           profile_mix{0.10, 0.20, 0.70});

    auto bench_escape_profile =
        [&](const char* label, const std::vector<std::string>& ds, int iterations) {
            std::string out;
            out.reserve(8192);
            size_t idx = 0;
            bench(label, iterations, [&]() -> size_t {
                const auto& input = ds[idx % ds.size()];
                ++idx;
                out.clear();
                katana::serde::escape_json_string_into(input, out);
                do_not_optimize(out.data());
                return input.size();
            });
        };

    bench_escape_profile("escape profile (best case)", escape_best_dataset, N);
    bench_escape_profile("escape profile (typical mixed)", escape_mixed_dataset, N);
    bench_escape_profile("escape profile (hard edge)", escape_hard_dataset, N / 10);

    const std::vector<serialization_case> object_best_pool = {
        {"Alice", "alice@example.com", "ok", 31, true},
        {"Bob", "bob@example.com", "ready", 27, false},
        {"Carol", "carol@example.com", "sync", 45, true},
    };
    const std::vector<serialization_case> object_typical_pool = {
        {"John Doe", "john.doe@example.org", "uses \"quotes\" sometimes", 38, true},
        {"Analyst", "ana@corp.dev", "line1\\nline2", 29, true},
        {"Operator", "ops@example.net", "path C:\\\\tmp\\\\job", 41, false},
    };
    const std::vector<serialization_case> object_hard_pool = {
        {std::string(256, 'n'), "edge@example.com", std::string(2048, '"'), 52, true},
        {"\n\t\"\\", "bad@@example", std::string(1024, '\\'), -1, false},
        {std::string(512, '\n'), "heavy@sample.org", std::string(1024, '\t'), 300, true},
    };

    const auto object_best_dataset = build_profile_dataset(profile_dataset_size,
                                                           profile_seed + 4,
                                                           object_best_pool,
                                                           object_typical_pool,
                                                           object_hard_pool,
                                                           profile_mix{1.0, 0.0, 0.0});
    const auto object_mixed_dataset = build_profile_dataset(profile_dataset_size,
                                                            profile_seed + 5,
                                                            object_best_pool,
                                                            object_typical_pool,
                                                            object_hard_pool,
                                                            profile_mix{0.70, 0.25, 0.05});
    const auto object_hard_dataset = build_profile_dataset(profile_dataset_size,
                                                           profile_seed + 6,
                                                           object_best_pool,
                                                           object_typical_pool,
                                                           object_hard_pool,
                                                           profile_mix{0.10, 0.20, 0.70});

    auto bench_object_profile =
        [&](const char* label, const std::vector<serialization_case>& ds, int iterations) {
            std::string out;
            out.reserve(32768);
            size_t idx = 0;
            bench(label, iterations, [&]() -> size_t {
                const auto& c = ds[idx % ds.size()];
                ++idx;
                serialize_profiled_object_into(c, out);
                do_not_optimize(out.data());
                return c.bytes();
            });
        };

    bench_object_profile("serialize object profile (best case)", object_best_dataset, N / 2);
    bench_object_profile("serialize object profile (typical mixed)", object_mixed_dataset, N / 2);
    bench_object_profile("serialize object profile (hard edge)", object_hard_dataset, N / 20);

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
        // Best-case baseline: direct write into flat buffer.
        char flat[512];
        bench("serialize array 100 ints (to_chars flat buffer)", N / 10, [&] {
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
        // Fair baseline against serialize_int_array_into: both target std::string.
        std::string buf;
        buf.reserve(512);
        bench("serialize array 100 ints (to_chars into string)", N / 10, [&] {
            buf.clear();
            buf.push_back('[');
            char local[16];
            for (int i = 0; i < 100; ++i) {
                if (i > 0) {
                    buf.push_back(',');
                }
                auto [end, ec] = std::to_chars(local, local + sizeof(local), i);
                buf.append(local, static_cast<size_t>(end - local));
            }
            buf.push_back(']');
            do_not_optimize(buf.data());
        });
    }
    {
        // Lookup-table serializer under the same target type (std::string).
        std::array<int, 100> arr;
        for (int i = 0; i < 100; ++i) {
            arr[i] = i;
        }
        std::string buf;
        buf.reserve(512);
        bench("serialize array 100 ints (lookup table into string)", N / 10, [&] {
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
        bench("serialize array 10000 ints (optimized)", N / 200, [&] {
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
        size_t idx = 0;
        constexpr size_t stride = 7919; // Prime stride to reduce modulo-pattern artifacts.
        bench("escape cold (different strings)", N, [&] {
            buf.clear();
            katana::serde::escape_json_string_into(cold_strs[idx % cold_strs.size()], buf);
            idx += stride;
            clobber_memory();
        });
    }

    std::printf("\n=== Benchmark Complete ===\n");
    return 0;
}
