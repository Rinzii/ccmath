/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <ccmath/math/nearest/nearbyint.hpp>
#include <gtest/gtest.h>
#include <cmath>
#include <vector>

// Disabling test case ccm::nearbyintl if run on clang linux.
#ifdef __clang__
	#ifdef __linux__
		#define CLANG_LINUX
	#endif
#endif

namespace
{
	using testing::TestWithParam;
	using testing::ValuesIn;

	template <typename InputType, typename OutputType = InputType>
	struct NearbyIntTestParams
	{
		InputType input{};
		OutputType expected{};
	};

	const std::vector<NearbyIntTestParams<int, double>> kNearbyIntIntegerTestParams{
		// Zero values
		{0, std::nearbyint(0)},

		// Positive values
		{1, std::nearbyint(1)},
		{2, std::nearbyint(2)},
		{4, std::nearbyint(4)},
		{9, std::nearbyint(9)},
		{123, std::nearbyint(123)},

		// Negative values
		{-1, std::nearbyint(-1)},
		{-2, std::nearbyint(-2)},
		{-4, std::nearbyint(-4)},
		{-9, std::nearbyint(-9)},
		{-123, std::nearbyint(-123)},

		// Edge cases
		{std::numeric_limits<int>::max(), std::nearbyint(std::numeric_limits<int>::max())},
		{std::numeric_limits<int>::min(), std::nearbyint(std::numeric_limits<int>::min())},
	};

	const std::vector<NearbyIntTestParams<float>> kNearbyIntFloatTestParams{
		// Zero values
		{0.0f, std::nearbyint(0.0f)},
		{-0.0f, std::nearbyint(0.0f)},

		// Positive values
		{0.05f, std::nearbyint(0.05f)},
		{0.5f, std::nearbyint(0.5f)},
		{1.0f, std::nearbyint(1.0f)},
		{1.5f, std::nearbyint(1.5f)},
		{123.0f, std::nearbyint(123.0f)},
		{123.1f, std::nearbyint(123.1f)},

		// Negative values
		{-0.05f, std::nearbyint(-0.05f)},
		{-0.5f, std::nearbyint(-0.5f)},
		{-1.0f, std::nearbyint(-1.0f)},
		{-1.5f, std::nearbyint(-1.5f)},
		{-123.0f, std::nearbyint(-123.0f)},
		{-123.1f, std::nearbyint(-123.1f)},

		// Edge cases
		{std::numeric_limits<float>::max(), std::nearbyint(std::numeric_limits<float>::max())},
		{std::numeric_limits<float>::min(), std::nearbyint(std::numeric_limits<float>::min())},
		{std::numeric_limits<float>::denorm_min(), std::nearbyint(std::numeric_limits<float>::denorm_min())},
		{std::numeric_limits<float>::infinity(), std::nearbyint(std::numeric_limits<float>::infinity())},
		{std::numeric_limits<float>::quiet_NaN(), std::nearbyint(std::numeric_limits<float>::quiet_NaN())},

		{-std::numeric_limits<float>::max(), std::nearbyint(-std::numeric_limits<float>::max())},
		{-std::numeric_limits<float>::min(), std::nearbyint(-std::numeric_limits<float>::min())},
		{-std::numeric_limits<float>::denorm_min(), -std::nearbyint(std::numeric_limits<float>::denorm_min())},
		{-std::numeric_limits<float>::infinity(), -std::nearbyint(std::numeric_limits<float>::infinity())},
		{-std::numeric_limits<float>::quiet_NaN(), -std::nearbyint(std::numeric_limits<float>::quiet_NaN())},
	};

