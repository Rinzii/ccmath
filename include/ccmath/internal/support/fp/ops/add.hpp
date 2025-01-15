
#pragma once

#include "ccmath/internal/support/fp/ops/add_or_sub.hpp"

#include <type_traits>

namespace ccm::support::fp::op
{
	template <typename OutType, typename InType>
	constexpr std::enable_if_t<std::is_floating_point_v<OutType> && std::is_floating_point_v<InType> && sizeof(OutType) <= sizeof(InType), OutType>
	add(InType x, InType y)
	{
		return add_or_sub</*IsSub=*/false, OutType>(x, y);
	}
} // namespace ccm::support::fp::op