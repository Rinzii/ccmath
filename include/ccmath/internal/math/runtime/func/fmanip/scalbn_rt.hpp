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

#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"
#include "ccmath/math/fmanip/impl/scalbn_double_impl.hpp"
#include "ccmath/math/fmanip/impl/scalbn_float_impl.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T scalbn_rt(T num, int exp) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_scalbn) || defined(__builtin_scalbn)
		if constexpr (std::is_same_v<T, float>) { return __builtin_scalbnf(num, exp); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_scalbn(num, exp); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_scalbnl(num, exp); }
		else
		{
			return static_cast<T>(__builtin_scalbnl(static_cast<long double>(num), exp));
		}
#else
		return detail::dispatch_float_double(
			num, [&](float value) { return internal::scalbn_float(value, exp); }, [&](double value) { return internal::scalbn_double(value, exp); });
#endif
	}
} // namespace ccm::rt
