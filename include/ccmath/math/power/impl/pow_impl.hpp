/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/floating_point_traits.hpp"
#include "ccmath/internal/support/fp_bits.hpp"
#include "ccmath/internal/types/big_int.hpp"

namespace ccm::internal::impl
{
	constexpr double minimax(double position, int depth, bool minimizingPlayer)
	{
		if (depth == 0) return 0;
	}

	template <typename T>
	constexpr T pow_exp_helper(T base, T exp) noexcept
	{
		return 0;
	}

	template <typename T>
	constexpr T pow_log_helper(T base, T exp) noexcept
	{
		return 0;
	}

	template <typename T>
	constexpr T pow_check_overflow(support::FPBits<T> & xbits, support::FPBits<T> & ybits) noexcept
	{
		using FloatStorageType = typename support::FPBits<T>::StorageType;
		FloatStorageType x_u = xbits.uintval();
		FloatStorageType x_abs = xbits.abs().uintval();
		FloatStorageType y_u = ybits.uintval();
		FloatStorageType y_abs = ybits.abs().uintval();

		if (CCM_UNLIKELY((y_abs & 0x0007'ffff) == 0) || (y_abs > 0x4f170000))
















		return 0;
	}

	template <typename T>
	constexpr T pow_impl(T base, T exp) noexcept
	{

		return 0;
	}

} // namespace ccm::internal::impl
