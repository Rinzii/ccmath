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

TEST(CcmathExtCeilDivTest, Smoke)
{
	static_assert(ccm::ext::ceil_div(7, 3) == 3);
	static_assert(ccm::ext::ceil_div(6, 3) == 2);
	static_assert(ccm::ext::ceil_div(1, 4) == 1);
	static_assert(ccm::ext::safe::ceil_div(7, 0) == 0);

	EXPECT_EQ(ccm::ext::ceil_div(10, 4), 3);
	EXPECT_EQ(ccm::ext::safe::ceil_div(10, 0), 0);
}
