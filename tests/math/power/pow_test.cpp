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
#include <cstdint>
#include <limits>
#include <type_traits>

#include "ccmath/ccmath.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "utils/conformance_suite.hpp"
#include "utils/ulp_suite.hpp"
#include "utils/worst_case_samples.hpp"

#if defined(_MSC_VER) && !defined(__clang__)
	#define CCMATH_SKIP_MSVC_FENV() GTEST_SKIP() << "fenv exception flags are not reliable under MSVC"
#else
	#define CCMATH_SKIP_MSVC_FENV() static_cast<void>(0)
#endif

namespace
{
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

// [cmath.syn]: Synopsis-level overloads are constexpr entry points.

TEST(CcmathPowerTests, PowFloatingOverloadsAreConstexpr)
{
	static_assert(ccm::pow(2.0, 3.0) == 8.0, "ccm::pow must be usable in constant evaluation");
	static_assert(ccm::powf(2.0F, 3.0F) == 8.0F, "ccm::powf must be usable in constant evaluation");
	static_assert(ccm::powl(2.0L, 3.0L) == 8.0L, "ccm::powl must be usable in constant evaluation");
}

// [cmath.syn]: Named C compatibility entry points have standard signatures.

TEST(CcmathPowerTests, PowNamedEntryPointsHaveStandardSignatures)
{
	static_assert(std::is_same_v<decltype(&ccm::powf), float (*)(float, float)>);
	static_assert(std::is_same_v<decltype(&ccm::powl), long double (*)(long double, long double)>);
}

// [c.math]/3: Integer arguments promote to double.

TEST(CcmathPowerTests, PowAdditionalOverloadsTreatIntegersAsDouble)
{
	static_assert(std::is_same_v<decltype(ccm::pow(2, 3)), double>);
	static_assert(std::is_same_v<decltype(ccm::pow(2.0F, 3)), double>);
	static_assert(std::is_same_v<decltype(ccm::pow(2, 3.0F)), double>);

	EXPECT_DOUBLE_EQ(ccm::pow(2, 3), static_cast<double (*)(double, double)>(std::pow)(2.0, 3.0));
	EXPECT_DOUBLE_EQ(ccm::pow(2.0F, 3), static_cast<double (*)(double, double)>(std::pow)(2.0, 3.0));
	EXPECT_DOUBLE_EQ(ccm::pow(2, 3.0F), static_cast<double (*)(double, double)>(std::pow)(2.0, 3.0));
}

// [c.math]/3: Mixed-rank arguments promote to the greatest floating rank.

TEST(CcmathPowerTests, PowAdditionalOverloadsChooseGreatestFloatingRank)
{
	static_assert(std::is_same_v<decltype(ccm::pow(2.0F, 3.0)), double>);
	static_assert(std::is_same_v<decltype(ccm::pow(2.0, 3.0F)), double>);
	static_assert(std::is_same_v<decltype(ccm::pow(2.0L, 3.0F)), long double>);
	static_assert(std::is_same_v<decltype(ccm::pow(2.0L, 3)), long double>);

	EXPECT_DOUBLE_EQ(ccm::pow(2.0F, 3.0), static_cast<double (*)(double, double)>(std::pow)(2.0, 3.0));
	EXPECT_DOUBLE_EQ(ccm::pow(2.0, 3.0F), static_cast<double (*)(double, double)>(std::pow)(2.0, 3.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(2.0L, 3.0F), std::pow(2.0L, 3.0L));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(2.0L, 3), std::pow(2.0L, 3.0L));
}

// [cmath.syn] and [c.math]/1: Named entry points match std for representative inputs.

TEST(CcmathPowerTests, PowNamedEntryPointsMatchStd)
{
	ccm::test::ExpectUlpBinaryVsStd(2.0, 10.0, ccm::pow<double>, static_cast<double (*)(double, double)>(std::pow));
	ccm::test::ExpectUlpBinaryVsStd(14.0, -0.5, ccm::pow<double>, static_cast<double (*)(double, double)>(std::pow));
	ccm::test::ExpectUlpBinaryVsStd(2.0F, 10.0F, ccm::powf, static_cast<float (*)(float, float)>(std::pow));
	ccm::test::ExpectSameFloatingAsStd(ccm::powl(2.0L, 10.0L), std::pow(2.0L, 10.0L));
}

// C17 F.10.4.4 / C++ [cmath.syn]: pow special cases.
// C++ defers to C17 7.12.7.4 and Annex F. Each test below cites its clause.

// F.10.4.4(1): pow(+/-0, y) = +/-inf and FE_DIVBYZERO for y an odd integer < 0
TEST(CcmathPowerTests, PowZeroBaseNegativeOddExponent)
{
	// pow(+0, -3) = +inf
	EXPECT_EQ(ccm::pow(0.0, -3.0), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::powf(0.0F, -3.0F), std::numeric_limits<float>::infinity());

	// pow(-0, -3) = -inf
	EXPECT_EQ(ccm::pow(-0.0, -3.0), -std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::powf(-0.0F, -3.0F), -std::numeric_limits<float>::infinity());
}

// F.10.4.4(2): pow(+/-0, y) = +inf and FE_DIVBYZERO for y finite, non-integer, < 0
TEST(CcmathPowerTests, PowZeroBaseNegativeNonIntegerExponent)
{
	EXPECT_EQ(ccm::pow(0.0, -2.5), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::pow(-0.0, -2.5), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::powf(0.0F, -2.5F), std::numeric_limits<float>::infinity());
}

// F.10.4.4(3): pow(+/-0, y) = +inf and FE_DIVBYZERO for y even integer < 0
TEST(CcmathPowerTests, PowZeroBaseNegativeEvenExponent)
{
	EXPECT_EQ(ccm::pow(0.0, -2.0), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::pow(-0.0, -2.0), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::powf(0.0F, -2.0F), std::numeric_limits<float>::infinity());
	EXPECT_EQ(ccm::powf(-0.0F, -2.0F), std::numeric_limits<float>::infinity());
}

// F.10.4.4(4): pow(+/-0, y) = +/-0 for y an odd integer > 0
TEST(CcmathPowerTests, PowZeroBasePositiveOddExponent)
{
	ccm::test::ExpectFpEq(ccm::pow(0.0, 3.0), 0.0);
	ccm::test::ExpectFpEq(ccm::pow(-0.0, 3.0), -0.0);
	ccm::test::ExpectFpEq(ccm::powf(0.0F, 3.0F), 0.0F);
	ccm::test::ExpectFpEq(ccm::powf(-0.0F, 3.0F), -0.0F);

	EXPECT_FALSE(std::signbit(ccm::pow(0.0, 3.0)));
	EXPECT_TRUE(std::signbit(ccm::pow(-0.0, 3.0)));
	EXPECT_FALSE(std::signbit(ccm::powf(0.0F, 3.0F)));
	EXPECT_TRUE(std::signbit(ccm::powf(-0.0F, 3.0F)));
}

// F.10.4.4(5): pow(+/-0, y) = +0 for y > 0 and not an odd integer
TEST(CcmathPowerTests, PowZeroBasePositiveNonOddExponent)
{
	ccm::test::ExpectFpEq(ccm::pow(0.0, 2.0), 0.0);
	ccm::test::ExpectFpEq(ccm::pow(-0.0, 2.0), 0.0);
	ccm::test::ExpectFpEq(ccm::pow(0.0, 2.5), 0.0);
	ccm::test::ExpectFpEq(ccm::pow(-0.0, 4.0), 0.0);
	ccm::test::ExpectFpEq(ccm::powf(0.0F, 2.0F), 0.0F);
	ccm::test::ExpectFpEq(ccm::powf(-0.0F, 2.0F), 0.0F);

	EXPECT_FALSE(std::signbit(ccm::pow(0.0, 2.0)));
	EXPECT_FALSE(std::signbit(ccm::pow(-0.0, 2.0)));
	EXPECT_FALSE(std::signbit(ccm::pow(-0.0, 4.0)));
}

// F.10.4.4(6): pow(-1, +/-inf) = 1
TEST(CcmathPowerTests, PowNegOneInfExponent)
{
	EXPECT_EQ(ccm::pow(-1.0, std::numeric_limits<double>::infinity()), 1.0);
	EXPECT_EQ(ccm::pow(-1.0, -std::numeric_limits<double>::infinity()), 1.0);
	EXPECT_EQ(ccm::powf(-1.0F, std::numeric_limits<float>::infinity()), 1.0F);
	EXPECT_EQ(ccm::powf(-1.0F, -std::numeric_limits<float>::infinity()), 1.0F);
}

// F.10.4.4(7): pow(+1, y) = 1 for any y, even NaN
TEST(CcmathPowerTests, PowOneBaseAnyExponent)
{
	EXPECT_EQ(ccm::pow(1.0, 0.0), 1.0);
	EXPECT_EQ(ccm::pow(1.0, -0.0), 1.0);
	EXPECT_EQ(ccm::pow(1.0, 42.0), 1.0);
	EXPECT_EQ(ccm::pow(1.0, -42.0), 1.0);
	EXPECT_EQ(ccm::pow(1.0, std::numeric_limits<double>::infinity()), 1.0);
	EXPECT_EQ(ccm::pow(1.0, -std::numeric_limits<double>::infinity()), 1.0);
	EXPECT_EQ(ccm::pow(1.0, std::numeric_limits<double>::quiet_NaN()), 1.0);

	EXPECT_EQ(ccm::powf(1.0F, std::numeric_limits<float>::quiet_NaN()), 1.0F);
	EXPECT_EQ(ccm::powf(1.0F, std::numeric_limits<float>::infinity()), 1.0F);
}

// F.10.4.4(8): pow(x, +/-0) = 1 for any x, even NaN
TEST(CcmathPowerTests, PowAnyBaseZeroExponent)
{
	EXPECT_EQ(ccm::pow(0.0, 0.0), 1.0);
	EXPECT_EQ(ccm::pow(-0.0, 0.0), 1.0);
	EXPECT_EQ(ccm::pow(2.0, 0.0), 1.0);
	EXPECT_EQ(ccm::pow(-2.0, 0.0), 1.0);
	EXPECT_EQ(ccm::pow(std::numeric_limits<double>::infinity(), 0.0), 1.0);
	EXPECT_EQ(ccm::pow(-std::numeric_limits<double>::infinity(), 0.0), 1.0);
	EXPECT_EQ(ccm::pow(std::numeric_limits<double>::quiet_NaN(), 0.0), 1.0);

	EXPECT_EQ(ccm::pow(0.0, -0.0), 1.0);
	EXPECT_EQ(ccm::pow(2.0, -0.0), 1.0);
	EXPECT_EQ(ccm::pow(std::numeric_limits<double>::quiet_NaN(), -0.0), 1.0);

	EXPECT_EQ(ccm::powf(0.0F, 0.0F), 1.0F);
	EXPECT_EQ(ccm::powf(-0.0F, 0.0F), 1.0F);
	EXPECT_EQ(ccm::powf(std::numeric_limits<float>::quiet_NaN(), 0.0F), 1.0F);
	EXPECT_EQ(ccm::powf(std::numeric_limits<float>::quiet_NaN(), -0.0F), 1.0F);
}

// F.10.4.4(9): pow(x, y) = NaN and FE_INVALID for finite x < 0 and finite non-integer y
TEST(CcmathPowerTests, PowNegativeBaseNonIntegerExponent)
{
	EXPECT_TRUE(std::isnan(ccm::pow(-2.0, 0.5)));
	EXPECT_TRUE(std::isnan(ccm::pow(-4.0, 1.5)));
	EXPECT_TRUE(std::isnan(ccm::pow(-1.0, 0.5)));

	EXPECT_TRUE(std::isnan(ccm::powf(-2.0F, 0.5F)));
	EXPECT_TRUE(std::isnan(ccm::powf(-4.0F, 1.5F)));
}

// F.10.4.4: pow(x, y) is well-defined for finite x < 0 and integer y
TEST(CcmathPowerTests, PowNegativeBaseIntegerExponent)
{
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(-2.0, 2.0), std::pow(-2.0, 2.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(-2.0, 3.0), std::pow(-2.0, 3.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(-3.0, 4.0), std::pow(-3.0, 4.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(-3.0, 5.0), std::pow(-3.0, 5.0));

	ccm::test::ExpectSameFloatingAsStd(ccm::powf(-2.0F, 2.0F), std::pow(-2.0F, 2.0F));
	ccm::test::ExpectSameFloatingAsStd(ccm::powf(-2.0F, 3.0F), std::pow(-2.0F, 3.0F));
}

// F.10.4.4(10,11): pow(|x| < 1, -inf) = +inf. pow(|x| > 1, -inf) = +0
TEST(CcmathPowerTests, PowFiniteBaseNegativeInfExponent)
{
	const double inf = std::numeric_limits<double>::infinity();
	const float inff = std::numeric_limits<float>::infinity();

	EXPECT_EQ(ccm::pow(0.5, -inf), inf);
	EXPECT_EQ(ccm::pow(-0.5, -inf), inf);
	EXPECT_EQ(ccm::powf(0.5F, -inff), inff);

	ccm::test::ExpectFpEq(ccm::pow(2.0, -inf), 0.0);
	EXPECT_FALSE(std::signbit(ccm::pow(2.0, -inf)));
	ccm::test::ExpectFpEq(ccm::pow(-2.0, -inf), 0.0);
	EXPECT_FALSE(std::signbit(ccm::pow(-2.0, -inf)));
	ccm::test::ExpectFpEq(ccm::powf(2.0F, -inff), 0.0F);
}

// F.10.4.4(12,13): pow(|x| < 1, +inf) = +0. pow(|x| > 1, +inf) = +inf
TEST(CcmathPowerTests, PowFiniteBasePositiveInfExponent)
{
	const double inf = std::numeric_limits<double>::infinity();
	const float inff = std::numeric_limits<float>::infinity();

	ccm::test::ExpectFpEq(ccm::pow(0.5, inf), 0.0);
	EXPECT_FALSE(std::signbit(ccm::pow(0.5, inf)));
	ccm::test::ExpectFpEq(ccm::pow(-0.5, inf), 0.0);
	ccm::test::ExpectFpEq(ccm::powf(0.5F, inff), 0.0F);

	EXPECT_EQ(ccm::pow(2.0, inf), inf);
	EXPECT_EQ(ccm::pow(-2.0, inf), inf);
	EXPECT_EQ(ccm::powf(2.0F, inff), inff);
}

// F.10.4.4(14): pow(-inf, y) = -0 for y an odd integer < 0
TEST(CcmathPowerTests, PowNegInfBaseNegativeOddExponent)
{
	ccm::test::ExpectFpEq(ccm::pow(-std::numeric_limits<double>::infinity(), -3.0), -0.0);
	EXPECT_TRUE(std::signbit(ccm::pow(-std::numeric_limits<double>::infinity(), -3.0)));
	ccm::test::ExpectFpEq(ccm::powf(-std::numeric_limits<float>::infinity(), -3.0F), -0.0F);
	EXPECT_TRUE(std::signbit(ccm::powf(-std::numeric_limits<float>::infinity(), -3.0F)));
}

// F.10.4.4(15): pow(-inf, y) = +0 for y < 0 and not an odd integer
TEST(CcmathPowerTests, PowNegInfBaseNegativeNonOddExponent)
{
	ccm::test::ExpectFpEq(ccm::pow(-std::numeric_limits<double>::infinity(), -2.0), 0.0);
	EXPECT_FALSE(std::signbit(ccm::pow(-std::numeric_limits<double>::infinity(), -2.0)));
	ccm::test::ExpectFpEq(ccm::pow(-std::numeric_limits<double>::infinity(), -4.0), 0.0);
	ccm::test::ExpectFpEq(ccm::powf(-std::numeric_limits<float>::infinity(), -2.0F), 0.0F);
}

// F.10.4.4(16): pow(-inf, y) = -inf for y an odd integer > 0
TEST(CcmathPowerTests, PowNegInfBasePositiveOddExponent)
{
	EXPECT_EQ(ccm::pow(-std::numeric_limits<double>::infinity(), 3.0), -std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::pow(-std::numeric_limits<double>::infinity(), 5.0), -std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::powf(-std::numeric_limits<float>::infinity(), 3.0F), -std::numeric_limits<float>::infinity());
}

// F.10.4.4(17): pow(-inf, y) = +inf for y > 0 and not an odd integer
TEST(CcmathPowerTests, PowNegInfBasePositiveNonOddExponent)
{
	EXPECT_EQ(ccm::pow(-std::numeric_limits<double>::infinity(), 2.0), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::pow(-std::numeric_limits<double>::infinity(), 4.0), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::powf(-std::numeric_limits<float>::infinity(), 2.0F), std::numeric_limits<float>::infinity());
	EXPECT_EQ(ccm::powf(-std::numeric_limits<float>::infinity(), 4.0F), std::numeric_limits<float>::infinity());

	// Non-integer exponents are not odd integers, so the same rule applies.
	EXPECT_EQ(ccm::pow(-std::numeric_limits<double>::infinity(), 0.5), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::pow(-std::numeric_limits<double>::infinity(), 2.5), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::powf(-std::numeric_limits<float>::infinity(), 0.5F), std::numeric_limits<float>::infinity());
}

// F.10.4.4(18): pow(+inf, y) = +0 for y < 0
TEST(CcmathPowerTests, PowPosInfBaseNegativeExponent)
{
	ccm::test::ExpectFpEq(ccm::pow(std::numeric_limits<double>::infinity(), -1.0), 0.0);
	EXPECT_FALSE(std::signbit(ccm::pow(std::numeric_limits<double>::infinity(), -1.0)));
	ccm::test::ExpectFpEq(ccm::pow(std::numeric_limits<double>::infinity(), -2.0), 0.0);
	ccm::test::ExpectFpEq(ccm::pow(std::numeric_limits<double>::infinity(), -100.5), 0.0);
	ccm::test::ExpectFpEq(ccm::powf(std::numeric_limits<float>::infinity(), -1.0F), 0.0F);
}

// F.10.4.4(19): pow(+inf, y) = +inf for y > 0
TEST(CcmathPowerTests, PowPosInfBasePositiveExponent)
{
	EXPECT_EQ(ccm::pow(std::numeric_limits<double>::infinity(), 1.0), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::pow(std::numeric_limits<double>::infinity(), 2.0), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::pow(std::numeric_limits<double>::infinity(), 100.5), std::numeric_limits<double>::infinity());
	EXPECT_EQ(ccm::powf(std::numeric_limits<float>::infinity(), 2.0F), std::numeric_limits<float>::infinity());
}

// C17 7.12.7.4: pow(x, NaN) = NaN for x != 1. pow(NaN, y) = NaN for y != 0
TEST(CcmathPowerTests, PowNaNPropagation)
{
	const double nan = std::numeric_limits<double>::quiet_NaN();
	const float nanf = std::numeric_limits<float>::quiet_NaN();

	EXPECT_TRUE(std::isnan(ccm::pow(nan, 1.0)));
	EXPECT_TRUE(std::isnan(ccm::pow(nan, 2.0)));
	EXPECT_TRUE(std::isnan(ccm::pow(nan, -1.0)));
	EXPECT_TRUE(std::isnan(ccm::pow(nan, nan)));

	EXPECT_TRUE(std::isnan(ccm::pow(2.0, nan)));
	EXPECT_TRUE(std::isnan(ccm::pow(-2.0, nan)));
	EXPECT_TRUE(std::isnan(ccm::pow(0.5, nan)));

	EXPECT_TRUE(std::isnan(ccm::powf(nanf, 1.0F)));
	EXPECT_TRUE(std::isnan(ccm::powf(2.0F, nanf)));

	// pow(NaN, 0) = 1 (already covered above, but verify NaN doesn't propagate)
	EXPECT_EQ(ccm::pow(nan, 0.0), 1.0);
	EXPECT_EQ(ccm::pow(nan, -0.0), 1.0);
	// pow(1, NaN) = 1 (already covered above)
	EXPECT_EQ(ccm::pow(1.0, nan), 1.0);
}

// FE_DIVBYZERO: pow(+/-0, y) for y < 0.

TEST(CcmathPowerTests, PowZeroPoleRaisesDivByZero)
{
	CCMATH_SKIP_MSVC_FENV();

	// pow(0, -1) should raise FE_DIVBYZERO
	ccm::test::ExpectFenvFlagsMatchStd(
		[] { consume(ccm::pow(runtime_value(0.0), runtime_value(-1.0))); },
		[] { consume(std::pow(runtime_value(0.0), runtime_value(-1.0))); },
		FE_DIVBYZERO);

	// pow(0, -2) should raise FE_DIVBYZERO
	ccm::test::ExpectFenvFlagsMatchStd(
		[] { consume(ccm::pow(runtime_value(0.0), runtime_value(-2.0))); },
		[] { consume(std::pow(runtime_value(0.0), runtime_value(-2.0))); },
		FE_DIVBYZERO);

	// pow(-0, -3) should raise FE_DIVBYZERO
	ccm::test::ExpectFenvFlagsMatchStd(
		[] { consume(ccm::pow(runtime_value(-0.0), runtime_value(-3.0))); },
		[] { consume(std::pow(runtime_value(-0.0), runtime_value(-3.0))); },
		FE_DIVBYZERO);

	// pow(0, -0.5) should raise FE_DIVBYZERO
	ccm::test::ExpectFenvFlagsMatchStd(
		[] { consume(ccm::pow(runtime_value(0.0), runtime_value(-0.5))); },
		[] { consume(std::pow(runtime_value(0.0), runtime_value(-0.5))); },
		FE_DIVBYZERO);
}

// FE_INVALID: pow(x, y) for finite x < 0 and finite non-integer y.

TEST(CcmathPowerTests, PowNegBaseNonIntExpRaisesInvalid)
{
	CCMATH_SKIP_MSVC_FENV();

	ccm::test::ExpectFenvFlagsMatchStd(
		[] { consume(ccm::pow(runtime_value(-1.0), runtime_value(0.5))); },
		[] { consume(std::pow(runtime_value(-1.0), runtime_value(0.5))); },
		FE_INVALID);

	ccm::test::ExpectFenvFlagsMatchStd(
		[] { consume(ccm::pow(runtime_value(-2.0), runtime_value(1.5))); },
		[] { consume(std::pow(runtime_value(-2.0), runtime_value(1.5))); },
		FE_INVALID);
}

// Overflow: pow with large positive results should match std overflow behavior.

TEST(CcmathPowerTests, PowOverflowMatchesStd)
{
	const double huge_base = std::numeric_limits<double>::max();
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(huge_base, 2.0), std::pow(huge_base, 2.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(2.0, 1024.0), std::pow(2.0, 1024.0));

	const float huge_basef = std::numeric_limits<float>::max();
	ccm::test::ExpectSameFloatingAsStd(ccm::powf(huge_basef, 2.0F), std::pow(huge_basef, 2.0F));
	ccm::test::ExpectSameFloatingAsStd(ccm::powf(2.0F, 128.0F), std::pow(2.0F, 128.0F));
}

// Underflow: pow with very small results.

TEST(CcmathPowerTests, PowUnderflowMatchesStd)
{
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(2.0, -1075.0), std::pow(2.0, -1075.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::powf(2.0F, -150.0F), std::pow(2.0F, -150.0F));
}

// Subnormal base handling.

TEST(CcmathPowerTests, PowSubnormalBase)
{
	const double subnormal = std::numeric_limits<double>::denorm_min();
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(subnormal, 1.0), std::pow(subnormal, 1.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(subnormal, 0.5), std::pow(subnormal, 0.5));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(subnormal, -1.0), std::pow(subnormal, -1.0));

	const float subnormalf = std::numeric_limits<float>::denorm_min();
	ccm::test::ExpectSameFloatingAsStd(ccm::powf(subnormalf, 1.0F), std::pow(subnormalf, 1.0F));
}

// Exact integer exponents.

TEST(CcmathPowerTests, PowExactIntegerExponents)
{
	// pow(x, 0) = 1
	ccm::test::ExpectFpEq(ccm::pow(2.0, 0.0), 1.0);

	// pow(x, 1) = x
	ccm::test::ExpectFpEq(ccm::pow(2.0, 1.0), 2.0);
	ccm::test::ExpectFpEq(ccm::powf(3.0F, 1.0F), 3.0F);

	// pow(x, 2) = x*x
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(3.0, 2.0), std::pow(3.0, 2.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::powf(3.0F, 2.0F), std::pow(3.0F, 2.0F));

	// pow(x, -1) = 1/x
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(4.0, -1.0), std::pow(4.0, -1.0));

	// pow(x, 0.5) = sqrt(x)
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(4.0, 0.5), std::pow(4.0, 0.5));
	ccm::test::ExpectSameFloatingAsStd(ccm::powf(4.0F, 0.5F), std::pow(4.0F, 0.5F));
}

