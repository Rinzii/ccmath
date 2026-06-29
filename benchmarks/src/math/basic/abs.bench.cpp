#include "shared/register.hpp"

#include <ccmath/ccmath.hpp>

#include <cmath>

CCMATH_BENCH_UNARY_COMPARE(basic, abs, return std::abs(x);, return ccm::abs(x);)

BENCHMARK_MAIN();
