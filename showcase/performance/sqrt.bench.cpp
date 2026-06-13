#include "bench/bench_common.hpp"

#include <benchmark/benchmark.h>

namespace
{
	static void BM_sqrt_std(benchmark::State & state)
	{ ccm::showcase::bench::register_random_doubles(state, ccm::showcase::backends::std_lib::eval_sqrt); }

	static void BM_sqrt_ccmath_public(benchmark::State & state)
	{ ccm::showcase::bench::register_random_doubles(state, ccm::showcase::backends::ccmath_public::eval); }

	static void BM_sqrt_ccmath_generic(benchmark::State & state)
	{ ccm::showcase::bench::register_random_doubles(state, ccm::showcase::backends::ccmath_generic_gen::eval); }

	static void BM_sqrt_ccmath_rt(benchmark::State & state)
	{ ccm::showcase::bench::register_random_doubles(state, ccm::showcase::backends::ccmath_runtime_rt::eval); }

	static void BM_sqrt_ccmath_simd(benchmark::State & state)
	{ ccm::showcase::bench::register_random_doubles(state, ccm::showcase::backends::ccmath_runtime_simd::eval); }

	static void BM_sqrt_ccmath_builtin(benchmark::State & state)
	{ ccm::showcase::bench::register_random_doubles(state, ccm::showcase::backends::ccmath_runtime_builtin::eval); }

#if defined(CCMATH_SHOWCASE_HAS_GCEM)
	static void BM_sqrt_gcem(benchmark::State & state)
	{ ccm::showcase::bench::register_random_doubles(state, ccm::showcase::backends::gcem_lib::eval_sqrt); }
#endif
} // namespace

BENCHMARK(BM_sqrt_std)->Name("sqrt/std")->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_sqrt_ccmath_public)->Name("sqrt/ccmath/public_default")->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_sqrt_ccmath_generic)->Name("sqrt/ccmath/generic_gen")->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_sqrt_ccmath_rt)->Name("sqrt/ccmath/runtime_rt")->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_sqrt_ccmath_simd)->Name("sqrt/ccmath/runtime_simd")->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_sqrt_ccmath_builtin)->Name("sqrt/ccmath/runtime_builtin")->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
#if defined(CCMATH_SHOWCASE_HAS_GCEM)
BENCHMARK(BM_sqrt_gcem)->Name("sqrt/gcem")->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
#endif

BENCHMARK_MAIN();
