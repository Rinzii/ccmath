// trunc.bench.hpp
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
#include <immintrin.h> // For SSE2 intrinsics

namespace bm = benchmark;

// NOLINTBEGIN

#ifndef CCM_BM_CONFIG_NO_CT

static void BM_power_trunc_std(bm::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        bm::DoNotOptimize(std::trunc(static_cast<double>(state.range(0))));
    }
    state.SetComplexityN(state.range(0));
}

static void BM_power_trunc_ccm(bm::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        bm::DoNotOptimize(ccm::trunc(static_cast<double>(state.range(0))));
    }
    state.SetComplexityN(state.range(0));
}

static void BM_power_trunc_rand_int_std(bm::State& state) {
    ccm::bench::Randomizer ran;
    auto randomIntegers = ran.generateRandomIntegers(state.range(0));
    while (state.KeepRunning()) {
        for (auto x : randomIntegers) {
            bm::DoNotOptimize(std::trunc(static_cast<double>(x)));
        }
    }
    state.SetComplexityN(state.range(0));
}

static void BM_power_trunc_rand_int_ccm(bm::State& state) {
    ccm::bench::Randomizer ran;
    auto randomIntegers = ran.generateRandomIntegers(state.range(0));
    while (state.KeepRunning()) {
        for (auto x : randomIntegers) {
            bm::DoNotOptimize(ccm::trunc(static_cast<double>(x)));
        }
    }
    state.SetComplexityN(state.range(0));
}

static void BM_power_trunc_rand_double_std(bm::State& state) {
    ccm::bench::Randomizer ran;
    auto randomDoubles = ran.generateRandomDoubles(state.range(0));
    while (state.KeepRunning()) {
        for (auto x : randomDoubles) {
            bm::DoNotOptimize(std::trunc(x));
        }
    }
    state.SetComplexityN(state.range(0));
}

static void BM_power_trunc_rand_double_ccm(bm::State& state) {
    ccm::bench::Randomizer ran;
    auto randomDoubles = ran.generateRandomDoubles(state.range(0));
    while (state.KeepRunning()) {
        for (auto x : randomDoubles) {
            bm::DoNotOptimize(ccm::trunc(x));
        }
    }
    state.SetComplexityN(state.range(0));
}

#endif

#ifndef CCM_BM_CONFIG_NO_RT

inline double trunc_rt_bm(double v) {
    return ccm::trunc(v);
}

static void BM_power_trunc_std_rt(bm::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        bm::DoNotOptimize(std::trunc(static_cast<double>(state.range(0))));
    }
    state.SetComplexityN(state.range(0));
}

static void BM_power_trunc_ccm_rt(bm::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        bm::DoNotOptimize(trunc_rt_bm(static_cast<double>(state.range(0))));
    }
    state.SetComplexityN(state.range(0));
}

static void BM_power_trunc_rand_int_std_rt(bm::State& state) {
    ccm::bench::Randomizer ran;
    auto randomIntegers = ran.generateRandomIntegers(state.range(0));
    while (state.KeepRunning()) {
        for (auto x : randomIntegers) {
            bm::DoNotOptimize(std::trunc(static_cast<double>(x)));
        }
    }
    state.SetComplexityN(state.range(0));
}

static void BM_power_trunc_rand_int_ccm_rt(bm::State& state) {
    ccm::bench::Randomizer ran;
    auto randomIntegers = ran.generateRandomIntegers(state.range(0));
    while (state.KeepRunning()) {
        for (auto x : randomIntegers) {
            bm::DoNotOptimize(trunc_rt_bm(static_cast<double>(x)));
        }
    }
    state.SetComplexityN(state.range(0));
}

static void BM_power_trunc_rand_double_std_rt(bm::State& state) {
    ccm::bench::Randomizer ran;
    auto randomDoubles = ran.generateRandomDoubles(state.range(0));
    while (state.KeepRunning()) {
        for (auto x : randomDoubles) {
            bm::DoNotOptimize(std::trunc(x));
        }
    }
    state.SetComplexityN(state.range(0));
}

static void BM_power_trunc_rand_double_ccm_rt(bm::State& state) {
    ccm::bench::Randomizer ran;
    auto randomDoubles = ran.generateRandomDoubles(state.range(0));
    while (state.KeepRunning()) {
        for (auto x : randomDoubles) {
            bm::DoNotOptimize(trunc_rt_bm(x));
        }
    }
    state.SetComplexityN(state.range(0));
}

#endif

// NOLINTEND
