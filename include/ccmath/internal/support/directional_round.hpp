#pragma once

#include "ccmath/math/nearest/ceil.hpp"
#include "ccmath/math/nearest/floor.hpp"

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
