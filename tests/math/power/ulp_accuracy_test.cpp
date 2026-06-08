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
#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "utils/math_samples.hpp"
#include "utils/ulp_suite.hpp"

#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <cstdint>
#include <limits>

TEST(CcmathPowerUlpTests, SqrtDouble)
{ ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kSqrtDouble, ccm::sqrt<double>, static_cast<double (*)(double)>(std::sqrt)); }

TEST(CcmathPowerUlpTests, SqrtFloat)
{ ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kSqrtFloat, ccm::sqrt<float>, static_cast<float (*)(float)>(std::sqrt)); }

// [cmath.syn] and [c.math]/1: validates the primary double overload against the C library semantics used by the standard.
TEST(CcmathPowerUlpTests, PowDouble)
{
	for (double base : ccm::test::samples::kPowBases)
	{
		for (double exp : ccm::test::samples::kPowExponents)
		{
			SCOPED_TRACE(base);
			SCOPED_TRACE(exp);
			if (std::isnan(std::pow(base, exp))) { continue; }
			ccm::test::ExpectUlpBinaryVsStd(base, exp, ccm::pow<double>, static_cast<double (*)(double, double)>(std::pow));
		}
	}
}

// [c.math]/1: validates the generic double fallback used for constant evaluation and non-builtin runtime paths.
TEST(CcmathPowerUlpTests, PowGenDouble)
{
	for (double base : ccm::test::samples::kPowBases)
	{
		for (double exp : ccm::test::samples::kPowExponents)
		{
			SCOPED_TRACE(base);
			SCOPED_TRACE(exp);
			if (std::isnan(std::pow(base, exp))) { continue; }
			ccm::test::ExpectSameFloatingAsStd(ccm::gen::pow_gen(base, exp), std::pow(base, exp), 1);
		}
	}
}

// [c.math]/1: regression coverage for double fallback inputs that previously produced large ULP outliers.
TEST(CcmathPowerUlpTests, PowGenDoubleRegressionCases)
{
	struct PowCase
	{
		double base;
		double exp;
	};

	constexpr std::array<PowCase, 3> kCases = { {
		{ 10.0, 4.0 },
		{ 30.637028068178267, -7.702539522452998 },
		{ 945971881662.053466796875, 15.38309228199631562 },
	} };

	for (const PowCase & test_case : kCases)
	{
		SCOPED_TRACE(test_case.base);
		SCOPED_TRACE(test_case.exp);
		ccm::test::ExpectSameFloatingAsStd(ccm::gen::pow_gen(test_case.base, test_case.exp), std::pow(test_case.base, test_case.exp), 1);
	}
}

// [cmath.syn]: validates the named float entry point required by the synopsis.
TEST(CcmathPowerUlpTests, PowFloat)
{
	for (std::size_t i = 0; i < ccm::test::samples::kPowFloatPairsBase.size(); ++i)
	{
		const float base = ccm::test::samples::kPowFloatPairsBase[i];
		const float exp	 = ccm::test::samples::kPowFloatPairsExp[i];
		SCOPED_TRACE(base);
		SCOPED_TRACE(exp);
		if (std::isnan(std::pow(base, exp))) { continue; }
		ccm::test::ExpectUlpBinaryVsStd(base, exp, ccm::powf, static_cast<float (*)(float, float)>(std::pow));
	}
}

