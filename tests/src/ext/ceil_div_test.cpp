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

#include <ccmath/ext/ceil_div.hpp>

#include <limits>

TEST(CcmathExtCeilDivTest, Smoke)
{
	static_assert(ccm::ext::ceil_div(7, 3) == 3);
	static_assert(ccm::ext::ceil_div(6, 3) == 2);
	static_assert(ccm::ext::ceil_div(1, 4) == 1);
	static_assert(ccm::ext::ceil_div(7, 0) == 0);

	EXPECT_EQ(ccm::ext::ceil_div(10, 4), 3);
	EXPECT_EQ(ccm::ext::ceil_div(10, 0), 0);
}

TEST(CcmathExtCeilDivTest, ExactDivisionDoesNotRoundUp)
{
	static_assert(ccm::ext::ceil_div(6, 2) == 3);
	static_assert(ccm::ext::ceil_div(0, 5) == 0);
	EXPECT_EQ(ccm::ext::ceil_div(100, 10), 10);
}

TEST(CcmathExtCeilDivTest, NegativeOperands)
{
	// Rounds toward positive infinity for every sign combination.
	static_assert(ccm::ext::ceil_div(-7, 2) == -3); // ceil(-3.5)
	static_assert(ccm::ext::ceil_div(7, -2) == -3); // ceil(-3.5)
	static_assert(ccm::ext::ceil_div(-7, -2) == 4); // ceil(3.5)
	static_assert(ccm::ext::ceil_div(-6, 2) == -3); // exact, no rounding
	static_assert(ccm::ext::ceil_div(-1, 4) == 0);	// ceil(-0.25)

	EXPECT_EQ(ccm::ext::ceil_div(-10, 4), -2); // ceil(-2.5)
	EXPECT_EQ(ccm::ext::ceil_div(10, -4), -2);
	EXPECT_EQ(ccm::ext::ceil_div(-7, 2), -3);
}

TEST(CcmathExtCeilDivTest, NoOverflowAtExtremes)
{
	// The old value + divisor - 1 form overflowed near the type maximum.
	constexpr int imax = std::numeric_limits<int>::max();
	static_assert(ccm::ext::ceil_div(imax, 1) == imax);
	static_assert(ccm::ext::ceil_div(imax, 2) == imax / 2 + 1);
	EXPECT_EQ(ccm::ext::ceil_div(imax, 1), imax);

	constexpr int imin = std::numeric_limits<int>::min();
	static_assert(ccm::ext::ceil_div(imin, 1) == imin);
}

TEST(CcmathExtCeilDivTest, Unsigned)
{
	static_assert(ccm::ext::ceil_div(5u, 2u) == 3u);
	static_assert(ccm::ext::ceil_div(8u, 4u) == 2u);

	constexpr unsigned umax = std::numeric_limits<unsigned>::max();
	static_assert(ccm::ext::ceil_div(umax, 1u) == umax); // no overflow
	EXPECT_EQ(ccm::ext::ceil_div(255u, 16u), 16u);
}
