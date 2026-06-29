/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "ccmath/internal/types/dyadic_float.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace
{
	using Float128 = ccm::types::DyadicFloat<128>;

	constexpr bool kNoFpExceptions = false;

	double to_double(const Float128 & value)
	{
		return value.template as<double, kNoFpExceptions>();
	}
} // namespace

TEST(CcmathInternalTypesTests, DyadicFloatQuickSubAndNegation)
{
	const Float128 a(1.5);
	const Float128 b(0.25);
	const Float128 diff = ccm::types::quick_sub(a, b);

	EXPECT_DOUBLE_EQ(to_double(diff), 1.25);
	EXPECT_EQ((-a).sign, ccm::types::Sign::NEG);
}

TEST(CcmathInternalTypesTests, DyadicFloatRoundedMulMatchesQuickMulNearOne)
{
	const Float128 a(1.125);
	const Float128 b(1.0625);
	const Float128 quick   = ccm::types::quick_mul(a, b);
	const Float128 rounded = ccm::types::rounded_mul(a, b);

	EXPECT_NEAR(to_double(quick), 1.1953125, 1e-12);
	EXPECT_NEAR(to_double(rounded), to_double(quick), 1e-9);
}

TEST(CcmathInternalTypesTests, DyadicFloatGenericAsMatchesDouble)
{
	const Float128 value(1.0 + std::ldexp(1.0, -40));
	const double generic = value.template generic_as<double, kNoFpExceptions>();
	const double fast	 = to_double(value);
	EXPECT_DOUBLE_EQ(generic, fast);
}