// [c.math]/1: checks that the implementation follows the C library pow semantics for negative bases and non-integral exponents.
TEST(CcmathPowerUlpTests, PowNegativeBaseIntegerExponent)
{
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(-4.0, 2.0), std::pow(-4.0, 2.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(-4.0, 3.0), std::pow(-4.0, 3.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(-2.0, 4.0), std::pow(-2.0, 4.0));
	ccm::test::ExpectDomainEdgeMatchesStd(-4.0, [](double b) { return ccm::pow(b, 0.5); }, [](double b) { return std::pow(b, 0.5); });
	ccm::test::ExpectDomainEdgeMatchesStd(-4.0, [](double b) { return ccm::pow(b, 1.5); }, [](double b) { return std::pow(b, 1.5); });
}

// [c.math]/1: checks that standard special-case behavior is preserved for NaNs, zero exponents, and pole cases.
TEST(CcmathPowerUlpTests, PowSpecialCasesMatchLibm)
{
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(1.0, 0.0), std::pow(1.0, 0.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(2.0, 0.0), std::pow(2.0, 0.0));
	ccm::test::ExpectDomainEdgeMatchesStd(
		std::numeric_limits<double>::quiet_NaN(), [](double x) { return ccm::pow(x, 1.0); }, [](double x) { return std::pow(x, 1.0); });
	ccm::test::ExpectDomainEdgeMatchesStd(
		2.0,
		[](double x) { return ccm::pow(x, std::numeric_limits<double>::quiet_NaN()); },
		[](double x) { return std::pow(x, std::numeric_limits<double>::quiet_NaN()); });
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(0.0, 2.0), std::pow(0.0, 2.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(0.0, -1.0), std::pow(0.0, -1.0));
}

// Hits every one of the 128 first-stage mantissa reduction buckets for the
// generic double kernel.  Each base is the left endpoint of bucket k (1 + k/128),
// and the 16-point exponent grid is chosen to spread lo6 = 64*frac(y*log2(x))
// across the exp2 polynomial domain [-0.5, 0.5].
TEST(CcmathPowerUlpTests, PowGenDoubleAllMantissaBuckets)
{
	for (int k = 0; k < 128; ++k)
	{
		const double base = 1.0 + static_cast<double>(k) / 128.0;
		for (double exp : ccm::test::samples::kPowExpGrid)
		{
			if (std::isnan(std::pow(base, exp))) { continue; }
			SCOPED_TRACE(base);
			SCOPED_TRACE(exp);
			ccm::test::ExpectSameFloatingAsStd(ccm::gen::pow_gen(base, exp), std::pow(base, exp), 1);
		}
	}
}

// Same coverage for the generic float kernel.
TEST(CcmathPowerUlpTests, PowGenFloatAllMantissaBuckets)
{
	for (int k = 0; k < 128; ++k)
	{
		const float base = 1.0F + static_cast<float>(k) / 128.0F;
		for (float exp : ccm::test::samples::kPowExpGridFloat)
		{
			if (std::isnan(std::pow(base, exp))) { continue; }
			SCOPED_TRACE(base);
			SCOPED_TRACE(exp);
			ccm::test::ExpectSameFloatingAsStd(ccm::gen::pow_gen(base, exp), std::pow(base, exp), 1);
		}
	}
}

// Exhaustive sweep over all 2^23 mantissa patterns in [1, 2) for the generic
// float kernel.  Every dx value the float log polynomial will ever receive is
// exercised. Failures are batched so the loop runs without per-iteration GTest
// overhead. Only the first failure and the total count are reported.
TEST(CcmathPowerUlpTests, DISABLED_PowGenFloatExhaustiveMantissa)
{
	constexpr std::array<float, 4> exponents = { 2.0F, 0.5F, -1.0F, 3.14159265F };

	int64_t fail_count		= 0;
	float first_fail_base	= 0.0F;
	float first_fail_exp	= 0.0F;
	float first_fail_actual = 0.0F;
	float first_fail_expect = 0.0F;

	for (std::uint32_t mant = 0; mant < (1u << 23); ++mant)
	{
		const float base = ccm::support::bit_cast<float>(0x3f800000u | mant);
		for (float exp : exponents)
		{
			const float expected = std::pow(base, exp);
			if (std::isnan(expected)) { continue; }
			const float actual = ccm::gen::pow_gen(base, exp);
			if (ulp_difference(actual, expected) > ccm::test::kMaxAllowedUlp)
			{
				if (fail_count == 0)
				{
					first_fail_base	  = base;
					first_fail_exp	  = exp;
					first_fail_actual = actual;
					first_fail_expect = expected;
				}
				++fail_count;
			}
		}
	}

	EXPECT_EQ(fail_count, 0) << "first failure: base=" << first_fail_base << " exp=" << first_fail_exp << " pow_gen=" << first_fail_actual
							 << " std::pow=" << first_fail_expect << " ulp=" << ulp_difference(first_fail_actual, first_fail_expect);
}

TEST(CcmathPowerUlpTests, CbrtDouble)
{ ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kCbrtDouble, ccm::cbrt<double>, static_cast<double (*)(double)>(std::cbrt)); }

TEST(CcmathPowerUlpTests, CbrtFloat)
{ ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kCbrtFloat, ccm::cbrt<float>, static_cast<float (*)(float)>(std::cbrt)); }

TEST(CcmathPowerUlpTests, HypotDouble)
{
	for (std::size_t i = 0; i < ccm::test::samples::kHypotPairXDouble.size(); ++i)
	{
		SCOPED_TRACE(ccm::test::samples::kHypotPairXDouble[i]);
		SCOPED_TRACE(ccm::test::samples::kHypotPairYDouble[i]);
		ccm::test::ExpectUlpBinaryVsStd(ccm::test::samples::kHypotPairXDouble[i],
										ccm::test::samples::kHypotPairYDouble[i],
										ccm::hypot<double>,
										static_cast<double (*)(double, double)>(std::hypot));
	}
}

TEST(CcmathPowerUlpTests, HypotFloat)
{
	for (std::size_t i = 0; i < ccm::test::samples::kHypotPairXFloat.size(); ++i)
	{
		SCOPED_TRACE(ccm::test::samples::kHypotPairXFloat[i]);
		SCOPED_TRACE(ccm::test::samples::kHypotPairYFloat[i]);
		ccm::test::ExpectUlpBinaryVsStd(ccm::test::samples::kHypotPairXFloat[i],
										ccm::test::samples::kHypotPairYFloat[i],
										ccm::hypot<float>,
										static_cast<float (*)(float, float)>(std::hypot));
	}
}
