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
#include "ccmath/detail/compare/isgreaterequal.hpp"

TEST(CcmathCompareTests, IsGreaterEqual)
{
	EXPECT_EQ(ccm::isgreaterequal(1.0, 1.0), std::isgreaterequal(1.0, 1.0));
	EXPECT_EQ(ccm::isgreaterequal(1.0, 0.0), std::isgreaterequal(1.0, 0.0));
	EXPECT_EQ(ccm::isgreaterequal(0.0, 1.0), std::isgreaterequal(0.0, 1.0));
	EXPECT_EQ(ccm::isgreaterequal(0.0, 0.0), std::isgreaterequal(0.0, 0.0));
	EXPECT_EQ(ccm::isgreaterequal(-1.0, 0.0), std::isgreaterequal(-1.0, 0.0));
	EXPECT_EQ(ccm::isgreaterequal(0.0, -1.0), std::isgreaterequal(0.0, -1.0));
	EXPECT_EQ(ccm::isgreaterequal(-1.0, -1.0), std::isgreaterequal(-1.0, -1.0));

}
