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
#include "ccmath/detail/compare/islessequal.hpp"

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
