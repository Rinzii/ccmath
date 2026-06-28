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
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kSqrtDouble, ccm::sqrt<double>, static_cast<double (*)(double)>(std::sqrt));
}

TEST(CcmathPowerUlpTests, SqrtFloat)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kSqrtFloat, ccm::sqrt<float>, static_cast<float (*)(float)>(std::sqrt));
}

// [cmath.syn] and [c.math]/1: validates the primary double overload against the C library semantics used by the standard.
TEST(CcmathPowerUlpTests, PowDouble)
{
	for (double base : ccm::test::samples::kPowBases)
	{
		for (double exp : ccm::test::samples::kPowExponents)
		{
			SCOPED_TRACE(base);
			SCOPED_TRACE(exp);
			if (std::isnan(std::pow(base, exp)))
			{
				continue;
			}
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
			if (std::isnan(std::pow(base, exp)))
			{
				continue;
			}
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

	constexpr std::array<PowCase, 6> kCases = { {
		{ 10.0, 4.0 },
		{ 30.637028068178267, -7.702539522452998 },
		{ 945971881662.053466796875, 15.38309228199631562 },
		// Base just above 1 with a huge exponent lands the result deep in the over/underflow
		// scaled band (here ~2^-735). That region used to drop to the fast single-double path
		// and lost ~38 ULP. It must now ride the accurate double-double reconstruction.
		{ 0x1.00000000000ffp+0, -0x1.0p53 },
		{ 0x1.000000000010p+0, -0x1.0p53 },
		// Symmetric overflow-adjacent band (~2^+735).
		{ 0x1.00000000000ffp+0, 0x1.0p53 },
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
		if (std::isnan(std::pow(base, exp)))
		{
			continue;
		}
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
			if (std::isnan(std::pow(base, exp)))
			{
				continue;
			}
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
			if (std::isnan(std::pow(base, exp)))
			{
				continue;
			}
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
			if (std::isnan(expected))
			{
				continue;
			}
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

// [c.math]/1: half-integer exponents take the exact dd_sqrt plus integer-power route, which the
// random and grid campaigns essentially never sample (half-integers are a measure-zero set).
// Covers positive and negative k out to the |2 exp| <= 2048 bound, including saturation parity.
TEST(CcmathPowerUlpTests, PowHalfIntegerExponentAccuracy)
{
	constexpr std::array<double, 4> kBases	   = { 3.7, 0.083, 1.001, 17.5 };
	constexpr std::array<double, 12> kHalfExps = { 1.5, -1.5, 2.5, -2.5, 7.5, -7.5, 99.5, -99.5, 501.5, -501.5, 1023.5, -1023.5 };

	for (double base : kBases)
	{
		for (double exp : kHalfExps)
		{
			SCOPED_TRACE(base);
			SCOPED_TRACE(exp);
			ccm::test::ExpectSameFloatingAsStd(ccm::pow(base, exp), std::pow(base, exp), 1);
			ccm::test::ExpectSameFloatingAsStd(ccm::gen::pow_gen(base, exp), std::pow(base, exp), 1);
		}
	}
}

// [c.math]/1: the dd_sqrt kernel rescales bases below 2^-1000 so the two_prod residual stays
// normal. Only exp = -0.5 can reach that branch with a normal result, so pin it directly,
// including a subnormal base.
TEST(CcmathPowerUlpTests, PowHalfIntegerTinySqrtBranch)
{
	constexpr std::array<double, 4> kTinyBases = { 0x1.0p-1010, 0x1.b333333333333p-1005, 0x1.0p-1040, 0x1.8p-1060 };

	for (double base : kTinyBases)
	{
		SCOPED_TRACE(base);
		ccm::test::ExpectSameFloatingAsStd(ccm::pow(base, -0.5), std::pow(base, -0.5), 1);
		ccm::test::ExpectSameFloatingAsStd(ccm::gen::pow_gen(base, -0.5), std::pow(base, -0.5), 1);
	}
}

// [c.math]/1: exponents beyond 0x43d74910d52d3052 are clamped to +/-2^100 before the kernel.
// Straddle the threshold from both sides with bases on both sides of one so the clamp keeps
// saturating to the same infinity or zero the C library produces.
TEST(CcmathPowerUlpTests, PowHugeExponentClampThreshold)
{
	const double threshold			  = ccm::support::bit_cast<double>(0x43d7'4910'd52d'3052ULL);
	const std::array<double, 5> kExps = {
		std::nextafter(threshold, 0.0), threshold, std::nextafter(threshold, std::numeric_limits<double>::infinity()), 1.0e19, 1.0e300
	};
	constexpr std::array<double, 4> kBases = { 1.0000000000000002, 0.9999999999999999, 2.0, 0.5 };

	for (double base : kBases)
	{
		for (double exp : kExps)
		{
			SCOPED_TRACE(base);
			SCOPED_TRACE(exp);
			ccm::test::ExpectSameFloatingAsStd(ccm::pow(base, exp), std::pow(base, exp), 1);
			ccm::test::ExpectSameFloatingAsStd(ccm::pow(base, -exp), std::pow(base, -exp), 1);
			// The clamp itself lives in the generic kernel, which builtin platforms bypass.
			ccm::test::ExpectSameFloatingAsStd(ccm::gen::pow_gen(base, exp), std::pow(base, exp), 1);
			ccm::test::ExpectSameFloatingAsStd(ccm::gen::pow_gen(base, -exp), std::pow(base, -exp), 1);
		}
	}
}

// [c.math]/1: bases of exactly 2 and 10 short-circuit to the dedicated exp2 and exp10 kernels.
// Integer exponents in (2, 24] are consumed by the small integer loop first, so non-integer and
// larger exponents are what actually reach the shortcuts.
TEST(CcmathPowerUlpTests, PowfBaseTwoAndTenShortcutAccuracy)
{
	constexpr std::array<float, 9> kExps = { 0.4F, 2.5F, -3.3F, 7.7F, -11.25F, 19.9F, 25.0F, 30.5F, -35.5F };

	for (float exp : kExps)
	{
		SCOPED_TRACE(exp);
		ccm::test::ExpectSameFloatingAsStd(ccm::powf(2.0F, exp), static_cast<float>(std::pow(2.0F, exp)), 1);
		ccm::test::ExpectSameFloatingAsStd(ccm::powf(10.0F, exp), static_cast<float>(std::pow(10.0F, exp)), 1);
	}
}

// [c.math]/1: regression for the base-10 shortcut. powf(10, n) routes integer exponents outside
// (2, 24] straight through exp10_float, which previously rounded y * log2(10) to float before
// exp2 and drifted up to 41 ULP (worst at n = 31, with n = -10 giving 7 ULP). Cover every integer
// power of 10 that stays finite and nonzero in float, both the small-int-loop band and the shortcut.
TEST(CcmathPowerUlpTests, PowfBaseTenIntegerExponentAccuracy)
{
	for (int n = -44; n <= 38; ++n)
	{
		const auto exp = static_cast<float>(n);
		SCOPED_TRACE(n);
		ccm::test::ExpectSameFloatingAsStd(ccm::powf(10.0F, exp), static_cast<float>(std::pow(10.0F, exp)), 1);
	}
}

// [c.math]/1: integer exponents in (2, 24] use an iterated double product when the base mantissa
// is narrow enough (extra_bits * iterations <= 25). Bracket that boundary from both sides with
// bases of known mantissa width so both the loop and the rejection into the main path are hit.
TEST(CcmathPowerUlpTests, PowfSmallIntegerLoopBoundary)
{
	// Mantissa widths: 1.5 -> 0 extra bits, 1.25 -> 1, 1 + 2^-8 -> 7, 1 + 2^-16 -> 15.
	constexpr std::array<float, 4> kBases = { 1.5F, 1.25F, 1.00390625F, 1.0000152587890625F };
	constexpr std::array<float, 6> kExps  = { 3.0F, 4.0F, 5.0F, 8.0F, 12.0F, 24.0F };

	for (float base : kBases)
	{
		for (float exp : kExps)
		{
			SCOPED_TRACE(base);
			SCOPED_TRACE(exp);
			ccm::test::ExpectSameFloatingAsStd(ccm::powf(base, exp), static_cast<float>(std::pow(base, exp)), 1);
		}
	}
}

// [c.math]/1: inputs whose phase-1 Ziv test fails, forcing the double-double fallback and its
// round-to-odd reconstruction. Mined by replaying the phase-1 kernel and kept only when the
// fallback path is taken on FMA targets. Expected bits are MPFR correctly rounded results.
TEST(CcmathPowerUlpTests, PowfZivFallbackRegressionCases)
{
	struct ZivCase
	{
		std::uint32_t base_bits;
		std::uint32_t exp_bits;
		std::uint32_t expected_bits;
	};

	constexpr std::array<ZivCase, 10> kCases = { {
		{ 0x42520505U, 0xc13230f3U, 0x1fa430e0U }, // powf(52.5049019, -11.1369505)
		{ 0x4206bdd9U, 0x3fc4629cU, 0x435c8acbU }, // powf(33.6853981, 1.53425932)
		{ 0x418d5c87U, 0xc164ebccU, 0x21d2e009U }, // powf(17.6701794, -14.3075676)
		{ 0x3ea17657U, 0x40375250U, 0x3d163830U }, // powf(0.315355986, 2.86439896)
		{ 0x3fa1f87aU, 0xc1a6e303U, 0x3bf180f6U }, // powf(1.2653954, -20.8608456)
		{ 0x3f36eccdU, 0xc36ee3bbU, 0x79646ae2U }, // powf(0.714550793, -238.889572)
		{ 0x3f7c682cU, 0xc2bee724U, 0x4076ad42U }, // powf(0.985964537, -95.4514465)
		{ 0x3f7f61dbU, 0xc33977a4U, 0x3fc85c71U }, // powf(0.997586906, -185.467346)
		{ 0x3faf9dddU, 0xc2d92ea4U, 0x26af067cU }, // powf(1.37200511, -108.591095)
		{ 0x3f6aeda8U, 0x4390d2efU, 0x2d89c122U }, // powf(0.917688847, 289.647919)
	} };

	for (const ZivCase & test_case : kCases)
	{
		const float base	 = ccm::support::bit_cast<float>(test_case.base_bits);
		const float exp		 = ccm::support::bit_cast<float>(test_case.exp_bits);
		const float expected = ccm::support::bit_cast<float>(test_case.expected_bits);
		SCOPED_TRACE(base);
		SCOPED_TRACE(exp);
		EXPECT_EQ(ccm::support::bit_cast<std::uint32_t>(ccm::gen::pow_gen(base, exp)), test_case.expected_bits)
			<< "powf(" << base << ", " << exp << ") expected " << expected;
		// The public entry point may route to the platform libm, which only promises faithful
		// rounding, so it gets the 1 ulp bound rather than the exact MPFR bits.
		ccm::test::ExpectSameFloatingAsStd(ccm::powf(base, exp), expected, 1);
	}
}

TEST(CcmathPowerUlpTests, CbrtDouble)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kCbrtDouble, ccm::cbrt<double>, static_cast<double (*)(double)>(std::cbrt));
}

TEST(CcmathPowerUlpTests, CbrtFloat)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kCbrtFloat, ccm::cbrt<float>, static_cast<float (*)(float)>(std::cbrt));
}

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
