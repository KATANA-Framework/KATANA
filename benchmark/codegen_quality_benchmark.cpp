// benchmark/codegen_quality_benchmark.cpp
#include <array>
#include <chrono>
#include <cstdio>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "bench_utils.hpp"
#include "katana/core/arena.hpp"
#include "katana/core/serde.hpp"

using bench_util::build_profile_dataset;
using bench_util::clobber_memory;
using bench_util::do_not_optimize;
using bench_util::profile_mix;

// This benchmark measures codegen quality independently of the full API stack.
// It isolates: JSON parse, serialize, validation, and key dispatch.

namespace {

struct bench_result {
    const char* name;
    double ops_per_sec;
    double ns_per_op;
    double bytes_per_sec;
    bool has_data_rate;
    double tail_p95_ns;
    double tail_p99_ns;
    bool has_tail;
};

template <typename Fn>
bench_result run_bench(const char* name, int iterations, Fn&& fn, size_t tail_sample_rate = 0) {
    using invoke_result = std::invoke_result_t<Fn&>;
    constexpr bool returns_bytes = std::is_same_v<invoke_result, size_t>;
    constexpr int max_rounds = 64;
    constexpr int min_rounds = 3;
    constexpr double min_measure_seconds = 0.20; // Avoid ultra-short timing windows.

    // Warmup
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

    size_t total_bytes = 0;
    size_t sample_counter = 0;
    std::vector<double> tail_samples_ns;
    if (tail_sample_rate > 0) {
        tail_samples_ns.reserve(static_cast<size_t>(iterations) / tail_sample_rate + 8);
    }
    double measured_seconds = 0.0;
    for (int round = 0; round < max_rounds; ++round) {
        auto round_start = std::chrono::high_resolution_clock::now();
        size_t round_bytes = 0;
        if constexpr (returns_bytes) {
            if (tail_sample_rate == 0) {
                for (int i = 0; i < iterations; ++i) {
                    round_bytes += fn();
                }
            } else {
                for (int i = 0; i < iterations; ++i) {
                    if ((sample_counter % tail_sample_rate) == 0) {
                        auto t0 = std::chrono::high_resolution_clock::now();
                        const size_t bytes = fn();
                        auto t1 = std::chrono::high_resolution_clock::now();
                        round_bytes += bytes;
                        tail_samples_ns.push_back(
                            std::chrono::duration<double, std::nano>(t1 - t0).count());
                    } else {
                        round_bytes += fn();
                    }
                    ++sample_counter;
                }
            }
        } else {
            if (tail_sample_rate == 0) {
                for (int i = 0; i < iterations; ++i) {
                    fn();
                }
            } else {
                for (int i = 0; i < iterations; ++i) {
                    if ((sample_counter % tail_sample_rate) == 0) {
                        auto t0 = std::chrono::high_resolution_clock::now();
                        fn();
                        auto t1 = std::chrono::high_resolution_clock::now();
                        tail_samples_ns.push_back(
                            std::chrono::duration<double, std::nano>(t1 - t0).count());
                    } else {
                        fn();
                    }
                    ++sample_counter;
                }
            }
        }
        auto round_end = std::chrono::high_resolution_clock::now();
        const double round_ns =
            std::chrono::duration<double, std::nano>(round_end - round_start).count();
        round_ns_per.push_back(round_ns / static_cast<double>(iterations));
        measured_seconds += std::chrono::duration<double>(round_end - round_start).count();

        if constexpr (returns_bytes) {
            round_bytes_per_op.push_back(static_cast<double>(round_bytes) /
                                         static_cast<double>(iterations));
            total_bytes += round_bytes;
        }

        if (measured_seconds >= min_measure_seconds && (round + 1) >= min_rounds) {
            break;
        }
    }

    if (round_ns_per.empty()) {
        return {name, 0.0, 0.0, 0.0, returns_bytes, 0.0, 0.0, false};
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
        do_not_optimize(total_bytes);
    }

    double tail_p95_ns = 0.0;
    double tail_p99_ns = 0.0;
    bool has_tail = !tail_samples_ns.empty();
    if (has_tail) {
        std::sort(tail_samples_ns.begin(), tail_samples_ns.end());
        const size_t p95_idx = (tail_samples_ns.size() * 95) / 100;
        const size_t p99_idx = (tail_samples_ns.size() * 99) / 100;
        tail_p95_ns = tail_samples_ns[std::min(p95_idx, tail_samples_ns.size() - 1)];
        tail_p99_ns = tail_samples_ns[std::min(p99_idx, tail_samples_ns.size() - 1)];
    }

    return {name, ops, ns_per, bytes_per_sec, returns_bytes, tail_p95_ns, tail_p99_ns, has_tail};
}

void print_result(const bench_result& r) {
    const auto print_tail = [&]() {
        if (r.has_tail) {
            std::printf("    p95: %.1f ns    p99: %.1f ns", r.tail_p95_ns, r.tail_p99_ns);
        }
    };

    if (r.ns_per_op < 1000.0) {
        if (r.has_data_rate) {
            std::printf("  %-45s %8.1f ns    %12.0f ops/sec    %14.0f bytes/sec",
                        r.name,
                        r.ns_per_op,
                        r.ops_per_sec,
                        r.bytes_per_sec);
            print_tail();
            std::printf("\n");
        } else {
            std::printf("  %-45s %8.1f ns    %12.0f ops/sec", r.name, r.ns_per_op, r.ops_per_sec);
            print_tail();
            std::printf("\n");
        }
    } else {
        if (r.has_data_rate) {
            std::printf("  %-45s %8.2f us    %12.0f ops/sec    %14.0f bytes/sec",
                        r.name,
                        r.ns_per_op / 1000.0,
                        r.ops_per_sec,
                        r.bytes_per_sec);
            print_tail();
            std::printf("\n");
        } else {
            std::printf(
                "  %-45s %8.2f us    %12.0f ops/sec", r.name, r.ns_per_op / 1000.0, r.ops_per_sec);
            print_tail();
            std::printf("\n");
        }
    }
}

struct parse_input_case {
    std::string payload;
    bool expect_valid = true;

