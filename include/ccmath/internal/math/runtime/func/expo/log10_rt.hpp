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

#include "ccmath/internal/math/generic/builtins/expo/log10.hpp"
#include "ccmath/internal/math/runtime/func/detail/msvc_libm.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/math/runtime/func/svml_dispatch.hpp"
#include "ccmath/internal/math/runtime/simd/func/catalog.hpp"
#include "ccmath/math/expo/impl/log10_impl.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T log10_rt(T num) noexcept
	{
#if defined(_MSC_VER) && !defined(__clang__)
		return detail::msvc_libm::log10_call(num);
#else
		if constexpr (ccm::builtin::has_runtime_log10<T>) { return ccm::builtin::log10_rt(num); }
		else
		{
			const auto scalar = [](T value) { return detail::dispatch_float_double(value, ccm::internal::log10_float, ccm::internal::log10_double); };
	#if defined(CCMATH_HAS_SIMD) && defined(CCMATH_HAS_SIMD_SVML) && !defined(_MSC_VER)
			return detail::unary_svml_or_impl(num, [](auto v) { return intrin::log10(v); }, scalar);
	#else
			return simd_impl::unary_via_scalar_abi(num, scalar);
	#endif
		}
#endif
	}
} // namespace ccm::rt
