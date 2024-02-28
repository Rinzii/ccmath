/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present cmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
*/

#pragma once


/*
CCMATH REQUIREMENTS:
 * The entire library mandates everything MUST be constexpr. If it is not constexpr, it is not part of the library.
 * We do not allow exceptions.
 * We do allow static_asserts.
 * We must mimic the api of the standard cmath library.
 * Anything that could be considered UB is strictly forbidden.
 * Performance is are primary concern. Everything must be as fast as possible.
 * Anything implemented by cmath that is already constexpr is allowed to be wrapped by ccmath and not implemented by ccmath.

 */

/// Basic math functions

#include "ccmath/detail/basic/abs.hpp"
#include "ccmath/detail/basic/fdim.hpp"
#include "ccmath/detail/basic/fma.hpp"
#include "ccmath/detail/basic/fmod.hpp"
#include "ccmath/detail/basic/max.hpp"
#include "ccmath/detail/basic/min.hpp"
#include "ccmath/detail/basic/remainder.hpp"
#include "ccmath/detail/basic/remquo.hpp"


/// Comparison functions

#include "ccmath/detail/compare/fpclassify.hpp"
#include "ccmath/detail/compare/isfinite.hpp"
#include "ccmath/detail/compare/isgreater.hpp"
#include "ccmath/detail/compare/isgreaterequal.hpp"
#include "ccmath/detail/compare/isinf.hpp"
#include "ccmath/detail/compare/isless.hpp"
#include "ccmath/detail/compare/islessequal.hpp"
#include "ccmath/detail/compare/islessgreater.hpp"
#include "ccmath/detail/compare/isnan.hpp"
#include "ccmath/detail/compare/isnormal.hpp"
#include "ccmath/detail/compare/isunordered.hpp"
#include "ccmath/detail/compare/signbit.hpp"


/// Exponential functions

#include "ccmath/detail/exponential/exp.hpp"
#include "ccmath/detail/exponential/exp2.hpp"
#include "ccmath/detail/exponential/expm1.hpp"
#include "ccmath/detail/exponential/log.hpp"
#include "ccmath/detail/exponential/log1p.hpp"
#include "ccmath/detail/exponential/log2.hpp"
#include "ccmath/detail/exponential/log10.hpp"


/// Float manipulation functions

#include "ccmath/detail/fmanip/copysign.hpp"
#include "ccmath/detail/fmanip/frexp.hpp"
#include "ccmath/detail/fmanip/ilogb.hpp"
#include "ccmath/detail/fmanip/ldexp.hpp"
#include "ccmath/detail/fmanip/logb.hpp"
#include "ccmath/detail/fmanip/modf.hpp"
#include "ccmath/detail/fmanip/nextafter.hpp"
#include "ccmath/detail/fmanip/scalbn.hpp"


/// Hyperbolic functions

#include "ccmath/detail/hyperbolic/acosh.hpp"
#include "ccmath/detail/hyperbolic/asinh.hpp"
#include "ccmath/detail/hyperbolic/atanh.hpp"
#include "ccmath/detail/hyperbolic/cosh.hpp"
#include "ccmath/detail/hyperbolic/sinh.hpp"
#include "ccmath/detail/hyperbolic/tanh.hpp"


/// Nearest functions

#include "ccmath/detail/nearest/ceil.hpp"
#include "ccmath/detail/nearest/floor.hpp"
#include "ccmath/detail/nearest/nearbyint.hpp"
#include "ccmath/detail/nearest/rint.hpp"
#include "ccmath/detail/nearest/round.hpp"
#include "ccmath/detail/nearest/trunc.hpp"


/// Power functions

#include "ccmath/detail/power/cbrt.hpp"
#include "ccmath/detail/power/hypot.hpp"
#include "ccmath/detail/power/pow.hpp"
#include "ccmath/detail/power/sqrt.hpp"


/// Special functions

#include "ccmath/detail/special/assoc_laguerre.hpp"
#include "ccmath/detail/special/assoc_legendre.hpp"
#include "ccmath/detail/special/beta.hpp"
#include "ccmath/detail/special/comp_ellint_1.hpp"
#include "ccmath/detail/special/comp_ellint_2.hpp"
#include "ccmath/detail/special/comp_ellint_3.hpp"
#include "ccmath/detail/special/cyl_bessel_i.hpp"
#include "ccmath/detail/special/cyl_bessel_j.hpp"
#include "ccmath/detail/special/cyl_bessel_k.hpp"
#include "ccmath/detail/special/cyl_neumann.hpp"
#include "ccmath/detail/special/ellint_1.hpp"
#include "ccmath/detail/special/ellint_2.hpp"
#include "ccmath/detail/special/ellint_3.hpp"
#include "ccmath/detail/special/expint.hpp"
#include "ccmath/detail/special/hermite.hpp"
#include "ccmath/detail/special/laguerre.hpp"
#include "ccmath/detail/special/legendre.hpp"
#include "ccmath/detail/special/riemann_zeta.hpp"
#include "ccmath/detail/special/sph_bessel.hpp"
#include "ccmath/detail/special/sph_legendre.hpp"
#include "ccmath/detail/special/sph_neumann.hpp"


/// Trigonometric functions
#include "ccmath/detail/trig/acos.hpp"
#include "ccmath/detail/trig/asin.hpp"
#include "ccmath/detail/trig/atan.hpp"
#include "ccmath/detail/trig/atan2.hpp"
#include "ccmath/detail/trig/cos.hpp"
#include "ccmath/detail/trig/sin.hpp"
#include "ccmath/detail/trig/tan.hpp"


/// Uncategorized functions

#include "ccmath/detail/gamma.hpp"
#include "ccmath/detail/lerp.hpp"
#include "ccmath/detail/lgamma.hpp"
