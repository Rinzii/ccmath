/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <random>

#include "ccmath/ccmath.hpp"
#include "ccmath/internal/config/long_double_format.hpp"
#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
#include "utils/ulp_suite.hpp"

namespace
{
#define REQUIRE_POWL_LD80()                                                                                                        \
	do                                                                                                                             \
	{                                                                                                                              \
		if (ccm::config::detect_long_double_format() != ccm::config::LongDoubleFormat::X87Extended)                                \
		{                                                                                                                          \
			GTEST_SKIP() << "x87 80-bit long double required";                                                                     \
		}                                                                                                                          \
	} while (false)

	void ExpectPowlMatchesStd(long double base, long double exponent)
	{
		const long double actual = ccm::gen::pow_gen(base, exponent);
		const long double expected = std::pow(base, exponent);
		if (std::isnan(expected))
		{
			EXPECT_TRUE(std::isnan(actual));
			return;
		}
		ccm::test::ExpectSameFloatingAsStd(actual, expected);
	}
} // namespace

TEST(PowlLd80GeneralFinite, NearOneAndPowerOfTwo)
{
	REQUIRE_POWL_LD80();

	ExpectPowlMatchesStd(1.0001L, 2.0L);
	ExpectPowlMatchesStd(0.9999L, -1.5L);
	ExpectPowlMatchesStd(2.0L, 0.5L);
	ExpectPowlMatchesStd(2.0L, -0.25L);
	ExpectPowlMatchesStd(8.0L, 1.0L / 3.0L);
}

TEST(PowlLd80GeneralFinite, ModerateExponentRange)
{
	REQUIRE_POWL_LD80();

	ExpectPowlMatchesStd(1.5L, 10.0L);
	ExpectPowlMatchesStd(0.75L, -12.0L);
	ExpectPowlMatchesStd(3.14159265358979323846L, 2.71828182845904523536L);
}

TEST(PowlLd80GeneralFinite, LargeAndSmallMagnitude)
{
	REQUIRE_POWL_LD80();

	ExpectPowlMatchesStd(1.0000000000001L, 1000.0L);
	ExpectPowlMatchesStd(1.0000000000001L, -1000.0L);
	ExpectPowlMatchesStd(0x1.0p-20L, 0.125L);
}

TEST(PowlLd80GeneralFinite, RandomFinitePositiveBase)
{
	REQUIRE_POWL_LD80();

	std::mt19937_64 rng(0xC0FFEEULL);
	std::uniform_real_distribution<long double> base_dist(0x1.0p-30L, 0x1.0p30L);
	std::uniform_real_distribution<long double> exp_dist(-500.0L, 500.0L);

	for (int i = 0; i < 256; ++i)
	{
		const long double base = base_dist(rng);
		const long double exponent = exp_dist(rng);
		if (base == 1.0L) { continue; }
		ExpectPowlMatchesStd(base, exponent);
	}
}
