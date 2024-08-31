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

#include "ccmath/internal/predef/expects_bool_condition.hpp"

#ifndef CCM_LIKELY
	#define CCM_LIKELY(x) ccm::predef::internal::expects_bool_condition((x), true)
#endif // CCM_LIKELY
