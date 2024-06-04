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
#include "ccmath/internal/support/fenv/rounding_mode.hpp"
#include "ccmath/internal/support/fp_bits.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"

#include <type_traits>

#ifdef CCM_CONFIG_USE_RT_SIMD
	#include "ccmath/internal/config/arch/simd.hpp"

#endif
#include <emmintrin.h>

namespace ccm::support::math
{
	namespace internal
	{
		template <typename T>
		struct Is80BitLongDouble
		{
			static constexpr bool value = false;
		};

		template <typename T>
		constexpr void normalize(int & exponent, typename support::FPBits<T>::storage_type & mantissa)
		{
			const int shift = support::countl_zero(mantissa) - (8 * static_cast<int>(sizeof(mantissa)) - 1 - support::FPBits<T>::fraction_length);
			exponent -= shift;
			mantissa <<= shift;
		}

#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
		template <>
		struct Is80BitLongDouble<long double>
		{
			static constexpr bool value = true;
		};
#endif

#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT64)
		template <>
		constexpr void normalize<long double>(int & exponent, std::uint64_t & mantissa)
		{
			normalize<double>(exponent, mantissa);
		}
#elif defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
		template <>
		constexpr void normalize<long double>(int & exponent, types::uint128_t & mantissa)
		{
			const auto shift = static_cast<unsigned int>(static_cast<unsigned long>(support::countl_zero(static_cast<std::uint64_t>(mantissa))) -
														 (8 * sizeof(std::uint64_t) - 1 - support::FPBits<long double>::fraction_length));
			exponent -= shift; // NOLINT(cppcoreguidelines-narrowing-conversions, bugprone-narrowing-conversions)
			mantissa <<= shift;
		}
#endif

		template <typename T>
		constexpr bool Is80BitLongDouble_v = Is80BitLongDouble<T>::value;

		namespace impl
		{
			namespace bit80
			{
				// This has to be defined for sqrt_impl to work as it still needs to see that this function exists
				constexpr long double sqrt_calc_80bits(long double x);

#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
				constexpr long double sqrt_calc_80bits(long double x)
				{
					using Bits				   = support::FPBits<long double>;
					using storage_type		   = typename Bits::storage_type;
					constexpr storage_type one = static_cast<storage_type>(1) << Bits::fraction_length;
					constexpr auto nan_type	   = Bits::quiet_nan().get_val();

					Bits bits(x);

					if (bits == Bits::inf(types::Sign::POS) || bits.is_zero() || bits.is_nan()) { return x; }
					if (bits.is_neg()) { return nan_type; }

					int x_exp			= bits.get_explicit_exponent();
					storage_type x_mant = bits.get_mantissa();

					// If we have denormal values, normalize it.
					if (bits.get_implicit_bit()) { x_mant |= one; }
					else if (bits.is_subnormal()) { normalize<long double>(x_exp, x_mant); }

					// Ensure that the exponent is even.
					if ((x_exp & 1) != 0)
					{
						--x_exp;
						x_mant <<= 1;
					}

					storage_type y = one;
					storage_type r = x_mant - one;

					for (storage_type current_bit = one >> 1; current_bit != 0U; current_bit >>= 1)
					{
						r <<= 1;
						if (const storage_type tmp = (y << 1) + current_bit; r >= tmp)
						{
							r -= tmp;
							y += current_bit;
						}
					}

					// We perform one more iteration to ensure that the result is correctly rounded.
					const auto lsb = static_cast<bool>(y & 1);
					bool round_bit = false;
					r <<= 2;
					if (const storage_type tmp = (y << 2) + 1; r >= tmp)
					{
						r -= tmp;
						round_bit = true;
					}

					// Append the exponent field.
					x_exp = ((x_exp >> 1) + Bits::exponent_bias);
					y |= (static_cast<storage_type>(x_exp) << (Bits::fraction_length + 1));

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

					// Extract output
					support::FPBits<long double> out(0.0L);
					out.set_biased_exponent(static_cast<storage_type>(x_exp));
					out.set_implicit_bit(true);
					out.set_mantissa(y & (one - 1));

					return out.get_val();
				}
#endif
			} // namespace bit80

