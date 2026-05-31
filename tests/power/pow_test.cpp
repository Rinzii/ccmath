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
#include "utils/ulp_suite.hpp"

// Smoke tests for the public pow API. Accuracy and special cases live in ulp_accuracy_test.cpp.

TEST(CcmathPowerTests, PowStaticAssert)
{
	static_assert(ccm::pow(2.0, 3.0) == 8.0, "ccm::pow must be usable in constant evaluation");
}

TEST(CcmathPowerTests, PowIntegralOverload)
{
	EXPECT_DOUBLE_EQ(ccm::pow(2, 3), std::pow(2.0, 3.0));
}

TEST(CcmathPowerTests, PowSpotCheck)
{
	ccm::test::ExpectUlpBinaryVsStd(2.0, 10.0, ccm::pow<double>, static_cast<double (*)(double, double)>(std::pow));
	ccm::test::ExpectUlpBinaryVsStd(14.0, -0.5, ccm::pow<double>, static_cast<double (*)(double, double)>(std::pow));
}
