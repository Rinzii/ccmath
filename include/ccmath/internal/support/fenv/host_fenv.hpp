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

// Single point of truth for whether ccmath can use the host floating-point environment.
//
// Most hosts (GCC, Clang, MSVC, Intel and the like) ship the C++ <cfenv> header and
// CCM_CONFIG_HAS_FENV is defined. Some freestanding or console toolchains (the PS5 / Prospero SDK,
// for one) ship no <cfenv>. When that happens ccmath falls back to the C <fenv.h> header if it is
// present, which keeps the runtime rounding-mode reads and exception signaling working. Only when
// neither header exists does ccmath assume round-to-nearest at runtime with no
// floating-point-exception signaling.
//
// Define CCM_CONFIG_DISABLE_FENV to force that last fallback on a host that does have the headers.
// It completely disables the host floating-point environment and drops every <cfenv> / <fenv.h>
// include from the library. The directed rounding modes then degrade to round-to-nearest at runtime,
// which is non-standard, the same tradeoff CCM_CONFIG_DISABLE_ERRNO makes for errno. Define
// CCM_CONFIG_FORCE_FENV_H to exercise the <fenv.h> tier on a host that also has <cfenv> (testing
// only).
//
// Freestanding builds are supported and resolved in config/freestanding.hpp. The host floating-point
// environment stays optional here, so a freestanding target needs no extra handling: this header
// uses <cfenv> or <fenv.h> when the toolchain ships them and degrades to round-to-nearest when it
// does not. errno is not a freestanding obstacle either, since <cerrno> is a freestanding header in
// C++26 (Table 27 in [compliance]) and only supplies the EDOM and ERANGE macros plus the errno
// lvalue. The actual errno write stays behind CCM_CONFIG_DISABLE_ERRNO in fenv_support.hpp.
//
// The detection rules out a missing __has_include first so that __has_include(...) is only used
// once the compiler is known to provide it, which keeps every supported compiler (including MSVC)
// happy.

#if defined(CCM_CONFIG_DISABLE_FENV)
// Forced off: leave CCM_CONFIG_HAS_FENV undefined so the no-fenv tier below is selected.
#elif !defined(__has_include)
	// A pre-C++17 compiler with no __has_include. Assume the standard <cfenv> header is present.
	#define CCM_CONFIG_HAS_FENV		  1
	#define CCM_CONFIG_FENV_USE_CFENV 1
#elif defined(CCM_CONFIG_FORCE_FENV_H)
	#if __has_include(<fenv.h>)
		#define CCM_CONFIG_HAS_FENV		   1
		#define CCM_CONFIG_FENV_USE_FENV_H 1
	#endif
#elif __has_include(<cfenv>)
	#define CCM_CONFIG_HAS_FENV		  1
	#define CCM_CONFIG_FENV_USE_CFENV 1
#elif __has_include(<fenv.h>)
	#define CCM_CONFIG_HAS_FENV		   1
	#define CCM_CONFIG_FENV_USE_FENV_H 1
#endif

#if defined(CCM_CONFIG_FENV_USE_CFENV)
	#include <cfenv>
#elif defined(CCM_CONFIG_FENV_USE_FENV_H)
	#include <fenv.h>
#else
	// No host floating-point environment. Provide fallback FE_* constants so the call sites still
	// compile. The values are not handed to a host fenv call in this mode, but they have to stay
	// distinct so that switches on the rounding mode keep working. The values match the common x86
	// glibc layout for the sake of familiarity.
	#ifndef FE_TONEAREST
		#define FE_TONEAREST 0x0000
	#endif
	#ifndef FE_DOWNWARD
		#define FE_DOWNWARD 0x0400
	#endif
	#ifndef FE_UPWARD
		#define FE_UPWARD 0x0800
	#endif
	#ifndef FE_TOWARDZERO
		#define FE_TOWARDZERO 0x0C00
	#endif
	#ifndef FE_INVALID
		#define FE_INVALID 0x01
	#endif
	#ifndef FE_DIVBYZERO
		#define FE_DIVBYZERO 0x04
	#endif
	#ifndef FE_OVERFLOW
		#define FE_OVERFLOW 0x08
	#endif
	#ifndef FE_UNDERFLOW
		#define FE_UNDERFLOW 0x10
	#endif
	#ifndef FE_INEXACT
		#define FE_INEXACT 0x20
	#endif
	#ifndef FE_ALL_EXCEPT
		#define FE_ALL_EXCEPT (FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW | FE_INEXACT)
	#endif
