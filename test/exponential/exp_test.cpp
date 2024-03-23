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

TEST(CcmathExponentialTests, Exp)
{
	EXPECT_EQ(ccm::exp(1.0), std::exp(1.0));
	EXPECT_EQ(ccm::exp(2.0), std::exp(2.0));
	EXPECT_EQ(ccm::exp(4.0), std::exp(4.0));
	EXPECT_EQ(ccm::exp(8.0), std::exp(8.0));
	EXPECT_EQ(ccm::exp(16.0), std::exp(16.0));
	EXPECT_EQ(ccm::exp(32.0), std::exp(32.0));
	EXPECT_EQ(ccm::exp(64.0), std::exp(64.0));
	EXPECT_EQ(ccm::exp(128.0), std::exp(128.0));
	EXPECT_EQ(ccm::exp(256.0), std::exp(256.0));
	EXPECT_EQ(ccm::exp(512.0), std::exp(512.0));
	EXPECT_EQ(ccm::exp(1024.0), std::exp(1024.0));
	EXPECT_EQ(ccm::exp(2048.0), std::exp(2048.0));
	EXPECT_EQ(ccm::exp(4096.0), std::exp(4096.0));

	// Test Edge Cases

	EXPECT_EQ(ccm::exp(0.0), std::exp(0.0));
	EXPECT_EQ(ccm::exp(-0.0), std::exp(-0.0));
	EXPECT_EQ(ccm::exp(std::numeric_limits<double>::infinity()), std::exp(std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::exp(-std::numeric_limits<double>::infinity()), std::exp(-std::numeric_limits<double>::infinity()));

	bool testCcmExpThatNanReturnsNan = std::isnan(ccm::exp(std::numeric_limits<double>::quiet_NaN()));
	bool testStdExpThatNanReturnsNan = std::isnan(std::exp(std::numeric_limits<double>::quiet_NaN()));
	bool testCcmExpThatNanIsPositive = std::signbit(ccm::exp(std::numeric_limits<double>::quiet_NaN()));
	bool testStdExpThatNanIsPositive = std::signbit(std::exp(std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(testCcmExpThatNanReturnsNan, testStdExpThatNanReturnsNan);
	EXPECT_EQ(testCcmExpThatNanIsPositive, testStdExpThatNanIsPositive);

}
