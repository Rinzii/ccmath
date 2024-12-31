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
#include <cstdint>
#include <limits>
#include "ccmath/ccmath.hpp"

// Function to calculate the ULP difference
int64_t ulp_difference(double a, double b)
{
	// Handle special cases like infinity or NaN
	if (std::isnan(a) || std::isnan(b)) { return std::numeric_limits<int64_t>::max(); }
	if (std::isinf(a) || std::isinf(b)) { return std::numeric_limits<int64_t>::max(); }

	// Interpret the bit patterns of the doubles as int64_t
	int64_t a_bits = *reinterpret_cast<int64_t *>(&a); // NOLINT
	int64_t b_bits = *reinterpret_cast<int64_t *>(&b); // NOLINT

	// Ensure the sign bit is handled correctly for bitwise comparison
	if (a_bits < 0) { a_bits = 0x8000000000000000 - a_bits; }
	if (b_bits < 0) { b_bits = 0x8000000000000000 - b_bits; }

	// Calculate the absolute ULP difference
	return std::abs(a_bits - b_bits);
}

#define EXPECT_ULP_EQ(val1, val2, max_ulp)                                                                                                                     \
	do {                                                                                                                                                       \
		const int64_t ulp_diff = ulp_difference(val1, val2);                                                                                                   \
		ASSERT_LE(ulp_diff, max_ulp) << "Values differ by " << ulp_diff << " ULPs, which exceeds " << (max_ulp) << " ULPs.\n"                                  \
									 << "  Actual: " << (val1) << "\n"                                                                                         \
									 << "  Expected: " << (val2);                                                                                              \
	} while (0)

#define EXPECT_2ULP_EQ(val1, val2) EXPECT_ULP_EQ(val1, val2, 2)

TEST(CcmathExponentialTests, Log2)
{
	static_assert(ccm::log2(1.0) == 0.0, "log2 has failed testing that it is static_assert-able!"); // Check that we can use static_assert with this function

	double const v1 = 2.7725887222397807;
	double const v2 = 2.7725887222397811;
	// EXPECT_DOUBLE_EQ(v1, v2);

	EXPECT_ULP_EQ(v1, v2, 1);

	EXPECT_NEAR(v1, v2, std::numeric_limits<double>::epsilon() + std::numeric_limits<double>::epsilon());

	// Define two values with a known difference of 2 ULP
	double value1 = 2.7725887222397807;
	double value2 = std::nextafter(std::nextafter(value1, std::numeric_limits<double>::max()), std::numeric_limits<double>::max());

	// Verify that the difference is exactly 2 ULP
	// EXPECT_ULP_EQ(value1, value2, 0);

	// Define two values with a known difference greater than 2 ULP
	double value3 = std::nextafter(std::nextafter(std::nextafter(value1, std::numeric_limits<double>::max()), std::numeric_limits<double>::max()),
								   std::numeric_limits<double>::max());

	// Verify that the difference exceeds 2 ULP and the test fails
	// EXPECT_ULP_EQ(value1, value3, 2);

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

	bool testCcmLog2ThatQuietNanReturnsNan	   = std::isnan(ccm::log2(std::numeric_limits<double>::quiet_NaN()));
	bool testStdLog2ThatQuietNanReturnsNan	   = std::isnan(std::log2(std::numeric_limits<double>::quiet_NaN()));
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
