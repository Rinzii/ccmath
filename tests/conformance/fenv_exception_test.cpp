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

#include "utils/conformance_suite.hpp"

namespace
{
	template <typename T>
	void consume(T value)
	{
		volatile T sink = value;
		(void)sink;
	}
} // namespace

TEST(CcmathFenvExceptionTests, DomainErrorsRaiseInvalidLikeStd)
{
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::sqrt(-1.0)); }, [] { consume(std::sqrt(-1.0)); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::log(-1.0)); }, [] { consume(std::log(-1.0)); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::log1p(-2.0)); }, [] { consume(std::log1p(-2.0)); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::fmod(1.0, 0.0)); }, [] { consume(std::fmod(1.0, 0.0)); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::remainder(1.0, 0.0)); }, [] { consume(std::remainder(1.0, 0.0)); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::sin(std::numeric_limits<double>::infinity())); },
									   [] { consume(std::sin(std::numeric_limits<double>::infinity())); },
									   FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::ilogb(0.0)); }, [] { consume(std::ilogb(0.0)); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::ilogb(-0.0)); }, [] { consume(std::ilogb(-0.0)); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::ilogb(std::numeric_limits<double>::infinity())); },
									   [] { consume(std::ilogb(std::numeric_limits<double>::infinity())); },
									   FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::ilogb(std::numeric_limits<double>::quiet_NaN())); },
									   [] { consume(std::ilogb(std::numeric_limits<double>::quiet_NaN())); },
									   FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::ilogb(std::numeric_limits<float>::quiet_NaN())); },
									   [] { consume(std::ilogb(std::numeric_limits<float>::quiet_NaN())); },
									   FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::asin(2.0)); }, [] { consume(std::asin(2.0)); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::acos(2.0)); }, [] { consume(std::acos(2.0)); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::gamma(-1.0)); }, [] { consume(std::tgamma(-1.0)); }, FE_INVALID);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::pow(-1.0, 0.5)); }, [] { consume(std::pow(-1.0, 0.5)); }, FE_INVALID);
}

TEST(CcmathFenvExceptionTests, DomainErrorsIndependentOfRoundingMode)
{
	ccm::test::ForEachRoundingModeOrSkip([&](int mode) {
		ccm::test::ForceRoundingMode force(mode);
		ASSERT_TRUE(force);
		ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::sqrt(-1.0)); }, [] { consume(std::sqrt(-1.0)); }, FE_INVALID);
	});
}

TEST(CcmathFenvExceptionTests, PoleErrorsRaiseDivByZeroLikeStd)
{
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::log(0.0)); }, [] { consume(std::log(0.0)); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::log2(0.0)); }, [] { consume(std::log2(0.0)); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::log10(0.0)); }, [] { consume(std::log10(0.0)); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::pow(0.0, -1.0)); }, [] { consume(std::pow(0.0, -1.0)); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::logb(0.0)); }, [] { consume(std::logb(0.0)); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::logb(-0.0)); }, [] { consume(std::logb(-0.0)); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::log1p(-1.0)); }, [] { consume(std::log1p(-1.0)); }, FE_DIVBYZERO);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::gamma(0.0)); }, [] { consume(std::tgamma(0.0)); }, FE_DIVBYZERO);
}

TEST(CcmathFenvExceptionTests, RangeErrorsRaiseOverflowOrUnderflowLikeStd)
{
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::exp(std::numeric_limits<double>::max())); },
									   [] { consume(std::exp(std::numeric_limits<double>::max())); },
									   FE_OVERFLOW);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::exp(-std::numeric_limits<double>::max())); },
									   [] { consume(std::exp(-std::numeric_limits<double>::max())); },
									   FE_UNDERFLOW);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::ldexp(std::numeric_limits<double>::max(), std::numeric_limits<int>::max())); },
									   [] { consume(std::ldexp(std::numeric_limits<double>::max(), std::numeric_limits<int>::max())); },
									   FE_OVERFLOW);
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::ldexp(std::numeric_limits<double>::denorm_min(), -1)); },
									   [] { consume(std::ldexp(std::numeric_limits<double>::denorm_min(), -1)); },
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
