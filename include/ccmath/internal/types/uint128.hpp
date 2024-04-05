

#pragma once

#include "ccmath/internal/types/uint.hpp"

namespace ccm
{
#ifdef CCM_HAS_INT128
	using uint128_t = __uint128_t;
	using int128_t = __int128_t;
#else
	using uint128_t = ccm::UInt<128>;
	using int128_t = ccm::Int<128>;
#endif // CCM_HAS_INT128

} // namespace ccm
