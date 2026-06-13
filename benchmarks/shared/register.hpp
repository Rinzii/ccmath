#pragma once

#include "harness.hpp"
#include "paths.hpp"

#include <benchmark/benchmark.h>

#include <cstdint>

#define CCMATH_BENCH_APPLY_RANGE ->RangeMultiplier(2)->Range(ccm::bench::kRangeMin, ccm::bench::kRangeMax)->Complexity()

#define CCMATH_BENCH_DECLARE_UNARY_SUITE(Module, Name, Suffix, StdBody, CcmBody)                                                                             \
	static void BM_##Module##_##Name##_std##Suffix##_scalar(benchmark::State& state)                                                                           \
	{ ccm::bench::unary_scalar<std::int64_t>(state, [&](std::int64_t x) { StdBody; }); }                                                                       \
	static void BM_##Module##_##Name##_ccm##Suffix##_scalar(benchmark::State& state)                                                                           \
	{ ccm::bench::unary_scalar<std::int64_t>(state, [&](std::int64_t x) { CcmBody; }); }                                                                       \
	static void BM_##Module##_##Name##_std##Suffix##_rand_int(benchmark::State& state)                                                                       \
	{                                                                                                                                                          \
		auto inputs = ccm::bench::random_integers(state.range(0));                                                                                             \
		ccm::bench::unary_vector(state, [&](int x) { StdBody; }, inputs);                                                                                      \
	}                                                                                                                                                          \
	static void BM_##Module##_##Name##_ccm##Suffix##_rand_int(benchmark::State& state)                                                                       \
	{                                                                                                                                                          \
		auto inputs = ccm::bench::random_integers(state.range(0));                                                                                             \
		ccm::bench::unary_vector(state, [&](int x) { CcmBody; }, inputs);                                                                                       \
	}                                                                                                                                                          \
	static void BM_##Module##_##Name##_std##Suffix##_rand_double(benchmark::State& state)                                                                    \
	{                                                                                                                                                          \
		auto inputs = ccm::bench::random_doubles(state.range(0));                                                                                              \
		ccm::bench::unary_vector(state, [&](double x) { StdBody; }, inputs);                                                                                    \
	}                                                                                                                                                          \
	static void BM_##Module##_##Name##_ccm##Suffix##_rand_double(benchmark::State& state)                                                                    \
	{                                                                                                                                                          \
		auto inputs = ccm::bench::random_doubles(state.range(0));                                                                                              \
		ccm::bench::unary_vector(state, [&](double x) { CcmBody; }, inputs);                                                                                    \
	}

