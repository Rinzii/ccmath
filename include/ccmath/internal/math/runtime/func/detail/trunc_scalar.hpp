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

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"

namespace ccm::rt::detail
{
	template <typename T> [[nodiscard]] inline T trunc_scalar(T num) noexcept
	{
		using FPBits_t	= ccm::support::fp::FPBits<T>;
		using Storage_t = typename FPBits_t::storage_type;

		FPBits_t bits(num);

		if (CCM_UNLIKELY(bits.is_inf_or_nan()))
		{
			return num;
		}
		if (CCM_UNLIKELY(num == static_cast<T>(0)))
		{
			return num;
		}

		const int exponent = bits.get_exponent();
		if (exponent >= FPBits_t::fraction_length)
		{
			return num;
		}
		if (exponent <= -1)
		{
			return FPBits_t::zero(bits.sign()).get_val();
		}

		const int trimming_size		  = FPBits_t::fraction_length - exponent;
		const auto truncated_mantissa = static_cast<Storage_t>((bits.get_mantissa() >> trimming_size) << trimming_size);
		bits.set_mantissa(truncated_mantissa);
		return bits.get_val();
	}

	template <typename T> [[nodiscard]] inline T floor_scalar(T num) noexcept
	{
		if (ccm::isinf(num) || num == static_cast<T>(0) || ccm::isnan(num))
		{
			return num;
		}

		const T truncated = trunc_scalar(num);
		if (truncated == num || num > static_cast<T>(0))
		{
			return truncated;
		}
		return truncated - static_cast<T>(1);
	}

	template <typename T> [[nodiscard]] inline T ceil_scalar(T num) noexcept
	{
		if (ccm::isinf(num) || num == static_cast<T>(0) || ccm::isnan(num))
		{
			return num;
		}

		const T truncated = trunc_scalar(num);
		if (truncated == num || num < static_cast<T>(0))
		{
			return truncated;
		}
		return truncated + static_cast<T>(1);
	}
} // namespace ccm::rt::detail
