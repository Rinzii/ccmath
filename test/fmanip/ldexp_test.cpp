/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present ccmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
*/

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include "ccmath/ccmath.hpp"

TEST(CcmathFmanipTests, Ldexp)
{
	static_assert(ccm::ldexp(1.0, 0) == 1.0, "ldexp has failed testing that it is static_assert-able!");

	EXPECT_FLOAT_EQ(std::ldexp(5.0, 4), ccm::ldexp(5.0, 4));
	EXPECT_FLOAT_EQ(std::ldexp(5.0, -4), ccm::ldexp(5.0, -4));
	EXPECT_FLOAT_EQ(std::ldexp(-0.0, 1), ccm::ldexp(-0.0, 1));
	EXPECT_FLOAT_EQ(std::ldexp(0.0, 1), ccm::ldexp(0.0, 1));
	EXPECT_FLOAT_EQ(std::ldexp(720.32f, 22), ccm::ldexp(720.32f, 22));
	EXPECT_FLOAT_EQ(std::ldexp(1, -1074), ccm::ldexp(1, -1074));
	EXPECT_FLOAT_EQ(std::ldexp(1, 1024), ccm::ldexp(1, 1024));

	EXPECT_FLOAT_EQ(std::ldexp(std::numeric_limits<float>::max(), std::numeric_limits<int>::max()),
					ccm::ldexp(std::numeric_limits<float>::max(), std::numeric_limits<int>::max()));

	EXPECT_FLOAT_EQ(std::ldexp(std::numeric_limits<float>::min(), std::numeric_limits<int>::min()),
					ccm::ldexp(std::numeric_limits<float>::min(), std::numeric_limits<int>::min()));

	EXPECT_FLOAT_EQ(std::ldexp(std::numeric_limits<float>::min(), std::numeric_limits<int>::max()),
					ccm::ldexp(std::numeric_limits<float>::min(), std::numeric_limits<int>::max()));

	EXPECT_FLOAT_EQ(std::ldexp(std::numeric_limits<float>::max(), std::numeric_limits<int>::min()),
					ccm::ldexp(std::numeric_limits<float>::max(), std::numeric_limits<int>::min()));

	EXPECT_FLOAT_EQ(ccm::ldexp(std::numeric_limits<float>::infinity(), 10), std::ldexp(std::numeric_limits<float>::infinity(), 10));

	// Test for edge case where if either argument is NaN, NaN is returned.
	auto testForNanCcmIfEitherArgumentIsNan		   = std::isnan(ccm::ldexp(std::numeric_limits<float>::quiet_NaN(), 10));
	auto testForNanStdIfEitherArgumentIsNan		   = std::isnan(std::ldexp(std::numeric_limits<float>::quiet_NaN(), 10));
	bool isCcmNanSameAsStdNanIfEitherArgumentIsNan = testForNanCcmIfEitherArgumentIsNan == testForNanStdIfEitherArgumentIsNan;
	EXPECT_TRUE(isCcmNanSameAsStdNanIfEitherArgumentIsNan);
}

