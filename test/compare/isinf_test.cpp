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

// TODO: add more tests for isinf

TEST(CcmathCompareTests, IsInf)
{
	// test isinf is static_assert-able
	static_assert(ccm::isinf(1.0) == false, "isinf has failed testing that it is static_assert-able!");

	EXPECT_EQ(ccm::isinf(1.0), std::isinf(1.0));
	EXPECT_EQ(ccm::isinf(0.0), std::isinf(0.0));
	EXPECT_EQ(ccm::isinf(-1.0), std::isinf(-1.0));
	EXPECT_EQ(ccm::isinf(std::numeric_limits<double>::infinity()), std::isinf(std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::isinf(-std::numeric_limits<double>::infinity()), std::isinf(-std::numeric_limits<double>::infinity()));
}
