/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <cfenv>

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/fp_bits.hpp"
#include "ccmath/internal/support/rounding_mode.hpp"


namespace ccm::helpers
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
	constexpr T ccm_ldexp(T x, int exp) {
		support::FPBits<T> bits(x);
		if (CCM_UNLIKELY((exp == 0) || bits.is_zero() || bits.is_inf_or_nan()))
			return x;

		// NormalFloat uses int32_t to store the true exponent value. We should ensure
		// that adding |exp| to it does not lead to integer rollover. But, if |exp|
		// value is larger the exponent range for type T, then we can return infinity
		// early. Because the result of the ldexp operation can be a subnormal number,
		// we need to accommodate the (mantissaWidth + 1) worth of shift in
		// calculating the limit.
		constexpr int EXP_LIMIT =
			support::FPBits<T>::MAX_BIASED_EXPONENT + support::FPBits<T>::FRACTION_LEN + 1;
		if (CCM_UNLIKELY(exp > EXP_LIMIT)) {
			int rounding_mode = quick_get_round();
			types::Sign sign = bits.sign();

			if ((sign == types::Sign::POS && rounding_mode == FE_DOWNWARD) ||
				(sign == types::Sign::NEG && rounding_mode == FE_UPWARD) ||
				(rounding_mode == FE_TOWARDZERO)) {
				return support::FPBits<T>::max_normal(sign).get_val();
}

			return support::FPBits<T>::inf(sign).get_val();
		}

		// Similarly on the negative side we return zero early if |exp| is too small.
		if (CCM_UNLIKELY(exp < -EXP_LIMIT)) {
			int rounding_mode = support::get_rounding_mode();
			types::Sign sign = bits.sign();

			if ((sign == types::Sign::POS && rounding_mode == FE_UPWARD) ||
				(sign == types::Sign::NEG && rounding_mode == FE_DOWNWARD)) {
				return support::FPBits<T>::min_subnormal(sign).get_val();
}

			//set_errno_if_required(ERANGE);
			//raise_except_if_required(FE_UNDERFLOW);
			return support::FPBits<T>::zero(sign).get_val();
		}

		// For all other values, NormalFloat to T conversion handles it the right way.
		support::DyadicFloat<support::FPBits<T>::STORAGE_LEN> normal(bits.get_val());
		normal.exponent += exp;
		return static_cast<T>(normal);
	}
}