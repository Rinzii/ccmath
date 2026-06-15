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

// Selects what the differential targets actually fuzz. The default exercises the public
// entry points, which on GCC and Clang dispatch most transcendentals to the platform libm
// at runtime, so the comparison degenerates to libm against itself. The CCMATH_FUZZ_GENERIC
// build routes these calls to the generic kernels instead, which is where ccmath's own
// numeric code gets fuzzed against the libm oracle.

#include <ccmath/ccmath.hpp>

#include <limits>

#ifdef CCMATH_FUZZ_GENERIC
	#include "ccmath/internal/math/generic/func/expo/exp2_gen.hpp"
	#include "ccmath/internal/math/generic/func/expo/exp_gen.hpp"
	#include "ccmath/internal/math/generic/func/expo/expm1_gen.hpp"
	#include "ccmath/internal/math/generic/func/expo/log10_gen.hpp"
	#include "ccmath/internal/math/generic/func/expo/log1p_gen.hpp"
	#include "ccmath/internal/math/generic/func/expo/log2_gen.hpp"
	#include "ccmath/internal/math/generic/func/expo/log_gen.hpp"
	#include "ccmath/internal/math/generic/func/power/cbrt_gen.hpp"
	#include "ccmath/internal/math/generic/func/power/hypot_gen.hpp"
	#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
	#include "ccmath/internal/math/generic/func/power/sqrt_gen.hpp"
	#include "ccmath/internal/math/generic/func/trig/cos_gen.hpp"
	#include "ccmath/internal/math/generic/func/trig/sin_gen.hpp"
	#include "ccmath/internal/math/generic/func/trig/tan_gen.hpp"
#endif

namespace ccm::fuzz::calls
{
#ifdef CCMATH_FUZZ_GENERIC
	// The public headers own special-case handling for most functions and only hand the
	// generic kernels pre-filtered inputs (pow is the exception: its kernel is complete).
	// The kernel mappings reproduce that contract: special operands route through the
	// public entry point and ordinary operands hit the kernel, so the kernel is fuzzed
	// strictly inside its own domain.
	template <typename T>
	bool kernel_domain_finite(T x)
	{ return x == x && x != std::numeric_limits<T>::infinity() && x != -std::numeric_limits<T>::infinity(); }

	#define CCMATH_FUZZ_UNARY_CALL(name)                                                                                                                       \
		template <typename T>                                                                                                                                  \
		T name(T x)                                                                                                                                            \
		{                                                                                                                                                      \
			if (!kernel_domain_finite(x)) { return ccm::name(x); }                                                                                             \
			return ccm::gen::name##_gen(x);                                                                                                                    \
		}
	#define CCMATH_FUZZ_UNARY_CALL_POSITIVE(name)                                                                                                              \
		template <typename T>                                                                                                                                  \
		T name(T x)                                                                                                                                            \
		{                                                                                                                                                      \
			if (!kernel_domain_finite(x) || !(x > T(0))) { return ccm::name(x); }                                                                              \
			return ccm::gen::name##_gen(x);                                                                                                                    \
		}
	#define CCMATH_FUZZ_UNARY_CALL_ABOVE_NEG_ONE(name)                                                                                                         \
		template <typename T>                                                                                                                                  \
		T name(T x)                                                                                                                                            \
		{                                                                                                                                                      \
			if (!kernel_domain_finite(x) || !(x > T(-1))) { return ccm::name(x); }                                                                             \
			return ccm::gen::name##_gen(x);                                                                                                                    \
		}
	#define CCMATH_FUZZ_BINARY_CALL(name)                                                                                                                      \
		template <typename T>                                                                                                                                  \
		T name(T x, T y)                                                                                                                                       \
		{                                                                                                                                                      \
			if (!kernel_domain_finite(x) || !kernel_domain_finite(y)) { return ccm::name(x, y); }                                                              \
			return ccm::gen::name##_gen(x, y);                                                                                                                 \
		}
	// The pow kernel owns its full special-case matrix, so it is fuzzed unfiltered.
	#define CCMATH_FUZZ_BINARY_CALL_COMPLETE(name)                                                                                                             \
		template <typename T>                                                                                                                                  \
		T name(T x, T y)                                                                                                                                       \
		{ return ccm::gen::name##_gen(x, y); }
#else
	#define CCMATH_FUZZ_UNARY_CALL(name)                                                                                                                       \
		template <typename T>                                                                                                                                  \
		T name(T x)                                                                                                                                            \
		{ return ccm::name(x); }
	#define CCMATH_FUZZ_UNARY_CALL_POSITIVE(name)	   CCMATH_FUZZ_UNARY_CALL(name)
	#define CCMATH_FUZZ_UNARY_CALL_ABOVE_NEG_ONE(name) CCMATH_FUZZ_UNARY_CALL(name)
	#define CCMATH_FUZZ_BINARY_CALL(name)                                                                                                                      \
		template <typename T>                                                                                                                                  \
		T name(T x, T y)                                                                                                                                       \
		{ return ccm::name(x, y); }
	#define CCMATH_FUZZ_BINARY_CALL_COMPLETE(name) CCMATH_FUZZ_BINARY_CALL(name)
#endif

	CCMATH_FUZZ_UNARY_CALL_POSITIVE(sqrt)
	CCMATH_FUZZ_UNARY_CALL(cbrt)
	CCMATH_FUZZ_BINARY_CALL_COMPLETE(pow)
	CCMATH_FUZZ_BINARY_CALL(hypot)

	CCMATH_FUZZ_UNARY_CALL(exp)
	CCMATH_FUZZ_UNARY_CALL(exp2)
	CCMATH_FUZZ_UNARY_CALL(expm1)
	CCMATH_FUZZ_UNARY_CALL_POSITIVE(log)
	CCMATH_FUZZ_UNARY_CALL_ABOVE_NEG_ONE(log1p)
	CCMATH_FUZZ_UNARY_CALL_POSITIVE(log2)
	CCMATH_FUZZ_UNARY_CALL_POSITIVE(log10)

	CCMATH_FUZZ_UNARY_CALL(sin)
	CCMATH_FUZZ_UNARY_CALL(cos)
	CCMATH_FUZZ_UNARY_CALL(tan)

	// Inverse trig has no generic kernels yet, so the public dispatch is fuzzed in both
	// build flavors and these calls run under round to nearest only.
	template <typename T>
	T asin(T x)
	{ return ccm::asin(x); }
	template <typename T>
	T acos(T x)
	{ return ccm::acos(x); }
	template <typename T>
	T atan(T x)
	{ return ccm::atan(x); }
	template <typename T>
	T atan2(T x, T y)
	{ return ccm::atan2(x, y); }

#undef CCMATH_FUZZ_UNARY_CALL
#undef CCMATH_FUZZ_UNARY_CALL_POSITIVE
#undef CCMATH_FUZZ_UNARY_CALL_ABOVE_NEG_ONE
#undef CCMATH_FUZZ_BINARY_CALL
#undef CCMATH_FUZZ_BINARY_CALL_COMPLETE
} // namespace ccm::fuzz::calls
