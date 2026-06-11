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

#include <limits>

namespace
{

	constexpr bool frexp_smoke()
	{
		int exp = 0;
		return ccm::frexp(8.0, &exp) == 0.5 && exp == 4;
	}

	constexpr bool modf_smoke()
	{
		double iptr = 0.0;
		return ccm::modf(3.25, &iptr) == 0.25 && iptr == 3.0;
	}

	constexpr bool remquo_smoke()
	{
		int quo			 = 0;
		const double rem = ccm::remquo(10.0, 3.0, &quo);
		return rem == 1.0 && quo == 3;
	}

} // namespace

TEST(CcmathMathConstexprSmokeTest, AllImplementedFunctions)
{
	static_assert(ccm::abs(-2) == 2);
	static_assert(ccm::fabs(-2.0) == 2.0);
	static_assert(ccm::fabsf(-2.0F) == 2.0F);
	EXPECT_EQ(ccm::fabsl(-2.0L), 2.0L);

	static_assert(ccm::fdim(3.0, 1.0) == 2.0);
	static_assert(ccm::fma(2.0, 3.0, 1.0) == 7.0);
	static_assert(ccm::fmod(7.0, 3.0) == 1.0);
	static_assert(ccm::max(1.0, 2.0) == 2.0);
	static_assert(ccm::min(1.0, 2.0) == 1.0);
	static_assert(ccm::isnan(ccm::nan("")));
	static_assert(ccm::isnan(ccm::nanf("")));
	// nanl omitted here for the same reason as nan_test.cpp
	static_assert(ccm::remainder(7.0, 3.0) == 1.0);
	static_assert(remquo_smoke());

	static_assert(ccm::fpclassify(1.0) == FP_NORMAL);
	static_assert(ccm::isfinite(1.0));
	static_assert(ccm::isgreater(1.0, 0.0));
	static_assert(ccm::isgreaterequal(1.0, 1.0));
	static_assert(!ccm::isinf(1.0));
	static_assert(ccm::isless(0.0, 1.0));
	static_assert(ccm::islessequal(1.0, 1.0));
	static_assert(ccm::islessgreater(1.0, 0.0));
	static_assert(!ccm::isnan(1.0));
	static_assert(ccm::isnormal(1.0));
	static_assert(!ccm::isunordered(1.0, 2.0));
	static_assert(ccm::signbit(-1.0));

	static_assert(ccm::exp(0.0) == 1.0);
	static_assert(ccm::exp2(3.0) == 8.0);
	static_assert(ccm::expm1(0.0) == 0.0);
	static_assert(ccm::log(1.0) == 0.0);
	static_assert(ccm::log10(100.0) == 2.0);
	static_assert(ccm::log1p(0.0) == 0.0);
	static_assert(ccm::log2(8.0) == 3.0);

	static_assert(ccm::copysign(1.0, -2.0) == -1.0);
	static_assert(frexp_smoke());
	static_assert(ccm::ilogb(1.0) == 0);
	static_assert(ccm::ldexp(0.5, 1) == 1.0);
	static_assert(ccm::logb(10.0) > 0.0);
	static_assert(modf_smoke());
	static_assert(ccm::nextafter(1.0, 2.0) > 1.0);
	static_assert(ccm::nextdown(1.0) < 1.0);
	static_assert(ccm::nextup(1.0) > 1.0);
	static_assert(ccm::nexttoward(1.0, 2.0L) > 1.0);
	static_assert(ccm::scalbn(1.0, 1) == 2.0);
	static_assert(ccm::scalbln(1.0, 1L) == 2.0);

	static_assert(ccm::ceil(1.25) == 2.0);
	static_assert(ccm::floor(1.75) == 1.0);
	static_assert(ccm::nearbyint(1.5) == 2.0);
	static_assert(ccm::rint(1.5) == 2.0);
	static_assert(ccm::round(1.5) == 2.0);
	static_assert(ccm::trunc(1.75) == 1.0);

	static_assert(ccm::cbrt(8.0) == 2.0);
	static_assert(ccm::hypot(3.0, 4.0) == 5.0);
	static_assert(ccm::pow(2.0, 3.0) == 8.0);
	static_assert(ccm::sqrt(4.0) == 2.0);

	static_assert(ccm::sin(0.0) == 0.0);
	static_assert(ccm::cos(0.0) == 1.0);
	static_assert(ccm::tan(0.0) == 0.0);
	static_assert(ccm::asin(0.0) == 0.0);
	static_assert(ccm::acos(1.0) == 0.0);
	static_assert(ccm::atan(0.0) == 0.0);
	static_assert(ccm::atan2(1.0, 1.0) > 0.0);

	static_assert(ccm::tgamma(5.0) == 24.0);
	static_assert(ccm::gamma(5.0) == 24.0);
	static_assert(ccm::lgamma(1.0) == 0.0);
	static_assert(ccm::lerp(0.0, 10.0, 0.5) == 5.0);
}
