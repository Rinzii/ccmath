/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "ccmath/internal/support/fp/except_value_utils.hpp"
#include "ccmath/internal/types/dyadic_float.hpp"
#include "ccmath/internal/types/normalized_float.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace
{
	using FPBits = ccm::support::fp::FPBits<float>;

	constexpr ccm::support::fp::ExceptValues<float, 1> kFloatExcepts{ { {
		{ 0x3f800000U, 0x3f000000U, 0U, 0U, 0U },
	} } };
} // namespace

TEST(CcmathInternalTypesTests, NormalizedFloatMul2MatchesLdexp)
{
	const ccm::types::NormalizedFloat<double> value(1.5);
	const double result = static_cast<double>(value.mul2(2));
	EXPECT_DOUBLE_EQ(result, 6.0);
}

TEST(CcmathInternalTypesTests, NormalizedFloatRoundTrip)
{
	const double input = 1.0 + std::ldexp(1.0, -20);
	const ccm::types::NormalizedFloat<double> normalized(input);
	EXPECT_DOUBLE_EQ(static_cast<double>(normalized), input);
}

TEST(CcmathInternalSupportTests, ExceptValuesLookupTowardZero)
{
	const float result = kFloatExcepts.lookup(0x3f800000U).value_or(0.0F);
	EXPECT_EQ(FPBits(result).uintval(), 0x3f000000U);
}

TEST(CcmathInternalSupportTests, ExceptValuesLookupMissReturnsNullopt)
{ EXPECT_FALSE(kFloatExcepts.lookup(0x40000000U).has_value()); }

TEST(CcmathInternalTypesTests, DyadicFloatRoundedDivNearOne)
{
	using Dyadic = ccm::types::DyadicFloat<128>;
	const Dyadic a(1.5);
	const Dyadic b(1.5);
	const Dyadic quotient	  = ccm::types::rounded_div(a, b);
	const double quotient_val = quotient.template as<double, false>();
	EXPECT_NEAR(quotient_val, 1.0, 1e-12);
}
