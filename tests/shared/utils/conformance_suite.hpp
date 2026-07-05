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

#include "utils/fenv_fixture.hpp"
#include "utils/fp_matcher.hpp"
#include "utils/ulp_suite.hpp"

#include <cmath>
#include <type_traits>

namespace ccm::test
{
	/// Evaluate ccm and std under the same rounding mode (LLVM ForceRoundingMode pattern).
	template <typename T, typename CcmFn, typename StdFn>
	void ExpectUlpUnaryVsStdInMode(int rounding_mode, T input, CcmFn ccm_fn, StdFn std_fn, int64_t max_ulp = kMaxAllowedUlp)
	{
		ForceRoundingMode force(rounding_mode);
		if (!force)
		{
			ADD_FAILURE() << "could not set " << RoundingModeName(rounding_mode);
		}
		ExpectSameFloatingAsStd(ccm_fn(input), std_fn(input), max_ulp);
	}

	template <typename CcmFn, typename StdFn> void ExpectFenvFlagsMatchStd(CcmFn ccm_fn, StdFn std_fn, int expected_flags_mask = FE_ALL_EXCEPT)
	{
		ScopedFenvEnvironment env;
		ASSERT_TRUE(env.active());

		ASSERT_TRUE(ClearFenvExceptions());
		static_cast<void>(ccm_fn());
		const int ccm_flags = CurrentFenvExceptions();

		ASSERT_TRUE(ClearFenvExceptions());
		static_cast<void>(std_fn());
		const int std_flags = CurrentFenvExceptions();

		EXPECT_EQ(ccm_flags & expected_flags_mask, std_flags & expected_flags_mask);
	}

	template <typename Fn> void ExpectFenvFlags(Fn fn, int expected_flags, int flags_mask = FE_ALL_EXCEPT)
	{
		ScopedFenvEnvironment env;
		ASSERT_TRUE(env.active());

		ASSERT_TRUE(ClearFenvExceptions());
		static_cast<void>(fn());
		EXPECT_EQ(CurrentFenvExceptions() & flags_mask, expected_flags & flags_mask);
	}

	template <typename T, typename CcmFn, typename StdFn>
	void ExpectUlpBinaryVsStdInMode(int rounding_mode, T x, T y, CcmFn ccm_fn, StdFn std_fn, int64_t max_ulp = kMaxAllowedUlp)
	{
		ForceRoundingMode force(rounding_mode);
		if (!force)
		{
			ADD_FAILURE() << "could not set " << RoundingModeName(rounding_mode);
		}
		ExpectSameFloatingAsStd(ccm_fn(x, y), std_fn(x, y), max_ulp);
	}

	template <typename T, typename CcmFn, typename StdFn, std::size_t N>
	void ExpectUlpUnaryOverInMode(int rounding_mode, const std::array<T, N> & inputs, CcmFn ccm_fn, StdFn std_fn, int64_t max_ulp = kMaxAllowedUlp)
	{
		for (T input : inputs)
		{
			SCOPED_TRACE(input);
			ExpectUlpUnaryVsStdInMode(rounding_mode, input, ccm_fn, std_fn, max_ulp);
		}
	}

	template <typename T, typename CcmFn, typename StdFn, std::size_t N>
	void ExpectUlpUnaryOverAllRoundingModes(const std::array<T, N> & inputs, CcmFn ccm_fn, StdFn std_fn, int64_t max_ulp = kMaxAllowedUlp)
	{
		ForEachRoundingModeOrSkip([&](int mode) { ExpectUlpUnaryOverInMode(mode, inputs, ccm_fn, std_fn, max_ulp); });
	}

	template <typename T, typename CcmFn, typename StdFn, std::size_t N>
	void ExpectUlpBinaryOverAllRoundingModes(
		const std::array<T, N> & bases, const std::array<T, N> & exponents, CcmFn ccm_fn, StdFn std_fn, int64_t max_ulp = kMaxAllowedUlp)
	{
		static_assert(N > 0, "need at least one (base, exponent) pair");
		ForEachRoundingModeOrSkip([&](int mode) {
			for (std::size_t i = 0; i < N; ++i)
			{
				SCOPED_TRACE(bases[i]);
				SCOPED_TRACE(exponents[i]);
				if (std::isnan(std_fn(bases[i], exponents[i])))
				{
					continue;
				}
				ExpectUlpBinaryVsStdInMode(mode, bases[i], exponents[i], ccm_fn, std_fn, max_ulp);
			}
		});
	}

