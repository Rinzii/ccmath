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

#include <ccmath/ccmath.hpp>
#include <cmath>
#include <limits>

#include "utils/math_samples.hpp"
#include "utils/ulp_suite.hpp"

TEST(CcmathMiscTests, LgammaStaticAssert)
{
	static_assert(ccm::lgamma(1.0) == 0.0, "lgamma(1) should be 0");
	static_assert(ccm::lgamma(2.0) == 0.0, "lgamma(2) should be 0");
}

TEST(CcmathMiscTests, LgammaEdgeCases)
{
	EXPECT_TRUE(std::isnan(ccm::lgamma(std::numeric_limits<double>::quiet_NaN())));
	EXPECT_EQ(ccm::lgamma(std::numeric_limits<double>::infinity()), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::lgamma(-std::numeric_limits<double>::infinity()), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::lgamma(0.0), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::lgamma(-0.0), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::lgamma(-1.0), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::lgamma(-2.0), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::lgamma(1.0), 0.0);
	EXPECT_EQ(ccm::lgamma(2.0), 0.0);
}

TEST(CcmathMiscTests, LgammaMatchesLibmDouble)
{
	for (double x : ccm::test::samples::kGammaProbeDouble)
	{
		SCOPED_TRACE(x);
		ccm::test::ExpectUlpUnaryVsStd(x, ccm::lgamma<double>, static_cast<double (*)(double)>(std::lgamma));
	}
}

TEST(CcmathMiscTests, LgammaMatchesLibmFloat)
{
	for (float x : ccm::test::samples::kGammaProbeFloat)
	{
		SCOPED_TRACE(x);
		ccm::test::ExpectUlpUnaryVsStd(x, ccm::lgamma<float>, static_cast<float (*)(float)>(std::lgamma));
	}
}

TEST(CcmathMiscTests, LgammaSignAgreement)
{
	// std::lgamma stores sign in signgam on some platforms. Compare magnitude only for negative non-integers.
	const double samples[] = {-0.25, -1.5, -2.5, -3.25, -10.5};
	for (double x : samples)
	{
		SCOPED_TRACE(x);
		ccm::test::ExpectUlpUnaryVsStd(x, ccm::lgamma<double>, static_cast<double (*)(double)>(std::lgamma));
	}
}
