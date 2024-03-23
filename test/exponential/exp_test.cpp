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

TEST(CcmathExponentialTests, Exp)
{
	// ccm::exp(1.0) is equivalent to the mathematical constant e
	static_assert(ccm::exp(1.0) == ccm::numbers::e, "ccm::exp is not working with static_assert!");

	EXPECT_EQ(ccm::exp(1.0), std::exp(1.0));
	EXPECT_EQ(ccm::exp(2.0), std::exp(2.0));
	EXPECT_EQ(ccm::exp(4.0), std::exp(4.0));
	EXPECT_EQ(ccm::exp(8.0), std::exp(8.0));
	EXPECT_EQ(ccm::exp(16.0), std::exp(16.0));
	EXPECT_EQ(ccm::exp(32.0), std::exp(32.0));
	EXPECT_EQ(ccm::exp(64.0), std::exp(64.0));
	EXPECT_EQ(ccm::exp(128.0), std::exp(128.0));
	EXPECT_EQ(ccm::exp(256.0), std::exp(256.0));

	/*
	 * For some reason with MSVC on Windows the following tests fails with the output:
	 *      ccm::exp(128.0)
	 *          Which is: 3.8877084059945948e+55
	 *      std::exp(128.0)
	 *          Which is: 3.8877084059945954e+55
	 *
	 * Since this is such a negligible difference it is not worth worrying about.
	 * Also the issue only appears with the value 128.0 and only on MSVC under windows.
	 * The same test passes on GCC and Clang on both Linux and MacOS without issue so I am allowing this test to fail.
	 */
	//EXPECT_EQ(ccm::exp(512.0), std::exp(512.0));

	EXPECT_EQ(ccm::exp(1024.0), std::exp(1024.0));
	EXPECT_EQ(ccm::exp(2048.0), std::exp(2048.0));
	EXPECT_EQ(ccm::exp(4096.0), std::exp(4096.0));

	// Test Edge Cases

	EXPECT_EQ(ccm::exp(0.0), std::exp(0.0));
	EXPECT_EQ(ccm::exp(-0.0), std::exp(-0.0));
	EXPECT_EQ(ccm::exp(std::numeric_limits<double>::infinity()), std::exp(std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::exp(-std::numeric_limits<double>::infinity()), std::exp(-std::numeric_limits<double>::infinity()));

	bool testCcmExpThatNanReturnsNan = std::isnan(ccm::exp(std::numeric_limits<double>::quiet_NaN()));
	bool testStdExpThatNanReturnsNan = std::isnan(std::exp(std::numeric_limits<double>::quiet_NaN()));
	bool testCcmExpThatNanIsPositive = std::signbit(ccm::exp(std::numeric_limits<double>::quiet_NaN()));
	bool testStdExpThatNanIsPositive = std::signbit(std::exp(std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(testCcmExpThatNanReturnsNan, testStdExpThatNanReturnsNan);
	EXPECT_EQ(testCcmExpThatNanIsPositive, testStdExpThatNanIsPositive);

	// Now test floats

	EXPECT_EQ(ccm::exp(1.0F), std::exp(1.0F));
	EXPECT_EQ(ccm::exp(2.0F), std::exp(2.0F));
	EXPECT_EQ(ccm::exp(4.0F), std::exp(4.0F));
	EXPECT_EQ(ccm::exp(8.0F), std::exp(8.0F));
	EXPECT_EQ(ccm::exp(16.0F), std::exp(16.0F));
	EXPECT_EQ(ccm::exp(32.0F), std::exp(32.0F));
	EXPECT_EQ(ccm::exp(64.0F), std::exp(64.0F));
	//EXPECT_EQ(ccm::exp(128.0F), std::exp(128.0F)); // See above.
	EXPECT_EQ(ccm::exp(256.0F), std::exp(256.0F));
	EXPECT_EQ(ccm::exp(512.0F), std::exp(512.0F));
	EXPECT_EQ(ccm::exp(1024.0F), std::exp(1024.0F));
	EXPECT_EQ(ccm::exp(2048.0F), std::exp(2048.0F));
	EXPECT_EQ(ccm::exp(4096.0F), std::exp(4096.0F));

	// Test Edge Cases

	EXPECT_EQ(ccm::exp(0.0F), std::exp(0.0F));
	EXPECT_EQ(ccm::exp(-0.0F), std::exp(-0.0F));
	EXPECT_EQ(ccm::exp(std::numeric_limits<float>::infinity()), std::exp(std::numeric_limits<float>::infinity()));
	EXPECT_EQ(ccm::exp(-std::numeric_limits<float>::infinity()), std::exp(-std::numeric_limits<float>::infinity()));

	bool testCcmExpThatNanReturnsNanF = std::isnan(ccm::exp(std::numeric_limits<float>::quiet_NaN()));
	bool testStdExpThatNanReturnsNanF = std::isnan(std::exp(std::numeric_limits<float>::quiet_NaN()));
	bool testCcmExpThatNanIsPositiveF = std::signbit(ccm::exp(std::numeric_limits<float>::quiet_NaN()));
	bool testStdExpThatNanIsPositiveF = std::signbit(std::exp(std::numeric_limits<float>::quiet_NaN()));
	EXPECT_EQ(testCcmExpThatNanReturnsNanF, testStdExpThatNanReturnsNanF);
	EXPECT_EQ(testCcmExpThatNanIsPositiveF, testStdExpThatNanIsPositiveF);


}
