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

#include <array>

TEST(CcmathBasicTests, LerpStaticAssert)
{
   static_assert(ccm::lerp(1, 2, 0.5) == 1.5, "lerp has failed testing that it is static_assert-able!");
}

TEST(CcmathBasicTests, LerpFloat)
{
   EXPECT_EQ(ccm::lerp(1.0F, 2.0F, 0.0F), 1.0F); // If t = 0, then the result is the first value
   EXPECT_EQ(ccm::lerp(1.0F, 2.0F, 1.0F), 2.0F); // If t = 1, then the result is the second value
   EXPECT_EQ(ccm::lerp(4.0F, 4.0F, 0.5F), 4.0F); // If the values are the same, then the result is the same

   EXPECT_EQ(ccm::lerp(1e8F, 1.0F, 0.5F), 5e+07);


   // NOLINTBEGIN
   std::size_t i = 0;
   std::array<float, 9> expected_values = { -5.0F, -2.5F, 0.0F, 2.5F, 5.0F, 7.5F, 10.0F, 12.5F, 15.0F };
   for (float t = -2.0; t <= 2.0; t += static_cast<float>(0.5))
   {
       // Testing extrapolation
       // Expected values are: -5 -2.5 0 2.5 5 7.5 10 12.5 15
       EXPECT_EQ(ccm::lerp(5.0, 10.0, t), expected_values.at(i)) << "ccm::lerp and std::lerp are not the same with t = " << t;
	   i++;
   }
   // NOLINTEND
}
