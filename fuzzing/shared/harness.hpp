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

#include "ccmath/ccmath.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "input.hpp"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <type_traits>
#include <utility>

namespace ccm::fuzz
{
	inline constexpr int64_t kMaxAllowedUlp = 4;

	// Differential fuzzing runs under the default round-to-nearest mode. Cycling the FPU
	// rounding mode around the kernels is deliberately not done: ccmath is header-only and
	// compiled without FENV_ACCESS (forbidden as unportable), so already-folded constants in
	// the inlined kernels do not honor a dynamically set mode, which makes such a comparison
	// nondeterministic and build-context dependent. Directed-mode correctness is covered by
	// the dedicated all-modes unit tests instead.

#define FUZZ_CHECK(cond)                                                                                                                                       \
	do                                                                                                                                                         \
	{                                                                                                                                                          \
		if (!(cond))                                                                                                                                           \
		{                                                                                                                                                      \
			std::fprintf(stderr, "FUZZ_CHECK failed: %s\n", #cond);                                                                                            \
			std::abort();                                                                                                                                      \
		}                                                                                                                                                      \
	} while (0)

	template <typename T> int64_t ulp_difference(T a, T b)
	{
		static_assert(std::is_floating_point_v<T>, "T must be floating-point");

		using namespace ccm::support::fp;
		using fp_bits_t = FPBits<T>;

		fp_bits_t fp_a(a);
		fp_bits_t fp_b(b);

		if (fp_a.is_nan() && fp_b.is_nan())
		{
			return 0;
		}
		if (fp_a.is_nan() || fp_b.is_nan())
		{
			return std::numeric_limits<int64_t>::max();
		}
		if (fp_a.is_inf() && fp_b.is_inf())
		{
			if (fp_a.sign() == fp_b.sign())
			{
				return 0;
			}
			return std::numeric_limits<int64_t>::max();
		}
		if (fp_a.is_inf() || fp_b.is_inf())
		{
			return std::numeric_limits<int64_t>::max();
		}

		using uint_type				  = typename fp_bits_t::storage_type;
		constexpr uint_type sign_mask = uint_type{ 1 } << (sizeof(uint_type) * 8 - 1);

		auto ordered_bits = [=](fp_bits_t bits) {
			uint_type const raw = bits.uintval();
			if ((raw & sign_mask) != 0)
			{
				return sign_mask - (raw & ~sign_mask);
			}
			return sign_mask + raw;
		};

		uint_type const a_bits = ordered_bits(fp_a);
		uint_type const b_bits = ordered_bits(fp_b);
		uint_type const diff   = (a_bits > b_bits) ? a_bits - b_bits : b_bits - a_bits;
		if (diff > static_cast<uint_type>(std::numeric_limits<int64_t>::max()))
		{
			return std::numeric_limits<int64_t>::max();
		}
		return static_cast<int64_t>(diff);
	}

	template <typename T> bool values_equal(T a, T b)
	{
		using fp_bits_t = ccm::support::fp::FPBits<T>;

		fp_bits_t fp_a(a);
		fp_bits_t fp_b(b);

		if (fp_a.is_nan() || fp_b.is_nan())
		{
			return fp_a.is_nan() && fp_b.is_nan();
		}

		return fp_a.uintval() == fp_b.uintval();
	}

	template <typename T> void check_same_floating(T actual, T expected, int64_t max_ulp = kMaxAllowedUlp)
	{
		using fp_bits_t = ccm::support::fp::FPBits<T>;

		fp_bits_t fp_expected(expected);

		if (fp_expected.is_nan())
		{
			FUZZ_CHECK(fp_bits_t(actual).is_nan());
			return;
		}

		if (fp_expected.is_inf())
		{
			FUZZ_CHECK(values_equal(actual, expected));
			return;
		}

		if (actual == T{} && expected == T{})
		{
			FUZZ_CHECK(std::signbit(actual) == std::signbit(expected));
			return;
		}

		FUZZ_CHECK(ulp_difference(actual, expected) <= max_ulp);
	}

	template <typename T, typename CcmFn> void check_ccm_negative_domain_nan(T input, CcmFn ccm_fn)
	{
		T const result = ccm_fn(input);
		FUZZ_CHECK(std::isnan(result));
		FUZZ_CHECK(std::signbit(result));
	}

	template <typename T, typename CcmFn, typename StdFn> void fuzz_unary_vs_std(T input, CcmFn && ccm_fn, StdFn && std_fn, int64_t max_ulp = kMaxAllowedUlp)
	{
		check_same_floating(ccm_fn(input), std_fn(input), max_ulp);
	}

	template <typename T, typename CcmFn, typename StdFn> void fuzz_unary_log_family(T input, CcmFn && ccm_fn, StdFn && std_fn)
	{
		if (input < T{} && !std::isnan(input))
		{
			check_ccm_negative_domain_nan(input, ccm_fn);
			return;
		}

		fuzz_unary_vs_std(input, std::forward<CcmFn>(ccm_fn), std::forward<StdFn>(std_fn));
	}

	template <typename T, typename CcmFn, typename StdFn> void fuzz_unary_log1p_family(T input, CcmFn && ccm_fn, StdFn && std_fn)
	{
		if (input < static_cast<T>(-1) && !std::isnan(input))
		{
			check_ccm_negative_domain_nan(input, ccm_fn);
			return;
		}

		fuzz_unary_vs_std(input, std::forward<CcmFn>(ccm_fn), std::forward<StdFn>(std_fn));
	}

	template <typename T, typename CcmFn, typename StdFn> void fuzz_binary_vs_std(T x, T y, CcmFn && ccm_fn, StdFn && std_fn, int64_t max_ulp = kMaxAllowedUlp)
	{
		check_same_floating(ccm_fn(x, y), std_fn(x, y), max_ulp);
	}

	template <typename T, typename CcmFn, typename StdFn> void fuzz_binary_exact_int(T x, T y, CcmFn && ccm_fn, StdFn && std_fn)
	{
		FUZZ_CHECK(ccm_fn(x, y) == std_fn(x, y));
	}

	template <typename T> void check_remquo_vs_std(T x, T y)
	{
		using fp_bits_t = ccm::support::fp::FPBits<T>;

		int ccm_quo{};
		int std_quo{};
		T const ccm_rem = ccm::remquo(x, y, &ccm_quo);
		T const std_rem = std::remquo(x, y, &std_quo);

		fp_bits_t fp_ccm(ccm_rem);
		fp_bits_t fp_std(std_rem);

		if (fp_std.is_nan())
		{
			FUZZ_CHECK(fp_ccm.is_nan());
			return;
		}

		if (fp_std.is_inf())
		{
			FUZZ_CHECK(fp_ccm.is_inf());
			FUZZ_CHECK(fp_ccm.sign() == fp_std.sign());
			return;
		}

		FUZZ_CHECK((ccm_quo & 7) == (std_quo & 7));
		FUZZ_CHECK(values_equal(ccm_rem, std_rem));
	}

	template <typename T> void check_frexp_vs_std(T x)
	{
		int ccm_exp{};
		int std_exp{};
		check_same_floating(ccm::frexp(x, ccm_exp), std::frexp(x, &std_exp));
		FUZZ_CHECK(ccm_exp == std_exp);
	}

	template <typename T> void check_modf_vs_std(T x)
	{
		T ccm_intpart{};
		T std_intpart{};
		check_same_floating(ccm::modf(x, &ccm_intpart), std::modf(x, &std_intpart));
		FUZZ_CHECK(values_equal(ccm_intpart, std_intpart));
	}

} // namespace ccm::fuzz
