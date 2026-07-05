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
#include <ccmath/math/numbers.hpp>

TEST(CcmathExtLerpAngleTest, Smoke)
{
	constexpr double pi = ccm::numbers::pi_v<double>;

	static_assert(ccm::ext::lerp_angle(0.0, pi / 2, 0.5) == pi / 4);
	static_assert(ccm::ext::lerp_angle(0.0, pi / 2, 0.0) == 0.0);

	EXPECT_DOUBLE_EQ(ccm::ext::lerp_angle(0.0, pi / 2, 0.5), pi / 4);
	EXPECT_DOUBLE_EQ(ccm::ext::lerp_angle(0.0, pi / 2, 1.0), pi / 2);
}
