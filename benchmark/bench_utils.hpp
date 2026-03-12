// benchmark/bench_utils.hpp
// Utility to prevent compiler from optimizing away benchmark operations.
//
// Usage: replace `(void)result;` with `do_not_optimize(result);`
// This forces the compiler to materialize the value without actually
// using it, preventing dead-code elimination at -O3.
#pragma once

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <random>
#include <vector>

namespace bench_util {

// Prevents the compiler from optimizing away a computed value.
// The value is forced into a register/memory location but not actually used.
template <typename T> inline void do_not_optimize(T const& val) {
    asm volatile("" : : "r,m"(val) : "memory");
}

// Mutable variant: additionally prevents the compiler from assuming
// the value hasn't changed after the asm statement.
template <typename T> inline void do_not_optimize(T& val) {
    asm volatile("" : "+r,m"(val) : : "memory");
}

// Prevents the compiler from reordering memory operations across this barrier.
inline void clobber_memory() {
    asm volatile("" : : : "memory");
}

struct profile_mix {
    double best = 0.70;
    double typical = 0.20;
    double hard = 0.10;
};

template <typename T>
std::vector<T> build_profile_dataset(size_t count,
                                     uint32_t seed,
                                     const std::vector<T>& best_pool,
                                     const std::vector<T>& typical_pool,
                                     const std::vector<T>& hard_pool,
                                     profile_mix mix = {}) {
    std::vector<T> out;
    out.reserve(count);
    if (count == 0) {
        return out;
    }

    const std::vector<T>* pools[3] = {&best_pool, &typical_pool, &hard_pool};
    double weights[3] = {mix.best, mix.typical, mix.hard};
    for (int i = 0; i < 3; ++i) {
        if (pools[i]->empty() || weights[i] < 0.0) {
            weights[i] = 0.0;
        }
    }

    const double total_weight = weights[0] + weights[1] + weights[2];
    if (total_weight <= 0.0) {
        for (const auto* pool : pools) {
            if (!pool->empty()) {
                std::mt19937 rng(seed);
                std::uniform_int_distribution<size_t> pick(0, pool->size() - 1);
                for (size_t i = 0; i < count; ++i) {
                    out.push_back((*pool)[pick(rng)]);
                }
                return out;
            }
        }
        return out;
    }

    weights[0] /= total_weight;
    weights[1] /= total_weight;
    weights[2] /= total_weight;

    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> which_pool(0.0, 1.0);
    std::uniform_int_distribution<size_t> pick_best(0,
                                                    best_pool.empty() ? 0 : best_pool.size() - 1);
    std::uniform_int_distribution<size_t> pick_typical(
        0, typical_pool.empty() ? 0 : typical_pool.size() - 1);
    std::uniform_int_distribution<size_t> pick_hard(0,
                                                    hard_pool.empty() ? 0 : hard_pool.size() - 1);

    for (size_t i = 0; i < count; ++i) {
        const double token = which_pool(rng);
        if (token < weights[0] && !best_pool.empty()) {
            out.push_back(best_pool[pick_best(rng)]);
            continue;
        }
        if (token < weights[0] + weights[1] && !typical_pool.empty()) {
            out.push_back(typical_pool[pick_typical(rng)]);
            continue;
        }
        if (!hard_pool.empty()) {
            out.push_back(hard_pool[pick_hard(rng)]);
        } else if (!typical_pool.empty()) {
            out.push_back(typical_pool[pick_typical(rng)]);
        } else {
            out.push_back(best_pool[pick_best(rng)]);
        }
    }

    return out;
}

} // namespace bench_util
