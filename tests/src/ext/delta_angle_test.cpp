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

#include <ccmath/ext/delta_angle.hpp>

TEST(CcmathExtDeltaAngleTest, Smoke)
{
	static_assert(ccm::ext::delta_angle(0.0, 90.0) == 90.0);
	static_assert(ccm::ext::delta_angle(350.0, 10.0) == 20.0);
	static_assert(ccm::ext::safe::delta_angle(350.0, 10.0) == 20.0);

	EXPECT_DOUBLE_EQ(ccm::ext::delta_angle(0.0, 180.0), 180.0);
}
