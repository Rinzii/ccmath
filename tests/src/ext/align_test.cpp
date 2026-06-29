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

#include <ccmath/ext/align.hpp>

#include <cstddef>

TEST(CcmathExtAlignTest, Smoke)
{
	static_assert(ccm::ext::align_up<unsigned, 8>(9) == 16);
	static_assert(ccm::ext::align_down<unsigned, 8>(9) == 8);
	static_assert(ccm::ext::align_up<unsigned, 8>(16) == 16);
	static_assert(ccm::ext::align_down<unsigned, 8>(16) == 16);

	EXPECT_EQ((ccm::ext::align_up<std::size_t, 4>(5)), 8U);
	EXPECT_EQ((ccm::ext::align_down<std::size_t, 4>(7)), 4U);
}
