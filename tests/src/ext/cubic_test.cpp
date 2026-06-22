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

#include <ccmath/ext/cubic.hpp>

TEST(CcmathExtCubicTest, Smoke)
{
	static_assert(ccm::ext::cubic(0.0, 1.0, 2.0, 3.0, 0.0) == 1.0);
	static_assert(ccm::ext::cubic(0.0, 1.0, 2.0, 3.0, 1.0) == 2.0);

	EXPECT_DOUBLE_EQ(ccm::ext::cubic(0.0, 1.0, 2.0, 3.0, 0.5), 1.5);
}
