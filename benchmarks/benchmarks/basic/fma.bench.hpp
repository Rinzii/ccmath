/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present ccmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
 */

#include "../../helpers/randomizers.hpp"
#include <benchmark/benchmark.h>
#include <ccmath/ccmath.hpp>
#include <cmath>

namespace bm = benchmark;

// NOLINTBEGIN

static void BM_basic_fma_std(benchmark::State& state) {
	for ([[maybe_unused]] auto _ : state) {
		benchmark::DoNotOptimize(std::fma(state.range(0), state.range(1), state.range(2)));
	}
	state.SetComplexityN(state.range(0));
}

static void BM_basic_fma_ccmath(benchmark::State& state) {
	for ([[maybe_unused]] auto _ : state) {
		benchmark::DoNotOptimize(ccm::fma(state.range(0), state.range(1), state.range(2)));
	}
	state.SetComplexityN(state.range(0));
}

static void BM_basic_fma_rand_int_std(benchmark::State& state) {
	ccm::bench::Randomizer ran;
	auto randomIntegers = ran.generateRandomIntegers(state.range(0));
	auto randomIntegers2 = ran.generateRandomIntegers(state.range(0));
	auto randomIntegers3 = ran.generateRandomIntegers(state.range(0));
	while (state.KeepRunning()) {
		for (size_t i = 0; i < randomIntegers.size(); ++i) {
			benchmark::DoNotOptimize(std::fma(randomIntegers[i], randomIntegers2[i], randomIntegers3[i]));
		}
	}
	state.SetComplexityN(state.range(0));
}

static void BM_basic_fma_rand_int_ccmath(benchmark::State& state) {
	ccm::bench::Randomizer ran;
	auto randomIntegers = ran.generateRandomIntegers(state.range(0));
	auto randomIntegers2 = ran.generateRandomIntegers(state.range(0));
	auto randomIntegers3 = ran.generateRandomIntegers(state.range(0));
	while (state.KeepRunning()) {
		for (size_t i = 0; i < randomIntegers.size(); ++i) {
			benchmark::DoNotOptimize(ccm::fma(randomIntegers[i], randomIntegers2[i], randomIntegers3[i]));
		}
	}
	state.SetComplexityN(state.range(0));
}

static void BM_basic_fma_rand_double_std(benchmark::State& state) {
	ccm::bench::Randomizer ran;
	auto randomDoubles = ran.generateRandomDoubles(state.range(0));
	auto randomDoubles2 = ran.generateRandomDoubles(state.range(0));
	auto randomDoubles3 = ran.generateRandomDoubles(state.range(0));
	while (state.KeepRunning()) {
		for (size_t i = 0; i < randomDoubles.size(); ++i) {
			benchmark::DoNotOptimize(std::fma(randomDoubles[i], randomDoubles2[i], randomDoubles3[i]));
		}
	}
	state.SetComplexityN(state.range(0));
}

static void BM_basic_fma_rand_double_ccmath(benchmark::State& state) {
	ccm::bench::Randomizer ran;
	auto randomDoubles = ran.generateRandomDoubles(state.range(0));
	auto randomDoubles2 = ran.generateRandomDoubles(state.range(0));
	auto randomDoubles3 = ran.generateRandomDoubles(state.range(0));
	while (state.KeepRunning()) {
		for (size_t i = 0; i < randomDoubles.size(); ++i) {
			benchmark::DoNotOptimize(ccm::fma(randomDoubles[i], randomDoubles2[i], randomDoubles3[i]));
		}
	}
	state.SetComplexityN(state.range(0));
}

// NOLINTEND