/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <type_traits>

namespace ccm::predef::internal
{
	/**
	 * @brief Helper function to prevent the user from accidentally passing an integer to an expect function.
	 */
	template <typename T, std::enable_if_t<std::is_same_v<T, bool>, bool> = true>
	constexpr bool expects_bool_condition(T value, bool expected)
	{
#if defined(__GNUC__) || defined(__clang__)
		return __builtin_expect(value, expected);
#else
		return value;
#endif // defined(__GNUC__) || defined(__clang__)
	}
} // namespace ccm::predef::internal