	const std::vector<NearbyIntTestParams<double>> kNearbyIntDoubleTestParams{
		// Zero values
		{0.0, std::nearbyint(0.0)},
		{-0.0, std::nearbyint(0.0)},

		// Positive values
		{0.05, std::nearbyint(0.05)},
		{0.5, std::nearbyint(0.5)},
		{1.0, std::nearbyint(1.0)},
		{1.5, std::nearbyint(1.5)},
		{123.0, std::nearbyint(123.0)},
		{123.1, std::nearbyint(123.1)},

		// Negative values
		{-0.05, std::nearbyint(-0.05)},
		{-0.5, std::nearbyint(-0.5)},
		{-1.0, std::nearbyint(-1.0)},
		{-1.5, std::nearbyint(-1.5)},
		{-123.0, std::nearbyint(-123.0)},
		{-123.1, std::nearbyint(-123.1)},

		// Edge cases
		{std::numeric_limits<double>::max(), std::nearbyint(std::numeric_limits<double>::max())},
		{std::numeric_limits<double>::min(), std::nearbyint(std::numeric_limits<double>::min())},
		{std::numeric_limits<double>::denorm_min(), std::nearbyint(std::numeric_limits<double>::denorm_min())},
		{std::numeric_limits<double>::infinity(), std::nearbyint(std::numeric_limits<double>::infinity())},
		{std::numeric_limits<double>::quiet_NaN(), std::nearbyint(std::numeric_limits<double>::quiet_NaN())},

		{-std::numeric_limits<double>::max(), std::nearbyint(-std::numeric_limits<double>::max())},
		{-std::numeric_limits<double>::min(), std::nearbyint(-std::numeric_limits<double>::min())},
		{-std::numeric_limits<double>::denorm_min(), -std::nearbyint(std::numeric_limits<double>::denorm_min())},
		{-std::numeric_limits<double>::infinity(), -std::nearbyint(std::numeric_limits<double>::infinity())},
		{-std::numeric_limits<double>::quiet_NaN(), -std::nearbyint(std::numeric_limits<double>::quiet_NaN())},
	};

	const std::vector<NearbyIntTestParams<long double>> kNearbyIntLongDoubleTestParams{
		// Zero values
		{0.0L, std::nearbyint(0.0L)},
		{-0.0L, std::nearbyint(0.0L)},

		// Positive values
		{0.05L, std::nearbyint(0.05L)},
		{0.5L, std::nearbyint(0.5L)},
		{1.0L, std::nearbyint(1.0L)},
		{1.5L, std::nearbyint(1.5L)},
		{123.0L, std::nearbyint(123.0L)},
		{123.1L, std::nearbyint(123.1L)},

		// Negative values
		{-0.05L, std::nearbyint(-0.05L)},
		{-0.5L, std::nearbyint(-0.5L)},
		{-1.0L, std::nearbyint(-1.0L)},
		{-1.5L, std::nearbyint(-1.5L)},
		{-123.0L, std::nearbyint(-123.0L)},
		{-123.1L, std::nearbyint(-123.1L)},

		// Edge cases
		{std::numeric_limits<long double>::max(), std::nearbyint(std::numeric_limits<long double>::max())},
		{std::numeric_limits<long double>::min(), std::nearbyint(std::numeric_limits<long double>::min())},
		{std::numeric_limits<long double>::denorm_min(), std::nearbyint(std::numeric_limits<long double>::denorm_min())},
		{std::numeric_limits<long double>::infinity(), std::nearbyint(std::numeric_limits<long double>::infinity())},
		{std::numeric_limits<long double>::quiet_NaN(), std::nearbyint(std::numeric_limits<long double>::quiet_NaN())},

		{-std::numeric_limits<long double>::max(), std::nearbyint(-std::numeric_limits<long double>::max())},
		{-std::numeric_limits<long double>::min(), std::nearbyint(-std::numeric_limits<long double>::min())},
		{-std::numeric_limits<long double>::denorm_min(), -std::nearbyint(std::numeric_limits<long double>::denorm_min())},
		{-std::numeric_limits<long double>::infinity(), -std::nearbyint(std::numeric_limits<long double>::infinity())},
		{-std::numeric_limits<long double>::quiet_NaN(), -std::nearbyint(std::numeric_limits<long double>::quiet_NaN())},
	};

} // namespace

class CcmathNearbyIntIntegerTests : public TestWithParam<NearbyIntTestParams<int, double>>
{
};

class CcmathNearbyIntDoubleTests : public TestWithParam<NearbyIntTestParams<double>>
{
};

class CcmathNearbyIntFloatTests : public TestWithParam<NearbyIntTestParams<float>>
{
};

class CcmathNearbyIntLongDoubleTests : public TestWithParam<NearbyIntTestParams<long double>>
{
};

INSTANTIATE_TEST_SUITE_P(NearestIntIntegerTests, CcmathNearbyIntIntegerTests, ValuesIn(kNearbyIntIntegerTestParams));
INSTANTIATE_TEST_SUITE_P(NearestIntDoubleTests, CcmathNearbyIntDoubleTests, ValuesIn(kNearbyIntDoubleTestParams));
INSTANTIATE_TEST_SUITE_P(NearbyIntFloatTests, CcmathNearbyIntFloatTests, ValuesIn(kNearbyIntFloatTestParams));
INSTANTIATE_TEST_SUITE_P(NearbyIntLongDoubleTests, CcmathNearbyIntLongDoubleTests, ValuesIn(kNearbyIntLongDoubleTestParams));

