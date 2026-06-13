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

#include <ccmath/ext/unlerp.hpp>

TEST(CcmathExtUnlerpTest, Smoke)
{
	static_assert(ccm::ext::unlerp(0.0, 10.0, 5.0) == 0.5);
	static_assert(ccm::ext::safe::unlerp(5.0, 5.0, 7.0) == 0.0);

	EXPECT_DOUBLE_EQ(ccm::ext::unlerp(2.0, 6.0, 4.0), 0.5);
	EXPECT_DOUBLE_EQ(ccm::ext::safe::unlerp(1.0, 1.0, 3.0), 0.0);
}
