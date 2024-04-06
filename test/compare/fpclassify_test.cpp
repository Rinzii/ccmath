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
#include <ccmath/ccmath.hpp>

TEST(CcmathCompareTests, Fpclassify)
{
	// Test that fpclassify is static_assert-able
	// No implementation should be lower than -100.
	// Since we are only testing that fpclassify is static_assert-able, we don't need to test the value it returns.
	static_assert(ccm::fpclassify(1.0) > -100, "fpclassify has failed testing that it is static_assert-able!");

	EXPECT_EQ(ccm::fpclassify(1.0), std::fpclassify(1.0));
    EXPECT_EQ(ccm::fpclassify(0.0), std::fpclassify(0.0));
    EXPECT_EQ(ccm::fpclassify(-1.0), std::fpclassify(-1.0));
    EXPECT_EQ(ccm::fpclassify(std::numeric_limits<double>::infinity()), std::fpclassify(std::numeric_limits<double>::infinity()));
    EXPECT_EQ(ccm::fpclassify(-std::numeric_limits<double>::infinity()), std::fpclassify(-std::numeric_limits<double>::infinity()));
    EXPECT_EQ(ccm::fpclassify(std::numeric_limits<double>::quiet_NaN()), std::fpclassify(std::numeric_limits<double>::quiet_NaN()));
    EXPECT_EQ(ccm::fpclassify(std::numeric_limits<double>::signaling_NaN()), std::fpclassify(std::numeric_limits<double>::signaling_NaN()));
    EXPECT_EQ(ccm::fpclassify(std::numeric_limits<double>::denorm_min()), std::fpclassify(std::numeric_limits<double>::denorm_min()));
    EXPECT_EQ(ccm::fpclassify(std::numeric_limits<double>::min()), std::fpclassify(std::numeric_limits<double>::min()));
    EXPECT_EQ(ccm::fpclassify(std::numeric_limits<double>::max()), std::fpclassify(std::numeric_limits<double>::max()));
    EXPECT_EQ(ccm::fpclassify(std::numeric_limits<double>::epsilon()), std::fpclassify(std::numeric_limits<double>::epsilon()));
    EXPECT_EQ(ccm::fpclassify(std::numeric_limits<double>::round_error()), std::fpclassify(std::numeric_limits<double>::round_error()));
    EXPECT_EQ(ccm::fpclassify(std::numeric_limits<double>::infinity()), std::fpclassify(std::numeric_limits<double>::infinity()));
    EXPECT_EQ(ccm::fpclassify(std::numeric_limits<double>::min()), std::fpclassify(std::numeric_limits<double>::min()));
    EXPECT_EQ(ccm::fpclassify(std::numeric_limits<double>::max()), std::fpclassify(std::numeric_limits<double>::max()));
    EXPECT_EQ(ccm::fpclassify(std::numeric_limits<double>::epsilon()), std::fpclassify(std::numeric_limits<double>::epsilon()));
    EXPECT_EQ(ccm::fpclassify(std::numeric_limits<double>::round_error()), std::fpclassify(std::numeric_limits<double>::round_error()));
}
