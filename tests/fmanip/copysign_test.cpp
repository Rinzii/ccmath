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

#include <cmath>
#include <limits>
#include "ccmath/ccmath.hpp"
#include "utils/std_compare.hpp"

TEST(CcmathFmanipTests, Copysign)
{
	ccm::test::ExpectSameAsStd(ccm::copysign(1.0, +2.0), std::copysign(1.0, +2.0));
	ccm::test::ExpectSameAsStd(ccm::copysign(1.0, -2.0), std::copysign(1.0, -2.0));
	ccm::test::ExpectSameAsStd(ccm::copysign(std::numeric_limits<double>::infinity(), -2.0), std::copysign(std::numeric_limits<double>::infinity(), -2.0));
	ccm::test::ExpectFpEq(ccm::copysign(std::numeric_limits<double>::quiet_NaN(), -2.0), std::copysign(std::numeric_limits<double>::quiet_NaN(), -2.0));

	// TODO: Add more tests
}

