
#pragma once

#include <type_traits>

#include "ccmath/internal/support/fp/ops/add_or_sub.hpp"

namespace ccm::support::fp::op
{
	template <typename OutType, typename InType>
	constexpr std::enable_if_t<std::is_floating_point_v<OutType> && std::is_floating_point_v<InType> && sizeof(OutType) <= sizeof(InType), OutType>
	sub(InType x, InType y)
	{
		return add_or_sub</*IsSub=*/true, OutType>(x, y);
	}
} // namespace ccm::support::fp::op