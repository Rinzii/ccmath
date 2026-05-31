/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#pragma once

#include "ccmath/internal/support/fp/fp_bits.hpp"

#include <gtest/gtest.h>
#include <type_traits>

namespace ccm::test
{
	/// Bit-exact floating-point equality (LLVM libc EXPECT_FP_EQ semantics).
	template <typename T>
	void ExpectFpEq(T actual, T expected)
	{
		static_assert(std::is_floating_point_v<T>, "T must be floating-point");

		using FPBits = ccm::support::fp::FPBits<T>;
		FPBits actual_bits(actual);
		FPBits expected_bits(expected);

		if (expected_bits.is_nan())
		{
			EXPECT_TRUE(actual_bits.is_nan()) << "expected NaN";
			return;
		}

		EXPECT_EQ(actual_bits.uintval(), expected_bits.uintval()) << "actual=" << actual << " expected=" << expected;
	}

	template <typename T>
	void ExpectFpNe(T actual, T expected)
	{
		using FPBits = ccm::support::fp::FPBits<T>;
		FPBits actual_bits(actual);
		FPBits expected_bits(expected);

		if (actual_bits.is_nan()) { EXPECT_FALSE(expected_bits.is_nan()); }
		else if (expected_bits.is_nan()) { EXPECT_TRUE(actual_bits.is_nan()); }
		else { EXPECT_NE(actual_bits.uintval(), expected_bits.uintval()); }
	}

	template <typename T>
	void ExpectSignedZero(T value, bool negative)
	{
		EXPECT_EQ(value, T{});
		EXPECT_EQ(std::signbit(value), negative);
	}

} // namespace ccm::test

#define CCM_EXPECT_FP_EQ(actual, expected) ::ccm::test::ExpectFpEq((actual), (expected))
#define CCM_EXPECT_FP_NE(actual, expected) ::ccm::test::ExpectFpNe((actual), (expected))
