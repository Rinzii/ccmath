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

#include "ccmath/ccmath.hpp"
#include "utils/ulp_suite.hpp"
#include "utils/worst_case_samples.hpp"

namespace
{
	template <typename T, typename CcmFn, typename StdFn, std::size_t N>
	void ExpectWorstCaseUnaryOver(const std::array<T, N> & inputs, CcmFn ccm_fn, StdFn std_fn)
	{
		for (T input : inputs)
		{
			SCOPED_TRACE(input);
			ccm::test::ExpectUlpUnaryVsStd(input, ccm_fn, std_fn);
		}
	}

	template <typename T, typename CcmFn, typename StdFn, std::size_t N>
	void ExpectWorstCaseBinaryOver(const std::array<T, N> & bases, const std::array<T, N> & exponents, CcmFn ccm_fn, StdFn std_fn)
	{
		for (std::size_t i = 0; i < N; ++i)
		{
			SCOPED_TRACE(bases[i]);
			SCOPED_TRACE(exponents[i]);
			ccm::test::ExpectUlpBinaryVsStd(bases[i], exponents[i], ccm_fn, std_fn);
		}
	}

	template <typename T, typename CcmFn, typename StdFn, std::size_t N>
	void ExpectWorstCaseBinaryPairsOver(const std::array<ccm::test::worst_case::PowCase<T>, N> & cases, CcmFn ccm_fn, StdFn std_fn)
	{
		for (const auto & test_case : cases)
		{
			SCOPED_TRACE(test_case.base);
			SCOPED_TRACE(test_case.exponent);
			SCOPED_TRACE(test_case.provenance);
			ccm::test::ExpectSameFloatingAsStd(ccm_fn(test_case.base, test_case.exponent), std_fn(test_case.base, test_case.exponent));
		}
	}
} // namespace

TEST(CcmathWorstCaseUlpTests, SinFloatExceptional)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kSinFloatExceptional, ccm::sin<float>, static_cast<float (*)(float)>(std::sin));
}

TEST(CcmathWorstCaseUlpTests, SinFloatLargeReduction)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kSinFloatLargeReduction, ccm::sin<float>, static_cast<float (*)(float)>(std::sin));
}

TEST(CcmathWorstCaseUlpTests, CosFloatExceptional)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kCosFloatExceptional, ccm::cos<float>, static_cast<float (*)(float)>(std::cos));
}

TEST(CcmathWorstCaseUlpTests, SinDoubleHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kSinDoubleHard, ccm::sin<double>, static_cast<double (*)(double)>(std::sin));
}

TEST(CcmathWorstCaseUlpTests, CosDoubleHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kSinDoubleHard, ccm::cos<double>, static_cast<double (*)(double)>(std::cos));
}

TEST(CcmathWorstCaseUlpTests, Expm1FloatHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kExpm1FloatHard, ccm::expm1<float>, static_cast<float (*)(float)>(std::expm1));
}

TEST(CcmathWorstCaseUlpTests, Expm1DoubleHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kExpm1DoubleHard, ccm::expm1<double>, static_cast<double (*)(double)>(std::expm1));
}

TEST(CcmathWorstCaseUlpTests, ExpFloatHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kExpFloatHard, ccm::exp<float>, static_cast<float (*)(float)>(std::exp));
}

TEST(CcmathWorstCaseUlpTests, ExpDoubleHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kExpDoubleHard, ccm::exp<double>, static_cast<double (*)(double)>(std::exp));
}

TEST(CcmathWorstCaseUlpTests, Log1pFloatHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kLog1pFloatHard, ccm::log1p<float>, static_cast<float (*)(float)>(std::log1p));
}

TEST(CcmathWorstCaseUlpTests, Log1pDoubleHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kLog1pDoubleHard, ccm::log1p<double>, static_cast<double (*)(double)>(std::log1p));
}

TEST(CcmathWorstCaseUlpTests, LogFloatHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kLogFloatHard, ccm::log<float>, static_cast<float (*)(float)>(std::log));
}

