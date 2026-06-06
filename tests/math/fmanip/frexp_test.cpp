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

TEST(CcmathFmanipTests, FrexpBasic)
{
	int exp{};

	ccm::test::ExpectFrexpMatchesStd(12.0);
	ccm::frexp(12.0, exp);
	EXPECT_EQ(exp, 4);

	ccm::test::ExpectFrexpMatchesStd(-12.0);
	ccm::frexp(-12.0, exp);
	EXPECT_EQ(exp, 4);

	ccm::test::ExpectFrexpMatchesStd(0.0);
	ccm::frexp(0.0, exp);
	EXPECT_EQ(exp, 0);
	ccm::test::ExpectFrexpMatchesStd(-0.0);
	ccm::frexp(-0.0, exp);
	EXPECT_EQ(exp, 0);
}

TEST(CcmathFmanipTests, FrexpPointerOverloadMatchesStd)
{
	int exp{};
	int std_exp{};

	const double ccm_value = ccm::frexp(12.0, &exp);
	const double std_value = std::frexp(12.0, &std_exp);

	EXPECT_EQ(ccm_value, std_value);
	EXPECT_EQ(exp, std_exp);
}

TEST(CcmathFmanipTests, FrexpFloat)
{
	int exp{};

	ccm::test::ExpectFrexpMatchesStd(3.5F);
	ccm::frexp(3.5F, exp);
	EXPECT_EQ(exp, 2);

	ccm::test::ExpectFrexpMatchesStd(0.125F);
	ccm::frexp(0.125F, exp);
	EXPECT_EQ(exp, -2);
}

TEST(CcmathFmanipTests, FrexpSpecialValues)
{
	ccm::test::ExpectFrexpMatchesStd(std::numeric_limits<double>::quiet_NaN());
	ccm::test::ExpectFrexpMatchesStd(std::numeric_limits<double>::infinity());
	ccm::test::ExpectFrexpMatchesStd(-std::numeric_limits<double>::infinity());
}

TEST(CcmathFmanipTests, FrexpSubnormal)
{
	int exp{};
	int std_exp{};

	const double x = std::numeric_limits<double>::denorm_min();
	ccm::test::ExpectSameAsStd(ccm::frexp(x, exp), std::frexp(x, &std_exp));
	EXPECT_EQ(exp, std_exp);
}
