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

#include "ccmath/ccmath.hpp"

#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

// ULP Difference Function
template <typename T>
int64_t ulp_difference(T a, T b)
{
	static_assert(std::is_floating_point_v<T>, "T must be a floating-point type.");

	// Handle special cases like NaN or Infinity
	if (std::isnan(a) && std::isnan(b))
	{
		return 0; // Both are NaN, considered equal
	}
	if (std::isnan(a) || std::isnan(b))
	{
		return std::numeric_limits<int64_t>::max(); // One is NaN, the other is not
	}
	if (std::isinf(a) && std::isinf(b))
	{
		if (std::signbit(a) == std::signbit(b))
		{
			return 0; // Both infinities with the same sign are considered equal
		}
		return std::numeric_limits<int64_t>::max(); // Opposite sign infinities
	}
	if (std::isinf(a) || std::isinf(b))
	{
		return std::numeric_limits<int64_t>::max(); // One is infinity, the other is not
	}

	using UIntType				 = std::conditional_t<std::is_same_v<T, float>, uint32_t, uint64_t>;
	constexpr UIntType sign_mask = std::is_same_v<T, float> ? 0x80000000U : 0x8000000000000000ULL;

	// Use __builtin_bit_cast for bit-level reinterpretation
	auto a_bits = __builtin_bit_cast(UIntType, a);
	auto b_bits = __builtin_bit_cast(UIntType, b);

	// Handle the sign bit by flipping it to treat all values as positive
	if (static_cast<std::make_signed_t<UIntType>>(a_bits) < 0) { a_bits = sign_mask - a_bits; }
	if (static_cast<std::make_signed_t<UIntType>>(b_bits) < 0) { b_bits = sign_mask - b_bits; }

	// Return the absolute difference using unsigned arithmetic
	return (a_bits > b_bits) ? a_bits - b_bits : b_bits - a_bits;
}

// ULP Comparison Macro for Google Test
#define EXPECT_ULP_EQ(V1, V2, MAX_ULP)                                                                                                                         \
	do {                                                                                                                                                       \
		const int64_t ulp_diff = ulp_difference(V1, V2);                                                                                                       \
		if (ulp_diff > (MAX_ULP))                                                                                                                              \
		{                                                                                                                                                      \
			ADD_FAILURE() << "ULP Difference Exceeded:\n"                                                                                                      \
						  << "  Actual: " << (V1) << "\n"                                                                                                      \
						  << "  Expected: " << (V2) << "\n"                                                                                                    \
						  << "  ULP Difference: " << ulp_diff << "\n"                                                                                          \
						  << "  Maximum Allowed ULPs: " << (MAX_ULP);                                                                                          \
		}                                                                                                                                                      \
		else { SUCCEED(); }                                                                                                                                    \
	} while (0)

#define ASSERT_ULP_EQ(val1, val2, max_ulp)                                                                                                                     \
	do {                                                                                                                                                       \
		const int64_t ulp_diff = ulp_difference(val1, val2);                                                                                                   \
		ASSERT_LE(ulp_diff, max_ulp) << "ULP Difference Exceeded:\n"                                                                                           \
									 << "  Actual: " << (val1) << "\n"                                                                                         \
									 << "  Expected: " << (val2) << "\n"                                                                                       \
									 << "  ULP Difference: " << ulp_diff << "\n"                                                                               \
									 << "  Maximum Allowed ULPs: " << (max_ulp);                                                                               \
	} while (0)

#define EXPECT_ONE_ULP_EQ(V1, V2) EXPECT_ULP_EQ(V1, V2, 1)
#define ASSERT_ONE_ULP_EQ(V1, V2) ASSERT_ULP_EQ(V1, V2, 1)

TEST(ULPTest, HandlesNegativeNumbers)
{
	double value1 = -1.0;
	double value2 = -1.0;
	EXPECT_EQ(ulp_difference(value1, value2), 0.5);
}

TEST(CcmathExponentialTests, Log2)
{
	static_assert(ccm::log2(1.0) == 0.0, "log2 has failed testing that it is static_assert-able!"); // Check that we can use static_assert with this function

	double const v1 = 2.7725887222397807;
	double const v2 = 2.7725887222397811;
	// EXPECT_DOUBLE_EQ(v1, v2);

	EXPECT_ULP_EQ(v1, v2, 0);

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
