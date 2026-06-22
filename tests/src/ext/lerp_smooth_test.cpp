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

#include <ccmath/ext/lerp_smooth.hpp>

#include <cmath>

TEST(CcmathExtLerpSmoothTest, Smoke)
{
	static_assert(ccm::ext::lerp_smooth(0.0, 10.0, 0.0, 1.0) == 0.0);

	EXPECT_DOUBLE_EQ(ccm::ext::lerp_smooth(0.0, 10.0, 0.0, 1.0), 0.0);
	EXPECT_NEAR(ccm::ext::lerp_smooth(0.0, 10.0, 1.0, 1.0), 5.0, 1e-12);
	EXPECT_NEAR(ccm::ext::lerp_smooth(0.0, 10.0, 100.0, 1.0), 10.0, 1e-9);
}
