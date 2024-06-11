/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/predef/versioning/version_encode.hpp"

#if defined(CCM_GCC_VERSION)
	#undef CCM_GCC_VERSION
#endif
#if defined(__GNUC__) && defined(__GNUC_PATCHLEVEL__)
	#define CCM_GCC_VERSION CCM_VERSION_ENCODE(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#elif defined(__GNUC__)
	#define CCM_GCC_VERSION CCM_VERSION_ENCODE(__GNUC__, __GNUC_MINOR__, 0)
#endif

#if defined(CCM_GCC_VERSION_CHECK)
	#undef CCM_GCC_VERSION_CHECK
#endif
#if defined(CCM_GCC_VERSION)
	#define CCM_GCC_VERSION_CHECK(major, minor, patch) (CCM_GCC_VERSION >= CCM_VERSION_ENCODE(major, minor, patch))
#else
	#define CCM_GCC_VERSION_CHECK(major, minor, patch) (0)
#endif