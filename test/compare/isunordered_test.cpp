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

TEST(CcmathCompareTests, IsUnordered)
{
	// Test that isunordered is static_assert-able
    static_assert(ccm::isunordered(1.0, 0.0) == false, "isunordered has failed testing that it is static_assert-able!");

	EXPECT_EQ(ccm::isunordered(1.0, 1.0), std::isunordered(1.0, 1.0));
    EXPECT_EQ(ccm::isunordered(1.0, 0.0), std::isunordered(1.0, 0.0));
    EXPECT_EQ(ccm::isunordered(0.0, 1.0), std::isunordered(0.0, 1.0));
    EXPECT_EQ(ccm::isunordered(0.0, 0.0), std::isunordered(0.0, 0.0));
    EXPECT_EQ(ccm::isunordered(-1.0, 0.0), std::isunordered(-1.0, 0.0));
    EXPECT_EQ(ccm::isunordered(0.0, -1.0), std::isunordered(0.0, -1.0));
    EXPECT_EQ(ccm::isunordered(-1.0, -1.0), std::isunordered(-1.0, -1.0));

	// Check for NaN
	EXPECT_EQ(ccm::isunordered(std::numeric_limits<double>::quiet_NaN(), 1.0), std::isunordered(std::numeric_limits<double>::quiet_NaN(), 1.0));
	EXPECT_EQ(ccm::isunordered(1.0, std::numeric_limits<double>::quiet_NaN()), std::isunordered(1.0, std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(ccm::isunordered(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()), std::isunordered(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()));

}
