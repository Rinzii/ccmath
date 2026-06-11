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

#include <ccmath/ext/mix.hpp>

TEST(CcmathExtMixTest, Smoke)
{
	static_assert(ccm::ext::mix(0.0, 10.0, 0.25) == 2.5);
	static_assert(ccm::ext::mix(0.0, 10.0, 0.0) == 0.0);
	static_assert(ccm::ext::mix(0.0, 10.0, 1.0) == 10.0);

	EXPECT_DOUBLE_EQ(ccm::ext::mix(1.0, 3.0, 0.5), 2.0);
	EXPECT_DOUBLE_EQ(ccm::ext::mix(0, 100, 0.25), 25.0);
}
