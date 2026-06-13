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
#include "utils/math_samples.hpp"
#include "utils/std_compare.hpp"
#include "utils/ulp_suite.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>

TEST(CcmathPowerTests, HypotMatchesStdGrid)
{
	for (std::size_t i = 0; i < ccm::test::samples::kHypotPairXDouble.size(); ++i)
	{
		SCOPED_TRACE(ccm::test::samples::kHypotPairXDouble[i]);
		SCOPED_TRACE(ccm::test::samples::kHypotPairYDouble[i]);
		ccm::test::ExpectSameFloatingAsStd(ccm::hypot(ccm::test::samples::kHypotPairXDouble[i], ccm::test::samples::kHypotPairYDouble[i]),
										   std::hypot(ccm::test::samples::kHypotPairXDouble[i], ccm::test::samples::kHypotPairYDouble[i]));
	}
}

TEST(CcmathPowerTests, HypotMatchesStdGridFloat)
{
	for (std::size_t i = 0; i < ccm::test::samples::kHypotPairXFloat.size(); ++i)
	{
		SCOPED_TRACE(ccm::test::samples::kHypotPairXFloat[i]);
		SCOPED_TRACE(ccm::test::samples::kHypotPairYFloat[i]);
		ccm::test::ExpectSameFloatingAsStd(ccm::hypot(ccm::test::samples::kHypotPairXFloat[i], ccm::test::samples::kHypotPairYFloat[i]),
										   std::hypot(ccm::test::samples::kHypotPairXFloat[i], ccm::test::samples::kHypotPairYFloat[i]));
	}
}

TEST(CcmathPowerTests, HypotEdgeCases)
{
	const double nan = std::numeric_limits<double>::quiet_NaN();
	const double inf = std::numeric_limits<double>::infinity();
	ccm::test::ExpectSameFloatingAsStd(ccm::hypot(nan, 1.0), std::hypot(nan, 1.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::hypot(inf, 1.0), std::hypot(inf, 1.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::hypot(nan, inf), std::hypot(nan, inf));
	ccm::test::ExpectSameFloatingAsStd(ccm::hypot(inf, nan), std::hypot(inf, nan));
	ccm::test::ExpectSameFloatingAsStd(ccm::hypot(0.0, 0.0), std::hypot(0.0, 0.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::hypot(-0.0, 0.0), std::hypot(-0.0, 0.0));
}

TEST(CcmathPowerTests, HypotTypedAliases)
{
	EXPECT_DOUBLE_EQ(ccm::hypotf(3.0F, 4.0F), ccm::hypot(3.0F, 4.0F));
	EXPECT_EQ(ccm::hypotl(3.0L, 4.0L), ccm::hypot(3.0L, 4.0L));
}

TEST(CcmathPowerTests, CbrtMatchesStdGrid)
{
	for (double input : ccm::test::samples::kCbrtDouble)
	{
		SCOPED_TRACE(input);
		ccm::test::ExpectSameFloatingAsStd(ccm::cbrt(input), std::cbrt(input));
	}
}

TEST(CcmathPowerTests, CbrtMatchesStdGridFloat)
{
	for (float input : ccm::test::samples::kCbrtFloat)
	{
		SCOPED_TRACE(input);
		ccm::test::ExpectSameFloatingAsStd(ccm::cbrt(input), std::cbrt(input));
	}
}

TEST(CcmathPowerTests, CbrtEdgeCases)
{
	const double nan = std::numeric_limits<double>::quiet_NaN();
	const double inf = std::numeric_limits<double>::infinity();
	ccm::test::ExpectSameFloatingAsStd(ccm::cbrt(nan), std::cbrt(nan));
	ccm::test::ExpectSameFloatingAsStd(ccm::cbrt(inf), std::cbrt(inf));
	ccm::test::ExpectSameFloatingAsStd(ccm::cbrt(-inf), std::cbrt(-inf));
}

TEST(CcmathPowerTests, CbrtTypedAliases)
{
	EXPECT_FLOAT_EQ(ccm::cbrtf(27.0F), ccm::cbrt(27.0F));
	EXPECT_EQ(ccm::cbrtl(27.0L), ccm::cbrt(27.0L));
}

TEST(CcmathPowerTests, HypotCbrtCompileTime)
{
	static_assert(ccm::hypot(3.0, 0.0) == 3.0);
	static_assert(ccm::cbrt(8.0) == 2.0);
}
TEST(CcmathPowerTests, HypotOverflowBoundary)
{
    constexpr float x = 3.40282347e38f;
    constexpr float y = 9.18e34f;

    ccm::test::ExpectSameFloatingAsStd(
        ccm::hypot(x, y),
        std::hypot(x, y)
    );
}
