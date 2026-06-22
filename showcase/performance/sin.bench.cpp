#include "bench/bench_common.hpp"

#include <benchmark/benchmark.h>

namespace
{
	static void BM_sin_std(benchmark::State & state)
	{ ccm::showcase::bench::register_random_doubles(state, ccm::showcase::backends::std_lib::eval_sin); }

	static void BM_sin_ccmath_public(benchmark::State & state)
	{ ccm::showcase::bench::register_random_doubles(state, ccm::showcase::backends::ccmath_sin_public::eval); }

	static void BM_sin_ccmath_generic(benchmark::State & state)
	{ ccm::showcase::bench::register_random_doubles(state, ccm::showcase::backends::ccmath_sin_generic_gen::eval); }

	static void BM_sin_ccmath_rt(benchmark::State & state)
	{ ccm::showcase::bench::register_random_doubles(state, ccm::showcase::backends::ccmath_sin_runtime_rt::eval); }

	static void BM_sin_ccmath_simd(benchmark::State & state)
	{ ccm::showcase::bench::register_random_doubles(state, ccm::showcase::backends::ccmath_sin_runtime_simd::eval); }

	static void BM_sin_ccmath_builtin(benchmark::State & state)
	{ ccm::showcase::bench::register_random_doubles(state, ccm::showcase::backends::ccmath_sin_runtime_builtin::eval); }

#if defined(CCMATH_SHOWCASE_HAS_GCEM)
	static void BM_sin_gcem(benchmark::State & state)
	{ ccm::showcase::bench::register_random_doubles(state, ccm::showcase::backends::gcem_lib::eval_sin); }
#endif
} // namespace

BENCHMARK(BM_sin_std)->Name("sin/std")->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_sin_ccmath_public)->Name("sin/ccmath/public_default")->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_sin_ccmath_generic)->Name("sin/ccmath/generic_gen")->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_sin_ccmath_rt)->Name("sin/ccmath/runtime_rt")->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_sin_ccmath_simd)->Name("sin/ccmath/runtime_simd")->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_sin_ccmath_builtin)->Name("sin/ccmath/runtime_builtin")->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
#if defined(CCMATH_SHOWCASE_HAS_GCEM)
BENCHMARK(BM_sin_gcem)->Name("sin/gcem")->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
#endif

BENCHMARK_MAIN();
