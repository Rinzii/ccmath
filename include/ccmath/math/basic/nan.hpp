/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/basic/impl/nan_double_impl.hpp"
#include "ccmath/math/basic/impl/nan_float_impl.hpp"

namespace ccm
{
	/**
	 * @brief
	 * @param arg
	 * @return
	 */
	constexpr double nan(const char* arg) noexcept
	{
		return ccm::internal::impl::nan_double_impl(arg);
	}

	constexpr float nanf(const char* arg) noexcept
	{
		return ccm::internal::impl::nan_float_impl(arg);
	}

	constexpr long double nanl([[maybe_unused]] const char* arg) noexcept
	{
		// Currently we do not yet support long double for ccm::nan
		// Idk if we ever will, but for the time being if someone calls the function. Just return a quiet NaN.
		return std::numeric_limits<long double>::quiet_NaN();
	}
} // namespace ccm
