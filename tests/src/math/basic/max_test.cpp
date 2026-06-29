/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "utils/std_compare.hpp"

#include <gtest/gtest.h>

#include <ccmath/ccmath.hpp>

#include <cmath>
#include <limits>

TEST(CcmathBasicTests, Fmax)
{
	// Test that fmax works with static_assert
	static_assert(ccm::max(1.0, 2.0) == 2, "max has failed testing that it is static_assert-able!");

	const auto ccm_fmax_double = [](double x, double y) { return ccm::fmax(x, y); };

	ccm::test::ExpectBinaryMatchesStd(1.0, 2.0, ccm_fmax_double, static_cast<double (*)(double, double)>(std::fmax));
	ccm::test::ExpectBinaryMatchesStd(2.0, 1.0, ccm_fmax_double, static_cast<double (*)(double, double)>(std::fmax));
	ccm::test::ExpectBinaryMatchesStd(1.0, 1.0, ccm_fmax_double, static_cast<double (*)(double, double)>(std::fmax));
	ccm::test::ExpectBinaryMatchesStd(std::numeric_limits<double>::infinity(), 1.0, ccm_fmax_double, static_cast<double (*)(double, double)>(std::fmax));
	ccm::test::ExpectBinaryMatchesStd(1.0, std::numeric_limits<double>::infinity(), ccm_fmax_double, static_cast<double (*)(double, double)>(std::fmax));
	ccm::test::ExpectBinaryMatchesStd(std::numeric_limits<double>::quiet_NaN(), 1.0, ccm_fmax_double, static_cast<double (*)(double, double)>(std::fmax));
	ccm::test::ExpectBinaryMatchesStd(1.0, std::numeric_limits<double>::quiet_NaN(), ccm_fmax_double, static_cast<double (*)(double, double)>(std::fmax));
	ccm::test::ExpectBinaryMatchesStd(std::numeric_limits<double>::quiet_NaN(),
									  std::numeric_limits<double>::quiet_NaN(),
									  ccm_fmax_double,
									  static_cast<double (*)(double, double)>(std::fmax));
}
