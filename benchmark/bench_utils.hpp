// benchmark/bench_utils.hpp
// Utility to prevent compiler from optimizing away benchmark operations.
//
// Usage: replace `(void)result;` with `do_not_optimize(result);`
// This forces the compiler to materialize the value without actually
// using it, preventing dead-code elimination at -O3.
#pragma once

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

} // namespace bench_util
