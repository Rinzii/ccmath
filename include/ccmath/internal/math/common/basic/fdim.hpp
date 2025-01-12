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

#include "ccmath/internal/math/generic/func/basic/fdim_gen.hpp"

namespace ccm::func
{

	/**
	 * @internal
	 * Internal implementation that switches between compile time and runtime implementations.
	 */
	template <typename T>
	constexpr auto fdim(T x, T y) -> std::enable_if_t<std::is_floating_point_v<T>, T>
	{
		return gen::fdim(x, y);
	}

} // namespace ccm::func
