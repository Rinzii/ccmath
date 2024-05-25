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

TEST(CcmathNearestTests, Ceil)
{
   EXPECT_EQ(ccm::ceil(1.0), std::ceil(1.0));
   EXPECT_EQ(ccm::ceil(1.0000000000000000000000000000000000000000000001), std::ceil(1.0000000000000000000000000000000000000000000001));
   EXPECT_EQ(ccm::ceil(1.5), std::ceil(1.5));
   EXPECT_EQ(ccm::ceil(1.9), std::ceil(1.9));
   EXPECT_EQ(ccm::ceil(-1.0), std::ceil(-1.0));
   EXPECT_EQ(ccm::ceil(-1.5), std::ceil(-1.5));
   EXPECT_EQ(ccm::ceil(-1.9), std::ceil(-1.9));
   EXPECT_EQ(ccm::ceil(std::numeric_limits<double>::infinity()), std::ceil(std::numeric_limits<double>::infinity()));
   EXPECT_EQ(ccm::ceil(-std::numeric_limits<double>::infinity()), std::ceil(-std::numeric_limits<double>::infinity()));

   bool isCcmPositiveNanPositive = (std::signbit(ccm::ceil(std::numeric_limits<double>::quiet_NaN())) == false && std::isnan(ccm::ceil(std::numeric_limits<double>::quiet_NaN())) == true); // NOLINT
   bool isStdPositiveNanPositive = (std::signbit(std::ceil(std::numeric_limits<double>::quiet_NaN())) == false && std::isnan(std::ceil(std::numeric_limits<double>::quiet_NaN())) == true); // NOLINT
   EXPECT_EQ(isCcmPositiveNanPositive, isStdPositiveNanPositive);

   bool isCcmNegativeNanNegative = (std::signbit(ccm::ceil(-std::numeric_limits<double>::quiet_NaN())) == true && std::isnan(ccm::ceil(-std::numeric_limits<double>::quiet_NaN())) == true); // NOLINT
   bool isStdNegativeNanNegative = (std::signbit(std::ceil(-std::numeric_limits<double>::quiet_NaN())) == true && std::isnan(std::ceil(-std::numeric_limits<double>::quiet_NaN())) == true); // NOLINT
   EXPECT_EQ(isCcmNegativeNanNegative, isStdNegativeNanNegative);

   bool testThatCcmCeilThatNanReturnsNan = (std::isnan(ccm::ceil(std::numeric_limits<double>::quiet_NaN())));
   bool testThatCcmCeilThatNegativeNanReturnsNegativeNan = (std::isnan(ccm::ceil(-std::numeric_limits<double>::quiet_NaN())));
   EXPECT_EQ(testThatCcmCeilThatNanReturnsNan, testThatCcmCeilThatNegativeNanReturnsNegativeNan);

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

}
