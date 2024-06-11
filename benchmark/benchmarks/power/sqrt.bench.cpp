/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include "sqrt.bench.hpp"

// NOLINTBEGIN


BENCHMARK(BM_power_sqrt_std)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_ccm)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_rand_int_std)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_rand_int_ccm)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_rand_double_std)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_power_sqrt_rand_double_ccm)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK_MAIN();

// NOLINTEND