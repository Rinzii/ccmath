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

TEST(CcmathBasicTests, FmodLargeQuotientCompileTime)
{
	// The exact fdlibm bit-reduction must reduce these at compile time even though abs(x / y) is far
	// above 2^53. The old constexpr x - trunc(x / y) * y formula collapsed to 0 here.
	static_assert(ccm::fmod(1e30, 3.0) == 1.0, "fmod(1e30, 3) must be 1");
	static_assert(ccm::fmod(-1e30, 3.0) == -1.0, "fmod(-1e30, 3) must be -1");
	static_assert(ccm::fmod(1e300, 7.0) == 1.0, "fmod(1e300, 7) must reduce exactly to 1");
	static_assert(ccm::fmod(1e30F, 3.0F) == 0.0F, "fmodf(1e30, 3) must reduce exactly to 0");

	// Small / normal cases stay exact.
	static_assert(ccm::fmod(10.0, 3.0) == 1.0, "fmod(10, 3) must be 1");
	static_assert(ccm::fmod(-10.0, 3.0) == -1.0, "fmod(-10, 3) must be -1");
	static_assert(ccm::fmod(7.5, 2.0) == 1.5, "fmod(7.5, 2) must be 1.5");
}

TEST(CcmathBasicTests, Fmod)
{

	// Test that fmod works with static_assert
	static_assert(ccm::fmod(1, 2) == 1, "fmod has failed testing that it is static_assert-able!");

	ccm::test::ExpectBinaryMatchesStd(10.0F, 3.0F, ccm::fmod<float>, static_cast<float (*)(float, float)>(std::fmod));
	ccm::test::ExpectBinaryMatchesStd(10.0F, -3.0F, ccm::fmod<float>, static_cast<float (*)(float, float)>(std::fmod));
	ccm::test::ExpectBinaryMatchesStd(-10.0F, 3.0F, ccm::fmod<float>, static_cast<float (*)(float, float)>(std::fmod));
	ccm::test::ExpectBinaryMatchesStd(-10.0F, -3.0F, ccm::fmod<float>, static_cast<float (*)(float, float)>(std::fmod));
	ccm::test::ExpectBinaryMatchesStd(0.0F, 3.0F, ccm::fmod<float>, static_cast<float (*)(float, float)>(std::fmod));

	// This is a tough test as it forces rounding precision issues.
	// EXPECT_FLOAT_EQ(ccm::fmod(30.508474576271183309f, 6.1016949152542370172f), std::fmod(30.508474576271183309f, 6.1016949152542370172f));

	// Test fmod with integer numbers
	ccm::test::ExpectSameAsStd(ccm::fmod(10, 3), std::fmod(10, 3));

	ccm::test::ExpectBinaryMatchesStd(1.5, 2.781342323457793e-309, ccm::fmod<double>, static_cast<double (*)(double, double)>(std::fmod));

	/// Test Edge Cases

	ccm::test::ExpectBinaryMatchesStd(0.0F, 1.0F, ccm::fmod<float>, static_cast<float (*)(float, float)>(std::fmod));
	ccm::test::ExpectBinaryMatchesStd(-0.0F, 1.0F, ccm::fmod<float>, static_cast<float (*)(float, float)>(std::fmod));
	ccm::test::ExpectBinaryMatchesStd(10.0F, 0.0F, ccm::fmod<float>, static_cast<float (*)(float, float)>(std::fmod));
	ccm::test::ExpectBinaryMatchesStd(10.0F, std::numeric_limits<float>::infinity(), ccm::fmod<float>, static_cast<float (*)(float, float)>(std::fmod));
	ccm::test::ExpectBinaryMatchesStd(10.0F, -std::numeric_limits<float>::infinity(), ccm::fmod<float>, static_cast<float (*)(float, float)>(std::fmod));
	ccm::test::ExpectBinaryMatchesStd(std::numeric_limits<float>::quiet_NaN(), 10.0F, ccm::fmod<float>, static_cast<float (*)(float, float)>(std::fmod));
	ccm::test::ExpectBinaryMatchesStd(10.0F, std::numeric_limits<float>::quiet_NaN(), ccm::fmod<float>, static_cast<float (*)(float, float)>(std::fmod));
}
