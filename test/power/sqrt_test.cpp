/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// Workaround for the GitHub Actions causing SEH exceptions
// https://github.com/actions/runner-images/issues/10004#
// NOLINTNEXTLINE

#include <gtest/gtest.h>

#include "ccmath/ccmath.hpp"

#include <cmath>
#include <limits>

/*
TEST(CcmathPowerTests, Sqrt_StaticAssert)
{
	//static_assert(ccm::sqrt(2.0) == ccm::sqrt(2.0), "ccm::sqrt is not a compile time constant!");
}
*/

TEST(CcmathPowerTests, Sqrt_Double_CheckCCM)
{
	EXPECT_EQ(ccm::sqrt(0.0), ccm::sqrt(0.0));
}

TEST(CcmathPowerTests, Sqrt_Double_CheckCCM_static)
{
	constexpr double sqrt_0 = ccm::sqrt(0.0);
	EXPECT_EQ(sqrt_0, sqrt_0);
}

TEST(CcmathPowerTests, Sqrt_Double_CheckSTD)
{
	EXPECT_EQ(std::sqrt(0.0), std::sqrt(0.0));
}

/*
TEST(CcmathPowerTests, Sqrt_Double)
{
	// Test values that are 2^x

	//simd::sqrt<T, simd::abi::native>(num);
	//rt::internal_sqrt_rt




	EXPECT_EQ(ccm::sqrt(0.0), ccm::sqrt(0.0));

	EXPECT_EQ(ccm::sqrt(1.0), std::sqrt(1.0));
	EXPECT_EQ(ccm::sqrt(2.0), std::sqrt(2.0));
	EXPECT_EQ(ccm::sqrt(4.0), std::sqrt(4.0));
	EXPECT_EQ(ccm::sqrt(8.0), std::sqrt(8.0));
	EXPECT_EQ(ccm::sqrt(16.0), std::sqrt(16.0));
	EXPECT_EQ(ccm::sqrt(32.0), std::sqrt(32.0));
	EXPECT_EQ(ccm::sqrt(64.0), std::sqrt(64.0));
	EXPECT_EQ(ccm::sqrt(128.0), std::sqrt(128.0));
	EXPECT_EQ(ccm::sqrt(256.0), std::sqrt(256.0));

	// Test decimal values
	EXPECT_EQ(ccm::sqrt(0.1), std::sqrt(0.1));
	EXPECT_EQ(ccm::sqrt(0.2), std::sqrt(0.2));
	EXPECT_EQ(ccm::sqrt(0.3), std::sqrt(0.3));
	EXPECT_EQ(ccm::sqrt(0.4), std::sqrt(0.4));
	EXPECT_EQ(ccm::sqrt(0.5), std::sqrt(0.5));

}


TEST(CcmathPowerTests, Sqrt_Double_EdgeCases)
{
	// Test edge cases
	EXPECT_EQ(ccm::sqrt(std::numeric_limits<double>::min()), std::sqrt(std::numeric_limits<double>::min()));
	EXPECT_EQ(ccm::sqrt(std::numeric_limits<double>::max()), std::sqrt(std::numeric_limits<double>::max()));


	//EXPECT_EQ(ccm::sqrt(std::numeric_limits<double>::lowest()), std::sqrt(std::numeric_limits<double>::lowest()));
}

TEST(CcmathPowerTests, Sqrt_LDouble)
{
	// Test values that are 2^x
	EXPECT_EQ(ccm::sqrt(0.0L), std::sqrt(0.0L));
	EXPECT_EQ(ccm::sqrt(1.0L), std::sqrt(1.0L));
	EXPECT_EQ(ccm::sqrt(2.0L), std::sqrt(2.0L));
	EXPECT_EQ(ccm::sqrt(4.0L), std::sqrt(4.0L));
	EXPECT_EQ(ccm::sqrt(8.0L), std::sqrt(8.0L));
	EXPECT_EQ(ccm::sqrt(16.0L), std::sqrt(16.0L));
	EXPECT_EQ(ccm::sqrt(32.0L), std::sqrt(32.0L));
	EXPECT_EQ(ccm::sqrt(64.0L), std::sqrt(64.0L));
	EXPECT_EQ(ccm::sqrt(128.0L), std::sqrt(128.0L));
	EXPECT_EQ(ccm::sqrt(256.0L), std::sqrt(256.0L));

	// Test decimal values
	EXPECT_EQ(ccm::sqrt(0.1L), std::sqrt(0.1L));
	EXPECT_EQ(ccm::sqrt(0.2L), std::sqrt(0.2L));
	EXPECT_EQ(ccm::sqrt(0.3L), std::sqrt(0.3L));
	EXPECT_EQ(ccm::sqrt(0.4L), std::sqrt(0.4L));
	EXPECT_EQ(ccm::sqrt(0.5L), std::sqrt(0.5L));
}

TEST(CcmathPowerTests, Sqrt_LDouble_EdgeCases)
{
	// Test edge cases
	EXPECT_EQ(ccm::sqrt(std::numeric_limits<long double>::min()), std::sqrt(std::numeric_limits<long double>::min()));
	EXPECT_EQ(ccm::sqrt(std::numeric_limits<long double>::max()), std::sqrt(std::numeric_limits<long double>::max()));
}

#if !(defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__))

TEST(CcmathPowerTests, Sqrt_RT_Float)
{
	// Test values that are 2^x

	using namespace ccm::rt;

	EXPECT_EQ(sqrt_rt(0.0F), std::sqrt(0.0F));
	EXPECT_EQ(sqrt_rt(1.0F), std::sqrt(1.0F));
	EXPECT_EQ(sqrt_rt(2.0F), std::sqrt(2.0F));
	EXPECT_EQ(sqrt_rt(4.0F), std::sqrt(4.0F));
	EXPECT_EQ(sqrt_rt(8.0F), std::sqrt(8.0F));
	EXPECT_EQ(sqrt_rt(16.0F), std::sqrt(16.0F));
	EXPECT_EQ(sqrt_rt(32.0F), std::sqrt(32.0F));
	EXPECT_EQ(sqrt_rt(64.0F), std::sqrt(64.0F));
	EXPECT_EQ(sqrt_rt(128.0F), std::sqrt(128.0F));
	EXPECT_EQ(sqrt_rt(256.0F), std::sqrt(256.0F));

	// Test decimal values
	EXPECT_EQ(sqrt_rt(0.1F), std::sqrt(0.1F));
	EXPECT_EQ(sqrt_rt(0.2F), std::sqrt(0.2F));
	EXPECT_EQ(sqrt_rt(0.3F), std::sqrt(0.3F));
	EXPECT_EQ(sqrt_rt(0.4F), std::sqrt(0.4F));
	EXPECT_EQ(sqrt_rt(0.5F), std::sqrt(0.5F));
}

TEST(CcmathPowerTests, Sqrt_RT_Float_EdgeCases)
{
	using namespace ccm::rt;

	// Test edge cases
	EXPECT_EQ(sqrt_rt(std::numeric_limits<float>::min()), std::sqrt(std::numeric_limits<float>::min()));
	EXPECT_EQ(sqrt_rt(std::numeric_limits<float>::max()), std::sqrt(std::numeric_limits<float>::max()));


	//EXPECT_EQ(ccm::sqrt(std::numeric_limits<double>::lowest()), std::sqrt(std::numeric_limits<double>::lowest()));
}

TEST(CcmathPowerTests, Sqrt_RT_Double)
{
	// Test values that are 2^x

	using namespace ccm::rt;

	EXPECT_EQ(sqrt_rt(0.0), std::sqrt(0.0));
	EXPECT_EQ(sqrt_rt(1.0), std::sqrt(1.0));
	EXPECT_EQ(sqrt_rt(2.0), std::sqrt(2.0));
	EXPECT_EQ(sqrt_rt(4.0), std::sqrt(4.0));
	EXPECT_EQ(sqrt_rt(8.0), std::sqrt(8.0));
	EXPECT_EQ(sqrt_rt(16.0), std::sqrt(16.0));
	EXPECT_EQ(sqrt_rt(32.0), std::sqrt(32.0));
	EXPECT_EQ(sqrt_rt(64.0), std::sqrt(64.0));
	EXPECT_EQ(sqrt_rt(128.0), std::sqrt(128.0));
	EXPECT_EQ(sqrt_rt(256.0), std::sqrt(256.0));

	// Test decimal values
	EXPECT_EQ(sqrt_rt(0.1), std::sqrt(0.1));
	EXPECT_EQ(sqrt_rt(0.2), std::sqrt(0.2));
	EXPECT_EQ(sqrt_rt(0.3), std::sqrt(0.3));
	EXPECT_EQ(sqrt_rt(0.4), std::sqrt(0.4));
	EXPECT_EQ(sqrt_rt(0.5), std::sqrt(0.5));
}

TEST(CcmathPowerTests, Sqrt_RT_Double_EdgeCases)
{
	using namespace ccm::rt;

	// Test edge cases
	EXPECT_EQ(sqrt_rt(std::numeric_limits<double>::min()), std::sqrt(std::numeric_limits<double>::min()));
	EXPECT_EQ(sqrt_rt(std::numeric_limits<double>::max()), std::sqrt(std::numeric_limits<double>::max()));


	//EXPECT_EQ(ccm::sqrt(std::numeric_limits<double>::lowest()), std::sqrt(std::numeric_limits<double>::lowest()));
}

// LDouble test cases are to ensure LDouble still works during runtime.
// If LDouble is not the same as double Simd will not be used and will instead use the generic runtime version.

TEST(CcmathPowerTests, Sqrt_RT_LDouble)
{
	// Test values that are 2^x

	using namespace ccm::rt;

	EXPECT_EQ(sqrt_rt(0.0L), std::sqrt(0.0L));
	EXPECT_EQ(sqrt_rt(1.0L), std::sqrt(1.0L));
	EXPECT_EQ(sqrt_rt(2.0L), std::sqrt(2.0L));
	EXPECT_EQ(sqrt_rt(4.0L), std::sqrt(4.0L));
	EXPECT_EQ(sqrt_rt(8.0L), std::sqrt(8.0L));
	EXPECT_EQ(sqrt_rt(16.0L), std::sqrt(16.0L));
	EXPECT_EQ(sqrt_rt(32.0L), std::sqrt(32.0L));
	EXPECT_EQ(sqrt_rt(64.0L), std::sqrt(64.0L));
	EXPECT_EQ(sqrt_rt(128.0L), std::sqrt(128.0L));
	EXPECT_EQ(sqrt_rt(256.0L), std::sqrt(256.0L));

	// Test decimal values
	EXPECT_EQ(sqrt_rt(0.1L), std::sqrt(0.1L));
	EXPECT_EQ(sqrt_rt(0.2L), std::sqrt(0.2L));
	EXPECT_EQ(sqrt_rt(0.3L), std::sqrt(0.3L));
	EXPECT_EQ(sqrt_rt(0.4L), std::sqrt(0.4L));
	EXPECT_EQ(sqrt_rt(0.5L), std::sqrt(0.5L));
}

TEST(CcmathPowerTests, Sqrt_RT_LDouble_EdgeCases)
{
	using namespace ccm::rt;

	// Test edge cases
	EXPECT_EQ(sqrt_rt(std::numeric_limits<long double>::min()), std::sqrt(std::numeric_limits<long double>::min()));
	EXPECT_EQ(sqrt_rt(std::numeric_limits<long double>::max()), std::sqrt(std::numeric_limits<long double>::max()));


	//EXPECT_EQ(ccm::sqrt(std::numeric_limits<double>::lowest()), std::sqrt(std::numeric_limits<double>::lowest()));
}
#endif // defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__)
*/
