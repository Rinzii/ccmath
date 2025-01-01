/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <ccmath/ccmath.hpp>
#include <gtest/gtest.h>
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
	EXPECT_EQ(ccm::fmax(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()),
			  std::fmax(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::fmax(-std::numeric_limits<double>::infinity(), 1.0), std::fmax(-std::numeric_limits<double>::infinity(), 1.0));
	EXPECT_EQ(ccm::fmax(1.0, -std::numeric_limits<double>::infinity()), std::fmax(1.0, -std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::fmax(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()),
			  std::fmax(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::fmax(std::numeric_limits<double>::quiet_NaN(), 1.0), std::fmax(std::numeric_limits<double>::quiet_NaN(), 1.0));
	EXPECT_EQ(ccm::fmax(1.0, std::numeric_limits<double>::quiet_NaN()), std::fmax(1.0, std::numeric_limits<double>::quiet_NaN()));

	bool const CcmFmaxWhenPassedPositiveNanReturnsNan =
		std::isnan(ccm::fmax(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()));
	bool const StdFmaxWhenPassedPositiveNanReturnsNan =
		std::isnan(std::fmax(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(CcmFmaxWhenPassedPositiveNanReturnsNan, StdFmaxWhenPassedPositiveNanReturnsNan);

	bool const CcmFmaxWhenPassedNegativeNanReturnsNan =
		std::isnan(ccm::fmax(-std::numeric_limits<double>::quiet_NaN(), -std::numeric_limits<double>::quiet_NaN()));
	bool const StdFmaxWhenPassedNegativeNanReturnsNan =
		std::isnan(std::fmax(-std::numeric_limits<double>::quiet_NaN(), -std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(CcmFmaxWhenPassedNegativeNanReturnsNan, StdFmaxWhenPassedNegativeNanReturnsNan);
}
