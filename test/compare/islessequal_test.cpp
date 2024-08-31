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
#include <ccmath/ccmath.hpp>

TEST(CcmathCompareTests, IsLessEqual)
{
	// Test that islessequal is static_assert-able
    static_assert(ccm::islessequal(1.0, 0.0) == false, "islessequal has failed testing that it is static_assert-able!");

	EXPECT_EQ(ccm::islessequal(1.0, 1.0), std::islessequal(1.0, 1.0));
	EXPECT_EQ(ccm::islessequal(1.0, 0.0), std::islessequal(1.0, 0.0));
	EXPECT_EQ(ccm::islessequal(0.0, 1.0), std::islessequal(0.0, 1.0));
	EXPECT_EQ(ccm::islessequal(0.0, 0.0), std::islessequal(0.0, 0.0));
	EXPECT_EQ(ccm::islessequal(-1.0, 0.0), std::islessequal(-1.0, 0.0));
	EXPECT_EQ(ccm::islessequal(0.0, -1.0), std::islessequal(0.0, -1.0));
	EXPECT_EQ(ccm::islessequal(-1.0, -1.0), std::islessequal(-1.0, -1.0));

}
