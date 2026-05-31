/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <gtest/gtest.h>

#include "ccmath/ccmath.hpp"

#include <cmath>
#include <limits>

#include "utils/std_compare.hpp"

TEST(CcmathExponentialTests, Exp2_Double_ValidInput)
{
	constexpr double inputs[] = { 1.0, 2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0, 256.0, 512.0, 1024.0, 2048.0, 4096.0 };
	for (double input : inputs) { ccm::test::ExpectUnaryMatchesStd(input, ccm::exp2<double>, static_cast<double (*)(double)>(std::exp2)); }

	// Test that exp2 is mathematically the same as exp( X * log(2) )
	ccm::test::ExpectSameAsStd(ccm::exp2(4.0), std::exp(4.0 * std::log(2.0)));
	ccm::test::ExpectSameAsStd(ccm::exp2(4.0), ccm::exp(4.0 * ccm::log(2.0)));


}

TEST(CcmathExponentialTests, Exp2_Double_EdgeCases)
{
	constexpr double inputs[] = { 0.0,
								  -0.0,
								  std::numeric_limits<double>::infinity(),
								  -std::numeric_limits<double>::infinity(),
								  std::numeric_limits<double>::min(),
								  -std::numeric_limits<double>::min(),
								  std::numeric_limits<double>::max(),
								  -std::numeric_limits<double>::max(),
								  std::numeric_limits<double>::denorm_min(),
								  -std::numeric_limits<double>::denorm_min(),
								  std::numeric_limits<double>::quiet_NaN() };
	for (double input : inputs) { ccm::test::ExpectUnaryMatchesStd(input, ccm::exp2<double>, static_cast<double (*)(double)>(std::exp2)); }
}



TEST(CcmathExponentialTests, Exp2_Float)
{
	constexpr float inputs[] = { 1.0F, 2.0F, 4.0F, 8.0F, 16.0F, 32.0F, 64.0F, 128.0F, 256.0F, 512.0F, 1024.0F, 2048.0F, 4096.0F };
	for (float input : inputs) { ccm::test::ExpectUnaryMatchesStd(input, ccm::exp2<float>, static_cast<float (*)(float)>(std::exp2)); }
}

TEST(CcmathExponentialTests, Exp2_Float_EdgeCases)
{
	constexpr float inputs[] = { 0.0F,
								 -0.0F,
								 std::numeric_limits<float>::infinity(),
								 -std::numeric_limits<float>::infinity(),
								 -1.038470636961323e34F,
								 std::numeric_limits<float>::quiet_NaN() };
	for (float input : inputs) { ccm::test::ExpectUnaryMatchesStd(input, ccm::exp2<float>, static_cast<float (*)(float)>(std::exp2)); }
}

