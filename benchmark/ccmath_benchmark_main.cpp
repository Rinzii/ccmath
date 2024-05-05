/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <benchmark/benchmark.h>
#include <random>
#include <vector>

#include <cmath>
#include "ccmath/ccmath.hpp"
#include "helpers/randomizers.hpp"

// NOLINTBEGIN

namespace bm = benchmark;

static ccm::bench::Randomizer ran{};

static void BM_basic_abs_ccm(benchmark::State& state) {
	auto randomIntegers = ran.generateRandomIntegers(static_cast<size_t>(state.range(0)));
	while (state.KeepRunning()) {
		for (auto x : randomIntegers) {
			benchmark::DoNotOptimize(std::abs(x));
		}
	}
	state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_basic_abs_ccm)->Range(8, 8<<10)->Complexity();

static void BM_basic_abs_std(benchmark::State& state) {
	auto randomIntegers = ran.generateRandomIntegers(static_cast<size_t>(state.range(0)));
	while (state.KeepRunning()) {
		for (auto x : randomIntegers) {
			benchmark::DoNotOptimize(ccm::abs(x));
		}
	}
	state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_basic_abs_std)->Range(8, 8<<10)->Complexity();




/*

static void BM_std_fma(bm::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        bm::DoNotOptimize(std::fma(state.range(0), state.range(1), state.range(2)));
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_std_fma)->Args({16, 16, 16})->Args({256, 256, 256})->Args({4096, 4096, 4096})->Args({65536, 65536, 65536})->Complexity();

static void BM_ccm_fma(bm::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        bm::DoNotOptimize(ccm::fma(state.range(0), state.range(1), state.range(2)));
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_ccm_fma)->Args({16, 16, 16})->Args({256, 256, 256})->Args({4096, 4096, 4096})->Args({65536, 65536, 65536})->Complexity();

*/

/*
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
*/
/*

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


static void BM_std_log_rand_double(bm::State& state) {
	auto randomDoubles = generateRandomDoubles(static_cast<size_t>(state.range(0)), DefaultSeed);
	while (state.KeepRunning()) {
		for (auto x : randomDoubles) {
			bm::DoNotOptimize(std::log(x));
		}
	}
	state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_std_log_rand_double)->Range(8, 8<<10)->Complexity();

static void BM_ccm_log_rand_double(bm::State& state) {
    auto randomDoubles = generateRandomDoubles(static_cast<size_t>(state.range(0)), DefaultSeed);
    while (state.KeepRunning()) {
        for (auto x : randomDoubles) {
            bm::DoNotOptimize(ccm::log(x));
        }
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_ccm_log_rand_double)->Range(8, 8<<10)->Complexity();

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

static void BM_std_log2_rand_double(bm::State& state) {
    auto randomDoubles = generateRandomDoubles(static_cast<size_t>(state.range(0)), DefaultSeed);
    while (state.KeepRunning()) {
        for (auto x : randomDoubles) {
            bm::DoNotOptimize(std::log2(x));
        }
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_std_log2_rand_double)->Range(8, 8<<10)->Complexity();

static void BM_ccm_log2_rand_double(bm::State& state) {
    auto randomDoubles = generateRandomDoubles(static_cast<size_t>(state.range(0)), DefaultSeed);
    while (state.KeepRunning()) {
        for (auto x : randomDoubles) {
            bm::DoNotOptimize(ccm::log2(x));
        }
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_ccm_log2_rand_double)->Range(8, 8<<10)->Complexity();

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

static void BM_ccm_lerp(bm::State& state) {
    for (auto _ : state) {
        bm::DoNotOptimize(ccm::lerp(state.range(0), state.range(1), state.range(2)));
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_ccm_lerp)->Args({16, 16, 16})->Args({256, 256, 256})->Args({4096, 4096, 4096})->Args({65536, 65536, 65536})->Complexity();

static void BM_ccm_lerp2(bm::State& state) {
	for (auto _ : state) {
		bm::DoNotOptimize(ccm::lerp2(state.range(0), state.range(1), state.range(2)));
	}
	state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_ccm_lerp2)->Args({16, 16, 16})->Args({256, 256, 256})->Args({4096, 4096, 4096})->Args({65536, 65536, 65536})->Complexity();


static void BM_std_lerp(bm::State& state) {
    for (auto _ : state) {
        bm::DoNotOptimize(ccm::lerp(state.range(0), state.range(1), state.range(2)));
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_std_lerp)->Args({16, 16, 16})->Args({256, 256, 256})->Args({4096, 4096, 4096})->Args({65536, 65536, 65536})->Complexity();

*/

// fma
static void BM_std_fma(bm::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        bm::DoNotOptimize(std::fma(state.range(0), state.range(1), state.range(2)));
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_std_fma)->Args({16, 16, 16})->Args({256, 256, 256})->Args({4096, 4096, 4096})->Args({65536, 65536, 65536})->Complexity();

static void BM_ccm_fma(bm::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        bm::DoNotOptimize(ccm::fma(state.range(0), state.range(1), state.range(2)));
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_ccm_fma)->Args({16, 16, 16})->Args({256, 256, 256})->Args({4096, 4096, 4096})->Args({65536, 65536, 65536})->Complexity();

static void BM_std_fma_rand_double(bm::State& state) {
    auto randomDoubles = generateRandomDoubles(static_cast<size_t>(state.range(0)), DefaultSeed);
    while (state.KeepRunning()) {
        for (auto x : randomDoubles) {
            bm::DoNotOptimize(std::fma(x, x, x));
        }
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_std_fma_rand_double)->Range(8, 8<<10)->Complexity();

static void BM_ccm_fma_rand_double(bm::State& state) {
    auto randomDoubles = generateRandomDoubles(static_cast<size_t>(state.range(0)), DefaultSeed);
    while (state.KeepRunning()) {
        for (auto x : randomDoubles) {
            bm::DoNotOptimize(ccm::fma(x, x, x));
        }
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_ccm_fma_rand_double)->Range(8, 8<<10)->Complexity();



BENCHMARK_MAIN();


// NOLINTEND
