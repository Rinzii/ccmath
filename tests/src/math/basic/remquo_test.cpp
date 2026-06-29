/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "utils/std_compare.hpp"

#include <gtest/gtest.h>

#include <ccmath/ccmath.hpp>

#include <cmath>
#include <limits>

constexpr double get_ccm_rem(double x, double y)
{
	int quotient{ 0 };
	double remainder = ccm::remquo(x, y, &quotient);
	return remainder;
}

constexpr int get_ccm_quo(double x, double y)
{
	int quotient{ 0 };
	ccm::remquo(x, y, &quotient); // remainder = 1
	return quotient;
}

double get_std_rem(double x, double y)
{
	int quotient{ 0 };
	double remainder = std::remquo(x, y, &quotient);
	return remainder;
}

int get_std_quo(double x, double y)
{
	int quotient{ 0 };
	std::remquo(x, y, &quotient);
	return quotient;
}

TEST(CcmathBasicTests, Remquo)
{
	// Test that remquo can be uses in a static_assert
	constexpr double sa_x		  = -7.0;
	constexpr double sa_y		  = 2.0;
	constexpr int sa_quotient	  = get_ccm_quo(sa_x, sa_y); // quotient = -4
	constexpr double sa_remainder = get_ccm_rem(sa_x, sa_y); // remainder = 1
	static_assert(sa_quotient == -4, "sa_quotient == -4");
	static_assert(sa_remainder == 1, "sa_quotient == 1");

	// abs(x / y) well above 2^53 must reduce exactly at compile time. remquo coarse-reduces with
	// ccm::fmod, so this locks in that the exact fmod reduction propagates through remquo.
	static_assert(get_ccm_rem(1e30, 3.0) == 1.0, "remquo(1e30, 3) remainder must be 1");
	static_assert(get_ccm_rem(-1e30, 3.0) == -1.0, "remquo(-1e30, 3) remainder must be -1");

	// Test with positive values
	ccm::test::ExpectRemquoMatchesStd(7.0, 2.0);

	// Test with negative values
	ccm::test::ExpectRemquoMatchesStd(-7.0, 2.0);

	// Test with zero values
	ccm::test::ExpectRemquoMatchesStd(0.0, 2.0);

	// Domain cases: the standard returns a NaN remainder when x is infinite or y is zero, and
	// propagates a NaN argument. The quotient (and the NaN sign) are unspecified in these cases, so only
	// the NaN result is asserted. The earlier version compared quo and signbit here, which is what made
	// it pass locally but fail on other platforms.
	constexpr double inf = std::numeric_limits<double>::infinity();
	constexpr double nan = std::numeric_limits<double>::quiet_NaN();
	EXPECT_TRUE(std::isnan(get_ccm_rem(inf, 2.0)));
	EXPECT_TRUE(std::isnan(get_ccm_rem(-inf, 2.0)));
	EXPECT_TRUE(std::isnan(get_ccm_rem(2.0, 0.0)));
	EXPECT_TRUE(std::isnan(get_ccm_rem(nan, 2.0)));
	EXPECT_TRUE(std::isnan(get_ccm_rem(2.0, nan)));

	// std agrees that these are NaN, confirming the shared contract.
	EXPECT_TRUE(std::isnan(get_std_rem(inf, 2.0)));
	EXPECT_TRUE(std::isnan(get_std_rem(2.0, 0.0)));
}
