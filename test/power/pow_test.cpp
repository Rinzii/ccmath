/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present ccmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
*/

#include <gtest/gtest.h>

#include "ccmath/ccmath.hpp"
#include "ccmath/numbers.hpp"
#include <cmath>
#include <limits>

TEST(CcmathExponentialTests, Pow)
{
	/*
	// ccm::exp(1.0) is equivalent to the mathematical constant e
	static_assert(ccm::pow(2.0, 2.0) == 4, "ccm::pow is not working with static_assert!");

	EXPECT_EQ(ccm::pow(2.0, 2.0), std::pow(2.0, 2.0));
	EXPECT_EQ(ccm::pow(2.0, 3.0), std::pow(2.0, 3.0));
	EXPECT_EQ(ccm::pow(2.0, 4.0), std::pow(2.0, 4.0));
	EXPECT_EQ(ccm::pow(2.0, 5.0), std::pow(2.0, 5.0));
	EXPECT_EQ(ccm::pow(2.0, 6.0), std::pow(2.0, 6.0));
*/
	unsigned int x = 2;
	unsigned int y = 2;
	EXPECT_EQ(ccm::internal::impl::pow_expo_by_sqr(x, y), std::pow(x, y));


}
