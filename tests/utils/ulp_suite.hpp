/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#pragma once

#include "utils/helpers.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <gtest/gtest.h>
#include <limits>
#include <type_traits>

namespace ccm::test
{
	// Informal policy: stay within 4 ULP of the standard-library result. Document known exceptions
	// with a TODO in the implementation plus a regression test.
	inline constexpr int64_t kMaxAllowedUlp = 4;

	template <typename T>
	void ExpectSameFloatingAsStd(T actual, T expected, int64_t max_ulp = kMaxAllowedUlp)
	{
		static_assert(std::is_floating_point_v<T>, "T must be floating-point");

		if (std::isnan(expected))
		{
			EXPECT_TRUE(std::isnan(actual)) << "expected NaN result";
			return;
		}

		if (std::isinf(expected))
		{
			EXPECT_EQ(actual, expected);
			return;
		}

		if (actual == T{} && expected == T{})
		{
			EXPECT_EQ(std::signbit(actual), std::signbit(expected)) << "signed-zero mismatch";
			return;
		}

		EXPECT_LE(ulp_difference(actual, expected), max_ulp) << "actual=" << actual << " expected=" << expected;
	}

	template <typename T>
	void ExpectUlpNearVsStd(T actual, T expected, int64_t max_ulp = kMaxAllowedUlp)
	{
		ExpectSameFloatingAsStd(actual, expected, max_ulp);
	}

	template <typename T, typename CcmFn, typename StdFn>
	void ExpectUlpUnaryVsStd(T input, CcmFn ccm_fn, StdFn std_fn, int64_t max_ulp = kMaxAllowedUlp)
	{
		ExpectSameFloatingAsStd(ccm_fn(input), std_fn(input), max_ulp);
	}

	template <typename T, typename CcmFn, typename StdFn>
	void ExpectUlpBinaryVsStd(T x, T y, CcmFn ccm_fn, StdFn std_fn, int64_t max_ulp = kMaxAllowedUlp)
	{
		ExpectSameFloatingAsStd(ccm_fn(x, y), std_fn(x, y), max_ulp);
	}

	template <typename T, typename CcmFn, typename StdFn, std::size_t N>
	void ExpectUlpUnaryOver(const T (&inputs)[N], CcmFn ccm_fn, StdFn std_fn, int64_t max_ulp = kMaxAllowedUlp)
	{
		for (T input : inputs)
		{
			SCOPED_TRACE(input);
			ExpectUlpUnaryVsStd(input, ccm_fn, std_fn, max_ulp);
		}
	}

	template <typename T, typename CcmFn, typename StdFn, std::size_t N>
	void ExpectUlpUnaryOver(const std::array<T, N> & inputs, CcmFn ccm_fn, StdFn std_fn, int64_t max_ulp = kMaxAllowedUlp)
	{
		for (T input : inputs)
		{
			SCOPED_TRACE(input);
			ExpectUlpUnaryVsStd(input, ccm_fn, std_fn, max_ulp);
		}
	}

	template <typename T, typename CcmFn, typename StdFn>
	void ExpectDomainEdgeMatchesStd(T input, CcmFn ccm_fn, StdFn std_fn)
	{
		SCOPED_TRACE(input);
		ExpectSameFloatingAsStd(ccm_fn(input), std_fn(input), kMaxAllowedUlp);
	}

	template <typename T, typename CcmFn>
	void ExpectCcmNegativeDomainNaN(T input, CcmFn ccm_fn)
	{
		SCOPED_TRACE(input);
		const T result = ccm_fn(input);
		EXPECT_TRUE(std::isnan(result));
		EXPECT_TRUE(std::signbit(result));
	}

	template <typename T, typename CcmFn, typename StdFn>
	bool IsLikelyUnimplementedStub(T probe, CcmFn ccm_fn, StdFn std_fn)
	{
		return ccm_fn(probe) == T{} && std_fn(probe) != T{};
	}

} // namespace ccm::test
