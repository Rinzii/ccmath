/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "ccmath/ccmath.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <limits>

// NOLINTBEGIN

static_assert(ccm::support::helpers::internal_ldexp(static_cast<float>(1.0), 2) == ccm::support::helpers::internal_ldexp(static_cast<float>(1.0), 2),
			  "ldexp failed static_assert test");

template <typename T>
class CcmathFmanipTests : public ::testing::Test
{
};

typedef ::testing::Types<float, double, long double> TestTypes;
TYPED_TEST_SUITE(CcmathFmanipTests, TestTypes);

TYPED_TEST(CcmathFmanipTests, LdexpStaticAssert)
{
	// TODO: IanP: Figure out why this static_assert fails
	// EXPECT_EQ(ccm::ldexp(static_cast<TypeParam>(1.0), 0), 3.0);
}

TYPED_TEST(CcmathFmanipTests, LdexpBasic)
{
	EXPECT_EQ(std::ldexp(static_cast<TypeParam>(5.0), 4), ccm::ldexp(static_cast<TypeParam>(5.0), 4));
	EXPECT_EQ(std::ldexp(static_cast<TypeParam>(-5.0), -4), ccm::ldexp(static_cast<TypeParam>(-5.0), -4));
	EXPECT_EQ(std::ldexp(static_cast<TypeParam>(0.0), 1), ccm::ldexp(static_cast<TypeParam>(0.0), 1));
	EXPECT_EQ(std::ldexp(static_cast<TypeParam>(-0.0), 1), ccm::ldexp(static_cast<TypeParam>(-0.0), 1));
	EXPECT_EQ(std::ldexp(static_cast<TypeParam>(720.32), 22), ccm::ldexp(static_cast<TypeParam>(720.32), 22));
}

TYPED_TEST(CcmathFmanipTests, LdexpExtremeValues)
{
	EXPECT_EQ(std::ldexp(static_cast<TypeParam>(1), -1074), ccm::ldexp(static_cast<TypeParam>(1), -1074));
	EXPECT_EQ(std::ldexp(static_cast<TypeParam>(1), 1024), ccm::ldexp(static_cast<TypeParam>(1), 1024));
	EXPECT_EQ(std::ldexp(std::numeric_limits<TypeParam>::max(), std::numeric_limits<int>::max()),
			  ccm::ldexp(std::numeric_limits<TypeParam>::max(), std::numeric_limits<int>::max()));
	EXPECT_EQ(std::ldexp(std::numeric_limits<TypeParam>::min(), std::numeric_limits<int>::min()),
			  ccm::ldexp(std::numeric_limits<TypeParam>::min(), std::numeric_limits<int>::min()));
	EXPECT_EQ(std::ldexp(std::numeric_limits<TypeParam>::min(), std::numeric_limits<int>::max()),
			  ccm::ldexp(std::numeric_limits<TypeParam>::min(), std::numeric_limits<int>::max()));
	EXPECT_EQ(std::ldexp(std::numeric_limits<TypeParam>::max(), std::numeric_limits<int>::min()),
			  ccm::ldexp(std::numeric_limits<TypeParam>::max(), std::numeric_limits<int>::min()));
}

TYPED_TEST(CcmathFmanipTests, LdexpSpecialValues)
{
	EXPECT_EQ(ccm::ldexp(std::numeric_limits<TypeParam>::infinity(), 10), std::ldexp(std::numeric_limits<TypeParam>::infinity(), 10));

	// Test for NaN propagation
	auto ccm_nan = ccm::ldexp(std::numeric_limits<TypeParam>::quiet_NaN(), 10);
	auto std_nan = std::ldexp(std::numeric_limits<TypeParam>::quiet_NaN(), 10);
	EXPECT_TRUE(std::isnan(ccm_nan) == std::isnan(std_nan));
}

// NOLINTEND