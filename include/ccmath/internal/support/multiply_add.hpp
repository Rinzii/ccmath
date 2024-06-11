/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/builtin/fma_support.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"

#include <type_traits>

namespace ccm::support
{
	// Simple wrapper for FMA. Will use constexpr builtin FMA if available. Will also use non-constexpr builtin FMA if available when not in constexpr context.

	template <typename T>
	constexpr std::enable_if_t<(sizeof(T) > sizeof(void *)), T> multiply_add(const T & x, const T & y, const T & z)
	{
#if defined(CCMATH_HAS_CONSTEXPR_BUILTIN_FMA)
		if constexpr (std::is_same_v<T, float>) { return __builtin_fmaf(x, y, z); }
		if constexpr (std::is_same_v<T, double>) { return __builtin_fma(x, y, z); }
		if constexpr (std::is_same_v<T, long double>) { return __builtin_fmal(x, y, z); }

		return static_cast<T>(__builtin_fmal(x, y, z));
#elif CCM_HAS_BUILTIN(__builtin_fma) // If we don't have constexpr FMA, but we have a non-constexpr FMA, use that when not in constexpr context.
		if constexpr (is_constant_evaluated())
		{
			return x * y + z; // We can only hope the compiler optimizes this.
		}
		else
		{
			if constexpr (std::is_same_v<T, float>) { return __builtin_fmaf(x, y, z); }
			if constexpr (std::is_same_v<T, double>) { return __builtin_fma(x, y, z); }
			if constexpr (std::is_same_v<T, long double>) { return __builtin_fmal(x, y, z); }
		}

#else
		return x * y + z; // If we don't have FMA, just do the multiply and add and hope the compiler optimizes it.
#endif
	}

	template <typename T>
	constexpr std::enable_if_t<(sizeof(T) <= sizeof(void *)), T> multiply_add(T x, T y, T z)
	{
#if defined(CCMATH_HAS_CONSTEXPR_BUILTIN_FMA)
		if constexpr (std::is_same_v<T, float>) { return __builtin_fmaf(x, y, z); }
		if constexpr (std::is_same_v<T, double>) { return __builtin_fma(x, y, z); }
		if constexpr (std::is_same_v<T, long double>) { return __builtin_fmal(x, y, z); }

		return static_cast<T>(__builtin_fmal(x, y, z));
#elif CCM_HAS_BUILTIN(__builtin_fma) // If we don't have constexpr FMA, but we have a non-constexpr FMA, use that when not in constexpr context.
		if constexpr (is_constant_evaluated())
		{
			return x * y + z; // We can only hope the compiler optimizes this.
		}
		else
		{
			if constexpr (std::is_same_v<T, float>) { return __builtin_fmaf(x, y, z); }
			if constexpr (std::is_same_v<T, double>) { return __builtin_fma(x, y, z); }
			if constexpr (std::is_same_v<T, long double>) { return __builtin_fmal(x, y, z); }
		}
#else
		return x * y + z;
#endif
	}
} // namespace ccm::support