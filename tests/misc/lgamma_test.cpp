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
		const T scale	 = std::fabs(expected) > T(1) ? std::fabs(expected) : T(1);
		EXPECT_NEAR(actual, expected, rel_tol * scale);
	}
} // namespace

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
#if defined(_MSC_VER) && !defined(__clang__)
		ExpectRelativeNearStd(x, ccm::lgamma<double>, static_cast<double (*)(double)>(std::lgamma), 1e-3);
#else
		ccm::test::ExpectUlpUnaryVsStd(x, ccm::lgamma<double>, static_cast<double (*)(double)>(std::lgamma));
#endif
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
	const double samples[] = { -0.25, -1.5, -2.5, -3.25, -10.5 };
	for (double x : samples)
	{
		SCOPED_TRACE(x);
#if defined(_MSC_VER) && !defined(__clang__)
		ExpectRelativeNearStd(x, ccm::lgamma<double>, static_cast<double (*)(double)>(std::lgamma), 1e-3);
#else
		ccm::test::ExpectUlpUnaryVsStd(x, ccm::lgamma<double>, static_cast<double (*)(double)>(std::lgamma));
#endif
	}
}
