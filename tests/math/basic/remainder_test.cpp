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

TEST(CcmathBasicTests, Remainder)
{
	static_assert(ccm::remainder(1.0, 1.0) == 0.0, "remainder has failed testing that it is static_assert-able!");

	ccm::test::ExpectBinaryMatchesStd(1.0, 1.0, ccm::remainder<double>, static_cast<double (*)(double, double)>(std::remainder));
	ccm::test::ExpectBinaryMatchesStd(1.0, 0.0, ccm::remainder<double>, static_cast<double (*)(double, double)>(std::remainder));
	ccm::test::ExpectBinaryMatchesStd(0.0, 1.0, ccm::remainder<double>, static_cast<double (*)(double, double)>(std::remainder));
	ccm::test::ExpectBinaryMatchesStd(0.0, 0.0, ccm::remainder<double>, static_cast<double (*)(double, double)>(std::remainder));
	ccm::test::ExpectBinaryMatchesStd(-1.0, 1.0, ccm::remainder<double>, static_cast<double (*)(double, double)>(std::remainder));
	ccm::test::ExpectBinaryMatchesStd(1.0, -1.0, ccm::remainder<double>, static_cast<double (*)(double, double)>(std::remainder));
	ccm::test::ExpectBinaryMatchesStd(-1.0, -1.0, ccm::remainder<double>, static_cast<double (*)(double, double)>(std::remainder));
	ccm::test::ExpectBinaryMatchesStd(-1.0, 0.0, ccm::remainder<double>, static_cast<double (*)(double, double)>(std::remainder));

	constexpr double subnormal_dividend = -5.0166534782602e-198;
	constexpr double subnormal_divisor	= 5.27085811e-315;
	ccm::test::ExpectBinaryMatchesStd(subnormal_dividend, subnormal_divisor, ccm::remainder<double>, static_cast<double (*)(double, double)>(std::remainder));
}
