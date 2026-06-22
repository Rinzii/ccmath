/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
#include "utils/conformance_suite.hpp"
#include "utils/test_runtime.hpp"

#include <gtest/gtest.h>

#include <ccmath/ccmath.hpp>

#include <cmath>
#include <limits>

#if defined(_MSC_VER) && !defined(__clang__)
	#define CCMATH_SKIP_MSVC_FENV_EXCEPTIONS() GTEST_SKIP() << "fenv exception flags are not reliable under MSVC"
#else
	#define CCMATH_SKIP_MSVC_FENV_EXCEPTIONS() static_cast<void>(0)
#endif

namespace
{
	using ccm::test::runtime_value;

	template <typename T>
	void consume(T value)
	{ [[maybe_unused]] volatile T sink = value; }
} // namespace

TEST(CcmathFenvExceptionTests, DomainErrorsRaiseInvalidLikeStd)
{
	CCMATH_SKIP_MSVC_FENV_EXCEPTIONS();
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::sqrt(runtime_value(-1.0))); }, [] { consume(std::sqrt(runtime_value(-1.0))); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::log(runtime_value(-1.0))); }, [] { consume(std::log(runtime_value(-1.0))); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::log1p(runtime_value(-2.0))); }, [] { consume(std::log1p(runtime_value(-2.0))); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd(
		[] { consume(ccm::fmod(runtime_value(1.0), runtime_value(0.0))); }, [] { consume(std::fmod(runtime_value(1.0), runtime_value(0.0))); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::fmod(runtime_value(std::numeric_limits<double>::infinity()), runtime_value(1.0))); },
									   [] { consume(std::fmod(runtime_value(std::numeric_limits<double>::infinity()), runtime_value(1.0))); },
									   FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::remainder(runtime_value(1.0), runtime_value(0.0))); },
									   [] { consume(std::remainder(runtime_value(1.0), runtime_value(0.0))); },
									   FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::sin(runtime_value(std::numeric_limits<double>::infinity()))); },
									   [] { consume(std::sin(runtime_value(std::numeric_limits<double>::infinity()))); },
									   FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::ilogb(runtime_value(0.0))); }, [] { consume(std::ilogb(runtime_value(0.0))); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::ilogb(runtime_value(-0.0))); }, [] { consume(std::ilogb(runtime_value(-0.0))); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::ilogb(runtime_value(std::numeric_limits<double>::infinity()))); },
									   [] { consume(std::ilogb(runtime_value(std::numeric_limits<double>::infinity()))); },
									   FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::ilogb(runtime_value(std::numeric_limits<double>::quiet_NaN()))); },
									   [] { consume(std::ilogb(runtime_value(std::numeric_limits<double>::quiet_NaN()))); },
									   FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::ilogb(runtime_value(std::numeric_limits<float>::quiet_NaN()))); },
									   [] { consume(std::ilogb(runtime_value(std::numeric_limits<float>::quiet_NaN()))); },
									   FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::asin(runtime_value(2.0))); }, [] { consume(std::asin(runtime_value(2.0))); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::acos(runtime_value(2.0))); }, [] { consume(std::acos(runtime_value(2.0))); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::gamma(runtime_value(-1.0))); }, [] { consume(std::tgamma(runtime_value(-1.0))); }, FE_INVALID);
	// [c.math]/1: pow inherits the C library domain-error semantics, including FE_INVALID for negative bases with non-integral exponents.
	ccm::test::ExpectFenvFlagsMatchStd(
		[] { consume(ccm::pow(runtime_value(-1.0), runtime_value(0.5))); }, [] { consume(std::pow(runtime_value(-1.0), runtime_value(0.5))); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::gen::pow_gen(runtime_value(-1.0), runtime_value(0.5))); },
									   [] { consume(std::pow(runtime_value(-1.0), runtime_value(0.5))); },
									   FE_INVALID);
}

TEST(CcmathFenvExceptionTests, DomainErrorsIndependentOfRoundingMode)
{
	CCMATH_SKIP_MSVC_FENV_EXCEPTIONS();
	ccm::test::ForEachRoundingModeOrSkip(
		[&](int mode)
		{
			ccm::test::ForceRoundingMode force(mode);
			ASSERT_TRUE(force);
			ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::sqrt(runtime_value(-1.0))); }, [] { consume(std::sqrt(runtime_value(-1.0))); }, FE_INVALID);
			ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::pow(runtime_value(-1.0), runtime_value(0.5))); },
											   [] { consume(std::pow(runtime_value(-1.0), runtime_value(0.5))); },
											   FE_INVALID);
			ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::gen::pow_gen(runtime_value(-1.0), runtime_value(0.5))); },
											   [] { consume(std::pow(runtime_value(-1.0), runtime_value(0.5))); },
											   FE_INVALID);
		});
}

