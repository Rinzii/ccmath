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

// Disabling test case ccm::truncl if run on clang linux.
#ifdef __clang__
	#ifdef __linux__
		#define CLANG_LINUX
	#endif
#endif

namespace
{

	using testing::TestWithParam;
	using testing::ValuesIn;

	template <typename T> struct TruncTestParams
	{
		T input{};
		T expected{};
	};

	const std::vector<TruncTestParams<double>> kTruncDoubleTestsParams{

		// Basic double values
		{ 1.0, std::trunc(1.0) },
		{ 1.5, std::trunc(1.5) },
		{ 1.9, std::trunc(1.9) },
		{ -1.0, std::trunc(-1.0) },
		{ -1.5, std::trunc(-1.5) },
		{ -1.9, std::trunc(-1.9) },

		// Infinity values
		{ std::numeric_limits<double>::infinity(), std::trunc(std::numeric_limits<double>::infinity()) },
		{ -std::numeric_limits<double>::infinity(), std::trunc(-std::numeric_limits<double>::infinity()) },

		// Zero values
		{ 0.0, std::trunc(0.0) },
		{ -0.0, std::trunc(-0.0) },

		// Fractional values
		{ 0.5, std::trunc(0.5) },
		{ -0.5, std::trunc(-0.5) },
		{ 0.9, std::trunc(0.9) },
		{ -0.9, std::trunc(-0.9) },

		// Very close to whole numbers
		{ 0.9999999999999999, std::trunc(0.9999999999999999) },
		{ -0.9999999999999999, std::trunc(-0.9999999999999999) },
		{ 1.0000000000000001, std::trunc(1.0000000000000001) },
		{ -1.0000000000000001, std::trunc(-1.0000000000000001) }
	};

	const std::vector<TruncTestParams<float>> kTruncFloatTestParams{

		// Basic float values
		{ 1.0f, std::truncf(1.0f) },
		{ 1.5f, std::truncf(1.5f) },
		{ 1.9f, std::truncf(1.9f) },
		{ -1.0f, std::truncf(-1.0f) },
		{ -1.5f, std::truncf(-1.5f) },
		{ -1.9f, std::truncf(-1.9f) },

		// Infinity values
		{ std::numeric_limits<float>::infinity(), std::truncf(std::numeric_limits<float>::infinity()) },
		{ -std::numeric_limits<float>::infinity(), std::truncf(-std::numeric_limits<float>::infinity()) },

		// Zero values
		{ 0.0f, std::truncf(0.0f) },
		{ -0.0f, std::truncf(-0.0f) },

		// Fractional values
		{ 0.5f, std::truncf(0.5f) },
		{ -0.5f, std::truncf(-0.5f) },
		{ 0.9f, std::truncf(0.9f) },
		{ -0.9f, std::truncf(-0.9f) },

		// Very close to whole numbers
		{ 0.9999999999999999f, std::truncf(0.9999999999999999f) },
		{ -0.9999999999999999f, std::truncf(-0.9999999999999999f) },

		{ 30.508474576271183309F, std::truncf(30.508474576271183309F) },
	};

	const std::vector<TruncTestParams<long double>> kTruncLongDoubleTestParams{

		// Basic long double values
		{ 1.0L, std::truncl(1.0L) },
		{ 1.5L, std::truncl(1.5L) },
		{ 1.9L, std::truncl(1.9L) },
		{ -1.0L, std::truncl(-1.0L) },
		{ -1.5L, std::truncl(-1.5L) },
		{ -1.9L, std::truncl(-1.9L) },

		// Infinity values
		{ std::numeric_limits<long double>::infinity(), std::truncl(std::numeric_limits<long double>::infinity()) },
		{ -std::numeric_limits<long double>::infinity(), std::truncl(-std::numeric_limits<long double>::infinity()) },

		// Fractional values
		{ 0.0L, std::truncl(0.0L) },
		{ -0.0L, std::truncl(-0.0L) },
		{ 0.5L, std::truncl(0.5L) },
		{ -0.5L, std::truncl(-0.5L) },
		{ 0.9L, std::truncl(0.9L) },
		{ -0.9L, std::truncl(-0.9L) },

		// Very close to whole numbers
		{ 0.9999999999999999L, std::truncl(0.9999999999999999L) },
		{ -0.9999999999999999L, std::truncl(-0.9999999999999999L) },
	};

} // namespace

// TODO(IanP): Add coverage for compiler-specific trunc dispatch paths when they diverge.

class CcmathTruncDoubleTests : public TestWithParam<TruncTestParams<double>>
{};
class CcmathTruncFloatTests : public TestWithParam<TruncTestParams<float>>
{};
class CcmathTruncLongDoubleTests : public TestWithParam<TruncTestParams<long double>>
{};

INSTANTIATE_TEST_SUITE_P(TruncDoubleTests, CcmathTruncDoubleTests, ValuesIn(kTruncDoubleTestsParams));
INSTANTIATE_TEST_SUITE_P(TruncFloatTests, CcmathTruncFloatTests, ValuesIn(kTruncFloatTestParams));
INSTANTIATE_TEST_SUITE_P(TruncLongDoubleTests, CcmathTruncLongDoubleTests, ValuesIn(kTruncLongDoubleTestParams));

TEST_P(CcmathTruncDoubleTests, Trunc)
{
	const auto param{ GetParam() };
	const auto actual{ ccm::trunc(param.input) };
	ccm::test::ExpectSameAsStd(actual, param.expected);
}

TEST_P(CcmathTruncFloatTests, TruncF)
{
	const auto param{ GetParam() };
	const auto actual{ ccm::truncf(param.input) };
	ccm::test::ExpectSameAsStd(actual, param.expected);
}

TEST_P(CcmathTruncLongDoubleTests, TruncL)
{
	const auto param{ GetParam() };
	const auto actual{ ccm::truncl(param.input) };
	ccm::test::ExpectSameAsStd(actual, param.expected);
}

TEST(CcmathNearestTests, CcmTruncTestNanValues)
{
	ccm::test::ExpectUnaryMatchesStd(std::numeric_limits<double>::quiet_NaN(), ccm::trunc<double>, static_cast<double (*)(double)>(std::trunc));
	ccm::test::ExpectUnaryMatchesStd(-std::numeric_limits<double>::quiet_NaN(), ccm::trunc<double>, static_cast<double (*)(double)>(std::trunc));
	ccm::test::ExpectUnaryMatchesStd(std::nan(""), ccm::trunc<double>, static_cast<double (*)(double)>(std::trunc));
	ccm::test::ExpectUnaryMatchesStd(-std::nan(""), ccm::trunc<double>, static_cast<double (*)(double)>(std::trunc));
}

TEST(CcmathNearestTests, CcmTruncCanBeEvaluatedAtCompileTime)
{
	constexpr auto trunc{ ccm::trunc(1.0) };
	static_assert(trunc == 1.0);
}

TEST(CcmathNearestTests, CcmTruncfCanBeEvaluatedAtCompileTime)
{
	constexpr auto truncf{ ccm::truncf(1.0f) };
	static_assert(truncf == 1.0f);
}

#ifndef CLANG_LINUX
TEST(CcmathNearestTests, CcmTrunclCanBeEvaluatedAtCompileTime)
{
	constexpr auto truncl{ ccm::truncl(1.0L) };
	static_assert(truncl == 1.0L);
}
#endif

// Undefining clang linux macro
#ifdef CLANG_LINUX
	#undef CLANG_LINUX
#endif
