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

#include "ccmath/internal/math/generic/builtins/expo/expm1.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/math/expo/impl/expm1_impl.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T expm1_rt(T num) noexcept
	{
		// TODO(IanP): add the FE_TONEAREST guard the other expo runtime headers use once the generic
		// expm1 double kernel meets the accuracy contract. It is not accurate enough yet, so routing
		// directed rounding to it would regress expm1 below the libm builtin used here in every mode.
		if constexpr (ccm::builtin::has_runtime_expm1<T>) { return ccm::builtin::expm1_rt(num); }
		else
		{
			const auto scalar = [](T value) { return detail::dispatch_float_double(value, ccm::internal::expm1_float, ccm::internal::expm1_double); };
			return simd_impl::unary_via_scalar_abi(num, scalar);
		}
	}
} // namespace ccm::rt
