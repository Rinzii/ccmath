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
#include "ccmath/internal/math/generic/func/power/powl_gen.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>

namespace
{
#define REQUIRE_POWL_LD80()                                                                                                                                    \
	do                                                                                                                                                         \
	{                                                                                                                                                          \
		if (ccm::config::detect_long_double_format() != ccm::config::LongDoubleFormat::X87Extended) { GTEST_SKIP() << "x87 80-bit long double required"; }     \
	} while (false)

	void ExpectInteger(long double value, bool is_integer, bool is_odd = false)
	{
		EXPECT_EQ(ccm::gen::internal::powl_bits::is_integer(value), is_integer) << "value=" << value;
		if (is_integer) { EXPECT_EQ(ccm::gen::internal::powl_bits::is_odd_integer(value), is_odd) << "value=" << value; }
	}
} // namespace

TEST(PowlLd80IntegerClass, ParityThresholdsAroundTwoTo62ThroughTwoTo65)
{
	REQUIRE_POWL_LD80();

	const long double two_to_62 = 0x1.0p62L;
	const long double two_to_63 = 0x1.0p63L;

	ExpectInteger(two_to_62 - 1.0L, true, true);
	ExpectInteger(two_to_62, true, false);
	ExpectInteger(two_to_62 + 1.0L, true, true);
	ExpectInteger(two_to_62 + 2.0L, true, false);
	ExpectInteger(two_to_63 - 1.0L, true, true);
	ExpectInteger(two_to_63, true, false);
	ExpectInteger(0x1.0p64L, true, false);
	ExpectInteger(0x1.0p65L, true, false);
	ExpectInteger(std::nextafter(0x1.0p65L, std::numeric_limits<long double>::infinity()), true, false);

	ExpectInteger(-(two_to_62 - 1.0L), true, true);
	ExpectInteger(-two_to_62, true, false);
}

TEST(PowlLd80IntegerClass, HalfIntegersAreNonInteger)
{
	REQUIRE_POWL_LD80();

	ExpectInteger(0.5L, false);
	ExpectInteger(1.5L, false);
	ExpectInteger(-2.5L, false);
	ExpectInteger(std::nextafter(1.0L, 2.0L), false);
}

TEST(PowlLd80IntegerClass, NegativeBaseParityMatchesStdPow)
{
	REQUIRE_POWL_LD80();

	const auto expect_parity = [](long double exponent, long double expected)
	{
		const long double actual = ccm::gen::pow_gen(-1.0L, exponent);
		EXPECT_EQ(actual, expected) << "exponent=" << exponent;
	};

	expect_parity(3.0L, -1.0L);
	expect_parity(4.0L, 1.0L);
	expect_parity(0x1.0p62L, 1.0L);
	expect_parity(0x1.0p62L + 1.0L, -1.0L);
}

TEST(PowlLd80IntegerClass, NonIntegerNegativeBaseIsDomainError)
{
	REQUIRE_POWL_LD80();

	const long double actual = ccm::gen::pow_gen(-2.0L, 0.5L);
	EXPECT_TRUE(std::isnan(actual));
}