TEST(CcmathFenvExceptionTests, PoleErrorsRaiseDivByZeroLikeStd)
{
	CCMATH_SKIP_MSVC_FENV_EXCEPTIONS();
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::log(runtime_value(0.0))); }, [] { consume(std::log(runtime_value(0.0))); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::log2(runtime_value(0.0))); }, [] { consume(std::log2(runtime_value(0.0))); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::log10(runtime_value(0.0))); }, [] { consume(std::log10(runtime_value(0.0))); }, FE_DIVBYZERO);
	// [c.math]/1: pow inherits the C library pole-error semantics, including FE_DIVBYZERO for zero raised to a negative exponent.
	ccm::test::ExpectFenvFlagsMatchStd(
		[] { consume(ccm::pow(runtime_value(0.0), runtime_value(-1.0))); }, [] { consume(std::pow(runtime_value(0.0), runtime_value(-1.0))); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::gen::pow_gen(runtime_value(0.0), runtime_value(-1.0))); },
									   [] { consume(std::pow(runtime_value(0.0), runtime_value(-1.0))); },
									   FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::logb(runtime_value(0.0))); }, [] { consume(std::logb(runtime_value(0.0))); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::logb(runtime_value(-0.0))); }, [] { consume(std::logb(runtime_value(-0.0))); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::log1p(runtime_value(-1.0))); }, [] { consume(std::log1p(runtime_value(-1.0))); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::gamma(runtime_value(0.0))); }, [] { consume(std::tgamma(runtime_value(0.0))); }, FE_DIVBYZERO);
}

TEST(CcmathFenvExceptionTests, PoleErrorsIndependentOfRoundingMode)
{
	CCMATH_SKIP_MSVC_FENV_EXCEPTIONS();
	ccm::test::ForEachRoundingModeOrSkip(
		[&](int mode)
		{
			ccm::test::ForceRoundingMode force(mode);
			ASSERT_TRUE(force);
			ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::pow(runtime_value(0.0), runtime_value(-1.0))); },
											   [] { consume(std::pow(runtime_value(0.0), runtime_value(-1.0))); },
											   FE_DIVBYZERO);
			ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::gen::pow_gen(runtime_value(0.0), runtime_value(-1.0))); },
											   [] { consume(std::pow(runtime_value(0.0), runtime_value(-1.0))); },
											   FE_DIVBYZERO);
		});
}

TEST(CcmathFenvExceptionTests, RangeErrorsRaiseOverflowOrUnderflowLikeStd)
{
	CCMATH_SKIP_MSVC_FENV_EXCEPTIONS();
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::exp(runtime_value(std::numeric_limits<double>::max()))); },
									   [] { consume(std::exp(runtime_value(std::numeric_limits<double>::max()))); },
									   FE_OVERFLOW);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::exp(runtime_value(-std::numeric_limits<double>::max()))); },
									   [] { consume(std::exp(runtime_value(-std::numeric_limits<double>::max()))); },
									   FE_UNDERFLOW);
	ccm::test::ExpectFenvFlagsMatchStd(
		[] { consume(ccm::ldexp(runtime_value(std::numeric_limits<double>::max()), runtime_value(std::numeric_limits<int>::max()))); },
		[] { consume(std::ldexp(runtime_value(std::numeric_limits<double>::max()), runtime_value(std::numeric_limits<int>::max()))); },
		FE_OVERFLOW);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::ldexp(runtime_value(std::numeric_limits<double>::denorm_min()), runtime_value(-1))); },
									   [] { consume(std::ldexp(runtime_value(std::numeric_limits<double>::denorm_min()), runtime_value(-1))); },
									   FE_UNDERFLOW);
}

TEST(CcmathFenvExceptionTests, ExactNonExceptionalCallsDoNotRaiseFlags)
{
	ccm::test::ExpectFenvFlags([] { consume(ccm::fabs(-1.0)); }, 0);
	ccm::test::ExpectFenvFlags([] { consume(ccm::copysign(1.0, -2.0)); }, 0);
	ccm::test::ExpectFenvFlags([] { consume(ccm::nearbyint(2.0)); }, 0);
	ccm::test::ExpectFenvFlags([] { consume(ccm::sqrt(4.0)); }, 0);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::rint(std::numeric_limits<double>::quiet_NaN())); },
									   [] { consume(std::rint(std::numeric_limits<double>::quiet_NaN())); },
									   FE_ALL_EXCEPT);
}

#undef CCMATH_SKIP_MSVC_FENV_EXCEPTIONS
