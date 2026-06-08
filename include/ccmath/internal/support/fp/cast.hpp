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

#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/support/type_traits.hpp"
#include "ccmath/internal/types/dyadic_float.hpp"

#include <cfenv>
#include <type_traits>

namespace ccm::support::fp
{
	template <typename OutType, typename InType>
	[[nodiscard]] constexpr std::enable_if_t<traits::ccm_is_floating_point_v<OutType> && traits::ccm_is_floating_point_v<InType>, OutType> cast(
		InType x)
	{
		if constexpr (std::is_same_v<InType, OutType>) { return x; }
		else if constexpr (sizeof(OutType) >= sizeof(InType)) { return static_cast<OutType>(x); }
		else
		{
			using InFPBits	  = FPBits<InType>;
			using InStorage	  = typename InFPBits::storage_type;
			using OutFPBits	  = FPBits<OutType>;
			using OutStorage  = typename OutFPBits::storage_type;
			const InFPBits x_bits(x);

			if (x_bits.is_nan())
			{
				if (x_bits.is_signaling_nan())
				{
					fenv::raise_except_if_required(FE_INVALID);
					return OutFPBits::quiet_nan().get_val();
				}

				InStorage x_mant = x_bits.get_mantissa();
				if (InFPBits::fraction_length > OutFPBits::fraction_length)
				{
					x_mant >>= InFPBits::fraction_length - OutFPBits::fraction_length;
				}
				return OutFPBits::quiet_nan(x_bits.sign(), static_cast<OutStorage>(x_mant)).get_val();
			}

			if (x_bits.is_inf()) { return OutFPBits::inf(x_bits.sign()).get_val(); }

			constexpr std::size_t max_fraction_len =
				InFPBits::fraction_length > OutFPBits::fraction_length ? InFPBits::fraction_length : OutFPBits::fraction_length;
			types::DyadicFloat<bit_ceil(max_fraction_len + 1)> xd(x);
			return xd.template as<OutType, /*ShouldSignalExceptions=*/true>();
		}
	}

} // namespace ccm::support::fp
