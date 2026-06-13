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

#include <ccmath/ext/abs_diff.hpp>

TEST(CcmathExtAbsDiffTest, Smoke)
{
	static_assert(ccm::ext::abs_diff(10, 3) == 7);
	static_assert(ccm::ext::abs_diff(3, 10) == 7);
	static_assert(ccm::ext::abs_diff(0U, 5U) == 5U);

	EXPECT_EQ(ccm::ext::abs_diff(42, 17), 25);
	EXPECT_EQ(ccm::ext::abs_diff(17, 42), 25);
}
