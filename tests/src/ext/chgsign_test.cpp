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

#include <ccmath/ext/chgsign.hpp>

TEST(CcmathExtChgsignTest, Smoke)
{
	static_assert(ccm::ext::chgsign(5.0, -1.0) == -5.0);
	static_assert(ccm::ext::chgsign(-3.0, 2.0) == 3.0);
	static_assert(ccm::ext::chgsign(4.0, -0.0) == -4.0);

	EXPECT_DOUBLE_EQ(ccm::ext::chgsign(5.0, -1.0), -5.0);
	EXPECT_DOUBLE_EQ(ccm::ext::chgsign(-3.0, 2.0), 3.0);
	EXPECT_DOUBLE_EQ(ccm::ext::chgsign(4.0, -2.0), -4.0);
	EXPECT_DOUBLE_EQ(ccm::ext::chgsign(-4.0, 2.0), 4.0);
	EXPECT_DOUBLE_EQ(ccm::ext::chgsign(4.0, -0.0), -4.0);
}
