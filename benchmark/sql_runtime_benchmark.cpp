#include "bench_utils.hpp"
#include "generated_sql_repository.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace {

class fake_executor final : public katana::sql::executor {
public:
    katana::result<katana::sql::rows> query(std::string_view statement_name,
                                            std::string_view,
                                            const katana::sql::parameters&) override {
        if (statement_name == "get_user") {
            return point_lookup_rows_;
        }
        if (statement_name == "list_users") {
            return list_rows_;
        }
        return katana::sql::rows{};
    }

    katana::result<katana::sql::exec_result> exec(std::string_view,
                                                  std::string_view,
                                                  const katana::sql::parameters&) override {
        return katana::sql::exec_result{.affected_rows = 1};
    }

    katana::result<void> query_each(std::string_view statement_name,
                                    std::string_view,
                                    const katana::sql::parameters&,
                                    katana::sql::row_handler handler) override {
        const katana::sql::rows* source = nullptr;
        if (statement_name == "get_user") {
            source = &point_lookup_rows_;
        } else if (statement_name == "list_users") {
            source = &list_rows_;
        }
        if (source == nullptr) {
            return {};
        }

        std::vector<katana::sql::cell_view> scratch;
        for (const auto& input_row : *source) {
            scratch.clear();
            scratch.reserve(input_row.size());
            for (std::size_t index = 0; index < input_row.size(); ++index) {
                const auto& cell = input_row.at(index);
                if (cell.has_value()) {
                    scratch.push_back(katana::sql::cell_view::borrowed(*cell));
                } else {
                    scratch.push_back(katana::sql::cell_view::null());
                }
            }
            auto status = handler(katana::sql::row_view(scratch.data(), scratch.size()));
            if (!status) {
                return std::unexpected(status.error());
            }
        }
        return {};
    }

    void seed() {
        point_lookup_rows_ = {
            katana::sql::row{
                {{"id", katana::sql::cell(std::string("42"))},
                 {"name", katana::sql::cell(std::string("Ada"))},
                 {"active", katana::sql::cell(std::string("true"))}}},
        };

        list_rows_.clear();
        list_rows_.reserve(32);
        for (int64_t index = 0; index < 32; ++index) {
            list_rows_.push_back(katana::sql::row{
                {{"id", katana::sql::cell(std::to_string(index))},
                 {"name", katana::sql::cell(std::string("user_") + std::to_string(index))}},
            });
        }
    }

private:
    katana::sql::rows point_lookup_rows_;
    katana::sql::rows list_rows_;
};

struct bench_stats {
    double mean_ns = 0.0;
    double p95_ns = 0.0;
    double p99_ns = 0.0;
};

template <typename Fn> bench_stats run_bench(int samples, int batch_size, Fn&& fn) {
    std::vector<double> samples_ns;
    samples_ns.reserve(samples);

    for (int i = 0; i < samples / 10; ++i) {
        std::uint64_t warmup = 0;
        for (int j = 0; j < batch_size; ++j) {
            warmup += static_cast<std::uint64_t>(fn());
        }
        bench_util::do_not_optimize(warmup);
    }

    double total_ns = 0.0;
    for (int i = 0; i < samples; ++i) {
        const auto start = std::chrono::steady_clock::now();
        std::uint64_t batch_total = 0;
        for (int j = 0; j < batch_size; ++j) {
            batch_total += static_cast<std::uint64_t>(fn());
        }
        const auto end = std::chrono::steady_clock::now();
        bench_util::do_not_optimize(batch_total);
        const auto elapsed_ns =
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        const double sample = static_cast<double>(elapsed_ns) / static_cast<double>(batch_size);
        samples_ns.push_back(sample);
        total_ns += sample;
    }

    std::sort(samples_ns.begin(), samples_ns.end());
    const auto percentile = [&](double q) -> double {
        const std::size_t index =
            static_cast<std::size_t>(q * static_cast<double>(samples_ns.size() - 1));
        return samples_ns[index];
    };

    return bench_stats{
        .mean_ns = total_ns / static_cast<double>(samples),
        .p95_ns = percentile(0.95),
        .p99_ns = percentile(0.99),
    };
}

void print_result(std::string_view name, const bench_stats& stats) {
    std::printf("  %-40.*s %8.2f ns    %12.0f ops/sec    p95: %.0f ns    p99: %.0f ns\n",
                static_cast<int>(name.size()),
                name.data(),
                stats.mean_ns,
                1e9 / stats.mean_ns,
                stats.p95_ns,
                stats.p99_ns);
}

} // namespace

int main() {
    fake_executor executor;
    executor.seed();
    katana::sql::generated::generated_repository repo(executor);
    constexpr int samples = 4000;

    const auto get_user_stats = run_bench(samples, 256, [&]() -> int64_t {
        auto result = repo.get_user(42);
        if (!result || !result->has_value() || !result->value().id.has_value()) {
            return -1;
        }
        return *result->value().id;
    });

    const auto list_users_stats = run_bench(samples, 16, [&]() -> std::size_t {
        auto result = repo.list_users(true);
        if (!result) {
            return 0;
        }
        return result->size();
    });

    const auto exec_stats = run_bench(samples, 512, [&]() -> std::size_t {
        auto result = repo.touch_user(42);
        if (!result) {
            return 0;
        }
        return result->affected_rows;
    });

    std::printf("SQL Runtime Benchmarks\n");
    print_result("generated repo get_user (1 row)", get_user_stats);
    print_result("generated repo list_users (32 rows)", list_users_stats);
    print_result("generated repo touch_user (exec)", exec_stats);
    return 0;
}
