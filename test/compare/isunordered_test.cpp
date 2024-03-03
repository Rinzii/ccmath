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
#include "ccmath/detail/compare/isunordered.hpp"

TEST(CcmathCompareTests, IsUnordered)
{
	EXPECT_EQ(ccm::isunordered(1.0, 1.0), std::isunordered(1.0, 1.0));
    EXPECT_EQ(ccm::isunordered(1.0, 0.0), std::isunordered(1.0, 0.0));
    EXPECT_EQ(ccm::isunordered(0.0, 1.0), std::isunordered(0.0, 1.0));
    EXPECT_EQ(ccm::isunordered(0.0, 0.0), std::isunordered(0.0, 0.0));
    EXPECT_EQ(ccm::isunordered(-1.0, 0.0), std::isunordered(-1.0, 0.0));
    EXPECT_EQ(ccm::isunordered(0.0, -1.0), std::isunordered(0.0, -1.0));
    EXPECT_EQ(ccm::isunordered(-1.0, -1.0), std::isunordered(-1.0, -1.0));

	// Check for NaN
	EXPECT_EQ(ccm::isunordered(std::numeric_limits<double>::quiet_NaN(), 1.0), std::isunordered(std::numeric_limits<double>::quiet_NaN(), 1.0));
	EXPECT_EQ(ccm::isunordered(1.0, std::numeric_limits<double>::quiet_NaN()), std::isunordered(1.0, std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(ccm::isunordered(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()), std::isunordered(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()));

}
