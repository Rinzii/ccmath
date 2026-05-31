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

#include "ccmath/internal/math/generic/builtins/expo/exp2.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/math/expo/impl/exp2_double_impl.hpp"
#include "ccmath/math/expo/impl/exp2_float_impl.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T exp2_rt(T num) noexcept
	{
		if constexpr (ccm::builtin::has_runtime_exp2<T>) { return ccm::builtin::runtime_exp2(num); }
		else
		{
			const auto scalar = [](T value) {
				return detail::dispatch_float_double(value, ccm::internal::exp2_float, ccm::internal::exp2_double);
			};
			return simd_impl::unary_via_scalar_abi(num, scalar);
		}
	}
} // namespace ccm::rt
