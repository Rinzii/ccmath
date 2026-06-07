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

// Rounding mode assumed during constant evaluation. The C++ constant evaluator always performs
// floating-point arithmetic in round-to-nearest, ties-to-even, so ccmath assumes the same by
// default. Users who want ccmath's constexpr results to follow a directed rounding mode can define
// CCM_CONSTEXPR_ROUNDING_MODE to FE_UPWARD, FE_DOWNWARD, or FE_TOWARDZERO before including ccmath.
#ifndef CCM_CONSTEXPR_ROUNDING_MODE
	#define CCM_CONSTEXPR_ROUNDING_MODE FE_TONEAREST
#endif

namespace ccm::support::fenv
{
	/**
	 * @brief The rounding mode ccmath assumes while a function is being constant-evaluated.
	 * @return CCM_CONSTEXPR_ROUNDING_MODE, which defaults to FE_TONEAREST.
	 */
	constexpr int constant_eval_rounding_mode() noexcept
	{
		return CCM_CONSTEXPR_ROUNDING_MODE;
	}

	namespace internal
	{
		inline bool rt_rounding_mode_is_round_up()
		{
			return std::fegetround() == FE_UPWARD;
		}

		inline bool rt_rounding_mode_is_round_down()
		{
			return std::fegetround() == FE_DOWNWARD;
		}

		inline bool rt_rounding_mode_is_round_to_nearest()
		{
			return std::fegetround() == FE_TONEAREST;
		}

		inline bool rt_rounding_mode_is_round_to_zero()
		{
			return std::fegetround() == FE_TOWARDZERO;
		}

		inline int rt_get_rounding_mode()
		{
			return std::fegetround();
		}
	} // namespace internal

	/**
	 * @brief Free-standing function that tests whether fegetround() == FE_UPWARD using common floating point observations.
	 * @return True if the rounding mode is set to FE_UPWARD, false otherwise.
	 */
	constexpr bool rounding_mode_is_round_up()
	{
		if (is_constant_evaluated()) { return constant_eval_rounding_mode() == FE_UPWARD; }
		return internal::rt_rounding_mode_is_round_up();
	}

	/**
	 * @brief Free-standing function that tests whether fegetround() == FE_DOWNWARD using common floating point observations.
	 * @return True if the rounding mode is set to FE_DOWNWARD, false otherwise.
	 */
	constexpr bool rounding_mode_is_round_down()
	{
		if (is_constant_evaluated()) { return constant_eval_rounding_mode() == FE_DOWNWARD; }
		return internal::rt_rounding_mode_is_round_down();
	}

	/**
	 * @brief Quick free-standing function that tests whether fegetround() == FE_TONEAREST using common floating point observations.
	 * @return True if the rounding mode is set to FE_TONEAREST, false otherwise.
	 */
	constexpr bool rounding_mode_is_round_to_nearest()
	{
		if (is_constant_evaluated()) { return constant_eval_rounding_mode() == FE_TONEAREST; }
		return internal::rt_rounding_mode_is_round_to_nearest();
	}

	/**
	 * @brief Free-standing function that tests whether fegetround() == FE_TOWARDZERO using common floating point observations.
	 * @return True if the rounding mode is set to FE_TOWARDZERO, false otherwise.
	 */
	constexpr bool rounding_mode_is_round_to_zero()
	{
		if (is_constant_evaluated()) { return constant_eval_rounding_mode() == FE_TOWARDZERO; }
		return internal::rt_rounding_mode_is_round_to_zero();
	}

	/**
	 * @brief Free-standing function that identifies the current rounding mode.
	 * @return The current rounding mode.
	 */
	constexpr int get_rounding_mode()
	{
		if (is_constant_evaluated()) { return constant_eval_rounding_mode(); }
		return internal::rt_get_rounding_mode();
	}
} // namespace ccm::support::fenv
