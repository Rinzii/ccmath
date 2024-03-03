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
#include "ccmath/detail/exponential/log.hpp"


// Helper function to compare floating point numbers
template <typename T>
bool is_close(T a, T b, T epsilon = 3.32628e-08)
{
    return std::abs(a - b) < epsilon;
}

TEST(CcmathExponentialTests, Log)
{
    EXPECT_TRUE(is_close(ccm::log(1.0), std::log(1.0)));
	EXPECT_TRUE(is_close(ccm::log(2.0), std::log(2.0)));
	EXPECT_TRUE(is_close(ccm::log(4.0), std::log(4.0)));
	EXPECT_TRUE(is_close(ccm::log(8.0), std::log(8.0)));
	EXPECT_TRUE(is_close(ccm::log(16.0), std::log(16.0)));
	EXPECT_TRUE(is_close(ccm::log(32.0), std::log(32.0)));
	EXPECT_TRUE(is_close(ccm::log(64.0), std::log(64.0)));
	EXPECT_TRUE(is_close(ccm::log(128.0), std::log(128.0)));
	EXPECT_TRUE(is_close(ccm::log(256.0), std::log(256.0)));
	EXPECT_TRUE(is_close(ccm::log(512.0), std::log(512.0)));
	EXPECT_TRUE(is_close(ccm::log(1024.0), std::log(1024.0)));
	/*
	EXPECT_EQ(ccm::log(4.0), std::log(4.0));
	EXPECT_EQ(ccm::log(8.0), std::log(8.0));
	EXPECT_EQ(ccm::log(16.0), std::log(16.0));
	EXPECT_EQ(ccm::log(32.0), std::log(32.0));
	EXPECT_EQ(ccm::log(64.0), std::log(64.0));
	EXPECT_EQ(ccm::log(128.0), std::log(128.0));
	EXPECT_EQ(ccm::log(256.0), std::log(256.0));
	EXPECT_EQ(ccm::log(512.0), std::log(512.0));
	EXPECT_EQ(ccm::log(1024.0), std::log(1024.0));
	 */

}
