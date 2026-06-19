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

#include "ccmath/internal/math/generic/builtins/basic/fmod.hpp"
#include "ccmath/internal/math/runtime/func/detail/system_math.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/math/basic/impl/fmod_double_impl.hpp"
#include "ccmath/math/basic/impl/fmod_float_impl.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T fmod_rt(T x, T y) noexcept
	{
#if defined(CCM_CONFIG_SYSTEM_MATH)
		return detail::sys::fmod_call(x, y);
#else
		if constexpr (ccm::builtin::has_runtime_fmod<T>) { return ccm::builtin::fmod_rt(x, y); }
		else
		{
			// No builtin and no system math, so reuse the exact fdlibm bit-reduction. It is exact for
			// every magnitude, unlike the trunc formula the other no-builtin basic fallbacks share.
			if constexpr (std::is_same_v<T, float>) { return ccm::internal::fmod_float(x, y); }
			else
			{
				return static_cast<T>(ccm::internal::fmod_double(static_cast<double>(x), static_cast<double>(y)));
			}
		}
#endif
	}
} // namespace ccm::rt
