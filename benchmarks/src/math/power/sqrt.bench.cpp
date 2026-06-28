#include "shared/register.hpp"

#include <ccmath/ccmath.hpp>

#include <cmath>

namespace
{
	inline double sqrt_rt(double v)
	{
		return ccm::sqrt(v);
	}
} // namespace

CCMATH_BENCH_UNARY_COMPARE_PATHS(
	power, sqrt, return std::sqrt(static_cast<double>(x)), return ccm::sqrt(static_cast<double>(x)), return sqrt_rt(static_cast<double>(x)))

CCMATH_BENCH_UNARY_PROFILE_PATHS(power, sqrt, positive_finite_general, 4.0, return std::sqrt(x), return ccm::sqrt(x), return sqrt_rt(x))

BENCHMARK_MAIN();
