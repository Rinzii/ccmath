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

TEST(CcmathFmaRoundingConformanceTests, FmafMatchesStdAllModesStructuredCases)
{
	const auto ccm_fmaf = [](float x, float y, float z) { return ccm::fmaf(runtime_value(x), runtime_value(y), runtime_value(z)); };
	const auto std_fmaf = [](float x, float y, float z) {
		return static_cast<float (*)(float, float, float)>(std::fma)(runtime_value(x), runtime_value(y), runtime_value(z));
	};

	ccm::test::ExpectFpTernaryOverMatchesStdAllModes(ccm::test::samples::kFmaAllModesFloatCases, ccm_fmaf, std_fmaf);
}

TEST(CcmathFmaRoundingConformanceTests, FmaMatchesStdAllModesStructuredCases)
{
	const auto ccm_fma = [](double x, double y, double z) { return ccm::fma(runtime_value(x), runtime_value(y), runtime_value(z)); };
	const auto std_fma = [](double x, double y, double z) {
		return static_cast<double (*)(double, double, double)>(std::fma)(runtime_value(x), runtime_value(y), runtime_value(z));
	};

	ccm::test::ExpectFpTernaryOverMatchesStdAllModes(ccm::test::samples::kFmaAllModesDoubleCases, ccm_fma, std_fma);
}

TEST(CcmathFmaRoundingConformanceTests, ExactCancellationZeroSignMatchesStdAllModes)
{
	const auto ccm_float = [](float x, float y, float z) { return ccm::fmaf(runtime_value(x), runtime_value(y), runtime_value(z)); };
	const auto std_float = [](float x, float y, float z) {
		return static_cast<float (*)(float, float, float)>(std::fma)(runtime_value(x), runtime_value(y), runtime_value(z));
	};
	const auto ccm_double = [](double x, double y, double z) { return ccm::fma(runtime_value(x), runtime_value(y), runtime_value(z)); };
	const auto std_double = [](double x, double y, double z) {
		return static_cast<double (*)(double, double, double)>(std::fma)(runtime_value(x), runtime_value(y), runtime_value(z));
	};

	ccm::test::ExpectFpTernaryMatchesStdAllModes(1.0F, 1.0F, -1.0F, ccm_float, std_float);
	ccm::test::ExpectFpTernaryMatchesStdAllModes(-1.0F, -1.0F, -1.0F, ccm_float, std_float);
	ccm::test::ExpectFpTernaryMatchesStdAllModes(1.0, 1.0, -1.0, ccm_double, std_double);
	ccm::test::ExpectFpTernaryMatchesStdAllModes(-1.0, -1.0, -1.0, ccm_double, std_double);
}
