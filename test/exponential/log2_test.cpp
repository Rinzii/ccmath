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
#include "ccmath/detail/exponential/log2.hpp"

TEST(CcmathExponentialTests, Log2)
{
	EXPECT_EQ(ccm::log2(1.0), std::log2(1.0));
	EXPECT_EQ(ccm::log2(2.0), std::log2(2.0));
	EXPECT_EQ(ccm::log2(4.0), std::log2(4.0));
	EXPECT_EQ(ccm::log2(8.0), std::log2(8.0));
	EXPECT_EQ(ccm::log2(16.0), std::log2(16.0));
	EXPECT_EQ(ccm::log2(32.0), std::log2(32.0));
	EXPECT_EQ(ccm::log2(64.0), std::log2(64.0));
	EXPECT_EQ(ccm::log2(128.0), std::log2(128.0));
	EXPECT_EQ(ccm::log2(256.0), std::log2(256.0));
	EXPECT_EQ(ccm::log2(512.0), std::log2(512.0));
	EXPECT_EQ(ccm::log2(1024.0), std::log2(1024.0));
	EXPECT_EQ(ccm::log2(2048.0), std::log2(2048.0));
	EXPECT_EQ(ccm::log2(4096.0), std::log2(4096.0));

}
