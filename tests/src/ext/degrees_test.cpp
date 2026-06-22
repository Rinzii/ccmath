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

#include <ccmath/ext/degrees.hpp>
#include <ccmath/math/numbers.hpp>

TEST(CcmathExtDegreesTest, Smoke)
{
	static_assert(ccm::ext::degrees(ccm::numbers::pi_v<double>) == 180.0);

	EXPECT_DOUBLE_EQ(ccm::ext::degrees(ccm::numbers::pi_v<double> / 2.0), 90.0);
}