    size_t bytes() const noexcept { return payload.size(); }
};

struct validation_case {
    std::string username;
    std::string email;
    int age = 0;
    std::string role;
    bool expect_valid = true;

    size_t bytes() const noexcept {
        return username.size() + email.size() + role.size() + sizeof(age);
    }
};

bool validate_user_record(const validation_case& c) {
    const bool username_ok = c.username.size() >= 3 && c.username.size() <= 32;
    const bool email_ok = c.email.find('@') != std::string::npos &&
                          c.email.find('.') != std::string::npos && c.email.size() <= 128;
    const bool age_ok = c.age >= 0 && c.age <= 130;
    const bool role_ok = (c.role == "user" || c.role == "admin" || c.role == "moderator");
    return username_ok && email_ok && age_ok && role_ok;
}

template <typename ParseFn>
size_t run_parse_case(ParseFn&& parse, const parse_input_case& c, size_t& mismatches) {
    katana::serde::json_cursor cur{c.payload.data(), c.payload.data() + c.payload.size()};
    const auto parsed = parse(cur);

    bool ok = parsed.has_value();
    if (ok) {
        cur.skip_ws();
        ok = cur.eof();
    }
    if (ok != c.expect_valid) {
        ++mismatches;
    }
    do_not_optimize(parsed);
    return c.bytes();
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
    {
        // Mixed inputs reduce branch-predictor bias from a single repeated literal.
        std::array<std::string_view, 4> bool_cases = {"true", "false", "\"true\"", "\"false\""};
        int idx = 0;
        auto r = run_bench("parse_bool (mixed true/false/quoted)", N, [&] {
            const auto sv = bool_cases[static_cast<size_t>(idx) & 3];
            ++idx;
            katana::serde::json_cursor cur{sv.data(), sv.data() + sv.size()};
            auto v = katana::serde::parse_bool(cur);
            do_not_optimize(v);
        });
        print_result(r);
    }

    // --- Profiled parsing + validation datasets ---
    std::printf("\n--- Profiled Workloads (seeded datasets) ---\n");
    constexpr size_t profile_dataset_size = 4096;
    constexpr uint32_t profile_seed = 0xC0DEBEEF;

    const std::vector<parse_input_case> int_best_pool = {
        {"42", true},
        {"0", true},
        {"123456", true},
        {"-7", true},
    };
    const std::vector<parse_input_case> int_typical_pool = {
        {"  77", true},
        {"-12345", true},
        {"2147483647", true},
        {"9223372036854775807", true},
        {"15 trailing", false},
    };
    const std::vector<parse_input_case> int_hard_pool = {
        {"", false},
        {"--1", false},
        {"9223372036854775808", false},
        {"-9223372036854775809", false},
        {"184467440737095516161234", false},
        {"\t\n", false},
    };

    const auto int_best_dataset = build_profile_dataset(profile_dataset_size,
                                                        profile_seed + 1,
                                                        int_best_pool,
                                                        int_typical_pool,
                                                        int_hard_pool,
                                                        profile_mix{1.0, 0.0, 0.0});
    const auto int_mixed_dataset = build_profile_dataset(profile_dataset_size,
                                                         profile_seed + 2,
                                                         int_best_pool,
                                                         int_typical_pool,
                                                         int_hard_pool,
                                                         profile_mix{0.70, 0.25, 0.05});
    const auto int_hard_dataset = build_profile_dataset(profile_dataset_size,
                                                        profile_seed + 3,
                                                        int_best_pool,
                                                        int_typical_pool,
                                                        int_hard_pool,
                                                        profile_mix{0.10, 0.20, 0.70});

    auto bench_parse_int_profile = [&](const char* label, const std::vector<parse_input_case>& ds) {
        size_t idx = 0;
        size_t mismatches = 0;
        auto r = run_bench(
            label,
            N,
            [&]() -> size_t {
                const auto& c = ds[idx % ds.size()];
                ++idx;
                return run_parse_case(
                    [](katana::serde::json_cursor& cur) { return katana::serde::parse_int64(cur); },
                    c,
                    mismatches);
            },
            1024);
        do_not_optimize(mismatches);
        print_result(r);
    };

    bench_parse_int_profile("parse_int64 profile (best case)", int_best_dataset);
    bench_parse_int_profile("parse_int64 profile (typical mixed)", int_mixed_dataset);
    bench_parse_int_profile("parse_int64 profile (hard edge)", int_hard_dataset);

    const std::vector<parse_input_case> bool_best_pool = {
        {"true", true},
        {"false", true},
        {" true ", true},
    };
    const std::vector<parse_input_case> bool_typical_pool = {
        {"\"true\"", true},
        {"\"false\"", true},
        {"\" true \"", true},
        {" true ", true},
    };
    const std::vector<parse_input_case> bool_hard_pool = {
        {"TRUE", false},
        {"False", false},
        {"tru", false},
        {"null", false},
        {"\"false", false},
        {"\" true but with long trailing payload .................. \"", false},
        {std::string(128, 't'), false},
        {"  \"false\" garbage_suffix", false},
    };

    const auto bool_best_dataset = build_profile_dataset(profile_dataset_size,
                                                         profile_seed + 4,
                                                         bool_best_pool,
                                                         bool_typical_pool,
                                                         bool_hard_pool,
                                                         profile_mix{1.0, 0.0, 0.0});
    const auto bool_mixed_dataset = build_profile_dataset(profile_dataset_size,
                                                          profile_seed + 5,
                                                          bool_best_pool,
                                                          bool_typical_pool,
                                                          bool_hard_pool,
                                                          profile_mix{0.65, 0.25, 0.10});
    const auto bool_hard_dataset = build_profile_dataset(profile_dataset_size,
                                                         profile_seed + 6,
                                                         bool_best_pool,
                                                         bool_typical_pool,
                                                         bool_hard_pool,
                                                         profile_mix{0.10, 0.20, 0.70});

    auto bench_parse_bool_profile = [&](const char* label,
                                        const std::vector<parse_input_case>& ds) {
        size_t idx = 0;
        size_t mismatches = 0;
        auto r = run_bench(
            label,
            N,
            [&]() -> size_t {
                const auto& c = ds[idx % ds.size()];
                ++idx;
                return run_parse_case(
                    [](katana::serde::json_cursor& cur) { return katana::serde::parse_bool(cur); },
                    c,
                    mismatches);
            },
            1024);
        do_not_optimize(mismatches);
        print_result(r);
    };

    bench_parse_bool_profile("parse_bool profile (best case)", bool_best_dataset);
    bench_parse_bool_profile("parse_bool profile (typical mixed)", bool_mixed_dataset);
    bench_parse_bool_profile("parse_bool profile (hard edge)", bool_hard_dataset);

    const std::vector<validation_case> validation_best_pool = {
        {"alice", "alice@example.com", 31, "user", true},
        {"bob_admin", "bob@corp.dev", 44, "admin", true},
        {"mod_1", "mod1@forum.org", 28, "moderator", true},
    };
    const std::vector<validation_case> validation_typical_pool = {
        {"john_doe", "john.doe@sample.org", 37, "user", true},
        {"young_user", "young@sample.org", 18, "user", true},
        {"legacy-user", "legacy@old.io", 64, "moderator", true},
        {"qa", "qa@sample.org", 29, "user", false},
    };
    const std::vector<validation_case> validation_hard_pool = {
        {"x", "bademail", -1, "root", false},
        {"this_name_is_way_too_long_for_the_validator", "mail@ok.dev", 20, "user", false},
        {"edge", "edge@sample", 10, "user", false},
        {"normal", "normal@ok.dev", 200, "admin", false},
        {"hacker", "hack@example.com", 25, "superuser", false},
    };

    const auto validation_best_dataset = build_profile_dataset(profile_dataset_size,
                                                               profile_seed + 7,
                                                               validation_best_pool,
                                                               validation_typical_pool,
                                                               validation_hard_pool,
                                                               profile_mix{1.0, 0.0, 0.0});
    const auto validation_mixed_dataset = build_profile_dataset(profile_dataset_size,
                                                                profile_seed + 8,
                                                                validation_best_pool,
                                                                validation_typical_pool,
                                                                validation_hard_pool,
                                                                profile_mix{0.70, 0.25, 0.05});
    const auto validation_hard_dataset = build_profile_dataset(profile_dataset_size,
                                                               profile_seed + 9,
                                                               validation_best_pool,
                                                               validation_typical_pool,
                                                               validation_hard_pool,
                                                               profile_mix{0.10, 0.20, 0.70});

    auto bench_validation_profile =
        [&](const char* label, const std::vector<validation_case>& ds, int iterations) {
            size_t idx = 0;
            size_t mismatches = 0;
            auto r = run_bench(
                label,
                iterations,
                [&]() -> size_t {
                    const auto& c = ds[idx % ds.size()];
                    ++idx;
                    const bool ok = validate_user_record(c);
                    if (ok != c.expect_valid) {
                        ++mismatches;
                    }
                    do_not_optimize(ok);
                    return c.bytes();
                },
                1024);
            do_not_optimize(mismatches);
            print_result(r);
        };

    bench_validation_profile("validate_user profile (best case)", validation_best_dataset, N);
    bench_validation_profile("validate_user profile (typical mixed)", validation_mixed_dataset, N);
    bench_validation_profile("validate_user profile (hard edge)", validation_hard_dataset, N);

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
