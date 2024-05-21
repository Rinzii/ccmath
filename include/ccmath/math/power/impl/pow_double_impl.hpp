/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

namespace ccm::internal
{
	namespace impl
	{

		constexpr double pow_double_impl(double base, double exp) noexcept
		{

			return 0;
		}

	} // namespace impl

	constexpr double pow_double(double base, double exp) noexcept
	{
		return impl::pow_double_impl(base, exp);
	}
} // namespace ccm::internal