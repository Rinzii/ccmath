/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "utils/std_compare.hpp"

#include <gtest/gtest.h>

#include <ccmath/ccmath.hpp>

#include <cmath>
#include <limits>

namespace
{
	constexpr double make_remainder(double x, double y)
	{ return ccm::remainder(x, y); }

	// Pulls the result of remainder(finite, inf) into a constant context.
	constexpr double remainder_by_inf(double x)
	{ return ccm::remainder(x, std::numeric_limits<double>::infinity()); }
} // namespace

TEST(CcmathBasicTests, RemainderCompileTime)
{
	// remainder rounds the quotient to nearest, ties-to-even, so these are the values the standard
	// requires. The old trunc/fmod formula returned 2.0 and 1.5 here.
	static_assert(make_remainder(5.0, 3.0) == -1.0, "remainder(5, 3) must be -1");
	static_assert(make_remainder(7.5, 2.0) == -0.5, "remainder(7.5, 2) must be -0.5");

	// A half-way quotient rounds to even.
	static_assert(make_remainder(2.0, 1.0) == 0.0, "remainder(2, 1) must round the tie to 0");
	static_assert(make_remainder(3.0, 2.0) == -1.0, "remainder(3, 2) must be -1");

	// remainder of a finite value by an infinity is that finite value.
	static_assert(remainder_by_inf(3.0) == 3.0, "remainder(3, inf) must be 3");

	// abs(x / y) well above 2^53 must still reduce exactly at compile time. This is the residual that
	// slipped through before fmod's generic kernel became the exact fdlibm reduction (remainder routes
	// through remquo, which reduces with ccm::fmod).
	static_assert(make_remainder(1e30, 3.0) == 1.0, "remainder(1e30, 3) must be 1");
	static_assert(make_remainder(-1e30, 3.0) == -1.0, "remainder(-1e30, 3) must be -1");

	static_assert(ccm::remainder(1.0, 1.0) == 0.0, "remainder(1, 1) must be 0");
	static_assert(ccm::remainderf(5.0F, 3.0F) == -1.0F, "remainderf(5, 3) must be -1");
}

TEST(CcmathBasicTests, RemainderConfirmedCases)
{
	// The three confirmed-wrong cases from the H1 bug report, checked at runtime.
	EXPECT_EQ(ccm::remainder(5.0, 3.0), -1.0);
	EXPECT_EQ(ccm::remainder(7.5, 2.0), -0.5);

	const double inf = std::numeric_limits<double>::infinity();
	EXPECT_EQ(ccm::remainder(3.0, inf), 3.0);
	EXPECT_EQ(ccm::remainder(-3.0, inf), -3.0);
	EXPECT_EQ(ccm::remainder(3.0, -inf), 3.0);
}

TEST(CcmathBasicTests, RemainderSignOfZero)
{
	// remainder(+/-0, y) is +/-0 for finite nonzero y, sign of x preserved.
	const double pos = ccm::remainder(0.0, 3.0);
	const double neg = ccm::remainder(-0.0, 3.0);
	EXPECT_EQ(pos, 0.0);
	EXPECT_EQ(neg, 0.0);
	EXPECT_FALSE(std::signbit(pos));
	EXPECT_TRUE(std::signbit(neg));

	const float posf = ccm::remainderf(0.0F, 3.0F);
	const float negf = ccm::remainderf(-0.0F, 3.0F);
	EXPECT_EQ(posf, 0.0F);
	EXPECT_EQ(negf, 0.0F);
	EXPECT_FALSE(std::signbit(posf));
	EXPECT_TRUE(std::signbit(negf));
}

