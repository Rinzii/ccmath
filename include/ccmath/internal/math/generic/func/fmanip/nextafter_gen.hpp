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

#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"

#include <type_traits>

namespace ccm::gen
{
	template <class TFrom, class TTo,
			  std::enable_if_t<std::is_floating_point_v<TFrom> && std::is_floating_point_v<TTo> && (sizeof(TFrom) <= sizeof(TTo)), bool> = true>
	[[nodiscard]] constexpr TFrom nextafter_gen(TFrom from, TTo to) noexcept
	{

		ccm::support::fp::FPBits<TFrom> from_bits(from);
		if (from_bits.is_nan()) { return from; }

		ccm::support::fp::FPBits<TTo> to_bits(to);
		if (to_bits.is_nan()) { return static_cast<TFrom>(to); }

		// This evaluation will only work so long as 'Arithmetic2' is a greater than or equal to the precision of 'Arithmetic1'
		// If 'Arithmetic2' is a lower precision than 'Arithmetic1', then the following statement could incorrectly evaluate to true
		// This is due to loss of precision in the conversion of 'from' to Arithmetic2
		if (static_cast<TTo>(from) == to) { return static_cast<TFrom>(to); }

		using Storage_t = typename ccm::support::fp::FPBits<TFrom>::storage_type;
		if (from != TFrom(0))
		{
			if ((static_cast<TTo>(from) < to) == (from > TFrom(0))) { from_bits = ccm::support::fp::FPBits<TFrom>(Storage_t(from_bits.uintval() + 1)); }
			else { from_bits = ccm::support::fp::FPBits<TFrom>(Storage_t(from_bits.uintval() - 1)); }
		}
		else { from_bits = ccm::support::fp::FPBits<TFrom>::min_subnormal(to_bits.sign()); }

		if (from_bits.is_subnormal()) { ccm::support::fenv::raise_except_if_required(FE_UNDERFLOW | FE_INEXACT); }
		else if (from_bits.is_inf()) { ccm::support::fenv::raise_except_if_required(FE_OVERFLOW | FE_INEXACT); }

		return from_bits.get_val();
	}

#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
	// Specialization for 80-bit long double
	// NOLINTNEXTLINE(readability-function-cognitive-complexity) - This function is complex by nature due to the nature of handling 80-bit long double
	constexpr long double nextafter(long double from, long double to)
	{
		using FPBits_t = ccm::support::fp::FPBits<long double>;
		FPBits_t from_bits(from);
		if (from_bits.is_nan()) { return from; }

		const FPBits_t to_bits(to);
		if (to_bits.is_nan()) { return to; }
		if (from == to) { return to; }

		// Convert pseudo subnormal number to normal number.
		if (static_cast<int>(from_bits.get_implicit_bit()) == 1 && from_bits.is_subnormal()) { from_bits.set_biased_exponent(1); }

		using Storage_t					  = FPBits_t::storage_type;
		constexpr Storage_t fraction_mask = FPBits_t::fraction_mask;

		if (from == 0.0L) // +/-0.0
		{
			from_bits = FPBits_t::min_subnormal(from > to ? types::Sign::NEG : types::Sign::POS);
		}
		else if (from < 0.0L)
		{
			if (to < from) // toward -Inf
			{
				if (from_bits == FPBits_t::max_subnormal(types::Sign::NEG))
				{
					// We deal with normal/subnormal boundary separately to avoid dealing with the implicit bit.
					from_bits = FPBits_t::min_normal(types::Sign::NEG);
				}
				else if (from_bits.get_mantissa() == fraction_mask)
				{
					from_bits.set_mantissa(0);
					// When incrementing the exponent, there is a possibility that the value will overflow to infinity which is expected.
					// Since NaNs are handled separately, it will never overflow "beyond" infinity.
					from_bits.set_biased_exponent(from_bits.get_biased_exponent() + 1);
					if (from_bits.is_inf()) { support::fenv::raise_except_if_required(FE_OVERFLOW | FE_INEXACT); }
					return from_bits.get_val();
				}
				else { from_bits = FPBits_t(static_cast<Storage_t>(from_bits.uintval() + 1)); }
			}
			else // toward +Inf
			{
				if (from_bits == FPBits_t::min_normal(types::Sign::NEG))
				{
					// We deal with normal/subnormal boundary separately to avoid
					// dealing with the implicit bit.
					from_bits = FPBits_t::max_subnormal(types::Sign::NEG);
				}
				else if (from_bits.get_mantissa() == 0)
				{
					from_bits.set_mantissa(fraction_mask);
					// Since we handle from == 0 separately, we can decrement the exponent and not worry about underflow.
					from_bits.set_biased_exponent(from_bits.get_biased_exponent() - 1);
					return from_bits.get_val();
				}
				else { from_bits = FPBits_t(static_cast<Storage_t>(from_bits.uintval() - 1)); }
			}
		}
		else
		{
			if (to < from) // toward -Inf
			{
				if (from_bits == FPBits_t::min_normal(types::Sign::POS)) { from_bits = FPBits_t::max_subnormal(types::Sign::POS); }
				else if (from_bits.get_mantissa() == 0)
				{
					from_bits.set_mantissa(fraction_mask);
					// Since we handle from == 0 separately, we can decrement the exponent and not worry about underflow.
					from_bits.set_biased_exponent(from_bits.get_biased_exponent() - 1);
					return from_bits.get_val();
				}
				else { from_bits = FPBits_t(static_cast<Storage_t>(from_bits.uintval() - 1)); }
			}
			else // toward +Inf
			{
				if (from_bits == FPBits_t::max_subnormal(types::Sign::POS)) { from_bits = FPBits_t::min_normal(types::Sign::POS); }
				else if (from_bits.get_mantissa() == fraction_mask)
				{
					from_bits.set_mantissa(0);
					// When incrementing the exponent, there is a possibility that the value will overflow to infinity which is expected.
					// Since NaNs are handled separately, it will never overflow "beyond" infinity.
					from_bits.set_biased_exponent(from_bits.get_biased_exponent() + 1);
					if (from_bits.is_inf()) { support::fenv::raise_except_if_required(FE_OVERFLOW | FE_INEXACT); }
					return from_bits.get_val();
				}
				else { from_bits = FPBits_t(static_cast<Storage_t>(from_bits.uintval() + 1)); }
			}
		}

		if (!from_bits.get_implicit_bit()) { support::fenv::raise_except_if_required(FE_UNDERFLOW | FE_INEXACT); }

		return from_bits.get_val();
	}
#endif
} // namespace ccm::gen
