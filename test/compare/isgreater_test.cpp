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

#include <ccmath/ccmath.hpp>
#include <cmath>
#include <limits>

TEST(CcmathCompareTests, IsGreater)
{
	// Test that isgreater is static_assert-able
	static_assert(ccm::isgreater(1.0, 0.0) == true, "isgreater has failed testing that it is static_assert-able!");

	// Test the basic functionality of isgreater
    EXPECT_EQ(ccm::isgreater(1.0, 0.0), std::isgreater(1.0, 0.0));
	EXPECT_EQ(ccm::isgreater(0.0, 1.0), std::isgreater(0.0, 1.0));
	EXPECT_EQ(ccm::isgreater(0.0, 0.0), std::isgreater(0.0, 0.0));
	EXPECT_EQ(ccm::isgreater(1.0, 1.0), std::isgreater(1.0, 1.0));
	EXPECT_EQ(ccm::isgreater(-1.0, 0.0), std::isgreater(-1.0, 0.0));
	EXPECT_EQ(ccm::isgreater(0.0, -1.0), std::isgreater(0.0, -1.0));
	EXPECT_EQ(ccm::isgreater(-1.0, -1.0), std::isgreater(-1.0, -1.0));
}
