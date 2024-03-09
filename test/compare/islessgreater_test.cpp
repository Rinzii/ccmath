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
#include "ccmath/detail/compare/islessgreater.hpp"

TEST(CcmathCompareTests, IsLessGreater)
{
	// Test that islessgreater is static_assert-able
    static_assert(ccm::islessgreater(1.0, 0.0) == true, "islessgreater has failed testing that it is static_assert-able!");

	EXPECT_EQ(ccm::islessgreater(1.0, 1.0), std::islessgreater(1.0, 1.0));
    EXPECT_EQ(ccm::islessgreater(1.0, 0.0), std::islessgreater(1.0, 0.0));
    EXPECT_EQ(ccm::islessgreater(0.0, 1.0), std::islessgreater(0.0, 1.0));
    EXPECT_EQ(ccm::islessgreater(0.0, 0.0), std::islessgreater(0.0, 0.0));
    EXPECT_EQ(ccm::islessgreater(-1.0, 0.0), std::islessgreater(-1.0, 0.0));
    EXPECT_EQ(ccm::islessgreater(0.0, -1.0), std::islessgreater(0.0, -1.0));
    EXPECT_EQ(ccm::islessgreater(-1.0, -1.0), std::islessgreater(-1.0, -1.0));
}
