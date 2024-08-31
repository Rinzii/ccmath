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

#if defined(CCM_VERSION_ENCODE)
	#undef CCM_VERSION_ENCODE
#endif
#define CCM_VERSION_ENCODE(major, minor, revision) (((major) * 1000000) + ((minor) * 1000) + (revision))
