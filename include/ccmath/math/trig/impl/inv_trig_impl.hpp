/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// Float/double inverse trig adapted from LLVM-libc (acosf.h, inv_trigf_utils.h).

#pragma once

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/support/multiply_add.hpp"
#include "ccmath/internal/support/poly_eval.hpp"
#include "ccmath/math/basic/fabs.hpp"
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/numbers.hpp"
#include "ccmath/math/power/sqrt.hpp"
#include "ccmath/math/trig/impl/inv_trig_data.hpp"

#include <cerrno>
#include <cfenv>
#include <type_traits>

namespace ccm::internal::impl
{
	namespace inv_trig_detail
	{
		template <typename T>
		constexpr T asin_eval(T xsq) noexcept
		{
			using namespace inv_trig_data;

			const T x4 = xsq * xsq;
			const T c0 = ccm::support::multiply_add(xsq, static_cast<T>(k_asin_coeffs[1]), static_cast<T>(k_asin_coeffs[0]));
			const T c1 = ccm::support::multiply_add(xsq, static_cast<T>(k_asin_coeffs[3]), static_cast<T>(k_asin_coeffs[2]));
			const T c2 = ccm::support::multiply_add(xsq, static_cast<T>(k_asin_coeffs[5]), static_cast<T>(k_asin_coeffs[4]));
			const T c3 = ccm::support::multiply_add(xsq, static_cast<T>(k_asin_coeffs[7]), static_cast<T>(k_asin_coeffs[6]));
			const T c4 = ccm::support::multiply_add(xsq, static_cast<T>(k_asin_coeffs[9]), static_cast<T>(k_asin_coeffs[8]));
			const T c5 = ccm::support::multiply_add(xsq, static_cast<T>(k_asin_coeffs[11]), static_cast<T>(k_asin_coeffs[10]));
			const T x8 = x4 * x4;
			const T d0 = ccm::support::multiply_add(x4, c1, c0);
			const T d1 = ccm::support::multiply_add(x4, c3, c2);
			const T d2 = ccm::support::multiply_add(x4, c5, c4);
			return ccm::support::polyeval(x8, d0, d1, d2);
		}

		template <typename T>
		constexpr T acos_kernel(T x) noexcept
		{
			using namespace inv_trig_data;
			using fp_bits_t = ccm::support::fp::FPBits<T>;

			const T ax = ccm::fabs(x);

			if (ax <= static_cast<T>(0.5))
			{
				if (ax < static_cast<T>(0x1.0p-10))
				{
					return static_cast<T>(k_pi_over_2) + ccm::support::multiply_add(static_cast<T>(-0x1.5555555555555p-3) * x, x * x, -x);
				}

				const T xsq = x * x;
				const T x3	= x * xsq;
				const T r	= asin_eval<T>(xsq);
				return ccm::support::multiply_add(-x3, r, static_cast<T>(k_pi_over_2) - x);
			}

			fp_bits_t pos_bits(ax);
			const T xd = pos_bits.get_val();
			const T u  = ccm::support::multiply_add(static_cast<T>(-0.5), xd, static_cast<T>(0.5));
			const T cv = static_cast<T>(2) * ccm::sqrt(u);
			const T r3 = asin_eval<T>(u);
			const T r  = ccm::support::multiply_add(cv * u, r3, cv);
			return x < static_cast<T>(0) ? static_cast<T>(k_pi) - r : r;
		}
		template <typename T>
		constexpr T asin_small_impl(T x) noexcept
		{
			const T ax = ccm::fabs(x);

			// asin(x) = x + x^3/6 + O(x^5). The cubic coefficient is +1/6 (this fast path previously
			// used -1/6, which underran by ~x^3/3 over the tiny-|x| band).
			if (ax < static_cast<T>(0x1.0p-14)) { return x + x * x * x * static_cast<T>(0x1.5555555555555p-3); }

			// Float evaluates in double then rounds once. The same-type path is in the else so it is
			// discarded rather than left unreachable for the float instantiation (MSVC C4702).
			if constexpr (sizeof(T) == sizeof(float))
			{
				const double dx	 = static_cast<double>(x);
				const double xsq = dx * dx;
				const double r	 = asin_eval<double>(xsq);
				return static_cast<T>(dx + dx * xsq * r);
			}
			else
			{
				const T xsq = x * x;
				const T r	= asin_eval<T>(xsq);
				return x + x * xsq * r;
			}
		}
	} // namespace inv_trig_detail

	template <typename T>
	constexpr T acos_impl(T x) noexcept
	{
		using fp_bits_t = ccm::support::fp::FPBits<T>;

		fp_bits_t bits(x);

		if (CCM_UNLIKELY(bits.is_nan())) { return x; }

		if (CCM_UNLIKELY(bits.is_inf()))
		{
			ccm::support::fenv::set_errno_if_required(EDOM);
			ccm::support::fenv::raise_except_if_required(FE_INVALID);
			return x + fp_bits_t::quiet_nan().get_val();
		}

		const T ax = ccm::fabs(x);
		if (ax > static_cast<T>(1))
		{
			ccm::support::fenv::set_errno_if_required(EDOM);
			ccm::support::fenv::raise_except_if_required(FE_INVALID);
			return x + fp_bits_t::quiet_nan().get_val();
		}

		if (ax == static_cast<T>(1)) { return x < static_cast<T>(0) ? static_cast<T>(ccm::numbers::pi_v<T>) : static_cast<T>(0); }

		return inv_trig_detail::acos_kernel(x);
	}

	template <typename T>
	constexpr T asin_impl(T x) noexcept
	{
		if (x == static_cast<T>(0)) { return x; }

		if (ccm::fabs(x) <= static_cast<T>(0.5)) { return inv_trig_detail::asin_small_impl(x); }

		return static_cast<T>(ccm::numbers::pi_v<T>) / static_cast<T>(2) - acos_impl(x);
	}

