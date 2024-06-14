/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#ifndef CCM_GPU_HOST_DEVICE
	#if defined(__CUDACC__) || defined(__HIPCC__)
		#define CCM_GPU_HOST_DEVICE __host__ __device__
	#else
		#define CCM_GPU_HOST_DEVICE
	#endif
#endif
