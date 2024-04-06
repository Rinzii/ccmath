/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <gtest/gtest.h>

#include <ccmath/ccmath.hpp>
#include <cmath>


TEST(CcmathBasicTests, Remainder)
{
	    static_assert(ccm::remainder(1.0, 1.0) == 0.0, "remainder has failed testing that it is static_assert-able!");

	EXPECT_EQ(ccm::remainder(1.0, 1.0), std::remainder(1.0, 1.0));
    EXPECT_EQ(std::isnan(ccm::remainder(1.0, 0.0)), std::isnan(std::remainder(1.0, 0.0)));
    EXPECT_EQ(std::isnan(ccm::remainder(0.0, 1.0)), std::isnan(std::remainder(0.0, 1.0)));
    EXPECT_EQ(std::isnan(ccm::remainder(0.0, 0.0)), std::isnan(std::remainder(0.0, 0.0)));
    EXPECT_EQ(ccm::remainder(-1.0, 1.0), std::remainder(-1.0, 1.0));
    EXPECT_EQ(ccm::remainder(1.0, -1.0), std::remainder(1.0, -1.0));
    EXPECT_EQ(ccm::remainder(-1.0, -1.0), std::remainder(-1.0, -1.0));
    EXPECT_EQ(std::isnan(ccm::remainder(-1.0, 0.0)), std::isnan(std::remainder(-1.0, 0.0)));


}
