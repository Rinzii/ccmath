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
#include "ccmath/internal/config/long_double_format.hpp"
#include "ccmath/internal/config/powl_policy.hpp"
#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
#include "oracle/powl_path_reporting.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <iostream>

namespace
{
	using ccm::test::oracle::classify_powl_gen_path;
	using ccm::test::oracle::powl_path_is_native_phase1;
	using ccm::test::oracle::powl_path_is_reduced_precision;
	using ccm::test::oracle::PowlImplementationPath;

	void ExpectPath(long double base, long double exponent, PowlImplementationPath expected)
	{ EXPECT_EQ(classify_powl_gen_path(base, exponent), expected) << "base=" << base << " exponent=" << exponent; }
} // namespace

TEST(PowlFallbackPolicy, ReportsFallbackPolicyState)
{
	std::cout << "powl fallback policy enabled=" << (ccm::config::reduced_precision_powl_fallback_enabled() ? "yes" : "no")
			  << " format=" << ccm::config::long_double_format_name(ccm::config::detect_long_double_format()) << '\n';
}

TEST(PowlFallbackPolicy, Ld64AliasIsNotReducedPrecision)
{
	if (ccm::config::detect_long_double_format() != ccm::config::LongDoubleFormat::Double) { GTEST_SKIP() << "double-shaped long double required"; }

	ExpectPath(2.0L, 0.5L, PowlImplementationPath::Ld64DoubleAlias);
	EXPECT_FALSE(powl_path_is_reduced_precision(PowlImplementationPath::Ld64DoubleAlias));
	EXPECT_TRUE(powl_path_is_native_phase1(PowlImplementationPath::Ld64DoubleAlias));

	const long double actual   = ccm::gen::pow_gen(2.0L, 10.0L);
	const long double expected = static_cast<long double>(ccm::gen::pow_gen(2.0, 10.0));
	EXPECT_EQ(actual, expected);
}

#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
TEST(PowlFallbackPolicy, Ld80BoundedIntegerDoesNotUseReducedFallback)
{
	ExpectPath(2.0L, 10.0L, PowlImplementationPath::Ld80BoundedInteger);
	EXPECT_FALSE(powl_path_is_reduced_precision(classify_powl_gen_path(2.0L, 10.0L)));
	EXPECT_EQ(ccm::gen::pow_gen(2.0L, 10.0L), 1024.0L);
}

TEST(PowlFallbackPolicy, Ld80SpecialCaseDoesNotUseReducedFallback)
{
	ExpectPath(2.0L, 0.0L, PowlImplementationPath::Ld80SpecialCase);
	ExpectPath(1.0L, 2.0L, PowlImplementationPath::Ld80SpecialCase);
	EXPECT_EQ(ccm::gen::pow_gen(2.0L, 0.0L), 1.0L);
}

TEST(PowlFallbackPolicy, Ld80GeneralRealExponentUsesNativeKernel)
{
	const long double base	   = 2.0L;
	const long double exponent = 0.5L;

	ExpectPath(base, exponent, PowlImplementationPath::Ld80GeneralFinite);
	EXPECT_FALSE(powl_path_is_reduced_precision(classify_powl_gen_path(base, exponent)));
	const long double actual = ccm::gen::pow_gen(base, exponent);
	EXPECT_NEAR(static_cast<double>(actual), std::sqrt(2.0), 0x1.0p-50);
}
#endif

#if !defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT64) && !defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
TEST(PowlFallbackPolicy, UnsupportedTierPolicy)
{
	const long double base	   = 2.0L;
	const long double exponent = 3.0L;

	if (ccm::config::reduced_precision_powl_fallback_enabled())
	{
		const auto path = classify_powl_gen_path(base, exponent);
		EXPECT_TRUE(path == PowlImplementationPath::Ld128ReducedPrecisionFallback || path == PowlImplementationPath::UnknownReducedPrecisionFallback);
		EXPECT_TRUE(powl_path_is_reduced_precision(path));
	}
	else
	{
		const auto path = classify_powl_gen_path(base, exponent);
		EXPECT_TRUE(path == PowlImplementationPath::Ld128Unsupported || path == PowlImplementationPath::UnknownUnsupported);
		EXPECT_TRUE(std::isnan(ccm::gen::pow_gen(base, exponent)));
	}
}
#endif

#if defined(CCM_CONFIG_ENABLE_REDUCED_PRECISION_POWL) && defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
TEST(PowlFallbackPolicyCompat, Ld80GeneralStillUsesNativeKernelWhenFallbackEnabled)
{
	ExpectPath(3.0L, 0.25L, PowlImplementationPath::Ld80GeneralFinite);
	EXPECT_FALSE(powl_path_is_reduced_precision(classify_powl_gen_path(3.0L, 0.25L)));
}
#endif
