/*
 * asmlab golden calibration benchmark: Horner vs Estrin.
 * Tool-owned fixture only. Does not use CCMath headers.
 */

#include <benchmark/benchmark.h>

#include "../golden/poly_estrin/kernel.hpp"
#include "../golden/poly_horner/kernel.hpp"

namespace
{
	float load_input(float v)
	{
		volatile float storage = v;
		return storage;
	}
} // namespace

static void BM_golden_poly_horner(benchmark::State& state)
{
	const float x = load_input(1.25f);
	for (auto _ : state) { benchmark::DoNotOptimize(asmlab::golden::poly_horner::poly_horner(x)); }
}
BENCHMARK(BM_golden_poly_horner);

static void BM_golden_poly_estrin(benchmark::State& state)
{
	const float x = load_input(1.25f);
	for (auto _ : state) { benchmark::DoNotOptimize(asmlab::golden::poly_estrin::poly_estrin(x)); }
}
BENCHMARK(BM_golden_poly_estrin);

BENCHMARK_MAIN();
