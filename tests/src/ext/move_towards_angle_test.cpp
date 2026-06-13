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

#include <ccmath/ext/move_towards_angle.hpp>

TEST(CcmathExtMoveTowardsAngleTest, Smoke)
{
	static_assert(ccm::ext::move_towards_angle(0.0, 90.0, 45.0) == 45.0);
	static_assert(ccm::ext::safe::move_towards_angle(0.0, 90.0, 45.0) == 45.0);

	EXPECT_DOUBLE_EQ(ccm::ext::move_towards_angle(0.0, 90.0, 45.0), 45.0);
	EXPECT_DOUBLE_EQ(ccm::ext::move_towards_angle(0.0, 90.0, 200.0), 90.0);
	EXPECT_DOUBLE_EQ(ccm::ext::move_towards_angle(350.0, 10.0, 15.0), 365.0);
	EXPECT_DOUBLE_EQ(ccm::ext::safe::move_towards_angle(350.0, 10.0, 15.0), 365.0);
}
