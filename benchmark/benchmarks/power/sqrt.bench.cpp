/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include "sqrt.bench.hpp"

// NOLINTBEGIN


BENCHMARK(BM_power_sqrt_generic)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_simd)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_rand_int_generic)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_rand_int_simd)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_rand_double_generic)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_rand_double_simd)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK_MAIN();

// NOLINTEND