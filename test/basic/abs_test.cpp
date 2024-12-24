/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <gtest/gtest.h>
#include <ccmath/math/basic/abs.hpp>
#include <cmath>
#include <limits>

// NOLINTBEGIN

#if defined(_MSC_VER) && !defined(__clang__)
#include "ccmath/internal/predef/compiler_suppression/msvc_compiler_suppression.hpp"
// TODO: Look into this issue at a later date.
CCM_DISABLE_MSVC_WARNING(4756) // 4756: overflow in constant arithmetic - Not sure why this is happening
#endif

namespace
{

	using testing::TestWithParam;
	using testing::ValuesIn;

	template <typename T>
	struct AbsTestParams
	{
		T input{};
		T expected{};
	};

	const std::vector<AbsTestParams<double>> kAbsDoubleTestParams{

		// Floating point whole values
		{1.0, std::abs(1.0)},
		{-1.0, std::abs(-1.0)},
		{2.0, std::abs(2.0)},
		{-2.0, std::abs(-2.0)},
		{4.0, std::abs(4.0)},
		{-4.0, std::abs(-4.0)},
		{8.0, std::abs(8.0)},
		{-8.0, std::abs(-8.0)},
		{16.0, std::abs(16.0)},
		{-16.0, std::abs(-16.0)},
		{32.0, std::abs(32.0)},
		{-32.0, std::abs(-32.0)},

		// Testing zeroes
		{0.0, std::abs(0.0)},
		{-0.0, std::abs(-0.0)},

		// Floating point non-whole values
		{1.12345, std::abs(1.12345)},
		{-1.12345, std::abs(1.12345)},
		{74.5928, std::abs(74.5928)},
		{-74.5928, std::abs(-74.5928)},

		// Edge cases
		{std::numeric_limits<double>::min(), std::abs(std::numeric_limits<double>::min())},
		{std::numeric_limits<double>::max(), std::abs(std::numeric_limits<double>::max())},
		{std::numeric_limits<double>::denorm_min(), std::abs(std::numeric_limits<double>::denorm_min())},
		{-std::numeric_limits<double>::denorm_min(), std::abs(-std::numeric_limits<double>::denorm_min())},
		{std::numeric_limits<double>::infinity(), std::abs(std::numeric_limits<double>::infinity())},
		{-std::numeric_limits<double>::infinity(), std::abs(-std::numeric_limits<double>::infinity())},
		{-std::numeric_limits<double>::max() * 10, std::abs(-std::numeric_limits<double>::max() * 10)},
		{std::numeric_limits<double>::max() * 10, std::abs(std::numeric_limits<double>::max() * 10)},
		{std::numeric_limits<double>::quiet_NaN(), std::abs(std::numeric_limits<double>::quiet_NaN())},
	};

	const std::vector<AbsTestParams<float>> kAbsFloatTestParams{

		// Floating point whole values
		{1.0F, std::abs(1.0F)},
		{-1.0F, std::abs(-1.0F)},
		{2.0F, std::abs(2.0F)},
		{-2.0F, std::abs(-2.0F)},
		{4.0F, std::abs(4.0F)},
		{-4.0F, std::abs(-4.0F)},
		{8.0F, std::abs(8.0F)},
		{-8.0F, std::abs(-8.0F)},
		{16.0F, std::abs(16.0F)},
		{-16.0F, std::abs(-16.0F)},
		{32.0F, std::abs(32.0F)},
		{-32.0F, std::abs(-32.0F)},

		// Testing zeroes
		{0.0F, std::abs(0.0F)},
		{-0.0F, std::abs(-0.0F)},

		// Floating point non-whole values
		{1.12345F, std::abs(1.12345F)},
		{-1.12345F, std::abs(1.12345F)},
		{74.5928F, std::abs(74.5928F)},
		{-74.5928F, std::abs(-74.5928F)},

		// Edge cases
		{std::numeric_limits<float>::min(), std::abs(std::numeric_limits<float>::min())},
		{std::numeric_limits<float>::max(), std::abs(std::numeric_limits<float>::max())},
		{std::numeric_limits<float>::denorm_min(), std::abs(std::numeric_limits<float>::denorm_min())},
		{-std::numeric_limits<float>::denorm_min(), std::abs(-std::numeric_limits<float>::denorm_min())},
		{std::numeric_limits<float>::infinity(), std::abs(std::numeric_limits<float>::infinity())},
		{-std::numeric_limits<float>::infinity(), std::abs(-std::numeric_limits<float>::infinity())},
		{-std::numeric_limits<float>::max() * 10, std::abs(-std::numeric_limits<float>::max() * 10)},
		{std::numeric_limits<float>::max() * 10, std::abs(std::numeric_limits<float>::max() * 10)},
		{std::numeric_limits<float>::quiet_NaN(), std::abs(std::numeric_limits<float>::quiet_NaN())},
	};

