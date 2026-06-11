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

#include "ccmath/internal/math/generic/builtins/expo/log1p.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/math/expo/impl/log1p_impl.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T log1p_rt(T num) noexcept
	{
		if constexpr (ccm::builtin::has_runtime_log1p<T>) { return ccm::builtin::log1p_rt(num); }
		else
		{
			const auto scalar = [](T value) { return detail::dispatch_float_double(value, ccm::internal::log1p_float, ccm::internal::log1p_double); };
			return simd_impl::unary_via_scalar_abi(num, scalar);
		}
	}
} // namespace ccm::rt
