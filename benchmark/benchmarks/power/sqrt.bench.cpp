/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include "sqrt.bench.hpp"

// NOLINTBEGIN

// Regular

BENCHMARK(BM_power_sqrt_std)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_ccm)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_rand_int_std)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_rand_int_ccm)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_rand_double_std)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_rand_double_ccm)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

// Runtime
#if !(defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__)) // GCC 6.1+ has constexpr sqrt builtins.

BENCHMARK(BM_power_sqrt_std_rt)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_ccm_rt)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_rand_int_std_rt)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_rand_int_ccm_rt)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_rand_double_std_rt)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_rand_double_ccm_rt)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

#endif

BENCHMARK_MAIN();

// NOLINTEND