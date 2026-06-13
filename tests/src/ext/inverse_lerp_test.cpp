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

#include <ccmath/ext/inverse_lerp.hpp>

TEST(CcmathExtInverseLerpTest, Smoke)
{
	static_assert(ccm::ext::inverse_lerp(0.0, 10.0, 5.0) == 0.5);
	static_assert(ccm::ext::safe::inverse_lerp(5.0, 5.0, 7.0) == 0.0);

	EXPECT_DOUBLE_EQ(ccm::ext::inverse_lerp(2.0, 6.0, 4.0), 0.5);
	EXPECT_DOUBLE_EQ(ccm::ext::safe::inverse_lerp(1.0, 1.0, 3.0), 0.0);
}
