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

#include "ccmath/internal/math/generic/builtins/fmanip/scalbn.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/math/fmanip/impl/scalbn_double_impl.hpp"
#include "ccmath/math/fmanip/impl/scalbn_float_impl.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T scalbn_rt(T num, int exp) noexcept
	{
		if constexpr (ccm::builtin::has_runtime_scalbn<T>) { return ccm::builtin::scalbn_rt(num, exp); }
		else
		{
			return detail::dispatch_float_double(
				num, [&](float value) { return internal::scalbn_float(value, exp); }, [&](double value) { return internal::scalbn_double(value, exp); });
		}
	}
} // namespace ccm::rt
