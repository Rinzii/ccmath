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
#include <ccmath/math/numbers.hpp>

TEST(CcmathExtMoveTowardsAngleTest, Smoke)
{
	constexpr double pi = ccm::numbers::pi_v<double>;

	static_assert(ccm::ext::move_towards_angle(0.0, pi / 2, pi / 4) == pi / 4);
	static_assert(ccm::ext::move_towards_angle(0.0, pi / 2, pi) == pi / 2);

	EXPECT_DOUBLE_EQ(ccm::ext::move_towards_angle(0.0, pi / 2, pi / 4), pi / 4);
	EXPECT_DOUBLE_EQ(ccm::ext::move_towards_angle(0.0, pi / 2, pi), pi / 2);
	EXPECT_DOUBLE_EQ(ccm::ext::move_towards_angle(1.5 * pi, 0.0, 0.25 * pi), 1.75 * pi);
}
