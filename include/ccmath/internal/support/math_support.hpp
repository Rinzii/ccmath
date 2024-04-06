/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <climits>
#include <type_traits>

namespace ccm::support
{
	// Create a bitmask with the count right-most bits set to 1, and all other bits
	// set to 0.  Only unsigned types are allowed.
	template <typename T, std::size_t count>
	constexpr std::enable_if_t<std::is_unsigned_v<T>, T>
	mask_trailing_ones() {
		constexpr unsigned T_BITS = CHAR_BIT * sizeof(T);
		static_assert(count <= T_BITS && "Invalid bit index");
		return count == 0 ? 0 : (T(-1) >> (T_BITS - count));
	}

	// Create a bitmask with the count left-most bits set to 1, and all other bits
	// set to 0.  Only unsigned types are allowed.
	template <typename T, std::size_t count>
	constexpr std::enable_if_t<std::is_unsigned_v<T>, T>
	mask_leading_ones() {
		return T(~mask_trailing_ones<T, CHAR_BIT * sizeof(T) - count>());
	}
}