#endif

// Thin wrappers over the host floating-point environment. This is the only place in ccmath that
// names the host fenv functions, so every consumer reaches them through here regardless of which
// tier resolved above.
namespace ccm::support::fenv::host
{
#if defined(CCM_CONFIG_HAS_FENV)

	#if defined(CCM_CONFIG_FENV_USE_CFENV)
		#define CCM_FENV_HOST_QUAL std
	#else
		#define CCM_FENV_HOST_QUAL
	#endif

	using fenv_t = CCM_FENV_HOST_QUAL ::fenv_t;

	inline int get_round()
	{ return CCM_FENV_HOST_QUAL ::fegetround(); }

	inline int set_round(const int rounding_mode)
	{ return CCM_FENV_HOST_QUAL ::fesetround(rounding_mode); }

	inline int raise_except(const int err_code)
	{ return CCM_FENV_HOST_QUAL ::feraiseexcept(err_code); }

	inline int clear_except(const int err_code)
	{ return CCM_FENV_HOST_QUAL ::feclearexcept(err_code); }

	inline int test_except(const int err_code)
	{ return CCM_FENV_HOST_QUAL ::fetestexcept(err_code); }

	inline int get_env(fenv_t * envp)
	{ return CCM_FENV_HOST_QUAL ::fegetenv(envp); }

	inline int set_env(const fenv_t * envp)
	{ return CCM_FENV_HOST_QUAL ::fesetenv(envp); }

	#undef CCM_FENV_HOST_QUAL

	// fesetexcept, fegetexcept, feenableexcept and fedisableexcept are GNU extensions that live in
	// the global namespace, so they are always called unqualified and guarded on __USE_GNU. Other
	// toolchains (MSVC included) return zero from these instead.
	inline int set_except([[maybe_unused]] int err_code)
	{
	#ifdef __USE_GNU
		return ::fesetexcept(err_code);
	#else
		return 0;
	#endif
	}

	inline int get_except()
	{
	#ifdef __USE_GNU
		return ::fegetexcept();
	#else
		return 0;
	#endif
	}

	inline int enable_except([[maybe_unused]] int err_code)
	{
	#ifdef __USE_GNU
		return ::feenableexcept(err_code);
	#else
		return 0;
	#endif
	}

	inline int disable_except([[maybe_unused]] int err_code)
	{
	#ifdef __USE_GNU
		return ::fedisableexcept(err_code);
	#else
		return 0;
	#endif
	}

#else // No host floating-point environment. Report round-to-nearest and make every write a no-op.

	using fenv_t = int;

	inline int get_round()
	{ return FE_TONEAREST; }

	inline int set_round([[maybe_unused]] int rounding_mode)
	{ return 0; }

	inline int raise_except([[maybe_unused]] int err_code)
	{ return 0; }

	inline int clear_except([[maybe_unused]] int err_code)
	{ return 0; }

	inline int test_except([[maybe_unused]] int err_code)
	{ return 0; }

	inline int get_env([[maybe_unused]] fenv_t * envp)
	{ return 0; }

	inline int set_env([[maybe_unused]] const fenv_t * envp)
	{ return 0; }

	inline int set_except([[maybe_unused]] int err_code)
	{ return 0; }

	inline int get_except()
	{ return 0; }

	inline int enable_except([[maybe_unused]] int err_code)
	{ return 0; }

	inline int disable_except([[maybe_unused]] int err_code)
	{ return 0; }

#endif
} // namespace ccm::support::fenv::host
