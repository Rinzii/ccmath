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
#include "ccmath/detail/compare/isinf.hpp"

// TODO: add more tests for isinf

TEST(CcmathCompareTests, IsInf)
{
	// test isinf is static_assert-able
	static_assert(ccm::isinf(1.0) == false, "isinf has failed testing that it is static_assert-able!");

	EXPECT_EQ(ccm::isinf(1.0), std::isinf(1.0));
	EXPECT_EQ(ccm::isinf(0.0), std::isinf(0.0));
	EXPECT_EQ(ccm::isinf(-1.0), std::isinf(-1.0));
	EXPECT_EQ(ccm::isinf(std::numeric_limits<double>::infinity()), std::isinf(std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::isinf(-std::numeric_limits<double>::infinity()), std::isinf(-std::numeric_limits<double>::infinity()));

}
