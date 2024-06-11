/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present ccmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
*/

#pragma once

#include "ccmath/internal/predef/expects_bool_condition.hpp"

#ifndef CCM_LIKELY
	#define CCM_LIKELY(x) ccm::predef::internal::expects_bool_condition((x), true)
#endif // CCM_LIKELY
