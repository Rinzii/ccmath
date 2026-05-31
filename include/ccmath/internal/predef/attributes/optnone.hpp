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
