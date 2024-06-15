/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/integer_literals.hpp"
#include "ccmath/internal/types/int128_types.hpp"

#include <cstdint>
#include <limits>
#include <type_traits>

namespace ccm::support
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	struct bitmask_traits
	{
		// If we've not specialized for the provided type then assume double.
		using MaskType = std::uint64_t;

		static constexpr MaskType pos_zero		  = 0x0000000000000000;
		static constexpr MaskType neg_zero		  = 0x8000000000000000;
		static constexpr MaskType half_one		  = 0x3FE0000000000000;
		static constexpr MaskType neg_half_one	  = 0xBFE0000000000000;
		static constexpr MaskType one			  = 0x3FF0000000000000;
		static constexpr MaskType two			  = 0x4000000000000000;
		static constexpr MaskType max_norm		  = 0x7FEFFFFFFFFFFFFF;
		static constexpr MaskType min_pos_norm	  = 0x0010000000000000;
		static constexpr MaskType max_subnorm	  = 0x000FFFFFFFFFFFFF;
		static constexpr MaskType min_pos_subnorm = 0x0000000000000001;
		static constexpr MaskType pos_inf		  = 0x7FF0000000000000;
		static constexpr MaskType neg_inf		  = 0xFFF0000000000000;
		static constexpr MaskType pos_nan		  = 0x7FF8000000000000;
		static constexpr MaskType neg_nan		  = 0xFFF8000000000000;
	};

	template <>
	struct bitmask_traits<float>
	{
		using MaskType = std::uint32_t;

		static constexpr MaskType pos_zero		  = 0x00000000;
		static constexpr MaskType neg_zero		  = 0x80000000;
		static constexpr MaskType half_one		  = 0x3F000000;
		static constexpr MaskType neg_half_one	  = 0xBF000000;
		static constexpr MaskType one			  = 0x3F800000;
		static constexpr MaskType two			  = 0x40000000;
		static constexpr MaskType max_norm		  = 0x7F7FFFFF;
		static constexpr MaskType min_pos_norm	  = 0x00800000;
		static constexpr MaskType max_subnorm	  = 0x007FFFFF;
		static constexpr MaskType min_pos_subnorm = 0x00000001;
		static constexpr MaskType pos_inf		  = 0x7F800000;
		static constexpr MaskType neg_inf		  = 0xFF800000;
		static constexpr MaskType pos_nan		  = 0x7FC00000;
		static constexpr MaskType neg_nan		  = 0xFFC00000;
	};

	template <>
	struct bitmask_traits<double>
	{
		using MaskType = std::uint64_t;

		static constexpr MaskType pos_zero		  = 0x0000000000000000;
		static constexpr MaskType neg_zero		  = 0x8000000000000000;
		static constexpr MaskType half_one		  = 0x3FE0000000000000;
		static constexpr MaskType neg_half_one	  = 0xBFE0000000000000;
		static constexpr MaskType one			  = 0x3FF0000000000000;
		static constexpr MaskType two			  = 0x4000000000000000;
		static constexpr MaskType max_norm		  = 0x7FEFFFFFFFFFFFFF;
		static constexpr MaskType min_pos_norm	  = 0x0010000000000000;
		static constexpr MaskType max_subnorm	  = 0x000FFFFFFFFFFFFF;
		static constexpr MaskType min_pos_subnorm = 0x0000000000000001;
		static constexpr MaskType pos_inf		  = 0x7FF0000000000000;
		static constexpr MaskType neg_inf		  = 0xFFF0000000000000;
		static constexpr MaskType pos_nan		  = 0x7FF8000000000000;
		static constexpr MaskType neg_nan		  = 0xFFF8000000000000;
	};

	// TODO: Implement long double bit masks for both extended and quad precision.

