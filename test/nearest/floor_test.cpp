/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include "ccmath/ccmath.hpp"

namespace
{

	using testing::TestWithParam;
	using testing::ValuesIn;

	struct FloorTestParams
	{
		double input{};
		double expected{};
	};

	const std::vector<FloorTestParams> kFloorTestsParams{
		// Basic double values
		{1.0, std::floor(1.0)},
		{1.5, std::floor(1.5)},
		{1.9, std::floor(1.9)},
		{-1.0, std::floor(-1.0)},
		{-1.5, std::floor(-1.5)},
		{-1.9, std::floor(-1.9)},

		// Zero values
		{0.0, std::floor(0.0)},
		{-0.0, std::floor(-0.0)},

		// Fractional values
		{0.5, std::floor(0.5)},
		{-0.5, std::floor(-0.5)},
		{0.9, std::floor(0.9)},
		{-0.9, std::floor(-0.9)},

		// Very close to whole numbers
		{0.9999999999999999, std::floor(0.9999999999999999)},
		{-0.9999999999999999, std::floor(-0.9999999999999999)},
		{1.0000000000000001, std::floor(1.0000000000000001)},
		{-1.0000000000000001, std::floor(-1.0000000000000001)},
		{1.0000000000000000000000000000000000000000000001, std::floor(1.0000000000000000000000000000000000000000000001)},
	};

} // namespace

class CcmathFloorTests : public TestWithParam<FloorTestParams>
{
};

INSTANTIATE_TEST_SUITE_P(FloorTests, CcmathFloorTests, ValuesIn(kFloorTestsParams));

TEST_P(CcmathFloorTests, Floor)
{
	const auto param{GetParam()};
	const auto actual{ccm::floor(param.input)};
	EXPECT_EQ(actual, param.expected) << "ccm::floor(" << param.input << ") expected to equal " << param.expected << ". Instead got " << actual << ".";
}

TEST(CcmathNearestTests, CcmFloorTestNanValues)
{
	// Check if ccm::floor and std::floor return NaN for positive NaN
	EXPECT_EQ(std::isnan(ccm::floor(std::numeric_limits<double>::quiet_NaN())), std::isnan(std::floor(std::numeric_limits<double>::quiet_NaN())));

	// Check if ccm::floor and std::floor have the same sign bit for positive NaN
	EXPECT_EQ(std::signbit(ccm::floor(std::numeric_limits<double>::quiet_NaN())), std::signbit(std::floor(std::numeric_limits<double>::quiet_NaN())));

	// Check if ccm::floor and std::floor return NaN for negative NaN
	EXPECT_EQ(std::isnan(ccm::floor(-std::numeric_limits<double>::quiet_NaN())), std::isnan(std::floor(-std::numeric_limits<double>::quiet_NaN())));

	// Check if ccm::floor and std::floor have the same sign bit for negative NaN
	EXPECT_EQ(std::signbit(ccm::floor(-std::numeric_limits<double>::quiet_NaN())), std::signbit(std::floor(-std::numeric_limits<double>::quiet_NaN())));
}

TEST(CcmathNearestTests, CcmFloorCanBeEvaluatedAtCompileTime)
{
	constexpr auto floor{ccm::floor(1.0)};
	static_assert(floor == 1.0);
}
