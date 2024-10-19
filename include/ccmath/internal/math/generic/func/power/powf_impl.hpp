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
		constexpr float powf_impl(float base, float exp) noexcept
		{
			// TODO: Implement this.
			return 0;
		}
	}

	constexpr float powf_impl(float base, float exp) noexcept
	{
		return internal::impl::powf_impl(base, exp);
	}

} // namespace ccm::gen::impl