#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT128)

	// Values from this table:
	// https://en.wikipedia.org/wiki/Quadruple-precision_floating-point_format/
	template <>
	struct bitmask_traits<long double>
	{
		using MaskType = ccm::types::uint128_t;

		static constexpr MaskType pos_zero		  = 0x00000000000000000000000000000000_u128;
		static constexpr MaskType neg_zero		  = 0x80000000000000000000000000000000_u128;
		static constexpr MaskType half_one		  = 0x3FFE0000000000000000000000000000_u128;
		static constexpr MaskType neg_half_one	  = 0xBFFE0000000000000000000000000000_u128;
		static constexpr MaskType one			  = 0x3FFF0000000000000000000000000000_u128;
		static constexpr MaskType two			  = 0x40000000000000000000000000000000_u128;
		static constexpr MaskType max_norm		  = 0x7FFEFFFFFFFFFFFFFFFFFFFFFFFFFFFF_u128;
		static constexpr MaskType min_pos_norm	  = 0x00010000000000000000000000000000_u128;
		static constexpr MaskType max_subnorm	  = 0x0000FFFFFFFFFFFFFFFFFFFFFFFFFFFF_u128;
		static constexpr MaskType min_pos_subnorm = 0x00000000000000000000000000000001_u128;
		static constexpr MaskType pos_inf		  = 0x7FFF0000000000000000000000000000_u128;
		static constexpr MaskType neg_inf		  = 0xFFFF0000000000000000000000000000_u128;
		static constexpr MaskType pos_nan		  = 0x7FFF8000000000000000000000000000_u128;
		static constexpr MaskType neg_nan		  = 0xFFFF8000000000000000000000000000_u128;
	};
#elif defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
	template <>
	struct bitmask_traits<long double>
	{
		using MaskType = ccm::types::uint128_t;

		static constexpr MaskType pos_zero		  = 0x00000000000000000000000000000000_u128;
		static constexpr MaskType neg_zero		  = 0x80000000000000000000000000000000_u128;
		static constexpr MaskType half_one		  = 0x3FFE0000000000000000000000000000_u128;
		static constexpr MaskType neg_half_one	  = 0xBFFE0000000000000000000000000000_u128;
		static constexpr MaskType one			  = 0x3FFF0000000000000000000000000000_u128;
		static constexpr MaskType two			  = 0x40000000000000000000000000000000_u128;
		static constexpr MaskType max_norm		  = 0x7FFEFFFFFFFFFFFFFFFFFFFFFFFFFFFF_u128;
		static constexpr MaskType min_pos_norm	  = 0x00010000000000000000000000000000_u128;
		static constexpr MaskType max_subnorm	  = 0x0000FFFFFFFFFFFFFFFFFFFFFFFFFFFF_u128;
		static constexpr MaskType min_pos_subnorm = 0x00000000000000000000000000000001_u128;
		static constexpr MaskType pos_inf		  = 0x7FFF0000000000000000000000000000_u128;
		static constexpr MaskType neg_inf		  = 0xFFFF0000000000000000000000000000_u128;
		static constexpr MaskType pos_nan		  = 0x7FFF8000000000000000000000000000_u128;
		static constexpr MaskType neg_nan		  = 0xFFFF8000000000000000000000000000_u128;
	};
#else // long double is the same as double
	template <>
	struct bitmask_traits<long double>
	{
		using MaskType = std::uint64_t;

		static constexpr MaskType pos_zero		  = 0x0000000000000000;
		static constexpr MaskType neg_zero		  = 0x8000000000000000;
		static constexpr MaskType half_one		  = 0x3FE0000000000000;
		static constexpr MaskType neg_half_one	  = 0xBFE0000000000000;
		static constexpr MaskType one			  = 0x3FF0000000000000;
		static constexpr MaskType two			  = 0x4000000000000000;
		static constexpr MaskType max_norm		  = 0x7FEFFFFFFFFFFFFF;
		static constexpr MaskType min_pos_norm	  = 0x0010000000000000;
		static constexpr MaskType max_subnorm	  = 0x000FFFFFFFFFFFFF;
		static constexpr MaskType min_pos_subnorm = 0x0000000000000001;
		static constexpr MaskType pos_inf		  = 0x7FF0000000000000;
		static constexpr MaskType neg_inf		  = 0xFFF0000000000000;
		static constexpr MaskType pos_nan		  = 0x7FF8000000000000;
		static constexpr MaskType neg_nan		  = 0xFFF8000000000000;
	};
#endif

} // namespace ccm::support