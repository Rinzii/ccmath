/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "utils/ulp_distance.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>

TEST(CcmathUlpDistanceTests, SignedZerosHaveZeroFiniteDistanceButNotBitwiseEquality)
{
	const auto result = ccm::test::ulp::classify_distance(0.0, -0.0);
	EXPECT_EQ(result.kind, ccm::test::ulp::relation::finite);
	EXPECT_EQ(result.distance, 0U);
	EXPECT_TRUE(result.numerically_equal);
	EXPECT_FALSE(result.exact_bitwise_equal);
}

TEST(CcmathUlpDistanceTests, AdjacentFloatValuesAreOneUlpApart)
{
	const float a	  = 1.0F;
	const float b	  = std::nextafter(a, std::numeric_limits<float>::infinity());
	const auto result = ccm::test::ulp::classify_distance(a, b);
	EXPECT_EQ(result.kind, ccm::test::ulp::relation::finite);
	EXPECT_EQ(result.distance, 1U);
}

TEST(CcmathUlpDistanceTests, SubnormalsAreMeasuredOnRawBitOrdering)
{
	const double a	  = std::numeric_limits<double>::denorm_min();
	const double b	  = std::nextafter(a, std::numeric_limits<double>::infinity());
	const auto result = ccm::test::ulp::classify_distance(a, b);
	EXPECT_EQ(result.kind, ccm::test::ulp::relation::finite);
	EXPECT_EQ(result.distance, 1U);
}

TEST(CcmathUlpDistanceTests, SameSignInfinitiesCompareAsZeroDistance)
{
	const auto result = ccm::test::ulp::classify_distance(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
	EXPECT_EQ(result.kind, ccm::test::ulp::relation::both_infinity_same_sign);
	EXPECT_EQ(result.distance, 0U);
	EXPECT_TRUE(result.exact_bitwise_equal);
}

TEST(CcmathUlpDistanceTests, NaNMismatchIsNotFoldedIntoNumericDistance)
{
	const auto result = ccm::test::ulp::classify_distance(std::numeric_limits<double>::quiet_NaN(), 1.0);
	EXPECT_EQ(result.kind, ccm::test::ulp::relation::nan_mismatch);
	EXPECT_EQ(ccm::test::ulp::distance_or_max(std::numeric_limits<double>::quiet_NaN(), 1.0), std::numeric_limits<std::uint64_t>::max());
}
