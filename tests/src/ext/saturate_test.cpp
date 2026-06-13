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

#include <ccmath/ext/saturate.hpp>

TEST(CcmathExtSaturateTest, Smoke)
{
	static_assert(ccm::ext::saturate(0.5) == 0.5);
	static_assert(ccm::ext::saturate(-0.25) == 0.0);
	static_assert(ccm::ext::saturate(1.25) == 1.0);

	EXPECT_DOUBLE_EQ(ccm::ext::saturate(2.0), 1.0);
	EXPECT_DOUBLE_EQ(ccm::ext::saturate(-1.0), 0.0);
}