TEST(CcmathBasicTests, RemainderSpecialPropagation)
{
	const double inf = std::numeric_limits<double>::infinity();
	const double nan = std::numeric_limits<double>::quiet_NaN();

	// NaN is returned for an infinite dividend, a zero divisor, or a NaN argument.
	EXPECT_TRUE(std::isnan(ccm::remainder(inf, 3.0)));
	EXPECT_TRUE(std::isnan(ccm::remainder(-inf, 3.0)));
	EXPECT_TRUE(std::isnan(ccm::remainder(3.0, 0.0)));
	EXPECT_TRUE(std::isnan(ccm::remainder(3.0, -0.0)));
	EXPECT_TRUE(std::isnan(ccm::remainder(nan, 3.0)));
	EXPECT_TRUE(std::isnan(ccm::remainder(3.0, nan)));
	EXPECT_TRUE(std::isnan(ccm::remainder(nan, nan)));
}

TEST(CcmathBasicTests, RemainderLargeQuotient)
{
	// |x / y| well above 2^53 must still reduce exactly. The trunc formula collapsed here.
	ccm::test::ExpectBinaryMatchesStd(1e30, 3.0, ccm::remainder<double>, static_cast<double (*)(double, double)>(std::remainder));
	ccm::test::ExpectBinaryMatchesStd(-1e30, 3.0, ccm::remainder<double>, static_cast<double (*)(double, double)>(std::remainder));
	ccm::test::ExpectBinaryMatchesStd(1e300, 7.0, ccm::remainder<double>, static_cast<double (*)(double, double)>(std::remainder));
	ccm::test::ExpectBinaryMatchesStd(1e30F, 3.0F, ccm::remainder<float>, static_cast<float (*)(float, float)>(std::remainder));
}

TEST(CcmathBasicTests, Remainder)
{
	static_assert(ccm::remainder(1.0, 1.0) == 0.0, "remainder has failed testing that it is static_assert-able!");

	using double_fn = double (*)(double, double);
	using float_fn	= float (*)(float, float);

	// A spread of normal values, both signs, for double.
	const double doubles[][2] = {
		{ 1.0, 1.0 },  { 1.0, 0.0 }, { 0.0, 1.0 },	 { 0.0, 0.0 },	  { -1.0, 1.0 },	{ 1.0, -1.0 }, { -1.0, -1.0 },
		{ -1.0, 0.0 }, { 5.0, 3.0 }, { 7.5, 2.0 },	 { -5.0, 3.0 },	  { 5.0, -3.0 },	{ -7.5, 2.0 }, { 9.0, 4.0 },
		{ 2.0, 1.0 },  { 3.0, 2.0 }, { 10.5, 3.25 }, { -10.5, 3.25 }, { 123.456, 7.0 }, { 0.1, 0.03 }, { -0.1, 0.03 },
	};
	for (const auto & c : doubles) { ccm::test::ExpectBinaryMatchesStd(c[0], c[1], ccm::remainder<double>, static_cast<double_fn>(std::remainder)); }

	// A spread of normal values, both signs, for float.
	const float floats[][2] = {
		{ 1.0F, 1.0F }, { 5.0F, 3.0F }, { 7.5F, 2.0F },	  { -5.0F, 3.0F },	  { 5.0F, -3.0F },
		{ 9.0F, 4.0F }, { 2.0F, 1.0F }, { 10.5F, 3.25F }, { 123.456F, 7.0F }, { 0.1F, 0.03F },
	};
	for (const auto & c : floats) { ccm::test::ExpectBinaryMatchesStd(c[0], c[1], ccm::remainder<float>, static_cast<float_fn>(std::remainder)); }

	// Subnormal operands.
	constexpr double subnormal_dividend = -5.0166534782602e-198;
	constexpr double subnormal_divisor	= 5.27085811e-315;
	ccm::test::ExpectBinaryMatchesStd(subnormal_dividend, subnormal_divisor, ccm::remainder<double>, static_cast<double_fn>(std::remainder));

	// Long double goes through the same public entry point.
	ccm::test::ExpectBinaryMatchesStd(5.0L, 3.0L, ccm::remainderl, static_cast<long double (*)(long double, long double)>(std::remainder));
	ccm::test::ExpectBinaryMatchesStd(7.5L, 2.0L, ccm::remainderl, static_cast<long double (*)(long double, long double)>(std::remainder));
}
