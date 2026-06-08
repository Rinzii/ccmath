/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "utils/std_compare.hpp"

#include <gtest/gtest.h>

#include <ccmath/ccmath.hpp>

#include <cmath>
#include <limits>

TEST(CcmathBasicTests, LerpStaticAssert)
{ static_assert(ccm::lerp(1, 2, 0.5) == 1.5, "lerp has failed testing that it is static_assert-able!"); }

TEST(CcmathBasicTests, LerpFloat)
{
	ccm::test::ExpectTernaryMatchesStd(1.0F, 2.0F, 0.0F, ccm::lerp<float>, static_cast<float (*)(float, float, float)>(std::lerp));
	ccm::test::ExpectTernaryMatchesStd(1.0F, 2.0F, 1.0F, ccm::lerp<float>, static_cast<float (*)(float, float, float)>(std::lerp));
	ccm::test::ExpectTernaryMatchesStd(4.0F, 4.0F, 0.5F, ccm::lerp<float>, static_cast<float (*)(float, float, float)>(std::lerp));
	ccm::test::ExpectTernaryMatchesStd(
		8.457017397928852e37F, 8.457017397928852e37F, 8.457017397928852e37F, ccm::lerp<float>, static_cast<float (*)(float, float, float)>(std::lerp));
	EXPECT_TRUE(std::isnan(ccm::lerp(std::numeric_limits<float>::quiet_NaN(), 0.0F, 3.857427150267069e-39F)));

	ccm::test::ExpectTernaryMatchesStd(1e8F, 1.0F, 0.5F, ccm::lerp<float>, static_cast<float (*)(float, float, float)>(std::lerp));

	// NOLINTBEGIN
	for (float t = -2.0; t <= 2.0; t += static_cast<float>(0.5))
	{
		// Testing extrapolation
		// Expected values are: -5 -2.5 0 2.5 5 7.5 10 12.5 15
		ccm::test::ExpectTernaryMatchesStd(5.0, 10.0, static_cast<double>(t), ccm::lerp<double>, static_cast<double (*)(double, double, double)>(std::lerp));
	}
	// NOLINTEND
}
