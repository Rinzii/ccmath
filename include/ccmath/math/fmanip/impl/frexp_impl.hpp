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

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"

namespace ccm::internal::impl
{
	template <typename T>
	constexpr T frexp_impl(T x, int & exp) noexcept
	{
		using fp_bits_t = ccm::support::fp::FPBits<T>;

		fp_bits_t bits(x);

		if (CCM_UNLIKELY(bits.is_zero()))
		{
			exp = 0;
			return x;
		}

		if (CCM_UNLIKELY(bits.is_inf_or_nan())) { return x; }

		int exponent = bits.get_exponent();

		if (CCM_UNLIKELY(bits.is_subnormal()))
		{
			exponent = bits.get_explicit_exponent();
			T value	 = bits.get_val();
			while (bits.is_subnormal())
			{
				value *= static_cast<T>(2.0);
				--exponent;
				bits = fp_bits_t(value);
			}
		}

		bits.set_biased_exponent(static_cast<typename fp_bits_t::storage_type>(fp_bits_t::exponent_bias - 1));
		exp = exponent + 1;
		return bits.get_val();
	}
} // namespace ccm::internal::impl
