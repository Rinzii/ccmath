#include "shared/register.hpp"

#include <ccmath/ccmath.hpp>

#include <cmath>

CCMATH_BENCH_UNARY_COMPARE(nearest, trunc, return std::trunc(x);, return ccm::trunc(x);)

BENCHMARK_MAIN();
