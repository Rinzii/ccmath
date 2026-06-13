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

#include <ccmath/ext/rcp.hpp>

TEST(CcmathExtRcpTest, Smoke)
{
	static_assert(ccm::ext::rcp(4.0) == 0.25);
	static_assert(ccm::ext::rcp(0.5) == 2.0);

	EXPECT_DOUBLE_EQ(ccm::ext::rcp(10.0), 0.1);
	EXPECT_FLOAT_EQ(ccm::ext::rcp(2.0f), 0.5f);
}