	const std::vector<AbsTestParams<long double>> kAbsLongDoubleTestParams{

		// Floating point whole values
		{1.0L, std::abs(1.0L)},
		{-1.0L, std::abs(-1.0L)},
		{2.0L, std::abs(2.0L)},
		{-2.0L, std::abs(-2.0L)},
		{4.0L, std::abs(4.0L)},
		{-4.0L, std::abs(-4.0L)},
		{8.0L, std::abs(8.0L)},
		{-8.0L, std::abs(-8.0L)},
		{16.0L, std::abs(16.0L)},
		{-16.0L, std::abs(-16.0L)},
		{32.0L, std::abs(32.0L)},
		{-32.0L, std::abs(-32.0L)},

		// Testing zeroes
		{0.0L, std::abs(0.0L)},
		{-0.0L, std::abs(-0.0L)},

		// Floating point non-whole values
		{1.12345L, std::abs(1.12345L)},
		{-1.12345L, std::abs(1.12345L)},
		{74.5928L, std::abs(74.5928L)},
		{-74.5928L, std::abs(-74.5928L)},

		// Edge cases
		{std::numeric_limits<long double>::min(), std::abs(std::numeric_limits<long double>::min())},
		{std::numeric_limits<long double>::max(), std::abs(std::numeric_limits<long double>::max())},
		{std::numeric_limits<long double>::denorm_min(), std::abs(std::numeric_limits<long double>::denorm_min())},
		{-std::numeric_limits<long double>::denorm_min(), std::abs(-std::numeric_limits<long double>::denorm_min())},
		{std::numeric_limits<long double>::infinity(), std::abs(std::numeric_limits<long double>::infinity())},
		{-std::numeric_limits<long double>::infinity(), std::abs(-std::numeric_limits<long double>::infinity())},
		{-std::numeric_limits<long double>::max() * 10, std::abs(-std::numeric_limits<long double>::max() * 10)},
		{std::numeric_limits<long double>::max() * 10, std::abs(std::numeric_limits<long double>::max() * 10)},
		{std::numeric_limits<long double>::quiet_NaN(), std::abs(std::numeric_limits<long double>::quiet_NaN())},
	};

	const std::vector<AbsTestParams<int>> kAbsIntTestParams{

		{1, std::abs(1)},
		{-1, std::abs(-1)},
		{2, std::abs(2)},
		{-2, std::abs(-2)},
		{4, std::abs(4)},
		{-4, std::abs(-4)},
		{8, std::abs(8)},
		{-8, std::abs(-8)},
		{16, std::abs(16)},
		{-16, std::abs(-16)},
		{32, std::abs(32)},
		{-32, std::abs(-32)},

		// Testing zeroes
		{0, std::abs(0)},

		// Edge cases
		{std::numeric_limits<int>::max(), std::abs(std::numeric_limits<int>::max())},
		{std::numeric_limits<int>::denorm_min(), std::abs(std::numeric_limits<int>::denorm_min())},
		{-std::numeric_limits<int>::denorm_min(), std::abs(-std::numeric_limits<int>::denorm_min())},
		{std::numeric_limits<int>::infinity(), std::abs(std::numeric_limits<int>::infinity())},
		{-std::numeric_limits<int>::infinity(), std::abs(-std::numeric_limits<int>::infinity())},
		{-std::numeric_limits<int>::max() * 10, std::abs(-std::numeric_limits<int>::max() * 10)},
		{std::numeric_limits<int>::max() * 10, std::abs(std::numeric_limits<int>::max() * 10)},
	};

} // namespace

