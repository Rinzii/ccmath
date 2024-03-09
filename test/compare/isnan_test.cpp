/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include "ccmath/detail/compare/isnan.hpp"

TEST(CcmathCompareTests, IsNan)
{
	// Test that isnan is static_assert-able
    static_assert(ccm::isnan(1.0) == false, "isnan has failed testing that it is static_assert-able!");

	EXPECT_EQ(ccm::isnan(1.0), std::isnan(1.0));
	EXPECT_EQ(ccm::isnan(0.0), std::isnan(0.0));
	EXPECT_EQ(ccm::isnan(-1.0), std::isnan(-1.0));
	EXPECT_EQ(ccm::isnan(std::numeric_limits<double>::infinity()), std::isnan(std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::isnan(-std::numeric_limits<double>::infinity()), std::isnan(-std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::isnan(std::numeric_limits<double>::quiet_NaN()), std::isnan(std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(ccm::isnan(std::numeric_limits<double>::signaling_NaN()), std::isnan(std::numeric_limits<double>::signaling_NaN()));

}
