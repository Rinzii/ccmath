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

#include <ccmath/ext/normalize.hpp>

TEST(CcmathExtNormalizeTest, Smoke)
{
	static_assert(ccm::ext::normalize(5.0, 0.0, 10.0) == 0.5);
	static_assert(ccm::ext::normalize(-5.0, 0.0, 10.0) == 0.0);
	static_assert(ccm::ext::normalize(15.0, 0.0, 10.0) == 1.0);
	static_assert(ccm::ext::safe::normalize(5.0, 5.0, 5.0) == 0.0);

	EXPECT_DOUBLE_EQ(ccm::ext::normalize(2.5, 0.0, 10.0), 0.25);
	EXPECT_DOUBLE_EQ(ccm::ext::safe::normalize(7.0, 3.0, 3.0), 0.0);
}
