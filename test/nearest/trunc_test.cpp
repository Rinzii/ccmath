/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <gtest/gtest.h>

#include "ccmath/ccmath.hpp"

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

	template <typename T>
	struct TruncTestParams
	{
		T input{};
		T expected{};
	};

	const std::vector<TruncTestParams<double>> kTruncDoubleTestsParams{

		// Basic double values
		{1.0, std::trunc(1.0)},
		{1.5, std::trunc(1.5)},
		{1.9, std::trunc(1.9)},
		{-1.0, std::trunc(-1.0)},
		{-1.5, std::trunc(-1.5)},
		{-1.9, std::trunc(-1.9)},

		// Infinity values
		{std::numeric_limits<double>::infinity(), std::trunc(std::numeric_limits<double>::infinity())},
		{-std::numeric_limits<double>::infinity(), std::trunc(-std::numeric_limits<double>::infinity())},

		// Zero values
		{0.0, std::trunc(0.0)},
		{-0.0, std::trunc(-0.0)},

		// Fractional values
		{0.5, std::trunc(0.5)},
		{-0.5, std::trunc(-0.5)},
		{0.9, std::trunc(0.9)},
		{-0.9, std::trunc(-0.9)},

		// Very close to whole numbers
		{0.9999999999999999, std::trunc(0.9999999999999999)},
		{-0.9999999999999999, std::trunc(-0.9999999999999999)},
		{1.0000000000000001, std::trunc(1.0000000000000001)},
		{-1.0000000000000001, std::trunc(-1.0000000000000001)}};

	const std::vector<TruncTestParams<float>> kTruncFloatTestParams{

		// Basic float values
		{1.0f, std::truncf(1.0f)},
		{1.5f, std::truncf(1.5f)},
		{1.9f, std::truncf(1.9f)},
		{-1.0f, std::truncf(-1.0f)},
		{-1.5f, std::truncf(-1.5f)},
		{-1.9f, std::truncf(-1.9f)},

		// Infinity values
		{std::numeric_limits<float>::infinity(), std::truncf(std::numeric_limits<float>::infinity())},
		{-std::numeric_limits<float>::infinity(), std::truncf(-std::numeric_limits<float>::infinity())},

		// Zero values
		{0.0f, std::truncf(0.0f)},
		{-0.0f, std::truncf(-0.0f)},

		// Fractional values
		{0.5f, std::truncf(0.5f)},
		{-0.5f, std::truncf(-0.5f)},
		{0.9f, std::truncf(0.9f)},
		{-0.9f, std::truncf(-0.9f)},

		// Very close to whole numbers
		{0.9999999999999999f, std::truncf(0.9999999999999999f)},
		{-0.9999999999999999f, std::truncf(-0.9999999999999999f)},

		{30.508474576271183309F, std::truncf(30.508474576271183309F)},
	};

	const std::vector<TruncTestParams<long double>> kTruncLongDoubleTestParams{

		// Basic long double values
		{1.0L, std::truncl(1.0L)},
		{1.5L, std::truncl(1.5L)},
		{1.9L, std::truncl(1.9L)},
		{-1.0L, std::truncl(-1.0L)},
		{-1.5L, std::truncl(-1.5L)},
		{-1.9L, std::truncl(-1.9L)},

		// Infinity values
		{std::numeric_limits<long double>::infinity(), std::truncl(std::numeric_limits<long double>::infinity())},
		{-std::numeric_limits<long double>::infinity(), std::truncl(-std::numeric_limits<long double>::infinity())},

		// Fractional values
		{0.0L, std::truncl(0.0L)},
		{-0.0L, std::truncl(-0.0L)},
		{0.5L, std::truncl(0.5L)},
		{-0.5L, std::truncl(-0.5L)},
		{0.9L, std::truncl(0.9L)},
		{-0.9L, std::truncl(-0.9L)},

		// Very close to whole numbers
		{0.9999999999999999L, std::truncl(0.9999999999999999L)},
		{-0.9999999999999999L, std::truncl(-0.9999999999999999L)},
	};

} // namespace

// TODO: Find a way to test all of the different ways the function may behave internally to work based on the provided compiler.

class CcmathTruncDoubleTests : public TestWithParam<TruncTestParams<double>>
{
};
class CcmathTruncFloatTests : public TestWithParam<TruncTestParams<float>>
{
};
class CcmathTruncLongDoubleTests : public TestWithParam<TruncTestParams<long double>>
{
};

