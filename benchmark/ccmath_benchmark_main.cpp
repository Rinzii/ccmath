/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <benchmark/benchmark.h>
#include <vector>
#include <random>

#include <cmath>
#include "ccmath/ccmath.hpp"

namespace bm = benchmark;

// Global seed value for random number generator
constexpr unsigned int DefaultSeed = 937162211; // Using a long prime number as our default seed

// Generate a fixed set of random integers for benchmarking
std::vector<int> generateRandomIntegers(size_t count, unsigned int seed) {
	std::vector<int> randomIntegers;
	std::mt19937 gen(seed);
	std::uniform_int_distribution<int> dist(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
	for (size_t i = 0; i < count; ++i) {
		randomIntegers.push_back(dist(gen));
	}
	return randomIntegers;
}

// Generate a fixed set of random integers for benchmarking
std::vector<double> generateRandomDoubles(size_t count, unsigned int seed) {
	std::vector<double> randomDouble;
	std::mt19937 gen(seed);
	std::uniform_real_distribution<double> dist(std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
	for (size_t i = 0; i < count; ++i) {
		randomDouble.push_back(dist(gen));
	}
	return randomDouble;
}

// Benchmarking std::abs with the same set of random integers
static void BM_std_abs_rand_int(benchmark::State& state) {
	auto randomIntegers = generateRandomIntegers(static_cast<size_t>(state.range(0)), DefaultSeed);
	while (state.KeepRunning()) {
		for (auto x : randomIntegers) {
			benchmark::DoNotOptimize(std::abs(x));
		}
	}
	state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_std_abs_rand_int)->Range(8, 8<<10)->Complexity();

// Benchmarking ccm::abs with the same set of random integers
static void BM_ccm_abs_rand_int(benchmark::State& state) {
	auto randomIntegers = generateRandomIntegers(static_cast<size_t>(state.range(0)), DefaultSeed);
	while (state.KeepRunning()) {
		for (auto x : randomIntegers) {
			benchmark::DoNotOptimize(ccm::abs(x));
		}
	}
	state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_ccm_abs_rand_int)->Range(8, 8<<10)->Complexity();

// Benchmarking std::abs with the same set of random integers
static void BM_std_abs_rand_double(benchmark::State& state) {
	auto randomIntegers = generateRandomDoubles(static_cast<size_t>(state.range(0)), DefaultSeed);
	while (state.KeepRunning()) {
		for (auto x : randomIntegers) {
			benchmark::DoNotOptimize(std::abs(x));
		}
	}
	state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_std_abs_rand_double)->Range(8, 8<<10)->Complexity();

// Benchmarking ccm::abs with the same set of random integers
static void BM_ccm_abs_rand_double(benchmark::State& state) {
	auto randomIntegers = generateRandomDoubles(static_cast<size_t>(state.range(0)), DefaultSeed);
	while (state.KeepRunning()) {
		for (auto x : randomIntegers) {
			benchmark::DoNotOptimize(ccm::abs(x));
		}
	}
	state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_ccm_abs_rand_double)->Range(8, 8<<10)->Complexity();


static void BM_std_abs(benchmark::State& state) {
	for (auto _ : state) {
		benchmark::DoNotOptimize(std::abs(state.range(0)));
	}
	state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_std_abs)->Arg(16)->Arg(256)->Arg(4096)->Arg(65536)->Complexity();

static void BM_ccm_abs(benchmark::State& state) {
	for (auto _ : state) {
		benchmark::DoNotOptimize(ccm::abs(state.range(0)));
	}
	state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_ccm_abs)->Arg(16)->Arg(256)->Arg(4096)->Arg(65536)->Complexity();


static void BM_ccm_log(bm::State& state) {
	for (auto _ : state) {
		bm::DoNotOptimize(ccm::log(state.range(0)));
	}
	state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_ccm_log)->Arg(16)->Arg(256)->Arg(4096)->Arg(65536)->Complexity();

static void BM_std_log(bm::State& state) {
	for (auto _ : state) {
		bm::DoNotOptimize(std::log(state.range(0)));
	}
	state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_std_log)->Arg(16)->Arg(256)->Arg(4096)->Arg(65536)->Complexity();

static void BM_ccm_log2(bm::State& state) {
	for (auto _ : state) {
		bm::DoNotOptimize(ccm::log2(state.range(0)));
	}
	state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_ccm_log2)->Arg(16)->Arg(256)->Arg(4096)->Arg(65536)->Complexity();

static void BM_std_log2(bm::State& state) {
	for (auto _ : state) {
		bm::DoNotOptimize(std::log2(state.range(0)));
	}
	state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_std_log2)->Arg(16)->Arg(256)->Arg(4096)->Arg(65536)->Complexity();

BENCHMARK_MAIN();