#define CCMATH_BENCH_REGISTER_UNARY_SUITE(Module, Name, Suffix)                                                                                                 \
	BENCHMARK(BM_##Module##_##Name##_std##Suffix##_scalar) CCMATH_BENCH_APPLY_RANGE;                                                                           \
	BENCHMARK(BM_##Module##_##Name##_ccm##Suffix##_scalar) CCMATH_BENCH_APPLY_RANGE;                                                                           \
	BENCHMARK(BM_##Module##_##Name##_std##Suffix##_rand_int) CCMATH_BENCH_APPLY_RANGE;                                                                       \
	BENCHMARK(BM_##Module##_##Name##_ccm##Suffix##_rand_int) CCMATH_BENCH_APPLY_RANGE;                                                                       \
	BENCHMARK(BM_##Module##_##Name##_std##Suffix##_rand_double) CCMATH_BENCH_APPLY_RANGE;                                                                    \
	BENCHMARK(BM_##Module##_##Name##_ccm##Suffix##_rand_double) CCMATH_BENCH_APPLY_RANGE;

#define CCMATH_BENCH_DECLARE_BINARY_SUITE(Module, Name, Suffix, StdBody, CcmBody)                                                                              \
	static void BM_##Module##_##Name##_std##Suffix##_scalar(benchmark::State& state)                                                                           \
	{ ccm::bench::binary_scalar<double>(state, [&](double x, double y) { StdBody; }); }                                                                      \
	static void BM_##Module##_##Name##_ccm##Suffix##_scalar(benchmark::State& state)                                                                           \
	{ ccm::bench::binary_scalar<double>(state, [&](double x, double y) { CcmBody; }); }                                                                        \
	static void BM_##Module##_##Name##_std##Suffix##_rand_int(benchmark::State& state)                                                                       \
	{                                                                                                                                                          \
		auto xs = ccm::bench::random_integers(state.range(0));                                                                                                  \
		auto ys = ccm::bench::random_integers(state.range(0));                                                                                                  \
		ccm::bench::binary_vector(state, [&](int x, int y) { StdBody; }, xs, ys);                                                                              \
	}                                                                                                                                                          \
	static void BM_##Module##_##Name##_ccm##Suffix##_rand_int(benchmark::State& state)                                                                       \
	{                                                                                                                                                          \
		auto xs = ccm::bench::random_integers(state.range(0));                                                                                                  \
		auto ys = ccm::bench::random_integers(state.range(0));                                                                                                  \
		ccm::bench::binary_vector(state, [&](int x, int y) { CcmBody; }, xs, ys);                                                                               \
	}                                                                                                                                                          \
	static void BM_##Module##_##Name##_std##Suffix##_rand_double(benchmark::State& state)                                                                    \
	{                                                                                                                                                          \
		auto xs = ccm::bench::random_doubles(state.range(0));                                                                                                   \
		auto ys = ccm::bench::random_doubles(state.range(0));                                                                                                  \
		ccm::bench::binary_vector(state, [&](double x, double y) { StdBody; }, xs, ys);                                                                        \
	}                                                                                                                                                          \
	static void BM_##Module##_##Name##_ccm##Suffix##_rand_double(benchmark::State& state)                                                                    \
	{                                                                                                                                                          \
		auto xs = ccm::bench::random_doubles(state.range(0));                                                                                                   \
		auto ys = ccm::bench::random_doubles(state.range(0));                                                                                                  \
		ccm::bench::binary_vector(state, [&](double x, double y) { CcmBody; }, xs, ys);                                                                        \
	}

#define CCMATH_BENCH_REGISTER_BINARY_SUITE(Module, Name, Suffix)                                                                                               \
	BENCHMARK(BM_##Module##_##Name##_std##Suffix##_scalar) CCMATH_BENCH_APPLY_RANGE;                                                                           \
	BENCHMARK(BM_##Module##_##Name##_ccm##Suffix##_scalar) CCMATH_BENCH_APPLY_RANGE;                                                                           \
	BENCHMARK(BM_##Module##_##Name##_std##Suffix##_rand_int) CCMATH_BENCH_APPLY_RANGE;                                                                       \
	BENCHMARK(BM_##Module##_##Name##_ccm##Suffix##_rand_int) CCMATH_BENCH_APPLY_RANGE;                                                                       \
	BENCHMARK(BM_##Module##_##Name##_std##Suffix##_rand_double) CCMATH_BENCH_APPLY_RANGE;                                                                    \
	BENCHMARK(BM_##Module##_##Name##_ccm##Suffix##_rand_double) CCMATH_BENCH_APPLY_RANGE;

#define CCMATH_BENCH_DECLARE_TERNARY_SUITE(Module, Name, Suffix, StdBody, CcmBody)                                                                             \
	static void BM_##Module##_##Name##_std##Suffix##_scalar(benchmark::State& state)                                                                           \
	{ ccm::bench::ternary_scalar<double>(state, [&](double x, double y, double z) { StdBody; }); }                                                             \
	static void BM_##Module##_##Name##_ccm##Suffix##_scalar(benchmark::State& state)                                                                           \
	{ ccm::bench::ternary_scalar<double>(state, [&](double x, double y, double z) { CcmBody; }); }                                                              \
	static void BM_##Module##_##Name##_std##Suffix##_rand_int(benchmark::State& state)                                                                       \
	{                                                                                                                                                          \
		auto xs = ccm::bench::random_integers(state.range(0));                                                                                                  \
		auto ys = ccm::bench::random_integers(state.range(0));                                                                                                  \
		auto zs = ccm::bench::random_integers(state.range(0));                                                                                                  \
		ccm::bench::ternary_vector(state, [&](int x, int y, int z) { StdBody; }, xs, ys, zs);                                                                  \
	}                                                                                                                                                          \
	static void BM_##Module##_##Name##_ccm##Suffix##_rand_int(benchmark::State& state)                                                                       \
	{                                                                                                                                                          \
		auto xs = ccm::bench::random_integers(state.range(0));                                                                                                  \
		auto ys = ccm::bench::random_integers(state.range(0));                                                                                                  \
		auto zs = ccm::bench::random_integers(state.range(0));                                                                                                  \
		ccm::bench::ternary_vector(state, [&](int x, int y, int z) { CcmBody; }, xs, ys, zs);                                                                   \
	}                                                                                                                                                          \
	static void BM_##Module##_##Name##_std##Suffix##_rand_double(benchmark::State& state)                                                                    \
	{                                                                                                                                                          \
		auto xs = ccm::bench::random_doubles(state.range(0));                                                                                                   \
		auto ys = ccm::bench::random_doubles(state.range(0));                                                                                                  \
		auto zs = ccm::bench::random_doubles(state.range(0));                                                                                                  \
		ccm::bench::ternary_vector(state, [&](double x, double y, double z) { StdBody; }, xs, ys, zs);                                                         \
	}                                                                                                                                                          \
	static void BM_##Module##_##Name##_ccm##Suffix##_rand_double(benchmark::State& state)                                                                    \
	{                                                                                                                                                          \
		auto xs = ccm::bench::random_doubles(state.range(0));                                                                                                   \
		auto ys = ccm::bench::random_doubles(state.range(0));                                                                                                  \
		auto zs = ccm::bench::random_doubles(state.range(0));                                                                                                  \
		ccm::bench::ternary_vector(state, [&](double x, double y, double z) { CcmBody; }, xs, ys, zs);                                                         \
	}

#define CCMATH_BENCH_REGISTER_TERNARY_SUITE(Module, Name, Suffix)                                                                                              \
	BENCHMARK(BM_##Module##_##Name##_std##Suffix##_scalar) CCMATH_BENCH_APPLY_RANGE;                                                                           \
	BENCHMARK(BM_##Module##_##Name##_ccm##Suffix##_scalar) CCMATH_BENCH_APPLY_RANGE;                                                                           \
	BENCHMARK(BM_##Module##_##Name##_std##Suffix##_rand_int) CCMATH_BENCH_APPLY_RANGE;                                                                       \
	BENCHMARK(BM_##Module##_##Name##_ccm##Suffix##_rand_int) CCMATH_BENCH_APPLY_RANGE;                                                                       \
	BENCHMARK(BM_##Module##_##Name##_std##Suffix##_rand_double) CCMATH_BENCH_APPLY_RANGE;                                                                    \
	BENCHMARK(BM_##Module##_##Name##_ccm##Suffix##_rand_double) CCMATH_BENCH_APPLY_RANGE;

#define CCMATH_BENCH_DECLARE_UNARY_PROFILE_SUITE(Module, Name, Suffix, Profile, Value, StdBody, CcmBody)                                                       \
	static void BM_##Module##_##Name##_std##Suffix##_##Profile(benchmark::State& state)                                                                        \
	{ ccm::bench::unary_fixed<double>(state, Value, [&](double x) { StdBody; }); }                                                                            \
	static void BM_##Module##_##Name##_ccm##Suffix##_##Profile(benchmark::State& state)                                                                        \
	{ ccm::bench::unary_fixed<double>(state, Value, [&](double x) { CcmBody; }); }

#define CCMATH_BENCH_REGISTER_UNARY_PROFILE_SUITE(Module, Name, Suffix, Profile)                                                                               \
	BENCHMARK(BM_##Module##_##Name##_std##Suffix##_##Profile);                                                                                                 \
	BENCHMARK(BM_##Module##_##Name##_ccm##Suffix##_##Profile);

#define CCMATH_BENCH_DECLARE_BINARY_PROFILE_SUITE(Module, Name, Suffix, Profile, X, Y, StdBody, CcmBody)                                                       \
	static void BM_##Module##_##Name##_std##Suffix##_##Profile(benchmark::State& state)                                                                        \
	{ ccm::bench::binary_fixed<double>(state, X, Y, [&](double x, double y) { StdBody; }); }                                                                  \
	static void BM_##Module##_##Name##_ccm##Suffix##_##Profile(benchmark::State& state)                                                                        \
	{ ccm::bench::binary_fixed<double>(state, X, Y, [&](double x, double y) { CcmBody; }); }

#define CCMATH_BENCH_REGISTER_BINARY_PROFILE_SUITE(Module, Name, Suffix, Profile)                                                                              \
	BENCHMARK(BM_##Module##_##Name##_std##Suffix##_##Profile);                                                                                                \
	BENCHMARK(BM_##Module##_##Name##_ccm##Suffix##_##Profile);

#define CCMATH_BENCH_UNARY_COMPARE(Module, Name, StdBody, CcmBody)                                                                                             \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_DECLARE_UNARY_SUITE(Module, Name, , StdBody, CcmBody))                                                                     \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_REGISTER_UNARY_SUITE(Module, Name, ))

#define CCMATH_BENCH_UNARY_COMPARE_PATHS(Module, Name, StdBody, CcmCtBody, CcmRtBody)                                                                          \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_DECLARE_UNARY_SUITE(Module, Name, , StdBody, CcmCtBody))                                                                   \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_REGISTER_UNARY_SUITE(Module, Name, ))                                                                                      \
	CCMATH_BENCH_IF_RT(CCMATH_BENCH_DECLARE_UNARY_SUITE(Module, Name, _rt, StdBody, CcmRtBody))                                                               \
	CCMATH_BENCH_IF_RT(CCMATH_BENCH_REGISTER_UNARY_SUITE(Module, Name, _rt))

#define CCMATH_BENCH_BINARY_COMPARE(Module, Name, StdBody, CcmBody)                                                                                            \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_DECLARE_BINARY_SUITE(Module, Name, , StdBody, CcmBody))                                                                     \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_REGISTER_BINARY_SUITE(Module, Name, ))

#define CCMATH_BENCH_BINARY_COMPARE_PATHS(Module, Name, StdBody, CcmCtBody, CcmRtBody)                                                                         \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_DECLARE_BINARY_SUITE(Module, Name, , StdBody, CcmCtBody))                                                                  \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_REGISTER_BINARY_SUITE(Module, Name, ))                                                                                     \
	CCMATH_BENCH_IF_RT(CCMATH_BENCH_DECLARE_BINARY_SUITE(Module, Name, _rt, StdBody, CcmRtBody))                                                              \
	CCMATH_BENCH_IF_RT(CCMATH_BENCH_REGISTER_BINARY_SUITE(Module, Name, _rt))

#define CCMATH_BENCH_TERNARY_COMPARE(Module, Name, StdBody, CcmBody)                                                                                            \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_DECLARE_TERNARY_SUITE(Module, Name, , StdBody, CcmBody))                                                                   \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_REGISTER_TERNARY_SUITE(Module, Name, ))

#define CCMATH_BENCH_TERNARY_COMPARE_PATHS(Module, Name, StdBody, CcmCtBody, CcmRtBody)                                                                        \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_DECLARE_TERNARY_SUITE(Module, Name, , StdBody, CcmCtBody))                                                                 \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_REGISTER_TERNARY_SUITE(Module, Name, ))                                                                                    \
	CCMATH_BENCH_IF_RT(CCMATH_BENCH_DECLARE_TERNARY_SUITE(Module, Name, _rt, StdBody, CcmRtBody))                                                             \
	CCMATH_BENCH_IF_RT(CCMATH_BENCH_REGISTER_TERNARY_SUITE(Module, Name, _rt))

#define CCMATH_BENCH_UNARY_PROFILE(Module, Name, Profile, Value, StdBody, CcmBody)                                                                             \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_DECLARE_UNARY_PROFILE_SUITE(Module, Name, , Profile, Value, StdBody, CcmBody))                                             \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_REGISTER_UNARY_PROFILE_SUITE(Module, Name, , Profile))

#define CCMATH_BENCH_UNARY_PROFILE_PATHS(Module, Name, Profile, Value, StdBody, CcmCtBody, CcmRtBody)                                                          \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_DECLARE_UNARY_PROFILE_SUITE(Module, Name, , Profile, Value, StdBody, CcmCtBody))                                           \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_REGISTER_UNARY_PROFILE_SUITE(Module, Name, , Profile))                                                                     \
	CCMATH_BENCH_IF_RT(CCMATH_BENCH_DECLARE_UNARY_PROFILE_SUITE(Module, Name, _rt, Profile, Value, StdBody, CcmRtBody))                                      \
	CCMATH_BENCH_IF_RT(CCMATH_BENCH_REGISTER_UNARY_PROFILE_SUITE(Module, Name, _rt, Profile))

#define CCMATH_BENCH_BINARY_PROFILE(Module, Name, Profile, X, Y, StdBody, CcmBody)                                                                             \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_DECLARE_BINARY_PROFILE_SUITE(Module, Name, , Profile, X, Y, StdBody, CcmBody))                                               \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_REGISTER_BINARY_PROFILE_SUITE(Module, Name, , Profile))

#define CCMATH_BENCH_BINARY_PROFILE_PATHS(Module, Name, Profile, X, Y, StdBody, CcmCtBody, CcmRtBody)                                                          \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_DECLARE_BINARY_PROFILE_SUITE(Module, Name, , Profile, X, Y, StdBody, CcmCtBody))                                           \
	CCMATH_BENCH_IF_CT(CCMATH_BENCH_REGISTER_BINARY_PROFILE_SUITE(Module, Name, , Profile))                                                                      \
	CCMATH_BENCH_IF_RT(CCMATH_BENCH_DECLARE_BINARY_PROFILE_SUITE(Module, Name, _rt, Profile, X, Y, StdBody, CcmRtBody))                                      \
	CCMATH_BENCH_IF_RT(CCMATH_BENCH_REGISTER_BINARY_PROFILE_SUITE(Module, Name, _rt, Profile))

#define CCMATH_BENCH_DECLARE_UNARY_COMPARE(Module, Name, StdBody, CcmBody)                                                                                     \
	CCMATH_BENCH_DECLARE_UNARY_SUITE(Module, Name, , StdBody, CcmBody)

#define CCMATH_BENCH_REGISTER_UNARY_COMPARE(Module, Name) CCMATH_BENCH_REGISTER_UNARY_SUITE(Module, Name, )

#define CCMATH_BENCH_DECLARE_BINARY_COMPARE(Module, Name, StdBody, CcmBody)                                                                                    \
	CCMATH_BENCH_DECLARE_BINARY_SUITE(Module, Name, , StdBody, CcmBody)

#define CCMATH_BENCH_REGISTER_BINARY_COMPARE(Module, Name) CCMATH_BENCH_REGISTER_BINARY_SUITE(Module, Name, )

#define CCMATH_BENCH_DECLARE_TERNARY_COMPARE(Module, Name, StdBody, CcmBody)                                                                                   \
	CCMATH_BENCH_DECLARE_TERNARY_SUITE(Module, Name, , StdBody, CcmBody)

#define CCMATH_BENCH_REGISTER_TERNARY_COMPARE(Module, Name) CCMATH_BENCH_REGISTER_TERNARY_SUITE(Module, Name, )
