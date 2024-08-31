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

#include "ccmath/internal/predef/has_attribute.hpp"

#ifndef CCM_NODEBUG
	#if defined(__GNUC__) || defined(__clang__)
		#if CCM_HAS_ATTR(__nodebug__)
			#define CCM_NODEBUG __attribute__((__nodebug__))
		#else
			#define CCM_NODEBUG
		#endif
	#else
		#define CCM_NODEBUG
	#endif
#endif
