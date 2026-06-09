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
#include "utils/std_compare.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>

TEST(CcmathExponentialTests, Log)
{
	// Verify function is static assert-able
	// 1.3862943611198906 was generated with std::log(4.0)
	static_assert(ccm::log(4.0) == 1.3862943611198906, "log has failed testing that it is static_assert-able!");

	constexpr double inputs[] = { 1.0,	  2.0,	  4.0,	  8.0,	   16.0,	32.0,	 64.0,	   128.0,	 256.0,	   512.0,	 1024.0,
								  2048.0, 4096.0, 8192.0, 16384.0, 32768.0, 65536.0, 131072.0, 262144.0, 524288.0, 1048576.0 };
	for (double input : inputs) { ccm::test::ExpectUnaryMatchesStd(input, ccm::log<double>, static_cast<double (*)(double)>(std::log)); }

	// Check for edge cases
	ccm::test::ExpectUnaryMatchesStd(std::numeric_limits<double>::quiet_NaN(), ccm::log<double>, static_cast<double (*)(double)>(std::log));
	ccm::test::ExpectUnaryMatchesStd(std::numeric_limits<double>::infinity(), ccm::log<double>, static_cast<double (*)(double)>(std::log));
	ccm::test::ExpectCcmNegativeDomainNaN(-1.0, ccm::log<double>);
	ccm::test::ExpectUnaryMatchesStd(0.0, ccm::log<double>, static_cast<double (*)(double)>(std::log));
	ccm::test::ExpectUnaryMatchesStd(-0.0, ccm::log<double>, static_cast<double (*)(double)>(std::log));
}
