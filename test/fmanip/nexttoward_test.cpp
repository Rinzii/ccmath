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

#include "ccmath/ccmath.hpp"

#include <cmath>
#include <limits>

TEST(CcmathFmanipTests, Nexttoward)
{
	EXPECT_EQ(ccm::nexttoward(1.0, 2.0), std::nexttoward(1.0, 2.0));

}

