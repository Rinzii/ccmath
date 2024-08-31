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

TEST(CcmathFmanipTests, Nexttoward)
{
	EXPECT_EQ(ccm::nexttoward(1.0, 2.0), std::nexttoward(1.0, 2.0));

}

