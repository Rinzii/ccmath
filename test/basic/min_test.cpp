/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <gtest/gtest.h>

#include <ccmath/detail/basic/min.hpp>
#include <cmath>
#include <limits>


TEST(CcmathBasicTests, Min)
{
	// Test the ccm::abs function against std::abs
	EXPECT_EQ(ccm::min(1, 2), std::min(1, 2));
	EXPECT_EQ(ccm::min(2, 1), std::min(2, 1));
	EXPECT_EQ(ccm::min(1, 1), std::min(1, 1));
	EXPECT_EQ(ccm::min(0, 0), std::min(0, 0));
	EXPECT_EQ(ccm::min(-1, 0), std::min(-1, 0));
	EXPECT_EQ(ccm::min(0, -1), std::min(0, -1));
	EXPECT_EQ(ccm::min(-1, -1), std::min(-1, -1));

	// now floats
	EXPECT_EQ(ccm::min(1.0f, 2.0f), std::min(1.0f, 2.0f));
	EXPECT_EQ(ccm::min(2.0f, 1.0f), std::min(2.0f, 1.0f));
	EXPECT_EQ(ccm::min(1.0f, 1.0f), std::min(1.0f, 1.0f));
	EXPECT_EQ(ccm::min(0.0f, 0.0f), std::min(0.0f, 0.0f));
	EXPECT_EQ(ccm::min(-1.0f, 0.0f), std::min(-1.0f, 0.0f));
	EXPECT_EQ(ccm::min(0.0f, -1.0f), std::min(0.0f, -1.0f));
	EXPECT_EQ(ccm::min(-1.0f, -1.0f), std::min(-1.0f, -1.0f));

	// Now mix and match types
	EXPECT_EQ(ccm::fmin(1, 2.0f), std::fmin(1, 2.0f));
	EXPECT_EQ(ccm::fmin(2.0f, 1), std::fmin(2.0f, 1));
	EXPECT_EQ(ccm::fmin(1, 1.0f), std::fmin(1, 1.0f));
	EXPECT_EQ(ccm::fmin(0.0f, 0), std::fmin(0.0f, 0));
	EXPECT_EQ(ccm::fmin(-1, 0.0f), std::fmin(-1, 0.0f));
	EXPECT_EQ(ccm::fmin(0.0f, -1), std::fmin(0.0f, -1));
	EXPECT_EQ(ccm::fmin(-1.0f, -1), std::fmin(-1.0f, -1));

}
