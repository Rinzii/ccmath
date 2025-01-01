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

TEST(CcmathBasicTests, Remainder)
{
	static_assert(ccm::remainder(1.0, 1.0) == 0.0, "remainder has failed testing that it is static_assert-able!");

	EXPECT_EQ(ccm::remainder(1.0, 1.0), std::remainder(1.0, 1.0));
	EXPECT_EQ(std::isnan(ccm::remainder(1.0, 0.0)), std::isnan(std::remainder(1.0, 0.0)));
	EXPECT_EQ(std::isnan(ccm::remainder(0.0, 1.0)), std::isnan(std::remainder(0.0, 1.0)));
	EXPECT_EQ(std::isnan(ccm::remainder(0.0, 0.0)), std::isnan(std::remainder(0.0, 0.0)));
	EXPECT_EQ(ccm::remainder(-1.0, 1.0), std::remainder(-1.0, 1.0));
	EXPECT_EQ(ccm::remainder(1.0, -1.0), std::remainder(1.0, -1.0));
	EXPECT_EQ(ccm::remainder(-1.0, -1.0), std::remainder(-1.0, -1.0));
	EXPECT_EQ(std::isnan(ccm::remainder(-1.0, 0.0)), std::isnan(std::remainder(-1.0, 0.0)));
}
