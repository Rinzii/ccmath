/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <limits>
#include "ccmath/detail/compare/isnan.hpp"
#include "ccmath/detail/compare/isinf.hpp"

namespace ccm
{
	namespace
	{
		namespace impl
		{
			template <typename T>
			inline constexpr bool isfinite_impl(T num)
			{
				return (!ccm::isnan(num)) && (!ccm::isinf(num));
			}
		}
	}

	template <typename T>
	inline constexpr bool isfinite(T num)
	{
		return impl::isfinite_impl(num);
	}

} // namespace ccm
