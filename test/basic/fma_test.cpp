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

// TODO: Add more tests for fma

TEST(CcmathBasicTests, Fma)
{
	// Test that fma works with static_assert
	static_assert(ccm::fma(1, 2, 3) == 5, "fma has failed testing that it is static_assert-able!");

    EXPECT_EQ(ccm::fma(1.0, 2.0, 3.0), std::fma(1.0, 2.0, 3.0));
	EXPECT_EQ(ccm::fma(1.0f, 2.0f, 3.0f), std::fma(1.0f, 2.0f, 3.0f));
	EXPECT_EQ(ccm::fma(1.0L, 2.0L, 3.0L), std::fma(1.0L, 2.0L, 3.0L));

	// Test edge cases
	EXPECT_EQ(ccm::fma(0.0, 0.0, 0.0), std::fma(0.0, 0.0, 0.0));
	EXPECT_EQ(ccm::fma(-0.0, -0.0, -0.0), std::fma(-0.0, -0.0, -0.0));


}
