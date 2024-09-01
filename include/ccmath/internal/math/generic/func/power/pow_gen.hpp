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

#include "ccmath/internal/math/generic/func/power/pow_impl.hpp"
#include "ccmath/internal/math/generic/func/power/powf_impl.hpp"

#include <type_traits>

namespace ccm::gen
{
	template <typename T>
	constexpr T pow_gen(T base, T exp) noexcept
	{
		//if constexpr (std::is

		return 0;
	}

} // namespace ccm