	template <typename T>
	constexpr T atan_impl(T x) noexcept
	{
		// The |x|<=1 path composes sqrt, a division, and asin. In float those roundings compound to
		// ~5 ULP. Evaluate float atan through the double kernel and round once (double atan is within
		// a couple of double ULP, so the float result is effectively correctly rounded). The double
		// path lives in the else so it is discarded for float rather than left as unreachable code,
		// which MSVC rejects under /W4 (C4702).
		if constexpr (std::is_same_v<T, float>) { return static_cast<float>(atan_impl<double>(static_cast<double>(x))); }
		else
		{
			if (CCM_UNLIKELY(ccm::isnan(x))) { return x; }

			if (x == static_cast<T>(0)) { return x; }

			const bool neg = x < static_cast<T>(0);
			const T ax	   = ccm::fabs(x);

			if (CCM_UNLIKELY(ccm::isinf(ax)))
			{
				return neg ? -static_cast<T>(ccm::numbers::pi_v<T>) / static_cast<T>(2) : static_cast<T>(ccm::numbers::pi_v<T>) / static_cast<T>(2);
			}

			if (ax > static_cast<T>(1))
			{
				const T pi_over_2 = static_cast<T>(ccm::numbers::pi_v<T>) / static_cast<T>(2);
				const T recip	  = static_cast<T>(1) / ax;

				T small{};
				if (recip < static_cast<T>(0x1.0p-14))
				{
					const T recip_sq = recip * recip;
					small			 = recip + recip_sq * recip * static_cast<T>(-0x1.5555555555555p-2);
				}
				else
				{
					small = atan_impl(recip);
				}

				return neg ? -pi_over_2 + small : pi_over_2 - small;
			}

			if (ax < static_cast<T>(0x1.0p-14))
			{
				const T ax_sq = ax * ax;
				const T val	  = ax + ax_sq * ax * static_cast<T>(-0x1.5555555555555p-2);
				return neg ? -val : val;
			}

			const T root = ccm::sqrt(static_cast<T>(1) + ax * ax);
			return asin_impl(neg ? -ax / root : ax / root);
		}
	}

	template <typename T>
	constexpr T atan2_impl(T y, T x) noexcept
	{
		using fp_bits_t = ccm::support::fp::FPBits<T>;

		if (CCM_UNLIKELY(ccm::isnan(y) || ccm::isnan(x))) { return y + x; }

		if (CCM_UNLIKELY(ccm::isinf(x)))
		{
			const T pi = static_cast<T>(ccm::numbers::pi_v<T>);

			if (x > static_cast<T>(0))
			{
				if (CCM_UNLIKELY(ccm::isinf(y)))
				{
					if (y > static_cast<T>(0)) { return pi / static_cast<T>(4); }
					if (y < static_cast<T>(0)) { return -pi / static_cast<T>(4); }
					return y;
				}

				if (y > static_cast<T>(0)) { return static_cast<T>(0); }
				if (y < static_cast<T>(0)) { return static_cast<T>(-0.0); }
				return y;
			}

			if (CCM_UNLIKELY(ccm::isinf(y)))
			{
				if (y > static_cast<T>(0)) { return static_cast<T>(3) * pi / static_cast<T>(4); }
				if (y < static_cast<T>(0)) { return -static_cast<T>(3) * pi / static_cast<T>(4); }
				return fp_bits_t(y).is_neg() ? -pi : pi;
			}

			if (y > static_cast<T>(0)) { return pi; }
			if (y < static_cast<T>(0)) { return -pi; }
			return fp_bits_t(y).is_neg() ? -pi : pi;
		}

		if (CCM_UNLIKELY(ccm::isinf(y)))
		{
			const T pi_over_2 = static_cast<T>(ccm::numbers::pi_v<T>) / static_cast<T>(2);
			return fp_bits_t(y).is_neg() ? -pi_over_2 : pi_over_2;
		}

		if (CCM_UNLIKELY(x == static_cast<T>(0)))
		{
			const T pi_over_2 = static_cast<T>(ccm::numbers::pi_v<T>) / static_cast<T>(2);

			if (y > static_cast<T>(0)) { return pi_over_2; }
			if (y < static_cast<T>(0)) { return -pi_over_2; }
			if (fp_bits_t(x).is_neg())
			{
				const T pi = static_cast<T>(ccm::numbers::pi_v<T>);
				return fp_bits_t(y).is_neg() ? -pi : pi;
			}

			return y;
		}

		const T ratio = y / x;
		T angle		  = atan_impl(ratio);

		if (x < static_cast<T>(0))
		{
			if (y >= static_cast<T>(0)) { return angle + static_cast<T>(ccm::numbers::pi_v<T>); }
			return angle - static_cast<T>(ccm::numbers::pi_v<T>);
		}
		return angle;
	}

	inline constexpr float acos_float(float x) noexcept
	{ return acos_impl(x); }

	inline constexpr double acos_double(double x) noexcept
	{ return acos_impl(x); }

	inline constexpr float asin_float(float x) noexcept
	{ return asin_impl(x); }

	inline constexpr double asin_double(double x) noexcept
	{ return asin_impl(x); }

	inline constexpr float atan_float(float x) noexcept
	{ return atan_impl(x); }

	inline constexpr double atan_double(double x) noexcept
	{ return atan_impl(x); }

	inline constexpr float atan2_float(float y, float x) noexcept
	{ return atan2_impl(y, x); }

	inline constexpr double atan2_double(double y, double x) noexcept
	{ return atan2_impl(y, x); }
} // namespace ccm::internal::impl
