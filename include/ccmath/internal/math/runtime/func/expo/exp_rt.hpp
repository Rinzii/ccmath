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

#include "ccmath/internal/math/generic/builtins/expo/exp.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/math/runtime/func/svml_dispatch.hpp"
#include "ccmath/internal/math/runtime/simd/func/catalog.hpp"
#include "ccmath/math/expo/impl/exp_double_impl.hpp"
#include "ccmath/math/expo/impl/exp_float_impl.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true> [[nodiscard]] inline T exp_rt(T num) noexcept
	{
		const auto scalar = [](T value) {
			return detail::dispatch_float_double(value, ccm::internal::impl::exp_float_impl, ccm::internal::impl::exp_double_impl);
		};
		if constexpr (ccm::builtin::has_runtime_exp<T>)
		{
			// The runtime builtin lowers to libm, which is not correctly rounded outside round to
			// nearest. Outside FE_TONEAREST use the generic kernel instead.
			if (CCM_UNLIKELY(ccm::support::fenv::get_rounding_mode() != FE_TONEAREST))
			{
				return scalar(num);
			}
			return ccm::builtin::exp_rt(num);
		} else
		{
#if defined(CCMATH_HAS_SIMD) && defined(CCMATH_HAS_SIMD_SVML) && !defined(_MSC_VER)
			return detail::unary_svml_or_impl(num, [](auto v) { return intrin::exp(v); }, scalar);
#else
			return simd_impl::unary_via_scalar_abi(num, scalar);
#endif
		}
	}
} // namespace ccm::rt
