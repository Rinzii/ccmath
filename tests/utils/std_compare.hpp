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

#include "utils/fp_matcher.hpp"
#include "utils/ulp_suite.hpp"

#include <gtest/gtest.h>

#include <ccmath/ccmath.hpp>

#include <cmath>
#include <type_traits>

namespace ccm::test
{
	template <typename Actual, typename Expected>
	void ExpectSameAsStd(Actual actual, Expected expected, int64_t max_ulp = kMaxAllowedUlp)
	{
		using common_type = std::common_type_t<Actual, Expected>;
		if constexpr (std::is_floating_point_v<common_type>)
		{
			ExpectSameFloatingAsStd(static_cast<common_type>(actual), static_cast<common_type>(expected), max_ulp);
		}
		else { EXPECT_EQ(actual, expected); }
	}

	template <typename T, typename CcmFn, typename StdFn>
	void ExpectUnaryMatchesStd(T input, CcmFn ccm_fn, StdFn std_fn, int64_t max_ulp = kMaxAllowedUlp)
	{
		SCOPED_TRACE(input);
		ExpectSameAsStd(ccm_fn(input), std_fn(input), max_ulp);
	}

	template <typename T, typename CcmFn, typename StdFn>
	void ExpectBinaryMatchesStd(T x, T y, CcmFn ccm_fn, StdFn std_fn, int64_t max_ulp = kMaxAllowedUlp)
	{
		SCOPED_TRACE(x);
		SCOPED_TRACE(y);
		ExpectSameAsStd(ccm_fn(x, y), std_fn(x, y), max_ulp);
	}

	template <typename T, typename CcmFn, typename StdFn>
	void ExpectTernaryMatchesStd(T x, T y, T z, CcmFn ccm_fn, StdFn std_fn, int64_t max_ulp = kMaxAllowedUlp)
	{
		SCOPED_TRACE(x);
		SCOPED_TRACE(y);
		SCOPED_TRACE(z);
		ExpectSameAsStd(ccm_fn(x, y, z), std_fn(x, y, z), max_ulp);
	}

	template <typename T>
	void ExpectFrexpMatchesStd(T input)
	{
		int ccm_exp{};
		int std_exp{};
		T const actual	 = ccm::frexp(input, ccm_exp);
		T const expected = std::frexp(input, &std_exp);
		ExpectSameFloatingAsStd(actual, expected);
		if (std::isfinite(input) || input == T(0)) { EXPECT_EQ(ccm_exp, std_exp); }
	}

	template <typename T>
	void ExpectModfMatchesStd(T input)
	{
		T ccm_int{};
		T std_int{};
		T const actual	 = ccm::modf(input, &ccm_int);
		T const expected = std::modf(input, &std_int);
		ExpectSameFloatingAsStd(actual, expected);
		ExpectFpEq(ccm_int, std_int);
	}

	template <typename T>
	void ExpectRemquoMatchesStd(T x, T y)
	{
		int ccm_quo{};
		int std_quo{};
		T const actual	 = ccm::remquo(x, y, &ccm_quo);
		T const expected = std::remquo(x, y, &std_quo);
		ExpectSameFloatingAsStd(actual, expected);
		if (!std::isnan(expected)) { EXPECT_EQ(ccm_quo & 7, std_quo & 7); }
	}

} // namespace ccm::test
