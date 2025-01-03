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

#include "ccmath/math/expo/impl/log_double_impl.hpp"
#include "ccmath/internal/math/generic/func/expo/log_gen.hpp"
#include "ccmath/math/expo/impl/log_float_impl.hpp"
#include "ccmath/internal/math/generic/builtins/expo/log.hpp"


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
			// If the number is 1, return +0.
			if (num == static_cast<T>(1)) { return static_cast<T>(0); }

			// If the argument is ±0, -∞ is returned.
			if (num == static_cast<T>(0)) { return -std::numeric_limits<T>::infinity(); }

			// If the argument is negative, -NaN is returned.
			if (num < static_cast<T>(0)) { return -std::numeric_limits<T>::quiet_NaN(); }

			// If the argument is +∞, +∞ is returned.
			if (CCM_UNLIKELY(num == std::numeric_limits<T>::infinity())) { return std::numeric_limits<T>::infinity(); }

			// If the argument is NaN, NaN is returned.
			if (CCM_UNLIKELY(ccm::isnan(num))) { return std::numeric_limits<T>::quiet_NaN(); }

			// Select the correct implementation based on the type.
			if constexpr (std::is_same_v<T, float>) { return internal::log_float(num); }
			if constexpr (std::is_same_v<T, double>) { return internal::log_double(num); }
			if constexpr (std::is_same_v<T, long double>) { return static_cast<long double>(internal::log_double(static_cast<double>(num))); }
			return static_cast<T>(internal::log_double(num));
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
	constexpr double logl(const double num) noexcept
	{
		return ccm::log<double>(num);
	}
} // namespace ccm

#if defined(_MSC_VER) && !defined(__clang__)
CCM_RESTORE_MSVC_WARNING()
#endif
