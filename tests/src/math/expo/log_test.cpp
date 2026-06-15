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
#include "ccmath/internal/math/generic/func/expo/log_gen.hpp"
#include "utils/std_compare.hpp"
#include "utils/test_runtime.hpp"

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

TEST(CcmathExponentialTests, LogDoubleLastTableBucket)
{
	// The double log table was missing its final entry, so every input whose
	// significand fell in the last of the 128 buckets collapsed to k*ln2.
	// Pin the originally failing bit patterns through the constexpr path.
	static_assert(ccm::log(0x1.5fdffffffff32p+0) == 0x1.45bba0a0754c4p-2, "log must use the final table entry in the last bucket");
	static_assert(ccm::log(0x1.5fdffffffff32p+1) == 0x1.02e1001fef229p+0, "log must add k*ln2 to the final table entry");

	// Same patterns plus the bucket boundaries through the runtime generic kernel.
	constexpr double inputs[] = {
		0x1.5fdffffffff32p-1011, 0x1.5fdffffffff32p-1, 0x1.5fdffffffff32p+0,   0x1.5fdffffffff32p+1, 0x1.5ep+0,
		0x1.5ffffffffffffp+0,	 0x1.5ep-512,		   0x1.5ffffffffffffp+512, 0x1.5dfffffffffffp+0, 0x1.6p+0,
	};
	for (double input : inputs)
	{
		ccm::test::ExpectUnaryMatchesStd(ccm::test::runtime_value(input), ccm::gen::log_gen<double>, static_cast<double (*)(double)>(std::log));
	}
}
