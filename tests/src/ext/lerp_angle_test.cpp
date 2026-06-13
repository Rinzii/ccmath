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

#include <ccmath/ext/lerp_angle.hpp>

TEST(CcmathExtLerpAngleTest, Smoke)
{
	static_assert(ccm::ext::lerp_angle(0.0, 90.0, 0.5) == 45.0);
	static_assert(ccm::ext::safe::lerp_angle(0.0, 90.0, 0.5) == 45.0);

	EXPECT_DOUBLE_EQ(ccm::ext::lerp_angle(0.0, 90.0, 0.5), 45.0);
}
