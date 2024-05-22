/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present ccmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
*/

#include <gtest/gtest.h>

#include "ccmath/ccmath.hpp"

#include <cmath>
#include <limits>

TEST(CcmathPowerTests, Sqrt_StaticAssert)
{
	static_assert(ccm::sqrt(2.0) == ccm::sqrt(2.0), "ccm::sqrt is not a compile time constant!");
}

TEST(CcmathPowerTests, Sqrt_Double)
{
	// Test values that are 2^x
	EXPECT_EQ(ccm::sqrt(0.0), std::sqrt(0.0));
	EXPECT_EQ(ccm::sqrt(1.0), std::sqrt(1.0));
	EXPECT_EQ(ccm::sqrt(2.0), std::sqrt(2.0));
	EXPECT_EQ(ccm::sqrt(4.0), std::sqrt(4.0));
	EXPECT_EQ(ccm::sqrt(8.0), std::sqrt(8.0));
	EXPECT_EQ(ccm::sqrt(16.0), std::sqrt(16.0));
	EXPECT_EQ(ccm::sqrt(32.0), std::sqrt(32.0));
	EXPECT_EQ(ccm::sqrt(64.0), std::sqrt(64.0));
	EXPECT_EQ(ccm::sqrt(128.0), std::sqrt(128.0));
	EXPECT_EQ(ccm::sqrt(256.0), std::sqrt(256.0));

	// Test decimal values
	EXPECT_EQ(ccm::sqrt(0.1), std::sqrt(0.1));
	EXPECT_EQ(ccm::sqrt(0.2), std::sqrt(0.2));
	EXPECT_EQ(ccm::sqrt(0.3), std::sqrt(0.3));
	EXPECT_EQ(ccm::sqrt(0.4), std::sqrt(0.4));
	EXPECT_EQ(ccm::sqrt(0.5), std::sqrt(0.5));
}

TEST(CcmathPowerTests, Sqrt_Double_EdgeCases)
{
	// Test edge cases
	EXPECT_EQ(ccm::sqrt(std::numeric_limits<double>::min()), std::sqrt(std::numeric_limits<double>::min()));
	EXPECT_EQ(ccm::sqrt(std::numeric_limits<double>::max()), std::sqrt(std::numeric_limits<double>::max()));


	//EXPECT_EQ(ccm::sqrt(std::numeric_limits<double>::lowest()), std::sqrt(std::numeric_limits<double>::lowest()));
}

TEST(CcmathPowerTests, Sqrt_LDouble)
{
	// Test values that are 2^x
	EXPECT_EQ(ccm::sqrt(0.0L), std::sqrt(0.0L));
	EXPECT_EQ(ccm::sqrt(1.0L), std::sqrt(1.0L));
	EXPECT_EQ(ccm::sqrt(2.0L), std::sqrt(2.0L));
	EXPECT_EQ(ccm::sqrt(4.0L), std::sqrt(4.0L));
	EXPECT_EQ(ccm::sqrt(8.0L), std::sqrt(8.0L));
	EXPECT_EQ(ccm::sqrt(16.0L), std::sqrt(16.0L));
	EXPECT_EQ(ccm::sqrt(32.0L), std::sqrt(32.0L));
	EXPECT_EQ(ccm::sqrt(64.0L), std::sqrt(64.0L));
	EXPECT_EQ(ccm::sqrt(128.0L), std::sqrt(128.0L));
	EXPECT_EQ(ccm::sqrt(256.0L), std::sqrt(256.0L));

	// Test decimal values
	EXPECT_EQ(ccm::sqrt(0.1L), std::sqrt(0.1L));
	EXPECT_EQ(ccm::sqrt(0.2L), std::sqrt(0.2L));
	EXPECT_EQ(ccm::sqrt(0.3L), std::sqrt(0.3L));
	EXPECT_EQ(ccm::sqrt(0.4L), std::sqrt(0.4L));
	EXPECT_EQ(ccm::sqrt(0.5L), std::sqrt(0.5L));
}

TEST(CcmathPowerTests, Sqrt_LDouble_EdgeCases)
{
	// Test edge cases
	EXPECT_EQ(ccm::sqrt(std::numeric_limits<long double>::min()), std::sqrt(std::numeric_limits<long double>::min()));
	EXPECT_EQ(ccm::sqrt(std::numeric_limits<long double>::max()), std::sqrt(std::numeric_limits<long double>::max()));
}
