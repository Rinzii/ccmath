/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/is_constant_evaluated.hpp"

#include <cfenv>
#include <limits>

namespace ccm::support::fenv
{
	namespace internal
	{
		inline bool rt_rounding_mode_is_round_up()
		{
			volatile constexpr float x = 0x1.0p-25F;
			return (1.0F + x != 1.0F);
		}

		inline bool rt_rounding_mode_is_round_down()
		{
			volatile constexpr float x = 0x1.0p-25F;
			return (-1.0F - x != -1.0F);
		}

		inline bool rt_rounding_mode_is_round_to_nearest()
		{
			static volatile constexpr float x = 0x1.0p-24F;
			float const y					  = x;
			return (1.5F + y == 1.5F - y);
		}

		inline bool rt_rounding_mode_is_round_to_zero()
		{

			static volatile float const x = 0x1.0p-24F;
			const float y			= x;
			return ((0x1.000002p0F + y) + (-1.0F - y) == 0x1.0p-23F);
		}

		inline int rt_get_rounding_mode()
		{
			static volatile float const x = 0x1.0p-24F;
			float const y			= x;
			float const z			= (0x1.000002p0F + y) + (-1.0F - y);

			if (z == 0.0F) { return FE_DOWNWARD; }
			if (z == 0x1.0p-23F) { return FE_TOWARDZERO; }
			return (2.0F + y == 2.0F) ? FE_TONEAREST : FE_UPWARD;
		}
	} // namespace internal

	/**
	 * @brief Free-standing function that tests whether fegetround() == FE_UPWARD using common floating point observations.
	 * @return True if the rounding mode is set to FE_UPWARD, false otherwise.
	 */
	constexpr bool rounding_mode_is_round_up()
	{
		if (is_constant_evaluated()) { return std::numeric_limits<float>::round_style == std::float_round_style::round_toward_infinity; }
		return internal::rt_rounding_mode_is_round_up();
	}

	/**
	 * @brief Free-standing function that tests whether fegetround() == FE_DOWNWARD using common floating point observations.
	 * @return True if the rounding mode is set to FE_DOWNWARD, false otherwise.
	 */
	constexpr bool rounding_mode_is_round_down()
	{
		if (is_constant_evaluated()) { return std::numeric_limits<float>::round_style == std::float_round_style::round_toward_neg_infinity; }
		return internal::rt_rounding_mode_is_round_down();
	}

	/**
	 * @brief Quick free-standing function that tests whether fegetround() == FE_TONEAREST using common floating point observations.
	 * @return True if the rounding mode is set to FE_TONEAREST, false otherwise.
	 */
	constexpr bool rounding_mode_is_round_to_nearest()
	{
		if (is_constant_evaluated()) { return std::numeric_limits<float>::round_style == std::float_round_style::round_to_nearest; }
		return internal::rt_rounding_mode_is_round_to_nearest();
	}

	/**
	 * @brief Free-standing function that tests whether fegetround() == FE_TOWARDZERO using common floating point observations.
	 * @return True if the rounding mode is set to FE_TOWARDZERO, false otherwise.
	 */
	constexpr bool rounding_mode_is_round_to_zero()
	{
		if (is_constant_evaluated()) { return std::numeric_limits<float>::round_style == std::float_round_style::round_toward_zero; }
		return internal::rt_rounding_mode_is_round_to_zero();
	}

	/**
	 * @brief Free-standing function that identifies the current rounding mode.
	 * @return The current rounding mode.
	 */
	constexpr int get_rounding_mode()
	{
		if (is_constant_evaluated())
		{
			switch (std::numeric_limits<float>::round_style)
			{
			case std::float_round_style::round_toward_infinity: return FE_UPWARD;
			case std::float_round_style::round_toward_zero: return FE_TOWARDZERO;
			case std::float_round_style::round_toward_neg_infinity: return FE_DOWNWARD;
			default: return FE_TONEAREST; // Default rounding mode.
			}
		}
		else { return internal::rt_get_rounding_mode(); }
	}
} // namespace ccm::support::fenv
