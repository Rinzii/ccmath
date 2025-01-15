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

#include "ccmath/internal/math/generic/func/basic/max_gen.hpp"
#include "ccmath/internal/math/generic/func/basic/min_gen.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fenv/rounding_mode.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/types/dyadic_float.hpp"

#include <type_traits>

namespace ccm::support::fp::op
{
	template <bool IsSub, typename OutType, typename InType>
	constexpr std::enable_if_t<std::is_floating_point_v<OutType> && std::is_floating_point_v<InType> && sizeof(OutType) <= sizeof(InType), OutType>
	add_or_sub(InType x, InType y) noexcept // NOLINT(readability-function-cognitive-complexity)
	{
		using OutFPBits		 = FPBits<OutType>;
		using OutStorageType = typename OutFPBits::storage_type;
		using InFPBits		 = FPBits<InType>;
		using InStorageType	 = typename InFPBits::storage_type;

		constexpr int guard_bits_length		 = 3;
		constexpr int result_fraction_length = InFPBits::fraction_length + guard_bits_length;
		constexpr int result_mantissa_length = result_fraction_length + 1;

		using DyadicFloat = types::DyadicFloat<bit_ceil(static_cast<std::size_t>(result_mantissa_length))>;

		InFPBits x_bits(x);
		InFPBits y_bits(y);

		bool is_effectively_add = (x_bits.sign() == y_bits.sign()) != IsSub;

		if (CCM_UNLIKELY(x_bits.is_inf_or_nan() || y_bits.is_inf_or_nan() || x_bits.is_zero() || y_bits.is_zero()))
		{
			if (x_bits.is_nan() || y_bits.is_nan())
			{
				if (x_bits.is_signaling_nan() || y_bits.is_signaling_nan()) { fenv::raise_except_if_required(FE_INVALID); }

				if (x_bits.is_quiet_nan())
				{
					InStorageType x_payload = x_bits.get_mantissa();
					x_payload >>= InFPBits::fraction_length - OutFPBits::fraction_length;
					return OutFPBits::quiet_nan(x_bits.sign(), static_cast<OutStorageType>(x_payload)).get_val();
				}

				if (y_bits.is_quiet_nan())
				{
					InStorageType y_payload = y_bits.get_mantissa();
					y_payload >>= InFPBits::fraction_length - OutFPBits::fraction_length;
					return OutFPBits::quiet_nan(y_bits.sign(), static_cast<OutStorageType>(y_payload)).get_val();
				}

				return OutFPBits::quiet_nan().get_val();
			}

			if (x_bits.is_inf())
			{
				if (y_bits.is_inf())
				{
					if (!is_effectively_add)
					{
						fenv::raise_except_if_required(FE_INVALID);
						return OutFPBits::quiet_nan().get_val();
					}

					return OutFPBits::inf(x_bits.sign()).get_val();
				}

				return OutFPBits::inf(x_bits.sign()).get_val();
			}

			if (y_bits.is_inf()) { return OutFPBits::inf(y_bits.sign()).get_val(); }

			if (x_bits.is_zero())
			{
				if (y_bits.is_zero())
				{
					switch (fenv::get_rounding_mode())
					{
					case FE_DOWNWARD: return OutFPBits::zero(types::Sign::NEG).get_val();
					default: return OutFPBits::zero(types::Sign::POS).get_val();
					}
				}

				// volatile prevents Clang from converting tmp to OutType and then
				// immediately back to InType before negating it, resulting in double
				// rounding.
				volatile InType tmp = y;
				if constexpr (IsSub) { tmp = -tmp; }
				return static_cast<OutType>(tmp);
			}
			if (y_bits.is_zero())
			{
				volatile InType tmp = y;
				if constexpr (IsSub) { tmp = -tmp; }
				return static_cast<OutType>(tmp);
			}
		}

		InType x_abs = x_bits.abs().get_val();
		InType y_abs = y_bits.abs().get_val();

		if (x_abs == y_abs && !is_effectively_add)
		{
			switch (fenv::get_rounding_mode())
			{
			case FE_DOWNWARD: return OutFPBits::zero(types::Sign::NEG).get_val();
			default: return OutFPBits::zero(types::Sign::POS).get_val();
			}
		}

		types::Sign result_sign = types::Sign::POS;

		if (x_abs > y_abs) { result_sign = x_bits.sign(); } // NOLINT(bugprone-branch-clone)
		else if (x_abs < y_abs)
		{
			if (is_effectively_add) { result_sign = y_bits.sign(); }
			else if (y_bits.is_pos()) { result_sign = types::Sign::NEG; }
		}
		else if (is_effectively_add) { result_sign = x_bits.sign(); }

		InFPBits max_bits(ccm::gen::max(x_abs, y_abs));
		InFPBits min_bits(ccm::gen::min(x_abs, y_abs));

		InStorageType result_mant;

		if (max_bits.is_subnormal())
		{
			// min_bits must be subnormal too.

			if (is_effectively_add) { result_mant = max_bits.get_mantissa() + min_bits.get_mantissa(); }
			else { result_mant = max_bits.get_mantissa() - min_bits.get_mantissa(); }

			result_mant <<= guard_bits_length;
		}
		else
		{
			InStorageType max_mant = max_bits.get_explicit_mantissa() << guard_bits_length;
			InStorageType min_mant = min_bits.get_explicit_mantissa() << guard_bits_length;
			int alignment		   = max_bits.get_biased_exponent() - min_bits.get_biased_exponent();

			InStorageType aligned_min_mant = min_mant >> ccm::gen::min(alignment, result_mantissa_length);
			bool aligned_min_mant_sticky   = false;

			if (alignment <= 3) { aligned_min_mant_sticky = false; }
			else if (alignment <= InFPBits::fraction_length + 3) { aligned_min_mant_sticky = (min_mant << (InFPBits::storage_length - alignment)) != 0; }
			else { aligned_min_mant_sticky = true; }

			InStorageType min_mant_sticky(static_cast<int>(aligned_min_mant_sticky));

			if (is_effectively_add) { result_mant = max_mant + (aligned_min_mant | min_mant_sticky); }
			else { result_mant = max_mant - (aligned_min_mant | min_mant_sticky); }
		}

		int result_exp = max_bits.get_exponent() - result_fraction_length;
		DyadicFloat result(result_sign, result_exp, result_mant);
		return result.template as<OutType, /*ShouldSignalExceptions=*/true>();
	}

} // namespace ccm::support::fp::op