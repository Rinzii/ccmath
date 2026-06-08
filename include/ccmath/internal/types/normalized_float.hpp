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

#include "ccmath/internal/config/type_support.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/support/type_traits.hpp"
#include "ccmath/internal/types/sign.hpp"

#include <cstdint>

namespace ccm::types
{
	// Normalized floating-point mantissa in the form (-1)^sign * 2^exponent * 1.fraction.
	// Zero, infinity, and NaN bit patterns are not represented here.
	template <typename T>
	struct NormalizedFloat
	{
		static_assert(support::traits::ccm_is_floating_point_v<T>, "NormalizedFloat requires a floating-point type.");

		using FPBits	   = support::fp::FPBits<T>;
		using StorageType  = typename FPBits::storage_type;
		static constexpr StorageType ONE = StorageType(1) << FPBits::fraction_length;

		std::int32_t exponent = 0;
		StorageType mantissa  = 0;
		Sign sign			  = Sign::POS;

		constexpr NormalizedFloat() = default;

		constexpr NormalizedFloat(Sign s, std::int32_t e, StorageType m) : exponent(e), mantissa(m), sign(s)
		{
			if (mantissa >= ONE) { return; }

			const unsigned normalization_shift = evaluate_normalization_shift(mantissa);
			mantissa <<= normalization_shift;
			exponent -= static_cast<std::int32_t>(normalization_shift);
		}

		constexpr explicit NormalizedFloat(T x) { init_from_bits(FPBits(x)); }

		constexpr explicit NormalizedFloat(FPBits bits) { init_from_bits(bits); }

		[[nodiscard]] constexpr int cmp(const NormalizedFloat & other) const
		{
			const int result = sign.is_neg() ? -1 : 1;
			if (sign != other.sign) { return result; }

			if (exponent > other.exponent) { return result; }
			if (exponent == other.exponent)
			{
				if (mantissa > other.mantissa) { return result; }
				if (mantissa == other.mantissa) { return 0; }
				return -result;
			}
			return -result;
		}

		[[nodiscard]] constexpr NormalizedFloat mul2(int e) const
		{
			NormalizedFloat result = *this;
			result.exponent += e;
			return result;
		}

		constexpr explicit operator T() const
		{
			if constexpr (support::fp::get_fp_type<T>() == support::fp::FPType::eBinary80)
			{
				return convert_float80();
			}
			else
			{
				return convert_ieee();
			}
		}

	private:
		static constexpr unsigned evaluate_normalization_shift(StorageType m)
		{
			unsigned shift = 0;
			for (; (ONE & m) == 0 && shift < FPBits::fraction_length; m <<= 1, ++shift) {}
			return shift;
		}

		constexpr void init_from_bits(FPBits bits)
		{
			sign = bits.sign();

			if (bits.is_inf_or_nan() || bits.is_zero())
			{
				exponent = 0;
				mantissa = 0;
				return;
			}

			if constexpr (support::fp::get_fp_type<T>() == support::fp::FPType::eBinary80) { init_from_bits_float80(bits); }
			else { init_from_bits_ieee(bits); }
		}

		constexpr void init_from_bits_ieee(FPBits bits)
		{
			if (bits.is_subnormal())
			{
				const unsigned shift = evaluate_normalization_shift(bits.get_mantissa());
				mantissa			 = static_cast<StorageType>(bits.get_mantissa() << shift);
				exponent			 = 1 - FPBits::exponent_bias - static_cast<std::int32_t>(shift);
			}
			else
			{
				exponent = bits.get_biased_exponent() - FPBits::exponent_bias;
				mantissa = ONE | bits.get_mantissa();
			}
		}

