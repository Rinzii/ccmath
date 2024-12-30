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
		constexpr double pow_impl([[maybe_unused]] double base, [[maybe_unused]] double exp) noexcept
		{
			// TODO: Implement this.
			return 0;
		}
	} // namespace internal::impl

	constexpr double pow_impl(double base, double exp) noexcept
	{
		return internal::impl::pow_impl(base, exp);
	}

} // namespace ccm::gen::impl
