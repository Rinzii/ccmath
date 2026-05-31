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

TEST(CcmathFmanipTests, ModfBasic)
{
	double iptr{};

	ccm::test::ExpectModfMatchesStd(3.75);
	ccm::modf(3.75, &iptr);
	EXPECT_DOUBLE_EQ(iptr, 3.0);

	ccm::test::ExpectModfMatchesStd(-3.75);
	ccm::modf(-3.75, &iptr);
	EXPECT_DOUBLE_EQ(iptr, -3.0);
}

TEST(CcmathFmanipTests, ModfFloat)
{
	float iptr{};

	ccm::test::ExpectModfMatchesStd(2.25F);
	ccm::modf(2.25F, &iptr);
	EXPECT_FLOAT_EQ(iptr, 2.0F);
}

TEST(CcmathFmanipTests, ModfIntegerValue)
{
	double iptr{};

	ccm::test::ExpectModfMatchesStd(4.0);
	ccm::modf(4.0, &iptr);
	EXPECT_DOUBLE_EQ(iptr, 4.0);
	EXPECT_EQ(ccm::modf(4.0, &iptr), 0.0);
	ccm::test::ExpectModfMatchesStd(-4.0);
}

TEST(CcmathFmanipTests, ModfSpecialValues)
{
	double iptr{};

	ccm::test::ExpectModfMatchesStd(std::numeric_limits<double>::quiet_NaN());
	ccm::test::ExpectModfMatchesStd(std::numeric_limits<double>::infinity());
	ccm::test::ExpectModfMatchesStd(0.0);
}
