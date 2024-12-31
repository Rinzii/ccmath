/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// Portions of this code was borrowed from the LLVM Project,
// under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <type_traits>

#include "ccmath/internal/math/generic/func/basic/max_gen.hpp"
#include "ccmath/internal/math/generic/func/basic/min_gen.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fenv/rounding_mode.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/types/dyadic_float.hpp"

namespace ccm::support::fp::op
{
	template <typename OutType, typename InType>
	constexpr std::enable_if_t<std::is_floating_point_v<OutType> && std::is_floating_point_v<InType> && sizeof(OutType) <= sizeof(InType), OutType>
	mul(InType x, InType y)
	{
		using OutFPBits		 = FPBits<OutType>;
		using OutStorageType = typename OutFPBits::StorageType;
		using InFPBits		 = FPBits<InType>;
		using InStorageType	 = typename InFPBits::StorageType;

		// The product of two p-digit numbers is a 2p-digit number.
		using DyadicFloat = types::DyadicFloat<ccm::support::bit_ceil(2 * static_cast<size_t>(InFPBits::SIG_LEN))>;

		InFPBits x_bits(x);
		InFPBits y_bits(y);

		types::Sign result_sign = x_bits.sign() == y_bits.sign() ? types::Sign::POS : types::Sign::NEG;

		if (CCM_UNLIKELY(x_bits.is_inf_or_nan() || y_bits.is_inf_or_nan() || x_bits.is_zero() || y_bits.is_zero()))
		{
			if (x_bits.is_nan() || y_bits.is_nan())
			{
				if (x_bits.is_signaling_nan() || y_bits.is_signaling_nan()) { fenv::raise_except_if_required(FE_INVALID); }

				if (x_bits.is_quiet_nan())
				{
					InStorageType x_payload = x_bits.get_mantissa();
					x_payload >>= InFPBits::fraction_length - OutFPBits::fraction_length;
					return OutFPBits::quiet_nan(x_bits.sign(), static_cast<OutStorageType>(x_payload)).get_val();
				}

				if (y_bits.is_quiet_nan())
				{
					InStorageType y_payload = y_bits.get_mantissa();
					y_payload >>= InFPBits::fraction_length - OutFPBits::fraction_length;
					return OutFPBits::quiet_nan(y_bits.sign(), static_cast<OutStorageType>(y_payload)).get_val();
				}

				return OutFPBits::quiet_nan().get_val();
			}

			if (x_bits.is_inf())
			{
				if (y_bits.is_zero())
				{
					fenv::set_errno_if_required(EDOM);
					fenv::raise_except_if_required(FE_INVALID);
					return OutFPBits::quiet_nan().get_val();
				}

				return OutFPBits::inf(result_sign).get_val();
			}

			if (y_bits.is_inf())
			{
				if (x_bits.is_zero())
				{
					fenv::set_errno_if_required(EDOM);
					fenv::raise_except_if_required(FE_INVALID);
					return OutFPBits::quiet_nan().get_val();
				}

				return OutFPBits::inf(result_sign).get_val();
			}

			// Now either x or y is zero, and the other one is finite.
			return OutFPBits::zero(result_sign).get_val();
		}

		DyadicFloat xd(x);
		DyadicFloat yd(y);

		DyadicFloat result = quick_mul(xd, yd);
		return result.template as<OutType, /*ShouldSignalExceptions=*/true>();
	}
} // namespace ccm::support::fp::op