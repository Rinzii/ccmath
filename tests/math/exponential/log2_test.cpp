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

#include <cmath>
#include <limits>
#include "ccmath/ccmath.hpp"
#include "utils/std_compare.hpp"

TEST(CcmathExponentialTests, Log2)
{
	static_assert(ccm::log2(1.0) == 0.0, "log2 has failed testing that it is static_assert-able!"); // Check that we can use static_assert with this function

	constexpr double double_inputs[] = { 1.0, 2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0, 256.0, 512.0, 1024.0, 2048.0, 4096.0 };
	for (double input : double_inputs) { ccm::test::ExpectUnaryMatchesStd(input, ccm::log2<double>, static_cast<double (*)(double)>(std::log2)); }

	// Test floating point precision
	constexpr float float_inputs[] = { 1.0F, 2.0F, 4.0F, 8.0F, 16.0F, 32.0F, 64.0F, 128.0F, 256.0F, 512.0F, 1024.0F, 2048.0F, 4096.0F };
	for (float input : float_inputs) { ccm::test::ExpectUnaryMatchesStd(input, ccm::log2<float>, static_cast<float (*)(float)>(std::log2)); }

	// Test edge cases
	ccm::test::ExpectUnaryMatchesStd(0.0, ccm::log2<double>, static_cast<double (*)(double)>(std::log2));
	ccm::test::ExpectUnaryMatchesStd(-0.0, ccm::log2<double>, static_cast<double (*)(double)>(std::log2));
	ccm::test::ExpectUnaryMatchesStd(std::numeric_limits<double>::infinity(), ccm::log2<double>, static_cast<double (*)(double)>(std::log2));
	ccm::test::ExpectUnaryMatchesStd(-std::numeric_limits<double>::infinity(), ccm::log2<double>, static_cast<double (*)(double)>(std::log2));
	ccm::test::ExpectUnaryMatchesStd(std::numeric_limits<double>::quiet_NaN(), ccm::log2<double>, static_cast<double (*)(double)>(std::log2));
	ccm::test::ExpectUnaryMatchesStd(-std::numeric_limits<double>::quiet_NaN(), ccm::log2<double>, static_cast<double (*)(double)>(std::log2));
}
