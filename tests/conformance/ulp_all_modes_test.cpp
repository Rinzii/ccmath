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
#include "utils/conformance_suite.hpp"
#include "utils/math_samples.hpp"

#include <gtest/gtest.h>

#include <cmath>

TEST(CcmathAllModesUlpTests, ExpDouble)
{
	ccm::test::ExpectUlpUnaryOverAllRoundingModes(ccm::test::samples::kAllModesProbeDouble, ccm::exp<double>, static_cast<double (*)(double)>(std::exp));
}

TEST(CcmathAllModesUlpTests, LogDouble)
{
	ccm::test::ExpectUlpUnaryOverAllRoundingModes(ccm::test::samples::kAllModesProbeDouble, ccm::log<double>, static_cast<double (*)(double)>(std::log));
}

TEST(CcmathAllModesUlpTests, Log2Double)
{
	ccm::test::ExpectUlpUnaryOverAllRoundingModes(ccm::test::samples::kAllModesProbeDouble, ccm::log2<double>, static_cast<double (*)(double)>(std::log2));
}

TEST(CcmathAllModesUlpTests, Log10Double)
{
	ccm::test::ExpectUlpUnaryOverAllRoundingModes(ccm::test::samples::kAllModesProbeDouble, ccm::log10<double>, static_cast<double (*)(double)>(std::log10));
}

TEST(CcmathAllModesUlpTests, SinDouble)
{
	ccm::test::ExpectUlpUnaryOverAllRoundingModes(ccm::test::samples::kAllModesProbeDouble, ccm::sin<double>, static_cast<double (*)(double)>(std::sin));
}

TEST(CcmathAllModesUlpTests, CosDouble)
{
	ccm::test::ExpectUlpUnaryOverAllRoundingModes(ccm::test::samples::kAllModesProbeDouble, ccm::cos<double>, static_cast<double (*)(double)>(std::cos));
}

// [c.math]/1: validates that the double overload preserves the standard C library behavior across active rounding modes.
TEST(CcmathAllModesUlpTests, PowDouble)
{
	ccm::test::ExpectUlpBinaryOverAllRoundingModes(
		ccm::test::samples::kAllModesPowBases, ccm::test::samples::kAllModesPowExponents, ccm::pow<double>, static_cast<double (*)(double, double)>(std::pow));
}

TEST(CcmathAllModesUlpTests, Expm1Double)
{
	ccm::test::ExpectUlpUnaryOverAllRoundingModes(ccm::test::samples::kAllModesProbeDouble, ccm::expm1<double>, static_cast<double (*)(double)>(std::expm1));
}

TEST(CcmathAllModesUlpTests, Log1pDouble)
{
	ccm::test::ExpectUlpUnaryOverAllRoundingModes(ccm::test::samples::kAllModesProbeDouble, ccm::log1p<double>, static_cast<double (*)(double)>(std::log1p));
}

TEST(CcmathAllModesUlpTests, Log2Float)
{
	ccm::test::ExpectUlpUnaryOverAllRoundingModes(ccm::test::samples::kExpoFloat, ccm::log2<float>, static_cast<float (*)(float)>(std::log2));
}
