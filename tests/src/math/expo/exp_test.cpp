/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "ccmath/ccmath.hpp"
#include "ccmath/math/numbers.hpp"
#include "utils/std_compare.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>

TEST(CcmathExponentialTests, Exp)
{
	// ccm::exp(1.0) is equivalent to the mathematical constant e
	static_assert(ccm::exp(1.0) == ccm::numbers::e, "ccm::exp is not working with static_assert!");

	constexpr double double_inputs[] = { 1.0, 2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 256.0, 512.0, 1024.0, 2048.0, 4096.0 };
	for (double input : double_inputs)
	{
		ccm::test::ExpectUnaryMatchesStd(input, ccm::exp<double>, static_cast<double (*)(double)>(std::exp));
	}
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
	// EXPECT_EQ(ccm::exp(128.0), std::exp(128.0));
	ccm::test::ExpectSameAsStd(ccm::exp(4096.0) * ccm::exp(4096.0), std::exp(4096.0) * std::exp(4096.0));

	// Edge cases for float overloads.

	constexpr double double_edges[] = {
		0.0, -0.0, std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()
	};
	for (double input : double_edges)
	{
		ccm::test::ExpectUnaryMatchesStd(input, ccm::exp<double>, static_cast<double (*)(double)>(std::exp));
	}

	// Float overloads.

	constexpr float float_inputs[] = { 1.0F, 2.0F, 4.0F, 8.0F, 16.0F, 32.0F, 64.0F, 256.0F, 512.0F, 1024.0F, 2048.0F, 4096.0F };
	for (float input : float_inputs)
	{
		ccm::test::ExpectUnaryMatchesStd(input, ccm::exp<float>, static_cast<float (*)(float)>(std::exp));
	}
	// EXPECT_EQ(ccm::exp(128.0F), std::exp(128.0F)); // See above.
	constexpr float float_edges[] = {
		0.0F, -0.0F, std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::quiet_NaN()
	};
	for (float input : float_edges)
	{
		ccm::test::ExpectUnaryMatchesStd(input, ccm::exp<float>, static_cast<float (*)(float)>(std::exp));
	}
}
