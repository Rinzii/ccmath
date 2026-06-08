#pragma once

namespace ccm::test
{
	// Prevent constexpr evaluation of inputs when exercising runtime dispatch paths.
	template <typename T>
	inline T runtime_value(T value)
	{
		volatile T sink = value;
		return sink;
	}
} // namespace ccm::test

// MSVC on Windows has no __int128, so multiword::mul2 uses a fallback path that
// MSVC cannot evaluate in constexpr contexts.
#if defined(_MSC_VER) && !defined(__clang__)
	#define CCMATH_SUPPORTS_DEEP_CONSTEXPR 0
#else
	#define CCMATH_SUPPORTS_DEEP_CONSTEXPR 1
#endif
