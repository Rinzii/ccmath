/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <gtest/gtest.h>

#include <ccmath/detail/basic/abs.hpp>
#include <cmath>
#include <limits>


TEST(CcmathBasicTests, Abs)
{
	// Verify that ccm::abs works with static_assert
	static_assert(ccm::abs(1) == 1, "abs has failed testing that it is static_assert-able!");

	EXPECT_EQ(ccm::abs(-0.0), std::abs(-0.0));

	// Test the ccm::abs function against std::abs
	EXPECT_EQ(ccm::abs(0), std::abs(0));



	EXPECT_EQ(ccm::abs(1), std::abs(1));
	EXPECT_EQ(ccm::abs(-1), std::abs(-1));
	EXPECT_EQ(ccm::abs(1.0), std::abs(1.0));
	EXPECT_EQ(ccm::abs(-1.0), std::abs(-1.0));
	EXPECT_EQ(ccm::abs(1.0f), std::abs(1.0f));
	EXPECT_EQ(ccm::abs(-1.0f), std::abs(-1.0f));
	EXPECT_EQ(ccm::abs(1.0L), std::abs(1.0L));
	EXPECT_EQ(ccm::abs(-1.0L), std::abs(-1.0L));
	EXPECT_EQ(ccm::abs(1.0l), std::abs(1.0l));
	EXPECT_EQ(ccm::abs(-1.0l), std::abs(-1.0l));

	// TODO: Find a way to test for NaN.

}
