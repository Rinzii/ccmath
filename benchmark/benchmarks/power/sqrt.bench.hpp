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
// std::sqrt


#ifndef CCM_BM_CONFIG_NO_CT
static void BM_power_sqrt_std(benchmark::State & state)
{
	for ([[maybe_unused]] auto _ : state) { benchmark::DoNotOptimize(std::sqrt(static_cast<double>(state.range(0)))); }
	state.SetComplexityN(state.range(0));
}

static void BM_power_sqrt_ccm(benchmark::State & state)
{
	for ([[maybe_unused]] auto _ : state) { benchmark::DoNotOptimize(ccm::sqrt(static_cast<double>(state.range(0)))); }
	state.SetComplexityN(state.range(0));
}

static void BM_power_sqrt_rand_int_std(benchmark::State & state)
{
	ccm::bench::Randomizer ran;
	auto randomIntegers = ran.generateRandomIntegers(state.range(0));
	while (state.KeepRunning())
	{
		for (auto x : randomIntegers) { benchmark::DoNotOptimize(std::sqrt(static_cast<double>(x))); }
	}
	state.SetComplexityN(state.range(0));
}
//
static void BM_power_sqrt_rand_int_ccm(benchmark::State & state)
{
	ccm::bench::Randomizer ran;
	auto randomIntegers = ran.generateRandomIntegers(state.range(0));
	while (state.KeepRunning())
	{
		for (auto x : randomIntegers) { benchmark::DoNotOptimize(ccm::sqrt(static_cast<double>(x))); }
	}
	state.SetComplexityN(state.range(0));
}

static void BM_power_sqrt_rand_double_std(benchmark::State & state)
{
	ccm::bench::Randomizer ran;
	auto randomDoubles = ran.generateRandomDoubles(state.range(0));
	while (state.KeepRunning())
	{
		for (auto x : randomDoubles) { benchmark::DoNotOptimize(std::sqrt(x)); }
	}
	state.SetComplexityN(state.range(0));
}

static void BM_power_sqrt_rand_double_ccm(benchmark::State & state)
{
	ccm::bench::Randomizer ran;
	auto randomDoubles = ran.generateRandomDoubles(state.range(0));
	while (state.KeepRunning())
	{
		for (auto x : randomDoubles) { benchmark::DoNotOptimize(ccm::sqrt(x)); }
	}
	state.SetComplexityN(state.range(0));
}

#endif

// Runtime evaluation

#ifndef CCM_BM_CONFIG_NO_RT
	#if !(defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__)) // GCC 6.1+ has constexpr sqrt builtins.

inline double sqrt_rt_bm(double v)
{
	return ccm::sqrt(v);
}

static void BM_power_sqrt_std_rt(benchmark::State & state)
{
	for ([[maybe_unused]] auto _ : state) { benchmark::DoNotOptimize(std::sqrt(static_cast<double>(state.range(0)))); }
	state.SetComplexityN(state.range(0));
}

static void BM_power_sqrt_ccm_rt(benchmark::State & state)
{
	for ([[maybe_unused]] auto _ : state) { benchmark::DoNotOptimize(sqrt_rt_bm(static_cast<double>(state.range(0)))); }
	state.SetComplexityN(state.range(0));
}

static void BM_power_sqrt_rand_int_std_rt(benchmark::State & state)
{
	ccm::bench::Randomizer ran;
	auto randomIntegers = ran.generateRandomIntegers(state.range(0));
	while (state.KeepRunning())
	{
		for (auto x : randomIntegers) { benchmark::DoNotOptimize(std::sqrt(static_cast<double>(x))); }
	}
	state.SetComplexityN(state.range(0));
}
//
static void BM_power_sqrt_rand_int_ccm_rt(benchmark::State & state)
{
	ccm::bench::Randomizer ran;
	auto randomIntegers = ran.generateRandomIntegers(state.range(0));
	while (state.KeepRunning())
	{
		for (auto x : randomIntegers) { benchmark::DoNotOptimize(sqrt_rt_bm(static_cast<double>(x))); }
	}
	state.SetComplexityN(state.range(0));
}

static void BM_power_sqrt_rand_double_std_rt(benchmark::State & state)
{
	ccm::bench::Randomizer ran;
	auto randomDoubles = ran.generateRandomDoubles(state.range(0));
	while (state.KeepRunning())
	{
		for (auto x : randomDoubles) { benchmark::DoNotOptimize(std::sqrt(x)); }
	}
	state.SetComplexityN(state.range(0));
}

static void BM_power_sqrt_rand_double_ccm_rt(benchmark::State & state)
{
	ccm::bench::Randomizer ran;
	auto randomDoubles = ran.generateRandomDoubles(state.range(0));
	while (state.KeepRunning())
	{
		for (auto x : randomDoubles) { benchmark::DoNotOptimize(sqrt_rt_bm(x)); }
	}
	state.SetComplexityN(state.range(0));
}

	#endif
#endif

// NOLINTEND