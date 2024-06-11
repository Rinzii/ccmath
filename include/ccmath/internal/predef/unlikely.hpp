/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/predef/expects_bool_condition.hpp"

#ifndef CCM_UNLIKELY
	#define CCM_UNLIKELY(x) ccm::predef::internal::expects_bool_condition((x), false)
#endif // CCM_UNLIKELY