INSTANTIATE_TEST_SUITE_P(TruncDoubleTests, CcmathTruncDoubleTests, ValuesIn(kTruncDoubleTestsParams));
INSTANTIATE_TEST_SUITE_P(TruncFloatTests, CcmathTruncFloatTests, ValuesIn(kTruncFloatTestParams));
INSTANTIATE_TEST_SUITE_P(TruncLongDoubleTests, CcmathTruncLongDoubleTests, ValuesIn(kTruncLongDoubleTestParams));

TEST_P(CcmathTruncDoubleTests, Trunc)
{
	const auto param{GetParam()};
	const auto actual{ccm::trunc(param.input)};
	EXPECT_EQ(actual, param.expected) << "ccm::trunc(" << param.input << ") expected to equal " << param.expected << ". Instead got " << actual << ".";
}

TEST_P(CcmathTruncFloatTests, TruncF)
{
	const auto param{GetParam()};
	const auto actual{ccm::truncf(param.input)};
	EXPECT_EQ(actual, param.expected) << "ccm::truncf(" << param.input << ") expected to equal " << param.expected << ". Instead got " << actual << ".";
}

TEST_P(CcmathTruncLongDoubleTests, TruncL)
{
	const auto param{GetParam()};
	const auto actual{ccm::truncl(param.input)};
	EXPECT_EQ(actual, param.expected) << "ccm::truncl(" << param.input << ") expected to equal " << param.expected << ". Instead got " << actual << ".";
}

TEST(CcmathNearestTests, CcmTruncTestNanValues)
{
	// Check if ccm::trunc and std::trunc return NaN for positive NaN
	EXPECT_EQ(std::isnan(ccm::trunc(std::numeric_limits<double>::quiet_NaN())), std::isnan(std::trunc(std::numeric_limits<double>::quiet_NaN())));

	// Check if ccm::trunc and std::trunc have the same sign bit for positive NaN
	EXPECT_EQ(std::signbit(ccm::trunc(std::numeric_limits<double>::quiet_NaN())), std::signbit(std::trunc(std::numeric_limits<double>::quiet_NaN())));

	// Check if ccm::trunc and std::trunc return NaN for negative NaN
	EXPECT_EQ(std::isnan(ccm::trunc(-std::numeric_limits<double>::quiet_NaN())), std::isnan(std::trunc(-std::numeric_limits<double>::quiet_NaN())));

	// Check if ccm::trunc and std::trunc have the same sign bit for negative NaN
	EXPECT_EQ(std::signbit(ccm::trunc(-std::numeric_limits<double>::quiet_NaN())), std::signbit(std::trunc(-std::numeric_limits<double>::quiet_NaN())));

	// Google Test is apparently incapable of comparing NaNs or I do not know enough about gtest to find a solution. I've though personally validated that
	// ccm::signbit handles NaNs correctly
	// EXPECT_EQ(ccm::trunc(std::numeric_limits<double>::quiet_NaN()), std::trunc(std::numeric_limits<double>::quiet_NaN()));
	// EXPECT_EQ(ccm::trunc(-std::numeric_limits<double>::quiet_NaN()), std::trunc(-std::numeric_limits<double>::quiet_NaN()));

	EXPECT_TRUE(std::isnan(ccm::trunc(std::nan(""))));
	EXPECT_TRUE(std::isnan(ccm::trunc(-std::nan(""))));
}

TEST(CcmathNearestTests, CcmTruncCanBeEvaluatedAtCompileTime)
{
	constexpr auto trunc{ccm::trunc(1.0)};
	static_assert(trunc == 1.0);
}

TEST(CcmathNearestTests, CcmTruncfCanBeEvaluatedAtCompileTime)
{
	constexpr auto truncf{ccm::truncf(1.0f)};
	static_assert(truncf == 1.0f);
}

#ifndef CLANG_LINUX
TEST(CcmathNearestTests, CcmTrunclCanBeEvaluatedAtCompileTime)
{
	constexpr auto truncl{ccm::truncl(1.0L)};
	static_assert(truncl == 1.0L);
}
#endif

// Undefining clang linux macro
#ifdef CLANG_LINUX
#undef CLANG_LINUX
#endif
