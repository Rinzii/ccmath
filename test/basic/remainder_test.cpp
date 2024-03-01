/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <gtest/gtest.h>

#include <ccmath/detail/basic/remainder.hpp>
#include <cmath>


TEST(CcmathBasicTests, Remainder)
{
	EXPECT_EQ(ccm::remainder(1.0, 1.0), std::remainder(1.0, 1.0));
    EXPECT_EQ(std::isnan(ccm::remainder(1.0, 0.0)), std::isnan(std::remainder(1.0, 0.0)));
    EXPECT_EQ(std::isnan(ccm::remainder(0.0, 1.0)), std::isnan(std::remainder(0.0, 1.0)));
    EXPECT_EQ(std::isnan(ccm::remainder(0.0, 0.0)), std::isnan(std::remainder(0.0, 0.0)));
    EXPECT_EQ(ccm::remainder(-1.0, 1.0), std::remainder(-1.0, 1.0));
    EXPECT_EQ(ccm::remainder(1.0, -1.0), std::remainder(1.0, -1.0));
    EXPECT_EQ(ccm::remainder(-1.0, -1.0), std::remainder(-1.0, -1.0));
    EXPECT_EQ(std::isnan(ccm::remainder(-1.0, 0.0)), std::isnan(std::remainder(-1.0, 0.0)));


}
