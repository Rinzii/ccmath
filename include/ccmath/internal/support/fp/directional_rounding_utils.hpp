/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/fp_bits.hpp"

#include <cfenv>
#include <cstdint>
#include <type_traits>

namespace ccm::support::fp
{

	// NOLINTNEXTLINE
	enum class rounding_mode
	{
		eFE_TONEAREST  = FE_TONEAREST,
		eFE_DOWNWARD   = FE_DOWNWARD,
		eFE_UPWARD	   = FE_UPWARD,
		eFE_TOWARDZERO = FE_TOWARDZERO,
		eFE_TONEARESTFROMZERO,
	};

	template <typename T>
	// NOLINTNEXTLINE(readability-function-cognitive-complexity) - Has a natural complexity of around 60+. To avoid fracturing, we allow this.
	constexpr std::enable_if_t<std::is_floating_point_v<T>, T> directional_round(T val, int desired_rounding_mode) noexcept
	{
		using FPBits_t	= ccm::support::FPBits<T>;
		using Storage_t = typename FPBits_t::storage_type;

		FPBits_t bits(val);

		// Check if the value is NaN, infinity, or zero. If so, return the value.
		if (bits.is_inf_or_nan() || bits.is_zero()) { return val; }

		bool is_neg	 = bits.is_neg();
		int exponent = bits.get_exponent();

		// If our exponent is greater than the most negative possible mantissa, then x is in fact already an integral.
		if (exponent >= static_cast<int>(FPBits_t::fraction_length)) { return val; }

		auto provided_round_mode = static_cast<rounding_mode>(desired_rounding_mode);

		if (exponent <= -1)
		{
			switch (provided_round_mode)
			{
			case rounding_mode::eFE_DOWNWARD: return is_neg ? T(-1.0) : T(0.0);
			case rounding_mode::eFE_UPWARD: return is_neg ? T(-0.0) : T(1.0);
			case rounding_mode::eFE_TOWARDZERO: return is_neg ? T(-0.0) : T(0.0);
			case rounding_mode::eFE_TONEARESTFROMZERO:
				if (exponent < -1)
				{
					return T(is_neg ? -0.0 : 0.0); // abs(x) < 0.5
				}
				return T(is_neg ? -1.0 : 1.0); // abs(x) >= 0.5
			case rounding_mode::eFE_TONEAREST: [[fallthrough]];
			default:
				if (exponent <= -2 || bits.get_mantissa() == 0)
				{
					// abs(x) <= 0.5
					return T(is_neg ? -0.0 : 0.0);
				}
				// abs(x) > 0.5
				return T(is_neg ? -1.0 : 1.0);
			}
		}

		auto trimming_length = static_cast<std::uint32_t>(FPBits_t::fraction_length - exponent);
		FPBits_t new_bits			  = bits;
		new_bits.set_mantissa((bits.get_mantissa() >> trimming_length) << trimming_length);
		T truncated_value = new_bits.get_val();

		// If the truncated value is the same as the original value, then the value is already integral.
		if (truncated_value == val) { return val; }

		// Calculate the trimmed value and the half-value for rounding decisions.
		Storage_t trimmed_value = bits.get_mantissa() & ((Storage_t(1) << trimming_length) - 1);
		Storage_t half_value	= Storage_t(1) << (trimming_length - 1);

		// If the event that the exponent is 0, the trimmed_length variable will be equal to the width of the mantissa and truncated_value_is_odd
		// will not be the correct value. In such instances, we handle this special case below inside the switch statement.
		Storage_t truncated_value_is_odd = new_bits.get_mantissa() & (Storage_t(1) << trimming_length);

		switch (provided_round_mode)
		{
		case rounding_mode::eFE_DOWNWARD: return is_neg ? truncated_value - T(1.0) : truncated_value;
		case rounding_mode::eFE_UPWARD: return is_neg ? truncated_value : truncated_value + T(1.0);
		case rounding_mode::eFE_TOWARDZERO: return truncated_value;
		case rounding_mode::eFE_TONEARESTFROMZERO:
			if (trimmed_value >= half_value) { return is_neg ? truncated_value - T(1.0) : truncated_value + T(1.0); }
			return truncated_value;
		case rounding_mode::eFE_TONEAREST: [[fallthrough]]; // The Default case is round to the nearest
		default:
			if (trimmed_value > half_value) { return is_neg ? truncated_value - T(1.0) : truncated_value + T(1.0); }
			else if (trimmed_value == half_value)
			{
				if (exponent == 0) { return is_neg ? T(-2.0) : T(2.0); }
				if (truncated_value_is_odd) { return is_neg ? truncated_value - T(1.0) : truncated_value + T(1.0); }
				return truncated_value;
			}
			else { return truncated_value; }
		}
	}
} // namespace ccm::support::fp