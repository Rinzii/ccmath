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

#include <ccmath/ext/ping_pong.hpp>

TEST(CcmathExtPingPongTest, Smoke)
{
	static_assert(ccm::ext::ping_pong(1.0, 0.0) == 0.0);

	EXPECT_DOUBLE_EQ(ccm::ext::ping_pong(0.0, 1.0), 0.0);
	EXPECT_DOUBLE_EQ(ccm::ext::ping_pong(0.5, 1.0), 0.5);
	EXPECT_DOUBLE_EQ(ccm::ext::ping_pong(1.5, 1.0), 0.5);
	EXPECT_DOUBLE_EQ(ccm::ext::ping_pong(2.0, 1.0), 0.0);
}
