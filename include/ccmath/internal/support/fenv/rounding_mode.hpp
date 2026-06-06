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

#include "ccmath/internal/support/is_constant_evaluated.hpp"

#include <cfenv>
#include <limits>

namespace ccm::support::fenv
{
	namespace internal
	{
		inline bool rt_rounding_mode_is_round_up()
		{ return std::fegetround() == FE_UPWARD; }

		inline bool rt_rounding_mode_is_round_down()
		{ return std::fegetround() == FE_DOWNWARD; }

		inline bool rt_rounding_mode_is_round_to_nearest()
		{ return std::fegetround() == FE_TONEAREST; }

		inline bool rt_rounding_mode_is_round_to_zero()
		{ return std::fegetround() == FE_TOWARDZERO; }

		inline int rt_get_rounding_mode()
		{ return std::fegetround(); }
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
		return internal::rt_get_rounding_mode();
	}
} // namespace ccm::support::fenv
