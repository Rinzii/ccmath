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

#include "ccmath/internal/math/generic/builtins/trig/atan2.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/math/trig/impl/inv_trig_impl.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T atan2_rt(T y, T x) noexcept
	{
		if constexpr (ccm::builtin::has_runtime_atan2<T>) { return ccm::builtin::atan2_rt(y, x); }
		else
		{
			const auto scalar = [](T yv, T xv)
			{
				return detail::dispatch_float_double(
					yv,
					[&](float yf) { return internal::impl::atan2_float(yf, static_cast<float>(xv)); },
					[&](double yd) { return internal::impl::atan2_double(yd, static_cast<double>(xv)); });
			};
			return simd_impl::binary_via_scalar_abi(y, x, scalar);
		}
	}
} // namespace ccm::rt
