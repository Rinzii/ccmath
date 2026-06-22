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

#include <ccmath/ext/move_towards.hpp>

TEST(CcmathExtMoveTowardsTest, Smoke)
{
	static_assert(ccm::ext::move_towards(0.0, 10.0, 3.0) == 3.0);
	static_assert(ccm::ext::move_towards(0.0, 10.0, 20.0) == 10.0);
	static_assert(ccm::ext::move_towards(10.0, 0.0, 3.0) == 7.0);

	EXPECT_DOUBLE_EQ(ccm::ext::move_towards(1.0, 1.5, 0.1), 1.1);
	EXPECT_DOUBLE_EQ(ccm::ext::move_towards(1.0, 0.5, 0.1), 0.9);
}