TEST(CcmathWorstCaseUlpTests, LogDoubleHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kLogDoubleHard, ccm::log<double>, static_cast<double (*)(double)>(std::log));
}

TEST(CcmathWorstCaseUlpTests, Log2DoubleHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kLogDoubleHard, ccm::log2<double>, static_cast<double (*)(double)>(std::log2));
}

TEST(CcmathWorstCaseUlpTests, Log10DoubleHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kLogDoubleHard, ccm::log10<double>, static_cast<double (*)(double)>(std::log10));
}

TEST(CcmathWorstCaseUlpTests, Exp2FloatHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kExp2FloatHard, ccm::exp2<float>, static_cast<float (*)(float)>(std::exp2));
}

TEST(CcmathWorstCaseUlpTests, Exp2DoubleHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kExp2DoubleHard, ccm::exp2<double>, static_cast<double (*)(double)>(std::exp2));
}

TEST(CcmathWorstCaseUlpTests, SqrtFloatHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kSqrtFloatHard, ccm::sqrt<float>, static_cast<float (*)(float)>(std::sqrt));
}

TEST(CcmathWorstCaseUlpTests, SqrtDoubleHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kSqrtDoubleHard, ccm::sqrt<double>, static_cast<double (*)(double)>(std::sqrt));
}

// [c.math]/1: validates that the double overload tracks the C library semantics even on the known hardest pow inputs in this suite.
TEST(CcmathWorstCaseUlpTests, PowDoubleHard)
{
	ExpectWorstCaseBinaryPairsOver(
		ccm::test::worst_case::kPowDoubleHard, ccm::pow<double>, static_cast<double (*)(double, double)>(std::pow));
}

TEST(CcmathWorstCaseUlpTests, PowFloatHard)
{
	ExpectWorstCaseBinaryPairsOver(
		ccm::test::worst_case::kPowFloatHard, ccm::powf, static_cast<float (*)(float, float)>(std::pow));
}

TEST(CcmathWorstCaseUlpTests, PowLongDoubleHard)
{
	constexpr std::array<ccm::test::worst_case::PowCase<long double>, 3> kPowLongDoubleHard = { {
		{ 0x1.0p-50L, 0x1.0p+50L, "long-double-shaped alias of the legacy tiny-base / huge-exponent case" },
		{ -1.0L, 3.0L, "long-double-shaped odd-integer sign regression anchor" },
		{ -1.0L, 1.5L, "long-double-shaped non-integer domain regression anchor" },
	} };

	ExpectWorstCaseBinaryPairsOver(
		kPowLongDoubleHard, ccm::powl, static_cast<long double (*)(long double, long double)>(std::pow));
}

TEST(CcmathWorstCaseUlpTests, AsinDoubleHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kAsinDoubleHard, ccm::asin<double>, static_cast<double (*)(double)>(std::asin));
}

TEST(CcmathWorstCaseUlpTests, AsinFloatHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kAsinFloatHard, ccm::asin<float>, static_cast<float (*)(float)>(std::asin));
}

TEST(CcmathWorstCaseUlpTests, AtanDoubleHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kAtanDoubleHard, ccm::atan<double>, static_cast<double (*)(double)>(std::atan));
}

TEST(CcmathWorstCaseUlpTests, CbrtDoubleHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kCbrtDoubleHard, ccm::cbrt<double>, static_cast<double (*)(double)>(std::cbrt));
}

TEST(CcmathWorstCaseUlpTests, CbrtFloatHard)
{
	ExpectWorstCaseUnaryOver(ccm::test::worst_case::kCbrtFloatHard, ccm::cbrt<float>, static_cast<float (*)(float)>(std::cbrt));
}

TEST(CcmathWorstCaseUlpTests, GammaDoubleHard)
{
	for (double x : ccm::test::worst_case::kGammaDoubleHard)
	{
		SCOPED_TRACE(x);
		if (x < 0.0 && x == std::trunc(x)) { continue; }
		ccm::test::ExpectUlpUnaryVsStd(x, ccm::gamma<double>, static_cast<double (*)(double)>(std::tgamma));
	}
}
