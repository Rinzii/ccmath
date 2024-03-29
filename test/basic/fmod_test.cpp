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
#include <limits>
#include <type_traits>


TEST(CcmathBasicTests, Fmod)
{

	// Test that fmod works with static_assert
	static_assert(ccm::fmod(1, 2) == 1, "fmod has failed testing that it is static_assert-able!");


    // Test fmod with floating point numbers
    EXPECT_FLOAT_EQ(ccm::fmod(10.0f, 3.0f), std::fmod(10.0f, 3.0f));
    EXPECT_FLOAT_EQ(ccm::fmod(10.0f, -3.0f), std::fmod(10.0f, -3.0f));
    EXPECT_FLOAT_EQ(ccm::fmod(-10.0f, 3.0f), std::fmod(-10.0f, 3.0f));
    EXPECT_FLOAT_EQ(ccm::fmod(-10.0f, -3.0f), std::fmod(-10.0f, -3.0f));
	EXPECT_FLOAT_EQ(ccm::fmod(0.0f, 3.0f), std::fmod(0.0f, 3.0f));


	// This is a tough test as it forces rounding precision issues.
	//EXPECT_FLOAT_EQ(ccm::fmod(30.508474576271183309f, 6.1016949152542370172f), std::fmod(30.508474576271183309f, 6.1016949152542370172f));

	// Test fmod with integer numbers
	EXPECT_FLOAT_EQ(ccm::fmod(10, 3), std::fmod(10, 3));


	/// Test Edge Cases

	// Test for edge case where if x is ±0 and y is not zero, ±0 is returned.
	EXPECT_FLOAT_EQ(ccm::fmod(0.0f, 1.0f), std::fmod(0.0f, 1.0f));
	EXPECT_FLOAT_EQ(ccm::fmod(-0.0f, 1.0f), std::fmod(-0.0f, 1.0f));

	// Test for edge case where if x is ±∞ and y is not NaN, NaN is returned.
	auto testForNanCcmIfXIsInfAndYIsNotNan = std::isnan(ccm::fmod(10.0f, 0.0f));
	auto testForNanStdIfXIsInfAndYIsNotNan = std::isnan(std::fmod(10.0f, 0.0f));
	bool isCcmNanSameAsStdNanIfXIsInfAndYIsNotNan = testForNanCcmIfXIsInfAndYIsNotNan == testForNanStdIfXIsInfAndYIsNotNan;
	EXPECT_TRUE(isCcmNanSameAsStdNanIfXIsInfAndYIsNotNan);

	// Test for edge case where if y is ±0 and x is not NaN, NaN is returned.
    auto testForNanCcmIfYIsZeroAndXIsNotNan = std::isnan(ccm::fmod(10.0f, 0.0f));
	auto testForNanStdIfYIsZeroAndXIsNotNan = std::isnan(std::fmod(10.0f, 0.0f));
	bool isCcmNanSameAsStdNanIfYIsZeroAndXIsNotNan = testForNanCcmIfYIsZeroAndXIsNotNan == testForNanStdIfYIsZeroAndXIsNotNan;
	EXPECT_TRUE(isCcmNanSameAsStdNanIfYIsZeroAndXIsNotNan);

	// Test for edge case where if y is ±∞ and x is finite, x is returned.
	EXPECT_FLOAT_EQ(ccm::fmod(10.0f, std::numeric_limits<float>::infinity()), std::fmod(10.0f, std::numeric_limits<float>::infinity()));
	EXPECT_FLOAT_EQ(ccm::fmod(10.0f, -std::numeric_limits<float>::infinity()), std::fmod(10.0f, -std::numeric_limits<float>::infinity()));

	// Test for edge case where if either argument is NaN, NaN is returned.
	auto testForNanCcmIfEitherArgumentIsNan = std::isnan(ccm::fmod(std::numeric_limits<float>::quiet_NaN(), 10.0f));
	auto testForNanStdIfEitherArgumentIsNan = std::isnan(std::fmod(std::numeric_limits<float>::quiet_NaN(), 10.0f));
	bool isCcmNanSameAsStdNanIfEitherArgumentIsNan = testForNanCcmIfEitherArgumentIsNan == testForNanStdIfEitherArgumentIsNan;
	EXPECT_TRUE(isCcmNanSameAsStdNanIfEitherArgumentIsNan);

	testForNanCcmIfEitherArgumentIsNan = std::isnan(ccm::fmod(10.0f, std::numeric_limits<float>::quiet_NaN()));
	testForNanStdIfEitherArgumentIsNan = std::isnan(std::fmod(10.0f, std::numeric_limits<float>::quiet_NaN()));
	isCcmNanSameAsStdNanIfEitherArgumentIsNan = testForNanCcmIfEitherArgumentIsNan == testForNanStdIfEitherArgumentIsNan;
	EXPECT_TRUE(isCcmNanSameAsStdNanIfEitherArgumentIsNan);






}
