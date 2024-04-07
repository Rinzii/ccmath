/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <gtest/gtest.h>

#include <ccmath/ccmath.hpp>
#include <cmath>

TEST(CcmathBasicTests, LerpStaticAssert)
{
    static_assert(ccm::lerp(1, 2, 0.5) == 1.5, "lerp has failed testing that it is static_assert-able!");
}

TEST(CcmathBasicTests, LerpFloat)
{
    EXPECT_EQ(ccm::lerp(1.0F, 2.0F, 0.0F), std::lerp(1.0F, 2.0F, 0.0F));
	EXPECT_EQ(ccm::lerp(1.0F, 2.0F, 1.0F), std::lerp(1.0F, 2.0F, 1.0F));
	EXPECT_EQ(ccm::lerp(4.0F, 4.0F, 0.5F), std::lerp(4.0F, 4.0F, 0.5F));

	EXPECT_EQ(ccm::lerp(1e8F, 1.0F, 0.5F), std::lerp(1e8F, 1.0F, 0.5F));


	// NOLINTBEGIN
	for (float t = -2.0; t <= 2.0; t += static_cast<float>(0.5))
	{
		// Testing extrapolation
		// Expected values are: -5 -2.5 0 2.5 5 7.5 10 12.5 15
		EXPECT_EQ(ccm::lerp(5.0, 10.0, t), std::lerp(5.0, 10.0, t)) << "ccm::lerp and std::lerp are not the same with t = " << t;
	}
	// NOLINTEND
}
