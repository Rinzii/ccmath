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

#include <type_traits>

namespace ccm::rt::detail::msvc_libm
{
#if defined(_MSC_VER) && !defined(__clang__)
	extern "C" float exp2f(float);
	extern "C" double exp2(double);
	extern "C" float logf(float);
	extern "C" double log(double);
	extern "C" float log2f(float);
	extern "C" double log2(double);
	extern "C" float log10f(float);
	extern "C" double log10(double);
	extern "C" float sinf(float);
	extern "C" double sin(double);
	extern "C" float cosf(float);
	extern "C" double cos(double);
	extern "C" float tanf(float);
	extern "C" double tan(double);
	extern "C" float fmodf(float, float);
	extern "C" double fmod(double, double);
	extern "C" float remainderf(float, float);
	extern "C" double remainder(double, double);
	extern "C" float tgammaf(float);
	extern "C" double tgamma(double);
	extern "C" float lgammaf(float);
	extern "C" double lgamma(double);

	template <typename T>
	[[nodiscard]] inline T exp2_call(T x) noexcept
	{
		if constexpr (std::is_same_v<T, float>) { return exp2f(x); }
		else { return static_cast<T>(exp2(static_cast<double>(x))); }
	}

	template <typename T>
	[[nodiscard]] inline T log_call(T x) noexcept
	{
		if constexpr (std::is_same_v<T, float>) { return logf(x); }
		else { return static_cast<T>(log(static_cast<double>(x))); }
	}

	template <typename T>
	[[nodiscard]] inline T log2_call(T x) noexcept
	{
		if constexpr (std::is_same_v<T, float>) { return log2f(x); }
		else { return static_cast<T>(log2(static_cast<double>(x))); }
	}

	template <typename T>
	[[nodiscard]] inline T log10_call(T x) noexcept
	{
		if constexpr (std::is_same_v<T, float>) { return log10f(x); }
		else { return static_cast<T>(log10(static_cast<double>(x))); }
	}

	template <typename T>
	[[nodiscard]] inline T sin_call(T x) noexcept
	{
		if constexpr (std::is_same_v<T, float>) { return sinf(x); }
		else { return static_cast<T>(sin(static_cast<double>(x))); }
	}

	template <typename T>
	[[nodiscard]] inline T cos_call(T x) noexcept
	{
		if constexpr (std::is_same_v<T, float>) { return cosf(x); }
		else { return static_cast<T>(cos(static_cast<double>(x))); }
	}

	template <typename T>
	[[nodiscard]] inline T tan_call(T x) noexcept
	{
		if constexpr (std::is_same_v<T, float>) { return tanf(x); }
		else { return static_cast<T>(tan(static_cast<double>(x))); }
	}

	template <typename T>
	[[nodiscard]] inline T fmod_call(T x, T y) noexcept
	{
		if constexpr (std::is_same_v<T, float>) { return fmodf(x, y); }
		else { return static_cast<T>(fmod(static_cast<double>(x), static_cast<double>(y))); }
	}

	template <typename T>
	[[nodiscard]] inline T remainder_call(T x, T y) noexcept
	{
		if constexpr (std::is_same_v<T, float>) { return remainderf(x, y); }
		else { return static_cast<T>(remainder(static_cast<double>(x), static_cast<double>(y))); }
	}

	template <typename T>
	[[nodiscard]] inline T gamma_call(T x) noexcept
	{
		if constexpr (std::is_same_v<T, float>) { return tgammaf(x); }
		else { return static_cast<T>(tgamma(static_cast<double>(x))); }
	}

	template <typename T>
	[[nodiscard]] inline T lgamma_call(T x) noexcept
	{
		if constexpr (std::is_same_v<T, float>) { return lgammaf(x); }
		else { return static_cast<T>(lgamma(static_cast<double>(x))); }
	}
#endif
} // namespace ccm::rt::detail::msvc_libm
