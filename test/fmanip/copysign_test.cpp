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

TEST(CcmathFmanipTests, Copysign)
{
	EXPECT_EQ(ccm::copysign(1.0, +2.0), std::copysign(1.0, +2.0));
	EXPECT_EQ(ccm::copysign(1.0, -2.0), std::copysign(1.0, -2.0));
	EXPECT_EQ(ccm::copysign(std::numeric_limits<double>::infinity(), -2.0), std::copysign(std::numeric_limits<double>::infinity(), -2.0));

	bool isCcmCopysignNan = std::isnan(ccm::copysign(std::numeric_limits<double>::quiet_NaN(), -2.0));
	bool isStdCopysignNan = std::isnan(std::copysign(std::numeric_limits<double>::quiet_NaN(), -2.0));
	bool isCcmCopysignNanNegative = std::signbit(ccm::copysign(std::numeric_limits<double>::quiet_NaN(), -2.0));
	bool isStdCopysignNanNegative = std::signbit(std::copysign(std::numeric_limits<double>::quiet_NaN(), -2.0));
	EXPECT_EQ(isCcmCopysignNan, isStdCopysignNan);
	EXPECT_EQ(isCcmCopysignNanNegative, isStdCopysignNanNegative);

	// TODO: Add more tests
}

