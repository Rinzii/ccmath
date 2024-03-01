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
#include "ccmath/internal/helpers/not_null.hpp"

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

	//EXPECT_FLOAT_EQ(ccm::fmod(30.508474576271183309f, 6.1016949152542370172f), std::fmod(30.508474576271183309f, 6.1016949152542370172f));
	EXPECT_EQ(ccm::truncf(30.508474576271183309f), std::truncf(30.508474576271183309f));


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
