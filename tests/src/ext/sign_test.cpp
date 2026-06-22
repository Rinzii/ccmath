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

#include <ccmath/ext/sign.hpp>

TEST(CcmathExtSignTest, Smoke)
{
	static_assert(ccm::ext::sign(-5) == -1);
	static_assert(ccm::ext::sign(0) == 0);
	static_assert(ccm::ext::sign(3) == 1);
	static_assert(ccm::ext::sign(-2.5) == -1);
	static_assert(ccm::ext::sign(0.0) == 0.0);
	static_assert(ccm::ext::sign(2.5) == 1.0);

	EXPECT_EQ(ccm::ext::sign(-7), -1);
	EXPECT_EQ(ccm::ext::sign(0), 0);
	EXPECT_EQ(ccm::ext::sign(7), 1);
}
