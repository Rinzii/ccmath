/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

// Ref:
// https://clang.llvm.org/docs/AttributeReference.html#optnone

#ifdef CCM_ATTR_OPTNONE
	#undef CCM_ATTR_OPTNONE
#endif // CCM_ATTR_OPTNONE

#if defined(__has_attribute)
	#if __has_attribute(optnone)
		#define CCM_ATTR_OPTNONE __attribute__((optnone))
	#else
		#define CCM_ATTR_OPTNONE
	#endif
#else
	#define CCM_ATTR_OPTNONE
#endif

