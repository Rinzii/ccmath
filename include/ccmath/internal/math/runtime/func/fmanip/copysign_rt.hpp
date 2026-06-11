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

#include "ccmath/internal/math/generic/builtins/fmanip/copysign.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/compare/signbit.hpp"

#include <limits>
#include <type_traits>

namespace ccm::rt
{
	namespace detail
	{
		template <typename T>
		[[nodiscard]] inline T copysign_scalar(T mag, T sgn) noexcept
		{
			if (ccm::isnan(mag) || ccm::isnan(sgn))
			{
				if (ccm::signbit(sgn)) { return -std::numeric_limits<T>::quiet_NaN(); }
				return std::numeric_limits<T>::quiet_NaN();
			}

			const T sign_bit = ccm::signbit(sgn) ? static_cast<T>(-1) : static_cast<T>(1);
			return ccm::support::fp::FPBits<T>(mag).abs().get_val() * sign_bit;
		}
	} // namespace detail

	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T copysign_rt(T x, T y) noexcept
	{
		if constexpr (ccm::builtin::has_runtime_copysign<T>) { return ccm::builtin::copysign_rt(x, y); }
		else
		{
			return detail::copysign_scalar(x, y);
		}
	}
} // namespace ccm::rt
