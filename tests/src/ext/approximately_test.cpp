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

#include <ccmath/ext/approximately.hpp>

TEST(CcmathExtApproximatelyTest, Smoke)
{
	static_assert(ccm::ext::approximately(1.0, 1.0, 0.001));
	static_assert(!ccm::ext::approximately(1.0, 1.1, 0.001));

	EXPECT_TRUE(ccm::ext::approximately(1.0, 1.0005, 0.001));
	EXPECT_FALSE(ccm::ext::approximately(1.0, 1.01, 0.001));
}
