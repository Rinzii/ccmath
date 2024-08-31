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

TEST(CcmathCompareTests, IsLessGreater)
{
	// Test that islessgreater is static_assert-able
    static_assert(ccm::islessgreater(1.0, 0.0) == true, "islessgreater has failed testing that it is static_assert-able!");

	EXPECT_EQ(ccm::islessgreater(1.0, 1.0), std::islessgreater(1.0, 1.0));
    EXPECT_EQ(ccm::islessgreater(1.0, 0.0), std::islessgreater(1.0, 0.0));
    EXPECT_EQ(ccm::islessgreater(0.0, 1.0), std::islessgreater(0.0, 1.0));
    EXPECT_EQ(ccm::islessgreater(0.0, 0.0), std::islessgreater(0.0, 0.0));
    EXPECT_EQ(ccm::islessgreater(-1.0, 0.0), std::islessgreater(-1.0, 0.0));
    EXPECT_EQ(ccm::islessgreater(0.0, -1.0), std::islessgreater(0.0, -1.0));
    EXPECT_EQ(ccm::islessgreater(-1.0, -1.0), std::islessgreater(-1.0, -1.0));
}
