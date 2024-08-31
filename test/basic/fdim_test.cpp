/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <gtest/gtest.h>

#include <ccmath/ccmath.hpp>
#include <cmath>
#include <limits>

namespace
{

	using testing::TestWithParam;
	using testing::ValuesIn;

	template <typename T>
	struct FDimTestParams
	{
		T x{};
		T y{};
		T expected{};
	};

	const std::vector<FDimTestParams<double>> kFDimDoubleTestParams{
		{1.0, 1.0, std::fdim(1.0, 1.0)},
		{2.0, 2.0, std::fdim(2.0, 2.0)},
		{4.0, 4.0, std::fdim(4.0, 4.0)},
		{8.0, 8.0, std::fdim(8.0, 8.0)},
		{16.0, 16.0, std::fdim(16.0, 16.0)},
		{32.0, 32.0, std::fdim(32.0, 32.0)},

		{1.0, 0.0, std::fdim(1.0, 0.0)},
		{0.0, 1.0, std::fdim(0.0, 1.0)},
		{0.0, 0.0, std::fdim(0.0, 0.0)},
		{-1.0, 1.0, std::fdim(-1.0, 1.0)},
		{1.0, -1.0, std::fdim(1.0, -1.0)},
		{-1.0, -1.0, std::fdim(-1.0, -1.0)},

		// Edge cases
		{std::numeric_limits<double>::infinity(), 1.0, std::fdim(std::numeric_limits<double>::infinity(), 1.0)},
		{1.0, std::numeric_limits<double>::infinity(), std::fdim(1.0, std::numeric_limits<double>::infinity())},
		{std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(),
		 std::fdim(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity())},
		{std::numeric_limits<double>::quiet_NaN(), 1.0, std::fdim(std::numeric_limits<double>::quiet_NaN(), 1.0)},
		{1.0, std::numeric_limits<double>::quiet_NaN(), std::fdim(1.0, std::numeric_limits<double>::quiet_NaN())},
		{std::numeric_limits<double>::max(), std::numeric_limits<double>::max() - 1.0,
		 std::fdim(std::numeric_limits<double>::max(), std::numeric_limits<double>::max() - 1.0)},
		{-std::numeric_limits<double>::max(), std::numeric_limits<double>::max(),
		 std::fdim(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max())},
	};

	const std::vector<FDimTestParams<float>> kFDimFloatTestParams{
		{1.0F, 1.0F, std::fdim(1.0F, 1.0F)},
		{2.0F, 2.0F, std::fdim(2.0F, 2.0F)},
		{4.0F, 4.0F, std::fdim(4.0F, 4.0F)},
		{8.0F, 8.0F, std::fdim(8.0F, 8.0F)},
		{16.0F, 16.0F, std::fdim(16.0F, 16.0F)},
		{32.0F, 32.0F, std::fdim(32.0F, 32.0F)},

		{1.0F, 0.0F, std::fdim(1.0F, 0.0F)},
		{0.0F, 1.0F, std::fdim(0.0F, 1.0F)},
		{0.0F, 0.0F, std::fdim(0.0F, 0.0F)},
		{-1.0F, 1.0F, std::fdim(-1.0F, 1.0F)},
		{1.0F, -1.0F, std::fdim(1.0F, -1.0F)},
		{-1.0F, -1.0F, std::fdim(-1.0F, -1.0F)},

		// Edge cases
		{std::numeric_limits<float>::infinity(), 1.0F, std::fdim(std::numeric_limits<float>::infinity(), 1.0F)},
		{1.0F, std::numeric_limits<float>::infinity(), std::fdim(1.0F, std::numeric_limits<float>::infinity())},
		{std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(),
		 std::fdim(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity())},
		{std::numeric_limits<float>::quiet_NaN(), 1.0F, std::fdim(std::numeric_limits<float>::quiet_NaN(), 1.0F)},
		{1.0F, std::numeric_limits<float>::quiet_NaN(), std::fdim(1.0F, std::numeric_limits<float>::quiet_NaN())},
		{std::numeric_limits<float>::max(), std::numeric_limits<float>::max() - 1.0F,
		 std::fdim(std::numeric_limits<float>::max(), std::numeric_limits<float>::max() - 1.0F)},
		{-std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
		 std::fdim(-std::numeric_limits<float>::max(), std::numeric_limits<float>::max())},
	};

