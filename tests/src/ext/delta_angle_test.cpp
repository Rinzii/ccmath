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
#include <ccmath/math/numbers.hpp>

TEST(CcmathExtDeltaAngleTest, Smoke)
{
	constexpr double pi = ccm::numbers::pi_v<double>;

	static_assert(ccm::ext::delta_angle(0.0, pi / 2) == pi / 2);
	static_assert(ccm::ext::delta_angle(0.0, 2.0 * pi) == 0.0);
	static_assert(ccm::ext::delta_angle(0.0, pi) == pi);

	EXPECT_DOUBLE_EQ(ccm::ext::delta_angle(0.0, pi), pi);
	EXPECT_DOUBLE_EQ(ccm::ext::delta_angle(0.0, 1.5 * pi), -0.5 * pi);
}
