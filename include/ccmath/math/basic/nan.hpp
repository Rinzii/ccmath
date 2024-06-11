/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/basic/impl/nan_ldouble_impl.hpp"
#include "ccmath/math/basic/impl/nan_double_impl.hpp"
#include "ccmath/math/basic/impl/nan_float_impl.hpp"

namespace ccm
{
	/**
	 * @brief not-a-number (NaN) function
	 * @param arg Narrow character string identifying the contents of a NaN
	 * @return The quiet NaN value that corresponds to the identifying string arg or zero if the implementation does not support quiet NaNs.
	 */
	constexpr double nan(const char * arg) noexcept
	{
		return internal::impl::nan_double_impl(arg);
	}

	/**
	 * @brief not-a-number (NaN) function
	 * @param arg Narrow character string identifying the contents of a NaN
	 * @return The quiet NaN value that corresponds to the identifying string arg or zero if the implementation does not support quiet NaNs.
	 */
	constexpr float nanf(const char * arg) noexcept
	{
		return internal::impl::nan_float_impl(arg);
	}

	/**
	 * @brief not-a-number (NaN) function
	 * @return The quiet NaN value that corresponds to the identifying string arg or zero if the implementation does not support quiet NaNs.
	 * @note This function is not yet supported for long double and can only return either 0.0 or a quiet NaN based on if quiet NaNs are supported.
	 */
	constexpr long double nanl(const char * arg) noexcept
	{
#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT64)
		return static_cast<long double>(ccm::nan(arg));
#else
		return internal::impl::nan_ldouble_impl(arg);
#endif
	}
} // namespace ccm
