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
		{ 1.0, std::floor(1.0) },
		{ 1.5, std::floor(1.5) },
		{ 1.9, std::floor(1.9) },
		{ -1.0, std::floor(-1.0) },
		{ -1.5, std::floor(-1.5) },
		{ -1.9, std::floor(-1.9) },

		// Zero values
		{ 0.0, std::floor(0.0) },
		{ -0.0, std::floor(-0.0) },

		// Fractional values
		{ 0.5, std::floor(0.5) },
		{ -0.5, std::floor(-0.5) },
		{ 0.9, std::floor(0.9) },
		{ -0.9, std::floor(-0.9) },

		// Very close to whole numbers
		{ 0.9999999999999999, std::floor(0.9999999999999999) },
		{ -0.9999999999999999, std::floor(-0.9999999999999999) },
		{ 1.0000000000000001, std::floor(1.0000000000000001) },
		{ -1.0000000000000001, std::floor(-1.0000000000000001) },
		{ 1.0000000000000000000000000000000000000000000001, std::floor(1.0000000000000000000000000000000000000000000001) },
	};

} // namespace

class CcmathFloorTests : public TestWithParam<FloorTestParams>
{
};

INSTANTIATE_TEST_SUITE_P(FloorTests, CcmathFloorTests, ValuesIn(kFloorTestsParams));

TEST_P(CcmathFloorTests, Floor)
{
	const auto param{ GetParam() };
	const auto actual{ ccm::floor(param.input) };
	ccm::test::ExpectSameAsStd(actual, param.expected);
}

TEST(CcmathNearestTests, CcmFloorTestNanValues)
{
	ccm::test::ExpectUnaryMatchesStd(std::numeric_limits<double>::quiet_NaN(), ccm::floor<double>, static_cast<double (*)(double)>(std::floor));
	ccm::test::ExpectUnaryMatchesStd(-std::numeric_limits<double>::quiet_NaN(), ccm::floor<double>, static_cast<double (*)(double)>(std::floor));
}

TEST(CcmathNearestTests, CcmFloorCanBeEvaluatedAtCompileTime)
{
	constexpr auto floor{ ccm::floor(1.0) };
	static_assert(floor == 1.0);
}

TEST(CcmathNearestTests, CcmFloorHandlesLargeNegativeFiniteValues)
{
	constexpr double input	= -std::numeric_limits<double>::max();
	constexpr double actual = ccm::floor(input);
	static_assert(actual == input);
	ccm::test::ExpectSameAsStd(actual, std::floor(input));
}
