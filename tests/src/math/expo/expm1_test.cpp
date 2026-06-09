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

TEST(CcmathExponentialTests, Expm1IdentityAtZero)
{
	EXPECT_EQ(ccm::expm1(0.0), 0.0);
	EXPECT_EQ(ccm::expm1(-0.0), -0.0);
	EXPECT_FLOAT_EQ(ccm::expm1(0.0F), 0.0F);
}

TEST(CcmathExponentialTests, Expm1MatchesLibm)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kExpoDouble, ccm::expm1<double>, static_cast<double (*)(double)>(std::expm1));
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kExpoFloat, ccm::expm1<float>, static_cast<float (*)(float)>(std::expm1));
	ccm::test::ExpectUnaryMatchesStd(0.1323242190755991, ccm::expm1<double>, static_cast<double (*)(double)>(std::expm1));
}
