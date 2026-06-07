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

#include <cstddef>

#include <ccmath/ext/align.hpp>
#include <ccmath/ext/delta_angle.hpp>
#include <ccmath/ext/is_power_of_two.hpp>
#include <ccmath/ext/lerp_angle.hpp>

TEST(CcmathExtTests, IsPowerOfTwo)
{
	static_assert(ccm::ext::is_power_of_two(1U));
	static_assert(ccm::ext::is_power_of_two(8U));
	static_assert(!ccm::ext::is_power_of_two(0U));
	static_assert(!ccm::ext::is_power_of_two(6U));
	static_assert(ccm::ext::safe::is_power_of_two(4));
	static_assert(!ccm::ext::safe::is_power_of_two(-4));

	EXPECT_TRUE(ccm::ext::is_power_of_two(16U));
	EXPECT_FALSE(ccm::ext::is_power_of_two(15U));
}

TEST(CcmathExtTests, DeltaAngle)
{
	static_assert(ccm::ext::delta_angle(0.0, 90.0) == 90.0);
	static_assert(ccm::ext::delta_angle(350.0, 10.0) == 20.0);

	EXPECT_DOUBLE_EQ(ccm::ext::delta_angle(0.0, 180.0), 180.0);
	EXPECT_DOUBLE_EQ(ccm::ext::lerp_angle(0.0, 90.0, 0.5), 45.0);
}

TEST(CcmathExtTests, Align)
{
	static_assert(ccm::ext::align_up<unsigned, 8>(9) == 16);
	static_assert(ccm::ext::align_down<unsigned, 8>(9) == 8);

	EXPECT_EQ((ccm::ext::align_up<std::size_t, 4>(5)), 8U);
}
