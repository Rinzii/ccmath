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

#include <cfenv>

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fenv/rounding_mode.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/types/dyadic_float.hpp"

namespace ccm::support::helpers
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
	constexpr T internal_ldexp(T x, int exp)
	{
		support::fp::FPBits<T> bits(x);
		if (CCM_UNLIKELY((exp == 0) || bits.is_zero() || bits.is_inf_or_nan())) { return x; }

		constexpr int EXP_LIMIT = support::fp::FPBits<T>::MAX_BIASED_EXPONENT + support::fp::FPBits<T>::fraction_length + 1;
		if (CCM_UNLIKELY(exp > EXP_LIMIT))
		{
			int const rounding_mode = support::fenv::get_rounding_mode();
			types::Sign sign		= bits.sign();

			if ((sign == types::Sign::POS && rounding_mode == FE_DOWNWARD) || (sign == types::Sign::NEG && rounding_mode == FE_UPWARD) ||
				(rounding_mode == FE_TOWARDZERO))
			{
				return fp::FPBits<T>::max_normal(sign).get_val();
			}

			if constexpr (fenv::is_errno_enabled())
			{
				// These func do nothing at compile time, but at runtime will set errno and raise exceptions if required.
				fenv::set_errno_if_required(ERANGE);
				fenv::raise_except_if_required(FE_OVERFLOW);
			}

			return fp::FPBits<T>::inf(sign).get_val();
		}

		if (CCM_UNLIKELY(exp < -EXP_LIMIT))
		{
			int const rounding_mode = support::fenv::get_rounding_mode();
			types::Sign sign		= bits.sign();

			if ((sign == types::Sign::POS && rounding_mode == FE_UPWARD) || (sign == types::Sign::NEG && rounding_mode == FE_DOWNWARD))
			{
				return support::fp::FPBits<T>::min_subnormal(sign).get_val();
			}

			// These func do nothing at compile time, but at runtime will set errno and raise exceptions if required.
			support::fenv::set_errno_if_required(ERANGE);
			support::fenv::raise_except_if_required(FE_UNDERFLOW);

			return support::fp::FPBits<T>::zero(sign).get_val();
		}

		// For all other values, NormalFloat to T conversion handles it the right way.
		types::DyadicFloat<support::fp::FPBits<T>::storage_length> normal(bits.get_val());
		normal.exponent += exp;
		return static_cast<T>(normal);
	}
} // namespace ccm::support::helpers
