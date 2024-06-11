/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

// turn on fenv access
// #pragma STDC FENV_ACCESS ON

#include "ccmath/internal/support/bit_mask.hpp"
#include "ccmath/internal/support/floating_point_traits.hpp"
#include "ccmath/internal/support/fp/directional_rounding_utils.hpp"
#include "ccmath/internal/support/fp_bits.hpp"
#include "ccmath/internal/support/math/internal_sqrt.hpp"
#include "ccmath/internal/support/unreachable.hpp"
#include "ccmath/internal/types/big_int.hpp"
#include "ccmath/math/basic/abs.hpp"
#include "ccmath/math/compare/isnan.hpp"

#include <limits>

namespace ccm::internal::impl
{
	template <typename T>
	constexpr T pow_exp_helper(T base, T exp) noexcept
	{
		return 0;
	}

	template <typename T>
	constexpr T pow_log_helper(T base, T exp) noexcept
	{
		return 0;
	}

	template <typename T>
	constexpr T pow_round_even(T x) noexcept
	{
		return ccm::support::fp::directional_round<double>(x, FE_TONEAREST);
	}

	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr auto convert_fp_to_uint(T x) noexcept
	{
		using FPbits_t	= support::FPBits<T>;
		using Storage_t = typename FPbits_t::storage_type;
		FPbits_t x_bits(x);
		Storage_t x_abs = x_bits.abs().uintval();
		return x_abs;
	}

	template <typename T>
	constexpr bool pow_impl_is_integer(T x) noexcept
	{
		return ccm::support::fp::directional_round<double>(x, FE_TONEAREST) == x;
	}

	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr bool is_odd_integer(T x)
	{
		using FPBits_t				 = typename support::FPBits<T>;
		using Storage_t				 = typename FPBits_t::storage_type;
		using SignedStorage_t		 = support::float_signed_bits_t<T>;
		auto x_u					 = support::bit_cast<Storage_t>(x);
		auto x_e					 = static_cast<SignedStorage_t>((x_u & FPBits_t::exponent_mask) >> FPBits_t::fraction_length);
		auto lsb					 = support::countr_zero(x_u | FPBits_t::exponent_mask);
		constexpr auto UNIT_EXPONENT = FPBits_t::exponent_bias + static_cast<SignedStorage_t>(FPBits_t::fraction_length);
		return (x_e + lsb == UNIT_EXPONENT);
	}

	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr bool is_integer(T x)
	{
		using FPBits_t							= typename support::FPBits<T>;
		using Storage_t							= typename FPBits_t::storage_type;
		using SignedStorage_t					= support::float_signed_bits_t<T>;
		Storage_t x_u							= support::bit_cast<Storage_t>(x);
		auto x_e								= static_cast<SignedStorage_t>((x_u & FPBits_t::exponent_mask) >> FPBits_t::fraction_length);
		SignedStorage_t lsb						= support::countr_zero(x_u | FPBits_t::exponent_mask);
		constexpr SignedStorage_t UNIT_EXPONENT = FPBits_t::exponent_bias + static_cast<SignedStorage_t>(FPBits_t::fraction_length);
		return (x_e + lsb >= UNIT_EXPONENT);
	}

	template <typename T>
	// NOLINTNEXTLINE(readability-function-cognitive-complexity) - Has a naturally high complexity. To avoid fracturing, we allow this.
	constexpr std::enable_if_t<std::is_floating_point_v<T>, T> pow_impl_handle_special_cases(support::FPBits<T> & x_bits, support::FPBits<T> & y_bits, T & x,
																							 T & y) noexcept
	{

		if (x_bits.is_nan())
		{
			// IEEE 754-2019: pow(x, ±0) = 1 if x is not a signaling NaN
			if (y == 0.0 && !x_bits.is_signaling_nan()) { return 1.0; }

			// For GCC based compilers, they do not account for signaling nan and just return 1.0
			// For clang, it will return 1.0 if the optimization flag is set.
			// AppleClang always returns 1 no matter the optimization level.
#if (defined(__GNUC__) && !defined(__clang__)) || (defined(__clang__) && defined(__OPTIMIZE__)) || (defined(__clang__) && defined(__APPLE__)) ||               \
	(defined(__clang__) && defined(CCM_CONFIG_AGGRESSIVELY_OPTIMIZE))
			if (y == 0.0 && x_bits.is_signaling_nan()) { return 1.0; }
#endif

			return x + x;
		}

		if (y_bits.is_nan())
		{
			// IEEE 754-2019: pow(1,y) = 1 for any y (even a quiet NaN)
			if (x == 1.0 && !y_bits.is_signaling_nan()) { return 1.0; }

			// For GCC based compilers, they do not account for signaling nan and just return 1.0
			// For clang, it will return 1.0 if the optimization flag is set.
			// AppleClang always returns 1 no matter the optimization level.
#if (defined(__GNUC__) && !defined(__clang__)) || (defined(__clang__) && defined(__OPTIMIZE__)) || (defined(__clang__) && defined(__APPLE__)) ||               \
	(defined(__clang__) && defined(CCM_CONFIG_AGGRESSIVELY_OPTIMIZE))
			if (x == 1.0 && y_bits.is_signaling_nan()) { return 1.0; }
#endif

			return y + y;
		}

		// Handle the special case where x is a positive infinity and y is not negative
		// x = +inf
		if (x_bits.is_inf() && !x_bits.is_neg())
		{
			if (y == 0.0) { return 1.0; }

			if (y < 0.0) { return 0.0; }

			if (y > 0.0) { return std::numeric_limits<T>::infinity(); }
		}

		// Handle the special case where x is a negative infinity
		// x = -inf
		if (x_bits.is_inf() && x_bits.is_neg())
		{
			// If y is an odd integer
			if (pow_impl_is_integer(y) && !pow_impl_is_integer(y * std::numeric_limits<T>::round_error()))
			{
				// y is a negative odd integer
				if (y < 0.0) { return -0.0; }

				// y is a positive odd integer
				return -std::numeric_limits<T>::infinity();
			}

			// y is a negative even integer or a negative non-integer
			if (y < 0.0) { return 0.0; }

			// y is a positive even integer or a positive non-integer
			if (y > 0.0) { return std::numeric_limits<T>::infinity(); }
		}

		// y = +inf
		if (y_bits.is_inf() && !y_bits.is_neg())
		{
			if (x == 0.0) { return 0.0; }

			if (x == -1.0 || x == 1.0) { return 1.0; }

			if (-1.0 < x && x < 1.0) { return 0.0; }

			if (x < -1.0 || 1.0 < x) { return std::numeric_limits<double>::infinity(); }
		}

		// y = -inf
		if (y_bits.is_inf() && y_bits.is_neg())
		{
			if (x == 0.0) { return std::numeric_limits<T>::infinity(); }

			if (x == -1.0 || x == 1.0) { return 1.0; }

			if (-1.0 < x && x < 1.0) { return std::numeric_limits<T>::infinity(); }

			if (x < -1.0 || 1.0 < x) { return 0.0; }
		}

		support::unreachable(); // Something went horribly wrong if we reach this point.
		return 0.0;				// This should never be reached.
	}

