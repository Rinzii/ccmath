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

TEST(CcmathBasicTests, AbsStaticAssert)
{
    // Verify that ccm::abs works with static_assert
    static_assert(ccm::abs(1) == 1, "abs has failed testing that it is static_assert-able!");
}

TEST(CcmathBasicTests, AbsZeroSign)
{
	EXPECT_EQ(ccm::abs(-0.0), std::abs(-0.0));
	EXPECT_EQ(ccm::abs(0), std::abs(0));
}

TEST(CcmathBasicTests, AbsDouble)
{
	EXPECT_EQ(ccm::abs(1.0), std::abs(1.0));
	EXPECT_EQ(ccm::abs(-1.0), std::abs(-1.0));
	EXPECT_EQ(ccm::abs(2.0), std::abs(2.0));
	EXPECT_EQ(ccm::abs(-2.0), std::abs(-2.0));
	EXPECT_EQ(ccm::abs(4.0), std::abs(4.0));
	EXPECT_EQ(ccm::abs(-4.0), std::abs(-4.0));
	EXPECT_EQ(ccm::abs(8.0), std::abs(8.0));
	EXPECT_EQ(ccm::abs(-8.0), std::abs(-8.0));
	EXPECT_EQ(ccm::abs(16.0), std::abs(16.0));
	EXPECT_EQ(ccm::abs(-16.0), std::abs(-16.0));
	EXPECT_EQ(ccm::abs(32.0), std::abs(32.0));
	EXPECT_EQ(ccm::abs(-32.0), std::abs(-32.0));
}

TEST(CcmathBasicTests, AbsFloat)
{
    EXPECT_EQ(ccm::abs(1.0F), std::abs(1.0F));
    EXPECT_EQ(ccm::abs(-1.0F), std::abs(-1.0F));
    EXPECT_EQ(ccm::abs(2.0F), std::abs(2.0F));
    EXPECT_EQ(ccm::abs(-2.0F), std::abs(-2.0F));
    EXPECT_EQ(ccm::abs(4.0F), std::abs(4.0F));
    EXPECT_EQ(ccm::abs(-4.0F), std::abs(-4.0F));
    EXPECT_EQ(ccm::abs(8.0F), std::abs(8.0F));
    EXPECT_EQ(ccm::abs(-8.0F), std::abs(-8.0F));
    EXPECT_EQ(ccm::abs(16.0F), std::abs(16.0F));
    EXPECT_EQ(ccm::abs(-16.0F), std::abs(-16.0F));
    EXPECT_EQ(ccm::abs(32.0F), std::abs(32.0F));
    EXPECT_EQ(ccm::abs(-32.0F), std::abs(-32.0F));
}

TEST(CcmathBasicTests, AbsLongDouble)
{
	EXPECT_EQ(ccm::abs(1.0L), std::abs(1.0L));
    EXPECT_EQ(ccm::abs(-1.0L), std::abs(-1.0L));
    EXPECT_EQ(ccm::abs(2.0L), std::abs(2.0L));
    EXPECT_EQ(ccm::abs(-2.0L), std::abs(-2.0L));
    EXPECT_EQ(ccm::abs(4.0L), std::abs(4.0L));
    EXPECT_EQ(ccm::abs(-4.0L), std::abs(-4.0L));
    EXPECT_EQ(ccm::abs(8.0L), std::abs(8.0L));
    EXPECT_EQ(ccm::abs(-8.0L), std::abs(-8.0L));
    EXPECT_EQ(ccm::abs(16.0L), std::abs(16.0L));
    EXPECT_EQ(ccm::abs(-16.0L), std::abs(-16.0L));
    EXPECT_EQ(ccm::abs(32.0L), std::abs(32.0L));
    EXPECT_EQ(ccm::abs(-32.0L), std::abs(-32.0L));
}

TEST(CcmathBasicTests, AbsInt)
{
    EXPECT_EQ(ccm::abs(1), std::abs(1));
    EXPECT_EQ(ccm::abs(-1), std::abs(-1));
    EXPECT_EQ(ccm::abs(2), std::abs(2));
    EXPECT_EQ(ccm::abs(-2), std::abs(-2));
    EXPECT_EQ(ccm::abs(4), std::abs(4));
    EXPECT_EQ(ccm::abs(-4), std::abs(-4));
    EXPECT_EQ(ccm::abs(8), std::abs(8));
    EXPECT_EQ(ccm::abs(-8), std::abs(-8));
    EXPECT_EQ(ccm::abs(16), std::abs(16));
    EXPECT_EQ(ccm::abs(-16), std::abs(-16));
    EXPECT_EQ(ccm::abs(32), std::abs(32));
    EXPECT_EQ(ccm::abs(-32), std::abs(-32));
}

TEST(CcmathBasicTests, AbsEdgeCases)
{
	// TODO: Implement more edge cases for ABS.
	EXPECT_EQ(ccm::abs(std::numeric_limits<int>::min()), std::abs(std::numeric_limits<int>::min()));
    EXPECT_EQ(ccm::abs(std::numeric_limits<int>::max()), std::abs(std::numeric_limits<int>::max()));
    EXPECT_EQ(ccm::abs(std::numeric_limits<float>::min()), std::abs(std::numeric_limits<float>::min()));
    EXPECT_EQ(ccm::abs(std::numeric_limits<float>::max()), std::abs(std::numeric_limits<float>::max()));
    EXPECT_EQ(ccm::abs(std::numeric_limits<double>::min()), std::abs(std::numeric_limits<double>::min()));
    EXPECT_EQ(ccm::abs(std::numeric_limits<double>::max()), std::abs(std::numeric_limits<double>::max()));
    EXPECT_EQ(ccm::abs(std::numeric_limits<long double>::min()), std::abs(std::numeric_limits<long double>::min()));
    EXPECT_EQ(ccm::abs(std::numeric_limits<long double>::max()), std::abs(std::numeric_limits<long double>::max()));
}

