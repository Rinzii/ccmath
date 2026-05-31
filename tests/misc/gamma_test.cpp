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

TEST(CcmathMiscTests, GammaStaticAssert)
{
	static_assert(ccm::gamma(5.0) == 24.0, "gamma has failed testing that it is static_assert-able!");
	static_assert(ccm::gamma(1.0) == 1.0, "gamma(1) should be 1");
}

TEST(CcmathMiscTests, GammaEdgeCases)
{
	EXPECT_TRUE(std::isnan(ccm::gamma(-1.0)));
	EXPECT_TRUE(std::isnan(ccm::gamma(-2.0)));
	EXPECT_TRUE(std::isnan(ccm::gamma(std::numeric_limits<double>::quiet_NaN())));
	EXPECT_TRUE(std::isnan(ccm::gamma(-std::numeric_limits<double>::infinity())));
	EXPECT_EQ(ccm::gamma(std::numeric_limits<double>::infinity()), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::gamma(0.0), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::gamma(-0.0), -std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::gamma(5.0), std::tgamma(5.0));
	EXPECT_EQ(ccm::gamma(1.0), 1.0);
}

TEST(CcmathMiscTests, GammaMatchesLibmDouble)
{
	for (double x : ccm::test::samples::kGammaProbeDouble)
	{
		SCOPED_TRACE(x);
		if (x < 0.0 && x == std::trunc(x)) { continue; }
		ccm::test::ExpectUlpUnaryVsStd(x, ccm::gamma<double>, static_cast<double (*)(double)>(std::tgamma));
	}
}

TEST(CcmathMiscTests, GammaMatchesLibmFloat)
{
	for (float x : ccm::test::samples::kGammaProbeFloat)
	{
		SCOPED_TRACE(x);
		if (x < 0.0F && x == std::trunc(x)) { continue; }
		ccm::test::ExpectUlpUnaryVsStd(x, ccm::gamma<float>, static_cast<float (*)(float)>(std::tgamma));
	}
}
