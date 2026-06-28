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

// Umbrella for the C++17 port of the C++26 std::simd surface (ccm::pp). This is
// a standalone library. It does not replace the live ccm::intrin SIMD system
// until the project decides to switch over.

#include "ccmath/internal/math/runtime/pp/conversion.hpp"
#include "ccmath/internal/math/runtime/pp/declaration.hpp"
#include "ccmath/internal/math/runtime/pp/flags.hpp"
#include "ccmath/internal/math/runtime/pp/mask_reductions.hpp"
#include "ccmath/internal/math/runtime/pp/msvc_intrin.hpp"
#include "ccmath/internal/math/runtime/pp/reduce.hpp"
#include "ccmath/internal/math/runtime/pp/reference.hpp"
#include "ccmath/internal/math/runtime/pp/scalar.hpp"
#include "ccmath/internal/math/runtime/pp/simd.hpp"
#include "ccmath/internal/math/runtime/pp/simd_cat.hpp"
#include "ccmath/internal/math/runtime/pp/simd_mask.hpp"
#include "ccmath/internal/math/runtime/pp/simd_math.hpp"
#include "ccmath/internal/math/runtime/pp/traits.hpp"
#include "ccmath/internal/math/runtime/pp/utility.hpp"
#include "ccmath/internal/math/runtime/pp/vec_ext.hpp"
#include "ccmath/internal/math/runtime/pp/where.hpp"

namespace ccm::pp
{
	// [simd.alias] Width-parametrized aliases over the deduced ABI.
	template <typename T, detail::SimdSizeType N = basic_simd<T>::size()> using simd = basic_simd<T, detail::deduce_t<T, N>>;

	template <typename T, detail::SimdSizeType N = basic_simd<T>::size()> using simd_mask = basic_simd_mask<sizeof(T), detail::deduce_t<T, N>>;

	// Widest native register width for T.
	template <typename T> using native_simd = basic_simd<T, detail::NativeAbi<T>>;

	template <typename T> using native_simd_mask = basic_simd_mask<sizeof(T), detail::NativeAbi<T>>;
} // namespace ccm::pp
