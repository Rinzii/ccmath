/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#ifdef __has_include
	# if __has_include(<version>)
		#  include <version>
		#  ifdef __cpp_lib_is_constant_evaluated
			#   include <type_traits>
			#   define CCMATH_HAS_IS_CONSTANT_EVALUATED
		#  endif
	# endif
#endif

#ifdef __has_builtin
	#  if __has_builtin(__builtin_is_constant_evaluated)
		#    define CCMATH_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#  endif
#endif

// GCC 9 and later has __builtin_is_constant_evaluated
#if (__GNUC__ >= 9) && !defined(CCMATH_HAS_BUILTIN_IS_CONSTANT_EVALUATED)
	#  define CCMATH_HAS_BUILTIN_IS_CONSTANT_EVALUATED
#endif

// Visual Studio 2019 and later supports __builtin_is_constant_evaluated
#if defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 192528326)
	#  define CCMATH_HAS_BUILTIN_IS_CONSTANT_EVALUATED
#endif

namespace ccm::traits
{
	inline constexpr bool is_constant_evaluated() noexcept
	{
#if defined(CCMATH_HAS_IS_CONSTANT_EVALUATED)
		return std::is_constant_evaluated();
#elif defined(CCMATH_HAS_BUILTIN_IS_CONSTANT_EVALUATED)
        return __builtin_is_constant_evaluated();
#else
        return false;
#endif
	}
} // namespace ccm::type_traits


// Undefine the helper macros to clean up the macro namespace
// These macros should not be used outside of this file
#undef CCMATH_HAS_IS_CONSTANT_EVALUATED
#undef CCMATH_HAS_BUILTIN_IS_CONSTANT_EVALUATED

