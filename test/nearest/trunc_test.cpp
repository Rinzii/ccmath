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
#include "ccmath/detail/nearest/trunc.hpp"

TEST(CcmathNearestTests, Trunc)
{
	EXPECT_EQ(ccm::trunc(1.0), std::trunc(1.0));
    EXPECT_EQ(ccm::trunc(1.5), std::trunc(1.5));
    EXPECT_EQ(ccm::trunc(1.9), std::trunc(1.9));
    EXPECT_EQ(ccm::trunc(-1.0), std::trunc(-1.0));
    EXPECT_EQ(ccm::trunc(-1.5), std::trunc(-1.5));
    EXPECT_EQ(ccm::trunc(-1.9), std::trunc(-1.9));
    EXPECT_EQ(ccm::trunc(std::numeric_limits<double>::infinity()), std::trunc(std::numeric_limits<double>::infinity()));
    EXPECT_EQ(ccm::trunc(-std::numeric_limits<double>::infinity()), std::trunc(-std::numeric_limits<double>::infinity()));
    EXPECT_TRUE(std::isnan(ccm::trunc(std::nan(""))));
    EXPECT_TRUE(std::isnan(ccm::trunc(-std::nan(""))));
    EXPECT_EQ(ccm::trunc(0.0), std::trunc(0.0));
    EXPECT_EQ(ccm::trunc(-0.0), std::trunc(-0.0));
    EXPECT_EQ(ccm::trunc(0.5), std::trunc(0.5));
    EXPECT_EQ(ccm::trunc(-0.5), std::trunc(-0.5));
    EXPECT_EQ(ccm::trunc(0.9), std::trunc(0.9));
    EXPECT_EQ(ccm::trunc(-0.9), std::trunc(-0.9));
    EXPECT_EQ(ccm::trunc(0.9999999999999999), std::trunc(0.9999999999999999));
    EXPECT_EQ(ccm::trunc(-0.9999999999999999), std::trunc(-0.9999999999999999));
    EXPECT_EQ(ccm::trunc(1.0000000000000001), std::trunc(1.0000000000000001));
    EXPECT_EQ(ccm::trunc(-1.0000000000000001), std::trunc(-1.0000000000000001));

	// Test with float using std::truncf and ccm::truncf
	EXPECT_EQ(ccm::truncf(1.0f), std::truncf(1.0f));
    EXPECT_EQ(ccm::truncf(1.5f), std::truncf(1.5f));
    EXPECT_EQ(ccm::truncf(1.9f), std::truncf(1.9f));
    EXPECT_EQ(ccm::truncf(-1.0f), std::truncf(-1.0f));
    EXPECT_EQ(ccm::truncf(-1.5f), std::truncf(-1.5f));
    EXPECT_EQ(ccm::truncf(-1.9f), std::truncf(-1.9f));
    EXPECT_EQ(ccm::truncf(std::numeric_limits<float>::infinity()), std::truncf(std::numeric_limits<float>::infinity()));
    EXPECT_EQ(ccm::truncf(-std::numeric_limits<float>::infinity()), std::truncf(-std::numeric_limits<float>::infinity()));
    EXPECT_TRUE(std::isnan(ccm::truncf(std::nanf(""))));
    EXPECT_TRUE(std::isnan(ccm::truncf(-std::nanf(""))));
    EXPECT_EQ(ccm::truncf(0.0f), std::truncf(0.0f));
    EXPECT_EQ(ccm::truncf(-0.0f), std::truncf(-0.0f));
    EXPECT_EQ(ccm::truncf(0.5f), std::truncf(0.5f));
    EXPECT_EQ(ccm::truncf(-0.5f), std::truncf(-0.5f));
    EXPECT_EQ(ccm::truncf(0.9f), std::truncf(0.9f));
    EXPECT_EQ(ccm::truncf(-0.9f), std::truncf(-0.9f));
    EXPECT_EQ(ccm::truncf(0.9999999999999999f), std::truncf(0.9999999999999999f));
    EXPECT_EQ(ccm::truncf(-0.9999999999999999f), std::truncf(-0.9999999999999999f));


	// Test with long double using std::truncl and ccm::truncl
	EXPECT_EQ(ccm::truncl(1.0l), std::truncl(1.0l));
    EXPECT_EQ(ccm::truncl(1.5l), std::truncl(1.5l));
    EXPECT_EQ(ccm::truncl(1.9l), std::truncl(1.9l));
    EXPECT_EQ(ccm::truncl(-1.0l), std::truncl(-1.0l));
    EXPECT_EQ(ccm::truncl(-1.5l), std::truncl(-1.5l));
    EXPECT_EQ(ccm::truncl(-1.9l), std::truncl(-1.9l));
    EXPECT_EQ(ccm::truncl(std::numeric_limits<long double>::infinity()), std::truncl(std::numeric_limits<long double>::infinity()));
    EXPECT_EQ(ccm::truncl(-std::numeric_limits<long double>::infinity()), std::truncl(-std::numeric_limits<long double>::infinity()));
    EXPECT_TRUE(std::isnan(ccm::truncl(std::nanl(""))));
    EXPECT_TRUE(std::isnan(ccm::truncl(-std::nanl(""))));
    EXPECT_EQ(ccm::truncl(0.0l), std::truncl(0.0l));
    EXPECT_EQ(ccm::truncl(-0.0l), std::truncl(-0.0l));
    EXPECT_EQ(ccm::truncl(0.5l), std::truncl(0.5l));
    EXPECT_EQ(ccm::truncl(-0.5l), std::truncl(-0.5l));
    EXPECT_EQ(ccm::truncl(0.9l), std::truncl(0.9l));
    EXPECT_EQ(ccm::truncl(-0.9l), std::truncl(-0.9l));
    EXPECT_EQ(ccm::truncl(0.9999999999999999l), std::truncl(0.9999999999999999l));
    EXPECT_EQ(ccm::truncl(-0.9999999999999999l), std::truncl(-0.9999999999999999l));
}
