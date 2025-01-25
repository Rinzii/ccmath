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

#include <cmath>
#include <limits>
#include "ccmath/ccmath.hpp"

TEST(CcmathExponentialTests, Log2)
{
	static_assert(ccm::log2(1.0) == 0.0, "log2 has failed testing that it is static_assert-able!"); // Check that we can use static_assert with this function

	EXPECT_EQ(ccm::log2(1.0), std::log2(1.0));
	EXPECT_EQ(ccm::log2(2.0), std::log2(2.0));
	EXPECT_EQ(ccm::log2(4.0), std::log2(4.0));
	EXPECT_EQ(ccm::log2(8.0), std::log2(8.0));
	EXPECT_EQ(ccm::log2(16.0), std::log2(16.0));
	EXPECT_EQ(ccm::log2(32.0), std::log2(32.0));
	EXPECT_EQ(ccm::log2(64.0), std::log2(64.0));
	EXPECT_EQ(ccm::log2(128.0), std::log2(128.0));
	EXPECT_EQ(ccm::log2(256.0), std::log2(256.0));
	EXPECT_EQ(ccm::log2(512.0), std::log2(512.0));
	EXPECT_EQ(ccm::log2(1024.0), std::log2(1024.0));
	EXPECT_EQ(ccm::log2(2048.0), std::log2(2048.0));
	EXPECT_EQ(ccm::log2(4096.0), std::log2(4096.0));

	// Test floating point precision
	EXPECT_EQ(ccm::log2(1.0F), std::log2(1.0F));
	EXPECT_EQ(ccm::log2(2.0F), std::log2(2.0F));
	EXPECT_EQ(ccm::log2(4.0F), std::log2(4.0F));
	EXPECT_EQ(ccm::log2(8.0F), std::log2(8.0F));
	EXPECT_EQ(ccm::log2(16.0F), std::log2(16.0F));
	EXPECT_EQ(ccm::log2(32.0F), std::log2(32.0F));
	EXPECT_EQ(ccm::log2(64.0F), std::log2(64.0F));
	EXPECT_EQ(ccm::log2(128.0F), std::log2(128.0F));
	EXPECT_EQ(ccm::log2(256.0F), std::log2(256.0F));
	EXPECT_EQ(ccm::log2(512.0F), std::log2(512.0F));
	EXPECT_EQ(ccm::log2(1024.0F), std::log2(1024.0F));
	EXPECT_EQ(ccm::log2(2048.0F), std::log2(2048.0F));
	EXPECT_EQ(ccm::log2(4096.0F), std::log2(4096.0F));

	// Test edge cases
	EXPECT_EQ(ccm::log2(0.0), std::log2(0.0));
	EXPECT_EQ(ccm::log2(-0.0), std::log2(-0.0));
	EXPECT_EQ(ccm::log2(std::numeric_limits<double>::infinity()), std::log2(std::numeric_limits<double>::infinity()));

	bool testCcmLog2ThatNegInfReturnsNan = std::isnan(ccm::log2(-std::numeric_limits<double>::infinity()));
	bool testStdLog2ThatNegInfReturnsNan = std::isnan(std::log2(-std::numeric_limits<double>::infinity()));
	EXPECT_EQ(testCcmLog2ThatNegInfReturnsNan, testStdLog2ThatNegInfReturnsNan);

	bool testCcmLog2ThatQuietNanReturnsNan = std::isnan(ccm::log2(std::numeric_limits<double>::quiet_NaN()));
	bool testStdLog2ThatQuietNanReturnsNan = std::isnan(std::log2(std::numeric_limits<double>::quiet_NaN()));
	bool testCcmLog2ThatPosQuietNanHasSameSign = std::signbit(ccm::log2(std::numeric_limits<double>::quiet_NaN()));
	bool testStdLog2ThatPosQuietNanHasSameSign = std::signbit(std::log2(std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(testCcmLog2ThatQuietNanReturnsNan, testStdLog2ThatQuietNanReturnsNan);
	EXPECT_EQ(testCcmLog2ThatPosQuietNanHasSameSign, testStdLog2ThatPosQuietNanHasSameSign);

	bool testCcmLog2ThatNegQuietNanReturnsNan = std::isnan(ccm::log2(-std::numeric_limits<double>::quiet_NaN()));
	bool testStdLog2ThatNegQuietNanReturnsNan = std::isnan(std::log2(-std::numeric_limits<double>::quiet_NaN()));
	bool testCcmLog2ThatNegQuietNanIsNegative = std::signbit(ccm::log2(-std::numeric_limits<double>::quiet_NaN()));
	bool testStdLog2ThatNegQuietNanIsNegative = std::signbit(std::log2(-std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(testCcmLog2ThatNegQuietNanReturnsNan, testStdLog2ThatNegQuietNanReturnsNan);
	EXPECT_EQ(testCcmLog2ThatNegQuietNanIsNegative, testStdLog2ThatNegQuietNanIsNegative);
}
