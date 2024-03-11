/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <iostream>
#include <cmath>
#include "ccmath/ccmath.hpp"
#include <benchmark/benchmark.h>

static void BM_ccm_log2(benchmark::State& state) {
	for (auto _ : state) {
		benchmark::DoNotOptimize(ccm::log2(state.range(0)));
	}
}
BENCHMARK(BM_ccm_log2)->Arg(16)->Arg(256)->Arg(4096)->Arg(65536);

static void BM_ccm_log(benchmark::State& state) {
	for (auto _ : state) {
		benchmark::DoNotOptimize(ccm::log(state.range(0)));
	}
}
BENCHMARK(BM_ccm_log)->Arg(16)->Arg(256)->Arg(4096)->Arg(65536);

static void BM_std_log(benchmark::State& state) {
	for (auto _ : state) {
		benchmark::DoNotOptimize(std::log(state.range(0)));
	}
}
BENCHMARK(BM_std_log)->Arg(16)->Arg(256)->Arg(4096)->Arg(65536);

static void BM_std_log2(benchmark::State& state) {
	for (auto _ : state) {
		benchmark::DoNotOptimize(std::log2(state.range(0)));
	}
}
BENCHMARK(BM_std_log2)->Arg(16)->Arg(256)->Arg(4096)->Arg(65536);

BENCHMARK_MAIN();
