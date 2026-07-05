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
		if (ccm::config::detect_long_double_format() != ccm::config::LongDoubleFormat::X87Extended)                                                            \
		{                                                                                                                                                      \
			GTEST_SKIP() << "x87 80-bit long double required";                                                                                                 \
		}                                                                                                                                                      \
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

TEST(PowlLd80SpecialCases, IdentityAndNaNRules)
{
	REQUIRE_POWL_LD80();

	EXPECT_EQ(ccm::gen::pow_gen(2.0L, 0.0L), 1.0L);
	EXPECT_EQ(ccm::gen::pow_gen(1.0L, 2.0L), 1.0L);
	EXPECT_TRUE(std::isnan(ccm::gen::pow_gen(std::numeric_limits<long double>::quiet_NaN(), 2.0L)));
	EXPECT_TRUE(std::isnan(ccm::gen::pow_gen(2.0L, std::numeric_limits<long double>::quiet_NaN())));
	EXPECT_EQ(ccm::gen::pow_gen(1.0L, std::numeric_limits<long double>::quiet_NaN()), 1.0L);
	EXPECT_EQ(ccm::gen::pow_gen(std::numeric_limits<long double>::quiet_NaN(), 0.0L), 1.0L);
}

TEST(PowlLd80SpecialCases, ZeroBaseAndPoleBehavior)
{
	REQUIRE_POWL_LD80();

	EXPECT_EQ(ccm::gen::pow_gen(0.0L, 2.0L), 0.0L);
	EXPECT_EQ(ccm::gen::pow_gen(-0.0L, 3.0L), -0.0L);
	EXPECT_EQ(ccm::gen::pow_gen(-0.0L, 2.0L), 0.0L);
	ExpectPowlMatchesStd(0.0L, -1.0L);
	ExpectPowlMatchesStd(-0.0L, -3.0L);
}

TEST(PowlLd80SpecialCases, InfiniteBaseAndExponent)
{
	REQUIRE_POWL_LD80();

	ExpectPowlMatchesStd(std::numeric_limits<long double>::infinity(), 2.0L);
	ExpectPowlMatchesStd(-std::numeric_limits<long double>::infinity(), 3.0L);
	ExpectPowlMatchesStd(-std::numeric_limits<long double>::infinity(), 2.0L);
	ExpectPowlMatchesStd(2.0L, std::numeric_limits<long double>::infinity());
	ExpectPowlMatchesStd(0.5L, -std::numeric_limits<long double>::infinity());
	ExpectPowlMatchesStd(2.0L, -std::numeric_limits<long double>::infinity());
	ExpectPowlMatchesStd(-std::numeric_limits<long double>::infinity(), 0.5L);
	EXPECT_EQ(ccm::gen::pow_gen(1.0L, std::numeric_limits<long double>::infinity()), 1.0L);
}

TEST(PowlLd80SpecialCases, NegativeBaseIntegerParity)
{
	REQUIRE_POWL_LD80();

	EXPECT_EQ(ccm::gen::pow_gen(-2.0L, 3.0L), -8.0L);
	EXPECT_EQ(ccm::gen::pow_gen(-2.0L, 4.0L), 16.0L);
	EXPECT_TRUE(std::isnan(ccm::gen::pow_gen(-2.0L, 0.5L)));
}
