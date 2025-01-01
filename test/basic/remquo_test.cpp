/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <ccmath/ccmath.hpp>
#include <gtest/gtest.h>
#include <cmath>
#include <limits>

constexpr double get_ccm_rem(double x, double y)
{
	int quotient{0};
	double remainder = ccm::remquo(x, y, &quotient);
	return remainder;
}

constexpr int get_ccm_quo(double x, double y)
{
	int quotient{0};
	ccm::remquo(x, y, &quotient); // remainder = 1
	return quotient;
}

double get_std_rem(double x, double y)
{
	int quotient{0};
	double remainder = std::remquo(x, y, &quotient);
	return remainder;
}

int get_std_quo(double x, double y)
{
	int quotient{0};
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

	// Test with positive values
	EXPECT_EQ(get_ccm_quo(7.0, 2.0), get_std_quo(7.0, 2.0));
	EXPECT_EQ(get_ccm_rem(7.0, 2.0), get_std_rem(7.0, 2.0));

	// Test with negative values
	EXPECT_EQ(get_ccm_quo(-7.0, 2.0), get_std_quo(-7.0, 2.0));
	EXPECT_EQ(get_ccm_rem(-7.0, 2.0), get_std_rem(-7.0, 2.0));

	// Test with zero values
	EXPECT_EQ(get_ccm_quo(0.0, 2.0), get_std_quo(0.0, 2.0));
	EXPECT_EQ(get_ccm_rem(0.0, 2.0), get_std_rem(0.0, 2.0));

	/* TODO: These test are failing on the CI, but not on my local machine. Investigate why.
	// Test with infinity
	bool isCcmLeftInfinityNegative = (std::signbit(get_ccm_rem(std::numeric_limits<double>::infinity(), 2.0)) == true); // NOLINT
	bool isStdLeftInfinityNegative = (std::signbit(get_std_rem(std::numeric_limits<double>::infinity(), 2.0)) == true); // NOLINT
	bool didCcmLeftInfinityReturnNan = std::isnan(get_ccm_rem(std::numeric_limits<double>::infinity(), 2.0));
	bool didStdLeftInfinityReturnNan = std::isnan(get_std_rem(std::numeric_limits<double>::infinity(), 2.0));
	EXPECT_EQ(isCcmLeftInfinityNegative, isStdLeftInfinityNegative);
	EXPECT_EQ(didCcmLeftInfinityReturnNan, didStdLeftInfinityReturnNan);
	EXPECT_EQ(get_ccm_quo(std::numeric_limits<double>::infinity(), 2.0), get_std_quo(std::numeric_limits<double>::infinity(), 2.0));

	// Test with negative infinity
	bool isCcmLeftNegativeInfinityNegative = (std::signbit(get_ccm_rem(-std::numeric_limits<double>::infinity(), 2.0)) == true); // NOLINT
	bool isStdLeftNegativeInfinityNegative = (std::signbit(get_std_rem(-std::numeric_limits<double>::infinity(), 2.0)) == true); // NOLINT
	bool didCcmLeftNegativeInfinityReturnNan = std::isnan(get_ccm_rem(-std::numeric_limits<double>::infinity(), 2.0));
	bool didStdLeftNegativeInfinityReturnNan = std::isnan(get_std_rem(-std::numeric_limits<double>::infinity(), 2.0));
	EXPECT_EQ(isCcmLeftNegativeInfinityNegative, isStdLeftNegativeInfinityNegative);
	EXPECT_EQ(didCcmLeftNegativeInfinityReturnNan, didStdLeftNegativeInfinityReturnNan);
	EXPECT_EQ(get_ccm_quo(-std::numeric_limits<double>::infinity(), 2.0), get_std_quo(-std::numeric_limits<double>::infinity(), 2.0));

	// Test with NaN
	bool isCcmLeftNanNegative = (std::signbit(get_ccm_rem(std::numeric_limits<double>::quiet_NaN(), 2.0)) == true &&
	std::isnan(get_ccm_rem(std::numeric_limits<double>::quiet_NaN(), 2.0)) == true); // NOLINT bool isStdLeftNanNegative =
	(std::signbit(get_std_rem(std::numeric_limits<double>::quiet_NaN(), 2.0)) == true && std::isnan(get_std_rem(std::numeric_limits<double>::quiet_NaN(), 2.0))
	== true); // NOLINT bool didCcmLeftNanReturnNan = std::isnan(get_ccm_rem(std::numeric_limits<double>::quiet_NaN(), 2.0)); bool didStdLeftNanReturnNan =
	std::isnan(get_std_rem(std::numeric_limits<double>::quiet_NaN(), 2.0)); EXPECT_EQ(isCcmLeftNanNegative, isStdLeftNanNegative);
	EXPECT_EQ(didCcmLeftNanReturnNan, didStdLeftNanReturnNan);
	EXPECT_EQ(get_ccm_quo(std::numeric_limits<double>::quiet_NaN(), 2.0), get_std_quo(std::numeric_limits<double>::quiet_NaN(), 2.0));

	// Test with negative NaN
	bool isCcmLeftNegativeNanNegative = (std::signbit(get_ccm_rem(-std::numeric_limits<double>::quiet_NaN(), 2.0)) == true &&
	std::isnan(get_ccm_rem(-std::numeric_limits<double>::quiet_NaN(), 2.0)) == true); // NOLINT bool isStdLeftNegativeNanNegative =
	(std::signbit(get_std_rem(-std::numeric_limits<double>::quiet_NaN(), 2.0)) == true &&
	std::isnan(get_std_rem(-std::numeric_limits<double>::quiet_NaN(), 2.0)) == true); // NOLINT bool didCcmLeftNegativeNanReturnNan =
	std::isnan(get_ccm_rem(-std::numeric_limits<double>::quiet_NaN(), 2.0)); bool didStdLeftNegativeNanReturnNan =
	std::isnan(get_std_rem(-std::numeric_limits<double>::quiet_NaN(), 2.0)); EXPECT_EQ(isCcmLeftNegativeNanNegative, isStdLeftNegativeNanNegative);
	EXPECT_EQ(didCcmLeftNegativeNanReturnNan, didStdLeftNegativeNanReturnNan);
	EXPECT_EQ(get_ccm_quo(-std::numeric_limits<double>::quiet_NaN(), 2.0), get_std_quo(-std::numeric_limits<double>::quiet_NaN(), 2.0));

	 */
	// TODO: Add more test cases for remquo.
}
