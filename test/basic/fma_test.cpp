/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <ccmath/ccmath.hpp>
#include <gtest/gtest.h>
#include <cmath>
#include <limits>

TEST(CcmathBasicTests, Fma)
{
	// Test that fma works with static_assert
	static_assert(ccm::fma(1, 2, 3) == 5, "fma has failed testing that it is static_assert-able!");

	EXPECT_EQ(ccm::fma(1.0, 2.0, 3.0), std::fma(1.0, 2.0, 3.0));
	EXPECT_EQ(ccm::fma(1.0F, 2.0F, 3.0F), std::fma(1.0F, 2.0F, 3.0F));
	EXPECT_EQ(ccm::fma(1.0L, 2.0L, 3.0L), std::fma(1.0L, 2.0L, 3.0L));

	EXPECT_DOUBLE_EQ(std::fma(2.0, 3.0, 4.0), 10.0);  // 2.0 * 3.0 + 4.0 = 10.0
	EXPECT_DOUBLE_EQ(std::fma(-2.5, 4.0, 1.5), -8.5); // -2.5 * 4.0 + 1.5 = -8.5
	EXPECT_DOUBLE_EQ(std::fma(0.0, 5.0, 6.0), 6.0);	  // 0.0 * 5.0 + 6.0 = 6.0

	// Test edge cases
	EXPECT_EQ(ccm::fma(0.0, 0.0, 0.0), std::fma(0.0, 0.0, 0.0));
	EXPECT_EQ(ccm::fma(-0.0, -0.0, -0.0), std::fma(-0.0, -0.0, -0.0));

	/* TODO: Add these test back in once the implementation is complete

	// If x is zero and y is infinity, or if y is zero and x is infinity and Z is not NaN, then the result is NaN.
	bool testCcmFmaZeroTimesInfinityIsNan		= std::isnan(ccm::fma(0.0, std::numeric_limits<double>::infinity(), 0.0));
	bool testStdFmaZeroTimesInfinityIsNan		= std::isnan(std::fma(0.0, std::numeric_limits<double>::infinity(), 0.0));
	bool testCcmFmaZeroTimesInfinityHasSameSign = std::signbit(ccm::fma(0.0, std::numeric_limits<double>::infinity(), 0.0));
	bool testStdFmaZeroTimesInfinityHasSameSign = std::signbit(std::fma(0.0, std::numeric_limits<double>::infinity(), 0.0));
	EXPECT_EQ(testCcmFmaZeroTimesInfinityIsNan, testStdFmaZeroTimesInfinityIsNan);
	EXPECT_EQ(testCcmFmaZeroTimesInfinityHasSameSign, testStdFmaZeroTimesInfinityHasSameSign);

	bool testCcmFmaZeroTimesNegativeInfinityIsNan		= std::isnan(ccm::fma(0.0, -std::numeric_limits<double>::infinity(), 0.0));
	bool testStdFmaZeroTimesNegativeInfinityIsNan		= std::isnan(std::fma(0.0, -std::numeric_limits<double>::infinity(), 0.0));
	bool testCcmFmaZeroTimesNegativeInfinityHasSameSign = std::signbit(ccm::fma(0.0, -std::numeric_limits<double>::infinity(), 0.0));
	bool testStdFmaZeroTimesNegativeInfinityHasSameSign = std::signbit(std::fma(0.0, -std::numeric_limits<double>::infinity(), 0.0));
	EXPECT_EQ(testCcmFmaZeroTimesNegativeInfinityIsNan, testStdFmaZeroTimesNegativeInfinityIsNan);
	EXPECT_EQ(testCcmFmaZeroTimesNegativeInfinityHasSameSign, testStdFmaZeroTimesNegativeInfinityHasSameSign);

	bool testCcmFmaInfinityTimesZeroIsNan		= std::isnan(ccm::fma(std::numeric_limits<double>::infinity(), 0.0, 0.0));
	bool testStdFmaInfinityTimesZeroIsNan		= std::isnan(std::fma(std::numeric_limits<double>::infinity(), 0.0, 0.0));
	bool testCcmFmaInfinityTimesZeroHasSameSign = std::signbit(ccm::fma(std::numeric_limits<double>::infinity(), 0.0, 0.0));
	bool testStdFmaInfinityTimesZeroHasSameSign = std::signbit(std::fma(std::numeric_limits<double>::infinity(), 0.0, 0.0));
	EXPECT_EQ(testCcmFmaInfinityTimesZeroIsNan, testStdFmaInfinityTimesZeroIsNan);
	EXPECT_EQ(testCcmFmaInfinityTimesZeroHasSameSign, testStdFmaInfinityTimesZeroHasSameSign);

	bool testCcmFmaNegativeInfinityTimesZeroIsNan		= std::isnan(ccm::fma(-std::numeric_limits<double>::infinity(), 0.0, 0.0));
	bool testStdFmaNegativeInfinityTimesZeroIsNan		= std::isnan(std::fma(-std::numeric_limits<double>::infinity(), 0.0, 0.0));
	bool testCcmFmaNegativeInfinityTimesZeroHasSameSign = std::signbit(ccm::fma(-std::numeric_limits<double>::infinity(), 0.0, 0.0));
	bool testStdFmaNegativeInfinityTimesZeroHasSameSign = std::signbit(std::fma(-std::numeric_limits<double>::infinity(), 0.0, 0.0));
	EXPECT_EQ(testCcmFmaNegativeInfinityTimesZeroIsNan, testStdFmaNegativeInfinityTimesZeroIsNan);
	EXPECT_EQ(testCcmFmaNegativeInfinityTimesZeroHasSameSign, testStdFmaNegativeInfinityTimesZeroHasSameSign);

	// If x is zero and y is infinity, or if y is zero and x is infinity and Z is NaN, then the result is NaN.
	bool testCcmFmaZeroTimesInfinityWithNanOnZIsNan =
		std::isnan(ccm::fma(0.0, std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()));
	bool testStdFmaZeroTimesInfinityWithNanOnZIsNan =
		std::isnan(std::fma(0.0, std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()));
	bool testCcmFmaZeroTimesInfinityWithNanOnZHasSameSign =
		std::signbit(ccm::fma(0.0, std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()));
	bool testStdFmaZeroTimesInfinityWithNanOnZHasSameSign =
		std::signbit(std::fma(0.0, std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(testCcmFmaZeroTimesInfinityWithNanOnZIsNan, testStdFmaZeroTimesInfinityWithNanOnZIsNan);
	EXPECT_EQ(testCcmFmaZeroTimesInfinityWithNanOnZHasSameSign, testStdFmaZeroTimesInfinityWithNanOnZHasSameSign);

	bool testCcmFmaZeroTimesNegativeInfinityWithNanOnZIsNan =
		std::isnan(ccm::fma(0.0, -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()));
	bool testStdFmaZeroTimesNegativeInfinityWithNanOnZIsNan =
		std::isnan(std::fma(0.0, -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()));
	bool testCcmFmaZeroTimesNegativeInfinityWithNanOnZHasSameSign =
		std::signbit(ccm::fma(0.0, -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()));
	bool testStdFmaZeroTimesNegativeInfinityWithNanOnZHasSameSign =
		std::signbit(std::fma(0.0, -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(testCcmFmaZeroTimesNegativeInfinityWithNanOnZIsNan, testStdFmaZeroTimesNegativeInfinityWithNanOnZIsNan);
	EXPECT_EQ(testCcmFmaZeroTimesNegativeInfinityWithNanOnZHasSameSign, testStdFmaZeroTimesNegativeInfinityWithNanOnZHasSameSign);

	bool testCcmFmaInfinityTimesZeroWithNanOnZIsNan =
		std::isnan(ccm::fma(std::numeric_limits<double>::infinity(), 0.0, std::numeric_limits<double>::quiet_NaN()));
	bool testStdFmaInfinityTimesZeroWithNanOnZIsNan =
		std::isnan(std::fma(std::numeric_limits<double>::infinity(), 0.0, std::numeric_limits<double>::quiet_NaN()));
	bool testCcmFmaInfinityTimesZeroWithNanOnZHasSameSign =
		std::signbit(ccm::fma(std::numeric_limits<double>::infinity(), 0.0, std::numeric_limits<double>::quiet_NaN()));
	bool testStdFmaInfinityTimesZeroWithNanOnZHasSameSign =
		std::signbit(std::fma(std::numeric_limits<double>::infinity(), 0.0, std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(testCcmFmaInfinityTimesZeroWithNanOnZIsNan, testStdFmaInfinityTimesZeroWithNanOnZIsNan);
	EXPECT_EQ(testCcmFmaInfinityTimesZeroWithNanOnZHasSameSign, testStdFmaInfinityTimesZeroWithNanOnZHasSameSign);

	bool testCcmFmaNegativeInfinityTimesZeroWithNanOnZIsNan =
		std::isnan(ccm::fma(-std::numeric_limits<double>::infinity(), 0.0, std::numeric_limits<double>::quiet_NaN()));
	bool testStdFmaNegativeInfinityTimesZeroWithNanOnZIsNan =
		std::isnan(std::fma(-std::numeric_limits<double>::infinity(), 0.0, std::numeric_limits<double>::quiet_NaN()));
	bool testCcmFmaNegativeInfinityTimesZeroWithNanOnZHasSameSign =
		std::signbit(ccm::fma(-std::numeric_limits<double>::infinity(), 0.0, std::numeric_limits<double>::quiet_NaN()));
	bool testStdFmaNegativeInfinityTimesZeroWithNanOnZHasSameSign =
		std::signbit(std::fma(-std::numeric_limits<double>::infinity(), 0.0, std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(testCcmFmaNegativeInfinityTimesZeroWithNanOnZIsNan, testStdFmaNegativeInfinityTimesZeroWithNanOnZIsNan);
	EXPECT_EQ(testCcmFmaNegativeInfinityTimesZeroWithNanOnZHasSameSign, testStdFmaNegativeInfinityTimesZeroWithNanOnZHasSameSign);

	// If x * y is an exact infinity and z is an infinity with the opposite sign, NaN is returned.
	bool testCcmFmaInfinityTimesInfinityWithOppositeSignIsNan =
		std::isnan(ccm::fma(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()));
	bool testStdFmaInfinityTimesInfinityWithOppositeSignIsNan =
		std::isnan(std::fma(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()));
	bool testCcmFmaInfinityTimesInfinityWithOppositeSignHasSameSign =
		std::signbit(ccm::fma(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()));
	bool testStdFmaInfinityTimesInfinityWithOppositeSignHasSameSign =
		std::signbit(std::fma(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()));
	EXPECT_EQ(testCcmFmaInfinityTimesInfinityWithOppositeSignIsNan, testStdFmaInfinityTimesInfinityWithOppositeSignIsNan);
	EXPECT_EQ(testCcmFmaInfinityTimesInfinityWithOppositeSignHasSameSign, testStdFmaInfinityTimesInfinityWithOppositeSignHasSameSign);

	bool testCcmFmaInfinityTimesInfinityWithOppositeSignIsNan2 =
		std::isnan(ccm::fma(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()));
	bool testStdFmaInfinityTimesInfinityWithOppositeSignIsNan2 =
		std::isnan(std::fma(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()));
	bool testCcmFmaInfinityTimesInfinityWithOppositeSignHasSameSign2 =
		std::signbit(ccm::fma(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()));
	bool testStdFmaInfinityTimesInfinityWithOppositeSignHasSameSign2 =
		std::signbit(std::fma(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()));
	EXPECT_EQ(testCcmFmaInfinityTimesInfinityWithOppositeSignIsNan2, testStdFmaInfinityTimesInfinityWithOppositeSignIsNan2);
	EXPECT_EQ(testCcmFmaInfinityTimesInfinityWithOppositeSignHasSameSign2, testStdFmaInfinityTimesInfinityWithOppositeSignHasSameSign2);

	// If x or y are NaN, NaN is returned.
	bool testCcmFmaIfXIsPosNanReturnsNan = std::isnan(ccm::fma(std::numeric_limits<double>::quiet_NaN(), 1.0, 1.0));
	bool testStdFmaIfXIsPosNanReturnsNan = std::isnan(std::fma(std::numeric_limits<double>::quiet_NaN(), 1.0, 1.0));
	bool testCcmFmaIfXIsPosNanReturnsSameSign = std::signbit(ccm::fma(std::numeric_limits<double>::quiet_NaN(), 1.0, 1.0));
	bool testStdFmaIfXIsPosNanReturnsSameSign = std::signbit(std::fma(std::numeric_limits<double>::quiet_NaN(), 1.0, 1.0));
	EXPECT_EQ(testCcmFmaIfXIsPosNanReturnsNan, testStdFmaIfXIsPosNanReturnsNan);
	EXPECT_EQ(testCcmFmaIfXIsPosNanReturnsSameSign, testStdFmaIfXIsPosNanReturnsSameSign);

	bool testCcmFmaIfYIsPosNanReturnsNan = std::isnan(ccm::fma(1.0, std::numeric_limits<double>::quiet_NaN(), 1.0));
	bool testStdFmaIfYIsPosNanReturnsNan = std::isnan(std::fma(1.0, std::numeric_limits<double>::quiet_NaN(), 1.0));
	bool testCcmFmaIfYIsPosNanReturnsSameSign = std::signbit(ccm::fma(1.0, std::numeric_limits<double>::quiet_NaN(), 1.0));
	bool testStdFmaIfYIsPosNanReturnsSameSign = std::signbit(std::fma(1.0, std::numeric_limits<double>::quiet_NaN(), 1.0));
	EXPECT_EQ(testCcmFmaIfYIsPosNanReturnsNan, testStdFmaIfYIsPosNanReturnsNan);
	EXPECT_EQ(testCcmFmaIfYIsPosNanReturnsSameSign, testStdFmaIfYIsPosNanReturnsSameSign);

	bool testCcmFmaIfXIsNegNanReturnsNan = std::isnan(ccm::fma(-std::numeric_limits<double>::quiet_NaN(), 1.0, 1.0));
	bool testStdFmaIfXIsNegNanReturnsNan = std::isnan(std::fma(-std::numeric_limits<double>::quiet_NaN(), 1.0, 1.0));
	bool testCcmFmaIfXIsNegNanReturnsSameSign = std::signbit(ccm::fma(-std::numeric_limits<double>::quiet_NaN(), 1.0, 1.0));
	bool testStdFmaIfXIsNegNanReturnsSameSign = std::signbit(std::fma(-std::numeric_limits<double>::quiet_NaN(), 1.0, 1.0));
	EXPECT_EQ(testCcmFmaIfXIsNegNanReturnsNan, testStdFmaIfXIsNegNanReturnsNan);
	EXPECT_EQ(testCcmFmaIfXIsNegNanReturnsSameSign, testStdFmaIfXIsNegNanReturnsSameSign);

	bool testCcmFmaIfYIsNegNanReturnsNan = std::isnan(ccm::fma(1.0, -std::numeric_limits<double>::quiet_NaN(), 1.0));
	bool testStdFmaIfYIsNegNanReturnsNan = std::isnan(std::fma(1.0, -std::numeric_limits<double>::quiet_NaN(), 1.0));
	bool testCcmFmaIfYIsNegNanReturnsSameSign = std::signbit(ccm::fma(1.0, -std::numeric_limits<double>::quiet_NaN(), 1.0));
	bool testStdFmaIfYIsNegNanReturnsSameSign = std::signbit(std::fma(1.0, -std::numeric_limits<double>::quiet_NaN(), 1.0));
	EXPECT_EQ(testCcmFmaIfYIsNegNanReturnsNan, testStdFmaIfYIsNegNanReturnsNan);
	EXPECT_EQ(testCcmFmaIfYIsNegNanReturnsSameSign, testStdFmaIfYIsNegNanReturnsSameSign);


	// If z is NaN, and x * y is not 0 * Inf or Inf * 0, then NaN is returned
	bool testCcmFmaXTimesYNotZeroTimesInfAndPosNanOnZIsNan		= std::isnan(ccm::fma(1.0, 1.0, std::numeric_limits<double>::quiet_NaN()));
	bool testStdFmaXTimesYNotZeroTimesInfAndPosNanOnZIsNan		= std::isnan(std::fma(1.0, 1.0, std::numeric_limits<double>::quiet_NaN()));
	bool testCcmFmaXTimesYNotZeroTimesInfAndPosNanOnZIsSameSign = std::signbit(ccm::fma(1.0, 1.0, std::numeric_limits<double>::quiet_NaN()));
	bool testStdFmaXTimesYNotZeroTimesInfAndPosNanOnZIsSameSign = std::signbit(std::fma(1.0, 1.0, std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(testCcmFmaXTimesYNotZeroTimesInfAndPosNanOnZIsNan, testStdFmaXTimesYNotZeroTimesInfAndPosNanOnZIsNan);
	EXPECT_EQ(testCcmFmaXTimesYNotZeroTimesInfAndPosNanOnZIsSameSign, testStdFmaXTimesYNotZeroTimesInfAndPosNanOnZIsSameSign);

	bool testCcmFmaXTimesYNotZeroTimesInfAndNegNanOnZIsNan		= std::isnan(ccm::fma(1.0, 1.0, -std::numeric_limits<double>::quiet_NaN()));
	bool testStdFmaXTimesYNotZeroTimesInfAndNegNanOnZIsNan		= std::isnan(std::fma(1.0, 1.0, -std::numeric_limits<double>::quiet_NaN()));
	bool testCcmFmaXTimesYNotZeroTimesInfAndNegNanOnZIsSameSign = std::signbit(ccm::fma(1.0, 1.0, -std::numeric_limits<double>::quiet_NaN()));
	bool testStdFmaXTimesYNotZeroTimesInfAndNegNanOnZIsSameSign = std::signbit(std::fma(1.0, 1.0, -std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(testCcmFmaXTimesYNotZeroTimesInfAndNegNanOnZIsNan, testStdFmaXTimesYNotZeroTimesInfAndNegNanOnZIsNan);
	EXPECT_EQ(testCcmFmaXTimesYNotZeroTimesInfAndNegNanOnZIsSameSign, testStdFmaXTimesYNotZeroTimesInfAndNegNanOnZIsSameSign);


*/
}
