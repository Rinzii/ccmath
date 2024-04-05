

#pragma once

#include "ccmath/internal/types/uint.hpp"

namespace ccm
{
#ifdef CCM_HAS_INT128
	using uint128 = __uint128_t;
	using int128 = __int128_t;
#else
	using uint128 = ccm::UInt<128>;
	using int128 = ccm::Int<128>;
#endif // CCM_HAS_INT128

} // namespace ccm
