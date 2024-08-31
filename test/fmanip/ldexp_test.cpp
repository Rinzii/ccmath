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
#include "ccmath/ccmath.hpp"

TEST(CcmathFmanipTests, Ldexp)
{
	static_assert(ccm::ldexp(1.0, 0) == 1.0, "ldexp has failed testing that it is static_assert-able!");

	EXPECT_EQ(std::ldexp(5.0, 4), ccm::ldexp(5.0, 4));
	EXPECT_EQ(std::ldexp(5.f, 4), ccm::ldexp(5.f, 4));

	EXPECT_EQ(std::ldexp(-5.0, -4), ccm::ldexp(-5.0, -4));
	EXPECT_EQ(std::ldexp(5.f, -4), ccm::ldexp(5.f, -4));

	EXPECT_EQ(std::ldexp(-0.0, 1), ccm::ldexp(-0.0, 1));
	EXPECT_EQ(std::ldexp(-0.f, 1), ccm::ldexp(-0.f, 1));

	EXPECT_EQ(std::ldexp(0.0, 1), ccm::ldexp(0.0, 1));
	EXPECT_EQ(std::ldexp(0.f, 1), ccm::ldexp(0.f, 1));

	EXPECT_EQ(std::ldexp(720.32, 22), ccm::ldexp(720.32, 22));
	EXPECT_EQ(std::ldexp(720.32f, 22), ccm::ldexp(720.32f, 22));

	EXPECT_EQ(std::ldexp(1, -1074), ccm::ldexp(1, -1074));
	EXPECT_EQ(std::ldexp(1.f, -1074), ccm::ldexp(1.f, -1074));

	EXPECT_EQ(std::ldexp(1, 1024), ccm::ldexp(1, 1024));
	EXPECT_EQ(std::ldexp(1.f, 1024), ccm::ldexp(1.f, 1024));

	EXPECT_EQ(std::ldexp(std::numeric_limits<double>::max(), std::numeric_limits<int>::max()),
					ccm::ldexp(std::numeric_limits<double>::max(), std::numeric_limits<int>::max()));
	EXPECT_EQ(std::ldexp(std::numeric_limits<float>::max(), std::numeric_limits<int>::max()),
					ccm::ldexp(std::numeric_limits<float>::max(), std::numeric_limits<int>::max()));

	EXPECT_EQ(std::ldexp(std::numeric_limits<double>::min(), std::numeric_limits<int>::min()),
					ccm::ldexp(std::numeric_limits<double>::min(), std::numeric_limits<int>::min()));
	EXPECT_EQ(std::ldexp(std::numeric_limits<float>::min(), std::numeric_limits<int>::min()),
					ccm::ldexp(std::numeric_limits<float>::min(), std::numeric_limits<int>::min()));

	EXPECT_EQ(std::ldexp(std::numeric_limits<double>::min(), std::numeric_limits<int>::max()),
					ccm::ldexp(std::numeric_limits<double>::min(), std::numeric_limits<int>::max()));
	EXPECT_EQ(std::ldexp(std::numeric_limits<float>::min(), std::numeric_limits<int>::max()),
					ccm::ldexp(std::numeric_limits<float>::min(), std::numeric_limits<int>::max()));

	EXPECT_EQ(std::ldexp(std::numeric_limits<double>::max(), std::numeric_limits<int>::min()),
					ccm::ldexp(std::numeric_limits<double>::max(), std::numeric_limits<int>::min()));
	EXPECT_EQ(std::ldexp(std::numeric_limits<float>::max(), std::numeric_limits<int>::min()),
					ccm::ldexp(std::numeric_limits<float>::max(), std::numeric_limits<int>::min()));

	EXPECT_EQ(ccm::ldexp(std::numeric_limits<double>::infinity(), 10), std::ldexp(std::numeric_limits<double>::infinity(), 10));
	EXPECT_EQ(ccm::ldexp(std::numeric_limits<float>::infinity(), 10), std::ldexp(std::numeric_limits<float>::infinity(), 10));

	// Test for edge case where if either argument is NaN, NaN is returned.
	auto testForNanCcmIfEitherArgumentIsNand	   = std::isnan(ccm::ldexp(std::numeric_limits<double>::quiet_NaN(), 10));
	auto testForNanStdIfEitherArgumentIsNand	   = std::isnan(std::ldexp(std::numeric_limits<double>::quiet_NaN(), 10));
	bool isCcmNanSameAsStdNanIfEitherArgumentIsNand = testForNanCcmIfEitherArgumentIsNand == testForNanStdIfEitherArgumentIsNand;
	EXPECT_TRUE(isCcmNanSameAsStdNanIfEitherArgumentIsNand);
	auto testForNanCcmIfEitherArgumentIsNanf	   = std::isnan(ccm::ldexp(std::numeric_limits<float>::quiet_NaN(), 10));
	auto testForNanStdIfEitherArgumentIsNanf	   = std::isnan(std::ldexp(std::numeric_limits<float>::quiet_NaN(), 10));
	bool isCcmNanSameAsStdNanIfEitherArgumentIsNanf = testForNanCcmIfEitherArgumentIsNanf == testForNanStdIfEitherArgumentIsNanf;
	EXPECT_TRUE(isCcmNanSameAsStdNanIfEitherArgumentIsNanf);
}

