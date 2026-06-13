#pragma once

// Compile-time path guards for benchmark suites.
// Per-target: -DCCM_BM_CONFIG_NO_CT or -DCCM_BM_CONFIG_NO_RT via CMake.

#if !defined(CCM_BM_CONFIG_NO_CT)
#define CCMATH_BENCH_HAS_CT 1
#else
#define CCMATH_BENCH_HAS_CT 0
#endif

#if !defined(CCM_BM_CONFIG_NO_RT)
#define CCMATH_BENCH_HAS_RT 1
#else
#define CCMATH_BENCH_HAS_RT 0
#endif

// GCC libstdc++ constexpr std::sqrt makes RT-vs-std comparison meaningless on GCC 6.1+.
#if CCMATH_BENCH_HAS_RT && (defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__))
#define CCMATH_BENCH_HAS_RT_SUITE 0
#else
#define CCMATH_BENCH_HAS_RT_SUITE CCMATH_BENCH_HAS_RT
#endif

#if CCMATH_BENCH_HAS_CT
#define CCMATH_BENCH_IF_CT(...) __VA_ARGS__
#else
#define CCMATH_BENCH_IF_CT(...)
#endif

#if CCMATH_BENCH_HAS_RT_SUITE
#define CCMATH_BENCH_IF_RT(...) __VA_ARGS__
#else
#define CCMATH_BENCH_IF_RT(...)
#endif
