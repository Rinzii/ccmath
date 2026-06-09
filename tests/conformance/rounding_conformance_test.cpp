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
#include "utils/test_runtime.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace
{
	using ccm::test::runtime_value;
} // namespace

TEST(CcmathRoundingConformanceTests, NearbyIntMatchesStdAllModesDouble)
{
	ccm::test::ExpectFpUnaryOverMatchesStdAllModes(
		ccm::test::samples::kNearbyIntProbeDouble, ccm::nearbyint<double>, static_cast<double (*)(double)>(std::nearbyint));
}

TEST(CcmathRoundingConformanceTests, NearbyIntMatchesStdAllModesFloat)
{
	ccm::test::ExpectFpUnaryOverMatchesStdAllModes(
		ccm::test::samples::kNearbyIntProbeFloat, ccm::nearbyint<float>, static_cast<float (*)(float)>(std::nearbyint));
}

TEST(CcmathRoundingConformanceTests, LogOneMatchesStdAllModes)
{
	ccm::test::ForEachRoundingModeOrSkip(
		[&](int mode)
		{
			ccm::test::ScopedRoundingMode scope(mode);
			ASSERT_TRUE(scope.active());

			const double one = runtime_value(1.0);
			ccm::test::ExpectFpEq(ccm::log(one), std::log(one));
		});
}

TEST(CcmathRoundingConformanceTests, Log10OneMatchesStdAllModes)
{
	ccm::test::ForEachRoundingModeOrSkip(
		[&](int mode)
		{
			ccm::test::ScopedRoundingMode scope(mode);
			ASSERT_TRUE(scope.active());

			const double one = runtime_value(1.0);
			ccm::test::ExpectFpEq(ccm::log10(one), std::log10(one));
		});
}

TEST(CcmathRoundingConformanceTests, Log2OneMatchesStdAllModes)
{
	ccm::test::ForEachRoundingModeOrSkip(
		[&](int mode)
		{
			ccm::test::ScopedRoundingMode scope(mode);
			ASSERT_TRUE(scope.active());

			const double one = runtime_value(1.0);
			ccm::test::ExpectFpEq(ccm::log2(one), std::log2(one));
		});
}

TEST(CcmathRoundingConformanceTests, Log1pZeroMatchesStdAllModes)
{
	ccm::test::ExpectFpUnaryMatchesStdAllModes(0.0, ccm::log1p<double>, static_cast<double (*)(double)>(std::log1p));
	ccm::test::ExpectFpUnaryMatchesStdAllModes(-0.0, ccm::log1p<double>, static_cast<double (*)(double)>(std::log1p));
}

TEST(CcmathRoundingConformanceTests, Expm1ZeroMatchesStdAllModes)
{
	ccm::test::ExpectFpUnaryMatchesStdAllModes(0.0, ccm::expm1<double>, static_cast<double (*)(double)>(std::expm1));
	ccm::test::ExpectFpUnaryMatchesStdAllModes(-0.0, ccm::expm1<double>, static_cast<double (*)(double)>(std::expm1));
}

TEST(CcmathRoundingConformanceTests, ExpZeroMatchesStdAllModes)
{
	ccm::test::ExpectFpUnaryMatchesStdAllModes(0.0, ccm::exp<double>, static_cast<double (*)(double)>(std::exp));
	ccm::test::ExpectFpUnaryMatchesStdAllModes(-0.0, ccm::exp<double>, static_cast<double (*)(double)>(std::exp));
}

TEST(CcmathRoundingConformanceTests, SqrtMatchesStdAllModes)
{
	ccm::test::ExpectUlpUnaryOverAllRoundingModes(ccm::test::samples::kAllModesProbeDouble, ccm::sqrt<double>, static_cast<double (*)(double)>(std::sqrt));
	ccm::test::ExpectUlpUnaryOverAllRoundingModes(ccm::test::samples::kAllModesProbeFloat, ccm::sqrt<float>, static_cast<float (*)(float)>(std::sqrt));
}

TEST(CcmathRoundingConformanceTests, RintMatchesStdAllModesDouble)
{ ccm::test::ExpectFpUnaryOverMatchesStdAllModes(ccm::test::samples::kNearbyIntProbeDouble, ccm::rint<double>, static_cast<double (*)(double)>(std::rint)); }

TEST(CcmathRoundingConformanceTests, RintMatchesStdAllModesFloat)
{ ccm::test::ExpectFpUnaryOverMatchesStdAllModes(ccm::test::samples::kNearbyIntProbeFloat, ccm::rint<float>, static_cast<float (*)(float)>(std::rint)); }

TEST(CcmathRoundingConformanceTests, CeilTruncRoundFloorIndependentOfMode)
{
	ccm::test::ExpectFpUnaryOverMatchesStdAllModes(ccm::test::samples::kNearbyIntProbeDouble, ccm::ceil<double>, static_cast<double (*)(double)>(std::ceil));
	ccm::test::ExpectFpUnaryOverMatchesStdAllModes(ccm::test::samples::kNearbyIntProbeDouble, ccm::trunc<double>, static_cast<double (*)(double)>(std::trunc));
	ccm::test::ExpectFpUnaryOverMatchesStdAllModes(ccm::test::samples::kNearestHalfwayDouble, ccm::round<double>, static_cast<double (*)(double)>(std::round));
	ccm::test::ExpectFpUnaryOverMatchesStdAllModes(ccm::test::samples::kNearbyIntProbeDouble, ccm::floor<double>, static_cast<double (*)(double)>(std::floor));
}
