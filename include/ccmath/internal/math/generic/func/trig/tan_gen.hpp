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

#include "ccmath/internal/math/generic/func/trig/cos_gen.hpp"
#include "ccmath/internal/math/generic/func/trig/sin_gen.hpp"

#include <limits>
#include <type_traits>

namespace ccm::gen
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T tan_gen(T num) noexcept
	{
		const T s = sin_gen(num);
		const T c = cos_gen(num);

		if (c == static_cast<T>(0)) { return std::numeric_limits<T>::quiet_NaN(); }

		return s / c;
	}
} // namespace ccm::gen
