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

#include "ccmath/internal/math/runtime/func/detail/msvc_libm.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/math/runtime/func/svml_dispatch.hpp"
#include "ccmath/internal/math/runtime/simd/func/catalog.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"
#include "ccmath/math/expo/impl/log_double_impl.hpp"
#include "ccmath/math/expo/impl/log_float_impl.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T log_rt(T num) noexcept
	{
#if defined(_MSC_VER) && !defined(__clang__)
		return detail::msvc_libm::log_call(num);
#else
	#if CCM_HAS_BUILTIN(__builtin_log) || defined(__builtin_log)
		if constexpr (std::is_same_v<T, float>) { return __builtin_logf(num); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_log(num); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_logl(num); }
		else { return static_cast<T>(__builtin_logl(static_cast<long double>(num))); }
	#else
		const auto scalar = [](T value) { return detail::dispatch_float_double(value, ccm::internal::log_float, ccm::internal::log_double); };
		#if defined(CCMATH_HAS_SIMD) && defined(CCMATH_HAS_SIMD_SVML) && !defined(_MSC_VER)
		return detail::unary_svml_or_impl(
			num, [](auto v) { return intrin::log(v); }, scalar);
		#else
		return simd_impl::unary_via_scalar_abi(num, scalar);
		#endif
	#endif
#endif
	}
} // namespace ccm::rt
