/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

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

		static constexpr MaskType pos_zero = 0x0000000000000000;
		static constexpr MaskType neg_zero = 0x8000000000000000;
		static constexpr MaskType one = 0x3FF0000000000000;
		static constexpr MaskType two = 0x4000000000000000;
		static constexpr MaskType max_norm = 0x7FEFFFFFFFFFFFFF;
		static constexpr MaskType min_pos_norm = 0x0010000000000000;
		static constexpr MaskType max_subnorm = 0x000FFFFFFFFFFFFF;
		static constexpr MaskType min_pos_subnorm = 0x0000000000000001;
		static constexpr MaskType pos_inf = 0x7FF0000000000000;
		static constexpr MaskType neg_inf = 0xFFF0000000000000;
		static constexpr MaskType pos_nan = 0x7FF8000000000000;
		static constexpr MaskType neg_nan = 0xFFF8000000000000;
	};

	template <>
	struct bitmask_traits<float>
	{
		using MaskType = std::uint32_t;

		static constexpr MaskType pos_zero = 0x00000000;
		static constexpr MaskType neg_zero = 0x80000000;
		static constexpr MaskType one = 0x3F800000;
		static constexpr MaskType two = 0x40000000;
		static constexpr MaskType max_norm = 0x7F7FFFFF;
		static constexpr MaskType min_pos_norm = 0x00800000;
		static constexpr MaskType max_subnorm = 0x007FFFFF;
		static constexpr MaskType min_pos_subnorm = 0x00000001;
		static constexpr MaskType pos_inf = 0x7F800000;
		static constexpr MaskType neg_inf = 0xFF800000;
		static constexpr MaskType pos_nan = 0x7FC00000;
		static constexpr MaskType neg_nan = 0xFFC00000;
	};

	template <>
	struct bitmask_traits<double>
	{
		using MaskType = std::uint64_t;

		static constexpr MaskType pos_zero = 0x0000000000000000;
		static constexpr MaskType neg_zero = 0x8000000000000000;
		static constexpr MaskType one = 0x3FF0000000000000;
		static constexpr MaskType two = 0x4000000000000000;
		static constexpr MaskType max_norm = 0x7FEFFFFFFFFFFFFF;
		static constexpr MaskType min_pos_norm = 0x0010000000000000;
		static constexpr MaskType max_subnorm = 0x000FFFFFFFFFFFFF;
		static constexpr MaskType min_pos_subnorm = 0x0000000000000001;
		static constexpr MaskType pos_inf = 0x7FF0000000000000;
		static constexpr MaskType neg_inf = 0xFFF0000000000000;
		static constexpr MaskType pos_nan = 0x7FF8000000000000;
		static constexpr MaskType neg_nan = 0xFFF8000000000000;
	};

	// TODO: Implement long double bit masks for both extended and quad precision.


} // namespace ccm::support