			template <typename T>
			constexpr std::enable_if_t<std::is_floating_point_v<T>, T> sqrt_calc_bits(support::FPBits<T> & bits)
			{
				using FPBits_t			   = support::FPBits<T>;
				using storage_type		   = typename FPBits_t::storage_type;
				constexpr storage_type one = storage_type(1) << FPBits_t::fraction_length;

				int x_exp			= bits.get_exponent();
				storage_type x_mant = bits.get_mantissa();

				// If we have denormal values, normalize it.
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

				storage_type y = one;
				storage_type r = x_mant - one;

				for (storage_type current_bit = one >> 1; current_bit; current_bit >>= 1)
				{
					r <<= 1;
					storage_type const tmp = (y << 1) + current_bit; // 2*y(n - 1) + 2^(-n-1)
					if (r >= tmp)
					{
						r -= tmp;
						y += current_bit;
					}
				}

				// We perform one more iteration to ensure that the result is correctly rounded.
				auto lsb	   = static_cast<bool>(y & 1); // Least significant bit
				bool round_bit = false;

				r <<= 2;

				if (storage_type const tmp = (y << 2) + 1; r >= tmp)
				{
					r -= tmp;
					round_bit = true;
				}

				// Remove the hidden bit and append the exponent field.
				x_exp = ((x_exp >> 1) + FPBits_t::exponent_bias);

				y = (y - one) | (static_cast<storage_type>(x_exp) << FPBits_t::fraction_length);

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
				if constexpr (Is80BitLongDouble_v<T>) // NOLINT
				{
					return bit80::sqrt_calc_80bits(x);
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

		namespace rt
		{
			namespace simd
			{
				// TODO: Change this to instead use SIMDe library for SIMD operations or implement our own generic simd functions.
				// SIMD version of the sqrt function.
				template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
				inline T sqrt_impl_simd(T num)
				{
#ifdef CCM_CONFIG_USE_RT_SIMD
#if defined(CCM_HAS_SIMD_SSE)
	#elif defined(CCM_HAS_SIMD_AVX)
					if constexpr (std::is_same_v<T, float>)
					{
						__m128 num_m		= _mm_set_ss(num);
						__m128 const sqrt_m = _mm_sqrt_ss(num_m);
						return _mm_cvtss_f32(sqrt_m);
					}
					if constexpr (std::is_same_v<T, double>)
					{
						__m128d num_m		 = _mm_set_sd(num);
						__m128d const sqrt_m = _mm_sqrt_sd(num_m, num_m);
						return _mm_cvtsd_f64(sqrt_m);
					}
		#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT64)
					if constexpr (std::is_same_v<T, long double>)
					{
						__m128d num_m		 = _mm_set_sd(static_cast<double>(num));
						__m128d const sqrt_m = _mm_sqrt_sd(num_m, num_m);
						return static_cast<long double>(_mm_cvtsd_f64(sqrt_m));
					}
		#else // If long double is not 64-bits, then we must use our generic sqrt function.
					return impl::sqrt_impl(num);
		#endif
	#else
					return impl::sqrt_impl(num);
	#endif
				}

				template <typename Integer, std::enable_if_t<!std::is_floating_point_v<Integer>, bool> = true>
				inline double sqrt_impl_simd(Integer num)
				{
					return sqrt_impl_simd(static_cast<double>(num));
				}
#endif
			} // namespace simd

			// Runtime version of the sqrt function.
			template <typename T>
			T sqrt_impl_rt(T num)
			{
#if CCM_HAS_BUILTIN(__builtin_sqrt) ||                                                                                                                         \
	defined(__builtin_sqrt) // If we have access to the builtin sqrt function, use it. Builtins will generally use hardware instructions.
				if constexpr (std::is_same_v<T, float>) { return __builtin_sqrtf(num); }
				if constexpr (std::is_same_v<T, double>) { return __builtin_sqrt(num); }
				if constexpr (std::is_same_v<T, long double>) { return __builtin_sqrtl(num); }
				return static_cast<T>(__builtin_sqrtl(num));
#elif defined(CCM_CONFIG_USE_RT_SIMD)
					return simd::sqrt_impl_simd(num);
#else
					return impl::sqrt_impl(num); // No builtin? Then handle it in software instead like normal.
#endif
			}
		} // namespace rt
	} // namespace internal

	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T internal_sqrt(T num)
	{
#if defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) // GCC 6.1+ has constexpr sqrt builtins.
		if constexpr (std::is_same_v<T, float>) { return __builtin_sqrtf(num); }
		if constexpr (std::is_same_v<T, double>) { return __builtin_sqrt(num); }
		if constexpr (std::is_same_v<T, long double>) { return __builtin_sqrtl(num); }
		return static_cast<T>(__builtin_sqrtl(num));
#else
			if constexpr (ccm::support::is_constant_evaluated()) { return internal::impl::sqrt_impl(num); }
			else { return internal::impl::sqrt_impl(num); }
			// else { return internal::rt::sqrt_impl_rt(num); }

#endif
	}

	template <typename Integer, std::enable_if_t<!std::is_floating_point_v<Integer>, bool> = true>
	constexpr double internal_sqrt(Integer num)
	{
		return internal_sqrt(static_cast<double>(num));
	}
} // namespace ccm::support::math