TEST_P(CcmathNearbyIntIntegerTests, NearbyIntForIntegerReturnsExpectedValue)
{
	// Arrange
	const auto & param{GetParam()};

	// Act
	const auto actual{ccm::nearbyint(param.input)};

	// Assert
	if (std::isnan(param.expected)) { EXPECT_TRUE(std::isnan(actual)); }
	else
	{
		EXPECT_EQ(param.expected, actual) << "ccm::nearbyint(" << param.input << ") expected to equal " << param.expected << ". Instead got " << actual << ".";
	}
}

TEST_P(CcmathNearbyIntDoubleTests, NearbyIntForDoubleReturnsExpectedValue)
{
	// Arrange
	const auto & param{GetParam()};

	// Act
	const auto actual{ccm::nearbyint(param.input)};

	// Assert
	if (std::isnan(param.expected)) { EXPECT_TRUE(std::isnan(actual)); }
	else
	{
		EXPECT_EQ(param.expected, actual) << "ccm::nearbyint(" << param.input << ") expected to equal " << param.expected << ". Instead got " << actual << ".";
	}
}

TEST_P(CcmathNearbyIntFloatTests, NearbyIntForFloatReturnsExpectedValue)
{
	// Arrange
	const auto & param{GetParam()};

	// Act
	const auto actual{ccm::nearbyint(param.input)};

	// Assert
	if (std::isnan(param.expected)) { EXPECT_TRUE(std::isnan(actual)); }
	else
	{
		EXPECT_EQ(param.expected, actual) << "ccm::nearbyint(" << param.input << ") expected to equal " << param.expected << ". Instead got " << actual << ".";
	}
}

TEST_P(CcmathNearbyIntFloatTests, NearbyIntFForFloatReturnsExpectedValue)
{
	// Arrange
	const auto & param{GetParam()};

	// Act
	const auto actual{ccm::nearbyintf(param.input)};

	// Assert
	if (std::isnan(param.expected)) { EXPECT_TRUE(std::isnan(actual)); }
	else
	{
		EXPECT_EQ(param.expected, actual) << "ccm::nearbyintf(" << param.input << ") expected to equal " << param.expected << ". Instead got " << actual << ".";
	}
}

TEST_P(CcmathNearbyIntLongDoubleTests, NearbyIntForLongDoubleReturnsExpectedValue)
{
	// Arrange
	const auto & param{GetParam()};

	// Act
	const auto actual{ccm::nearbyint(param.input)};

	// Assert
	if (std::isnan(param.expected)) { EXPECT_TRUE(std::isnan(actual)); }
	else
	{
		EXPECT_EQ(param.expected, actual) << "ccm::nearbyint(" << param.input << ") expected to equal " << param.expected << ". Instead got " << actual << ".";
	}
}

TEST_P(CcmathNearbyIntLongDoubleTests, NearbyIntLForLongDoubleReturnsExpectedValue)
{
	// Arrange
	const auto & param{GetParam()};

	// Act
	const auto actual{ccm::nearbyintl(param.input)};

	// Assert
	if (std::isnan(param.expected)) { EXPECT_TRUE(std::isnan(actual)); }
	else
	{
		EXPECT_EQ(param.expected, actual) << "ccm::nearbyintl(" << param.input << ") expected to equal " << param.expected << ". Instead got " << actual << ".";
	}
}

TEST(CcmathNearestTests, CcmNearbyIntCanBeEvaluatedAtCompileTime)
{
	constexpr auto nearbyint{ccm::nearbyint(1.0)};
	static_assert(nearbyint == 1.0);
}

TEST(CcmathNearestTests, CcmNearbyIntFCanBeEvaluatedAtCompileTime)
{
	constexpr auto nearbyintf{ccm::nearbyintf(1.0f)};
	static_assert(nearbyintf == 1.0);
}

#ifndef CLANG_LINUX
TEST(CcmathNearestTests, CcmNearbyIntLCanBeEvaluatedAtCompileTime)
{
	constexpr auto nearbyintl{ccm::nearbyintl(1.0L)};
	static_assert(nearbyintl == 1.0);
}
#endif

// Remove #define
#ifdef CLANG_LINUX
	#undef CLANG_LINUX
#endif
