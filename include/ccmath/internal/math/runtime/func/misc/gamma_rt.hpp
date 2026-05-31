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

#include "ccmath/internal/math/generic/builtins/misc/tgamma.hpp"
#include "ccmath/internal/math/runtime/func/detail/msvc_libm.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/math/misc/impl/gamma_impl.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T gamma_rt(T num) noexcept
	{
#if defined(_MSC_VER) && !defined(__clang__)
		return detail::msvc_libm::gamma_call(num);
#else
		if constexpr (ccm::builtin::has_runtime_gamma<T>) { return ccm::builtin::runtime_gamma(num); }
		else
		{
			const auto scalar = [](T value) { return detail::dispatch_float_double(value, ccm::internal::gamma_float, ccm::internal::gamma_double); };
			return simd_impl::unary_via_scalar_abi(num, scalar);
		}
#endif
	}
} // namespace ccm::rt