// Negative base with integer exponent: sign propagation.

TEST(CcmathPowerTests, PowNegativeBaseSignPropagation)
{
	// Even integer exponent: result is positive
	EXPECT_GT(ccm::pow(-2.0, 2.0), 0.0);
	EXPECT_GT(ccm::pow(-2.0, 4.0), 0.0);
	EXPECT_GT(ccm::pow(-3.0, 6.0), 0.0);

	// Odd integer exponent: result is negative
	EXPECT_LT(ccm::pow(-2.0, 3.0), 0.0);
	EXPECT_LT(ccm::pow(-2.0, 5.0), 0.0);
	EXPECT_LT(ccm::pow(-3.0, 7.0), 0.0);

	EXPECT_GT(ccm::powf(-2.0F, 2.0F), 0.0F);
	EXPECT_LT(ccm::powf(-2.0F, 3.0F), 0.0F);
}

// F.10.4.4(15): pow(-inf, y) = +0 for y < 0 and not an odd integer, including non-integers.

TEST(CcmathPowerTests, PowNegInfBaseNegativeNonIntegerExponent)
{
	const double neg_inf = -std::numeric_limits<double>::infinity();
	ccm::test::ExpectFpEq(ccm::pow(neg_inf, -0.5), 0.0);
	EXPECT_FALSE(std::signbit(ccm::pow(neg_inf, -0.5)));
	ccm::test::ExpectFpEq(ccm::pow(neg_inf, -2.5), 0.0);
	ccm::test::ExpectFpEq(ccm::powf(-std::numeric_limits<float>::infinity(), -0.5F), 0.0F);
}

