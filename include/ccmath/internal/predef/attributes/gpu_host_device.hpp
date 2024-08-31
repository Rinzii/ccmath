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

#ifndef CCM_GPU_HOST_DEVICE
	#if defined(__CUDACC__) || defined(__HIPCC__)
		#define CCM_GPU_HOST_DEVICE __host__ __device__
	#else
		#define CCM_GPU_HOST_DEVICE
	#endif
#endif