	template <typename T>
	constexpr std::enable_if_t<std::is_floating_point_v<T>, T> pow_impl_handle_zero_or_less(support::FPBits<T> & x_bits, T & x, T & y,
																							T & current_sign_of_result) noexcept
	{
		const bool is_x_zero	   = x_bits.is_zero();
		const bool is_y_an_integer = pow_impl_is_integer(y);
		// x = +0.0
		if (is_x_zero && !x_bits.is_neg())
		{
			// y is an odd integer
			if (is_y_an_integer && !pow_impl_is_integer(y * std::numeric_limits<T>::round_error()))
			{
				// y is a negative odd integer
				if (y < 0.0)
				{
					ccm::support::fenv::raise_except_if_required(FE_DIVBYZERO);
					return std::numeric_limits<T>::infinity();
				}

				// y is a positive odd integer
				return 0.0;
			}

			// y is positive even integer or a positive non-integer
			if (y > 0.0) { return 0.0; }

			// y is a negative, even, and finite integer or non-integer
			ccm::support::fenv::raise_except_if_required(FE_DIVBYZERO);
			return std::numeric_limits<T>::infinity();
		}

		// x = -0.0
		if (is_x_zero)
		{
			// y is an odd integer
			if (is_y_an_integer && !pow_impl_is_integer(y * std::numeric_limits<T>::round_error()))
			{
				// y is a negative odd integer
				if (y < 0.0)
				{
					ccm::support::fenv::raise_except_if_required(FE_DIVBYZERO);
					return -std::numeric_limits<T>::infinity();
				}

				// y is a positive odd integer
				return -0.0;
			}

			// y is a positive even integer or a positive non-integer
			if (y > 0.0) { return 0.0; }

			// y is a negative, even, and finite integer or non-integer
			ccm::support::fenv::raise_except_if_required(FE_DIVBYZERO);
			return -std::numeric_limits<T>::infinity();
		}

		if (!is_y_an_integer)
		{
			ccm::support::fenv::raise_except_if_required(FE_INVALID);
			return std::numeric_limits<T>::quiet_NaN();
		}

		std::array<T, 2> copy_sign = {1.0, -1.0};

		// set sign to 1 for even y and -1 for odd y
		int y_parity = ccm::abs<T>(y) >= support::floating_point_traits<T>::max_safe_integer ? 0 : static_cast<support::float_signed_bits_t<T>>(y) & 0x1;
		current_sign_of_result = copy_sign.at(y_parity);

		// Set x to the absolute value of x for the remainder of the function
		x = -x;

		// Indicate that we did not get a case that could've been handled, so we just set x above
		// then signal this by returning a max value of our type which this function would never return.
		// We have to do this as we are modifying x at this point but don't yet intend to return the full function.
		return std::numeric_limits<T>::min();
	}

	// make sure TStorage is of the type support::FPBits<T>::storage_type
	template <typename T, typename TStorage, std::enable_if_t<std::is_same_v<TStorage, typename support::FPBits<T>::storage_type>, bool> = true>
	constexpr std::enable_if_t<std::is_floating_point_v<T>, T> pow_check_over_under_flow(support::FPBits<T> & x_bits, support::FPBits<T> & y_bits,
																						 TStorage & x_abs, TStorage & y_abs) noexcept
	{

		return 0;
	}

	template <typename T>
	constexpr std::enable_if_t<std::is_floating_point_v<T>, T> pow_impl(T x, T y) noexcept
	{
		support::FPBits<T> x_bits(x);
		support::FPBits<T> y_bits(y);
		double sign_of_result = 1.0;

		// Handle edge cases when x or y is a non-finite value
		if (CCM_UNLIKELY(!x_bits.is_finite() || !y_bits.is_finite())) { return pow_impl_handle_special_cases<T>(x_bits, y_bits, x, y); }

		// Handle edge cases when x is zero or lees than zero
		if (x <= 0.0)
		{
			const auto handling_zero_or_less_result = pow_impl_handle_zero_or_less<T>(x_bits, x, y, sign_of_result);
			if (handling_zero_or_less_result != std::numeric_limits<T>::min()) { return handling_zero_or_less_result; }
		}
		// Begin the actual calculation

		return 0;
	}

} // namespace ccm::internal::impl
