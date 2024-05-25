/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present ccmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
*/

#pragma once

#include "ccmath/math/nearest/ceil.hpp"
#include "ccmath/math/nearest/floor.hpp"

#include <type_traits>

namespace ccm::support {
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr toward_zero(T x) noexcept
	{
		return (x < 0) ? ccm::ceil(x) : ccm::floor(x);
	}

	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr toward_inf(T x) noexcept
	{
		return (x < 0) ? ccm::floor(x) : ccm::ceil(x);
	}
}
