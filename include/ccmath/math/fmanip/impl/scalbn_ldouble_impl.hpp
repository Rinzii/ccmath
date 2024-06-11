/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
	#include "ccmath/math/fmanip/impl/scalbn_double_impl.hpp"
#elif (LDBL_MANT_DIG == 64 || LDBL_MANT_DIG == 113) && LDBL_MAX_EXP == 16384
	#include "ccmath/math/compare/isinf.hpp"
	#include "ccmath/math/compare/isnan.hpp"
	#include <limits>
#endif

namespace ccm::internal
{
	namespace impl
	{
		constexpr long double scalbn_ldouble_impl(long double arg, int exp) noexcept
		{
			#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024 // If long double is the same as double
			return static_cast<long double>(ccm::internal::impl::scalbn_double_impl(arg, exp));
			#elif (LDBL_MANT_DIG == 64 || LDBL_MANT_DIG == 113) && LDBL_MAX_EXP == 16384 // If long double is 80-bit or 128-bit large
			// This is a generic implementation for long double scalbn that does not use bit manipulation.
			// May be much slower than the double and float version.
			// Need to benchmark it.
			// TODO: Possibly implement a bit manipulation version of this function in the future if possible.
			if (arg == static_cast<long double>(0)) { return arg; }

			if (ccm::isinf(arg)) { return arg; }

			if (exp == static_cast<long double>(0)) { return arg; }

			if (ccm::isnan(arg)) { return std::numeric_limits<long double>::quiet_NaN(); }

			long double mult(1);
			if (exp > 0)
			{
				mult = std::numeric_limits<long double>::radix;
				--exp;
			}
			else
			{
				++exp;
				exp = -exp;
				mult /= std::numeric_limits<long double>::radix;
			}

			while (exp > 0)
			{
				if ((exp & 1) == 0)
				{
					mult *= mult;
					exp >>= 1;
				}
				else
				{
					arg *= mult;
					--exp;
				}
			}
			return arg;
			#else
			// This should never be called, but since we are not using templates here can can not static_assert it.
			return 0;
			#endif
		}
	} // namespace impl

	constexpr long double scalbn_ldouble(long double num, int exp) noexcept
	{
		return impl::scalbn_ldouble_impl(num, exp);
	}

	constexpr long double scalbn_ldouble(long double num, long exp) noexcept
	{
		return impl::scalbn_ldouble_impl(num, static_cast<int>(exp));
	}
} // namespace ccm::internal
