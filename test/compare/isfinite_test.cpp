/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <ccmath/ccmath.hpp>
#include <gtest/gtest.h>
#include <cmath>
#include <limits>

// TODO: add more tests for isfinite

TEST(CcmathCompareTests, IsFinite)
{
	// test isfinite is static_assert-able
	static_assert(ccm::isfinite(1.0), "isfinite has failed testing that it is static_assert-able!");

	EXPECT_EQ(ccm::isfinite(1.0), std::isfinite(1.0));
}
