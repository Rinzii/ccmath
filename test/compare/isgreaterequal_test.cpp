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

TEST(CcmathCompareTests, IsGreaterEqual)
{
	// Test that isgreaterequal is static_assert-able
    static_assert(ccm::isgreaterequal(1.0, 0.0) == true, "isgreaterequal has failed testing that it is static_assert-able!");

	EXPECT_EQ(ccm::isgreaterequal(1.0, 1.0), std::isgreaterequal(1.0, 1.0));
	EXPECT_EQ(ccm::isgreaterequal(1.0, 0.0), std::isgreaterequal(1.0, 0.0));
	EXPECT_EQ(ccm::isgreaterequal(0.0, 1.0), std::isgreaterequal(0.0, 1.0));
	EXPECT_EQ(ccm::isgreaterequal(0.0, 0.0), std::isgreaterequal(0.0, 0.0));
	EXPECT_EQ(ccm::isgreaterequal(-1.0, 0.0), std::isgreaterequal(-1.0, 0.0));
	EXPECT_EQ(ccm::isgreaterequal(0.0, -1.0), std::isgreaterequal(0.0, -1.0));
	EXPECT_EQ(ccm::isgreaterequal(-1.0, -1.0), std::isgreaterequal(-1.0, -1.0));

}
