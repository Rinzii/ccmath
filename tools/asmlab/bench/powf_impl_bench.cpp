/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 * Isolated Google Benchmark for powf_impl kernel (original or candidate).
 * Built by tools/asmlab/bench. Never replaces production headers.
 */

#include <benchmark/benchmark.h>

#include <cstring>
#include <random>
#include <vector>

#ifdef ASMLAB_POWF_IMPL_CANDIDATE
	#include "candidate.hpp"
	#define ASMLAB_POWF_IMPL_CALL(base, exp) ::asmlab::variant::powf_impl::ASMLAB_VARIANT_NAME::powf_impl((base), (exp))
#else
	#include "ccmath/internal/math/generic/func/power/pow_impl/powf_impl.hpp"
	#define ASMLAB_POWF_IMPL_CALL(base, exp) ::ccm::gen::impl::powf_impl((base), (exp))
#endif

namespace
{
	static float load_float(float v)
	{
		volatile float storage = v;
		return storage;
	}

	static float load_float(unsigned bits)
	{
		unsigned b = bits;
		float out;
		std::memcpy(&out, &b, sizeof(out));
		return out;
	}

	static void bench_pair(benchmark::State & state, float base_lit, float exp_lit)
	{
		for (auto _ : state)
		{
			const float base = load_float(base_lit);
			const float exp	 = load_float(exp_lit);
			benchmark::DoNotOptimize(ASMLAB_POWF_IMPL_CALL(base, exp));
		}
	}

	static void bench_rand_pairs(benchmark::State & state, float bmin, float bmax, float emin, float emax)
	{
		std::vector<float> bases(static_cast<std::size_t>(state.range(0)));
		std::vector<float> exps(static_cast<std::size_t>(state.range(0)));
		std::mt19937 gen(0xA51AB);
		std::uniform_real_distribution<float> bdist(bmin, bmax);
		std::uniform_real_distribution<float> edist(emin, emax);
		for (auto & b : bases)
		{
			b = bdist(gen);
		}
		for (auto & e : exps)
		{
			e = edist(gen);
		}
		while (state.KeepRunning())
		{
			for (std::size_t i = 0; i < bases.size(); ++i)
			{
				benchmark::DoNotOptimize(ASMLAB_POWF_IMPL_CALL(load_float(bases[i]), load_float(exps[i])));
			}
		}
		state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * static_cast<int64_t>(bases.size()));
	}
} // namespace

static void BM_asmlab_powf_impl_positive_finite_general(benchmark::State & state)
{
	bench_pair(state, 2.0f, 3.0f);
}
BENCHMARK(BM_asmlab_powf_impl_positive_finite_general);

static void BM_asmlab_powf_impl_near_one(benchmark::State & state)
{
	bench_pair(state, 1.0001f, 1.5f);
}
BENCHMARK(BM_asmlab_powf_impl_near_one);

static void BM_asmlab_powf_impl_integer_exponent(benchmark::State & state)
{
	bench_pair(state, 2.0f, 4.0f);
}
BENCHMARK(BM_asmlab_powf_impl_integer_exponent);

static void BM_asmlab_powf_impl_rand_positive_finite(benchmark::State & state)
{
	bench_rand_pairs(state, 0.5f, 8.0f, 0.25f, 6.0f);
}
BENCHMARK(BM_asmlab_powf_impl_rand_positive_finite)->RangeMultiplier(2)->Range(64, 64 << 4);

BENCHMARK_MAIN();
