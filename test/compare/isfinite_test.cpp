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
#include <limits>

// TODO: add more tests for isfinite

TEST(CcmathCompareTests, IsFinite)
{
	// test isfinite is static_assert-able
	static_assert(ccm::isfinite(1.0), "isfinite has failed testing that it is static_assert-able!");

	EXPECT_EQ(ccm::isfinite(1.0), std::isfinite(1.0));

}
