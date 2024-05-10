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

TEST(CcmathExponentialTests, Exp2_Double_ValidInput)
{
	EXPECT_EQ(ccm::exp2(1.0), std::exp2(1.0));
	EXPECT_EQ(ccm::exp2(2.0), std::exp2(2.0));
	EXPECT_EQ(ccm::exp2(4.0), std::exp2(4.0));
	EXPECT_EQ(ccm::exp2(8.0), std::exp2(8.0));
	EXPECT_EQ(ccm::exp2(16.0), std::exp2(16.0));
	EXPECT_EQ(ccm::exp2(32.0), std::exp2(32.0));
	EXPECT_EQ(ccm::exp2(64.0), std::exp2(64.0));
	EXPECT_EQ(ccm::exp2(128.0), std::exp2(128.0));
	EXPECT_EQ(ccm::exp2(256.0), std::exp2(256.0));
	EXPECT_EQ(ccm::exp2(512.0), std::exp2(512.0));
	EXPECT_EQ(ccm::exp2(1024.0), std::exp2(1024.0));
	EXPECT_EQ(ccm::exp2(2048.0), std::exp2(2048.0));
	EXPECT_EQ(ccm::exp2(4096.0), std::exp2(4096.0));

	// Test Edge Cases

	EXPECT_EQ(ccm::exp2(0.0), std::exp2(0.0));
	EXPECT_EQ(ccm::exp2(-0.0), std::exp2(-0.0));
	EXPECT_EQ(ccm::exp2(std::numeric_limits<double>::infinity()), std::exp2(std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::exp2(-std::numeric_limits<double>::infinity()), std::exp2(-std::numeric_limits<double>::infinity()));

	bool testCcmExp2ThatNanReturnsNan = std::isnan(ccm::exp2(std::numeric_limits<double>::quiet_NaN()));
	bool testStdExp2ThatNanReturnsNan = std::isnan(std::exp2(std::numeric_limits<double>::quiet_NaN()));
	bool testCcmExp2ThatNanIsPositive = std::signbit(ccm::exp2(std::numeric_limits<double>::quiet_NaN()));
	bool testStdExp2ThatNanIsPositive = std::signbit(std::exp2(std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(testCcmExp2ThatNanReturnsNan, testStdExp2ThatNanReturnsNan);
	EXPECT_EQ(testCcmExp2ThatNanIsPositive, testStdExp2ThatNanIsPositive);
}

TEST(CcmathExponentialTests, Exp2_Double_EdgeCases)
{
	EXPECT_EQ(ccm::exp2(0.0), std::exp2(0.0));
	EXPECT_EQ(ccm::exp2(-0.0), std::exp2(-0.0));
	EXPECT_EQ(ccm::exp2(std::numeric_limits<double>::infinity()), std::exp2(std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::exp2(-std::numeric_limits<double>::infinity()), std::exp2(-std::numeric_limits<double>::infinity()));

	bool testCcmExp2ThatNanReturnsNan = std::isnan(ccm::exp2(std::numeric_limits<double>::quiet_NaN()));
	bool testStdExp2ThatNanReturnsNan = std::isnan(std::exp2(std::numeric_limits<double>::quiet_NaN()));
	bool testCcmExp2ThatNanIsPositive = std::signbit(ccm::exp2(std::numeric_limits<double>::quiet_NaN()));
	bool testStdExp2ThatNanIsPositive = std::signbit(std::exp2(std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(testCcmExp2ThatNanReturnsNan, testStdExp2ThatNanReturnsNan);
	EXPECT_EQ(testCcmExp2ThatNanIsPositive, testStdExp2ThatNanIsPositive);
}



TEST(CcmathExponentialTests, Exp2_Float)
{
	EXPECT_EQ(ccm::exp2(1.0F), std::exp2(1.0F));
	EXPECT_EQ(ccm::exp2(2.0F), std::exp2(2.0F));
	EXPECT_EQ(ccm::exp2(4.0F), std::exp2(4.0F));
	EXPECT_EQ(ccm::exp2(8.0F), std::exp2(8.0F));
	EXPECT_EQ(ccm::exp2(16.0F), std::exp2(16.0F));
	EXPECT_EQ(ccm::exp2(32.0F), std::exp2(32.0F));
	EXPECT_EQ(ccm::exp2(64.0F), std::exp2(64.0F));
	EXPECT_EQ(ccm::exp2(128.0F), std::exp2(128.0F));
	EXPECT_EQ(ccm::exp2(256.0F), std::exp2(256.0F));
	EXPECT_EQ(ccm::exp2(512.0F), std::exp2(512.0F));
	EXPECT_EQ(ccm::exp2(1024.0F), std::exp2(1024.0F));
	EXPECT_EQ(ccm::exp2(2048.0F), std::exp2(2048.0F));
	EXPECT_EQ(ccm::exp2(4096.0F), std::exp2(4096.0F));
}

TEST(CcmathExponentialTests, Exp2_Float_EdgeCases)
{
	EXPECT_EQ(ccm::exp2(0.0F), std::exp2(0.0F));
	EXPECT_EQ(ccm::exp2(-0.0F), std::exp2(-0.0F));
	EXPECT_EQ(ccm::exp2(std::numeric_limits<float>::infinity()), std::exp2(std::numeric_limits<float>::infinity()));
	EXPECT_EQ(ccm::exp2(-std::numeric_limits<float>::infinity()), std::exp2(-std::numeric_limits<float>::infinity()));

	bool testCcmExp2ThatNanReturnsNan = std::isnan(ccm::exp2(std::numeric_limits<float>::quiet_NaN()));
	bool testStdExp2ThatNanReturnsNan = std::isnan(std::exp2(std::numeric_limits<float>::quiet_NaN()));
	bool testCcmExp2ThatNanIsPositive = std::signbit(ccm::exp2(std::numeric_limits<float>::quiet_NaN()));
	bool testStdExp2ThatNanIsPositive = std::signbit(std::exp2(std::numeric_limits<float>::quiet_NaN()));
	EXPECT_EQ(testCcmExp2ThatNanReturnsNan, testStdExp2ThatNanReturnsNan);
	EXPECT_EQ(testCcmExp2ThatNanIsPositive, testStdExp2ThatNanIsPositive);
}

