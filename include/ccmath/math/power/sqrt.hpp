/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/type_support.hpp"
#include "ccmath/internal/support/always_false.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/fp_bits.hpp"
#include "ccmath/internal/support/rounding_mode.hpp"

#include <type_traits>

namespace ccm
{
	namespace internal
	{
		template <typename T>
		struct Is80BitLongDouble
		{
			static constexpr bool value = false;
		};

#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
		template <>
		struct Is80BitLongDouble<long double>
		{
			static constexpr bool value = true;
		};
#endif

		template <typename T>
		constexpr bool Is80BitLongDouble_v = Is80BitLongDouble<T>::value;

		template <typename T>
		constexpr void normalize(int & exponent, typename support::FPBits<T>::StorageType & mantissa)
		{
			const int shift = support::countl_zero(mantissa) - (8 * static_cast<int>(sizeof(mantissa)) - 1 - support::FPBits<T>::FRACTION_LEN);
			exponent -= shift;
			mantissa <<= shift;
		}

#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT64)
		template <>
		constexpr void normalize<long double>(int & exponent, std::uint64_t & mantissa)
		{
			normalize<double>(exponent, mantissa);
		}
#endif

		namespace impl
		{
			// TODO: This likely does not work with 128 bit floats yet.
			template <typename T>
			constexpr std::enable_if_t<std::is_floating_point_v<T>, T> sqrt_calc_bits(support::FPBits<T> & bits)
			{
				using FPBits_t			  = support::FPBits<T>;
				using StorageType		  = typename FPBits_t::StorageType;
				constexpr StorageType one = StorageType(1) << FPBits_t::FRACTION_LEN;

				int x_exp		   = bits.get_exponent();
				StorageType x_mant = bits.get_mantissa();

				// If we have a denormal value, normalize it.
				if (bits.is_subnormal())
				{
					++x_exp; // ensure that x_exp is the correct exponent of one bit.
					internal::normalize<T>(x_exp, x_mant);
				}
				else { x_mant |= one; }

				// Ensure that the exponent is even.
				if (x_exp & 1)
				{
					--x_exp;
					x_mant <<= 1;
				}

				StorageType y = one;
				StorageType r = x_mant - one;

				for (StorageType current_bit = one >> 1; current_bit; current_bit >>= 1)
				{
					r <<= 1;
					StorageType tmp = (y << 1) + current_bit; // 2*y(n - 1) + 2^(-n-1)
					if (r >= tmp)
					{
						r -= tmp;
						y += current_bit;
					}
				}

				// We perform one more iteration to ensure that the result is correctly rounded.
				bool lsb	   = static_cast<bool>(y & 1); // Least significant bit
				bool round_bit = false;

				r <<= 2;
				StorageType tmp = (y << 2) + 1;

				if (r >= tmp)
				{
					r -= tmp;
					round_bit = true;
				}

				// Remove the hidden bit and append the exponent field.
				x_exp = ((x_exp >> 1) + FPBits_t::EXP_BIAS);

				y = (y - one) | (static_cast<StorageType>(x_exp) << FPBits_t::FRACTION_LEN);

				switch (support::get_rounding_mode())
				{
				case FE_TONEAREST:
					// Round to nearest, ties to even
					if (round_bit && (lsb || (r != 0))) { ++y; }
					break;
				case FE_UPWARD:
					if (round_bit || (r != 0)) { ++y; }
					break;
				default: break;
				}

				return support::bit_cast<T>(y);
			}

			// This calculates the square root of any IEEE-754 floating point number using the shift and add algorithm.
			// The function accounts for all rounding modes and special cases.
			template <typename T>
			constexpr std::enable_if_t<std::is_floating_point_v<T>, T> sqrt_impl(T x) // NOLINT
			{
				// TODO: Until we have implemented x86 long double support this will always be false.
				if constexpr (Is80BitLongDouble_v<T> && false) // NOLINT
				{
					// TODO: 80 bit long double sqrt implementation
					static_assert(support::always_false<T>, "80 bit long double sqrt not implemented yet.");
					return 0;
				}
				else
				{
					// IEEE floating points formats.
					using FPBits_t		   = support::FPBits<T>;
					constexpr auto flt_nan = FPBits_t::quiet_nan().get_val();

					FPBits_t bits(x);

					// Handle special cases where the bits are +Inf, ±0, or ±NaN
					if (bits == FPBits_t::inf(types::Sign::POS) || bits.is_zero() || bits.is_nan()) { return x; }

					// Handle special cases where the bits are -Inf or -num
					if (bits.is_neg()) { return -flt_nan; }

					// If we didn't encounter any special cases, we can calculate the square root normally.
					return sqrt_calc_bits(bits);
				}
			}
		} // namespace impl
	} // namespace internal


	/**
	 * @brief Calculates the square root of a number.
	 * @tparam T Floating-point type or integer type.
	 * @param num Floating-point or integer number.
	 * @return If no errors occur, square root of num (√num), is returned.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T sqrt(T num)
	{
		return internal::impl::sqrt_impl(num);
	}

	/**
	 * @brief Calculates the square root of a number.
	 * @tparam Integer Integer type.
	 * @param num Integer number.
	 * @return If no errors occur, square root of num (√num), is returned.
	 */
	template <typename Integer, std::enable_if_t<!std::is_floating_point_v<Integer>, bool> = true>
	constexpr double sqrt(Integer num)
	{
		return ccm::sqrt<double>(static_cast<double>(num));
	}

	/**
	 * @brief Calculates the square root of a number.
	 * @param num Floating-point number.
	 * @return If no errors occur, square root of num (√num), is returned.
	 */
	constexpr float sqrtf(float num)
	{
		return ccm::sqrt<float>(num);
	}

	/**
	 * @brief Calculates the square root of a number.
	 * @param num Floating-point number.
	 * @return If no errors occur, square root of num (√num), is returned.
	 */
	constexpr long double sqrtl(long double num)
	{
		return ccm::sqrt<long double>(num);
	}
} // namespace ccm

/// @ingroup power