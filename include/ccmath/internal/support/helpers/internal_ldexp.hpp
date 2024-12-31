/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// Portions of this code was borrowed from the LLVM Project,
// under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <cfenv>

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fenv/rounding_mode.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/types/dyadic_float.hpp"

namespace ccm::support::helpers
{
	template <typename T, typename U>
	constexpr std::enable_if_t<ccm::support::traits::ccm_is_floating_point_v<T> && ccm::support::traits::ccm_is_integral_v<U>, T> internal_ldexp(T x, U exp)
	{
		fp::FPBits<T> bits(x);
		if (CCM_UNLIKELY((exp == 0) || bits.is_zero() || bits.is_inf_or_nan())) { return x; }

		constexpr int exponent_limit = fp::FPBits<T>::max_biased_exponent + support::fp::FPBits<T>::fraction_length + 1;
		static_assert(exponent_limit <= INT_MAX && -exponent_limit >= INT_MIN);

		// Make sure that we can safely cast exp to int when not returning early.
		if (CCM_UNLIKELY(exp > exponent_limit))
		{
			int const rounding_mode = fenv::internal::rt_get_rounding_mode();
			types::Sign sign		= bits.sign();

			if ((sign == types::Sign::POS && rounding_mode == FE_DOWNWARD) || (sign == types::Sign::NEG && rounding_mode == FE_UPWARD) ||
				(rounding_mode == FE_TOWARDZERO))
			{
				return fp::FPBits<T>::max_normal(sign).get_val();
			}

			fenv::set_errno_if_required(ERANGE);
			fenv::raise_except_if_required(FE_OVERFLOW);
			return fp::FPBits<T>::inf(sign).get_val();
		}

		// Similarly, on the negative side, we return zero early if |exp| is too small.
		if (CCM_UNLIKELY(exp < -exponent_limit))
		{
			int const rounding_mode = fenv::internal::rt_get_rounding_mode();
			types::Sign sign		= bits.sign();

			if ((sign == types::Sign::POS && rounding_mode == FE_UPWARD) || (sign == types::Sign::NEG && rounding_mode == FE_DOWNWARD))
			{
				return fp::FPBits<T>::min_subnormal(sign).get_val();
			}

			fenv::set_errno_if_required(ERANGE);
			fenv::raise_except_if_required(FE_UNDERFLOW);
			return fp::FPBits<T>::zero(sign).get_val();
		}

		// For all other values, NormalFloat to T conversion handles it the right way.
		types::DyadicFloat<fp::FPBits<T>::storage_length> normal(bits.get_val());
		normal.exponent += static_cast<int>(exp);
		return normal.template as<T, /*ShouldRaiseExceptions=*/true>();
	}
} // namespace ccm::support::helpers
