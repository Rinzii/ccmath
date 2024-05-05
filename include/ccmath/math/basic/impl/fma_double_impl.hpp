/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present cmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
*/

#pragma once

namespace ccm::internal::impl
{
	constexpr double fma_double_impl(double x, double y, double z) noexcept
	{
		return x * y + z;
	}

} // namespace ccm::internal::impl