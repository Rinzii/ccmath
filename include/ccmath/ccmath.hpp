/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present cmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
*/

#pragma once

#include "ccmath/basic.hpp"


/*
CCMATH REQUIREMENTS:
 * The entire library mandates everything MUST be constexpr. If it is not constexpr, it is not part of the library.
 * We do not allow exceptions.
 * We do allow static_asserts.
 * We must mimic the api of the standard cmath library.
 * Anything that could be considered UB is strictly forbidden.
 * Performance is are primary concern. Everything must be as fast as possible.
 * Anything implemented by cmath that is already constexpr is allowed to be wrapped by ccmath and not implemented by ccmath.


TODO: These are the functions that are currently located in cmath and are to be implemented
TODO: We don't have ti implement the entire API of cmath exactly how it is implemented in cmath. We allow for some flexibility in the API.

 TYPES:
    float_t
    double_t

 MACROS:
    HUGE_VALF - indicates the overflow value for float, double and long double respectively
    HUGE_VAL
    HUGE_VALL
    INFINITY - evaluates to positive infinity or the value guaranteed to overflow a float
    NAN - evaluates to a quiet NaN of type float
    math_errhandling - defines the error handling mechanism used by the common mathematical functions
    MATH_ERRNO
    MATH_ERREXCEPT

    Classification macros:
    FP_NORMAL - indicates a floating-point category
    FP_SUBNORMAL
    FP_ZERO
    FP_INFINITE
    FP_NAN

 FUNCTIONS:
    abs - absolute value of a floating point value (|x|)
    fabs
    fabsf
    fabsl

    fmod - remainder of the floating point division operation
    fmodf
    fmodl

    remainder - signed remainder of the division operation
    remainderf
    remainderl

    remquo - signed remainder as well as the three last bits of the division operation
    remquof
    remquol

    fma - fused multiply-add operation
    fmaf
    fmal

    fmax - larger of two floating-point values
    fmaxf
    fmaxl

    fmin - smaller of two floating-point values
    fminf
    fminl

    fdim -positive difference of two floating point values (max(0,x−y))
    fdimf
    fdiml

    nan - not-a-number (NaN)
	nanf
    nanl

    lerp - linear interpolation function

    exp - returns e raised to the given power (e^x)
    expf
    expl

    exp2 - returns 2 raised to the given power (2^x)
    exp2f
    exp2l

    expm1 - returns e raised to the given power minus one (e^x - 1)
    expm1f
    expm1l

    log - computes natural (base e) logarithm (ln x)
    logf
    logl

    log10 - computes common (base 10) logarithm (log10 x)
    log10f
    log10l

    log2 - computes binary (base 2) logarithm (log2 x)
    log2f
    log2l







 */
