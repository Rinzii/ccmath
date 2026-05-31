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
#include "utils/ulp_suite.hpp"

TEST(CcmathExponentialTests, Log10)
{
	static_assert(ccm::log10(1.0) == 0.0, "ccm::log10(1) must be 0 at compile time");
	static_assert(ccm::log10(10.0) == 1.0, "ccm::log10(10) must be 1 at compile time");

	EXPECT_FLOAT_EQ(ccm::log10f(1000.0f), ccm::log10(1000.0f));
	EXPECT_DOUBLE_EQ(ccm::log10l(100.0L), ccm::log10(100.0L));

	ccm::test::ExpectUlpUnaryVsStd(2.0, ccm::log10<double>, static_cast<double (*)(double)>(std::log10));
	ccm::test::ExpectUlpUnaryVsStd(100.0, ccm::log10<double>, static_cast<double (*)(double)>(std::log10));
}
