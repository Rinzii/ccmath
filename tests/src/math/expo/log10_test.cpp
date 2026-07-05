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
#include "ccmath/internal/math/generic/func/expo/log10_gen.hpp"
#include "utils/test_runtime.hpp"
#include "utils/ulp_suite.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>

TEST(CcmathExponentialTests, Log10)
{
	static_assert(ccm::log10(1.0) == 0.0, "ccm::log10(1) must be 0 at compile time");
	static_assert(ccm::log10(10.0) == 1.0, "ccm::log10(10) must be 1 at compile time");

	EXPECT_FLOAT_EQ(ccm::log10f(1000.0f), ccm::log10(1000.0f));
	EXPECT_EQ(ccm::log10l(100.0L), ccm::log10(100.0L));

	ccm::test::ExpectUlpUnaryVsStd(2.0, ccm::log10<double>, static_cast<double (*)(double)>(std::log10));
	ccm::test::ExpectUlpUnaryVsStd(100.0, ccm::log10<double>, static_cast<double (*)(double)>(std::log10));
}

TEST(CcmathExponentialTests, Log10DoubleLastTableBucket)
{
	// log10 derives from the double log kernel, so the missing final log table
	// entry broke the same significand range. Pin the failing bit patterns.
	static_assert(ccm::log10(0x1.5fdffffffff32p+0) == 0x1.1aed92aadd55p-3, "log10 must use the final log table entry in the last bucket");

	constexpr double inputs[] = {
		0x1.5fdffffffff32p-1011, 0x1.5fdffffffff32p-1, 0x1.5fdffffffff32p+0,   0x1.5fdffffffff32p+1, 0x1.5ep+0,
		0x1.5ffffffffffffp+0,	 0x1.5ep-512,		   0x1.5ffffffffffffp+512,
	};
	for (double input : inputs)
	{
		ccm::test::ExpectUlpUnaryVsStd(ccm::test::runtime_value(input), ccm::gen::log10_gen<double>, static_cast<double (*)(double)>(std::log10));
	}
}
