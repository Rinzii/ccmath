/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present ccmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
*/

#include "fdim.bench.hpp"

// NOLINTBEGIN

BENCHMARK(BM_basic_fdim_std)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_basic_fdim_ccmath)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_basic_fdim_rand_int_std)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_basic_fdim_rand_int_ccmath)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_basic_fdim_rand_double_std)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK(BM_basic_fdim_rand_double_ccmath)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

BENCHMARK_MAIN();

// NOLINTEND