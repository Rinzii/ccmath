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
#include "ccmath/detail/compare/isgreater.hpp"

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
