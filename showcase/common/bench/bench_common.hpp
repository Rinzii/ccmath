#pragma once

#include "backends.hpp"
#include "ccmath_paths.hpp"
#include "helpers/randomizers.hpp"

#include <benchmark/benchmark.h>

namespace ccm::showcase::bench
{
	template <typename EvalFn>
	inline void register_random_doubles(benchmark::State & state, EvalFn eval_fn)
	{
		ccm::bench::Randomizer ran;
		const auto values = ran.generateRandomDoubles(static_cast<std::size_t>(state.range(0)));
		for ([[maybe_unused]] auto _ : state)
		{
			for (double x : values) { benchmark::DoNotOptimize(eval_fn(x)); }
		}
		state.SetComplexityN(state.range(0));
	}
} // namespace ccm::showcase::bench
