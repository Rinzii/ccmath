/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

namespace ccm::internal::impl
{
	// TODO: Implement this using fp_bits
	constexpr long double nan_ldouble_impl(const char * /*arg*/) noexcept
	{
		return 0.0;
	}
	static_assert(nan_ldouble_impl("") == 0.0, "temp");
} // namespace ccm::internal::impl