		constexpr void init_from_bits_float80(FPBits bits)
		{
			if (bits.is_subnormal())
			{
				if (bits.get_implicit_bit() == 0)
				{
					const int normalization_shift = static_cast<int>(evaluate_normalization_shift(bits.get_mantissa()));
					exponent					  = -16382 - normalization_shift;
					mantissa					  = bits.get_mantissa() << static_cast<unsigned>(normalization_shift);
				}
				else
				{
					exponent = -16382;
					mantissa = ONE | bits.get_mantissa();
				}
			}
			else
			{
				if (bits.get_implicit_bit() == 0)
				{
					exponent = 0;
					mantissa = 0;
				}
				else
				{
					exponent = bits.get_biased_exponent() - 16383;
					mantissa = ONE | bits.get_mantissa();
				}
			}
		}

		[[nodiscard]] constexpr T convert_ieee() const
		{
			int biased_exponent = exponent + FPBits::exponent_bias;
			constexpr int max_exponent_value = (1 << FPBits::exponent_length) - 2;
			if (biased_exponent > max_exponent_value) { return FPBits::inf(sign).get_val(); }

			FPBits result(T(0.0));
			result.set_sign(sign);

			constexpr int subnormal_exponent = -FPBits::exponent_bias + 1;
			if (exponent < subnormal_exponent)
			{
				const unsigned shift = static_cast<unsigned>(subnormal_exponent - exponent);
				if (shift <= FPBits::fraction_length + 1)
				{
					const StorageType shift_out_mask	= static_cast<StorageType>(StorageType(1) << shift) - 1;
					const StorageType shift_out_value	= mantissa & shift_out_mask;
					const StorageType halfway_value		= static_cast<StorageType>(StorageType(1) << (shift - 1));
					result.set_biased_exponent(0);
					result.set_mantissa(mantissa >> shift);
					StorageType new_mantissa = result.get_mantissa();
					if (shift_out_value > halfway_value) { new_mantissa += 1; }
					else if (shift_out_value == halfway_value)
					{
						if ((result.get_mantissa() & 1) != 0) { new_mantissa += 1; }
					}
					result.set_mantissa(new_mantissa);
					if (new_mantissa == ONE) { result.set_biased_exponent(1); }
					return result.get_val();
				}
				return result.get_val();
			}

			result.set_biased_exponent(static_cast<StorageType>(biased_exponent));
			result.set_mantissa(mantissa);
			return result.get_val();
		}

		[[nodiscard]] constexpr T convert_float80() const
		{
			int biased_exponent = exponent + 16383;
			constexpr int max_exponent_value = (1 << 15) - 2;
			if (biased_exponent > max_exponent_value) { return FPBits::inf(sign).get_val(); }

			FPBits result(0.0L);
			result.set_sign(sign);

			constexpr int subnormal_exponent = -16383 + 1;
			if (exponent < subnormal_exponent)
			{
				const unsigned shift = static_cast<unsigned>(subnormal_exponent - exponent);
				if (shift <= FPBits::fraction_length + 1)
				{
					const StorageType shift_out_mask	= (StorageType(1) << shift) - 1;
					const StorageType shift_out_value	= mantissa & shift_out_mask;
					const StorageType halfway_value		= StorageType(1) << (shift - 1);
					result.set_biased_exponent(0);
					result.set_mantissa(mantissa >> shift);
					StorageType new_mantissa = result.get_mantissa();
					if (shift_out_value > halfway_value) { new_mantissa += 1; }
					else if (shift_out_value == halfway_value)
					{
						if ((result.get_mantissa() & 1) != 0) { new_mantissa += 1; }
					}
					result.set_mantissa(new_mantissa);
					if (new_mantissa == ONE)
					{
						result.set_biased_exponent(1);
						result.set_implicit_bit(1);
					}
					else { result.set_implicit_bit(0); }
					return result.get_val();
				}
				return result.get_val();
			}

			result.set_biased_exponent(static_cast<StorageType>(biased_exponent));
			result.set_mantissa(mantissa);
			result.set_implicit_bit(1);
			return result.get_val();
		}
	};

	template <typename T>
	using NormalFloat = NormalizedFloat<T>;

} // namespace ccm::types
