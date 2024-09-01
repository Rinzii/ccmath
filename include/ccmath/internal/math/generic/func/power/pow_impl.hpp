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

#include <type_traits>

namespace ccm::gen::impl
{
	namespace internal::impl
	{
		template <typename T>
		constexpr T pow_impl(T base, T exp) noexcept
		{
			return 0;
		}
	}

	template <typename T>
	constexpr T pow_impl(T base, T exp) noexcept
	{
		return internal::impl::pow_impl(base, exp);
	}

} // namespace ccm
