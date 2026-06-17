/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#pragma once

// On MSVC there is no foldable __builtin math and no SVML transcendental path, so the runtime scalar
// fallback would otherwise use ccmath's generic kernels, which are not accurate enough to match the
// system libm. Including the real <math.h> and calling the global C runtime entry points gives UCRT
// speed and accuracy. Using the actual UCRT header (rather than forward declaring the symbols) means
// a consumer that also includes <cmath> just sees the same include-guarded declarations, so there is
// no C2733/C2375/C4273 linkage conflict.
//
// CCM_CONFIG_SYSTEM_MATH is on by default for MSVC-proper. Define CCM_CONFIG_DISABLE_SYSTEM_MATH to
// turn it off and keep the build free of <math.h> (the generic kernels are used instead). Define
// CCM_CONFIG_FORCE_SYSTEM_MATH to exercise this path on other compilers for testing.
#if !defined(CCM_CONFIG_DISABLE_SYSTEM_MATH) && (defined(CCM_CONFIG_FORCE_SYSTEM_MATH) || (defined(_MSC_VER) && !defined(__clang__)))
	#define CCM_CONFIG_SYSTEM_MATH 1
#endif

#if defined(CCM_CONFIG_SYSTEM_MATH)

	#include <math.h>

	#include <type_traits>

namespace ccm::rt::detail::sys
{
	// Expensive transcendentals. The runtime path uses these only under FE_TONEAREST, since libm is
	// not correctly rounded in the directed modes (the kernel handles those, like the builtin path).
	template <typename T>
	[[nodiscard]] inline T exp2_call(T x)
	{
		if constexpr (std::is_same_v<T, float>) { return ::exp2f(x); }
		else
		{
			return ::exp2(static_cast<double>(x));
		}
	}

	template <typename T>
	[[nodiscard]] inline T log_call(T x)
	{
		if constexpr (std::is_same_v<T, float>) { return ::logf(x); }
		else
		{
			return ::log(static_cast<double>(x));
		}
	}

	template <typename T>
	[[nodiscard]] inline T log2_call(T x)
	{
		if constexpr (std::is_same_v<T, float>) { return ::log2f(x); }
		else
		{
			return ::log2(static_cast<double>(x));
		}
	}

	template <typename T>
	[[nodiscard]] inline T log10_call(T x)
	{
		if constexpr (std::is_same_v<T, float>) { return ::log10f(x); }
		else
		{
			return ::log10(static_cast<double>(x));
		}
	}

	template <typename T>
	[[nodiscard]] inline T sin_call(T x)
	{
		if constexpr (std::is_same_v<T, float>) { return ::sinf(x); }
		else
		{
			return ::sin(static_cast<double>(x));
		}
	}

	template <typename T>
	[[nodiscard]] inline T cos_call(T x)
	{
		if constexpr (std::is_same_v<T, float>) { return ::cosf(x); }
		else
		{
			return ::cos(static_cast<double>(x));
		}
	}

	template <typename T>
	[[nodiscard]] inline T tan_call(T x)
	{
		if constexpr (std::is_same_v<T, float>) { return ::tanf(x); }
		else
		{
			return ::tan(static_cast<double>(x));
		}
	}

	template <typename T>
	[[nodiscard]] inline T gamma_call(T x)
	{
		if constexpr (std::is_same_v<T, float>) { return ::tgammaf(x); }
		else
		{
			return ::tgamma(static_cast<double>(x));
		}
	}

	template <typename T>
	[[nodiscard]] inline T lgamma_call(T x)
	{
		if constexpr (std::is_same_v<T, float>) { return ::lgammaf(x); }
		else
		{
			return ::lgamma(static_cast<double>(x));
		}
	}

	// fmod and remainder are exact in every rounding mode, so they are routed unconditionally. The
	// generic *_rt formula (x - trunc(x / y) * y) loses all precision for large quotients, and the
	// exact kernel cannot be reached here without pulling the nearest dispatch (and pp) back in.
	template <typename T>
	[[nodiscard]] inline T fmod_call(T x, T y)
	{
		if constexpr (std::is_same_v<T, float>) { return ::fmodf(x, y); }
		else
		{
			return ::fmod(static_cast<double>(x), static_cast<double>(y));
		}
	}

	template <typename T>
	[[nodiscard]] inline T remainder_call(T x, T y)
	{
		if constexpr (std::is_same_v<T, float>) { return ::remainderf(x, y); }
		else
		{
			return ::remainder(static_cast<double>(x), static_cast<double>(y));
		}
	}
} // namespace ccm::rt::detail::sys

#endif // CCM_CONFIG_SYSTEM_MATH
