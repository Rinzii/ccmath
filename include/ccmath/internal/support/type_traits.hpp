/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

 // Wrapper for type_traits with additional specializations

#pragma once

#include "ccmath/internal/config/type_support.hpp"

#include <type_traits>

#include "ccmath/internal/types/int128.hpp"

namespace ccm::support
{
	template <typename T>
	struct is_unsigned : std::is_unsigned<T> {};

	template <>
	struct is_unsigned<ccm::uint128_t> : std::true_type {};

	template <typename T>
	constexpr bool is_unsigned_v = is_unsigned<T>::value;


} // namespace ccm::support
