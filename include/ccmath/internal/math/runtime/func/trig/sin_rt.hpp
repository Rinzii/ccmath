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

#include "ccmath/internal/math/generic/func/trig/sin_gen.hpp"
#include "ccmath/internal/math/runtime/func/detail/msvc_libm.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/math/runtime/func/svml_dispatch.hpp"
#include "ccmath/internal/math/runtime/simd/func/catalog.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T sin_rt(T num) noexcept
	{
#if defined(_MSC_VER) && !defined(__clang__)
		return detail::msvc_libm::sin_call(num);
#else
	#if CCM_HAS_BUILTIN(__builtin_sin) || defined(__builtin_sin)
		if constexpr (std::is_same_v<T, float>) { return __builtin_sinf(num); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_sin(num); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_sinl(num); }
		else { return static_cast<T>(__builtin_sinl(static_cast<long double>(num))); }
	#else
		const auto scalar = [](T value) { return gen::sin_gen(value); };
		#if defined(CCMATH_HAS_SIMD) && defined(CCMATH_HAS_SIMD_SVML) && !defined(_MSC_VER)
		return detail::unary_trig_svml_or_impl(
			num, [](auto v) { return intrin::sin(v); }, scalar);
		#else
		return simd_impl::unary_via_scalar_abi(num, scalar);
		#endif
	#endif
#endif
	}
} // namespace ccm::rt
