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

#include "ccmath/internal/math/generic/builtins/expo/log.hpp"
#include "ccmath/internal/math/runtime/func/expo/log_rt.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fp/directional_rounding_utils.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/expo/impl/log_double_impl.hpp"
#include "ccmath/math/expo/impl/log_float_impl.hpp"

#if defined(_MSC_VER) && !defined(__clang__)
	#include "ccmath/internal/predef/compiler_suppression/msvc_compiler_suppression.hpp"
CCM_DISABLE_MSVC_WARNING(4702)
#endif

namespace ccm
{
	/**
	 * @brief Computes the natural (base e) logarithm (lnx) of a number.
	 * @tparam T The type of the number.
	 * @param num A floating-point or integer value to find the natural logarithm of.
	 * @return If no errors occur, the natural (base-e) logarithm of num (ln(num) or loge(num)) is returned.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T log(const T num) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_log<T>) { return ccm::builtin::log(num); }
		else
		{
			// If the argument is 1, exact zero is returned.
			if (num == static_cast<T>(1))
			{
				if (ccm::support::is_constant_evaluated()) { return static_cast<T>(0); }
				return ccm::support::fp::signed_zero_for_current_mode<T>();
			}

			// If the argument is ±0, -∞ is returned.
			if (num == static_cast<T>(0))
			{
				ccm::support::fenv::set_errno_if_required(ERANGE);
				ccm::support::fenv::raise_except_if_required(FE_DIVBYZERO);
				return -std::numeric_limits<T>::infinity();
			}

			// If the argument is negative, -NaN is returned.
			if (num < static_cast<T>(0))
			{
				ccm::support::fenv::set_errno_if_required(EDOM);
				ccm::support::fenv::raise_except_if_required(FE_INVALID);
				return -std::numeric_limits<T>::quiet_NaN();
			}

			// If the argument is +∞, +∞ is returned.
			if (CCM_UNLIKELY(num == std::numeric_limits<T>::infinity())) { return std::numeric_limits<T>::infinity(); }

			// If the argument is NaN, NaN is returned.
			if (CCM_UNLIKELY(ccm::isnan(num))) { return std::numeric_limits<T>::quiet_NaN(); }

			if (!ccm::support::is_constant_evaluated()) { return ccm::rt::log_rt(num); }

			if constexpr (std::is_same_v<T, float>) { return internal::log_float(num); }
			if constexpr (std::is_same_v<T, double>) { return internal::log_double(num); }
			if constexpr (std::is_same_v<T, long double>) { return static_cast<long double>(internal::log_double(static_cast<double>(num))); }
			return static_cast<T>(internal::log_double(static_cast<double>(num)));
		}
	}

	/**
	 * @brief Computes the natural (base e) logarithm (lnx) of a number.
	 * @tparam Integer The type of the integer.
	 * @param num An integer value to find the natural logarithm of.
	 * @return If no errors occur, the natural (base-e) logarithm of num (ln(num) or loge(num)) is returned.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double log(const Integer num) noexcept
	{
		return ccm::log<double>(static_cast<double>(num));
	}

	/**
	 * @brief Computes the natural (base e) logarithm (lnx) of a number.
	 * @param num A floating-point value to find the natural logarithm of.
	 * @return If no errors occur, the natural (base-e) logarithm of num (ln(num) or loge(num)) is returned.
	 */
	constexpr float logf(const float num) noexcept
	{
		return ccm::log<float>(num);
	}

	/**
	 * @brief Computes the natural (base e) logarithm (lnx) of a number.
	 * @param num A floating-point value to find the natural logarithm of.
	 * @return If no errors occur, the natural (base-e) logarithm of num (ln(num) or loge(num)) is returned.
	 */
	constexpr long double logl(long double num) noexcept
	{
		return ccm::log<long double>(num);
	}
} // namespace ccm

#if defined(_MSC_VER) && !defined(__clang__)
CCM_RESTORE_MSVC_WARNING()
#endif