class CcmathAbsDoubleTests : public TestWithParam<AbsTestParams<double>>
{
};
class CcmathAbsFloatTests : public TestWithParam<AbsTestParams<float>>
{
};
class CcmathAbsLongDoubleTests : public TestWithParam<AbsTestParams<long double>>
{
};
class CcmathAbsIntTests : public TestWithParam<AbsTestParams<int>>
{
};

INSTANTIATE_TEST_SUITE_P(AbsDoubleTests, CcmathAbsDoubleTests, ValuesIn(kAbsDoubleTestParams));
INSTANTIATE_TEST_SUITE_P(AbsFloatTests, CcmathAbsFloatTests, ValuesIn(kAbsFloatTestParams));
INSTANTIATE_TEST_SUITE_P(AbsLongDoubleTests, CcmathAbsLongDoubleTests, ValuesIn(kAbsLongDoubleTestParams));
INSTANTIATE_TEST_SUITE_P(AbsIntTests, CcmathAbsIntTests, ValuesIn(kAbsIntTestParams));

TEST_P(CcmathAbsDoubleTests, AbsWithDoubleReturnsAppropriateValue)
{
	// Arrange
	const auto & param{GetParam()};

	// Act
	const auto actual{ccm::abs(param.input)};

	// Assert
	if (std::isnan(param.expected)) { EXPECT_TRUE(std::isnan(actual)) << "ccm::abs(" << param.input << ") expected to be NaN. Instead got " << actual << "."; }
	else { EXPECT_EQ(actual, param.expected) << "ccm::abs(" << param.input << ") expected to equal " << param.expected << ". Instead got " << actual << "."; }
}

TEST_P(CcmathAbsFloatTests, AbsWithFloatReturnsAppropriateValue)
{
	// Arrange
	const auto & param{GetParam()};

	// Act
	const auto actual{ccm::abs(param.input)};

	// Assert
	if (std::isnan(param.expected)) { EXPECT_TRUE(std::isnan(actual)) << "ccm::abs(" << param.input << ") expected to be NaN. Instead got " << actual << "."; }
	else { EXPECT_EQ(actual, param.expected) << "ccm::abs(" << param.input << ") expected to equal " << param.expected << ". Instead got " << actual << "."; }
}

TEST_P(CcmathAbsLongDoubleTests, AbsWithLongDoubleReturnsAppropriateValue)
{
	// Arrange
	const auto & param{GetParam()};

	// Act
	const auto actual{ccm::abs(param.input)};

	// Assert
	if (std::isnan(param.expected)) { EXPECT_TRUE(std::isnan(actual)) << "ccm::abs(" << param.input << ") expected to be NaN. Instead got " << actual << "."; }
	else { EXPECT_EQ(actual, param.expected) << "ccm::abs(" << param.input << ") expected to equal " << param.expected << ". Instead got " << actual << "."; }
}

TEST_P(CcmathAbsIntTests, AbsWithIntReturnsAppropriateValue)
{
	// Arrange
	const auto & param{GetParam()};

	// Act
	const auto actual{ccm::abs(param.input)};

	// Assert
	EXPECT_EQ(actual, param.expected) << "ccm::abs(" << param.input << ") expected to equal " << param.expected << ". Instead got " << actual << ".";
}

TEST(CcmathBasicTests, CcmAbsCanBeEvaluatedAtCompileTime)
{
	// Verify that ccm::abs works with static_assert

	// Act
	constexpr auto abs{ccm::abs(1)};

	// Assert
	static_assert(abs == 1, "abs has failed testing that it is static_assert-able!");
}

// NOLINTEND

#if defined(_MSC_VER) && !defined(__clang__)
CCM_RESTORE_MSVC_WARNING()
#endif
