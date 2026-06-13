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

#include <ccmath/ext/radians.hpp>
#include <ccmath/math/numbers.hpp>

TEST(CcmathExtRadiansTest, Smoke)
{
	static_assert(ccm::ext::radians(180.0) == ccm::numbers::pi_v<double>);

	EXPECT_DOUBLE_EQ(ccm::ext::radians(90.0), ccm::numbers::pi_v<double> / 2.0);
}
