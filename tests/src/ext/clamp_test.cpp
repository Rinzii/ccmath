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

#include <ccmath/ext/clamp.hpp>

TEST(CcmathExtClampTest, Smoke)
{
	static_assert(ccm::ext::clamp(0.5) == 0.5);
	static_assert(ccm::ext::clamp(-0.25) == 0.0);
	static_assert(ccm::ext::clamp(1.25) == 1.0);
	static_assert(ccm::ext::clamp(5, 0, 10) == 5);
	static_assert(ccm::ext::clamp(-1, 0, 10) == 0);
	static_assert(ccm::ext::clamp(11, 0, 10) == 10);

	EXPECT_DOUBLE_EQ(ccm::ext::clamp(0.25, 0.0, 0.5), 0.25);
	EXPECT_DOUBLE_EQ((ccm::ext::clamp(1.5f, 0.0f, 1.0f)), 1.0f);
	EXPECT_EQ((ccm::ext::clamp<int>(3, 0, 10)), 3);
}
