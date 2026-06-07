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

TEST(CcmathFmanipTests, ScalbnDouble)
{
	EXPECT_EQ(ccm::scalbn(7.0, -4), std::scalbn(7.0, -4));
	EXPECT_EQ(ccm::scalbn(1.0, -1074), std::scalbn(1.0, -1074));
	EXPECT_EQ(ccm::scalbn(std::nextafter(1, 0), 1024), std::scalbn(std::nextafter(1, 0), 1024));
	EXPECT_EQ(ccm::scalbn(-0.0, 10), std::scalbn(-0.0, 10));
	EXPECT_EQ(ccm::scalbn(-std::numeric_limits<double>::infinity(), -1), std::scalbn(-std::numeric_limits<double>::infinity(), -1));
	EXPECT_EQ(ccm::scalbn(1.0, 1024), std::scalbn(1.0, 1024));

}

TEST(CcmathFmanipTests, ScalbnFloat)
{
	EXPECT_EQ(ccm::scalbn(7.0F, -4), std::scalbn(7.0F, -4));
	EXPECT_EQ(ccm::scalbn(1.0F, -1074), std::scalbn(1.0F, -1074));
	EXPECT_EQ(ccm::scalbn(std::nextafter(1, 0), 1024), std::scalbn(std::nextafter(1, 0), 1024));
	EXPECT_EQ(ccm::scalbn(-0.0F, 10), std::scalbn(-0.0F, 10));
	EXPECT_EQ(ccm::scalbn(-std::numeric_limits<float>::infinity(), -1), std::scalbn(-std::numeric_limits<float>::infinity(), -1));
	EXPECT_EQ(ccm::scalbn(1.0F, 1024), std::scalbn(1.0F, 1024));

}

TEST(CcmathFmanipTests, ScalbnLongDouble)
{
	// TODO(IanP): scalbn long double tests need native ld80/ld128 bit access. Delegates to double for now.
	/*
	EXPECT_EQ(ccm::scalbn(7.0L, -4), std::scalbn(7.0L, -4));
    EXPECT_EQ(ccm::scalbn(1.0L, -1074), std::scalbn(1.0L, -1074));
    EXPECT_EQ(ccm::scalbn(std::nextafter(1, 0), 1024), std::scalbn(std::nextafter(1, 0), 1024));
    EXPECT_EQ(ccm::scalbn(-0.0L, 10), std::scalbn(-0.0L, 10));
    EXPECT_EQ(ccm::scalbn(-std::numeric_limits<long double>::infinity(), -1), std::scalbn(-std::numeric_limits<long double>::infinity(), -1));
    EXPECT_EQ(ccm::scalbn(1.0L, 1024), std::scalbn(1.0L, 1024));
    */
}

TEST(CcmathFmanipTests, ScalblnMatchesStd)
{
	EXPECT_EQ(ccm::scalbln(7.0, -4L), std::scalbln(7.0, -4L));
	EXPECT_EQ(ccm::scalblnf(7.0F, -4L), std::scalbln(7.0F, -4L));
	EXPECT_EQ(ccm::scalblnl(7.0L, -4L), std::scalbln(7.0L, -4L));
}
