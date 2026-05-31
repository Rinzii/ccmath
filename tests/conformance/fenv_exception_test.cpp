/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "utils/conformance_suite.hpp"

#include <gtest/gtest.h>

#include <ccmath/ccmath.hpp>

#include <cmath>
#include <limits>

namespace
{
	bool ShouldSkipMsvcFenvExceptions()
	{
#if defined(_MSC_VER) && !defined(__clang__)
		GTEST_SKIP() << "fenv exception flags are not reliable under MSVC";
		return true;
#else
		return false;
#endif
	}

	template <typename T>
	void consume(T value)
	{
		volatile T sink = value;
		(void)sink;
	}

	template <typename T>
	T runtime_value(T value)
	{
		volatile T sink = value;
		return sink;
	}
} // namespace

TEST(CcmathFenvExceptionTests, DomainErrorsRaiseInvalidLikeStd)
{
	if (ShouldSkipMsvcFenvExceptions()) { return; }
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::sqrt(runtime_value(-1.0))); }, [] { consume(std::sqrt(runtime_value(-1.0))); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::log(runtime_value(-1.0))); }, [] { consume(std::log(runtime_value(-1.0))); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::log1p(runtime_value(-2.0))); }, [] { consume(std::log1p(runtime_value(-2.0))); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd(
		[] { consume(ccm::fmod(runtime_value(1.0), runtime_value(0.0))); }, [] { consume(std::fmod(runtime_value(1.0), runtime_value(0.0))); }, FE_INVALID);
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
	ccm::test::ExpectFenvFlagsMatchStd(
		[] { consume(ccm::pow(runtime_value(-1.0), runtime_value(0.5))); }, [] { consume(std::pow(runtime_value(-1.0), runtime_value(0.5))); }, FE_INVALID);
}

TEST(CcmathFenvExceptionTests, DomainErrorsIndependentOfRoundingMode)
{
	if (ShouldSkipMsvcFenvExceptions()) { return; }
	ccm::test::ForEachRoundingModeOrSkip(
		[&](int mode)
		{
			ccm::test::ForceRoundingMode force(mode);
			ASSERT_TRUE(force);
			ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::sqrt(runtime_value(-1.0))); }, [] { consume(std::sqrt(runtime_value(-1.0))); }, FE_INVALID);
		});
}

TEST(CcmathFenvExceptionTests, PoleErrorsRaiseDivByZeroLikeStd)
{
	if (ShouldSkipMsvcFenvExceptions()) { return; }
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::log(runtime_value(0.0))); }, [] { consume(std::log(runtime_value(0.0))); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::log2(runtime_value(0.0))); }, [] { consume(std::log2(runtime_value(0.0))); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::log10(runtime_value(0.0))); }, [] { consume(std::log10(runtime_value(0.0))); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd(
		[] { consume(ccm::pow(runtime_value(0.0), runtime_value(-1.0))); }, [] { consume(std::pow(runtime_value(0.0), runtime_value(-1.0))); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::logb(runtime_value(0.0))); }, [] { consume(std::logb(runtime_value(0.0))); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::logb(runtime_value(-0.0))); }, [] { consume(std::logb(runtime_value(-0.0))); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::log1p(runtime_value(-1.0))); }, [] { consume(std::log1p(runtime_value(-1.0))); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::gamma(runtime_value(0.0))); }, [] { consume(std::tgamma(runtime_value(0.0))); }, FE_DIVBYZERO);
}

TEST(CcmathFenvExceptionTests, RangeErrorsRaiseOverflowOrUnderflowLikeStd)
{
	if (ShouldSkipMsvcFenvExceptions()) { return; }
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
