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

#include <ccmath/ext/repeat.hpp>

TEST(CcmathExtRepeatTest, Smoke)
{
	static_assert(ccm::ext::repeat(370.0, 360.0) == 10.0);
	static_assert(ccm::ext::repeat(-10.0, 360.0) == 350.0);
	static_assert(ccm::ext::repeat(5.0, 0.0) == 0.0);
	static_assert(ccm::ext::unsafe::repeat(370.0, 360.0) == 10.0);

	EXPECT_DOUBLE_EQ(ccm::ext::repeat(7.5, 3.0), 1.5);
	EXPECT_DOUBLE_EQ(ccm::ext::repeat(7.5, 0.0), 0.0);
	EXPECT_DOUBLE_EQ(ccm::ext::unsafe::repeat(7.5, 3.0), 1.5);
}
