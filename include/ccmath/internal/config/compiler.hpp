/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

/**
 * Currently supported compilers:
 * - MSVC
 * - Clang-CL
 * - Intel DPC++
 * - Nvidia HPC SDK
 * - Nvidia CUDA
 * - AMD HIP
 * - Apple Clang
 * - Clang
 * - GCC
 */

#pragma once

/// MSVC
#if defined(_MSC_VER) && !defined(__clang__) && !defined(CCMATH_COMPILER_MSVC)
	#define CCMATH_COMPILER_MSVC
	#define CCMATH_COMPILER_MSVC_VER _MSC_VER


#elif defined(_MSC_VER) && defined(__clang__) && !defined(CCMATH_COMPILER_CLANG_CL)
	#define CCMATH_COMPILER_CLANG_CL
	#define CCMATH_COMPILER_CLANG_CL_VER	   ((__clang_major__ * 10000) + (__clang_minor__ * 100) + __clang_patchlevel__)
	#define CCMATH_COMPILER_CLANG_CL_VER_MAJOR __clang_major__
	#define CCMATH_COMPILER_CLANG_CL_VER_MINOR __clang_minor__
	#define CCMATH_COMPILER_CLANG_CL_VER_PATCH __clang_patchlevel__

	#ifndef CCMATH_COMPILER_CLANG_BASED
		#define CCMATH_COMPILER_CLANG_BASED
    #endif

// TODO: Add precise detection for specific compiler versions along with a warning if using unsupported compiler

/// Intel DPC++ Compiler
#elif defined(SYCL_LANGUAGE_VERSION) || defined(__INTEL_LLVM_COMPILER) && !defined(CCMATH_COMPILER_INTEL)
	#define CCMATH_COMPILER_INTEL
	#define CCMATH_COMPILER_INTEL_VER __INTEL_LLVM_COMPILER

	#ifndef CCMATH_COMPILER_CLANG_BASED
		#define CCMATH_COMPILER_CLANG_BASED
    #endif

// TODO: Add precise detection for specific compiler versions along with a warning if using unsupported compiler


/// Nvidia HPC SDK
#elif defined(__NVCOMPILER) || defined(__NVCOMPILER_LLVM__) && !defined(CCMATH_COMPILER_NVIDIA_HPC)
	#define CCMATH_COMPILER_NVIDIA_HPC
	#define CCMATH_COMPILER_NVIDIA_HPC_VER		 (__NVCOMPILER_MAJOR__ * 10000 + __NVCOMPILER_MINOR__ * 100 + __NVCOMPILER_PATCHLEVEL__)
	#define CCMATH_COMPILER_NVIDIA_HPC_VER_MAJOR __NVCOMPILER_MAJOR__
	#define CCMATH_COMPILER_NVIDIA_HPC_VER_MINOR __NVCOMPILER_MINOR__
	#define CCMATH_COMPILER_NVIDIA_HPC_VER_PATCH __NVCOMPILER_PATCHLEVEL__

// TODO: Add precise detection for specific compiler versions along with a warning if using unsupported compiler


/// Nvidia CUDA
#elif defined(__CUDACC__) && !defined(CCMATH_COMPILER_NVIDIA_CUDA)
	#if !defined(CUDA_VERSION)
		#include <cuda.h> // We need to make sure the version is defined since nvcc doesn't define it
	#endif

	#define CCMATH_COMPILER_NVIDIA_CUDA
	#define CCMATH_COMPILER_NVIDIA_CUDA_VER (CUDA_VERSION / 1000)

// TODO: Add precise detection for specific compiler versions along with a warning if using unsupported compiler


/// AMD HIP
#elif defined(__HIP__) && !defined(CCMATH_COMPILER_AMD_HIP)
    #define CCMATH_COMPILER_AMD_HIP

// TODO: Add precise detection for specific compiler versions along with a warning if using unsupported compiler


/// Apple Clang
#elif defined(__apple_build_version__) && defined(__clang__) && !defined(CCMATH_COMPILER_APPLE_CLANG)
	#define CCMATH_COMPILER_APPLE_CLANG
	#define CCMATH_COMPILER_APPLE_CLANG_VER		  ((__clang_major__ * 10000) + (__clang_minor__ * 100) + __clang_patchlevel__)
	#define CCMATH_COMPILER_APPLE_CLANG_VER_MAJOR __clang_major__
	#define CCMATH_COMPILER_APPLE_CLANG_VER_MINOR __clang_minor__
	#define CCMATH_COMPILER_APPLE_CLANG_VER_PATCH __clang_patchlevel__

	#ifndef CCMATH_COMPILER_CLANG_BASED
		#define CCMATH_COMPILER_CLANG_BASED
	#endif

// TODO: Add precise detection for specific compiler versions along with a warning if using unsupported compiler

/// Clang
#elif defined(__clang__) && !defined(CCMATH_COMPILER_CLANG)
	#define CCMATH_COMPILER_CLANG
	#define CCMATH_COMPILER_CLANG_VER		((__clang_major__ * 100) + (__clang_minor__ * 10) + __clang_patchlevel__)
	#define CCMATH_COMPILER_CLANG_VER_MAJOR __clang_major__
	#define CCMATH_COMPILER_CLANG_VER_MINOR __clang_minor__
	#define CCMATH_COMPILER_CLANG_VER_PATCH __clang_patchlevel__

	#ifndef CCMATH_COMPILER_CLANG_BASED
		#define CCMATH_COMPILER_CLANG_BASED
	#endif

// TODO: Add precise detection for specific compiler versions along with a warning if using unsupported compiler

/// GCC
#elif defined(__GNUC__) && !defined(CCMATH_COMPILER_GCC)
	#define CCMATH_COMPILER_GCC
	#define CCMATH_COMPILER_GCC_VER		  ((__GNUC__ * 10000) + (__GNUC_MINOR__ * 100) + __GNUC_PATCHLEVEL__)
	#define CCMATH_COMPILER_GCC_VER_MAJOR __GNUC__
	#define CCMATH_COMPILER_GCC_VER_MINOR __GNUC_MINOR__
	#define CCMATH_COMPILER_GCC_VER_PATCH __GNUC_PATCHLEVEL__


#else
	#define CCMATH_COMPILER_UNKNOWN
#endif
