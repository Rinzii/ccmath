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
#include <limits>


TEST(CcmathBasicTests, Fmax)
{
	// Test that fmax works with static_assert
	static_assert(ccm::max(1.0, 2.0) == 2, "max has failed testing that it is static_assert-able!");

    EXPECT_EQ(ccm::fmax(1.0, 2.0), std::fmax(1.0, 2.0));
    EXPECT_EQ(ccm::fmax(2.0, 1.0), std::fmax(2.0, 1.0));
    EXPECT_EQ(ccm::fmax(1.0, 1.0), std::fmax(1.0, 1.0));
    EXPECT_EQ(ccm::fmax(std::numeric_limits<double>::infinity(), 1.0), std::fmax(std::numeric_limits<double>::infinity(), 1.0));
	EXPECT_EQ(ccm::fmax(1.0, std::numeric_limits<double>::infinity()), std::fmax(1.0, std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::fmax(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()), std::fmax(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::fmax(-std::numeric_limits<double>::infinity(), 1.0), std::fmax(-std::numeric_limits<double>::infinity(), 1.0));
	EXPECT_EQ(ccm::fmax(1.0, -std::numeric_limits<double>::infinity()), std::fmax(1.0, -std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::fmax(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()), std::fmax(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::fmax(std::numeric_limits<double>::quiet_NaN(), 1.0), std::fmax(std::numeric_limits<double>::quiet_NaN(), 1.0));
	EXPECT_EQ(ccm::fmax(1.0, std::numeric_limits<double>::quiet_NaN()), std::fmax(1.0, std::numeric_limits<double>::quiet_NaN()));

	bool CcmFmaxWhenPassedPositiveNanReturnsNan = std::isnan(ccm::fmax(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()));
	bool StdFmaxWhenPassedPositiveNanReturnsNan = std::isnan(std::fmax(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(CcmFmaxWhenPassedPositiveNanReturnsNan, StdFmaxWhenPassedPositiveNanReturnsNan);

	bool CcmFmaxWhenPassedNegativeNanReturnsNan = std::isnan(ccm::fmax(-std::numeric_limits<double>::quiet_NaN(), -std::numeric_limits<double>::quiet_NaN()));
	bool StdFmaxWhenPassedNegativeNanReturnsNan = std::isnan(std::fmax(-std::numeric_limits<double>::quiet_NaN(), -std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(CcmFmaxWhenPassedNegativeNanReturnsNan, StdFmaxWhenPassedNegativeNanReturnsNan);


}
