/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
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
