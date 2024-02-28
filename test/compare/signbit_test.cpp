/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present ccmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
*/

#include <gtest/gtest.h>

#include <ccmath/detail/compare/signbit.hpp>
#include <cmath>
#include <limits>


TEST(CcmathCompareTests, Signbit)
{
	EXPECT_EQ(ccm::signbit(1.0), std::signbit(1.0));
    EXPECT_EQ(ccm::signbit(-1.0), std::signbit(-1.0));
    EXPECT_EQ(ccm::signbit(0.0), std::signbit(0.0));
    EXPECT_EQ(ccm::signbit(-0.0), std::signbit(-0.0));
    EXPECT_EQ(ccm::signbit(std::numeric_limits<double>::infinity()), std::signbit(std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::signbit(-std::numeric_limits<double>::infinity()), std::signbit(-std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::signbit(std::numeric_limits<double>::quiet_NaN()), std::signbit(std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(ccm::signbit(-std::numeric_limits<double>::quiet_NaN()), std::signbit(-std::numeric_limits<double>::quiet_NaN()));

	EXPECT_EQ(ccm::signbit(1.0f), std::signbit(1.0f));
    EXPECT_EQ(ccm::signbit(-1.0f), std::signbit(-1.0f));
    EXPECT_EQ(ccm::signbit(0.0f), std::signbit(0.0f));
    EXPECT_EQ(ccm::signbit(-0.0f), std::signbit(-0.0f));
    EXPECT_EQ(ccm::signbit(std::numeric_limits<float>::infinity()), std::signbit(std::numeric_limits<float>::infinity()));
	EXPECT_EQ(ccm::signbit(-std::numeric_limits<float>::infinity()), std::signbit(-std::numeric_limits<float>::infinity()));
	EXPECT_EQ(ccm::signbit(std::numeric_limits<float>::quiet_NaN()), std::signbit(std::numeric_limits<float>::quiet_NaN()));
	EXPECT_EQ(ccm::signbit(-std::numeric_limits<float>::quiet_NaN()), std::signbit(-std::numeric_limits<float>::quiet_NaN()));

	// Test integers
	EXPECT_EQ(ccm::signbit(1), std::signbit(1));
    EXPECT_EQ(ccm::signbit(-1), std::signbit(-1));
    EXPECT_EQ(ccm::signbit(0), std::signbit(0));
    EXPECT_EQ(ccm::signbit(-0), std::signbit(-0));
    EXPECT_EQ(ccm::signbit(std::numeric_limits<int>::max()), std::signbit(std::numeric_limits<int>::max()));
	EXPECT_EQ(ccm::signbit(std::numeric_limits<int>::min()), std::signbit(std::numeric_limits<int>::min()));
	EXPECT_EQ(ccm::signbit(std::numeric_limits<int>::lowest()), std::signbit(std::numeric_limits<int>::lowest()));
	EXPECT_EQ(ccm::signbit(std::numeric_limits<int>::infinity()), std::signbit(std::numeric_limits<int>::infinity()));
	EXPECT_EQ(ccm::signbit(-std::numeric_limits<int>::infinity()), std::signbit(-std::numeric_limits<int>::infinity()));
	EXPECT_EQ(ccm::signbit(std::numeric_limits<int>::quiet_NaN()), std::signbit(std::numeric_limits<int>::quiet_NaN()));
	EXPECT_EQ(ccm::signbit(-std::numeric_limits<int>::quiet_NaN()), std::signbit(-std::numeric_limits<int>::quiet_NaN()));




}
