#include "shared/register.hpp"

#include <ccmath/ccmath.hpp>

#include <cmath>

CCMATH_BENCH_TERNARY_COMPARE(basic, fma, return std::fma(x, y, z);, return ccm::fma(x, y, z);)

BENCHMARK_MAIN();