// x^-1 must be the correctly rounded reciprocal in every rounding mode. The integer-exponent
// kernel reciprocates an exact single-double divisor with IEEE division, which is correctly
// rounded, rather than a Newton step that could re-round the last bit to the wrong neighbor.
TEST(CcmathPowerTests, PowNegativeOneExponentMatchesDivision)
{
	for (const std::uint64_t base_bits :
		 {0x3f4fffffffffffffULL, 0x3fdfffffffffffffULL, 0x3fefffffffffffffULL, 0x3fffffffffffffffULL, 0x408fffffffffffffULL})
	{
		const double base = ccm::support::bit_cast<double>(base_bits);
		EXPECT_EQ(ccm::support::bit_cast<std::uint64_t>(ccm::pow(base, -1.0)), ccm::support::bit_cast<std::uint64_t>(1.0 / base));
	}
}

// Constexpr evaluation: All special cases should work at compile time.

TEST(CcmathPowerTests, PowSpecialCasesConstexpr)
{
	// pow(x, 0) = 1
	static_assert(ccm::pow(2.0, 0.0) == 1.0);
	static_assert(ccm::pow(-2.0, 0.0) == 1.0);
	static_assert(ccm::pow(0.0, 0.0) == 1.0);

	// pow(1, y) = 1
	static_assert(ccm::pow(1.0, 42.0) == 1.0);
	static_assert(ccm::pow(1.0, -42.0) == 1.0);

	// pow(0, positive) = 0
	static_assert(ccm::pow(0.0, 2.0) == 0.0);

	// Basic computations
	static_assert(ccm::pow(2.0, 1.0) == 2.0);
	static_assert(ccm::pow(2.0, 2.0) == 4.0);
	static_assert(ccm::pow(2.0, 10.0) == 1024.0);

	static_assert(ccm::powf(2.0F, 0.0F) == 1.0F);
	static_assert(ccm::powf(2.0F, 1.0F) == 2.0F);
	static_assert(ccm::powf(2.0F, 10.0F) == 1024.0F);

	static_assert(ccm::powl(2.0L, 0.0L) == 1.0L);
	static_assert(ccm::powl(2.0L, 10.0L) == 1024.0L);
}

