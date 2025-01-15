
#pragma once

#include "ccmath/internal/predef/attributes/always_inline.hpp"

#include <cstddef>

#if defined(_MSC_VER) && !defined(__clang__)
	#include <cstdint>
#endif

namespace ccm::pp
{
	template <std::size_t Alignment>
	CCM_ALWAYS_INLINE void * assume_aligned(void * ptr)
	{
		static_assert((Alignment & (Alignment - 1)) == 0, "Alignment must be a power of 2"); // TODO: Might remove this check not sure if it's necessary
#if defined(__GNUC__) || (defined(__clang__) && !defined(_MSC_VER))
		return __builtin_assume_aligned(ptr, Alignment);
#elif defined(_MSC_VER)
		__assume((reinterpret_cast<std::uintptr_t>(ptr) & (Alignment - 1)) == 0);
		return ptr;
#else
		return ptr;
#endif
	}
} // namespace ccm::pp