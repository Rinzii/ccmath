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
#include "ccmath/ccmath.hpp"

TEST(CcmathNearestTests, Ceil)
{
    static_assert(ccm::ceil(1.0) == ccm::ceil(1.0), "ccm::ceil is not static_assertable");
    EXPECT_EQ(ccm::ceil(1.0), std::ceil(1.0));
    EXPECT_EQ(ccm::ceil(1.5), std::ceil(1.5));
    EXPECT_EQ(ccm::ceil(1.9), std::ceil(1.9));
    EXPECT_EQ(ccm::ceil(-1.0), std::ceil(-1.0));
    EXPECT_EQ(ccm::ceil(-1.5), std::ceil(-1.5));
    EXPECT_EQ(ccm::ceil(-1.9), std::ceil(-1.9));
    EXPECT_EQ(ccm::ceil(std::numeric_limits<double>::infinity()), std::ceil(std::numeric_limits<double>::infinity()));
    EXPECT_EQ(ccm::ceil(-std::numeric_limits<double>::infinity()), std::ceil(-std::numeric_limits<double>::infinity()));

    // Check sign of NaN return
    bool testCmmCeilThatNanReturnsNan = std::isnan(ccm::ceil(std::numeric_limits<double>::quiet_NaN()));
    bool testStdCeilThatNanReturnsNan = std::isnan(std::ceil(std::numeric_limits<double>::quiet_NaN()));
    EXPECT_EQ(testCmmCeilThatNanReturnsNan, testStdCeilThatNanReturnsNan);

    bool testCmmCeilThatNanIsPositive = std::signbit(ccm::ceil(std::numeric_limits<double>::quiet_NaN()));
    bool testStdCeilThatNanIsPositive = std::signbit(std::ceil(std::numeric_limits<double>::quiet_NaN()));
    EXPECT_EQ(testCmmCeilThatNanIsPositive, testStdCeilThatNanIsPositive);

    bool testCmmCeilThatNanIsNegative = std::signbit(ccm::ceil(-std::numeric_limits<double>::quiet_NaN()));
    bool testStdCeilThatNanIsNegative = std::signbit(std::ceil(-std::numeric_limits<double>::quiet_NaN));
    EXPECT_EQ(testCemmCeilThatNanIsNegative, testStdCeilThatNanIsNegative);

    EXPECT_EQ(ccm::ceil(0.0), std::ceil(0.0));
    EXPECT_EQ(ccm::ceil(-0.0), std::ceil(-0.0));
    EXPECT_EQ(ccm::ceil(0.5), std::ceil(0.5));
    EXPECT_EQ(ccm::ceil(-0.5), std::ceil(-0.5));
    EXPECT_EQ(ccm::ceil(0.9), std::ceil(0.9));
    EXPECT_EQ(ccm::ceil(-0.9), std::ceil(-0.9));
    EXPECT_EQ(ccm::ceil(0.9999999999999999), std::ceil(0.9999999999999999));
    EXPECT_EQ(ccm::ceil(-0.9999999999999999), std::ceil(-0.9999999999999999));
    EXPECT_EQ(ccm::ceil(1.0000000000000001), std::ceil(1.0000000000000001));
    EXPECT_EQ(ccm::ceil(-1.0000000000000001), std::ceil(-1.0000000000000001));
    
    // Test with float using std::ceilf and ccm::ceilf
    static_assert(ccm::ceilf(1.0f) == ccm::ceilf(1.0f), "ccm::ceilf not static_assertable");
	EXPECT_EQ(ccm::ceilf(1.0f), std::ceilf(1.0f));
    EXPECT_EQ(ccm::ceilf(1.5f), std::ceilf(1.5f));
    EXPECT_EQ(ccm::ceilf(1.9f), std::ceilf(1.9f));
    EXPECT_EQ(ccm::ceilf(-1.0f), std::ceilf(-1.0f));
    EXPECT_EQ(ccm::ceilf(-1.5f), std::ceilf(-1.5f));
    EXPECT_EQ(ccm::ceilf(-1.9f), std::ceilf(-1.9f));
    EXPECT_EQ(ccm::ceilf(std::numeric_limits<float>::infinity()), std::ceilf(std::numeric_limits<float>::infinity()));
    EXPECT_EQ(ccm::ceilf(-std::numeric_limits<float>::infinity()), std::ceilf(-std::numeric_limits<float>::infinity()));
    EXPECT_TRUE(std::isnan(ccm::ceilf(std::nanf(""))));
    EXPECT_TRUE(std::isnan(ccm::ceilf(-std::nanf(""))));
    EXPECT_EQ(ccm::ceilf(0.0f), std::ceilf(0.0f));
    EXPECT_EQ(ccm::ceilf(-0.0f), std::ceilf(-0.0f));
    EXPECT_EQ(ccm::ceilf(0.5f), std::ceilf(0.5f));
    EXPECT_EQ(ccm::ceilf(-0.5f), std::ceilf(-0.5f));
    EXPECT_EQ(ccm::ceilf(0.9f), std::ceilf(0.9f));
    EXPECT_EQ(ccm::ceilf(-0.9f), std::ceilf(-0.9f));
    EXPECT_EQ(ccm::ceilf(0.9999999999999999f), std::ceilf(0.9999999999999999f));
    EXPECT_EQ(ccm::ceilf(-0.9999999999999999f), std::ceilf(-0.9999999999999999f));
    EXPECT_EQ(ccm::ceilf(30.508474576271183309f), std::ceilf(30.508474576271183309f)); 

    // Test with long double using std::ceill and ccm::ceill
    static_assert(ccm::ceill(1.0l) == ccm::ceill(1.0l), "ccm::ceill not static_assertable");
	EXPECT_EQ(ccm::ceill(1.0l), std::ceill(1.0l));
    EXPECT_EQ(ccm::ceill(1.5l), std::ceill(1.5l));
    EXPECT_EQ(ccm::ceill(1.9l), std::ceill(1.9l));
    EXPECT_EQ(ccm::ceill(-1.0l), std::ceill(-1.0l));
    EXPECT_EQ(ccm::ceill(-1.5l), std::ceill(-1.5l));
    EXPECT_EQ(ccm::ceill(-1.9l), std::ceill(-1.9l));
    EXPECT_EQ(ccm::ceill(std::numeric_limits<long double>::infinity()), std::ceill(std::numeric_limits<long double>::infinity()));
    EXPECT_EQ(ccm::ceill(-std::numeric_limits<long double>::infinity()), std::ceill(-std::numeric_limits<long double>::infinity()));
    EXPECT_TRUE(std::isnan(ccm::ceill(std::nanl(""))));
    EXPECT_TRUE(std::isnan(ccm::ceill(-std::nanl(""))));
    EXPECT_EQ(ccm::ceill(0.0l), std::ceill(0.0l));
    EXPECT_EQ(ccm::ceill(-0.0l), std::ceill(-0.0l));
    EXPECT_EQ(ccm::ceill(0.5l), std::ceill(0.5l));
    EXPECT_EQ(ccm::ceill(-0.5l), std::ceill(-0.5l));
    EXPECT_EQ(ccm::ceill(0.9l), std::ceill(0.9l));
    EXPECT_EQ(ccm::ceill(-0.9l), std::ceill(-0.9l));
    EXPECT_EQ(ccm::ceill(0.9999999999999999l), std::ceill(0.9999999999999999l));
    EXPECT_EQ(ccm::ceill(-0.9999999999999999l), std::ceill(-0.9999999999999999l));
}