// Cross-type consistency: powf, pow, powl agree on shared inputs.

TEST(CcmathPowerTests, PowCrossTypePrecisionConsistency)
{
	// For exact inputs, all types should agree
	EXPECT_EQ(static_cast<double>(ccm::powf(2.0F, 3.0F)), ccm::pow(2.0, 3.0));
	EXPECT_EQ(ccm::pow(2.0, 3.0), static_cast<double>(ccm::powl(2.0L, 3.0L)));

	// pow(2,10) = 1024 exactly in all precisions
	EXPECT_EQ(static_cast<double>(ccm::powf(2.0F, 10.0F)), 1024.0);
	EXPECT_EQ(ccm::pow(2.0, 10.0), 1024.0);
	EXPECT_EQ(static_cast<double>(ccm::powl(2.0L, 10.0L)), 1024.0);
}

// Large exponents at boundaries.

TEST(CcmathPowerTests, PowLargeExponents)
{
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(2.0, 53.0), std::pow(2.0, 53.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(2.0, -53.0), std::pow(2.0, -53.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(2.0, 1023.0), std::pow(2.0, 1023.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(1.0000000000000002, 1e15), std::pow(1.0000000000000002, 1e15));

	ccm::test::ExpectSameFloatingAsStd(ccm::powf(2.0F, 23.0F), std::pow(2.0F, 23.0F));
	ccm::test::ExpectSameFloatingAsStd(ccm::powf(2.0F, 127.0F), std::pow(2.0F, 127.0F));
}

// Identity: pow(x, 1) == x for all finite x.

TEST(CcmathPowerTests, PowIdentityExponent)
{
	const double values[] = { -100.0, -1.0, -0.0, 0.0, 1.0, 100.0,
							  std::numeric_limits<double>::min(),
							  std::numeric_limits<double>::max(),
							  std::numeric_limits<double>::denorm_min() };

	for (double x : values)
	{
		SCOPED_TRACE(x);
		ccm::test::ExpectSameFloatingAsStd(ccm::pow(x, 1.0), std::pow(x, 1.0));
	}
}

// Reciprocal: pow(x, -1) for positive finite x.

TEST(CcmathPowerTests, PowReciprocalExponent)
{
	const double values[] = { 0.5, 1.0, 2.0, 3.0, 10.0, 100.0,
							  std::numeric_limits<double>::min() };

	for (double x : values)
	{
		SCOPED_TRACE(x);
		ccm::test::ExpectSameFloatingAsStd(ccm::pow(x, -1.0), std::pow(x, -1.0));
	}
}

// Rounding-mode conformance: pow should produce correct results under all IEEE 754 rounding modes per C17 F.10.

TEST(CcmathPowerTests, PowDoubleUnderflowResidualPathMatrixRegression)
{
	for (const auto & test_case : ccm::test::worst_case::kPowDoubleUnderflowResidual)
	{
		const double base = ccm::support::bit_cast<double>(test_case.base_bits);
		const double expv = ccm::support::bit_cast<double>(test_case.exponent_bits);
		SCOPED_TRACE(test_case.provenance);
		ccm::test::ExpectFpEq(ccm::gen::pow_gen(runtime_value(base), runtime_value(expv)), 0.0);
		ccm::test::ExpectSameFloatingAsStd(ccm::pow(runtime_value(base), runtime_value(expv)), std::pow(base, expv));
	}
}

TEST(CcmathPowerTests, PowDoubleUnderflowScalePathBoundary)
{
	const double min_sub = std::numeric_limits<double>::denorm_min();

	// Scale-underflow path with hi unclamped: smallest positive subnormal must survive.
	ccm::test::ExpectFpEq(ccm::gen::pow_gen(runtime_value(2.0), runtime_value(-1074.0)), min_sub);
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(2.0, -1074.0), std::pow(2.0, -1074.0));

	// Adjacent subnormals above min subnormal (scale path, lo not cleared).
	ccm::test::ExpectSameFloatingAsStd(ccm::gen::pow_gen(2.0, -1073.0), std::pow(2.0, -1073.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::gen::pow_gen(2.0, -1072.0), std::pow(2.0, -1072.0));

	// Below min subnormal after scaling: flush to +0.
	ccm::test::ExpectFpEq(ccm::gen::pow_gen(runtime_value(2.0), runtime_value(-1075.0)), 0.0);
	ccm::test::ExpectFpEq(ccm::gen::pow_gen(runtime_value(2.0), runtime_value(-1076.0)), 0.0);
	ccm::test::ExpectFpEq(ccm::gen::pow_gen(runtime_value(2.0), runtime_value(-1077.0)), 0.0);
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(2.0, -1075.0), std::pow(2.0, -1075.0));

	// Exponent neighbors around the min-subnormal threshold.
	ccm::test::ExpectSameFloatingAsStd(ccm::gen::pow_gen(2.0, std::nextafter(-1074.0, 0.0)), std::pow(2.0, std::nextafter(-1074.0, 0.0)));
	ccm::test::ExpectSameFloatingAsStd(ccm::gen::pow_gen(2.0, std::nextafter(-1074.0, -2000.0)),
									   std::pow(2.0, std::nextafter(-1074.0, -2000.0)));

	// Base perturbation at the threshold (log2 table path still active).
	ccm::test::ExpectSameFloatingAsStd(ccm::gen::pow_gen(std::nextafter(2.0, 1.0), -1074.0),
									   std::pow(std::nextafter(2.0, 1.0), -1074.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::gen::pow_gen(std::nextafter(2.0, 3.0), -1074.0),
									   std::pow(std::nextafter(2.0, 3.0), -1074.0));

	// Normal to subnormal transition (no scale split on these exponents).
	ccm::test::ExpectSameFloatingAsStd(ccm::gen::pow_gen(2.0, -1023.0), std::pow(2.0, -1023.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::gen::pow_gen(2.0, -1022.0), std::pow(2.0, -1022.0));
}

TEST(CcmathPowerTests, PowRoundingModeConformance)
{
	ccm::test::ForEachRoundingModeOrSkip([](int mode) {
		SCOPED_TRACE(ccm::test::RoundingModeName(mode));

		// Special cases must hold regardless of rounding mode
		ccm::test::ExpectSameFloatingAsStd(ccm::pow(runtime_value(1.0), runtime_value(42.0)), std::pow(1.0, 42.0));
		ccm::test::ExpectSameFloatingAsStd(ccm::pow(runtime_value(2.0), runtime_value(0.0)), std::pow(2.0, 0.0));
		ccm::test::ExpectSameFloatingAsStd(ccm::pow(runtime_value(0.0), runtime_value(3.0)), std::pow(0.0, 3.0));

		// Representative numeric cases
		ccm::test::ExpectSameFloatingAsStd(ccm::pow(runtime_value(2.0), runtime_value(10.0)), std::pow(2.0, 10.0));
		ccm::test::ExpectSameFloatingAsStd(ccm::pow(runtime_value(3.0), runtime_value(7.0)), std::pow(3.0, 7.0));
	});
}
