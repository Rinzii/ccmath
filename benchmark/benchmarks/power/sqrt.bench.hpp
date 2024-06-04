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
//std::sqrt

static void BM_power_sqrt_generic(benchmark::State& state) {
   for ([[maybe_unused]] auto _ : state) {
	   benchmark::DoNotOptimize(std::sqrt(state.range(0)));
   }
   state.SetComplexityN(state.range(0));
}

static void BM_power_sqrt_simd(benchmark::State& state) {
   for ([[maybe_unused]] auto _ : state) {
	   benchmark::DoNotOptimize(ccm::support::math::internal::simd::sqrt_impl_simd(state.range(0)));
   }
   state.SetComplexityN(state.range(0));
}

static void BM_power_sqrt_rand_int_generic(benchmark::State& state) {
   ccm::bench::Randomizer ran;
   auto randomIntegers = ran.generateRandomIntegers(state.range(0));
   while (state.KeepRunning()) {
	   for (auto x : randomIntegers) {
		   benchmark::DoNotOptimize(std::sqrt(x));
	   }
   }
   state.SetComplexityN(state.range(0));
}

static void BM_power_sqrt_rand_int_simd(benchmark::State& state) {
   ccm::bench::Randomizer ran;
   auto randomIntegers = ran.generateRandomIntegers(state.range(0));
   while (state.KeepRunning()) {
	   for (auto x : randomIntegers) {
		   benchmark::DoNotOptimize(ccm::support::math::internal::simd::sqrt_impl_simd(x));
	   }
   }
   state.SetComplexityN(state.range(0));
}

static void BM_power_sqrt_rand_double_generic(benchmark::State& state) {
   ccm::bench::Randomizer ran;
   auto randomDoubles = ran.generateRandomDoubles(state.range(0));
   while (state.KeepRunning()) {
	   for (auto x : randomDoubles) {
		   benchmark::DoNotOptimize(std::sqrt(x));
	   }
   }
   state.SetComplexityN(state.range(0));
}

static void BM_power_sqrt_rand_double_simd(benchmark::State& state) {
   ccm::bench::Randomizer ran;
   auto randomDoubles = ran.generateRandomDoubles(state.range(0));
   while (state.KeepRunning()) {
	   for (auto x : randomDoubles) {
		   benchmark::DoNotOptimize(ccm::support::math::internal::simd::sqrt_impl_simd(x));
	   }
   }
   state.SetComplexityN(state.range(0));
}


// NOLINTEND