/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <ccmath/ccmath.hpp>

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
