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

TEST(CcmathCompareTests, IsLess)
{
	// Test that isless is static_assert-able
	static_assert(ccm::isless(1.0, 0.0) == false, "isless has failed testing that it is static_assert-able!");

	EXPECT_EQ(ccm::isless(1.0, 1.0), std::isless(1.0, 1.0));
	EXPECT_EQ(ccm::isless(1.0, 0.0), std::isless(1.0, 0.0));
	EXPECT_EQ(ccm::isless(0.0, 1.0), std::isless(0.0, 1.0));
	EXPECT_EQ(ccm::isless(0.0, 0.0), std::isless(0.0, 0.0));
	EXPECT_EQ(ccm::isless(-1.0, 0.0), std::isless(-1.0, 0.0));
	EXPECT_EQ(ccm::isless(0.0, -1.0), std::isless(0.0, -1.0));
	EXPECT_EQ(ccm::isless(-1.0, -1.0), std::isless(-1.0, -1.0));
}
