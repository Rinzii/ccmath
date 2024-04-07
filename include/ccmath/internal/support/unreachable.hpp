/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#if defined(__cpp_lib_unreachable) && __cpp_lib_unreachable >= 202202L
	#include <utility>
#endif

namespace ccm
{
	[[noreturn]] inline void unreachable()
	{
		// If we can't use the standard library's unreachable function,
		// Then we'll use the compiler specific extensions if possible.
#if defined(__cpp_lib_unreachable) && __cpp_lib_unreachable >= 202202L
		std::unreachable();
#elif defined(_MSC_VER) && !defined(__clang__) // MSVC
		__assume(false);
#elif defined(__GNUC__) || defined(__clang__)  // GCC, Clang
		__builtin_unreachable();
#else										   // Fallback
											   // In the event that no compiler specific extension is available
											   // Then do nothing.
		static_cast<void>(0);
#endif
	}

} // namespace ccm
