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
namespace temp = ccm::rt;

// NOLINTBEGIN
//std::sqrt

static void BM_power_sqrt_std(benchmark::State& state) {
   for ([[maybe_unused]] auto _ : state) {
	   benchmark::DoNotOptimize(std::sqrt(static_cast<double>(state.range(0))));
   }
   state.SetComplexityN(state.range(0));
}

static void BM_power_sqrt_ccm(benchmark::State& state) {
   for ([[maybe_unused]] auto _ : state) {
	   benchmark::DoNotOptimize(temp::sqrt_rt(static_cast<double>(state.range(0))));
   }
   state.SetComplexityN(state.range(0));
}

static void BM_power_sqrt_rand_int_std(benchmark::State& state) {
   ccm::bench::Randomizer ran;
   auto randomIntegers = ran.generateRandomIntegers(state.range(0));
   while (state.KeepRunning()) {
	   for (auto x : randomIntegers) {
		   benchmark::DoNotOptimize(std::sqrt(static_cast<double>(x)));
	   }
   }
   state.SetComplexityN(state.range(0));
}
//
static void BM_power_sqrt_rand_int_ccm(benchmark::State& state) {
   ccm::bench::Randomizer ran;
   auto randomIntegers = ran.generateRandomIntegers(state.range(0));
   while (state.KeepRunning()) {
	   for (auto x : randomIntegers) {
		   benchmark::DoNotOptimize(temp::sqrt_rt(static_cast<double>(x)));
	   }
   }
   state.SetComplexityN(state.range(0));
}

static void BM_power_sqrt_rand_double_std(benchmark::State& state) {
   ccm::bench::Randomizer ran;
   auto randomDoubles = ran.generateRandomDoubles(state.range(0));
   while (state.KeepRunning()) {
	   for (auto x : randomDoubles) {
		   benchmark::DoNotOptimize(std::sqrt(x));
	   }
   }
   state.SetComplexityN(state.range(0));
}

static void BM_power_sqrt_rand_double_ccm(benchmark::State& state) {
   ccm::bench::Randomizer ran;
   auto randomDoubles = ran.generateRandomDoubles(state.range(0));
   while (state.KeepRunning()) {
	   for (auto x : randomDoubles) {
		   benchmark::DoNotOptimize(temp::sqrt_rt(x));
	   }
   }
   state.SetComplexityN(state.range(0));
}


// NOLINTEND