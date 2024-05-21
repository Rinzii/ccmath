/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <gtest/gtest.h>

#include "ccmath/ccmath.hpp"

#include <cmath>
#include <limits>

// TODO: Find a way to test all of the different ways the function may behave internally to work based on the provided compiler.

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
	// Google Test is apparently incapable of comparing NaNs or I do not know enough about gtest to find a solution. I've though personally validated that ccm::signbit handles NaNs correctly
	//EXPECT_EQ(ccm::trunc(std::numeric_limits<double>::quiet_NaN()), std::trunc(std::numeric_limits<double>::quiet_NaN()));
	//EXPECT_EQ(ccm::trunc(-std::numeric_limits<double>::quiet_NaN()), std::trunc(-std::numeric_limits<double>::quiet_NaN()));

	// The standard when passed +NaN returns +NaN, and when passed -NaN returns -NaN
	bool isCcmPositiveNanPositive = (std::signbit(ccm::trunc(std::numeric_limits<double>::quiet_NaN())) == false && std::isnan(ccm::trunc(std::numeric_limits<double>::quiet_NaN())) == true); // NOLINT
	bool isStdPositiveNanPositive = (std::signbit(std::trunc(std::numeric_limits<double>::quiet_NaN())) == false && std::isnan(std::trunc(std::numeric_limits<double>::quiet_NaN())) == true); // NOLINT
	EXPECT_EQ(isCcmPositiveNanPositive, isStdPositiveNanPositive);


	bool isCcmNegativeNanNegative = (std::signbit(ccm::trunc(-std::numeric_limits<double>::quiet_NaN())) == true && std::isnan(ccm::trunc(-std::numeric_limits<double>::quiet_NaN())) == true); // NOLINT
	bool isStdNegativeNanNegative = (std::signbit(std::trunc(-std::numeric_limits<double>::quiet_NaN())) == true && std::isnan(std::trunc(-std::numeric_limits<double>::quiet_NaN())) == true); // NOLINT
	EXPECT_EQ(isCcmNegativeNanNegative, isStdNegativeNanNegative);


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
	EXPECT_EQ(ccm::truncf(1.0F), std::truncf(1.0F));
    EXPECT_EQ(ccm::truncf(1.5F), std::truncf(1.5F));
    EXPECT_EQ(ccm::truncf(1.9F), std::truncf(1.9F));
    EXPECT_EQ(ccm::truncf(-1.0F), std::truncf(-1.0F));
    EXPECT_EQ(ccm::truncf(-1.5F), std::truncf(-1.5F));
    EXPECT_EQ(ccm::truncf(-1.9F), std::truncf(-1.9F));
    EXPECT_EQ(ccm::truncf(std::numeric_limits<float>::infinity()), std::truncf(std::numeric_limits<float>::infinity()));
    EXPECT_EQ(ccm::truncf(-std::numeric_limits<float>::infinity()), std::truncf(-std::numeric_limits<float>::infinity()));
    EXPECT_TRUE(std::isnan(ccm::truncf(std::nanf(""))));
    EXPECT_TRUE(std::isnan(ccm::truncf(-std::nanf(""))));
    EXPECT_EQ(ccm::truncf(0.0F), std::truncf(0.0F));
    EXPECT_EQ(ccm::truncf(-0.0F), std::truncf(-0.0F));
    EXPECT_EQ(ccm::truncf(0.5F), std::truncf(0.5F));
    EXPECT_EQ(ccm::truncf(-0.5F), std::truncf(-0.5F));
    EXPECT_EQ(ccm::truncf(0.9F), std::truncf(0.9F));
    EXPECT_EQ(ccm::truncf(-0.9F), std::truncf(-0.9F));
    EXPECT_EQ(ccm::truncf(0.9999999999999999F), std::truncf(0.9999999999999999F));
    EXPECT_EQ(ccm::truncf(-0.9999999999999999F), std::truncf(-0.9999999999999999F));

	//EXPECT_FLOAT_EQ(ccm::fmod(30.508474576271183309f, 6.1016949152542370172F), std::fmod(30.508474576271183309f, 6.1016949152542370172F));
	EXPECT_EQ(ccm::truncf(30.508474576271183309F), std::truncf(30.508474576271183309F));


	// Test with long double using std::truncl and ccm::truncl
	EXPECT_EQ(ccm::truncl(1.0L), std::truncl(1.0L));
    EXPECT_EQ(ccm::truncl(1.5L), std::truncl(1.5L));
    EXPECT_EQ(ccm::truncl(1.9L), std::truncl(1.9L));
    EXPECT_EQ(ccm::truncl(-1.0L), std::truncl(-1.0L));
    EXPECT_EQ(ccm::truncl(-1.5L), std::truncl(-1.5L));
    EXPECT_EQ(ccm::truncl(-1.9L), std::truncl(-1.9L));
    EXPECT_EQ(ccm::truncl(std::numeric_limits<long double>::infinity()), std::truncl(std::numeric_limits<long double>::infinity()));
    EXPECT_EQ(ccm::truncl(-std::numeric_limits<long double>::infinity()), std::truncl(-std::numeric_limits<long double>::infinity()));
    EXPECT_TRUE(std::isnan(ccm::truncl(std::nanl(""))));
    EXPECT_TRUE(std::isnan(ccm::truncl(-std::nanl(""))));
    EXPECT_EQ(ccm::truncl(0.0L), std::truncl(0.0L));
    EXPECT_EQ(ccm::truncl(-0.0L), std::truncl(-0.0L));
    EXPECT_EQ(ccm::truncl(0.5L), std::truncl(0.5L));
    EXPECT_EQ(ccm::truncl(-0.5L), std::truncl(-0.5L));
    EXPECT_EQ(ccm::truncl(0.9L), std::truncl(0.9L));
    EXPECT_EQ(ccm::truncl(-0.9L), std::truncl(-0.9L));
    EXPECT_EQ(ccm::truncl(0.9999999999999999L), std::truncl(0.9999999999999999L));
    EXPECT_EQ(ccm::truncl(-0.9999999999999999L), std::truncl(-0.9999999999999999L));
}
