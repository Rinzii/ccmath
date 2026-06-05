/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "utils/math_samples.hpp"
#include "utils/ulp_suite.hpp"

#include <gtest/gtest.h>

#include <ccmath/ccmath.hpp>

#include <cmath>
#include <limits>

namespace
{
	template <typename T, typename CcmFn, typename StdFn>
	void ExpectRelativeNearStd(T input, CcmFn ccm_fn, StdFn std_fn, T rel_tol)
	{
		const T actual	 = ccm_fn(input);
		const T expected = std_fn(input);
		if (actual == expected) { return; }
		const T scale = std::fabs(expected) > T(1) ? std::fabs(expected) : T(1);
		EXPECT_NEAR(actual, expected, rel_tol * scale);
	}
} // namespace

TEST(CcmathMiscTests, GammaStaticAssert)
{
	static_assert(ccm::tgamma(5.0) == 24.0, "tgamma has failed testing that it is static_assert-able!");
	static_assert(ccm::tgamma(1.0) == 1.0, "tgamma(1) should be 1");
}

TEST(CcmathMiscTests, GammaEdgeCases)
{
	EXPECT_TRUE(std::isnan(ccm::tgamma(-1.0)));
	EXPECT_TRUE(std::isnan(ccm::tgamma(-2.0)));
	EXPECT_TRUE(std::isnan(ccm::tgamma(std::numeric_limits<double>::quiet_NaN())));
	EXPECT_TRUE(std::isnan(ccm::tgamma(-std::numeric_limits<double>::infinity())));
	EXPECT_EQ(ccm::tgamma(std::numeric_limits<double>::infinity()), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::tgamma(0.0), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::tgamma(-0.0), -std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::tgamma(5.0), std::tgamma(5.0));
	EXPECT_EQ(ccm::tgamma(1.0), 1.0);
	EXPECT_EQ(ccm::gamma(5.0), ccm::tgamma(5.0));
}

TEST(CcmathMiscTests, GammaMatchesLibmDouble)
{
	for (double x : ccm::test::samples::kGammaProbeDouble)
	{
		SCOPED_TRACE(x);
		if (x < 0.0 && x == std::trunc(x)) { continue; }
#if defined(_MSC_VER) && !defined(__clang__)
		ExpectRelativeNearStd(x, ccm::tgamma<double>, static_cast<double (*)(double)>(std::tgamma), 1e-6);
#else
		ccm::test::ExpectUlpUnaryVsStd(x, ccm::tgamma<double>, static_cast<double (*)(double)>(std::tgamma));
#endif
	}
}

TEST(CcmathMiscTests, GammaMatchesLibmFloat)
{
	for (float x : ccm::test::samples::kGammaProbeFloat)
	{
		SCOPED_TRACE(x);
		if (x < 0.0F && x == std::trunc(x)) { continue; }
		ccm::test::ExpectUlpUnaryVsStd(x, ccm::tgamma<float>, static_cast<float (*)(float)>(std::tgamma));
	}
}
