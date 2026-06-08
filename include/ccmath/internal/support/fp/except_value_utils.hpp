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

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fenv/rounding_mode.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/internal/support/type_traits.hpp"

#include <cfenv>
#include <optional>

namespace ccm::support::fp
{
	template <typename T, std::size_t N>
	struct ExceptValues
	{
		static_assert(traits::ccm_is_floating_point_v<T>, "ExceptValues requires a floating-point type.");

		using StorageType = typename FPBits<T>::storage_type;

		struct Mapping
		{
			StorageType input;
			StorageType rnd_towardzero_result;
			StorageType rnd_upward_offset;
			StorageType rnd_downward_offset;
			StorageType rnd_tonearest_offset;
		};

		Mapping values[N];

		[[nodiscard]] constexpr std::optional<T> lookup(StorageType x_bits) const
		{
			for (std::size_t i = 0; i < N; ++i)
			{
				if (CCM_UNLIKELY(x_bits == values[i].input))
				{
					StorageType out_bits = values[i].rnd_towardzero_result;
					switch (fenv::get_rounding_mode())
					{
					case FE_UPWARD: out_bits += values[i].rnd_upward_offset; break;
					case FE_DOWNWARD: out_bits += values[i].rnd_downward_offset; break;
					case FE_TONEAREST: out_bits += values[i].rnd_tonearest_offset; break;
					default: break;
					}
					return FPBits<T>(out_bits).get_val();
				}
			}
			return std::nullopt;
		}

		[[nodiscard]] constexpr std::optional<T> lookup_odd(StorageType x_abs, bool sign) const
		{
			for (std::size_t i = 0; i < N; ++i)
			{
				if (CCM_UNLIKELY(x_abs == values[i].input))
				{
					StorageType out_bits = values[i].rnd_towardzero_result;
					switch (fenv::get_rounding_mode())
					{
					case FE_UPWARD:
						if (sign) { out_bits += values[i].rnd_downward_offset; }
						else { out_bits += values[i].rnd_upward_offset; }
						break;
					case FE_DOWNWARD:
						if (sign) { out_bits += values[i].rnd_upward_offset; }
						else { out_bits += values[i].rnd_downward_offset; }
						break;
					case FE_TONEAREST: out_bits += values[i].rnd_tonearest_offset; break;
					default: break;
					}

					T result = FPBits<T>(out_bits).get_val();
					if (sign) { result = -result; }
					return result;
				}
			}
			return std::nullopt;
		}
	};

	template <typename T>
	[[nodiscard]] constexpr T round_result_slightly_down(T value_rn)
	{
		if (support::is_constant_evaluated()) { return value_rn; }

		volatile T tmp = value_rn;
		tmp -= FPBits<T>::min_normal().get_val();
		return tmp;
	}

	template <typename T>
	[[nodiscard]] constexpr T round_result_slightly_up(T value_rn)
	{
		if (support::is_constant_evaluated()) { return value_rn; }

		volatile T tmp = value_rn;
		tmp += FPBits<T>::min_normal().get_val();
		return tmp;
	}

} // namespace ccm::support::fp
