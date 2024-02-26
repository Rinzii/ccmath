/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <type_traits>

#include "ccmath/detail/compare/isnan.hpp"

#if defined(__cpp_lib_constexpr_cmath) && __cpp_lib_constexpr_cmath >= 202202L
	#include <cmath>
	#ifndef CCMATH_HAS_SIGNBIT
        #define CCMATH_HAS_SIGNBIT
	#endif
#endif

#if defined(__has_include) && !defined(CCMATH_HAS_SIGNBIT)
	#if __has_include(<bit>)
		#if defined(__cpp_lib_bit_cast) && __cpp_lib_bit_cast >= 201806L
			#include <bit>
			#ifndef CCMATH_HAS_BIT_CAST
                #define CCMATH_HAS_BIT_CAST
			#endif
		#endif
	#elif defined(__has_builtin)
		#if __has_builtin(__builtin_bit_cast)
			#ifndef CCMATH_HAS_BUILTIN_BIT_CAST
                #define CCMATH_HAS_BUILTIN_BIT_CAST
			#endif
		#endif
	#endif
#endif

namespace ccm
{
	namespace
	{
		namespace detail
		{
#if defined(CCMATH_HAS_BIT_CAST) || defined(CCMATH_HAS_BUILTIN_BIT_CAST)
			template <To Type, class From>
			std::enable_if_t<
				sizeof(To) == sizeof(From) &&
					std::is_trivially_copyable_v<From> &&
					std::is_trivially_copyable_v<To>,
				To>
			constexpr To internal_bit_cast(To type, Fram arg)
			{
	#if defined(CCMATH_HAS_BIT_CAST)
				return std::bit_cast<To>(arg);
    #elif defined(CCMATH_HAS_BUILTIN_BIT_CAST)
				return __builtin_bit_cast(type, arg);
    #endif
			}
#endif
		}

		namespace impl
		{
			template <typename T>
            constexpr bool signbit_impl(T x)
			{
				if constexpr (std::is_same_v<T, float>)
				{
				}

				return false;
			}

		}
	}

	template <typename T>
	constexpr bool signbit(T x) noexcept
	{
		//static_assert();

#if defined(CCMATH_HAS_SIGNBIT)
        return std::signbit(x);
#elif defined(CCMATH_HAS_BIT_CAST)
        return std::bit_cast<T>(x) >> (sizeof(T) * 8 - 1);
#elif defined(_MSC_VER)
		return (x == T(0)) ? (_fpclass(x) == _FPCLASS_NZ) : (x < T(0));
#elif defined(__GNUC__)
		return __builtin_signbit(x);
#else
		return x < static_cast<T>(0);
#endif
	}

} // namespace ccm
