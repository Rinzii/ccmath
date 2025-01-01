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

#include <gtest/gtest.h>
#include <cmath>
#include <limits>

TEST(CcmathFmanipTests, Copysign)
{
	EXPECT_EQ(ccm::copysign(1.0, +2.0), std::copysign(1.0, +2.0));
	EXPECT_EQ(ccm::copysign(1.0, -2.0), std::copysign(1.0, -2.0));
	EXPECT_EQ(ccm::copysign(std::numeric_limits<double>::infinity(), -2.0), std::copysign(std::numeric_limits<double>::infinity(), -2.0));

	bool const isCcmCopysignNan			= std::isnan(ccm::copysign(std::numeric_limits<double>::quiet_NaN(), -2.0));
	bool const isStdCopysignNan			= std::isnan(std::copysign(std::numeric_limits<double>::quiet_NaN(), -2.0));
	bool const isCcmCopysignNanNegative = std::signbit(ccm::copysign(std::numeric_limits<double>::quiet_NaN(), -2.0));
	bool const isStdCopysignNanNegative = std::signbit(std::copysign(std::numeric_limits<double>::quiet_NaN(), -2.0));
	EXPECT_EQ(isCcmCopysignNan, isStdCopysignNan);
	EXPECT_EQ(isCcmCopysignNanNegative, isStdCopysignNanNegative);

	// TODO: Add more tests
}
