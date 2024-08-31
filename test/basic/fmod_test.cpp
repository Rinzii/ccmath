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

#include <ccmath/ccmath.hpp>
#include <cmath>
#include <limits>

TEST(CcmathBasicTests, Fmod)
{

	// Test that fmod works with static_assert
	static_assert(ccm::fmod(1, 2) == 1, "fmod has failed testing that it is static_assert-able!");


    // Test fmod with floating point numbers
    EXPECT_FLOAT_EQ(ccm::fmod(10.0F, 3.0F), std::fmod(10.0F, 3.0F));
    EXPECT_FLOAT_EQ(ccm::fmod(10.0F, -3.0F), std::fmod(10.0F, -3.0F));
    EXPECT_FLOAT_EQ(ccm::fmod(-10.0F, 3.0F), std::fmod(-10.0F, 3.0F));
    EXPECT_FLOAT_EQ(ccm::fmod(-10.0F, -3.0F), std::fmod(-10.0F, -3.0F));
	EXPECT_FLOAT_EQ(ccm::fmod(0.0F, 3.0F), std::fmod(0.0F, 3.0F));


	// This is a tough test as it forces rounding precision issues.
	//EXPECT_FLOAT_EQ(ccm::fmod(30.508474576271183309f, 6.1016949152542370172f), std::fmod(30.508474576271183309f, 6.1016949152542370172f));

	// Test fmod with integer numbers
	EXPECT_EQ(ccm::fmod(10, 3), std::fmod(10, 3));


	/// Test Edge Cases

	// Test for edge case where if x is ±0 and y is not zero, ±0 is returned.
	EXPECT_FLOAT_EQ(ccm::fmod(0.0F, 1.0F), std::fmod(0.0F, 1.0F));
	EXPECT_FLOAT_EQ(ccm::fmod(-0.0F, 1.0F), std::fmod(-0.0F, 1.0F));

	// Test for edge case where if x is ±∞ and y is not NaN, NaN is returned.
	const auto testForNanCcmIfXIsInfAndYIsNotNan = std::isnan(ccm::fmod(10.0F, 0.0F));
	const auto testForNanStdIfXIsInfAndYIsNotNan = std::isnan(std::fmod(10.0F, 0.0F));
	const bool isCcmNanSameAsStdNanIfXIsInfAndYIsNotNan = testForNanCcmIfXIsInfAndYIsNotNan == testForNanStdIfXIsInfAndYIsNotNan;
	EXPECT_TRUE(isCcmNanSameAsStdNanIfXIsInfAndYIsNotNan);

	// Test for edge case where if y is ±0 and x is not NaN, NaN is returned.
    const auto testForNanCcmIfYIsZeroAndXIsNotNan = std::isnan(ccm::fmod(10.0F, 0.0F));
	const auto testForNanStdIfYIsZeroAndXIsNotNan = std::isnan(std::fmod(10.0F, 0.0F));
	const bool isCcmNanSameAsStdNanIfYIsZeroAndXIsNotNan = testForNanCcmIfYIsZeroAndXIsNotNan == testForNanStdIfYIsZeroAndXIsNotNan;
	EXPECT_TRUE(isCcmNanSameAsStdNanIfYIsZeroAndXIsNotNan);

	// Test for edge case where if y is ±∞ and x is finite, x is returned.
	EXPECT_FLOAT_EQ(ccm::fmod(10.0F, std::numeric_limits<float>::infinity()), std::fmod(10.0F, std::numeric_limits<float>::infinity()));
	EXPECT_FLOAT_EQ(ccm::fmod(10.0F, -std::numeric_limits<float>::infinity()), std::fmod(10.0F, -std::numeric_limits<float>::infinity()));

	// Test for edge case where if either argument is NaN, NaN is returned.
	auto testForNanCcmIfEitherArgumentIsNan = std::isnan(ccm::fmod(std::numeric_limits<float>::quiet_NaN(), 10.0F));
	auto testForNanStdIfEitherArgumentIsNan = std::isnan(std::fmod(std::numeric_limits<float>::quiet_NaN(), 10.0F));
	bool isCcmNanSameAsStdNanIfEitherArgumentIsNan = testForNanCcmIfEitherArgumentIsNan == testForNanStdIfEitherArgumentIsNan;
	EXPECT_TRUE(isCcmNanSameAsStdNanIfEitherArgumentIsNan);

	testForNanCcmIfEitherArgumentIsNan = std::isnan(ccm::fmod(10.0F, std::numeric_limits<float>::quiet_NaN()));
	testForNanStdIfEitherArgumentIsNan = std::isnan(std::fmod(10.0F, std::numeric_limits<float>::quiet_NaN()));
	isCcmNanSameAsStdNanIfEitherArgumentIsNan = testForNanCcmIfEitherArgumentIsNan == testForNanStdIfEitherArgumentIsNan;
	EXPECT_TRUE(isCcmNanSameAsStdNanIfEitherArgumentIsNan);






}
