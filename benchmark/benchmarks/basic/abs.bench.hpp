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


static void BM_basic_abs_std(benchmark::State& state) {
   for ([[maybe_unused]] auto _ : state) {
	   benchmark::DoNotOptimize(std::abs(state.range(0)));
   }
   state.SetComplexityN(state.range(0));
}

static void BM_basic_abs_ccmath(benchmark::State& state) {
   for ([[maybe_unused]] auto _ : state) {
	   benchmark::DoNotOptimize(ccm::abs(state.range(0)));
   }
   state.SetComplexityN(state.range(0));
}

static void BM_basic_abs_rand_int_std(benchmark::State& state) {
   ccm::bench::Randomizer ran;
   auto randomIntegers = ran.generateRandomIntegers(state.range(0));
   while (state.KeepRunning()) {
	   for (auto x : randomIntegers) {
		   benchmark::DoNotOptimize(std::abs(x));
	   }
   }
   state.SetComplexityN(state.range(0));
}

static void BM_basic_abs_rand_int_ccmath(benchmark::State& state) {
   ccm::bench::Randomizer ran;
   auto randomIntegers = ran.generateRandomIntegers(state.range(0));
   while (state.KeepRunning()) {
	   for (auto x : randomIntegers) {
		   benchmark::DoNotOptimize(ccm::abs(x));
	   }
   }
   state.SetComplexityN(state.range(0));
}

static void BM_basic_abs_rand_double_std(benchmark::State& state) {
   ccm::bench::Randomizer ran;
   auto randomDoubles = ran.generateRandomDoubles(state.range(0));
   while (state.KeepRunning()) {
	   for (auto x : randomDoubles) {
		   benchmark::DoNotOptimize(std::abs(x));
	   }
   }
   state.SetComplexityN(state.range(0));
}

static void BM_basic_abs_rand_double_ccmath(benchmark::State& state) {
   ccm::bench::Randomizer ran;
   auto randomDoubles = ran.generateRandomDoubles(state.range(0));
   while (state.KeepRunning()) {
	   for (auto x : randomDoubles) {
		   benchmark::DoNotOptimize(ccm::abs(x));
	   }
   }
   state.SetComplexityN(state.range(0));
}


// NOLINTEND