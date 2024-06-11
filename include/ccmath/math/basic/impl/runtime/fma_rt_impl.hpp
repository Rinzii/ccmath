/*
* Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <emmintrin.h>  // SSE2

namespace ccm::internal::rt::impl
{
	inline double fma_rt_double(double a, double b, double c) noexcept
	{
		// TODO: Apply generic implementation for non-SSE2 platforms
		// Load a, b, and c into SSE2 registers
		const __m128d va = _mm_set_sd(a); // Set the low double element and zero the upper element
		const __m128d vb = _mm_set_sd(b); // Set the low double element and zero the upper element
		const __m128d vc = _mm_set_sd(c); // Set the low double element and zero the upper element

		// Perform the multiplication and addition
		const __m128d vmul = _mm_mul_sd(va, vb); // Multiply the low elements of va and vb
		const __m128d vresult = _mm_add_sd(vmul, vc); // Add the low element of vc to the result of the multiplication

		// Extract the result
		const double result = _mm_cvtsd_f64(vresult); // Convert the low double element to a double

		return result;
	}
} // namespace ccm::internal::rt::impl
