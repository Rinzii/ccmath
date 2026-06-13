/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "ccmath/ccmath.hpp"
#include "ccmath/internal/config/long_double_format.hpp"
#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace
{
	void ExpectAliasMatchesDouble(long double base, long double exponent)
	{
		const long double via_powl = ccm::gen::pow_gen(base, exponent);
		const long double expected = static_cast<long double>(ccm::gen::pow_gen(static_cast<double>(base), static_cast<double>(exponent)));
		EXPECT_EQ(via_powl, expected) << "base=" << base << " exponent=" << exponent;
	}
} // namespace

TEST(PowlLd64Alias, SelectsDoubleShapedPath)
{
	if (ccm::config::detect_long_double_format() != ccm::config::LongDoubleFormat::Double)
	{
		GTEST_SKIP() << "long double is not double-shaped on this platform";
	}

	ExpectAliasMatchesDouble(2.0L, 3.0L);
	ExpectAliasMatchesDouble(0.5L, -2.0L);
	ExpectAliasMatchesDouble(10.0L, 0.5L);
	ExpectAliasMatchesDouble(-2.0L, 3.0L);
	ExpectAliasMatchesDouble(3.0L, 10.0L);
}

TEST(PowlLd64Alias, PublicPowlMatchesGenPath)
{
	if (ccm::config::detect_long_double_format() != ccm::config::LongDoubleFormat::Double)
	{
		GTEST_SKIP() << "long double is not double-shaped on this platform";
	}

	const std::array<long double, 4> bases	   = { 0.25L, 1.0L, 2.0L, 3.0L };
	const std::array<long double, 4> exponents = { -2.0L, 0.5L, 2.0L, 3.0L };
	for (long double base : bases)
	{
		for (long double exponent : exponents) { EXPECT_EQ(ccm::powl(base, exponent), ccm::gen::pow_gen(base, exponent)); }
	}
}
