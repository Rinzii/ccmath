/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "ccmath/ccmath.hpp"
#include "ccmath/internal/config/long_double_format.hpp"
#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
#include "utils/ulp_suite.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>

namespace
{
#define REQUIRE_POWL_LD80()                                                                                                                                    \
	do                                                                                                                                                         \
	{                                                                                                                                                          \
		if (ccm::config::detect_long_double_format() != ccm::config::LongDoubleFormat::X87Extended) { GTEST_SKIP() << "x87 80-bit long double required"; }     \
	} while (false)

	void ExpectPowlMatchesStd(long double base, long double exponent)
	{
		const long double actual   = ccm::gen::pow_gen(base, exponent);
		const long double expected = std::pow(base, exponent);
		if (std::isnan(expected))
		{
			EXPECT_TRUE(std::isnan(actual));
			return;
		}
		ccm::test::ExpectSameFloatingAsStd(actual, expected);
	}
} // namespace

TEST(PowlLd80BoundedInt, SmallPositiveIntegerExponents)
{
	REQUIRE_POWL_LD80();

	EXPECT_EQ(ccm::gen::pow_gen(2.0L, 10.0L), 1024.0L);
	EXPECT_EQ(ccm::gen::pow_gen(3.0L, 4.0L), 81.0L);
	EXPECT_EQ(ccm::gen::pow_gen(5.0L, 0.0L), 1.0L);
	EXPECT_EQ(ccm::gen::pow_gen(-1.0L, 4.0L), 1.0L);
	EXPECT_EQ(ccm::gen::pow_gen(-1.0L, 3.0L), -1.0L);
}

TEST(PowlLd80BoundedInt, NegativeIntegerExponents)
{
	REQUIRE_POWL_LD80();

	ExpectPowlMatchesStd(2.0L, -3.0L);
	ExpectPowlMatchesStd(4.0L, -2.0L);
	ExpectPowlMatchesStd(-2.0L, -3.0L);
}

TEST(PowlLd80BoundedInt, LargeRepresentableIntegerExponent)
{
	REQUIRE_POWL_LD80();

	const long double exponent = 0x1.0p40L;
	ExpectPowlMatchesStd(1.0001L, exponent);
	ExpectPowlMatchesStd(1.0001L, -exponent);
}

TEST(PowlLd80BoundedInt, OverflowAndUnderflowSamples)
{
	REQUIRE_POWL_LD80();

	ExpectPowlMatchesStd(10.0L, 1000.0L);
	ExpectPowlMatchesStd(0.1L, 1000.0L);
	ExpectPowlMatchesStd(10.0L, -1000.0L);
}

TEST(PowlLd80BoundedInt, SignedZeroResults)
{
	REQUIRE_POWL_LD80();

	EXPECT_EQ(ccm::gen::pow_gen(-1.0L, 0.0L), 1.0L);
	EXPECT_EQ(std::signbit(ccm::gen::pow_gen(-0.0L, 3.0L)), true);
	EXPECT_EQ(std::signbit(ccm::gen::pow_gen(-0.0L, 2.0L)), false);
}
