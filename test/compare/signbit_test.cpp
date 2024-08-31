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
#include <ccmath/ccmath.hpp>
#include <limits>


TEST(CcmathCompareTests, Signbit)
{
	// Test that signbit is static_assert-able
    static_assert(!ccm::signbit(1.0), "signbit has failed testing that it is static_assert-able!");

	// MSVC has issues when std::signbit is called using an integer due to ambiguity
	EXPECT_EQ(ccm::signbit(1.0), std::signbit(1.0));
    EXPECT_EQ(ccm::signbit(-1.0), std::signbit(-1.0));
    EXPECT_EQ(ccm::signbit(0.0), std::signbit(0.0));
    EXPECT_EQ(ccm::signbit(-0.0), std::signbit(-0.0));
    EXPECT_EQ(ccm::signbit(std::numeric_limits<double>::infinity()), std::signbit(std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::signbit(-std::numeric_limits<double>::infinity()), std::signbit(-std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::signbit(std::numeric_limits<double>::quiet_NaN()), std::signbit(std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(ccm::signbit(-std::numeric_limits<double>::quiet_NaN()), std::signbit(-std::numeric_limits<double>::quiet_NaN()));
}
