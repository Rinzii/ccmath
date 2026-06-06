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
#include <limits>

#include "ccmath/ccmath.hpp"
#include "ccmath/internal/config/type_support.hpp"
#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
#include "utils/ulp_suite.hpp"

namespace
{
	template <typename T>
	void ExpectParityResult(T exponent, T expected)
	{
		const T actual = ccm::pow<T>(static_cast<T>(-1.0), exponent);
		ccm::test::ExpectSameFloatingAsStd(actual, expected, 0);
	}

	template <typename T>
	void ExpectNegativeBaseDomainError(T exponent)
	{
		const T actual = ccm::pow<T>(static_cast<T>(-2.0), exponent);
		const T expected = static_cast<T>(std::pow(static_cast<T>(-2.0), exponent));
		EXPECT_TRUE(std::isnan(actual));
		ccm::test::ExpectSameFloatingAsStd(actual, expected);
	}

	template <typename T>
	void ExpectGenParityResult(T exponent, T expected)
	{
		const T actual = ccm::gen::pow_gen(static_cast<T>(-1.0), exponent);
		ccm::test::ExpectSameFloatingAsStd(actual, expected, 0);
	}

	template <typename T>
	void ExpectGenNegativeBaseDomainError(T exponent)
	{
		const T actual = ccm::gen::pow_gen(static_cast<T>(-2.0), exponent);
		const T expected = static_cast<T>(std::pow(static_cast<T>(-2.0), exponent));
		EXPECT_TRUE(std::isnan(actual));
		ccm::test::ExpectSameFloatingAsStd(actual, expected);
	}
} // namespace

TEST(CcmathPowerIntegerBoundaryTests, FloatParityThresholdsAroundTwoTo23AndTwoTo24)
{
	ExpectParityResult(8388607.0F, -1.0F);
	ExpectParityResult(8388608.0F, 1.0F);
	ExpectParityResult(8388609.0F, -1.0F);
	ExpectParityResult(16777215.0F, -1.0F);
	ExpectParityResult(16777216.0F, 1.0F);
	ExpectParityResult(std::nextafter(16777216.0F, std::numeric_limits<float>::infinity()), 1.0F);
	ExpectParityResult(-8388607.0F, -1.0F);
	ExpectParityResult(-8388608.0F, 1.0F);
	ExpectParityResult(std::numeric_limits<float>::max(), std::pow(-1.0F, std::numeric_limits<float>::max()));
}

TEST(CcmathPowerIntegerBoundaryTests, FloatNonIntegerExponentsRemainDomainErrors)
{
	ExpectNegativeBaseDomainError(0.5F);
	ExpectNegativeBaseDomainError(1.5F);
	ExpectNegativeBaseDomainError(3.5F);
	ExpectNegativeBaseDomainError(-3.5F);
	ExpectNegativeBaseDomainError(std::nextafter(1.0F, 2.0F));
}

TEST(CcmathPowerIntegerBoundaryTests, DoubleParityThresholdsAroundTwoTo52AndTwoTo53)
{
	ExpectParityResult(0x1.ffffffffffffep51, -1.0);
	ExpectParityResult(0x1.0p52, 1.0);
	ExpectParityResult(0x1.0000000000001p52, -1.0);
	ExpectParityResult(0x1.fffffffffffffp52, -1.0);
	ExpectParityResult(0x1.0p53, 1.0);
	ExpectParityResult(std::nextafter(0x1.0p53, std::numeric_limits<double>::infinity()), 1.0);
	ExpectParityResult(-0x1.ffffffffffffep51, -1.0);
	ExpectParityResult(-0x1.0p52, 1.0);
	ExpectParityResult(std::numeric_limits<double>::max(), std::pow(-1.0, std::numeric_limits<double>::max()));
}

TEST(CcmathPowerIntegerBoundaryTests, DoubleNonIntegerExponentsRemainDomainErrors)
{
	ExpectNegativeBaseDomainError(0.5);
	ExpectNegativeBaseDomainError(1.5);
	ExpectNegativeBaseDomainError(3.5);
	ExpectNegativeBaseDomainError(-3.5);
	ExpectNegativeBaseDomainError(std::nextafter(1.0, 2.0));
}

TEST(CcmathPowerIntegerBoundaryTests, LongDoubleParityThresholdsRespectPlatformShape)
{
#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT64)
	ExpectParityResult(static_cast<long double>(0x1.fffffffffffffp52), static_cast<long double>(-1.0));
	ExpectParityResult(static_cast<long double>(0x1.0p53), static_cast<long double>(1.0));
	ExpectNegativeBaseDomainError(static_cast<long double>(1.5));
#elif defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
	const long double two_to_62 = 0x1.0p62L;
	ExpectGenParityResult(two_to_62 - 1.0L, -1.0L);
	ExpectGenParityResult(two_to_62, 1.0L);
	ExpectGenParityResult(two_to_62 + 1.0L, -1.0L);
	ExpectGenParityResult(two_to_62 + 2.0L, 1.0L);
	ExpectGenParityResult(0x1.0p63L - 1.0L, -1.0L);
	ExpectGenParityResult(0x1.0p63L, 1.0L);
	ExpectGenParityResult(0x1.0p64L, 1.0L);
	ExpectGenParityResult(0x1.0p65L, 1.0L);
	ExpectGenParityResult(std::nextafter(0x1.0p65L, std::numeric_limits<long double>::infinity()),
						  std::pow(-1.0L, std::nextafter(0x1.0p65L, std::numeric_limits<long double>::infinity())));
	ExpectGenNegativeBaseDomainError(1.5L);
#elif defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT128)
	const long double kLargeEven = std::nextafter(static_cast<long double>(0x1.0p64L), std::numeric_limits<long double>::infinity());
	ExpectParityResult(static_cast<long double>(3.0L), static_cast<long double>(-1.0L));
	ExpectParityResult(static_cast<long double>(4.0L), static_cast<long double>(1.0L));
	ExpectParityResult(kLargeEven, std::pow(-1.0L, kLargeEven));
	ExpectNegativeBaseDomainError(static_cast<long double>(1.5L));
#else
	GTEST_SKIP() << "unknown long double representation";
#endif
}
