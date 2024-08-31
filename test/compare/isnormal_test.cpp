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

TEST(CcmathCompareTests, IsNormal)
{
	// Test that isnormal is static_assert-able
    static_assert(ccm::isnormal(1.0) == true, "isnormal has failed testing that it is static_assert-able!");

	// TODO: Add more tests for isnormal
	EXPECT_EQ(ccm::isnormal(1.0), std::isnormal(1.0));
    EXPECT_EQ(ccm::isnormal(0.0), std::isnormal(0.0));
    EXPECT_EQ(ccm::isnormal(-1.0), std::isnormal(-1.0));
    EXPECT_EQ(ccm::isnormal(std::numeric_limits<double>::infinity()), std::isnormal(std::numeric_limits<double>::infinity()));
    EXPECT_EQ(ccm::isnormal(-std::numeric_limits<double>::infinity()), std::isnormal(-std::numeric_limits<double>::infinity()));
    EXPECT_EQ(ccm::isnormal(std::numeric_limits<double>::quiet_NaN()), std::isnormal(std::numeric_limits<double>::quiet_NaN()));
    //EXPECT_EQ(ccm::isnormal(std::numeric_limits<double>::signaling_NaN()), std::isnormal(std::numeric_limits<double>::signaling_NaN)); // Won't work with std::isnormal
}
