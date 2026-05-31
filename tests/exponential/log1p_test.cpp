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
#include "utils/math_samples.hpp"
#include "utils/std_compare.hpp"
#include "utils/ulp_suite.hpp"

TEST(CcmathExponentialTests, Log1pIdentityAtZero)
{
	EXPECT_EQ(ccm::log1p(0.0), 0.0);
	EXPECT_EQ(ccm::log1p(-0.0), -0.0);
	EXPECT_FLOAT_EQ(ccm::log1p(0.0F), 0.0F);
}

TEST(CcmathExponentialTests, Log1pMatchesLibm)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kExpoDouble, ccm::log1p<double>, static_cast<double (*)(double)>(std::log1p));
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kExpoFloat, ccm::log1p<float>, static_cast<float (*)(float)>(std::log1p));
}

TEST(CcmathExponentialTests, Log1pTypedAliases)
{
	EXPECT_DOUBLE_EQ(ccm::log1pl(0.1L), ccm::log1p(0.1L));
	EXPECT_FLOAT_EQ(ccm::log1pf(0.1F), ccm::log1p(0.1F));
}

TEST(CcmathExponentialTests, Log1pNegativeDomainEdges)
{
	ccm::test::ExpectUnaryMatchesStd(-1.0, ccm::log1p<double>, static_cast<double (*)(double)>(std::log1p));
	ccm::test::ExpectUnaryMatchesStd(-1.0F, ccm::log1p<float>, static_cast<float (*)(float)>(std::log1p));

	ccm::test::ExpectCcmNegativeDomainNaN(-std::numeric_limits<double>::infinity(), ccm::log1p<double>);
	ccm::test::ExpectCcmNegativeDomainNaN(-2.0, ccm::log1p<double>);
}
