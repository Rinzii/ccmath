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

#include <ccmath/ext/step.hpp>

TEST(CcmathExtStepTest, Smoke)
{
	static_assert(ccm::ext::step(5, 3) == 0);
	static_assert(ccm::ext::step(5, 5) == 1);
	static_assert(ccm::ext::step(5.0, 6.0) == 1.0);
	static_assert(ccm::ext::step(5.0, 4.0) == 0.0);

	EXPECT_EQ(ccm::ext::step(10, 9), 0);
	EXPECT_EQ(ccm::ext::step(10, 10), 1);
}
