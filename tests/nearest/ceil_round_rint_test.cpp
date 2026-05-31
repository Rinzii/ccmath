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
#include "utils/math_samples.hpp"
#include "utils/std_compare.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <vector>

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
	struct NearestUnaryParams
	{
		T input{};
		T expected{};
	};

	template <typename T, T (*StdFn)(T)>
	const std::vector<NearestUnaryParams<T>> & BasicNearestParams()
	{
		static const std::vector<NearestUnaryParams<T>> params{
			{ static_cast<T>(1.0), StdFn(static_cast<T>(1.0)) },
			{ static_cast<T>(1.5), StdFn(static_cast<T>(1.5)) },
			{ static_cast<T>(1.9), StdFn(static_cast<T>(1.9)) },
			{ static_cast<T>(-1.0), StdFn(static_cast<T>(-1.0)) },
			{ static_cast<T>(-1.5), StdFn(static_cast<T>(-1.5)) },
			{ static_cast<T>(-1.9), StdFn(static_cast<T>(-1.9)) },
			{ static_cast<T>(0.0), StdFn(static_cast<T>(0.0)) },
			{ static_cast<T>(-0.0), StdFn(static_cast<T>(-0.0)) },
			{ static_cast<T>(0.5), StdFn(static_cast<T>(0.5)) },
			{ static_cast<T>(-0.5), StdFn(static_cast<T>(-0.5)) },
			{ static_cast<T>(0.9999999999999999), StdFn(static_cast<T>(0.9999999999999999)) },
			{ static_cast<T>(-0.9999999999999999), StdFn(static_cast<T>(-0.9999999999999999)) },
			{ std::numeric_limits<T>::infinity(), StdFn(std::numeric_limits<T>::infinity()) },
			{ -std::numeric_limits<T>::infinity(), StdFn(-std::numeric_limits<T>::infinity()) },
			{ std::numeric_limits<T>::denorm_min(), StdFn(std::numeric_limits<T>::denorm_min()) },
			{ -std::numeric_limits<T>::denorm_min(), StdFn(-std::numeric_limits<T>::denorm_min()) },
		};
		return params;
	}

	template <typename T, T (*StdFn)(T), T (*CcmFn)(T)>
	class NearestUnaryTest : public TestWithParam<NearestUnaryParams<T>>
	{
	};

#define INSTANTIATE_NEAREST_UNARY_TEST(SuiteName, Type, StdFn, CcmFn)                                                                                          \
	class SuiteName##Type##Tests : public NearestUnaryTest<Type, StdFn, CcmFn>                                                                                 \
	{                                                                                                                                                          \
	};                                                                                                                                                         \
	INSTANTIATE_TEST_SUITE_P(SuiteName##Type, SuiteName##Type##Tests, ValuesIn(BasicNearestParams<Type, StdFn>()));                                            \
	TEST_P(SuiteName##Type##Tests, MatchesStd)                                                                                                                 \
	{                                                                                                                                                          \
		const auto param = GetParam();                                                                                                                         \
		ccm::test::ExpectSameAsStd(CcmFn(param.input), param.expected);                                                                                        \
	}

	INSTANTIATE_NEAREST_UNARY_TEST(CcmCeil, double, std::ceil, ccm::ceil)
	INSTANTIATE_NEAREST_UNARY_TEST(CcmCeil, float, std::ceil, ccm::ceilf)

	INSTANTIATE_NEAREST_UNARY_TEST(CcmRound, double, std::round, ccm::round)
	INSTANTIATE_NEAREST_UNARY_TEST(CcmRound, float, std::round, ccm::roundf)

	INSTANTIATE_NEAREST_UNARY_TEST(CcmRint, double, std::rint, ccm::rint)
	INSTANTIATE_NEAREST_UNARY_TEST(CcmRint, float, std::rint, ccm::rintf)

#undef INSTANTIATE_NEAREST_UNARY_TEST

	template <typename T, T (*CcmFn)(T), T (*StdFn)(T)>
	void ExpectNearestNanPropagation()
	{
		ccm::test::ExpectUnaryMatchesStd(std::numeric_limits<T>::quiet_NaN(), CcmFn, StdFn);
		ccm::test::ExpectUnaryMatchesStd(-std::numeric_limits<T>::quiet_NaN(), CcmFn, StdFn);
	}

} // namespace

TEST(CcmathNearestCeilRoundRintTests, CeilNanPropagation)
{
	ExpectNearestNanPropagation<double, ccm::ceil, std::ceil>();
	ExpectNearestNanPropagation<float, ccm::ceilf, std::ceil>();
}

TEST(CcmathNearestCeilRoundRintTests, RoundHalfwayMatchesStd)
{
	for (double input : ccm::test::samples::kNearestHalfwayDouble)
	{
		SCOPED_TRACE(input);
		ccm::test::ExpectUnaryMatchesStd(input, ccm::round<double>, static_cast<double (*)(double)>(std::round));
	}
	for (float input : ccm::test::samples::kNearestHalfwayFloat)
	{
		SCOPED_TRACE(input);
		ccm::test::ExpectUnaryMatchesStd(input, ccm::round<float>, static_cast<float (*)(float)>(std::round));
	}
}

TEST(CcmathNearestCeilRoundRintTests, RintHalfwayMatchesStdDefaultMode)
{
	for (double input : ccm::test::samples::kNearestHalfwayDouble)
	{
		SCOPED_TRACE(input);
		ccm::test::ExpectUnaryMatchesStd(input, ccm::rint<double>, static_cast<double (*)(double)>(std::rint));
	}
}

TEST(CcmathNearestCeilRoundRintTests, CeilLargeNegativeFiniteMatchesStd)
{
	constexpr double input = -std::numeric_limits<double>::max();
	ccm::test::ExpectSameAsStd(ccm::ceil(input), std::ceil(input));
}

TEST(CcmathNearestCeilRoundRintTests, RoundNanPropagation)
{
	ExpectNearestNanPropagation<double, ccm::round, std::round>();
	ExpectNearestNanPropagation<float, ccm::roundf, std::round>();
}

TEST(CcmathNearestCeilRoundRintTests, RintNanPropagation)
{
	ExpectNearestNanPropagation<double, ccm::rint, std::rint>();
	ExpectNearestNanPropagation<float, ccm::rintf, std::rint>();
}

TEST(CcmathNearestCeilRoundRintTests, CeilCompileTime)
{
	static_assert(ccm::ceil(1.5) == 2.0);
	static_assert(ccm::ceil(-1.5) == -1.0);
}

TEST(CcmathNearestCeilRoundRintTests, RoundCompileTime)
{
	static_assert(ccm::round(1.5) == 2.0);
	static_assert(ccm::round(-1.5) == -2.0);
}

TEST(CcmathNearestCeilRoundRintTests, RintCompileTime)
{
	static_assert(ccm::rint(1.5) == 2.0);
	static_assert(ccm::rint(2.0) == 2.0);
}

TEST(CcmathNearestCeilRoundRintTests, CeilIntegralPromotion)
{
	EXPECT_DOUBLE_EQ(ccm::ceil(2), std::ceil(2.0));
	EXPECT_DOUBLE_EQ(ccm::ceil(-2), std::ceil(-2.0));
}

#ifdef CLANG_LINUX
	#undef CLANG_LINUX
#endif
