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

#include <ccmath/ext/is_power_of_two.hpp>

TEST(CcmathExtIsPowerOfTwoTest, Smoke)
{
	static_assert(ccm::ext::is_power_of_two(1U));
	static_assert(ccm::ext::is_power_of_two(8U));
	static_assert(!ccm::ext::is_power_of_two(0U));
	static_assert(!ccm::ext::is_power_of_two(6U));
	static_assert(ccm::ext::safe::is_power_of_two(4));
	static_assert(!ccm::ext::safe::is_power_of_two(-4));
	static_assert(ccm::ext::ispow2(16));
	static_assert(ccm::ext::safe::ispow2(32));

	EXPECT_TRUE(ccm::ext::is_power_of_two(16U));
	EXPECT_FALSE(ccm::ext::is_power_of_two(15U));
	EXPECT_TRUE(ccm::ext::ispow2(64U));
}
