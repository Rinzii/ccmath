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

#include <ccmath/ext/remap.hpp>

TEST(CcmathExtRemapTest, Smoke)
{
	static_assert(ccm::ext::remap(0.0, 10.0, 100.0, 200.0, 5.0) == 150.0);
	static_assert(ccm::ext::safe::remap(5.0, 5.0, 1.0, 2.0, 7.0) == 1.0);

	EXPECT_DOUBLE_EQ(ccm::ext::remap(0.0, 1.0, 10.0, 20.0, 0.5), 15.0);
	EXPECT_DOUBLE_EQ(ccm::ext::safe::remap(2.0, 2.0, 0.0, 1.0, 9.0), 0.0);
}
