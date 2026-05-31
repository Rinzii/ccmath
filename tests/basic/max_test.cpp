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

#include <ccmath/ccmath.hpp>
#include <cmath>
#include <limits>

#include "utils/std_compare.hpp"


TEST(CcmathBasicTests, Fmax)
{
	// Test that fmax works with static_assert
	static_assert(ccm::max(1.0, 2.0) == 2, "max has failed testing that it is static_assert-able!");

	ccm::test::ExpectBinaryMatchesStd(1.0, 2.0, ccm::fmax<double>, static_cast<double (*)(double, double)>(std::fmax));
	ccm::test::ExpectBinaryMatchesStd(2.0, 1.0, ccm::fmax<double>, static_cast<double (*)(double, double)>(std::fmax));
	ccm::test::ExpectBinaryMatchesStd(1.0, 1.0, ccm::fmax<double>, static_cast<double (*)(double, double)>(std::fmax));
	ccm::test::ExpectBinaryMatchesStd(std::numeric_limits<double>::infinity(), 1.0, ccm::fmax<double>, static_cast<double (*)(double, double)>(std::fmax));
	ccm::test::ExpectBinaryMatchesStd(1.0, std::numeric_limits<double>::infinity(), ccm::fmax<double>, static_cast<double (*)(double, double)>(std::fmax));
	ccm::test::ExpectBinaryMatchesStd(std::numeric_limits<double>::quiet_NaN(), 1.0, ccm::fmax<double>, static_cast<double (*)(double, double)>(std::fmax));
	ccm::test::ExpectBinaryMatchesStd(1.0, std::numeric_limits<double>::quiet_NaN(), ccm::fmax<double>, static_cast<double (*)(double, double)>(std::fmax));
	ccm::test::ExpectBinaryMatchesStd(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), ccm::fmax<double>,
									  static_cast<double (*)(double, double)>(std::fmax));


}
