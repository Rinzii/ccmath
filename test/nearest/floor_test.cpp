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

TEST(CcmathNearestTests, Floor)
{
   EXPECT_EQ(ccm::floor(1.0), std::floor(1.0));
   EXPECT_EQ(ccm::floor(1.0000000000000000000000000000000000000000000001), std::floor(1.0000000000000000000000000000000000000000000001));
   EXPECT_EQ(ccm::floor(1.5), std::floor(1.5));
   EXPECT_EQ(ccm::floor(1.9), std::floor(1.9));
   EXPECT_EQ(ccm::floor(-1.0), std::floor(-1.0));
   EXPECT_EQ(ccm::floor(-1.5), std::floor(-1.5));
   EXPECT_EQ(ccm::floor(-1.9), std::floor(-1.9));
   EXPECT_EQ(ccm::floor(std::numeric_limits<double>::infinity()), std::floor(std::numeric_limits<double>::infinity()));
   EXPECT_EQ(ccm::floor(-std::numeric_limits<double>::infinity()), std::floor(-std::numeric_limits<double>::infinity()));

   bool isCcmPositiveNanPositive = (std::signbit(ccm::floor(std::numeric_limits<double>::quiet_NaN())) == false && std::isnan(ccm::floor(std::numeric_limits<double>::quiet_NaN())) == true); // NOLINT
   bool isStdPositiveNanPositive = (std::signbit(std::floor(std::numeric_limits<double>::quiet_NaN())) == false && std::isnan(std::floor(std::numeric_limits<double>::quiet_NaN())) == true); // NOLINT
   EXPECT_EQ(isCcmPositiveNanPositive, isStdPositiveNanPositive);

   bool isCcmNegativeNanNegative = (std::signbit(ccm::floor(-std::numeric_limits<double>::quiet_NaN())) == true && std::isnan(ccm::floor(-std::numeric_limits<double>::quiet_NaN())) == true); // NOLINT
   bool isStdNegativeNanNegative = (std::signbit(std::floor(-std::numeric_limits<double>::quiet_NaN())) == true && std::isnan(std::floor(-std::numeric_limits<double>::quiet_NaN())) == true); // NOLINT
   EXPECT_EQ(isCcmNegativeNanNegative, isStdNegativeNanNegative);

   bool testThatCcmFloorThatNanReturnsNan = (std::isnan(ccm::floor(std::numeric_limits<double>::quiet_NaN())));
   bool testThatCcmFloorThatNegativeNanReturnsNegativeNan = (std::isnan(ccm::floor(-std::numeric_limits<double>::quiet_NaN())));
   EXPECT_EQ(testThatCcmFloorThatNanReturnsNan, testThatCcmFloorThatNegativeNanReturnsNegativeNan);

   EXPECT_EQ(ccm::floor(0.0), std::floor(0.0));
   EXPECT_EQ(ccm::floor(-0.0), std::floor(-0.0));
   EXPECT_EQ(ccm::floor(0.5), std::floor(0.5));
   EXPECT_EQ(ccm::floor(-0.5), std::floor(-0.5));
   EXPECT_EQ(ccm::floor(0.9), std::floor(0.9));
   EXPECT_EQ(ccm::floor(-0.9), std::floor(-0.9));
   EXPECT_EQ(ccm::floor(0.9999999999999999), std::floor(0.9999999999999999));
   EXPECT_EQ(ccm::floor(-0.9999999999999999), std::floor(-0.9999999999999999));
   EXPECT_EQ(ccm::floor(1.0000000000000001), std::floor(1.0000000000000001));
   EXPECT_EQ(ccm::floor(-1.0000000000000001), std::floor(-1.0000000000000001));

}
