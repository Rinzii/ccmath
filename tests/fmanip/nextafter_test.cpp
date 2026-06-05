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

TEST(CcmathFmanipTests, Nextafter)
{
	EXPECT_EQ(ccm::nextafter(1.0, 2.0), std::nextafter(1.0, 2.0));
	EXPECT_EQ(ccm::nextafter(-0.0, 1.0), std::nextafter(-0.0, 1.0));
	EXPECT_TRUE(std::isnan(ccm::nextafter(std::numeric_limits<double>::quiet_NaN(), 1.0)));
}

TEST(CcmathFmanipTests, NextupMatchesStdNextafterInfinity)
{
	EXPECT_EQ(ccm::nextup(1.0), std::nextafter(1.0, std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::nextupf(1.0F), std::nextafter(1.0F, std::numeric_limits<float>::infinity()));
	EXPECT_EQ(ccm::nextupl(1.0L), std::nextafter(1.0L, std::numeric_limits<long double>::infinity()));
}

TEST(CcmathFmanipTests, NextdownMatchesStdNextafterNegativeInfinity)
{
	EXPECT_EQ(ccm::nextdown(1.0), std::nextafter(1.0, -std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::nextdownf(1.0F), std::nextafter(1.0F, -std::numeric_limits<float>::infinity()));
	EXPECT_EQ(ccm::nextdownl(1.0L), std::nextafter(1.0L, -std::numeric_limits<long double>::infinity()));
}
