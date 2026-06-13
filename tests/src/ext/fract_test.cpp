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

#include <ccmath/ext/fract.hpp>

TEST(CcmathExtFractTest, Smoke)
{
	static_assert(ccm::ext::fract(3.75) == 0.75);
	static_assert(ccm::ext::fract(-1.25) == 0.75);

	EXPECT_DOUBLE_EQ(ccm::ext::fract(2.5), 0.5);
	EXPECT_DOUBLE_EQ(ccm::ext::fract(-2.5), 0.5);
}
