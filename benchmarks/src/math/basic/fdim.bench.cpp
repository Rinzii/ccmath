#include "shared/register.hpp"

#include <ccmath/ccmath.hpp>

#include <cmath>

CCMATH_BENCH_BINARY_COMPARE(basic, fdim, return std::fdim(x, y);, return ccm::fdim(x, y);)

BENCHMARK_MAIN();