	template <typename T, typename CcmFn, typename StdFn> void ExpectFpTernaryMatchesStdAllModes(T x, T y, T z, CcmFn ccm_fn, StdFn std_fn)
	{
		ForEachRoundingModeOrSkip([&](int mode) {
			ForceRoundingMode force(mode);
			if (!force)
			{
				ADD_FAILURE() << "could not set " << RoundingModeName(mode);
			}
			ExpectFpEq(ccm_fn(x, y, z), std_fn(x, y, z));
		});
	}

	template <typename TernaryCase, typename CcmFn, typename StdFn, std::size_t N>
	void ExpectFpTernaryOverMatchesStdAllModes(const std::array<TernaryCase, N> & inputs, CcmFn ccm_fn, StdFn std_fn)
	{
		for (const auto & input : inputs)
		{
			SCOPED_TRACE(input.x);
			SCOPED_TRACE(input.y);
			SCOPED_TRACE(input.z);
			ExpectFpTernaryMatchesStdAllModes(input.x, input.y, input.z, ccm_fn, std_fn);
		}
	}

	/// Bit-exact check under each rounding mode. Both sides evaluated inside the forced mode.
	template <typename T, typename CcmFn, typename StdFn> void ExpectFpUnaryMatchesStdAllModes(T input, CcmFn ccm_fn, StdFn std_fn)
	{
		ForEachRoundingModeOrSkip([&](int mode) {
			ForceRoundingMode force(mode);
			if (!force)
			{
				ADD_FAILURE() << "could not set " << RoundingModeName(mode);
			}
			ExpectFpEq(ccm_fn(input), std_fn(input));
		});
	}

	template <typename T, typename CcmFn, typename StdFn, std::size_t N>
	void ExpectFpUnaryOverMatchesStdAllModes(const std::array<T, N> & inputs, CcmFn ccm_fn, StdFn std_fn)
	{
		for (T input : inputs)
		{
			SCOPED_TRACE(input);
			ExpectFpUnaryMatchesStdAllModes(input, ccm_fn, std_fn);
		}
	}

	template <typename T, typename CcmFn, typename StdFn> void ExpectDomainEdgeMatchesStdAllModes(T input, CcmFn ccm_fn, StdFn std_fn)
	{
		ForEachRoundingModeOrSkip([&](int mode) {
			SCOPED_TRACE(input);
			ForceRoundingMode force(mode);
			if (!force)
			{
				ADD_FAILURE() << "could not set " << RoundingModeName(mode);
			}
			ExpectSameFloatingAsStd(ccm_fn(input), std_fn(input), kMaxAllowedUlp);
		});
	}

} // namespace ccm::test

#define CCM_EXPECT_FP_EQ_ROUNDING_MODE(expected, actual, rounding_mode)                                                                                        \
	do                                                                                                                                                         \
	{                                                                                                                                                          \
		::ccm::test::ForceRoundingMode __ccm_force_round((rounding_mode));                                                                                     \
		if (__ccm_force_round)                                                                                                                                 \
		{                                                                                                                                                      \
			CCM_EXPECT_FP_EQ((actual), (expected));                                                                                                            \
		}                                                                                                                                                      \
	} while (0)

#define CCM_EXPECT_FP_EQ_ALL_ROUNDING(expected, actual)                                                                                                        \
	do                                                                                                                                                         \
	{                                                                                                                                                          \
		for (int __ccm_mode : ::ccm::test::kStdRoundingModes)                                                                                                  \
		{                                                                                                                                                      \
			SCOPED_TRACE(::ccm::test::RoundingModeName(__ccm_mode));                                                                                           \
			CCM_EXPECT_FP_EQ_ROUNDING_MODE((expected), (actual), __ccm_mode);                                                                                  \
		}                                                                                                                                                      \
	} while (0)