	const std::vector<FDimTestParams<long double>> kFDimLongDoubleTestParams{
		{1.0L, 1.0L, std::fdim(1.0L, 1.0L)},
		{2.0L, 2.0L, std::fdim(2.0L, 2.0L)},
		{4.0L, 4.0L, std::fdim(4.0L, 4.0L)},
		{8.0L, 8.0L, std::fdim(8.0L, 8.0L)},
		{16.0L, 16.0L, std::fdim(16.0L, 16.0L)},
		{32.0L, 32.0L, std::fdim(32.0L, 32.0L)},

		{1.0L, 0.0L, std::fdim(1.0L, 0.0L)},
		{0.0L, 1.0L, std::fdim(0.0L, 1.0L)},
		{0.0L, 0.0L, std::fdim(0.0L, 0.0L)},
		{-1.0L, 1.0L, std::fdim(-1.0L, 1.0L)},
		{1.0L, -1.0L, std::fdim(1.0L, -1.0L)},
		{-1.0L, -1.0L, std::fdim(-1.0L, -1.0L)},

		// Edge cases
		{std::numeric_limits<long double>::infinity(), 1.0L, std::fdim(std::numeric_limits<long double>::infinity(), 1.0L)},
		{1.0L, std::numeric_limits<long double>::infinity(), std::fdim(1.0L, std::numeric_limits<long double>::infinity())},
		{std::numeric_limits<long double>::infinity(), std::numeric_limits<long double>::infinity(),
		 std::fdim(std::numeric_limits<long double>::infinity(), std::numeric_limits<long double>::infinity())},
		{std::numeric_limits<long double>::quiet_NaN(), 1.0L, std::fdim(std::numeric_limits<long double>::quiet_NaN(), 1.0L)},
		{1.0L, std::numeric_limits<long double>::quiet_NaN(), std::fdim(1.0L, std::numeric_limits<long double>::quiet_NaN())},
		{std::numeric_limits<long double>::max(), std::numeric_limits<long double>::max() - 1.0L,
		 std::fdim(std::numeric_limits<long double>::max(), std::numeric_limits<long double>::max() - 1.0L)},
		{-std::numeric_limits<long double>::max(), std::numeric_limits<long double>::max(),
		 std::fdim(-std::numeric_limits<long double>::max(), std::numeric_limits<long double>::max())},
	};

} // namespace

class CcmathFDimDoubleTests : public TestWithParam<FDimTestParams<double>>
{
};
class CcmathFDimFloatTests : public TestWithParam<FDimTestParams<float>>
{
};
class CcmathFDimLongDoubleTests : public TestWithParam<FDimTestParams<long double>>
{
};

INSTANTIATE_TEST_SUITE_P(FDimDoubleTests, CcmathFDimDoubleTests, ValuesIn(kFDimDoubleTestParams));
INSTANTIATE_TEST_SUITE_P(FDimFloatTests, CcmathFDimFloatTests, ValuesIn(kFDimFloatTestParams));
INSTANTIATE_TEST_SUITE_P(FDimLongDoubleTests, CcmathFDimLongDoubleTests, ValuesIn(kFDimLongDoubleTestParams));

TEST_P(CcmathFDimDoubleTests, FDimWithDoubleReturnsApropriateValue)
{
	// Arrange
	const auto & param{GetParam()};

	// Act
	const auto actual{ccm::fdim(param.x, param.y)};

	// Assert
	if (std::isnan(param.expected))
	{
		EXPECT_TRUE(std::isnan(actual)) << "ccm::fdim(" << param.x << ", " << param.y << ") expected to be NaN. Instead got " << actual << ".";
	}
	else
	{
		EXPECT_EQ(actual, param.expected) << "ccm::fdim(" << param.x << ", " << param.y << ") expected to equal " << param.expected << ". Instead got "
										  << actual << ".";
	}
}

TEST_P(CcmathFDimFloatTests, FDimWithFloatReturnsAppropriateValue)
{
	// Arrange
	const auto & param{GetParam()};

	// Act
	const auto actual{ccm::fdim(param.x, param.y)};

	// Assert
	if (std::isnan(param.expected))
	{
		EXPECT_TRUE(std::isnan(actual)) << "ccm::fdim(" << param.x << ", " << param.y << ") expected to be NaN. Instead got " << actual << ".";
	}
	else
	{
		EXPECT_EQ(actual, param.expected) << "ccm::fdim(" << param.x << ", " << param.y << ") expected to equal " << param.expected << ". Instead got "
										  << actual << ".";
	}
}

TEST_P(CcmathFDimLongDoubleTests, FDimWithLongDoubleReturnsAppropriateValue)
{
	// Arrange
	const auto & param{GetParam()};

	// Act
	const auto actual{ccm::fdim(param.x, param.y)};

	// Assert
	if (std::isnan(param.expected))
	{
		EXPECT_TRUE(std::isnan(actual)) << "ccm::fdim(" << param.x << ", " << param.y << ") expected to be NaN. Instead got " << actual << ".";
	}
	else
	{
		EXPECT_EQ(actual, param.expected) << "ccm::fdim(" << param.x << ", " << param.y << ") expected to equal " << param.expected << ". Instead got "
										  << actual << ".";
	}
}

TEST(CcmathBasicTests, FdimCanBeEvaluatedAtCompileTime)
{
	// Act
	constexpr auto fdim{ccm::fdim(1.0, 1.0)};

	// Assert
	static_assert(fdim == 0.0, "fdim has failed testing that it is static_assert-able!");
}
