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

#include "ccmath/internal/math/generic/builtins/basic/fma.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fenv/rounding_mode.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/internal/types/big_int.hpp"
#include "ccmath/internal/types/dyadic_float.hpp"
#include "ccmath/internal/types/sign.hpp"

#include <cfenv>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace ccm::support::fp
{
	namespace fma_internal
	{
		template <typename Int>
		[[nodiscard]] constexpr bool int_is_zero(const Int& value)
		{
			if constexpr (std::is_integral_v<Int>) { return value == Int(0); }
			else { return value.is_zero(); }
		}

		template <typename Int>
		[[nodiscard]] constexpr int int_bit_width(const Int& value)
		{
			if (int_is_zero(value)) { return 0; }
			return support::bit_width(value);
		}

		template <typename Int>
		[[nodiscard]] constexpr bool int_get_bit(const Int& value, int index)
		{
			if (index < 0) { return false; }
			if constexpr (std::is_integral_v<Int>) { return ((value >> index) & Int(1)) != Int(0); }
			else { return value.get_bit(static_cast<std::size_t>(index)); }
		}

		template <typename Int>
		[[nodiscard]] constexpr bool lower_bits_nonzero(const Int& value, int bit_count)
		{
			if (bit_count <= 0) { return false; }
			constexpr int DIGITS = std::numeric_limits<Int>::digits;
			if (bit_count >= DIGITS) { return !int_is_zero(value); }

			if constexpr (std::is_integral_v<Int>)
			{
				const Int mask = (Int{ 1 } << bit_count) - Int{ 1 };
				return (value & mask) != Int{ 0 };
			}
			else
			{
				for (int bit = 0; bit < bit_count; ++bit)
				{
					if (value.get_bit(static_cast<std::size_t>(bit))) { return true; }
				}
				return false;
			}
		}

		template <typename Int>
		[[nodiscard]] constexpr bool shift_right_with_sticky(int shift_length, Int& value)
		{
			if (shift_length <= 0 || int_is_zero(value)) { return false; }

			constexpr int DIGITS = std::numeric_limits<Int>::digits;
			if (shift_length >= DIGITS)
			{
				value = Int{ 0 };
				return true;
			}

			const bool sticky_bits = lower_bits_nonzero(value, shift_length);
			value >>= static_cast<std::size_t>(shift_length);
			return sticky_bits;
		}

		template <typename T>
		struct fixed_fma_traits;

		template <>
		struct fixed_fma_traits<float>
		{
			using product_int					  = std::uint64_t;
			using acc_int						  = types::UInt<128>;
			static constexpr int precision_bits	  = 24;
			static constexpr int accumulator_bits = 96;
		};

		template <>
		struct fixed_fma_traits<double>
		{
			using product_int					  = types::UInt<128>;
			using acc_int						  = types::UInt<256>;
			static constexpr int precision_bits	  = 53;
			static constexpr int accumulator_bits = 212;
		};

		template <typename T>
		inline constexpr bool has_fixed_fma_kernel_v = std::is_same_v<T, float> || std::is_same_v<T, double>;

		template <typename T>
		struct decoded_operand
		{
			types::Sign sign				 = types::Sign::POS;
			typename FPBits<T>::storage_type mantissa = 0;
			int width						 = 0;
			int lsb_exponent				 = 0;
			int top_exponent				 = 0;
		};

		template <typename T>
		[[nodiscard]] constexpr decoded_operand<T> decode_finite_nonzero(const FPBits<T>& bits)
		{
			decoded_operand<T> decoded{};
			decoded.sign		 = bits.sign();
			decoded.mantissa	 = bits.get_explicit_mantissa();
			decoded.width		 = support::bit_width(decoded.mantissa);
			decoded.lsb_exponent = bits.get_explicit_exponent() - static_cast<int>(FPBits<T>::fraction_length);
			decoded.top_exponent = decoded.lsb_exponent + decoded.width - 1;
			return decoded;
		}

		template <typename T, bool ShouldSignalExceptions>
		[[nodiscard]] constexpr std::enable_if_t<std::is_floating_point_v<T>, T> wide_fma(T x, T y, T z);

		template <typename T>
		[[nodiscard]] constexpr T propagate_quiet_nan(const FPBits<T>& bits)
		{
			return FPBits<T>::quiet_nan(bits.sign(), bits.get_mantissa()).get_val();
		}

		template <typename T, bool>
		[[nodiscard]] constexpr bool special_case_fma(const FPBits<T>& x_bits, const FPBits<T>& y_bits, const FPBits<T>& z_bits, T& result)
		{
			using Sign = types::Sign;

			if (CCM_UNLIKELY(x_bits.is_nan() || y_bits.is_nan() || z_bits.is_nan()))
			{
				if (x_bits.is_signaling_nan() || y_bits.is_signaling_nan() || z_bits.is_signaling_nan()) { fenv::raise_except_if_required(FE_INVALID); }

				// IEEE-754 fusedMultiplyAdd treats a 0*inf (or inf*0) product as an invalid operation
				// even when the addend is a quiet NaN. std::fma and the native fma instruction signal
				// it, so the software path matches them for consistency (7.2 leaves the quiet-NaN-addend
				// case implementation defined; x and y are non-NaN when they form the 0*inf product).
				if (CCM_UNLIKELY((x_bits.is_zero() && y_bits.is_inf()) || (x_bits.is_inf() && y_bits.is_zero())))
				{
					if (!is_constant_evaluated()) { fenv::set_errno_if_required(EDOM); }
					fenv::raise_except_if_required(FE_INVALID);
				}

				if (x_bits.is_quiet_nan())
				{
					result = propagate_quiet_nan(x_bits);
					return true;
				}
				if (y_bits.is_quiet_nan())
				{
					result = propagate_quiet_nan(y_bits);
					return true;
				}
				if (z_bits.is_quiet_nan())
				{
					result = propagate_quiet_nan(z_bits);
					return true;
				}

				result = FPBits<T>::quiet_nan().get_val();
				return true;
			}

			if (CCM_UNLIKELY(x_bits.is_inf() || y_bits.is_inf()))
			{
				if (x_bits.is_zero() || y_bits.is_zero())
				{
					if (!is_constant_evaluated()) { fenv::set_errno_if_required(EDOM); }
					fenv::raise_except_if_required(FE_INVALID);
					result = FPBits<T>::quiet_nan().get_val();
					return true;
				}

				const Sign product_sign = (x_bits.sign() == y_bits.sign()) ? Sign::POS : Sign::NEG;
				if (z_bits.is_inf() && z_bits.sign() != product_sign)
				{
					if (!is_constant_evaluated()) { fenv::set_errno_if_required(EDOM); }
					fenv::raise_except_if_required(FE_INVALID);
					result = FPBits<T>::quiet_nan().get_val();
					return true;
				}

				result = FPBits<T>::inf(product_sign).get_val();
				return true;
			}

			if (CCM_UNLIKELY(z_bits.is_inf()))
			{
				result = z_bits.get_val();
				return true;
			}

			if (CCM_UNLIKELY(x_bits.is_zero() || y_bits.is_zero()))
			{
				const Sign product_sign = x_bits.sign() == y_bits.sign() ? Sign::POS : Sign::NEG;
				if (z_bits.is_zero())
				{
					Sign zero_sign = product_sign;
					if (product_sign != z_bits.sign()) { zero_sign = (fenv::get_rounding_mode() == FE_DOWNWARD) ? Sign::NEG : Sign::POS; }
					result = FPBits<T>::zero(zero_sign).get_val();
				}
				else { result = z_bits.get_val(); }
				return true;
			}

			return false;
		}

		template <typename T, typename Int, int ActiveBits, bool ShouldSignalExceptions>
		[[nodiscard]] constexpr T round_normalized(types::Sign sign, int base_exponent, Int mantissa, bool sticky_tail)
		{
			using OutFPBits	  = FPBits<T>;
			using StorageType = typename OutFPBits::storage_type;

			if (int_is_zero(mantissa)) { return OutFPBits::zero(sign).get_val(); }

			constexpr int FRACTION_BITS	   = static_cast<int>(OutFPBits::fraction_length);
			constexpr int PRECISION_BITS   = FRACTION_BITS + 1;
			constexpr int NORMAL_SHIFT	   = ActiveBits - PRECISION_BITS;
			constexpr int MIN_EXPONENT	   = 1 - OutFPBits::exponent_bias;
			constexpr int MIN_SUBNORMAL_LS = MIN_EXPONENT - FRACTION_BITS;
			const int top_exponent		   = base_exponent + (ActiveBits - 1);

			if (top_exponent > OutFPBits::exponent_bias)
			{
				if constexpr (ShouldSignalExceptions)
				{
					if (!is_constant_evaluated())
					{
						fenv::set_errno_if_required(ERANGE);
						fenv::raise_except_if_required(FE_OVERFLOW | FE_INEXACT);
					}
				}

				switch (fenv::get_rounding_mode())
				{
				case FE_TONEAREST: return OutFPBits::inf(sign).get_val();
				case FE_TOWARDZERO: return OutFPBits::max_normal(sign).get_val();
				case FE_DOWNWARD:
					if (sign.is_pos()) { return OutFPBits::max_normal(types::Sign::POS).get_val(); }
					return OutFPBits::inf(types::Sign::NEG).get_val();
				case FE_UPWARD:
					if (sign.is_neg()) { return OutFPBits::max_normal(types::Sign::NEG).get_val(); }
					return OutFPBits::inf(types::Sign::POS).get_val();
				default: support::unreachable();
				}
			}

			StorageType out_biased_exponent = 0;
			StorageType out_mantissa		= 0;
			bool round_bit					= false;
			bool sticky_bit					= sticky_tail;
			bool underflow					= false;

			if (top_exponent >= MIN_EXPONENT)
			{
				out_biased_exponent = static_cast<StorageType>(top_exponent + OutFPBits::exponent_bias);
				round_bit			= int_get_bit(mantissa, NORMAL_SHIFT - 1);
				sticky_bit |= lower_bits_nonzero(mantissa, NORMAL_SHIFT - 1);
				out_mantissa = static_cast<StorageType>(mantissa >> static_cast<std::size_t>(NORMAL_SHIFT));
			}
			else
			{
				underflow				  = true;
				const int subnormal_shift = MIN_SUBNORMAL_LS - base_exponent;

				if (subnormal_shift > ActiveBits) { sticky_bit = true; }
				else if (subnormal_shift == ActiveBits)
				{
					round_bit = int_get_bit(mantissa, ActiveBits - 1);
					sticky_bit |= lower_bits_nonzero(mantissa, ActiveBits - 1);
				}
				else
				{
					round_bit = int_get_bit(mantissa, subnormal_shift - 1);
					sticky_bit |= lower_bits_nonzero(mantissa, subnormal_shift - 1);
					out_mantissa = static_cast<StorageType>(mantissa >> static_cast<std::size_t>(subnormal_shift));
				}
			}

			const bool lsb	   = (out_mantissa & StorageType(1)) != 0;
			StorageType result = OutFPBits::create_value(sign, out_biased_exponent, out_mantissa).uintval();

			switch (fenv::get_rounding_mode())
			{
			case FE_TONEAREST:
				if (round_bit && (lsb || sticky_bit)) { ++result; }
				break;
			case FE_DOWNWARD:
				if (sign.is_neg() && (round_bit || sticky_bit)) { ++result; }
				break;
			case FE_UPWARD:
				if (sign.is_pos() && (round_bit || sticky_bit)) { ++result; }
				break;
			default: break;
			}

			if constexpr (ShouldSignalExceptions)
			{
				if (round_bit || sticky_bit)
				{
					int excepts = FE_INEXACT;
					if (OutFPBits(result).is_inf())
					{
						if (!is_constant_evaluated()) { fenv::set_errno_if_required(ERANGE); }
						excepts |= FE_OVERFLOW;
					}
					else if (underflow)
					{
						if (!is_constant_evaluated()) { fenv::set_errno_if_required(ERANGE); }
						excepts |= FE_UNDERFLOW;
					}
					fenv::raise_except_if_required(excepts);
				}
			}

			return OutFPBits(result).get_val();
		}

		template <typename T, bool ShouldSignalExceptions>
		[[nodiscard]] constexpr std::enable_if_t<has_fixed_fma_kernel_v<T>, T> fixed_fma(T x, T y, T z)
		{
			using Traits	  = fixed_fma_traits<T>;
			using FPBits	  = FPBits<T>;
			using StorageType = typename FPBits::storage_type;
			using ProductInt  = Traits::product_int;
			using AccInt	  = Traits::acc_int;
			using Sign		  = types::Sign;

			FPBits x_bits(x);
			FPBits y_bits(y);
			FPBits z_bits(z);

			T special_result{};
			if (special_case_fma<T, ShouldSignalExceptions>(x_bits, y_bits, z_bits, special_result)) { return special_result; }

			const auto x_value = decode_finite_nonzero<T>(x_bits);
			const auto y_value = decode_finite_nonzero<T>(y_bits);

			const ProductInt product_mantissa = ProductInt(x_value.mantissa) * ProductInt(y_value.mantissa);
			const int product_width			  = int_bit_width(product_mantissa);
			const int product_lsb_exponent	  = x_value.lsb_exponent + y_value.lsb_exponent;
			const int product_top_exponent	  = product_lsb_exponent + product_width - 1;
			const Sign product_sign			  = (x_value.sign == y_value.sign) ? Sign::POS : Sign::NEG;

			AccInt product_acc = AccInt(product_mantissa) << static_cast<std::size_t>(Traits::accumulator_bits - product_width);
			if (z_bits.is_zero())
			{
				const int product_base_exponent = product_top_exponent - (Traits::accumulator_bits - 1);
				return round_normalized<T, AccInt, Traits::accumulator_bits, ShouldSignalExceptions>(product_sign, product_base_exponent, product_acc, false);
			}

			const auto z_value = decode_finite_nonzero<T>(z_bits);
			AccInt z_acc	   = AccInt(StorageType(z_value.mantissa)) << static_cast<std::size_t>(Traits::accumulator_bits - z_value.width);

			bool product_sticky = false;
			bool z_sticky		= false;
			int base_exponent	= product_top_exponent - (Traits::accumulator_bits - 1);

			if (product_top_exponent > z_value.top_exponent)
			{
				z_sticky	  = shift_right_with_sticky(product_top_exponent - z_value.top_exponent, z_acc);
				base_exponent = product_top_exponent - (Traits::accumulator_bits - 1);
			}
			else if (z_value.top_exponent > product_top_exponent)
			{
				product_sticky = shift_right_with_sticky(z_value.top_exponent - product_top_exponent, product_acc);
				base_exponent  = z_value.top_exponent - (Traits::accumulator_bits - 1);
			}

			AccInt result_mantissa{ 0 };
			bool sticky_tail = false;
			Sign result_sign = product_sign;

			if (product_sign == z_value.sign)
			{
				result_mantissa = product_acc + z_acc;
				sticky_tail		= product_sticky || z_sticky;

				if (int_bit_width(result_mantissa) > Traits::accumulator_bits)
				{
					sticky_tail |= int_get_bit(result_mantissa, 0);
					result_mantissa >>= 1;
					++base_exponent;
				}
			}
			else
			{
				int compare = 0;
				if (product_acc > z_acc) { compare = 1; }
				else if (product_acc < z_acc) { compare = -1; }
				else if (product_sticky != z_sticky) { compare = product_sticky ? 1 : -1; }

				if (compare == 0)
				{
					const Sign zero_sign = fenv::get_rounding_mode() == FE_DOWNWARD ? Sign::NEG : Sign::POS;
					return FPBits::zero(zero_sign).get_val();
				}

				if (compare > 0)
				{
					result_sign = product_sign;
					if (z_sticky)
					{
						result_mantissa = product_acc - z_acc - AccInt{ 1 };
						sticky_tail		= true;
					}
					else
					{
						result_mantissa = product_acc - z_acc;
						sticky_tail		= product_sticky;
					}
				}
				else
				{
					result_sign = z_value.sign;
					if (product_sticky)
					{
						result_mantissa = z_acc - product_acc - AccInt{ 1 };
						sticky_tail		= true;
					}
					else
					{
						result_mantissa = z_acc - product_acc;
						sticky_tail		= z_sticky;
					}
				}

				if (int_is_zero(result_mantissa))
				{
					if (!sticky_tail)
					{
						const Sign zero_sign = fenv::get_rounding_mode() == FE_DOWNWARD ? Sign::NEG : Sign::POS;
						return FPBits::zero(zero_sign).get_val();
					}

					return wide_fma<T, ShouldSignalExceptions>(x, y, z);
				}

				const int normalize_shift = Traits::accumulator_bits - int_bit_width(result_mantissa);
				result_mantissa <<= static_cast<std::size_t>(normalize_shift);
				base_exponent -= normalize_shift;
			}

			return round_normalized<T, AccInt, Traits::accumulator_bits, ShouldSignalExceptions>(result_sign, base_exponent, result_mantissa, sticky_tail);
		}

		// Exact wide-integer FMA used for directed modes, constexpr policy modes, and every
		// binary64 software fallback. The product is formed exactly, the addend is aligned with a
		// sticky bit, and the result is rounded once through DyadicFloat::generic_as.
		template <typename T, bool ShouldSignalExceptions>
		[[nodiscard]] constexpr std::enable_if_t<std::is_floating_point_v<T>, T> wide_fma(T x, T y, T z)
		{
			using FPBits	  = FPBits<T>;
			using StorageType = typename FPBits::storage_type;
			using Sign		  = types::Sign;

			constexpr int IN_EXPLICIT_MANT_LEN	 = static_cast<int>(FPBits::fraction_length) + 1;
			constexpr std::size_t PROD_LEN		 = 2U * static_cast<std::size_t>(IN_EXPLICIT_MANT_LEN);
			constexpr std::size_t TMP_RESULT_LEN = bit_ceil(PROD_LEN + 1U);
			using TmpResultType					 = types::UInt<TMP_RESULT_LEN>;
			using WideFloat						 = types::DyadicFloat<TMP_RESULT_LEN>;

			FPBits x_bits(x);
			FPBits y_bits(y);
			FPBits z_bits(z);

			if (CCM_UNLIKELY(x_bits.is_nan() || y_bits.is_nan() || z_bits.is_nan()))
			{
				if (x_bits.is_signaling_nan() || y_bits.is_signaling_nan() || z_bits.is_signaling_nan()) { fenv::raise_except_if_required(FE_INVALID); }
				if (x_bits.is_quiet_nan()) { return FPBits::quiet_nan(x_bits.sign(), x_bits.get_mantissa()).get_val(); }
				if (y_bits.is_quiet_nan()) { return FPBits::quiet_nan(y_bits.sign(), y_bits.get_mantissa()).get_val(); }
				if (z_bits.is_quiet_nan()) { return FPBits::quiet_nan(z_bits.sign(), z_bits.get_mantissa()).get_val(); }
				return FPBits::quiet_nan().get_val();
			}

			if (CCM_UNLIKELY(x == T(0) || y == T(0) || z == T(0))) { return x * y + z; }

			int x_exp = 0;
			int y_exp = 0;
			int z_exp = 0;

			const StorageType implicit_mask = static_cast<StorageType>(FPBits::significand_mask - FPBits::fraction_mask);
			const T denormal_scaling =
				FPBits::create_value(Sign::POS, static_cast<StorageType>(static_cast<int>(FPBits::fraction_length) + FPBits::exponent_bias), implicit_mask)
					.get_val();

			if (CCM_UNLIKELY(x_bits.is_subnormal()))
			{
				x_exp -= static_cast<int>(FPBits::fraction_length);
				x *= denormal_scaling;
			}
			if (CCM_UNLIKELY(y_bits.is_subnormal()))
			{
				y_exp -= static_cast<int>(FPBits::fraction_length);
				y *= denormal_scaling;
			}
			if (CCM_UNLIKELY(z_bits.is_subnormal()))
			{
				z_exp -= static_cast<int>(FPBits::fraction_length);
				z *= denormal_scaling;
			}

			x_bits			  = FPBits(x);
			y_bits			  = FPBits(y);
			z_bits			  = FPBits(z);
			const Sign z_sign = z_bits.sign();
			Sign prod_sign	  = x_bits.sign() == y_bits.sign() ? Sign::POS : Sign::NEG;
			x_exp += static_cast<int>(x_bits.get_biased_exponent());
			y_exp += static_cast<int>(y_bits.get_biased_exponent());
			z_exp += static_cast<int>(z_bits.get_biased_exponent());

			if (CCM_UNLIKELY(x_exp == FPBits::max_biased_exponent || y_exp == FPBits::max_biased_exponent || z_exp == FPBits::max_biased_exponent))
			{
				if (CCM_UNLIKELY(x_exp != FPBits::max_biased_exponent && y_exp != FPBits::max_biased_exponent && z_bits.is_inf())) { return z; }
				return x * y + z;
			}

			const StorageType x_mant = x_bits.get_explicit_mantissa();
			const StorageType y_mant = y_bits.get_explicit_mantissa();
			TmpResultType z_mant	 = TmpResultType(z_bits.get_explicit_mantissa());

			TmpResultType prod_mant = TmpResultType(x_mant) * TmpResultType(y_mant);
			int prod_lsb_exp		= x_exp + y_exp - (FPBits::exponent_bias + 2 * static_cast<int>(FPBits::fraction_length));

			constexpr int RESULT_MIN_LEN = static_cast<int>(PROD_LEN) - static_cast<int>(FPBits::fraction_length);
			z_mant <<= static_cast<std::size_t>(RESULT_MIN_LEN);
			int z_lsb_exp	 = z_exp - (static_cast<int>(FPBits::fraction_length) + RESULT_MIN_LEN);
			bool sticky_bits = false;
			bool z_shifted	 = false;

			if (prod_lsb_exp < z_lsb_exp)
			{
				sticky_bits	 = shift_right_with_sticky(z_lsb_exp - prod_lsb_exp, prod_mant);
				prod_lsb_exp = z_lsb_exp;
			}
			else if (z_lsb_exp < prod_lsb_exp)
			{
				z_shifted	= true;
				sticky_bits = shift_right_with_sticky(prod_lsb_exp - z_lsb_exp, z_mant);
			}

			if (prod_sign == z_sign) { prod_mant += z_mant; }
			else
			{
				if (prod_mant >= z_mant)
				{
					if (z_shifted && sticky_bits) { ++z_mant; }
					prod_mant -= z_mant;
				}
				else
				{
					if (!z_shifted && sticky_bits) { ++prod_mant; }
					prod_mant = z_mant - prod_mant;
					prod_sign = z_sign;
				}
			}

			if (prod_mant == TmpResultType(0)) { prod_sign = fenv::get_rounding_mode() == FE_DOWNWARD ? Sign::NEG : Sign::POS; }

			WideFloat result(prod_sign, prod_lsb_exp - FPBits::exponent_bias, prod_mant);
			if (sticky_bits) { result.mantissa |= TmpResultType(1); }
			return result.template generic_as<T, ShouldSignalExceptions>();
		}

		template <bool ShouldSignalExceptions>
		[[nodiscard]] constexpr float software_fmaf(float x, float y, float z)
		{
			// The fixed binary32 kernel forms the product exactly in a wide accumulator and rounds
			// once, so it is correctly rounded for every rounding mode. A "fast" double-precision
			// evaluation (double(x)*double(y) + double(z), cast back to float) is NOT correct: the
			// binary64 add rounds and the final cast rounds again, producing a double-rounding error
			// (e.g. fmaf(1+2^-12, 1+2^-12, 2^-53) is off by 1 ulp). Always use the fixed kernel.
			return fixed_fma<float, ShouldSignalExceptions>(x, y, z);
		}
	} // namespace fma_internal

	// Correctly-rounded software fused multiply-add for every rounding mode. Ported from LLVM
	// libc's generic FMA (libc/src/__support/FPUtil/generic/FMA.h). This quiet variant is used by
	// internal error-free transforms and deliberately suppresses public FE_INEXACT / range flag
	// signaling while still producing the exact fused result under the active rounding mode.
	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<std::is_floating_point_v<T>, T> generic_fma(T x, T y, T z)
	{
		if constexpr (std::is_same_v<T, float>) { return fma_internal::software_fmaf</*ShouldSignalExceptions=*/false>(x, y, z); }
		else if constexpr (std::is_same_v<T, double>) { return fma_internal::fixed_fma<double, /*ShouldSignalExceptions=*/false>(x, y, z); }
		else { return fma_internal::wide_fma<T, /*ShouldSignalExceptions=*/false>(x, y, z); }
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<std::is_floating_point_v<T>, T> public_fma(T x, T y, T z)
	{
		if constexpr (std::is_same_v<T, float>) { return fma_internal::software_fmaf</*ShouldSignalExceptions=*/true>(x, y, z); }
		else if constexpr (std::is_same_v<T, double>) { return fma_internal::fixed_fma<double, /*ShouldSignalExceptions=*/true>(x, y, z); }
		else { return fma_internal::wide_fma<T, /*ShouldSignalExceptions=*/true>(x, y, z); }
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<std::is_floating_point_v<T>, T> dispatch_fma(T x, T y, T z)
	{
		if constexpr (ccm::builtin::has_constexpr_fma<T>)
		{
			if (fenv::constant_eval_rounding_mode() == FE_TONEAREST) { return builtin::fma(x, y, z); }
		}

		if (!is_constant_evaluated())
		{
			// The native FMA instruction is only trusted under round-to-nearest. In directed rounding
			// modes the compiler may assume the default environment (FENV_ACCESS is widely unsupported,
			// e.g. ignored by Clang) and contract/fold __builtin_fma as if rounding were to-nearest,
			// yielding wrong directed-mode results. Fall back to the correctly-rounded software path.
			if constexpr (ccm::builtin::runtime_builtin_fma_trusted<T>)
			{
				if (fenv::get_rounding_mode() == FE_TONEAREST) { return builtin::fma(x, y, z); }
			}
		}

		return public_fma(x, y, z);
	}

	template <typename T>
	[[nodiscard]] std::enable_if_t<std::is_floating_point_v<T>, T> dispatch_runtime_fma(T x, T y, T z) noexcept
	{
		// See dispatch_fma: only trust the native FMA instruction under round-to-nearest.
		if constexpr (ccm::builtin::runtime_builtin_fma_trusted<T>)
		{
			if (fenv::get_rounding_mode() == FE_TONEAREST) { return builtin::fma(x, y, z); }
		}
		return public_fma(x, y, z);
	}
} // namespace ccm::support::fp
